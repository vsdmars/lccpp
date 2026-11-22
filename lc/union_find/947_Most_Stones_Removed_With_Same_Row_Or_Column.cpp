// https://leetcode.com/problems/most-stones-removed-with-same-row-or-column/
//
// On a 2D plane, stones are placed at integer coordinates `stones[i] = [r, c]`
// (at most one stone per coordinate). A stone can be removed if it shares a row
// or column with another stone that has not been removed. Return the largest
// possible number of stones that can be removed.

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
