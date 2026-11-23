// LeetCode 886: Possible Bipartition
// https://leetcode.com/problems/possible-bipartition/
//
// Problem:
//   Given `n` people labeled 1 through `n` and an array `dislikes` where
//   `dislikes[i] = [a_i, b_i]` means person `a_i` and person `b_i` dislike
//   each other, return true if everyone can be split into two groups such that
//   no pair of people who dislike each other end up in the same group.
//   Constraints: 1 <= n <= 2000, 0 <= dislikes.size() <= 10^4, no self-loops
//   or duplicate pairs; the dislike graph may be disconnected.
//
// Algorithm:
//   Model the problem as 2-coloring an undirected graph (testing whether it is
//   bipartite, i.e., free of odd-length cycles). Two implementations are
//   provided:
//   1. `possibleBipartition`: Recursive DFS 2-coloring using colors {0, 1}
//      (-1 for unvisited). For each uncolored person, assign color 0 and
//      recursively require every neighbor to take the opposite color (`c ^ 1`).
//   2. `rewriteBfs`: Iterative BFS 2-coloring using colors {+1, -1} (0 for
//      unvisited). For each unvisited component, color the root +1 and push it
//      onto a queue; every neighbor must be colored with `-cur_color`.
//   Key invariant: every explored edge connects vertices of opposite colors; a
//   conflict proves an odd cycle and immediately returns false.
//   Time: O(V + E), Space: O(V + E), where V = n and E = dislikes.size().
//
// Data Structures:
//   - `std::vector<std::vector<int>> adj`: 1-indexed adjacency list for the
//     undirected dislike graph.
//   - `std::vector<std::int8_t> color`: per-vertex group assignment and
//     visited marker.
//   - `std::queue<int> q` (in `rewriteBfs`): FIFO frontier for level-order
//     BFS traversal.
//
// Real-World Applications:
//   - Two-way conflict partitioning in exam or meeting scheduling when pairs
//     of events cannot share a time slot.
//   - Dual-rail / two-layer PCB and VLSI routing where crossing nets must be
//     assigned to opposite layers.
//   - Register allocation and interference-graph 2-coloring in compiler
//     backends for two-bank register architectures.
//
// Related LeetCode Problems:
//   - #785 Is Graph Bipartite? (direct 2-coloring on a given adjacency list)
//   - #207 Course Schedule (graph traversal and cycle detection on dependencies)
//   - #261 Graph Valid Tree (undirected connectivity and acyclicity check)
//   - #1042 Flower Planting With No Adjacent (4-coloring bounded-degree graphs)
//   - #990 Satisfiability of Equality Equations (constraint-based partitioning)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <queue>
#include <vector>

namespace {

class Solution {
 public:
  // DFS 2-coloring over the undirected dislike graph.
  // Time: O(V + E), Space: O(V + E).
  bool possibleBipartition(int n,
                           const std::vector<std::vector<int>>& dislikes) {
    const auto size = static_cast<std::size_t>(n) + 1uz;
    std::vector<std::vector<int>> adj(size);
    for (const auto& edge : dislikes) {
      adj[static_cast<std::size_t>(edge[0])].push_back(edge[1]);
      adj[static_cast<std::size_t>(edge[1])].push_back(edge[0]);
    }

    // -1 = uncolored, 0 = group A, 1 = group B.
    std::vector<std::int8_t> color(size, -1);

    auto dfs = [&](this auto&& self, int person, std::int8_t expected) -> bool {
      const auto idx = static_cast<std::size_t>(person);
      if (color[idx] != -1) {
        return color[idx] == expected;
      }
      color[idx] = expected;
      const auto opposite = static_cast<std::int8_t>(expected ^ 1);
      for (const int other : adj[idx]) {
        if (!self(other, opposite)) {
          return false;
        }
      }
      return true;
    };

    for (int i = 1; i <= n; ++i) {
      if (color[static_cast<std::size_t>(i)] == -1 && !dfs(i, 0)) {
        return false;
      }
    }
    return true;
  }

  // BFS 2-coloring (mirrors rewrite_bfs() in 886_Possible_Bipartition.py).
  // Time: O(V + E), Space: O(V + E).
  bool rewriteBfs(int n, const std::vector<std::vector<int>>& dislikes) {
    const auto size = static_cast<std::size_t>(n) + 1uz;
    std::vector<std::vector<int>> adj(size);
    for (const auto& edge : dislikes) {
      adj[static_cast<std::size_t>(edge[0])].push_back(edge[1]);
      adj[static_cast<std::size_t>(edge[1])].push_back(edge[0]);
    }

    // 0 = uncolored, +1 / -1 = the two groups.
    std::vector<std::int8_t> color(size, 0);
    std::queue<int> q;

    for (int i = 1; i <= n; ++i) {
      if (color[static_cast<std::size_t>(i)] != 0) {
        continue;
      }
      color[static_cast<std::size_t>(i)] = 1;
      q.push(i);

      while (!q.empty()) {
        const int cur = q.front();
        q.pop();
        const std::int8_t cur_color = color[static_cast<std::size_t>(cur)];

        for (const int nbr : adj[static_cast<std::size_t>(cur)]) {
          const auto unbr = static_cast<std::size_t>(nbr);
          if (color[unbr] == cur_color) {
            return false;
          }
          if (color[unbr] == 0) {
            color[unbr] = static_cast<std::int8_t>(-cur_color);
            q.push(nbr);
          }
        }
      }
    }

    return true;
  }
};

LC_PROBLEM(886, "Possible Bipartition", "graph", ctx) {
  Solution sol;

  ctx.test("example 1: bipartite dislike graph", [&] {
    const std::vector<std::vector<int>> dislikes = {{1, 2}, {1, 3}, {2, 4}};
    ctx.expect_true(sol.possibleBipartition(4, dislikes));
    ctx.expect_true(sol.rewriteBfs(4, dislikes));
  });

  ctx.test("example 2: triangle cannot be 2-colored", [&] {
    const std::vector<std::vector<int>> dislikes = {{1, 2}, {1, 3}, {2, 3}};
    ctx.expect_false(sol.possibleBipartition(3, dislikes));
    ctx.expect_false(sol.rewriteBfs(3, dislikes));
  });

  ctx.test("example 3: 5-cycle cannot be 2-colored", [&] {
    const std::vector<std::vector<int>> dislikes = {
        {1, 2}, {2, 3}, {3, 4}, {4, 5}, {1, 5}};
    ctx.expect_false(sol.possibleBipartition(5, dislikes));
    ctx.expect_false(sol.rewriteBfs(5, dislikes));
  });

  ctx.test("disconnected components with odd cycle in second component", [&] {
    const std::vector<std::vector<int>> dislikes = {
        {1, 2}, {3, 4}, {4, 5}, {3, 5}};
    ctx.expect_false(sol.possibleBipartition(5, dislikes));
    ctx.expect_false(sol.rewriteBfs(5, dislikes));
  });
}

}  // namespace
