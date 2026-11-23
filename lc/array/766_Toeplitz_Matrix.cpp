// LeetCode 766: Toeplitz Matrix
// https://leetcode.com/problems/toeplitz-matrix/
//
// Problem:
//   Given an `m x n` integer matrix, return `true` if the matrix is Toeplitz,
//   and `false` otherwise. A matrix is Toeplitz if every diagonal running from
//   top-left to bottom-right contains the same value throughout.
//   Constraints: 1 <= m, n <= 20; 0 <= matrix[r][c] <= 99.
//
// Examples:
//   Example 1:
//     Input:  matrix = [[1, 2, 3, 4], [5, 1, 2, 3], [9, 5, 1, 2]]
//     Output: true
//     Explanation:
//       - Determine dimensions rows = 3 and cols = 4, leaving row 0 and column 0 as
//         diagonal starts.
//       - At r = 1, verify matrix[1][1] == matrix[0][0] (1 == 1), matrix[1][2] ==
//         matrix[0][1] (2 == 2), and matrix[1][3] == matrix[0][2] (3 == 3).
//       - At r = 2, verify matrix[2][1] == matrix[1][0] (5 == 5), matrix[2][2] ==
//         matrix[1][1] (1 == 1), and matrix[2][3] == matrix[1][2] (2 == 2).
//       - All interior cells match their immediate top-left neighbors, so return
//         true.
//
//   Example 2:
//     Input:  matrix = [[1, 2], [2, 2]]
//     Output: false
//     Explanation:
//       - Determine dimensions rows = 2 and cols = 2, starting the nested check at
//         r = 1, c = 1.
//       - Compare matrix[1][1] (2) with its top-left neighbor matrix[0][0] (1).
//       - Since 2 != 1, return false immediately.
//
//   Example 3:
//     Input:  matrix = [[1, 2, 3, 4]]
//     Output: true
//     Explanation:
//       - Determine dimensions rows = 1 and cols = 4.
//       - The outer loop r from 1 while r < rows does not execute because rows ==
//         1.
//       - Every cell in the single row starts a length-1 diagonal, so return true.
//
// Algorithm:
//   Every cell `(r, c)` on a top-left-to-bottom-right diagonal shares the same
//   diagonal identifier `r - c` and has `(r - 1, c - 1)` as its immediate
//   predecessor along that diagonal whenever `r >= 1` and `c >= 1`. By
//   induction along each diagonal, all diagonals are constant if and only if
//   `matrix[r][c] == matrix[r - 1][c - 1]` for every `1 <= r < m` and
//   `1 <= c < n`. Because each row only depends on the immediately preceding
//   row, this also naturally supports streaming one row at a time from disk.
//   Time Complexity:  O(m * n), inspecting each interior cell at most once.
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - Direct 2D Matrix Indexing (`std::vector<std::vector<int>>`): compares
//     adjacent diagonal neighbors in place with zero auxiliary structures.
//
// Real-World Applications:
//   - Digital signal processing and linear systems: recognizing Toeplitz
//     covariance or convolution matrices enables O(n^2) Levinson-Durbin
//     solvers and O(n log n) FFT-based matrix-vector multiplication.
//   - Time-series stationarity checks: verifying that autocovariance matrices
//     depend only on lag `|i - j|` rather than absolute timestamps.
//   - Streaming matrix validation: checking structural invariants over huge
//     out-of-core matrices while keeping at most one previous row in memory.
//
// Related LeetCode Problems:
//   - #498 Diagonal Traverse [done] (traversing matrix diagonals using r + c / r - c invariants)
//   - #1329 Sort the Matrix Diagonally [done] (grouping and operating on top-left to bottom-right diagonals)
//   - #1424 Diagonal Traverse II [done] (anti-diagonal grouping in 2D arrays)
//   - #54 Spiral Matrix [done] (2D matrix traversal and boundary handling)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Checks whether every top-left to bottom-right diagonal of `matrix` has
  // identical elements by comparing each cell to its top-left neighbor.
  // Time: O(m * n), Space: O(1).
  bool isToeplitzMatrix(const std::vector<std::vector<int>>& matrix) {
    if (matrix.empty() || matrix[0].empty()) {
      return true;
    }

    const std::size_t rows = matrix.size();
    const std::size_t cols = matrix[0].size();

    // Row 0 and Column 0 start every diagonal; every remaining cell must match
    // its immediate predecessor one step up and one step left.
    for (std::size_t r = 1uz; r < rows; ++r) {
      for (std::size_t c = 1uz; c < cols; ++c) {
        if (matrix[r][c] != matrix[r - 1][c - 1]) {
          return false;
        }
      }
    }

    return true;
  }
};

LC_PROBLEM(766, "Toeplitz Matrix", "array", ctx) {
  Solution sol;

  ctx.test("example_1_valid_toeplitz", [&] {
    const std::vector<std::vector<int>> matrix = {
        {1, 2, 3, 4},
        {5, 1, 2, 3},
        {9, 5, 1, 2},
    };
    ctx.expect_true(sol.isToeplitzMatrix(matrix));
  });

  ctx.test("example_2_invalid_2x2", [&] {
    const std::vector<std::vector<int>> matrix = {
        {1, 2},
        {2, 2},
    };
    ctx.expect_false(sol.isToeplitzMatrix(matrix));
  });

  ctx.test("python_build_counterexamples", [&] {
    const std::vector<std::vector<int>> m1 = {
        {1, 9, 3, 4},
        {5, 1, 2, 3},
        {9, 5, 1, 2},
    };
    ctx.expect_false(sol.isToeplitzMatrix(m1));

    const std::vector<std::vector<int>> m2 = {
        {44, 35, 39}, {15, 44, 35}, {17, 15, 44},
        {80, 17, 15}, {43, 80, 0},  {77, 43, 80},
    };
    ctx.expect_false(sol.isToeplitzMatrix(m2));
  });

  ctx.test("degenerate_single_row_col_and_cell", [&] {
    ctx.expect_true(sol.isToeplitzMatrix({{1, 2, 3, 4}}));
    ctx.expect_true(sol.isToeplitzMatrix({{1}, {2}, {3}}));
    ctx.expect_true(sol.isToeplitzMatrix({{42}}));
  });
}

}  // namespace
