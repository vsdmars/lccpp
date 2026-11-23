// LeetCode 59: Spiral Matrix II
// https://leetcode.com/problems/spiral-matrix-ii/
//
// Problem:
//   Given a positive integer `n`, generate an `n x n` square matrix filled
//   with the integers `1` through `n^2` in clockwise spiral order starting at
//   the top-left cell `(0, 0)`.
//
// Algorithm:
//   Initialize an `n x n` matrix with `0` (serving as an unvisited sentinel)
//   and simulate a clockwise walk starting at `(0, 0)` with direction vector
//   `(dr, dc) = (0, 1)` (moving right). For each value `val` from `1` to `n^2`,
//   write `val` at `(row, col)` and probe `(row + dr, col + dc)`. Whenever the
//   next cell lies outside `[0, n) x [0, n)` or has already been filled
//   (non-zero), rotate the direction vector 90 degrees clockwise via the
//   2D rotation matrix `(dr, dc) -> (dc, -dr)` before advancing `(row, col)`.
//   Time Complexity: O(n^2), filling each of the n^2 cells once.
//   Space Complexity: O(1) auxiliary space (excluding the n x n output matrix).
//
// Data Structures:
//   - `std::vector<std::vector<int>>`: the `n x n` output grid, whose `0`
//     entries double as the unvisited-cell marker.
//   - Scalar coordinates `(row, col)` and direction vector `(dr, dc)`.
//
// Real-World Applications:
//   - Procedural grid and level generation in games and simulations.
//   - Spiral memory-layout / space-filling curve indexing and 2D matrix
//     visualization benchmarks.
//   - Automated CNC/3D-printer concentric infill path generation.
//
// Related LeetCode Problems:
//   - #54 Spiral Matrix [done] (reads an m x n matrix in clockwise spiral order)
//   - #885 Spiral Matrix III [done] (outward spiral coordinate generation)
//   - #2326 Spiral Matrix IV [new] (fills an m x n matrix in spiral order from a list)
//   - #48 Rotate Image [done] (90-degree clockwise coordinate rotation on an n x n matrix)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Generates an `n x n` matrix populated with `1 .. n^2` in clockwise spiral
  // order by rotating the direction vector `(dr, dc) -> (dc, -dr)` whenever
  // the next step hits a boundary or an already-filled cell.
  // Time: O(n^2), Space: O(1) auxiliary.
  std::vector<std::vector<int>> generateMatrix(int n) {
    if (n <= 0) {
      return {};
    }

    const auto size = static_cast<std::size_t>(n);
    std::vector<std::vector<int>> result(size, std::vector<int>(size, 0));

    int row = 0;
    int col = 0;
    // Start moving right: (0, 1) -> (1, 0) -> (0, -1) -> (-1, 0).
    int dr = 0;
    int dc = 1;
    const int total = n * n;

    for (int val = 1; val <= total; ++val) {
      result[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)] =
          val;

      const int next_r = row + dr;
      const int next_c = col + dc;
      const bool out_of_bounds =
          next_r < 0 || next_r >= n || next_c < 0 || next_c >= n;

      // Turn 90 degrees clockwise when hitting the matrix edge or a filled cell.
      if (out_of_bounds ||
          result[static_cast<std::size_t>(next_r)]
                [static_cast<std::size_t>(next_c)] != 0) {
        const int prev_dr = dr;
        dr = dc;
        dc = -prev_dr;
      }

      row += dr;
      col += dc;
    }

    return result;
  }
};

LC_PROBLEM(59, "Spiral Matrix II", "math", ctx) {
  Solution sol;

  ctx.test("example_n3", [&] {
    const std::vector<std::vector<int>> expected = {
        {1, 2, 3},
        {8, 9, 4},
        {7, 6, 5},
    };
    ctx.expect_eq(sol.generateMatrix(3), expected);
  });

  ctx.test("n1_and_n2", [&] {
    ctx.expect_eq(sol.generateMatrix(1),
                  std::vector<std::vector<int>>{{1}});

    const std::vector<std::vector<int>> expected2 = {
        {1, 2},
        {4, 3},
    };
    ctx.expect_eq(sol.generateMatrix(2), expected2);
  });

  ctx.test("even_n4", [&] {
    const std::vector<std::vector<int>> expected4 = {
        {1, 2, 3, 4},
        {12, 13, 14, 5},
        {11, 16, 15, 6},
        {10, 9, 8, 7},
    };
    ctx.expect_eq(sol.generateMatrix(4), expected4);
  });
}

}  // namespace
