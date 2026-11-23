// LeetCode 74: Search A 2D Matrix
// https://leetcode.com/problems/search-a-2d-matrix/
//
// Problem:
//   Given an `m x n` integer matrix where each row is sorted in ascending
//   order from left to right and the first integer of each row is strictly
//   greater than the last integer of the previous row, determine whether a
//   given `target` integer exists in the matrix in O(log(m * n)) time.
//
// Examples:
//   Example 1:
//     Input:  matrix = [[1, 3, 5, 7], [10, 11, 16, 20], [23, 30, 34, 50]], target =
//             3
//     Output: true
//     Explanation:
//       - With rows = 3 and cols = 4, initialize virtual 1D bounds left = 0 and
//         right = 12.
//       - At mid = 6, map to row = 6 / 4 = 1 and col = 6 % 4 = 2 (matrix[1][2] = 16
//         > 3), so set right = 6.
//       - At mid = 3, map to row = 0 and col = 3 (matrix[0][3] = 7 > 3), so set
//         right = 3.
//       - At mid = 1, map to row = 0 and col = 1 (matrix[0][1] = 3 == target),
//         returning true.
//
//   Example 2:
//     Input:  matrix = [[1, 3, 5, 7], [10, 11, 16, 20], [23, 30, 34, 50]], target =
//             13
//     Output: false
//     Explanation:
//       - Initialize virtual 1D bounds left = 0 and right = 12 for the 3 x 4
//         matrix.
//       - At mid = 6, matrix[1][2] = 16 > 13 sets right = 6; at mid = 3,
//         matrix[0][3] = 7 < 13 sets left = 4.
//       - At mid = 5, matrix[1][1] = 11 < 13 sets left = 6.
//       - The loop terminates with left == right == 6, so return false.
//
//   Example 3:
//     Input:  matrix = [[1], [3], [5]], target = 3
//     Output: true
//     Explanation:
//       - Verify the matrix and its first row are non-empty, then set rows = 3,
//         cols = 1, left = 0, and right = 3.
//       - Compute mid = 1 and map it to row = 1 / 1 = 1 and col = 1 % 1 = 0.
//       - Compare matrix[1][0] = 3 with target = 3 and return true.
//
// Algorithm:
//   Because the row-major ordering of the matrix is strictly sorted globally,
//   treat the `m x n` grid as a virtual 1D sorted array of length `m * n`
//   without allocating any flattened buffer. Run a standard half-open binary
//   search over `[0, m * n)`, mapping each flat index `mid` back to 2D
//   coordinates via `row = mid / cols` and `col = mid % cols`.
//   Time Complexity: O(log(m * n)).
//   Space Complexity: O(1).
//
// Data Structures:
//   Read-only 2D `std::vector<std::vector<int>>` indexed through virtual 1D
//   `std::size_t` bounds (`left`, `right`).
//
// Real-World Applications:
//   Querying row-major paged/chunked sorted tables (such as fixed-size B-tree
//   pages or memory-mapped sorted blocks) without copying pages into a single
//   contiguous array, and binary search over 2D lookup tables in embedded
//   control systems.
//
// Related LeetCode Problems:
//   - #240 Search a 2D Matrix II [done] (rows and columns sorted independently; O(m + n) staircase search)
//   - #35 Search Insert Position [done] (1D binary search foundation)
//   - #378 Kth Smallest Element in a Sorted Matrix [done] (value-space binary search on sorted matrices)
//   - #704 Binary Search [done] (standard 1D binary search)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Searches a globally row-major sorted 2D matrix for `target` in O(log(m*n)).
  //
  // Invariant: If `target` exists in `matrix`, its flat row-major index
  // `(r * cols) + c` lies in `[left, right)`.
  //
  // Time: O(log(m * n)), Space: O(1).
  bool searchMatrix(const std::vector<std::vector<int>>& matrix, int target) {
    // Guard against both an empty matrix and rows with zero columns so
    // `mid / cols` and `mid % cols` never divide by zero.
    if (matrix.empty() || matrix.front().empty()) {
      return false;
    }

    const std::size_t rows = matrix.size();
    const std::size_t cols = matrix.front().size();

    std::size_t left = 0uz;
    std::size_t right = rows * cols;

    while (left < right) {
      const std::size_t mid = left + ((right - left) / 2);
      // Map the virtual 1D index back to its 2D `(row, col)` cell.
      const std::size_t row = mid / cols;
      const std::size_t col = mid % cols;
      const int val = matrix[row][col];

      if (target < val) {
        right = mid;
      } else if (target > val) {
        left = mid + 1;
      } else {
        return true;
      }
    }

    return false;
  }
};

LC_PROBLEM(74, "Search A 2D Matrix", "binary_search", ctx) {
  Solution sol;

  ctx.test("examples_found_and_missing", [&] {
    const std::vector<std::vector<int>> matrix = {
        {1, 3, 5, 7},
        {10, 11, 16, 20},
        {23, 30, 34, 50},
    };
    ctx.expect_true(sol.searchMatrix(matrix, 3));
    ctx.expect_false(sol.searchMatrix(matrix, 13));
    ctx.expect_false(sol.searchMatrix(matrix, 51));
    ctx.expect_false(sol.searchMatrix(matrix, 0));
  });

  ctx.test("row_boundaries_and_corners", [&] {
    const std::vector<std::vector<int>> matrix = {
        {1, 3, 5, 7},
        {10, 11, 16, 20},
        {23, 30, 34, 50},
    };
    ctx.expect_true(sol.searchMatrix(matrix, 1));
    ctx.expect_true(sol.searchMatrix(matrix, 7));
    ctx.expect_true(sol.searchMatrix(matrix, 10));
    ctx.expect_true(sol.searchMatrix(matrix, 20));
    ctx.expect_true(sol.searchMatrix(matrix, 23));
    ctx.expect_true(sol.searchMatrix(matrix, 50));
  });

  ctx.test("edge_cases_empty_and_single_cell", [&] {
    ctx.expect_false(sol.searchMatrix({}, 10));
    ctx.expect_false(sol.searchMatrix({{}}, 10));
    ctx.expect_true(sol.searchMatrix({{5}}, 5));
    ctx.expect_false(sol.searchMatrix({{5}}, 4));
    ctx.expect_true(sol.searchMatrix({{1}, {3}, {5}}, 3));
    ctx.expect_false(sol.searchMatrix({{1}, {3}, {5}}, 2));
  });
}

}  // namespace
