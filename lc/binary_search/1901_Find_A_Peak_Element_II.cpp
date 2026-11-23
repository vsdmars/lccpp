// LeetCode 1901: Find A Peak Element II
// https://leetcode.com/problems/find-a-peak-element-ii/
//
// Problem:
//   Given a 0-indexed m x n integer matrix `mat` in which no two adjacent cells
//   (up, down, left, right) have equal values, find any 2D peak element
//   `mat[i][j]` strictly greater than all four orthogonal neighbors (treating
//   cells outside the matrix perimeter as -1) and return its coordinates [i, j]
//   in O(m log n) or O(n log m) time.
//
// Examples:
//   Example 1:
//     Input:  mat = [[1, 4], [3, 2]]
//     Output: [0, 1]
//     Explanation:
//       - Initialize row search bounds low = 0 and high = 1.
//       - At mid = 0, compare row_max(mat[0]) = 4 against row_max(mat[1]) = 3;
//         since 4 < 3 is false, set high = 0.
//       - The loop ends at low == high == 0, and std::ranges::max_element selects 4
//         at column 1, returning [0, 1].
//
//   Example 2:
//     Input:  mat = [[10, 20, 15], [21, 30, 14], [7, 16, 32]]
//     Output: [2, 2]
//     Explanation:
//       - Initialize row search bounds low = 0 and high = 2.
//       - At mid = 1, row_max(mat[1]) = 30 is strictly less than row_max(mat[2]) =
//         32, so low advances to mid + 1 = 2.
//       - The loop ends at low == high == 2, and the maximum in row 2 is 32 at
//         column 2, returning [2, 2] (strictly greater than neighbors 14 and 16).
//
//   Example 3:
//     Input:  mat = [[10, 1], [1, 10]]
//     Output: [0, 0]
//     Explanation:
//       - Initialize row search bounds low = 0 and high = 1.
//       - At mid = 0, row_max(mat[0]) = 10 equals row_max(mat[1]) = 10, so
//         row_max(mat[0]) < row_max(mat[1]) is false and high becomes 0.
//       - At row 0, the maximum element 10 sits at column 0, returning [0, 0] which
//         strictly exceeds its adjacent cells (1 below and 1 to the right).
//
// Algorithm:
//   Binary search on the row index range [low, high]. At each midpoint row
//   `mid`, compute the maximum value in row `mid` and in row `mid + 1`.
//   - If max(mat[mid]) < max(mat[mid + 1]), move `low = mid + 1`.
//   - Otherwise (max(mat[mid]) >= max(mat[mid + 1])), move `high = mid`.
//   Invariant: whenever `low` advances, row `low - 1` has strictly smaller row
//   maximum than row `low`; whenever `high` retreats, row `high` has row
//   maximum >= row `high + 1`. When the interval collapses to a single row `r`,
//   its row maximum `mat[r][c]` exceeds its left/right neighbors by maximality
//   (and distinctness of adjacent cells), strictly exceeds `mat[r - 1][c]`
//   (<= max(row r - 1) < max(row r)), and strictly exceeds `mat[r + 1][c]`
//   (<= max(row r + 1) <= max(row r), strict because adjacent cells differ).
//   Time Complexity: O(n log m), where m = rows and n = cols.
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - Two `std::size_t` bounds (`low`, `high`) tracking the active row interval.
//   - Direct read-only traversal of rows in the 2D `std::vector<std::vector<int>>`.
//
// Real-World Applications:
//   - Locating local maxima in 2D elevation/terrain DEM rasters or radar/LiDAR
//     intensity maps without scanning the entire grid.
//   - Finding local response peaks in 2D image processing, feature detection,
//     and two-parameter hyperparameter tuning surfaces.
//
// Related LeetCode Problems:
//   - #162 Find Peak Element [done] (1D binary search analogue of this problem)
//   - #852 Peak Index in a Mountain Array [done] (1D unimodal peak search)
//   - #74 Search a 2D Matrix [done] (binary search in a sorted 2D grid)
//   - #240 Search a 2D Matrix II [done] (eliminating rows/columns in a 2D grid)
//   - #1095 Find in Mountain Array [new] (peak finding plus two binary searches)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <vector>

namespace {

class Solution {
 public:
  // Finds any 2D peak cell [row, col] in `mat` in O(n log m) time and O(1)
  // auxiliary space by binary searching over rows using per-row maxima.
  std::vector<int> findPeakGrid(const std::vector<std::vector<int>>& mat) {
    if (mat.empty() || mat[0].empty()) {
      return {};
    }

    std::size_t low = 0;
    std::size_t high = mat.size() - 1;

    // Maintain the invariant that a guaranteed peak row lies within [low, high].
    while (low < high) {
      const std::size_t mid = low + ((high - low) / 2);

      // Compare the global maxima of adjacent rows `mid` and `mid + 1`.
      // Moving toward the strictly larger row maximum ensures we always ascend
      // toward a row whose maximum dominates both neighboring rows.
      if (row_max(mat[mid]) < row_max(mat[mid + 1])) {
        low = mid + 1;
      } else {
        high = mid;
      }
    }

    // Once converged to row `low`, its maximum element is strictly greater than
    // all four orthogonal neighbors.
    const auto max_it = std::ranges::max_element(mat[low]);
    const auto col = std::distance(mat[low].begin(), max_it);
    return {static_cast<int>(low), static_cast<int>(col)};
  }

 private:
  // Returns the maximum element in a non-empty row in O(n) time.
  static int row_max(const std::vector<int>& row) {
    return *std::ranges::max_element(row);
  }
};

// Verifies that `pos` is a valid in-bounds 2D peak in `mat` (strictly greater
// than every orthogonal neighbor, with out-of-bounds treated as -1).
bool is_valid_peak(const std::vector<std::vector<int>>& mat,
                   const std::vector<int>& pos) {
  if (pos.size() != 2 || mat.empty() || mat[0].empty()) {
    return false;
  }
  const int rows = static_cast<int>(mat.size());
  const int cols = static_cast<int>(mat[0].size());
  const int r = pos[0];
  const int c = pos[1];
  if (r < 0 || r >= rows || c < 0 || c >= cols) {
    return false;
  }

  const auto r_idx = static_cast<std::size_t>(r);
  const auto c_idx = static_cast<std::size_t>(c);
  const int val = mat[r_idx][c_idx];

  const int up = (r > 0) ? mat[r_idx - 1][c_idx] : -1;
  const int down = (r + 1 < rows) ? mat[r_idx + 1][c_idx] : -1;
  const int left = (c > 0) ? mat[r_idx][c_idx - 1] : -1;
  const int right = (c + 1 < cols) ? mat[r_idx][c_idx + 1] : -1;
  return val > up && val > down && val > left && val > right;
}

LC_PROBLEM(1901, "Find A Peak Element II", "binary_search", ctx) {
  Solution sol;

  ctx.test("example_1_2x2", [&] {
    const std::vector<std::vector<int>> mat = {{1, 4}, {3, 2}};
    const auto res = sol.findPeakGrid(mat);
    ctx.expect_true(is_valid_peak(mat, res));
    ctx.expect_eq(res, std::vector<int>{0, 1});
  });

  ctx.test("example_2_3x3", [&] {
    const std::vector<std::vector<int>> mat = {
        {10, 20, 15}, {21, 30, 14}, {7, 16, 32}};
    const auto res = sol.findPeakGrid(mat);
    ctx.expect_true(is_valid_peak(mat, res));
  });

  ctx.test("single_cell", [&] {
    const std::vector<std::vector<int>> mat = {{42}};
    const auto res = sol.findPeakGrid(mat);
    ctx.expect_eq(res, std::vector<int>{0, 0});
  });

  ctx.test("single_row", [&] {
    const std::vector<std::vector<int>> mat = {{1, 5, 3, 9, 2}};
    const auto res = sol.findPeakGrid(mat);
    ctx.expect_true(is_valid_peak(mat, res));
    ctx.expect_eq(res, std::vector<int>{0, 3});
  });

  ctx.test("single_column", [&] {
    const std::vector<std::vector<int>> mat = {{1}, {3}, {7}, {4}, {2}};
    const auto res = sol.findPeakGrid(mat);
    ctx.expect_true(is_valid_peak(mat, res));
    ctx.expect_eq(res, std::vector<int>{2, 0});
  });

  ctx.test("equal_row_maxima_different_columns", [&] {
    // Adjacent cells differ, but both rows share the same maximum value (10).
    const std::vector<std::vector<int>> mat = {{10, 1}, {1, 10}};
    const auto res = sol.findPeakGrid(mat);
    ctx.expect_true(is_valid_peak(mat, res));
  });

  ctx.test("monotonically_increasing_grid", [&] {
    const std::vector<std::vector<int>> mat = {
        {1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}};
    const auto res = sol.findPeakGrid(mat);
    ctx.expect_true(is_valid_peak(mat, res));
    ctx.expect_eq(res, std::vector<int>{2, 3});
  });
}

}  // namespace
