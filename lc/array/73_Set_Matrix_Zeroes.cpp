// LeetCode 73: Set Matrix Zeroes
// https://leetcode.com/problems/set-matrix-zeroes/
//
// Problem:
//   Given an m x n integer matrix, if an element is 0, set its entire row and
//   entire column to 0. Perform the modification in-place.
//
// Algorithm:
//   A naive single-pass zeroing corrupts later cells because newly written
//   zeros are indistinguishable from original zeros. We provide two approaches:
//   1. Constant-space in-place markers (`setZeroes`):
//      - First record in two booleans whether row 0 (`first_row_zero`) or
//        column 0 (`first_col_zero`) originally contains a 0.
//      - Scan the interior submatrix [1..m-1] x [1..n-1]; whenever
//        `matrix[r][c] == 0`, store markers in the first cell of its row
//        (`matrix[r][0] = 0`) and the first cell of its column
//        (`matrix[0][c] = 0`).
//      - Re-scan the interior and zero `matrix[r][c]` whenever either marker
//        is 0.
//      - Finally, zero row 0 if `first_row_zero` and column 0 if
//        `first_col_zero`.
//      Time complexity: O(m * n). Space complexity: O(1) auxiliary space.
//   2. Explicit row/column marker flags (`setZeroesWithMarkers`, directly
//      mirroring the Python solution's `rows`/`cols` sets):
//      - Record zeroed rows and columns in two byte-flag vectors of lengths
//        m and n, then apply them in a second pass.
//      Time complexity: O(m * n). Space complexity: O(m + n).
//
// Data Structures:
//   - Input matrix (`std::vector<std::vector<int>>`): row 0 and column 0 serve
//     as in-place marker arrays in `setZeroes`.
//   - Two `std::vector<std::uint8_t>` flag arrays in `setZeroesWithMarkers`
//     (faster and more cache-friendly than hash sets for dense integer indices).
//
// Real-World Applications:
//   - Spreadsheet / data-frame masking and NaN/null propagation across
//     correlated rows and columns without allocating a duplicate grid.
//   - Sparse-matrix pruning and constraint propagation in linear programming
//     and Sudoku / SAT solvers.
//   - Image masking and defect-line suppression in CMOS camera sensor
//     calibration pipelines.
//
// Related LeetCode Problems:
//   - #48 Rotate Image [done] (in-place O(1)-space 2D matrix transformation)
//   - #289 Game of Life [done] (in-place state encoding in a 2D grid without extra matrix)
//   - #1252 Cells with Odd Values in a Matrix [new] (row/column parity accumulation)
//   - #2125 Number of Laser Beams in a Bank [new] (row-wise binary scanning in grids)
//   - #2661 First Completely Painted Row or Column [new] (row/column completion tracking)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Zeroes every row and column containing a 0 in-place using row 0 and col 0
  // as O(1)-extra-space marker arrays.
  //
  // Time: O(m * n), Space: O(1).
  void setZeroes(std::vector<std::vector<int>>& matrix) {
    if (matrix.empty() || matrix[0].empty()) {
      return;
    }

    const std::size_t rows = matrix.size();
    const std::size_t cols = matrix[0].size();

    // Preserve whether row 0 or column 0 originally contained any zeros before
    // repurposing them as markers for the interior submatrix.
    const bool first_row_zero =
        std::ranges::any_of(matrix[0], [](int val) { return val == 0; });
    const bool first_col_zero = std::ranges::any_of(
        matrix, [](const std::vector<int>& row) { return row[0] == 0; });

    // Phase 1: Project interior zeros onto the row-0 and col-0 marker strips.
    for (std::size_t r = 1uz; r < rows; ++r) {
      for (std::size_t c = 1uz; c < cols; ++c) {
        if (matrix[r][c] == 0) {
          matrix[r][0] = 0;
          matrix[0][c] = 0;
        }
      }
    }

    // Phase 2: Zero out interior cells whose row or column was marked.
    for (std::size_t r = 1uz; r < rows; ++r) {
      for (std::size_t c = 1uz; c < cols; ++c) {
        if (matrix[r][0] == 0 || matrix[0][c] == 0) {
          matrix[r][c] = 0;
        }
      }
    }

    // Phase 3: Apply the saved zero flags to row 0 and column 0 last so they
    // do not clobber the markers read during Phase 2.
    if (first_row_zero) {
      std::ranges::fill(matrix[0], 0);
    }
    if (first_col_zero) {
      for (auto& row : matrix) {
        row[0] = 0;
      }
    }
  }

  // Straightforward two-pass solution using separate row/column marker arrays
  // (mirroring the Python solution's `rows` and `cols` sets).
  //
  // Time: O(m * n), Space: O(m + n).
  void setZeroesWithMarkers(std::vector<std::vector<int>>& matrix) {
    if (matrix.empty() || matrix[0].empty()) {
      return;
    }

    const std::size_t rows = matrix.size();
    const std::size_t cols = matrix[0].size();
    std::vector<std::uint8_t> zero_row(rows, 0);
    std::vector<std::uint8_t> zero_col(cols, 0);

    for (std::size_t r = 0uz; r < rows; ++r) {
      for (std::size_t c = 0uz; c < cols; ++c) {
        if (matrix[r][c] == 0) {
          zero_row[r] = 1;
          zero_col[c] = 1;
        }
      }
    }

    for (std::size_t r = 0uz; r < rows; ++r) {
      for (std::size_t c = 0uz; c < cols; ++c) {
        if (zero_row[r] != 0 || zero_col[c] != 0) {
          matrix[r][c] = 0;
        }
      }
    }
  }
};

LC_PROBLEM(73, "Set Matrix Zeroes", "array", ctx) {
  Solution sol;

  ctx.test("example_1_center_zero", [&] {
    const std::vector<std::vector<int>> input{
        {1, 1, 1},
        {1, 0, 1},
        {1, 1, 1},
    };
    const std::vector<std::vector<int>> expected{
        {1, 0, 1},
        {0, 0, 0},
        {1, 0, 1},
    };

    auto m1 = input;
    sol.setZeroes(m1);
    ctx.expect_eq(m1, expected);

    auto m2 = input;
    sol.setZeroesWithMarkers(m2);
    ctx.expect_eq(m2, expected);
  });

  ctx.test("example_2_top_row_zeros", [&] {
    const std::vector<std::vector<int>> input{
        {0, 1, 2, 0},
        {3, 4, 5, 2},
        {1, 3, 1, 5},
    };
    const std::vector<std::vector<int>> expected{
        {0, 0, 0, 0},
        {0, 4, 5, 0},
        {0, 3, 1, 0},
    };

    auto m1 = input;
    sol.setZeroes(m1);
    ctx.expect_eq(m1, expected);

    auto m2 = input;
    sol.setZeroesWithMarkers(m2);
    ctx.expect_eq(m2, expected);
  });

  ctx.test("python_build_case", [&] {
    const std::vector<std::vector<int>> input{
        {1, 0, 2, 0},
        {3, 4, 5, 2},
        {1, 3, 1, 5},
    };
    const std::vector<std::vector<int>> expected{
        {0, 0, 0, 0},
        {3, 0, 5, 0},
        {1, 0, 1, 0},
    };

    auto m1 = input;
    sol.setZeroes(m1);
    ctx.expect_eq(m1, expected);

    auto m2 = input;
    sol.setZeroesWithMarkers(m2);
    ctx.expect_eq(m2, expected);
  });

  ctx.test("first_col_zero_only_and_no_zeros", [&] {
    std::vector<std::vector<int>> first_col{
        {1, 2, 3},
        {0, 5, 6},
        {7, 8, 9},
    };
    sol.setZeroes(first_col);
    ctx.expect_eq(first_col, std::vector<std::vector<int>>{
                                 {0, 2, 3}, {0, 0, 0}, {0, 8, 9}});

    std::vector<std::vector<int>> no_zeros{{1, 2}, {3, 4}};
    sol.setZeroes(no_zeros);
    ctx.expect_eq(no_zeros, std::vector<std::vector<int>>{{1, 2}, {3, 4}});
  });

  ctx.test("edge_cases_1d_and_empty", [&] {
    std::vector<std::vector<int>> single_row{{1, 0, 3}};
    sol.setZeroes(single_row);
    ctx.expect_eq(single_row, std::vector<std::vector<int>>{{0, 0, 0}});

    std::vector<std::vector<int>> single_col{{1}, {0}, {3}};
    sol.setZeroes(single_col);
    ctx.expect_eq(single_col, std::vector<std::vector<int>>{{0}, {0}, {0}});

    std::vector<std::vector<int>> empty;
    sol.setZeroes(empty);
    ctx.expect_true(empty.empty());
  });
}

}  // namespace
