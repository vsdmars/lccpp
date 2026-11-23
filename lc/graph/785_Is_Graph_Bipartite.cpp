// LeetCode 785: Is Graph Bipartite?
// https://leetcode.com/problems/is-graph-bipartite/
//
// Problem:
//   Given an undirected graph of `n` nodes labeled `0..n-1` as an adjacency
//   list `graph` (with no self-edges or parallel edges; the graph may be
//   disconnected), determine whether the vertices can be partitioned into two
//   independent sets A and B such that every edge connects a node in A to a
//   node in B. Return `true` if the graph is bipartite and `false` otherwise.
//
// Algorithm:
//   Recursive DFS 2-coloring across all connected components. A graph is
//   bipartite if and only if it is 2-colorable (equivalently, contains no
//   odd-length cycles). For every uncolored vertex `i`, start a DFS assigning
//   color `0` to `i` and alternating colors (`expected ^ 1`) across every
//   incident edge. Whenever DFS reaches an already-colored vertex `u`, check
//   that `color[u] == expected`; any mismatch witnesses an odd cycle and
//   immediately returns `false`.
//   Time: O(V + E), Space: O(V) for the color array and DFS recursion stack.
//
// Data Structures:
//   - `const std::vector<std::vector<int>>& graph`: Input undirected adjacency
//     list.
//   - `std::vector<std::int8_t> color`: Per-vertex state array storing `-1`
//     (uncolored), `0` (group A), or `1` (group B).
//   - Implicit call stack: Drives recursive DFS 2-coloring per component.
//
// Real-World Applications:
//   - Conflict-free scheduling and partitioning: splitting competing jobs,
//     meetings, or microservices into two non-conflicting time slots or zones.
//   - Hardware and VLSI verification: checking whether a circuit layout or
//     double-patterning lithography mask can be split cleanly into two layers.
//   - Matching market modeling: validating two-sided graphs (e.g., buyers vs.
//     sellers, riders vs. drivers) before running bipartite matching.
//
// Related LeetCode Problems:
//   - #886 Possible Bipartition (2-coloring a dislike graph to split people
//     into two conflict-free groups)
//   - #1042 Flower Planting With No Adjacent (graph coloring with up to 4
//     colors under bounded degree)
//   - #2493 Divide Nodes Into the Maximum Number of Groups (requires verifying
//     bipartiteness per component before BFS layering)
//   - #207 Course Schedule (DFS node-state coloring to detect invalid cycles)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Checks bipartiteness by attempting a 2-coloring via DFS across all
  // connected components.
  // Time: O(V + E), Space: O(V).
  bool isBipartite(const std::vector<std::vector<int>>& graph) {
    const std::size_t n = graph.size();
    // -1 = uncolored, 0 = group A, 1 = group B.
    std::vector<std::int8_t> color(n, -1);

    auto dfs = [&](this auto&& self, std::size_t u,
                   std::int8_t expected) -> bool {
      if (color[u] != -1) {
        return color[u] == expected;
      }
      color[u] = expected;
      const auto next_color = static_cast<std::int8_t>(expected ^ 1);
      for (const int v : graph[u]) {
        if (!self(static_cast<std::size_t>(v), next_color)) {
          return false;
        }
      }
      return true;
    };

    for (std::size_t i = 0uz; i < n; ++i) {
      if (color[i] == -1 && !dfs(i, 0)) {
        return false;
      }
    }
    return true;
  }
};

LC_PROBLEM(785, "Is Graph Bipartite", "graph", ctx) {
  Solution sol;

  ctx.test("example 1: 4-cycle is bipartite", [&] {
    ctx.expect_true(sol.isBipartite({{1, 3}, {0, 2}, {1, 3}, {0, 2}}));
  });

  ctx.test("example 2: odd cycle makes graph non-bipartite", [&] {
    ctx.expect_false(sol.isBipartite({{1, 2, 3}, {0, 2}, {0, 1, 3}, {0, 2}}));
  });

  ctx.test("disconnected graph with isolated nodes", [&] {
    ctx.expect_true(sol.isBipartite({{}, {2}, {1}, {4}, {3}}));
  });
}

}  // namespace
