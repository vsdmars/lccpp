// LeetCode 361: Bomb Enemy
// https://leetcode.com/problems/bomb-enemy/
//
// Problem:
//   Given an m x n character grid where each cell is a wall `'W'`, an enemy
//   `'E'`, or empty `'0'`, return the maximum number of enemies that can be
//   killed using a single bomb. A bomb can only be placed on an empty cell
//   `'0'` and kills every enemy in the same row and column radiating outward
//   until blocked by a wall `'W'` or the grid boundary.
//
// Algorithm:
//   Row/Column Segment Caching (Space-Optimized 2D DP). Walls partition each
//   row and each column into independent maximal wall-free segments; every cell
//   inside the same segment shares the exact same enemy count along that axis.
//   Scanning top-to-bottom, left-to-right:
//   - Recompute `row_hits` only at the start of a row (`c == 0`) or immediately
//     after a wall (`grid[r][c - 1] == 'W'`), counting `'E'` forward until the
//     next `'W'` or row end.
//   - Recompute `col_hits[c]` only at the start of a column (`r == 0`) or
//     immediately after a wall above (`grid[r - 1][c] == 'W'`), counting `'E'`
//     downward until the next `'W'` or column end.
//   - Whenever `grid[r][c] == '0'`, update `best = max(best, row_hits +
//     col_hits[c])`.
//   Each cell is visited once by the main scan, at most once by a horizontal
//   segment scan, and at most once by a vertical segment scan.
//   Time Complexity:  O(m * n).
//   Space Complexity: O(n) for the per-column cached segment counts.
//
// Data Structures:
//   - Scalar `int row_hits` caching the active horizontal segment's enemy count.
//   - `std::vector<int> col_hits` of length n caching each column's active
//     vertical segment enemy count.
//
// Real-World Applications:
//   - 2D line-of-sight visibility and sensor/turret/camera placement analysis
//     in indoor floorplans with occluding walls.
//   - Orthogonal coverage optimization for warehouse Wi-Fi access points and
//     sprinkler layouts partitioned by fire walls.
//   - Cross-axis prefix/segment aggregation on sparse blocked matrices.
//
// Related LeetCode Problems:
//   - #764 Largest Plus Sign [new] (4-directional contiguous run lengths around grid cells)
//   - #2257 Count Unguarded Cells in the Grid [new] (orthogonal ray propagation blocked by walls)
//   - #999 Available Captures for Rook [new] (4-directional scan stopped by blockers)
//   - #221 Maximal Square [done] (2D grid DP with O(n) space optimization)
//   - #329 Longest Increasing Path In A Matrix [done] (2D grid dynamic programming)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the maximum enemies killable by planting one bomb on an empty cell.
  // Invariant: at `(r, c)`, `row_hits` equals the number of `'E'` cells in the
  // maximal wall-free horizontal segment containing `(r, c)`, and `col_hits[c]`
  // equals the number of `'E'` cells in the maximal wall-free vertical segment
  // containing `(r, c)`.
  // Time: O(m * n), Space: O(n).
  int maxKilledEnemies(const std::vector<std::vector<char>>& grid) {
    if (grid.empty() || grid[0uz].empty()) {
      return 0;
    }

    const std::size_t rows = grid.size();
    const std::size_t cols = grid[0uz].size();

    int row_hits = 0;
    std::vector<int> col_hits(cols, 0);
    int best = 0;

    for (std::size_t r = 0uz; r < rows; ++r) {
      for (std::size_t c = 0uz; c < cols; ++c) {
        // Recompute the horizontal segment count only when entering a new
        // wall-free row segment.
        if (c == 0uz || grid[r][c - 1uz] == 'W') {
          row_hits = countRowSegment(grid, r, c);
        }

        // Recompute the vertical segment count only when entering a new
        // wall-free column segment.
        if (r == 0uz || grid[r - 1uz][c] == 'W') {
          col_hits[c] = countColSegment(grid, r, c);
        }

        // Bombs can only be placed on empty cells ('0').
        if (grid[r][c] == '0') {
          best = std::max(best, row_hits + col_hits[c]);
        }
      }
    }

    return best;
  }

 private:
  // Counts enemies in row `r` starting at `start_col` until a wall or row end.
  static int countRowSegment(const std::vector<std::vector<char>>& grid,
                             std::size_t r, std::size_t start_col) {
    int count = 0;
    const std::size_t cols = grid[r].size();
    for (std::size_t c = start_col; c < cols && grid[r][c] != 'W'; ++c) {
      if (grid[r][c] == 'E') {
        ++count;
      }
    }
    return count;
  }

  // Counts enemies in column `c` starting at `start_row` until a wall or end.
  static int countColSegment(const std::vector<std::vector<char>>& grid,
                             std::size_t start_row, std::size_t c) {
    int count = 0;
    const std::size_t rows = grid.size();
    for (std::size_t r = start_row; r < rows && grid[r][c] != 'W'; ++r) {
      if (grid[r][c] == 'E') {
        ++count;
      }
    }
    return count;
  }
};

LC_PROBLEM(361, "Bomb Enemy", "dp", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    const std::vector<std::vector<char>> grid = {
        {'0', 'E', '0', '0'},
        {'E', '0', 'W', 'E'},
        {'0', 'E', '0', '0'},
    };
    // Placing a bomb at (1, 1) kills 3 enemies.
    ctx.expect_eq(sol.maxKilledEnemies(grid), 3);
  });

  ctx.test("example_2_walls_blocking", [&] {
    Solution sol;
    const std::vector<std::vector<char>> grid = {
        {'W', 'W', 'W'},
        {'0', '0', '0'},
        {'E', 'E', 'E'},
    };
    ctx.expect_eq(sol.maxKilledEnemies(grid), 1);
  });

  ctx.test("secondary_grid_with_multiple_walls", [&] {
    Solution sol;
    const std::vector<std::vector<char>> grid = {
        {'0', 'E', 'E', 'W'},
        {'E', '0', 'W', 'E'},
        {'0', 'E', 'W', 'E'},
    };
    ctx.expect_eq(sol.maxKilledEnemies(grid), 3);
  });

  ctx.test("edge_cases", [&] {
    Solution sol;
    ctx.expect_eq(sol.maxKilledEnemies({}), 0);
    ctx.expect_eq(sol.maxKilledEnemies({{'E', 'W'}, {'E', 'E'}}), 0);
    ctx.expect_eq(sol.maxKilledEnemies({{'0', '0'}, {'0', '0'}}), 0);
    ctx.expect_eq(sol.maxKilledEnemies({{'E', '0', 'E', 'W', 'E', 'E', 'E'}}),
                  2);
  });
}

}  // namespace
