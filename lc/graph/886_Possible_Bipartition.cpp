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
// Examples:
//   Example 1:
//     Input:  n = 4, dislikes = [[1, 2], [1, 3], [2, 4]]
//     Output: true
//     Explanation:
//       - Build undirected adjacency list adj (1 -> [2, 3], 2 -> [1, 4], 3 -> [1],
//         4 -> [2]) with color initialized to -1.
//       - Start dfs(1, 0) to set color[1] = 0, then call dfs(2, 1) which sets
//         color[2] = 1 and dfs(4, 0) which sets color[4] = 0.
//       - Return to vertex 1 and call dfs(3, 1) to set color[3] = 1; all vertices
//         1..4 are consistently 2-colored, returning true.
//
//   Example 2:
//     Input:  n = 3, dislikes = [[1, 2], [1, 3], [2, 3]]
//     Output: false
//     Explanation:
//       - Call dfs(1, 0) to set color[1] = 0, then recurse to neighbor 2 via dfs(2,
//         1) setting color[2] = 1.
//       - From vertex 2, recurse to uncolored neighbor 3 via dfs(3, 0) setting
//         color[3] = 0.
//       - From vertex 3, edge (3, 1) expects color[1] == 1, which conflicts with
//         color[1] == 0 and returns false.
//
//   Example 3:
//     Input:  n = 5, dislikes = [[1, 2], [3, 4], [4, 5], [3, 5]]
//     Output: false
//     Explanation:
//       - At i = 1, dfs(1, 0) successfully 2-colors the first component with
//         color[1] = 0 and color[2] = 1.
//       - At i = 3, dfs(3, 0) enters the disconnected second component and assigns
//         color[3] = 0, color[4] = 1, and color[5] = 0.
//       - From vertex 5, neighbor 3 has color[3] = 0 instead of expected 1,
//         detecting the odd cycle and returning false.
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
//   - #785 Is Graph Bipartite? [done] (direct 2-coloring on a given adjacency list)
//   - #207 Course Schedule [done] (graph traversal and cycle detection on dependencies)
//   - #261 Graph Valid Tree [done] (undirected connectivity and acyclicity check)
//   - #1042 Flower Planting With No Adjacent [new] (4-coloring bounded-degree graphs)
//   - #990 Satisfiability of Equality Equations [new] (constraint-based partitioning)

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
