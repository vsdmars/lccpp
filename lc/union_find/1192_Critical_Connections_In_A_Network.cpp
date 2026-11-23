// LeetCode 1192: Critical Connections in a Network
// https://leetcode.com/problems/critical-connections-in-a-network/
//
// Problem:
//   There are `n` servers numbered `0` to `n - 1` connected by undirected
//   network links `connections[i] = [ai, bi]` forming a connected simple graph
//   (no self-loops or duplicate edges). A critical connection (bridge) is any
//   edge whose removal disconnects the network so that some server can no
//   longer reach another. Return all critical connections in any order.
//   Typical constraints: 2 <= n <= 10^5, n - 1 <= connections.length <= 10^5.
//
// Algorithm:
//   Tarjan's Bridge-Finding Algorithm (Single-Pass DFS):
//   1. Build an undirected adjacency list `graph` from `connections`.
//   2. Perform DFS tracking two timestamps per vertex:
//      - `disc[u]`: The time at which vertex `u` is first discovered.
//      - `low[u]`: The smallest discovery time reachable from `u`'s DFS subtree
//        using tree edges plus at most one back-edge.
//   3. For each neighbor `v` of `u` (skipping the immediate `parent`):
//      - If `v` is unvisited (`disc[v] == -1`), recurse on `(v, u)`, then
//        propagate `low[u] = min(low[u], low[v])`. If `low[v] > disc[u]`, no
//        vertex in `v`'s subtree can reach `u` or any ancestor of `u` via an
//        alternate path, so `(u, v)` is not part of any cycle and is a bridge.
//      - Otherwise (`v` is already visited), `(u, v)` is a back-edge; update
//        `low[u] = min(low[u], disc[v])`.
//   4. Loop over all vertices `0..n-1` so disconnected inputs are also handled.
//
//   Complexity:
//     - Time: O(V + E), visiting every vertex and undirected edge once.
//     - Space: O(V + E) for the adjacency list, `disc`/`low` arrays, and DFS
//       recursion stack.
//
// Data Structures:
//   - `std::vector<std::vector<int>> graph`: Undirected adjacency list.
//   - `std::vector<int> disc`: DFS discovery timestamp per vertex (-1 = unseen).
//   - `std::vector<int> low`: Lowest reachable discovery time from each subtree.
//   - `std::vector<std::vector<int>> bridges`: Output list of bridge edges.
//
// Real-World Applications:
//   - Identifying single points of failure in data-center, telecom, and power
//     grid topologies to guide redundancy planning.
//   - Road, rail, and supply-chain vulnerability analysis (finding bottleneck
//     links whose outage isolates regions).
//   - Computing 2-edge-connected components in compilers and circuit design.
//
// Related LeetCode Problems:
//   - #1568 Minimum Number of Days to Disconnect Island (articulation/bridges)
//   - #1489 Find Critical and Pseudo-Critical Edges in Minimum Spanning Tree
//     (identifies edges essential to MST connectivity)
//   - #684 Redundant Connection (dual problem: finds a removable cycle edge)
//   - #1319 Number of Operations to Make Network Connected (network redundancy)
//   - #2360 Longest Cycle in a Graph (DFS timestamp analysis of graph cycles)

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
