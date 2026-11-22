// https://leetcode.com/problems/number-of-connected-components-in-an-undirected-graph/description/
//
// Given `n` nodes labeled 0..n-1 and a list of undirected edges, return the
// number of connected components in the graph.

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Counts connected components by running DFS from every unvisited vertex.
  // Time: O(V + E), Space: O(V + E).
  int countComponents(int n, const std::vector<std::vector<int>>& edges) {
    if (n <= 0) {
      return 0;
    }

    const auto count = static_cast<std::size_t>(n);
    std::vector<std::vector<int>> adj(count);
    for (const auto& edge : edges) {
      adj[static_cast<std::size_t>(edge[0])].push_back(edge[1]);
      adj[static_cast<std::size_t>(edge[1])].push_back(edge[0]);
    }

    std::vector<std::uint8_t> visited(count, 0);
    auto dfs = [&](this auto&& self, int u) -> void {
      visited[static_cast<std::size_t>(u)] = 1;
      for (const int v : adj[static_cast<std::size_t>(u)]) {
        if (visited[static_cast<std::size_t>(v)] == 0) {
          self(v);
        }
      }
    };

    int components = 0;
    for (int node = 0; node < n; ++node) {
      if (visited[static_cast<std::size_t>(node)] == 0) {
        dfs(node);
        ++components;
      }
    }
    return components;
  }
};

LC_PROBLEM(323, "Number of Connected Components in an Undirected Graph",
           "graph", ctx) {
  Solution sol;

  ctx.test("example 1: two components", [&] {
    ctx.expect_eq(sol.countComponents(5, {{0, 1}, {1, 2}, {3, 4}}), 2);
  });

  ctx.test("example 2: single chain", [&] {
    ctx.expect_eq(sol.countComponents(5, {{0, 1}, {1, 2}, {2, 3}, {3, 4}}), 1);
  });

  ctx.test("isolated vertex plus cycle", [&] {
    ctx.expect_eq(sol.countComponents(4, {{2, 3}, {1, 2}, {1, 3}}), 2);
  });

  ctx.test("no edges", [&] { ctx.expect_eq(sol.countComponents(4, {}), 4); });
}

}  // namespace
