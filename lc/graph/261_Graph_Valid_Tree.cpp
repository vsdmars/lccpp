// LeetCode 261: Graph Valid Tree
// https://leetcode.com/problems/graph-valid-tree/
//
// Problem:
//   Given `n` vertices labeled from 0 to `n - 1` and a list of undirected edges
//   `edges[i] = [u, v]`, determine whether the edges form a valid tree. Return
//   true if the graph is a single connected, acyclic component spanning all `n`
//   nodes, and false otherwise.
//
// Examples:
//   Example 1:
//     Input:  n = 5, edges = [[0, 1], [0, 2], [0, 3], [1, 4]]
//     Output: true
//     Explanation:
//       - Verify edges.size() == n - 1 (4 == 4) and initialize parent = [0, 1, 2,
//         3, 4], rank = [0, 0, 0, 0, 0].
//       - Process [0, 1], [0, 2], and [0, 3]: union [0, 1] sets parent[1] = 0 and
//         rank[0] = 1, then 2 and 3 attach directly under root 0.
//       - Process [1, 4]: find(1) = 0 and find(4) = 4 are distinct roots, so
//         parent[4] becomes 0 and validTree returns true.
//
//   Example 2:
//     Input:  n = 5, edges = [[0, 1], [1, 2], [2, 3], [1, 3], [1, 4]]
//     Output: false
//     Explanation:
//       - Check the edge-count invariant edges.size() == n - 1.
//       - Since 5 != 4, the graph has too many edges to be a tree and validTree
//         immediately returns false.
//
//   Example 3:
//     Input:  n = 5, edges = [[0, 1], [0, 4], [1, 4], [2, 3]]
//     Output: false
//     Explanation:
//       - Pass the edge-count check (4 == 5 - 1) and initialize parent = [0, 1, 2,
//         3, 4], rank = [0, 0, 0, 0, 0].
//       - Process [0, 1] (parent[1] = 0, rank[0] = 1) and [0, 4] (parent[4] = 0),
//         merging {0, 1, 4} under root 0.
//       - Process [1, 4]: find(1) == 0 == find(4), which detects a cycle and
//         returns false.
//
// Algorithm:
//   Use the graph-theoretic characterization of trees: an undirected graph on
//   `n` vertices is a tree if and only if it has exactly `n - 1` edges AND is
//   acyclic (which, combined with `n - 1` edges, implies full connectivity).
//   First reject immediately if `n <= 0` or `edges.size() != n - 1`. Then scan
//   each edge `[u, v]` using Disjoint Set Union (Union-Find) with path
//   compression and union by rank. If `find(u) == find(v)`, `u` and `v` are
//   already connected so adding the edge forms a cycle and we return false. If
//   all `n - 1` edges merge distinct components without a cycle, return true.
//   Time Complexity:  O(n * alpha(n)), where alpha is the inverse Ackermann
//                     function (nearly O(n) in practice).
//   Space Complexity: O(n) for the `parent` and `rank` arrays.
//
// Data Structures:
//   - `std::vector<int> parent`: disjoint-set forest parent pointers updated
//     lazily with recursive path compression in `find`.
//   - `std::vector<int> rank`: upper bound on each root's subtree height for
//     union by rank, keeping trees shallow.
//
// Real-World Applications:
//   - Network topology verification (e.g., Spanning Tree Protocol preventing
//     Ethernet switching loops while maintaining full reachability).
//   - Validating hierarchical data structures such as org charts, file system
//     mount trees, or scene graphs for accidental cycles or orphans.
//   - Kruskal's minimum spanning tree construction, which incrementally tests
//     candidate edges for cycle formation.
//
// Related LeetCode Problems:
//   - #323 Number of Connected Components in an Undirected Graph [done] (counts
//     disjoint components using graph traversal or Union-Find)
//   - #684 Redundant Connection [done] (finds the exact edge that creates a cycle)
//   - #685 Redundant Connection II [done] (directed-tree variant with 2-parent cases)
//   - #1319 Number of Operations to Make Network Connected [done] (connectivity and
//     redundant-edge counting via Union-Find)
//   - #1361 Validate Binary Tree Nodes [done] (tree-validity check on binary trees)

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
