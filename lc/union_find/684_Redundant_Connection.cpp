// https://leetcode.com/problems/redundant-connection/description/
//
// In an undirected graph that started as a tree with N nodes labeled 1..N plus
// one additional edge, return an edge that can be removed so the resulting
// graph is a tree. If there are multiple answers, return the one that occurs
// last in the input `edges` array.

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
