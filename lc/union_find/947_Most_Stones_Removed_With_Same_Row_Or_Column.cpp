// LeetCode 947: Most Stones Removed with Same Row or Column
// https://leetcode.com/problems/most-stones-removed-with-same-row-or-column/
//
// Problem:
//   On a 2D integer plane, `n` stones are placed at distinct coordinates
//   `stones[i] = [r, c]` (with `0 <= r, c`). A stone may be removed if it
//   shares either the same row or the same column with at least one other
//   stone that has not yet been removed. Return the maximum number of stones
//   that can be removed.
//
// Algorithm:
//   1. Model the grid as a bipartite graph between row nodes and column nodes.
//      Encode each row `r` as non-negative integer `r` and each column `c` as
//      negative integer `-(c + 1)` so row and column namespaces never collide
//      (including at coordinate 0).
//   2. For every stone `(r, c)`, unite `row_node` and `col_node` inside a
//      sparse Union-Find (`SparseDisjointSet`) that lazily registers unseen
//      coordinates and tracks the active number of connected components.
//   3. Return `stones.size() - dsu.components()`.
//   Key Invariant:
//     Two stones belong to the same connected component if and only if they
//     are linked by a chain of shared rows and columns. In any connected
//     component of `k` stones, removing stones in reverse spanning-tree order
//     (leaves first) always removes `k - 1` stones and leaves exactly 1 stone.
//     Summing across `C` components yields `total_stones - C` removals.
//   Time Complexity:
//     O(N * alpha(N)) average, where N = `stones.size()` and alpha is the
//     inverse Ackermann function.
//   Space Complexity:
//     O(N) for at most `2 * N` active row and column nodes in the hash map.
//
// Data Structures:
//   - `SparseDisjointSet` (`parent_`: `std::unordered_map<int, int>`,
//     `components_`: `int`): Hash-map-backed Union-Find pre-reserved for
//     `2 * stones.size()` entries, using lazy insertion on `find` and two-pass
//     iterative full path compression.
//
// Real-World Applications:
//   - Bipartite graph connectivity and redundancy reduction (e.g., pruning
//     redundant dependencies in matrix/grid constraints).
//   - Circuit-board and VLSI netlist connectivity across horizontal and
//     vertical routing tracks.
//   - Sparse matrix block-diagonal decomposition by grouping coupled rows and
//     columns into independent subproblems.
//
// Related LeetCode Problems:
//   - #547 Number of Provinces [done] (counting connected components via Union-Find)
//   - #200 Number of Islands [done] (connected components on a 2D grid)
//   - #1267 Count Servers that Communicate [new] (row and column connectivity on a
//     2D grid)
//   - #721 Accounts Merge [done] (bipartite-style merging of entities via Union-Find)
//   - #1319 Number of Operations to Make Network Connected [done] (spanning tree
//     edges vs. connected components)

#include "framework/lc_framework.h"

#include <cstddef>
#include <unordered_map>
#include <vector>

namespace {

class SparseDisjointSet {
 public:
  explicit SparseDisjointSet(std::size_t reserve_hint) {
    parent_.reserve(reserve_hint);
  }

  int find(int x) {
    if (parent_.emplace(x, x).second) {
      ++components_;
      return x;
    }
    int root = x;
    while (parent_[root] != root) {
      root = parent_[root];
    }
    while (parent_[x] != root) {
      const int next = parent_[x];
      parent_[x] = root;
      x = next;
    }
    return root;
  }

  void unite(int a, int b) {
    const int root_a = find(a);
    const int root_b = find(b);
    if (root_a != root_b) {
      parent_[root_a] = root_b;
      --components_;
    }
  }

  [[nodiscard]] int components() const { return components_; }

 private:
  std::unordered_map<int, int> parent_;
  int components_ = 0;
};

class Solution {
 public:
  // Model each row `r` as non-negative node `r` and each column `c` as negative
  // node `-(c + 1)` so row and column namespaces never collide. Every stone
  // `(r, c)` connects its row node and column node. Within each connected
  // component of `k` stones, exactly `k - 1` stones can be removed, leaving 1
  // stone per component: answer = total_stones - num_components.
  // Time: O(N * alpha(N)) average
  // Space: O(N)
  int removeStones(const std::vector<std::vector<int>>& stones) {
    SparseDisjointSet dsu(stones.size() * 2uz);
    for (const auto& stone : stones) {
      const int row_node = stone[0];
      const int col_node = -(stone[1] + 1);
      dsu.unite(row_node, col_node);
    }
    return static_cast<int>(stones.size()) - dsu.components();
  }
};

LC_PROBLEM(947, "Most Stones Removed With Same Row Or Column", "union_find",
           ctx) {
  ctx.test("example_1_single_component", [&] {
    Solution sol;
    const std::vector<std::vector<int>> stones = {
        {0, 0}, {0, 1}, {1, 0}, {1, 2}, {2, 1}, {2, 2}};
    ctx.expect_eq(sol.removeStones(stones), 5);
  });

  ctx.test("example_2_two_components", [&] {
    Solution sol;
    const std::vector<std::vector<int>> stones = {
        {0, 0}, {0, 2}, {1, 1}, {2, 0}, {2, 2}};
    ctx.expect_eq(sol.removeStones(stones), 3);
  });

  ctx.test("example_3_single_stone", [&] {
    Solution sol;
    const std::vector<std::vector<int>> stones = {{0, 0}};
    ctx.expect_eq(sol.removeStones(stones), 0);
  });

  ctx.test("l_shape_build_case", [&] {
    Solution sol;
    const std::vector<std::vector<int>> stones = {{0, 1}, {1, 0}, {1, 1}};
    ctx.expect_eq(sol.removeStones(stones), 2);
  });

  ctx.test("all_isolated_diagonal", [&] {
    Solution sol;
    const std::vector<std::vector<int>> stones = {{0, 0}, {1, 1}, {2, 2}};
    ctx.expect_eq(sol.removeStones(stones), 0);
  });

  ctx.test("large_coordinates", [&] {
    Solution sol;
    const std::vector<std::vector<int>> stones = {
        {0, 10000}, {10000, 10000}, {10000, 0}};
    ctx.expect_eq(sol.removeStones(stones), 2);
  });
}

}  // namespace
