// https://leetcode.com/problems/range-sum-query-2d-mutable/description/
//
// Given a 2D integer matrix, handle multiple queries of two types:
// 1. `update(row, col, val)`: update `matrix[row][col]` to `val`.
// 2. `sumRegion(row1, col1, row2, col2)`: return the sum of elements inside
//    the submatrix with upper-left corner `(row1, col1)` and lower-right
//    corner `(row2, col2)` inclusive.

#include "framework/lc_framework.h"

#include <cstddef>
#include <utility>
#include <vector>

namespace {

// 2D Binary Indexed Tree (Fenwick Tree) with O(M * N) construction and
// O(log M * log N) point updates and rectangle sum queries via 2D
// inclusion-exclusion.
// Space: O(M * N).
class NumMatrix {
 public:
  explicit NumMatrix(std::vector<std::vector<int>> matrix)
      : vals_(std::move(matrix)),
        rows_(vals_.size()),
        cols_(vals_.empty() ? 0uz : vals_[0].size()),
        tree_(rows_ + 1uz, std::vector<int>(cols_ + 1uz, 0)) {
    build_tree();
  }

  void update(int row, int col, int val) {
    const auto r0 = static_cast<std::size_t>(row);
    const auto c0 = static_cast<std::size_t>(col);
    const int delta = val - vals_[r0][c0];
    vals_[r0][c0] = val;

    for (std::size_t r = r0 + 1uz; r <= rows_; r += lowbit(r)) {
      for (std::size_t c = c0 + 1uz; c <= cols_; c += lowbit(c)) {
        tree_[r][c] += delta;
      }
    }
  }

  [[nodiscard]] bool empty() const noexcept {
    return rows_ == 0uz || cols_ == 0uz;
  }

  [[nodiscard]] int sumRegion(int row1, int col1, int row2, int col2) const {
    const auto r1 = static_cast<std::size_t>(row1);
    const auto c1 = static_cast<std::size_t>(col1);
    const auto r2 = static_cast<std::size_t>(row2) + 1uz;
    const auto c2 = static_cast<std::size_t>(col2) + 1uz;
    return prefix_sum(r2, c2) - prefix_sum(r1, c2) - prefix_sum(r2, c1) +
           prefix_sum(r1, c1);
  }

 private:
  static constexpr std::size_t lowbit(std::size_t x) noexcept {
    return x & ((~x) + 1uz);
  }

  void build_tree() {
    for (std::size_t r = 0uz; r < rows_; ++r) {
      for (std::size_t c = 0uz; c < cols_; ++c) {
        tree_[r + 1uz][c + 1uz] = vals_[r][c];
      }
    }
    for (std::size_t r = 1uz; r <= rows_; ++r) {
      const std::size_t parent_r = r + lowbit(r);
      if (parent_r <= rows_) {
        for (std::size_t c = 1uz; c <= cols_; ++c) {
          tree_[parent_r][c] += tree_[r][c];
        }
      }
    }
    for (std::size_t r = 1uz; r <= rows_; ++r) {
      for (std::size_t c = 1uz; c <= cols_; ++c) {
        const std::size_t parent_c = c + lowbit(c);
        if (parent_c <= cols_) {
          tree_[r][parent_c] += tree_[r][c];
        }
      }
    }
  }

  [[nodiscard]] int prefix_sum(std::size_t row, std::size_t col) const {
    int sum = 0;
    for (std::size_t r = row; r > 0uz; r -= lowbit(r)) {
      for (std::size_t c = col; c > 0uz; c -= lowbit(c)) {
        sum += tree_[r][c];
      }
    }
    return sum;
  }

  std::vector<std::vector<int>> vals_;
  std::size_t rows_ = 0uz;
  std::size_t cols_ = 0uz;
  std::vector<std::vector<int>> tree_;
};

LC_PROBLEM(308, "Range Sum Query 2D Mutable", "binary_index_tree", ctx) {
  ctx.test("leetcode_example", [&] {
    NumMatrix mat({
        {3, 0, 1, 4, 2},
        {5, 6, 3, 2, 1},
        {1, 2, 0, 1, 5},
        {4, 1, 0, 1, 7},
        {1, 0, 3, 0, 5},
    });
    ctx.expect_eq(mat.sumRegion(2, 1, 4, 3), 8);
    mat.update(3, 2, 2);
    ctx.expect_eq(mat.sumRegion(2, 1, 4, 3), 10);
  });

  ctx.test("single_cell_and_updates", [&] {
    NumMatrix mat({{5}});
    ctx.expect_eq(mat.sumRegion(0, 0, 0, 0), 5);
    mat.update(0, 0, -4);
    ctx.expect_eq(mat.sumRegion(0, 0, 0, 0), -4);
  });

  ctx.test("full_matrix_and_negative_values", [&] {
    NumMatrix mat({
        {1, -2, 3},
        {-4, 5, -6},
    });
    ctx.expect_eq(mat.sumRegion(0, 0, 1, 2), -3);
    ctx.expect_eq(mat.sumRegion(0, 1, 1, 1), 3);
    mat.update(1, 0, 4);
    mat.update(0, 2, -1);
    ctx.expect_eq(mat.sumRegion(0, 0, 1, 2), 1);
    ctx.expect_eq(mat.sumRegion(1, 0, 1, 2), 3);
  });

  ctx.test("empty_matrix_safe", [&] {
    const NumMatrix mat({});
    ctx.expect_true(mat.empty());
  });
}

}  // namespace
