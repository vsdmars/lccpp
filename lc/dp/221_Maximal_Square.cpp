// LeetCode 221: Maximal Square
// https://leetcode.com/problems/maximal-square/
//
// Problem:
//   Given an m x n binary matrix filled with characters '0' and '1', find the
//   largest square containing only '1's and return its area (side_length^2).
//
// Algorithm:
//   Bottom-up dynamic programming with 1D rolling-row space optimization.
//   Let dp(r, c) be the side length of the largest all-'1' square whose
//   bottom-right corner is at cell (r, c).
//     - If matrix[r][c] == '0', no valid square ends here: dp(r, c) = 0.
//     - If matrix[r][c] == '1', a square of side L can end at (r, c) if and
//       only if squares of side at least L - 1 end immediately above, left,
//       and diagonally above-left:
//         dp(r, c) = 1 + min(dp(r - 1, c), dp(r, c - 1), dp(r - 1, c - 1)).
//   We compress the 2D table into a single 1-indexed row vector `dp` of size
//   (cols + 1) (so column 0 acts as a zero sentinel eliminating boundary checks)
//   plus one scalar `prev_diag` storing the overwritten dp(r - 1, c - 1).
//   Tracking the maximum side length `max_side` across all cells gives the
//   maximal square area `max_side * max_side`.
//   Time Complexity:  O(m * n).
//   Space Complexity: O(n), where n is the number of columns.
//
// Data Structures:
//   - 1D rolling DP vector (`std::vector<int>`) of length cols + 1 representing
//     the current/previous row of maximal square side lengths.
//   - Scalar `prev_diag` preserving the upper-left diagonal neighbor before
//     in-place overwrite.
//
// Real-World Applications:
//   - Computer vision & binary image segmentation (finding largest obstacle-free
//     or uniform square regions in occupancy grids).
//   - VLSI / PCB layout and floorplanning (locating maximal contiguous square
//     placement blocks on a silicon die).
//   - 2D spatial memory / texture-atlas allocation for square mipmap tiles.
//
// Related LeetCode Problems:
//   - #85 Maximal Rectangle [done] (generalizes squares to arbitrary rectangles via monotonic stack)
//   - #1277 Count Square Submatrices with All Ones [new] (exact same DP recurrence, summing all dp cells)
//   - #764 Largest Plus Sign [new] (4-directional grid DP for maximal cross shape)
//   - #304 Range Sum Query 2D [done] - Immutable (2D prefix sums over grids)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the area of the largest all-'1' square submatrix in `matrix`.
  //
  // Invariant: During row r at column c (0-indexed), before updating `dp[c+1]`:
  //   - `dp[c]`     holds dp(r, c - 1)     (left neighbor in current row)
  //   - `dp[c + 1]` holds dp(r - 1, c)     (top neighbor from previous row)
  //   - `prev_diag` holds dp(r - 1, c - 1) (top-left diagonal neighbor)
  //
  // Time: O(m * n), Space: O(n).
  int maximalSquare(const std::vector<std::vector<char>>& matrix) {
    if (matrix.empty() || matrix[0].empty()) {
      return 0;
    }

    const std::size_t cols = matrix[0].size();
    // 1-based padding at index 0 stays 0 so first-column cells need no guards.
    std::vector<int> dp(cols + 1uz, 0);
    int max_side = 0;

    for (const auto& row : matrix) {
      // Before column 0 of each row, the virtual top-left neighbor is 0.
      int prev_diag = 0;

      for (std::size_t c = 0uz; c < cols; ++c) {
        // Save the old top value dp(r - 1, c) before overwriting it with
        // dp(r, c); it becomes `prev_diag` for the next column c + 1.
        const int saved_top = dp[c + 1uz];

        if (row[c] == '1') {
          dp[c + 1uz] = std::min({dp[c], dp[c + 1uz], prev_diag}) + 1;
          max_side = std::max(max_side, dp[c + 1uz]);
        } else {
          // A '0' cell cannot be the bottom-right corner of any all-'1' square.
          dp[c + 1uz] = 0;
        }

        prev_diag = saved_top;
      }
    }

    return max_side * max_side;
  }
};

LC_PROBLEM(221, "Maximal Square", "dp", ctx) {
  Solution sol;

  ctx.test("leetcode_example_1_and_build", [&] {
    const std::vector<std::vector<char>> matrix = {
        {'1', '0', '1', '0', '0'},
        {'1', '0', '1', '1', '1'},
        {'1', '1', '1', '1', '1'},
        {'1', '0', '0', '1', '0'},
    };
    ctx.expect_eq(sol.maximalSquare(matrix), 4);
  });

  ctx.test("leetcode_example_2", [&] {
    const std::vector<std::vector<char>> matrix = {
        {'0', '1'},
        {'1', '0'},
    };
    ctx.expect_eq(sol.maximalSquare(matrix), 1);
  });

  ctx.test("edge_cases", [&] {
    ctx.expect_eq(sol.maximalSquare({}), 0);
    ctx.expect_eq(sol.maximalSquare({{'0'}}), 0);
    ctx.expect_eq(sol.maximalSquare({{'1'}}), 1);
    ctx.expect_eq(sol.maximalSquare({{'0', '0'}, {'0', '0'}}), 0);
  });

  ctx.test("full_square_and_broken_interior", [&] {
    const std::vector<std::vector<char>> all_ones = {
        {'1', '1', '1'},
        {'1', '1', '1'},
        {'1', '1', '1'},
    };
    ctx.expect_eq(sol.maximalSquare(all_ones), 9);

    // Center '0' breaks any 3x3 square, leaving at most 1x1 squares.
    const std::vector<std::vector<char>> hole_in_center = {
        {'1', '1', '1'},
        {'1', '0', '1'},
        {'1', '1', '1'},
    };
    ctx.expect_eq(sol.maximalSquare(hole_in_center), 1);
  });
}

}  // namespace
