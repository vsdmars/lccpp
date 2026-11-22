// https://leetcode.com/problems/is-graph-bipartite/description/
//
// Given an undirected graph represented as an adjacency list `graph`, return
// true if and only if its vertices can be 2-colored so that no edge connects
// two vertices of the same color.

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
