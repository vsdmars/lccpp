// LeetCode 304: Range Sum Query 2D Immutable
// https://leetcode.com/problems/range-sum-query-2d-immutable/
//
// Problem:
//   Given an immutable 2D integer matrix `matrix`, design a data structure
//   `NumMatrix` supporting repeated queries `sumRegion(row1, col1, row2, col2)`
//   that return the sum of all elements inside the axis-aligned rectangle whose
//   upper-left corner is `(row1, col1)` and lower-right corner is
//   `(row2, col2)` (inclusive, with `row1 <= row2` and `col1 <= col2`).
//
// Examples:
//   Example 1:
//     Input:  matrix = [[3, 0, 1, 4, 2], [5, 6, 3, 2, 1], [1, 2, 0, 1, 5], [4, 1,
//             0, 1, 7], [1, 0, 3, 0, 5]], queries = [sumRegion(2, 1, 4, 3),
//             sumRegion(1, 1, 2, 2), sumRegion(1, 2, 2, 4)]
//     Output: [8, 11, 12]
//     Explanation:
//       - Build the 6 x 6 zero-padded prefix_ table where prefix_[r + 1][c + 1] =
//         matrix[r][c] + prefix_[r][c + 1] + prefix_[r + 1][c] - prefix_[r][c],
//         giving rows [0, 3, 3, 4, 8, 10], [0, 8, 14, 18, 24, 27], [0, 9, 17, 21,
//         28, 36], [0, 13, 22, 26, 34, 49], and [0, 14, 23, 30, 38, 58] after row 0
//         of zeros.
//       - For sumRegion(2, 1, 4, 3), compute prefix_[5][4] - prefix_[2][4] -
//         prefix_[5][1] + prefix_[2][1] = 38 - 24 - 14 + 8 = 8.
//       - For sumRegion(1, 1, 2, 2), compute prefix_[3][3] - prefix_[1][3] -
//         prefix_[3][1] + prefix_[1][1] = 21 - 4 - 9 + 3 = 11.
//       - For sumRegion(1, 2, 2, 4), compute prefix_[3][5] - prefix_[1][5] -
//         prefix_[3][2] + prefix_[1][2] = 36 - 10 - 17 + 3 = 12.
//
//   Example 2:
//     Input:  matrix = [[1, -2, 3], [-4, 5, -6]], queries = [sumRegion(0, 0, 0, 0),
//             sumRegion(1, 2, 1, 2), sumRegion(0, 0, 1, 2)]
//     Output: [1, -6, -3]
//     Explanation:
//       - Construct the 3 x 4 padded prefix_ table with row 0 = [0, 0, 0, 0], row 1
//         = [0, 1, -1, 2], and row 2 = [0, -3, 2, -3].
//       - For sumRegion(0, 0, 0, 0), compute prefix_[1][1] - prefix_[0][1] -
//         prefix_[1][0] + prefix_[0][0] = 1 - 0 - 0 + 0 = 1.
//       - For sumRegion(1, 2, 1, 2), compute prefix_[2][3] - prefix_[1][3] -
//         prefix_[2][2] + prefix_[1][2] = -3 - 2 - 2 + (-1) = -6.
//       - For sumRegion(0, 0, 1, 2), compute prefix_[2][3] - prefix_[0][3] -
//         prefix_[2][0] + prefix_[0][0] = -3 - 0 - 0 + 0 = -3.
//
//   Example 3:
//     Input:  matrix = [[-4]], query = sumRegion(0, 0, 0, 0)
//     Output: -4
//     Explanation:
//       - Build the 2 x 2 padded prefix_ table [[0, 0], [0, -4]].
//       - Evaluate sumRegion(0, 0, 0, 0) as prefix_[1][1] - prefix_[0][1] -
//         prefix_[1][0] + prefix_[0][0] = -4 - 0 - 0 + 0 = -4.
//
// Algorithm:
//   Two prefix-sum approaches are included:
//   1. `NumMatrix` (Optimal 2D Integral Image / 2D Prefix Sum):
//      Precompute a padded `(m + 1) x (n + 1)` matrix `prefix_` where
//      `prefix_[r + 1][c + 1]` stores the sum of the submatrix `[0..r] x [0..c]`:
//        prefix_[r + 1][c + 1] = matrix[r][c] + prefix_[r][c + 1]
//                                + prefix_[r + 1][c] - prefix_[r][c].
//      Each query then runs in O(1) via inclusion-exclusion:
//        sum = prefix_[r2 + 1][c2 + 1] - prefix_[r1][c2 + 1]
//              - prefix_[r2 + 1][c1] + prefix_[r1][c1].
//      Build: O(m * n) time and O(m * n) space. Query: O(1) time.
//   2. `NumMatrixRowPrefix` (Row-wise Prefix Sum from the original Python):
//      Precomputes 1D prefix sums independently along each row and sums each
//      row slice `[col1..col2]` across `row1..row2`.
//      Build: O(m * n) time and O(m * n) space. Query: O(m) time.
//
// Data Structures:
//   - `std::vector<std::vector<int>>` (`prefix_`): 1-indexed padded 2D
//     cumulative-sum table eliminating boundary checks during both construction
//     and query evaluation.
//
// Real-World Applications:
//   - Computer vision summed-area tables (Viola-Jones Haar feature evaluation,
//     fast box blur, and adaptive image thresholding in O(1) per window).
//   - OLAP data cubes and geospatial heatmap analytics (instant rectangular
//     region aggregations over static gridded telemetry).
//   - Game engines and rendering (fast mipmap / shadow-map area integration).
//
// Related LeetCode Problems:
//   - #303 Range Sum Query [done] - Immutable (1D prefix sum foundation)
//   - #308 Range Sum Query 2D [done] - Mutable (2D Binary Indexed Tree / Fenwick tree for updates)
//   - #1314 Matrix Block Sum [done] (sliding 2D neighborhood sums using a 2D prefix table)
//   - #1074 Number of Submatrices That Sum to Target [done] (combining 2D prefix sums with hash maps)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

// Optimal 2D prefix-sum (summed-area table) implementation answering each
// rectangular sum query in O(1) time.
class NumMatrix {
 public:
  // Precomputes the padded `(rows + 1) x (cols + 1)` 2D prefix-sum table.
  //
  // Invariant:
  //   `prefix_[r + 1][c + 1]` equals the sum of all `matrix[i][j]` with
  //   `0 <= i <= r` and `0 <= j <= c`. Row 0 and column 0 remain all zeros so
  //   inclusion-exclusion formulas need no edge-case branches.
  //
  // Time Complexity:  O(m * n)
  // Space Complexity: O(m * n)
  explicit NumMatrix(const std::vector<std::vector<int>>& matrix) {
    if (matrix.empty() || matrix[0].empty()) {
      return;
    }

    const std::size_t rows = matrix.size();
    const std::size_t cols = matrix[0].size();
    prefix_.assign(rows + 1uz, std::vector<int>(cols + 1uz, 0));

    for (std::size_t r = 0uz; r < rows; ++r) {
      for (std::size_t c = 0uz; c < cols; ++c) {
        // Add current cell plus top and left cumulative rectangles, then
        // subtract the doubly-counted top-left overlap.
        prefix_[r + 1uz][c + 1uz] = matrix[r][c] + prefix_[r][c + 1uz] +
                                    prefix_[r + 1uz][c] - prefix_[r][c];
      }
    }
  }

  // Returns the sum of elements in `[row1..row2] x [col1..col2]` in O(1) time
  // via 2D inclusion-exclusion.
  [[nodiscard]] int sumRegion(int row1, int col1, int row2, int col2) const {
    if (prefix_.empty()) {
      return 0;
    }

    const auto r1 = static_cast<std::size_t>(row1);
    const auto c1 = static_cast<std::size_t>(col1);
    const auto r2 = static_cast<std::size_t>(row2) + 1uz;
    const auto c2 = static_cast<std::size_t>(col2) + 1uz;
    return prefix_[r2][c2] - prefix_[r1][c2] - prefix_[r2][c1] +
           prefix_[r1][c1];
  }

 private:
  std::vector<std::vector<int>> prefix_;
};

// Row-wise prefix-sum implementation matching the original Python solution:
// O(m * n) preprocessing and O(m) per query.
class NumMatrixRowPrefix {
 public:
  explicit NumMatrixRowPrefix(const std::vector<std::vector<int>>& matrix)
      : row_prefix_(matrix) {
    if (row_prefix_.empty() || row_prefix_[0].empty()) {
      return;
    }
    const std::size_t cols = row_prefix_[0].size();
    for (auto& row : row_prefix_) {
      for (std::size_t c = 1uz; c < cols; ++c) {
        row[c] += row[c - 1uz];
      }
    }
  }

  // Sums each row's slice `[col1..col2]` for `r` in `[row1..row2]`.
  [[nodiscard]] int sumRegion(int row1, int col1, int row2, int col2) const {
    if (row_prefix_.empty()) {
      return 0;
    }

    int total = 0;
    const auto r1 = static_cast<std::size_t>(row1);
    const auto r2 = static_cast<std::size_t>(row2);
    const auto c1 = static_cast<std::size_t>(col1);
    const auto c2 = static_cast<std::size_t>(col2);
    for (std::size_t r = r1; r <= r2; ++r) {
      const int left = (c1 > 0uz) ? row_prefix_[r][c1 - 1uz] : 0;
      total += (row_prefix_[r][c2] - left);
    }
    return total;
  }

 private:
  std::vector<std::vector<int>> row_prefix_;
};

LC_PROBLEM(304, "Range Sum Query 2D Immutable", "array", ctx) {
  ctx.test("leetcode_examples", [&] {
    const std::vector<std::vector<int>> matrix = {
        {3, 0, 1, 4, 2},
        {5, 6, 3, 2, 1},
        {1, 2, 0, 1, 5},
        {4, 1, 0, 1, 7},
        {1, 0, 3, 0, 5},
    };
    const NumMatrix fast(matrix);
    const NumMatrixRowPrefix row_based(matrix);

    ctx.expect_eq(fast.sumRegion(2, 1, 4, 3), 8);
    ctx.expect_eq(fast.sumRegion(1, 1, 2, 2), 11);
    ctx.expect_eq(fast.sumRegion(1, 2, 2, 4), 12);

    ctx.expect_eq(row_based.sumRegion(2, 1, 4, 3), 8);
    ctx.expect_eq(row_based.sumRegion(1, 1, 2, 2), 11);
    ctx.expect_eq(row_based.sumRegion(1, 2, 2, 4), 12);
  });

  ctx.test("boundary_cells_and_full_matrix", [&] {
    const std::vector<std::vector<int>> matrix = {
        {1, -2, 3},
        {-4, 5, -6},
    };
    const NumMatrix fast(matrix);
    const NumMatrixRowPrefix row_based(matrix);

    // Top-left single cell, bottom-right single cell, full matrix.
    ctx.expect_eq(fast.sumRegion(0, 0, 0, 0), 1);
    ctx.expect_eq(fast.sumRegion(1, 2, 1, 2), -6);
    ctx.expect_eq(fast.sumRegion(0, 0, 1, 2), -3);

    ctx.expect_eq(row_based.sumRegion(0, 0, 0, 0), 1);
    ctx.expect_eq(row_based.sumRegion(1, 2, 1, 2), -6);
    ctx.expect_eq(row_based.sumRegion(0, 0, 1, 2), -3);
  });

  ctx.test("single_element_matrix", [&] {
    const std::vector<std::vector<int>> matrix = {{-4}};
    const NumMatrix fast(matrix);
    const NumMatrixRowPrefix row_based(matrix);

    ctx.expect_eq(fast.sumRegion(0, 0, 0, 0), -4);
    ctx.expect_eq(row_based.sumRegion(0, 0, 0, 0), -4);
  });
}

}  // namespace
