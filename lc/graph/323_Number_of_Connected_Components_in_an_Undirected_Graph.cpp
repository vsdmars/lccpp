// LeetCode 323: Number of Connected Components in an Undirected Graph
// https://leetcode.com/problems/number-of-connected-components-in-an-undirected-graph/
//
// Problem:
//   Given `n` vertices labeled from 0 to `n - 1` and a list of undirected edges
//   `edges[i] = [u, v]`, return the total number of connected components in the
//   graph (including isolated vertices that have no incident edges).
//
// Algorithm:
//   Construct an undirected adjacency list by inserting both `u -> v` and
//   `v -> u` for every edge. Iterate through all vertices `0..n-1`; whenever an
//   unvisited vertex is found, launch a recursive DFS from that vertex to mark
//   every reachable vertex in its component as visited, then increment the
//   `components` counter.
//   Key invariant: each top-level DFS explores one maximal connected component
//   in full, so every vertex is visited once and every edge is examined twice.
//   Time Complexity:  O(V + E), where V = n and E = edges.size().
//   Space Complexity: O(V + E) for the adjacency list, visited array, and DFS
//                     recursion stack (up to O(V) depth).
//
// Data Structures:
//   - `std::vector<std::vector<int>> adj`: undirected adjacency list storing
//     neighbors for each vertex `0..n-1`.
//   - `std::vector<std::uint8_t> visited`: byte-packed visitation table marking
//     explored vertices to prevent revisits and infinite recursion on cycles.
//   - Call stack (recursive lambda `dfs`): drives depth-first component flood.
//
// Real-World Applications:
//   - Social network and community analysis identifying isolated clusters of
//     users or accounts.
//   - Network monitoring and distributed systems detecting network partitions
//     (split-brain clusters) among nodes.
//   - Entity resolution and deduplication grouping linked customer or device
//     records into unified profiles.
//   - Image processing and mesh analysis counting separate geometric shells.
//
// Related LeetCode Problems:
//   - #261 Graph Valid Tree [done] (checks for 1 connected component and n - 1 edges)
//   - #547 Number of Provinces [done] (counts components given an adjacency matrix)
//   - #200 Number of Islands [done] (counts connected components on an implicit grid)
//   - #1319 Number of Operations to Make Network Connected [done] (uses component
//     count to determine minimum rewires)
//   - #2316 Count Unreachable Pairs of Nodes in an Undirected Graph [new] (uses
//     component sizes to count disconnected vertex pairs)

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
