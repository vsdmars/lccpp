// LeetCode 684: Redundant Connection
// https://leetcode.com/problems/redundant-connection/
//
// Problem:
//   Given an undirected graph that began as a tree of `n` nodes labeled `1..n`
//   with one additional edge inserted between two distinct vertices, the graph
//   is provided as an array `edges` of length `n` where `edges[i] = [ai, bi]`.
//   Return an edge that can be removed so the remaining graph is a tree of `n`
//   nodes. If multiple edges qualify, return the one that appears last in
//   `edges`. Typical constraints: 3 <= n <= 1000, no duplicate edges.
//
// Algorithm:
//   Incremental Cycle Detection via Disjoint Set Union (Union-Find):
//   1. Determine `max_node` across `edges` and initialize a 1-indexed
//      `DisjointSet` of size `max_node + 1` with every node in its own set.
//   2. Iterate through `edges` in input order and attempt `dsu.unite(u, v)`.
//      - If `u` and `v` belong to different components, merge them and keep
//        going.
//      - If `u` and `v` already share the same root (`unite` returns false),
//        then an alternate path between `u` and `v` already exists using
//        earlier edges, so `[u, v]` closes the unique cycle.
//   Key invariant: Because a tree plus one edge contains exactly one simple
//   cycle, the very first edge whose endpoints are already connected is
//   guaranteed to be the last edge of that cycle in input order, and removing
//   it breaks the cycle while keeping all `n` vertices connected.
//
//   Complexity:
//     - Time: O(N * alpha(N)), where N = edges.size() and alpha is the inverse
//       Ackermann function (effectively constant per union/find).
//     - Space: O(N) for the DSU parent and rank arrays.
//
// Data Structures:
//   - `DisjointSet`: Disjoint Set Union storing `std::vector<std::size_t>
//     parent_` (using iterative path halving in `find`) and `std::vector<int>
//     rank_` (using union by rank in `unite`).
//
// Real-World Applications:
//   - Loop detection and spanning-tree maintenance in Ethernet switches (STP)
//     and network topology provisioning.
//   - Kruskal's Minimum Spanning Tree algorithm (rejecting edges that would
//     form a cycle during incremental tree construction).
//   - Circuit-netlist verification and dependency-graph cycle checks.
//
// Related LeetCode Problems:
//   - #685 Redundant Connection II (directed-graph version with 2-parent case)
//   - #261 Graph Valid Tree (verifies a graph is a single acyclic tree via DSU)
//   - #547 Number of Provinces (counts undirected components using DSU)
//   - #1319 Number of Operations to Make Network Connected (redundant edges)
//   - #1192 Critical Connections in a Network (bridges outside any cycle)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

class DisjointSet {
 public:
  explicit DisjointSet(std::size_t n) : parent_(n), rank_(n, 0) {
    for (std::size_t i = 0uz; i < n; ++i) {
      parent_[i] = i;
    }
  }

  std::size_t find(std::size_t x) {
    while (parent_[x] != x) {
      parent_[x] = parent_[parent_[x]];
      x = parent_[x];
    }
    return x;
  }

  // Returns false if `a` and `b` are already in the same connected component.
  bool unite(std::size_t a, std::size_t b) {
    std::size_t root_a = find(a);
    std::size_t root_b = find(b);
    if (root_a == root_b) {
      return false;
    }
    if (rank_[root_a] < rank_[root_b]) {
      std::swap(root_a, root_b);
    }
    parent_[root_b] = root_a;
    if (rank_[root_a] == rank_[root_b]) {
      ++rank_[root_a];
    }
    return true;
  }

 private:
  std::vector<std::size_t> parent_;
  std::vector<int> rank_;
};

class Solution {
 public:
  // Process edges in input order with Disjoint Set Union (path halving + union
  // by rank). The first edge whose endpoints are already connected closes the
  // unique cycle and is necessarily the last edge of that cycle in input order.
  // Time: O(N * alpha(N))
  // Space: O(N)
  std::vector<int> findRedundantConnection(
      const std::vector<std::vector<int>>& edges) {
    int max_node = 0;
    for (const auto& edge : edges) {
      max_node = std::max({max_node, edge[0], edge[1]});
    }

    DisjointSet dsu(static_cast<std::size_t>(max_node) + 1uz);
    for (const auto& edge : edges) {
      const auto u = static_cast<std::size_t>(edge[0]);
      const auto v = static_cast<std::size_t>(edge[1]);
      if (!dsu.unite(u, v)) {
        return edge;
      }
    }
    return {};
  }
};

LC_PROBLEM(684, "Redundant Connection", "union_find", ctx) {
  ctx.test("example_1_triangle", [&] {
    Solution sol;
    const std::vector<std::vector<int>> edges = {{1, 2}, {1, 3}, {2, 3}};
    ctx.expect_eq(sol.findRedundantConnection(edges), std::vector<int>{2, 3});
  });

  ctx.test("example_2_cycle_with_tail", [&] {
    Solution sol;
    const std::vector<std::vector<int>> edges = {
        {1, 2}, {2, 3}, {3, 4}, {1, 4}, {1, 5}};
    ctx.expect_eq(sol.findRedundantConnection(edges), std::vector<int>{1, 4});
  });

  ctx.test("build_case", [&] {
    Solution sol;
    const std::vector<std::vector<int>> edges = {
        {2, 1}, {3, 1}, {4, 2}, {1, 4}};
    ctx.expect_eq(sol.findRedundantConnection(edges), std::vector<int>{1, 4});
  });

  ctx.test("cycle_closed_at_last_edge", [&] {
    Solution sol;
    const std::vector<std::vector<int>> edges = {
        {1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 2}};
    ctx.expect_eq(sol.findRedundantConnection(edges), std::vector<int>{5, 2});
  });
}

}  // namespace
