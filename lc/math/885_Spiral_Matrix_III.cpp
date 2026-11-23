// LeetCode 885: Spiral Matrix III
// https://leetcode.com/problems/spiral-matrix-iii/
//
// Problem:
//   You start at cell (rStart, cStart) of a `rows x cols` 2D grid (1 <= rows,
//   cols <= 100) facing east, where (0, 0) is the north-west corner. Walk in a
//   clockwise spiral to visit every position in the grid; whenever the spiral
//   steps outside the grid boundary, keep walking outside until it re-enters.
//   Return the list of all `rows * cols` grid coordinates in the order visited.
//
// Algorithm:
//   Simulate the expanding clockwise spiral directly:
//   - The spiral leg lengths follow the sequence 1, 1, 2, 2, 3, 3, 4, 4, ...
//     so leg index `turn` (starting at 0) walks `(turn / 2) + 1` steps.
//   - Start with direction vector `(dr, dc) = (0, 1)` (east). After each leg,
//     rotate 90 degrees clockwise via the 2D cross-product transformation
//     `(dr, dc) -> (dc, -dr)`, which cycles through:
//       East (0, 1) -> South (1, 0) -> West (0, -1) -> North (-1, 0).
//   - At each step, record `(r, c)` whenever it lies inside `[0, rows) x [0, cols)`,
//     stopping as soon as all `rows * cols` cells have been collected.
//   Time: O(max(rows, cols)^2), Space: O(rows * cols) for the output (O(1)
//   auxiliary space).
//
// Data Structures:
//   `std::vector<std::vector<int>>` pre-reserved to `rows * cols` elements to
//   store the visited coordinates without reallocations.
//
// Real-World Applications:
//   - Expanding-ring spatial search in robotics coverage planning, drone area
//     sweeps, and nearest-free-slot placement on 2D grids.
//   - Spiral tile loading around the camera viewport in mapping engines and
//     progressive image/texture rendering.
//
// Related LeetCode Problems:
//   - #54 Spiral Matrix [done] (clockwise in-bounds spiral traversal of a matrix)
//   - #59 Spiral Matrix II [done] (filling an n x n matrix in clockwise spiral order)
//   - #2326 Spiral Matrix IV [new] (filling a matrix in spiral order from a linked list)
//   - #874 Walking Robot Simulation [new] (2D grid walk with 90-degree directional turns)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Returns all coordinates of a `rows x cols` grid in the order visited by a
  // clockwise spiral starting at (rStart, cStart) facing east.
  //
  // Invariant: Leg `turn` advances `(turn / 2) + 1` steps along `(dr, dc)`
  // before rotating 90 degrees clockwise; every in-bounds grid cell is visited
  // exactly once.
  //
  // Time: O(max(rows, cols)^2), Space: O(rows * cols) for output (O(1) aux).
  std::vector<std::vector<int>> spiralMatrixIII(int rows, int cols, int rStart,
                                                int cStart) {
    const auto total_cells =
        static_cast<std::size_t>(rows) * static_cast<std::size_t>(cols);
    std::vector<std::vector<int>> visited;
    visited.reserve(total_cells);

    int r = rStart;
    int c = cStart;
    int dr = 0;   // Start facing east: row delta = 0, col delta = +1.
    int dc = 1;
    int turn = 0;

    while (visited.size() < total_cells) {
      // Spiral leg lengths follow 1, 1, 2, 2, 3, 3, ... = (turn / 2) + 1.
      const int leg_steps = (turn / 2) + 1;
      for (int step = 0; step < leg_steps && visited.size() < total_cells;
           ++step) {
        if (r >= 0 && r < rows && c >= 0 && c < cols) {
          visited.push_back({r, c});
        }
        r += dr;
        c += dc;
      }

      // Rotate 90 degrees clockwise: (0,1) -> (1,0) -> (0,-1) -> (-1,0).
      const int next_dr = dc;
      dc = -dr;
      dr = next_dr;
      ++turn;
    }

    return visited;
  }
};

LC_PROBLEM(885, "Spiral Matrix III", "math", ctx) {
  Solution sol;

  ctx.test("example_1_single_row", [&] {
    const std::vector<std::vector<int>> expected = {
        {0, 0}, {0, 1}, {0, 2}, {0, 3}};
    ctx.expect_eq(sol.spiralMatrixIII(1, 4, 0, 0), expected);
  });

  ctx.test("example_2_5x6_grid", [&] {
    const std::vector<std::vector<int>> expected = {
        {1, 4}, {1, 5}, {2, 5}, {2, 4}, {2, 3}, {1, 3}, {0, 3}, {0, 4},
        {0, 5}, {3, 5}, {3, 4}, {3, 3}, {3, 2}, {2, 2}, {1, 2}, {0, 2},
        {4, 5}, {4, 4}, {4, 3}, {4, 2}, {4, 1}, {3, 1}, {2, 1}, {1, 1},
        {0, 1}, {4, 0}, {3, 0}, {2, 0}, {1, 0}, {0, 0}};
    ctx.expect_eq(sol.spiralMatrixIII(5, 6, 1, 4), expected);
  });

  ctx.test("single_cell_1x1", [&] {
    const std::vector<std::vector<int>> expected = {{0, 0}};
    ctx.expect_eq(sol.spiralMatrixIII(1, 1, 0, 0), expected);
  });

  ctx.test("single_column_starting_bottom", [&] {
    const std::vector<std::vector<int>> expected = {{2, 0}, {1, 0}, {0, 0}};
    ctx.expect_eq(sol.spiralMatrixIII(3, 1, 2, 0), expected);
  });

  ctx.test("2x2_from_top_left", [&] {
    const std::vector<std::vector<int>> expected = {
        {0, 0}, {0, 1}, {1, 1}, {1, 0}};
    ctx.expect_eq(sol.spiralMatrixIII(2, 2, 0, 0), expected);
  });
}

}  // namespace
