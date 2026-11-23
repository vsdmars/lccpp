// LeetCode 59: Spiral Matrix II
// https://leetcode.com/problems/spiral-matrix-ii/
//
// Problem:
//   Given a positive integer `n`, generate an `n x n` square matrix filled
//   with the integers `1` through `n^2` in clockwise spiral order starting at
//   the top-left cell `(0, 0)`.
//
// Examples:
//   Example 1:
//     Input:  n = 3
//     Output: [[1, 2, 3], [8, 9, 4], [7, 6, 5]]
//     Explanation:
//       - Create a 3 x 3 matrix filled with 0 and start at (0, 0) with direction
//         (dr, dc) = (0, 1).
//       - Write 1, 2, 3 across row 0, then hit next_c = 3 out of bounds at (0, 2)
//         and rotate (0, 1) to (1, 0) to write 4, 5 down column 2.
//       - At (2, 2) rotate (1, 0) to (0, -1) to write 6, 7 across row 2, then at
//         (2, 0) rotate to (-1, 0) to write 8 at (1, 0).
//       - From (1, 0), probing (0, 0) sees non-zero entry 1, rotating direction to
//         (0, 1) and stepping to (1, 1) to write the final value 9.
//
//   Example 2:
//     Input:  n = 4
//     Output: [[1, 2, 3, 4], [12, 13, 14, 5], [11, 16, 15, 6], [10, 9, 8, 7]]
//     Explanation:
//       - Initialize a 4 x 4 zero matrix and walk clockwise from (0, 0) with
//         initial direction (0, 1).
//       - The outer ring fills 1..4 along row 0, turns down at (0, 3) for 5..7,
//         turns left at (3, 3) for 8..10, and turns up at (3, 0) for 11, 12.
//       - At (1, 0), the probe (0, 0) is already non-zero, rotating direction to
//         (0, 1) to fill the inner 2 x 2 ring: 13 at (1, 1), 14 at (1, 2), 15 at
//         (2, 2), and 16 at (2, 1).
//
//   Example 3:
//     Input:  n = 1
//     Output: [[1]]
//     Explanation:
//       - Allocate a 1 x 1 matrix initialized to [[0]] with (row, col) = (0, 0),
//         (dr, dc) = (0, 1), and total = 1.
//       - On the single loop step val = 1, write 1 to result[0][0], probe (0, 1)
//         out of bounds to rotate direction to (1, 0), and advance out of the loop
//         returning [[1]].
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
