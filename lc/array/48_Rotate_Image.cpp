// LeetCode 48: Rotate Image
// https://leetcode.com/problems/rotate-image/
//
// Problem:
//   Given an n x n 2D integer matrix representing an image, rotate the image
//   by 90 degrees clockwise in-place without allocating a second 2D matrix.
//
// Examples:
//   Example 1:
//     Input:  matrix = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
//     Output: [[7, 4, 1], [8, 5, 2], [9, 6, 3]]
//     Explanation:
//       - Compute quadrant bounds for `n = 3`: `half_rows = 1` and `half_cols = 2`,
//         so only `i = 0` (`opp_i = 2`) with `j = 0, 1` is visited.
//       - For `(i=0, j=0)` (`opp_j = 2`), perform a 4-way cyclic swap of the
//         corners `matrix[0][0]=1`, `matrix[2][0]=7`, `matrix[2][2]=9`, and
//         `matrix[0][2]=3` to place `7`, `9`, `3`, and `1` at `(0,0)`, `(2,0)`,
//         `(2,2)`, and `(0,2)`.
//       - For `(i=0, j=1)` (`opp_j = 1`), perform a 4-way cyclic swap of the edge
//         midpoints `matrix[0][1]=2`, `matrix[1][0]=4`, `matrix[2][1]=8`, and
//         `matrix[1][2]=6` to place `4`, `8`, `6`, and `2` at `(0,1)`, `(1,0)`,
//         `(2,1)`, and `(1,2)`.
//       - Leave the center cell `(1, 1)` (`5`) untouched, resulting in `[[7, 4, 1],
//         [8, 5, 2], [9, 6, 3]]`.
//
//   Example 2:
//     Input:  matrix = [[5, 1, 9, 11], [2, 4, 8, 10], [13, 3, 6, 7], [15, 14, 12,
//             16]]
//     Output: [[15, 13, 2, 5], [14, 3, 4, 1], [12, 6, 8, 9], [16, 7, 10, 11]]
//     Explanation:
//       - Compute quadrant bounds for `n = 4`: `half_rows = 2` and `half_cols = 2`.
//       - At `(0, 0)`, cycle outer corners `(0,0)=5`, `(3,0)=15`, `(3,3)=16`, and
//         `(0,3)=11` into `15`, `16`, `11`, and `5`; at `(0, 1)`, cycle `(0,1)=1`,
//         `(2,0)=13`, `(3,2)=12`, and `(1,3)=10` into `13`, `12`, `10`, and `1`.
//       - At `(1, 0)`, cycle `(1,0)=2`, `(3,1)=14`, `(2,3)=7`, and `(0,2)=9` into
//         `14`, `7`, `9`, and `2`.
//       - At `(1, 1)`, cycle the inner 2x2 ring `(1,1)=4`, `(2,1)=3`, `(2,2)=6`,
//         and `(1,2)=8` into `3`, `6`, `8`, and `4`, yielding the rotated 4x4
//         matrix.
//
//   Example 3:
//     Input:  matrix = [[42]]
//     Output: [[42]]
//     Explanation:
//       - Inspect `n = matrix.size()`, which is `1`.
//       - Because `n <= 1`, `rotate` returns immediately without modifying any
//         elements, leaving `[[42]]` unchanged.
//
// Algorithm:
//   A 90-degree clockwise rotation maps cell (r, c) -> (c, n - 1 - r). We
//   provide two classic O(1)-space in-place realizations:
//   1. Four-way cyclic ring rotation (`rotate`): partition the matrix into
//      four non-overlapping quadrants (handling odd n by giving the top-left
//      quadrant floor(n/2) rows and ceil(n/2) columns so the center cell stays
//      untouched). Each cell (i, j) participates in a 4-cycle:
//        (i, j) -> (j, n-1-i) -> (n-1-i, n-1-j) -> (n-1-j, i) -> (i, j)
//      which we rotate in-place using a single temporary scalar.
//   2. Reverse + transpose (`rotateTranspose`): reversing the row order flips
//      the matrix vertically ((r, c) -> (n-1-r, c)), and transposing across
//      the main diagonal swaps coordinates ((n-1-r, c) -> (c, n-1-r)),
//      yielding the exact 90-degree clockwise rotation (in-place C++
//      equivalent of Python's `zip(*matrix[::-1])`).
//   Time complexity: O(n^2) — every cell is moved once (or twice for
//     reverse+transpose).
//   Space complexity: O(1) auxiliary space.
//
// Data Structures:
//   - 2D matrix (`std::vector<std::vector<int>>`): mutated directly in-place
//     with only constant scalar temporaries.
//
// Real-World Applications:
//   - Image processing and camera EXIF orientation correction (rotating pixel
//     buffers 90/180/270 degrees on mobile and embedded GPUs without doubling
//     frame-buffer memory).
//   - Display/framebuffer drivers adapting landscape buffers to portrait OLED
//     panels in-place.
//   - Matrix-layout transformations in scientific computing and game engines
//     (e.g., rotating tile maps or Tetris-style tetromino grids).
//
// Related LeetCode Problems:
//   - #54 Spiral Matrix [done] (layer-by-layer 2D matrix traversal)
//   - #59 Spiral Matrix II [done] (filling concentric matrix rings)
//   - #73 Set Matrix Zeroes [done] (in-place O(1)-space 2D matrix transformation)
//   - #867 Transpose Matrix [new] (half of the reverse-then-transpose rotation)
//   - #1886 Determine Whether Matrix Can Be Obtained By Rotation [new] (applies 90-degree rotation up to 4 times)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Rotates an n x n matrix 90 degrees clockwise in-place via 4-way cyclic
  // swaps over the top-left quadrant.
  //
  // For odd n, rows span [0, floor(n/2)) and columns span [0, ceil(n/2)),
  // ensuring every non-center cell belongs to exactly one 4-cycle and the
  // single center cell (n/2, n/2) is left untouched.
  //
  // Time: O(n^2), Space: O(1).
  void rotate(std::vector<std::vector<int>>& matrix) {
    const std::size_t n = matrix.size();
    if (n <= 1uz) {
      return;
    }

    const std::size_t half_rows = n / 2uz;
    const std::size_t half_cols = n - half_rows;  // ceil(n / 2)

    for (std::size_t i = 0uz; i < half_rows; ++i) {
      const std::size_t opp_i = (n - 1uz) - i;
      for (std::size_t j = 0uz; j < half_cols; ++j) {
        const std::size_t opp_j = (n - 1uz) - j;

        // Save top-left and shift each corner 90 degrees clockwise into the
        // next position around the 4-cycle:
        //   bottom-left -> top-left
        //   bottom-right -> bottom-left
        //   top-right -> bottom-right
        //   saved top-left -> top-right
        const int top_left = matrix[i][j];
        matrix[i][j] = matrix[opp_j][i];
        matrix[opp_j][i] = matrix[opp_i][opp_j];
        matrix[opp_i][opp_j] = matrix[j][opp_i];
        matrix[j][opp_i] = top_left;
      }
    }
  }

  // Alternative in-place 90-degree clockwise rotation: flip vertically
  // (reverse row order), then transpose across the main diagonal.
  //
  // Time: O(n^2), Space: O(1).
  void rotateTranspose(std::vector<std::vector<int>>& matrix) {
    const std::size_t n = matrix.size();
    if (n <= 1uz) {
      return;
    }

    // Step 1: Vertical reflection maps (r, c) -> (n - 1 - r, c).
    std::ranges::reverse(matrix);

    // Step 2: Transpose across the main diagonal maps (n - 1 - r, c) ->
    // (c, n - 1 - r), completing the 90-degree clockwise rotation.
    for (std::size_t r = 0uz; r < n; ++r) {
      for (std::size_t c = r + 1uz; c < n; ++c) {
        std::swap(matrix[r][c], matrix[c][r]);
      }
    }
  }
};

LC_PROBLEM(48, "Rotate Image", "array", ctx) {
  Solution sol;

  ctx.test("example_1_3x3", [&] {
    const std::vector<std::vector<int>> expected{
        {7, 4, 1},
        {8, 5, 2},
        {9, 6, 3},
    };

    std::vector<std::vector<int>> m1{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    sol.rotate(m1);
    ctx.expect_eq(m1, expected);

    std::vector<std::vector<int>> m2{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    sol.rotateTranspose(m2);
    ctx.expect_eq(m2, expected);
  });

  ctx.test("example_2_4x4", [&] {
    const std::vector<std::vector<int>> input{
        {5, 1, 9, 11},
        {2, 4, 8, 10},
        {13, 3, 6, 7},
        {15, 14, 12, 16},
    };
    const std::vector<std::vector<int>> expected{
        {15, 13, 2, 5},
        {14, 3, 4, 1},
        {12, 6, 8, 9},
        {16, 7, 10, 11},
    };

    auto m1 = input;
    sol.rotate(m1);
    ctx.expect_eq(m1, expected);

    auto m2 = input;
    sol.rotateTranspose(m2);
    ctx.expect_eq(m2, expected);
  });

  ctx.test("edge_cases_empty_1x1_2x2", [&] {
    std::vector<std::vector<int>> empty;
    sol.rotate(empty);
    ctx.expect_true(empty.empty());

    std::vector<std::vector<int>> one{{42}};
    sol.rotate(one);
    ctx.expect_eq(one, std::vector<std::vector<int>>{{42}});

    std::vector<std::vector<int>> two{{1, 2}, {3, 4}};
    sol.rotate(two);
    ctx.expect_eq(two, std::vector<std::vector<int>>{{3, 1}, {4, 2}});
  });

  ctx.test("four_rotations_restore_original", [&] {
    const std::vector<std::vector<int>> original{
        {1, 2, 3, 4, 5},       {6, 7, 8, 9, 10},      {11, 12, 13, 14, 15},
        {16, 17, 18, 19, 20},  {21, 22, 23, 24, 25},
    };
    auto m = original;
    for (int k = 0; k < 4; ++k) {
      sol.rotate(m);
    }
    ctx.expect_eq(m, original);
  });
}

}  // namespace
