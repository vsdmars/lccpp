// https://leetcode.com/problems/graph-valid-tree/description/
//
// Given `n` nodes labeled 0..n-1 and a list of undirected edges, determine
// whether these edges form a valid tree.
//
// A graph with `n` vertices is a valid tree if and only if:
//   1. It has exactly `n - 1` edges, and
//   2. It is acyclic (equivalently, fully connected).

#include "framework/lc_framework.h"

#include <cstddef>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Checks tree validity via edge-count check + Union-Find cycle detection.
  // Time: O(n * alpha(n)), Space: O(n).
  bool validTree(int n, const std::vector<std::vector<int>>& edges) {
    if (n <= 0 || edges.size() != static_cast<std::size_t>(n - 1)) {
      return false;
    }

    const auto count = static_cast<std::size_t>(n);
    std::vector<int> parent(count);
    for (std::size_t i = 0uz; i < count; ++i) {
      parent[i] = static_cast<int>(i);
    }
    std::vector<int> rank(count, 0);

    // Find with path compression.
    auto find = [&](this auto&& self, int x) -> int {
      const auto idx = static_cast<std::size_t>(x);
      if (parent[idx] != x) {
        parent[idx] = self(parent[idx]);
      }
      return parent[idx];
    };

    for (const auto& edge : edges) {
      int rx = find(edge[0]);
      int ry = find(edge[1]);
      if (rx == ry) {
        return false;  // Edge closes a cycle.
      }
      // Union by rank.
      const auto ux = static_cast<std::size_t>(rx);
      const auto uy = static_cast<std::size_t>(ry);
      if (rank[ux] < rank[uy]) {
        std::swap(rx, ry);
      }
      parent[static_cast<std::size_t>(ry)] = rx;
      if (rank[static_cast<std::size_t>(rx)] ==
          rank[static_cast<std::size_t>(ry)]) {
        ++rank[static_cast<std::size_t>(rx)];
      }
    }

    return true;
  }
};

LC_PROBLEM(261, "Graph Valid Tree", "graph", ctx) {
  Solution sol;

  ctx.test("example 1: valid 5-node tree", [&] {
    ctx.expect_true(sol.validTree(5, {{0, 1}, {0, 2}, {0, 3}, {1, 4}}));
  });

  ctx.test("example 2: extra edge forms cycle", [&] {
    ctx.expect_false(
        sol.validTree(5, {{0, 1}, {1, 2}, {2, 3}, {1, 3}, {1, 4}}));
  });

  ctx.test("n-1 edges but disconnected with cycle", [&] {
    ctx.expect_false(sol.validTree(5, {{0, 1}, {0, 4}, {1, 4}, {2, 3}}));
  });

  ctx.test("small trees and triangles", [&] {
    ctx.expect_true(sol.validTree(1, {}));
    ctx.expect_true(sol.validTree(2, {{0, 1}}));
    ctx.expect_false(sol.validTree(3, {{0, 1}, {1, 2}, {2, 0}}));
  });
}

}  // namespace
