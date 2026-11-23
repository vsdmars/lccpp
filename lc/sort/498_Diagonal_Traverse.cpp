// LeetCode 498: Diagonal Traverse
// https://leetcode.com/problems/diagonal-traverse/
//
// Problem:
//   Given an `m x n` integer matrix `mat`, return all elements of the matrix
//   visited in zig-zag anti-diagonal order starting from `(0, 0)` moving
//   up-right, then alternating down-left and up-right across consecutive
//   anti-diagonals until reaching `(m - 1, n - 1)`.
//   Constraints: `1 <= m, n <= 10^4`, `1 <= m * n <= 10^4`.
//
// Examples:
//   Example 1:
//     Input:  mat = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
//     Output: [1, 2, 4, 7, 5, 3, 6, 8, 9]
//     Explanation:
//       - Group elements by anti-diagonal key d = r + c in row-major order,
//         yielding buckets d=0: [1], d=1: [2, 4], d=2: [3, 5, 7], d=3: [6, 8], d=4:
//         [9].
//       - Emit even diagonals in reverse (bottom-left to top-right) and odd
//         diagonals as-is: [1], [2, 4], [7, 5, 3], [6, 8], [9].
//       - Concatenate all emitted diagonals in order to produce [1, 2, 4, 7, 5, 3,
//         6, 8, 9].
//
//   Example 2:
//     Input:  mat = [[1, 2, 3, 10], [4, 5, 6, 11], [7, 8, 9, 12]]
//     Output: [1, 2, 4, 7, 5, 3, 10, 6, 8, 9, 11, 12]
//     Explanation:
//       - Bucket the 3x4 matrix by d = r + c for d in 0..5: [1], [2, 4], [3, 5, 7],
//         [10, 6, 8], [11, 9], and [12].
//       - Reverse even buckets (d = 0, 2, 4) to get [1], [7, 5, 3], and [9, 11],
//         while keeping odd buckets (d = 1, 3, 5) as [2, 4], [10, 6, 8], and [12].
//       - Append buckets d = 0 through 5 into the result vector to return [1, 2, 4,
//         7, 5, 3, 10, 6, 8, 9, 11, 12].
//
//   Example 3:
//     Input:  mat = [[1], [2], [3]]
//     Output: [1, 2, 3]
//     Explanation:
//       - Bucket the 3x1 matrix by d = r + c: d=0 holds [1], d=1 holds [2], and d=2
//         holds [3].
//       - Since each bucket contains a single element, reversing even diagonals (d
//         = 0, 2) leaves them unchanged, producing [1, 2, 3].
//
// Algorithm:
//   1. Anti-Diagonal Sum Grouping (`findDiagonalOrder`):
//      Every cell `(r, c)` on the same anti-diagonal shares the invariant
//      `d = r + c` (`0 <= d <= m + n - 2`). Scanning row-by-row appends cells
//      to bucket `d` in top-right to bottom-left order. Even-indexed diagonals
//      (`d % 2 == 0`) are emitted in reverse (bottom-left to top-right); odd
//      diagonals (`d % 2 == 1`) are emitted as-is.
//      Time Complexity:  O(m * n) — each cell is bucketed and copied once.
//      Space Complexity: O(m * n) for the diagonal buckets (plus output).
//   2. Direct Boundary Simulation (`findDiagonalOrderSimulation`):
//      Tracks current coordinates `(r, c)` and direction (`going_up_right`),
//      stepping `(-1, +1)` or `(+1, -1)` and bouncing cleanly at matrix edges
//      (checking right/bottom bounds before top/left bounds so corners turn
//      properly).
//      Time Complexity:  O(m * n).
//      Space Complexity: O(1) auxiliary space (excluding output vector).
//
// Data Structures:
//   - Bucket table (`std::vector<std::vector<int>>`): indexed by `r + c` to
//     group anti-diagonals without hash-map overhead.
//   - Result array (`std::vector<int>`): pre-reserved to `m * n` elements.
//
// Real-World Applications:
//   - JPEG / MPEG entropy coding (zig-zag scan of 8x8 DCT frequency blocks to
//     cluster high-frequency zeros for run-length encoding).
//   - Wavefront / anti-diagonal parallelization of 2D dynamic programming
//     tables (e.g., Smith-Waterman sequence alignment) where cells with equal
//     `r + c` have no mutual data dependencies.
//   - Raster-to-diagonal memory layout conversions in image processing and
//     sparse banded matrix storage.
//
// Related LeetCode Problems:
//   - #1329 Sort the Matrix Diagonally [done] (grouping main diagonals by `r - c`)
//   - #1424 Diagonal Traverse II [done] (anti-diagonal traversal on ragged 2D arrays)
//   - #54 Spiral Matrix [done] (boundary-driven 2D matrix traversal)
//   - #59 Spiral Matrix II [done] (directional matrix filling)
//   - #885 Spiral Matrix III [done] (step-based directional grid walk)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Groups matrix elements by anti-diagonal index `r + c`, then emits even
  // diagonals bottom-left to top-right and odd diagonals top-right to
  // bottom-left.
  //
  // Time: O(m * n), Space: O(m * n).
  std::vector<int> findDiagonalOrder(const std::vector<std::vector<int>>& mat) {
    if (mat.empty() || mat[0].empty()) {
      return {};
    }

    const std::size_t rows = mat.size();
    const std::size_t cols = mat[0].size();
    const std::size_t num_diags = (rows + cols) - 1;

    // Phase 1: Bucket every cell by its anti-diagonal key `r + c`.
    // Row-major traversal naturally orders each bucket from top-right to
    // bottom-left (increasing row index).
    std::vector<std::vector<int>> diagonals(num_diags);
    for (std::size_t r = 0; r < rows; ++r) {
      for (std::size_t c = 0; c < cols; ++c) {
        diagonals[r + c].push_back(mat[r][c]);
      }
    }

    // Phase 2: Concatenate buckets, reversing even-numbered diagonals to
    // achieve the alternating up-right / down-left zig-zag order.
    std::vector<int> result;
    result.reserve(rows * cols);
    for (std::size_t d = 0; d < num_diags; ++d) {
      const auto& diag = diagonals[d];
      if ((d % 2) != 0) {
        result.insert(result.end(), diag.begin(), diag.end());
      } else {
        result.insert(result.end(), diag.rbegin(), diag.rend());
      }
    }

    return result;
  }

  // Simulates the zig-zag walk directly using O(1) auxiliary space.
  //
  // Time: O(m * n), Space: O(1) extra (excluding output).
  std::vector<int> findDiagonalOrderSimulation(
      const std::vector<std::vector<int>>& mat) {
    if (mat.empty() || mat[0].empty()) {
      return {};
    }

    const std::size_t rows = mat.size();
    const std::size_t cols = mat[0].size();
    const std::size_t total = rows * cols;

    std::vector<int> result;
    result.reserve(total);

    std::size_t r = 0;
    std::size_t c = 0;
    bool going_up_right = true;

    for (std::size_t step = 0; step < total; ++step) {
      result.push_back(mat[r][c]);

      if (going_up_right) {
        // Check right edge before top edge so the top-right corner `(0, cols-1)`
        // steps downward rather than out of bounds to the right.
        if (c + 1 == cols) {
          ++r;
          going_up_right = false;
        } else if (r == 0) {
          ++c;
          going_up_right = false;
        } else {
          --r;
          ++c;
        }
      } else {
        // Check bottom edge before left edge so the bottom-left corner
        // `(rows-1, 0)` steps rightward rather than out of bounds downward.
        if (r + 1 == rows) {
          ++c;
          going_up_right = true;
        } else if (c == 0) {
          ++r;
          going_up_right = true;
        } else {
          ++r;
          --c;
        }
      }
    }

    return result;
  }
};

LC_PROBLEM(498, "Diagonal Traverse", "sort", ctx) {
  Solution sol;

  auto expect_both = [&](const std::vector<std::vector<int>>& mat,
                         const std::vector<int>& expected) {
    ctx.expect_eq(sol.findDiagonalOrder(mat), expected);
    ctx.expect_eq(sol.findDiagonalOrderSimulation(mat), expected);
  };

  ctx.test("square_3x3", [&] {
    const std::vector<std::vector<int>> mat = {
        {1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    const std::vector<int> expected = {1, 2, 4, 7, 5, 3, 6, 8, 9};
    expect_both(mat, expected);
  });

  ctx.test("rectangular_3x4", [&] {
    const std::vector<std::vector<int>> mat = {
        {1, 2, 3, 10}, {4, 5, 6, 11}, {7, 8, 9, 12}};
    const std::vector<int> expected = {1, 2, 4, 7, 5, 3, 10, 6, 8, 9, 11, 12};
    expect_both(mat, expected);
  });

  ctx.test("single_row", [&] {
    const std::vector<std::vector<int>> mat = {{1, 2, 3, 10}};
    const std::vector<int> expected = {1, 2, 3, 10};
    expect_both(mat, expected);
  });

  ctx.test("single_column", [&] {
    const std::vector<std::vector<int>> mat = {{1}, {2}, {3}};
    const std::vector<int> expected = {1, 2, 3};
    expect_both(mat, expected);
  });

  ctx.test("single_element", [&] {
    const std::vector<std::vector<int>> mat = {{42}};
    const std::vector<int> expected = {42};
    expect_both(mat, expected);
  });

  ctx.test("empty_matrix", [&] {
    const std::vector<std::vector<int>> mat = {};
    const std::vector<int> expected = {};
    expect_both(mat, expected);
  });
}

}  // namespace
