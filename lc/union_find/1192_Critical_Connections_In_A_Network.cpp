// https://leetcode.com/problems/critical-connections-in-a-network/
//
// There are `n` servers numbered `0..n-1` connected by undirected edges
// `connections[i] = [ai, bi]`. A critical connection (bridge) is an edge whose
// removal disconnects some servers from others. Return all critical connections
// in any order.

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Tarjan's bridge-finding DFS: an edge (u, v) is a bridge iff it is not part
  // of any cycle, which holds precisely when the earliest discovery time
  // reachable from v's DFS subtree (`low[v]`) is strictly greater than `disc[u]`.
  // Time: O(V + E)
  // Space: O(V + E)
  std::vector<std::vector<int>> criticalConnections(
      int n, const std::vector<std::vector<int>>& connections) {
    const auto node_count = static_cast<std::size_t>(n);
    std::vector<std::vector<int>> graph(node_count);
    for (const auto& edge : connections) {
      graph[static_cast<std::size_t>(edge[0])].push_back(edge[1]);
      graph[static_cast<std::size_t>(edge[1])].push_back(edge[0]);
    }

    std::vector<int> disc(node_count, -1);
    std::vector<int> low(node_count, 0);
    std::vector<std::vector<int>> bridges;
    int timer = 0;

    auto dfs = [&](this auto&& self, int u, int parent) -> void {
      const auto uid = static_cast<std::size_t>(u);
      disc[uid] = low[uid] = timer++;
      for (int v : graph[uid]) {
        if (v == parent) {
          continue;
        }
        const auto vid = static_cast<std::size_t>(v);
        if (disc[vid] == -1) {
          self(v, u);
          low[uid] = std::min(low[uid], low[vid]);
          if (low[vid] > disc[uid]) {
            bridges.push_back({u, v});
          }
        } else {
          low[uid] = std::min(low[uid], disc[vid]);
        }
      }
    };

    for (int i = 0; i < n; ++i) {
      if (disc[static_cast<std::size_t>(i)] == -1) {
        dfs(i, -1);
      }
    }
    return bridges;
  }
};

// Canonicalizes undirected edge lists for order-independent comparison.
std::vector<std::vector<int>> normalize_edges(
    std::vector<std::vector<int>> edges) {
  for (auto& edge : edges) {
    if (edge[0] > edge[1]) {
      std::swap(edge[0], edge[1]);
    }
  }
  std::ranges::sort(edges);
  return edges;
}

LC_PROBLEM(1192, "Critical Connections In A Network", "union_find", ctx) {
  ctx.test("example_1_triangle_with_tail", [&] {
    Solution sol;
    const std::vector<std::vector<int>> connections = {
        {0, 1}, {1, 2}, {2, 0}, {1, 3}};
    const std::vector<std::vector<int>> expected = {{1, 3}};
    ctx.expect_eq(normalize_edges(sol.criticalConnections(4, connections)),
                  expected);
  });

  ctx.test("example_2_single_edge", [&] {
    Solution sol;
    const std::vector<std::vector<int>> connections = {{0, 1}};
    const std::vector<std::vector<int>> expected = {{0, 1}};
    ctx.expect_eq(normalize_edges(sol.criticalConnections(2, connections)),
                  expected);
  });

  ctx.test("pure_cycle_no_bridges", [&] {
    Solution sol;
    const std::vector<std::vector<int>> connections = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}};
    const std::vector<std::vector<int>> expected = {};
    ctx.expect_eq(normalize_edges(sol.criticalConnections(4, connections)),
                  expected);
  });

  ctx.test("chain_all_bridges", [&] {
    Solution sol;
    const std::vector<std::vector<int>> connections = {{0, 1}, {1, 2}, {2, 3}};
    const std::vector<std::vector<int>> expected = {{0, 1}, {1, 2}, {2, 3}};
    ctx.expect_eq(normalize_edges(sol.criticalConnections(4, connections)),
                  expected);
  });

  ctx.test("two_triangles_with_bridge", [&] {
    Solution sol;
    const std::vector<std::vector<int>> connections = {
        {0, 1}, {1, 2}, {2, 0}, {2, 3}, {3, 4}, {4, 5}, {5, 3}};
    const std::vector<std::vector<int>> expected = {{2, 3}};
    ctx.expect_eq(normalize_edges(sol.criticalConnections(6, connections)),
                  expected);
  });

  ctx.test("figure_eight_shared_vertex_no_bridges", [&] {
    Solution sol;
    const std::vector<std::vector<int>> connections = {
        {0, 1}, {1, 2}, {2, 0}, {2, 3}, {3, 4}, {4, 2}};
    const std::vector<std::vector<int>> expected = {};
    ctx.expect_eq(normalize_edges(sol.criticalConnections(5, connections)),
                  expected);
  });
}

}  // namespace
