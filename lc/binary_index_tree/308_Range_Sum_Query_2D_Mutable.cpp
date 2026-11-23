// LeetCode 308: Range Sum Query 2D - Mutable
// https://leetcode.com/problems/range-sum-query-2d-mutable/
//
// Problem:
//   Design a data structure initialized with an `M x N` 2D integer matrix that
//   efficiently supports two interleaved operations:
//   1. `update(row, col, val)`: set `matrix[row][col]` to `val`.
//   2. `sumRegion(row1, col1, row2, col2)`: return the sum of all elements in
//      the inclusive submatrix bounded by upper-left `(row1, col1)` and
//      lower-right `(row2, col2)`.
//
// Examples:
//   Example 1:
//     Input:  NumMatrix([[3, 0, 1, 4, 2], [5, 6, 3, 2, 1], [1, 2, 0, 1, 5], [4, 1,
//             0, 1, 7], [1, 0, 3, 0, 5]]), sumRegion(2, 1, 4, 3), update(3, 2, 2),
//             sumRegion(2, 1, 4, 3)
//     Output: sumRegion(2, 1, 4, 3) = 8, sumRegion(2, 1, 4, 3) = 10
//     Explanation:
//       - build_tree() builds the 6x6 2D Fenwick tree_ by copying vals_ and
//         propagating sums along row parents then column parents.
//       - sumRegion(2, 1, 4, 3) evaluates prefix_sum(5, 4) - prefix_sum(2, 4) -
//         prefix_sum(5, 1) + prefix_sum(2, 1) = 38 - 24 - 14 + 8 = 8.
//       - update(3, 2, 2) computes delta = 2 - 0 = 2, sets vals_[3][2] = 2, and
//         adds 2 at 1-based row r = 4 across columns c = 3 and c = 4 (next r = 8
//         and c = 8 exceed bounds).
//       - Re-evaluating sumRegion(2, 1, 4, 3) sees prefix_sum(5, 4) rise by 2 to 40
//         (via tree_[4][4]) while the other three terms stay unchanged, returning
//         40 - 24 - 14 + 8 = 10.
//
//   Example 2:
//     Input:  NumMatrix([[1, -2, 3], [-4, 5, -6]]), sumRegion(0, 0, 1, 2),
//             sumRegion(0, 1, 1, 1), update(1, 0, 4), update(0, 2, -1),
//             sumRegion(0, 0, 1, 2), sumRegion(1, 0, 1, 2)
//     Output: sumRegion(0, 0, 1, 2) = -3, sumRegion(0, 1, 1, 1) = 3, sumRegion(0,
//             0, 1, 2) = 1, sumRegion(1, 0, 1, 2) = 3
//     Explanation:
//       - Initial queries evaluate sumRegion(0, 0, 1, 2) = prefix_sum(2, 3) = -3
//         and sumRegion(0, 1, 1, 1) = prefix_sum(2, 2) - prefix_sum(2, 1) = 0 -
//         (-3) = 3.
//       - update(1, 0, 4) adds delta = 4 - (-4) = 8 to tree_[2][1] and tree_[2][2],
//         and update(0, 2, -1) adds delta = -1 - 3 = -4 to tree_[1][3] and
//         tree_[2][3].
//       - After both updates, sumRegion(0, 0, 1, 2) returns -3 + 8 - 4 = 1, and
//         sumRegion(1, 0, 1, 2) returns prefix_sum(2, 3) - prefix_sum(1, 3) = 1 -
//         (-2) = 3.
//
//   Example 3:
//     Input:  NumMatrix([[5]]), sumRegion(0, 0, 0, 0), update(0, 0, -4),
//             sumRegion(0, 0, 0, 0)
//     Output: sumRegion(0, 0, 0, 0) = 5, sumRegion(0, 0, 0, 0) = -4
//     Explanation:
//       - build_tree() sets rows_ = 1, cols_ = 1, and tree_[1][1] = 5, so
//         sumRegion(0, 0, 0, 0) returns prefix_sum(1, 1) = 5.
//       - update(0, 0, -4) computes delta = -4 - 5 = -9, updates vals_[0][0] = -4,
//         and adds -9 to tree_[1][1], so the next sumRegion(0, 0, 0, 0) returns -4.
//
// Algorithm:
//   2D Binary Indexed Tree (2D Fenwick Tree). Entry `tree_[r][c]` stores the
//   sum of the rectangle of height `lowbit(r)` and width `lowbit(c)` ending at
//   1-based cell `(r, c)`. Construction runs in O(M * N) by copying `vals_`
//   into `tree_` and propagating partial sums first along row parents
//   (`r + lowbit(r)`) and then along column parents (`c + lowbit(c)`). Point
//   updates compute `delta = val - vals_[r0][c0]` and add `delta` across all
//   covering ancestors via nested `r += lowbit(r)` and `c += lowbit(c)` loops.
//   Rectangle queries combine four 2D prefix sums via inclusion-exclusion:
//   `P(r2, c2) - P(r1, c2) - P(r2, c1) + P(r1, c1)`.
//   Time: O(M * N) build, O(log M * log N) per `update` and `sumRegion`.
//   Space: O(M * N).
//
// Data Structures:
//   - `std::vector<std::vector<int>> vals_`: stores current cell values so
//     point updates can compute the change `delta` in O(1) time.
//   - `std::vector<std::vector<int>> tree_`: `(M + 1) x (N + 1)` 1-indexed 2D
//     Fenwick tree storing hierarchical 2D partial sums.
//   - `std::size_t rows_`, `cols_`: cached matrix dimensions for bounds checks.
//
// Real-World Applications:
//   - Dynamic summed-area tables (integral images) in computer vision and
//     graphics when localized pixel regions change interactively.
//   - Geospatial and heatmap analytics aggregating live event counts over
//     arbitrary rectangular map viewports.
//   - Online spreadsheet engines and OLAP data cubes supporting frequent cell
//     edits alongside fast rectangular range aggregations.
//
// Related LeetCode Problems:
//   - #304 Range Sum Query 2D [done] - Immutable (static 2D prefix sums using the
//     same 2D inclusion-exclusion formula)
//   - #307 Range Sum Query [done] - Mutable (1D Fenwick tree counterpart)
//   - #1314 Matrix Block Sum [done] (sliding 2D rectangular sums via 2D prefix sums)
//   - #1074 Number of Submatrices That Sum to Target [done] (combines 2D prefix sums
//     with hash-map range counting)

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
