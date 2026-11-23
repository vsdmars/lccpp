// LeetCode 240: Search A 2D Matrix II
// https://leetcode.com/problems/search-a-2d-matrix-ii/
//
// Problem:
//   Given an m x n integer matrix and an integer target, determine whether
//   target exists in the matrix. Each row is sorted in ascending order from
//   left to right, and each column is sorted in ascending order from top to
//   bottom (note that the first element of a row is not necessarily larger
//   than the last element of the previous row).
//
// Examples:
//   Example 1:
//     Input:  matrix = [[1, 4, 7, 11, 15], [2, 5, 8, 12, 19], [3, 6, 9, 16, 22],
//             [10, 13, 14, 17, 24], [18, 21, 23, 26, 30]], target = 5
//     Output: true
//     Explanation:
//       - Start at the top-right corner (row = 0, col = 4), where matrix[0][4] =
//         15.
//       - Since 15 > 5, eliminate column 4 (--col to 3), then eliminate column 3
//         (matrix[0][3] = 11 > 5) and column 2 (matrix[0][2] = 7 > 5) to reach (0,
//         1).
//       - At (0, 1), matrix[0][1] = 4 < 5, so eliminate row 0 by incrementing row
//         to 1.
//       - At (1, 1), matrix[1][1] = 5 matches target, so return true.
//
//   Example 2:
//     Input:  matrix = [[1, 4, 7, 11, 15], [2, 5, 8, 12, 19], [3, 6, 9, 16, 22],
//             [10, 13, 14, 17, 24], [18, 21, 23, 26, 30]], target = 20
//     Output: false
//     Explanation:
//       - Start at (0, 4) with 15 < 20 and step down rows (++row) through (1, 4) =
//         19 < 20 to (2, 4) = 22.
//       - Since 22 > 20, step left to (2, 3) = 16 < 20, then step down through (3,
//         3) = 17 < 20 to (4, 3) = 26.
//       - At row 4, 26 > 20, 23 > 20, and 21 > 20 eliminate columns 3, 2, and 1 via
//         --col until reaching (4, 0) = 18.
//       - Since 18 < 20, increment row to 5, exiting the while (row < rows) loop
//         and returning false.
//
//   Example 3:
//     Input:  matrix = [[1, 3, 5, 7, 9]], target = 6
//     Output: false
//     Explanation:
//       - Initialize rows = 1, cols = 5, and start at the top-right corner (row =
//         0, col = 4) where matrix[0][4] = 9.
//       - Since 9 > 6 and 7 > 6, decrement col twice to reach (0, 2) where
//         matrix[0][2] = 5.
//       - Because 5 < 6, increment row to 1, which equals rows and terminates the
//         loop returning false.
//
// Algorithm:
//   Two approaches are provided:
//   1. Staircase Search (optimal, O(m + n) time, O(1) space):
//      Start at the top-right corner (row = 0, col = n - 1). At any cell
//      (row, col), every element to the left in the same row is smaller and
//      every element below in the same column is larger:
//        - If matrix[row][col] == target, we found it.
//        - If matrix[row][col] > target, the entire remaining column below is
//          also > target, so we eliminate this column (--col).
//        - If matrix[row][col] < target, the entire remaining row to the left
//          is also < target, so we eliminate this row (++row).
//      Each step discards either one full row or one full column, so at most
//      m + n steps occur.
//   2. Row-wise Binary Search (O(m log n) time, O(1) space):
//      Mirrors the original Python solution: skip any row whose value range
//      [row.front(), row.back()] cannot contain target, and run binary search
//      on candidate rows.
//
// Data Structures:
//   Only O(1) scalar index cursors (row, col) operating directly on the input
//   2D std::vector<std::vector<int>> without extra allocation.
//
// Real-World Applications:
//   - Querying 2D range-partitioned tables and Young tableaux in databases.
//   - Saddleback search in image intensity / monotone cost grids and dynamic
//     programming lookup tables with monotonic rows and columns.
//   - Locating threshold crossings in sorted parameter-sweep matrices.
//
// Related LeetCode Problems:
//   - #74 Search a 2D Matrix [done] (strictly sorted rows allow a single O(log(m*n)) binary search)
//   - #378 Kth Smallest Element in a Sorted Matrix [done] (value-space binary search using staircase counting)
//   - #668 Kth Smallest Number in Multiplication Table [new] (staircase counting on an implicit sorted grid)
//   - #1351 Count Negative Numbers in a Sorted Matrix [done] (same O(m + n) staircase elimination)
//   - #1428 Leftmost Column with at Least a One [done] (top-right staircase walk on a binary row-sorted matrix)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Searches a row- and column-sorted matrix using top-right staircase
  // elimination.
  //
  // Invariant: target cannot reside in any row < row or any column > col.
  // Time: O(m + n), Space: O(1).
  bool searchMatrix(const std::vector<std::vector<int>>& matrix, int target) {
    if (matrix.empty() || matrix.front().empty()) {
      return false;
    }

    const std::size_t rows = matrix.size();
    const std::size_t cols = matrix.front().size();

    // Start at the top-right corner where moving left decreases values and
    // moving down increases values, giving a decisive elimination at each step.
    std::size_t row = 0uz;
    std::size_t col = cols - 1uz;

    while (row < rows) {
      const int current = matrix[row][col];
      if (current == target) {
        return true;
      }
      if (current > target) {
        // Every element below in this column is >= current > target, so the
        // entire column can be discarded. Guard against unsigned underflow.
        if (col == 0uz) {
          return false;
        }
        --col;
      } else {
        // Every element to the left in this row is <= current < target, so the
        // entire row can be discarded.
        ++row;
      }
    }

    return false;
  }

  // Row-wise binary search preserving the original Python solution's approach.
  // Skips rows whose [front, back] bounds exclude target.
  // Time: O(m log n), Space: O(1).
  bool searchMatrixBinarySearch(const std::vector<std::vector<int>>& matrix,
                                int target) {
    return std::ranges::any_of(matrix, [target](const std::vector<int>& row) {
      // Prune empty rows and rows whose endpoints cannot bracket target.
      return !row.empty() && row.front() <= target && row.back() >= target &&
             std::ranges::binary_search(row, target);
    });
  }
};

LC_PROBLEM(240, "Search A 2D Matrix II", "binary_search", ctx) {
  const std::vector<std::vector<int>> kSampleMatrix = {
      {1, 4, 7, 11, 15},
      {2, 5, 8, 12, 19},
      {3, 6, 9, 16, 22},
      {10, 13, 14, 17, 24},
      {18, 21, 23, 26, 30},
  };

  ctx.test("docstring_examples_and_build", [&] {
    Solution sol;
    ctx.expect_true(sol.searchMatrix(kSampleMatrix, 5));
    ctx.expect_false(sol.searchMatrix(kSampleMatrix, 20));
    ctx.expect_true(sol.searchMatrix(kSampleMatrix, 26));

    ctx.expect_true(sol.searchMatrixBinarySearch(kSampleMatrix, 5));
    ctx.expect_false(sol.searchMatrixBinarySearch(kSampleMatrix, 20));
    ctx.expect_true(sol.searchMatrixBinarySearch(kSampleMatrix, 26));
  });

  ctx.test("corners_and_out_of_range", [&] {
    Solution sol;
    for (const int corner : {1, 15, 18, 30}) {
      ctx.expect_true(sol.searchMatrix(kSampleMatrix, corner));
      ctx.expect_true(sol.searchMatrixBinarySearch(kSampleMatrix, corner));
    }
    for (const int missing : {0, 31, -5, 100}) {
      ctx.expect_false(sol.searchMatrix(kSampleMatrix, missing));
      ctx.expect_false(sol.searchMatrixBinarySearch(kSampleMatrix, missing));
    }
  });

  ctx.test("empty_and_single_element", [&] {
    Solution sol;
    const std::vector<std::vector<int>> empty_matrix;
    const std::vector<std::vector<int>> empty_row = {{}};
    const std::vector<std::vector<int>> single = {{-5}};

    ctx.expect_false(sol.searchMatrix(empty_matrix, 1));
    ctx.expect_false(sol.searchMatrix(empty_row, 20));
    ctx.expect_true(sol.searchMatrix(single, -5));
    ctx.expect_false(sol.searchMatrix(single, 5));

    ctx.expect_false(sol.searchMatrixBinarySearch(empty_row, 20));
    ctx.expect_true(sol.searchMatrixBinarySearch(single, -5));
  });

  ctx.test("rectangular_matrices", [&] {
    Solution sol;
    const std::vector<std::vector<int>> single_row = {{1, 3, 5, 7, 9}};
    const std::vector<std::vector<int>> single_col = {{1}, {3}, {5}, {7}, {9}};

    ctx.expect_true(sol.searchMatrix(single_row, 7));
    ctx.expect_false(sol.searchMatrix(single_row, 6));
    ctx.expect_true(sol.searchMatrix(single_col, 9));
    ctx.expect_false(sol.searchMatrix(single_col, 0));
  });
}

}  // namespace
