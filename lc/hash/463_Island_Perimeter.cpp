// LeetCode 463: Island Perimeter
// https://leetcode.com/problems/island-perimeter/
//
// Problem:
//   Given an R x C binary grid where 1 represents land and 0 represents water,
//   cells are connected horizontally and vertically (4-directionally). The
//   grid is completely surrounded by water, contains exactly one connected
//   island, and has no interior lakes. Each cell is a unit square with side
//   length 1. Return the total perimeter of the island.
//
// Algorithm:
//   Primary approach (Single-Pass Shared-Edge Subtraction):
//     Scan every cell (r, c) in row-major order. Each land cell initially
//     contributes 4 unit edges to the perimeter. Whenever its top neighbor
//     (r - 1, c) is also land, both cells share an internal horizontal edge
//     that must not be counted by either cell, so subtract 2. Likewise,
//     whenever its left neighbor (r, c - 1) is land, subtract 2 for the shared
//     vertical edge. Checking only top and left avoids double-counting pairs.
//   Alternative approach (Axis Transition Counting, mirroring Python rewrite):
//     Every 0 <-> 1 boundary transition along any row (padded with implicit 0s
//     at both ends) represents one vertical perimeter edge, and every 0 <-> 1
//     transition along any column represents one horizontal perimeter edge.
//   Time Complexity:  O(R * C) for both approaches.
//   Space Complexity: O(1) auxiliary space for both approaches.
//
// Data Structures:
//   - 2D grid (const std::vector<std::vector<int>>&): read directly in place
//     without extra copies or visited tables.
//   - Scalar accumulator (int): tracks the running perimeter count.
//
// Real-World Applications:
//   - Geographic Information Systems (GIS) computing coastline lengths and
//     parcel boundaries from rasterized satellite imagery.
//   - Computer vision and medical image segmentation measuring contour length
//     and compactness (perimeter^2 / area) of binary masks.
//   - VLSI chip layout verification calculating wiring and region boundary
//     lengths on discretized Manhattan grids.
//
// Related LeetCode Problems:
//   - #200 Number of Islands [done] (connected component counting on 2D binary grids)
//   - #695 Max Area of Island [done] (computing largest land region area on a grid)
//   - #1034 Coloring A Border [done] (identifying boundary cells of a connected component)
//   - #827 Making A Large Island [done] (component boundary analysis on binary grids)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the island perimeter in a single row-major pass by adding 4 for
  // each land cell and subtracting 2 for each shared top or left land edge.
  // Time: O(R * C), Space: O(1).
  int islandPerimeter(const std::vector<std::vector<int>>& grid) {
    if (grid.empty() || grid[0].empty()) {
      return 0;
    }

    const std::size_t rows = grid.size();
    const std::size_t cols = grid[0].size();
    int perimeter = 0;

    for (std::size_t r = 0uz; r < rows; ++r) {
      for (std::size_t c = 0uz; c < cols; ++c) {
        if (grid[r][c] != 1) {
          continue;
        }
        // Start with all 4 unit sides of this land square.
        perimeter += 4;
        // Deduct the shared edge once for each already-visited land neighbor
        // (top and left), removing 1 side from the current cell and 1 from
        // its neighbor.
        if (r > 0uz && grid[r - 1uz][c] == 1) {
          perimeter -= 2;
        }
        if (c > 0uz && grid[r][c - 1uz] == 1) {
          perimeter -= 2;
        }
      }
    }

    return perimeter;
  }

  // Alternative XOR/transition-counting formulation (from Python `rewrite`):
  // counts every 0 <-> 1 change along all rows (vertical edges) and all
  // columns (horizontal edges), treating out-of-bounds as water (0).
  // Time: O(R * C), Space: O(1).
  int islandPerimeterTransitions(const std::vector<std::vector<int>>& grid) {
    if (grid.empty() || grid[0].empty()) {
      return 0;
    }

    const std::size_t rows = grid.size();
    const std::size_t cols = grid[0].size();
    int perimeter = 0;

    // Count vertical edges by scanning horizontal 0 <-> 1 transitions per row.
    for (std::size_t r = 0uz; r < rows; ++r) {
      int prev = 0;
      for (std::size_t c = 0uz; c < cols; ++c) {
        if (grid[r][c] != prev) {
          ++perimeter;
        }
        prev = grid[r][c];
      }
      // Close trailing land cell against the implicit right water border.
      if (prev != 0) {
        ++perimeter;
      }
    }

    // Count horizontal edges by scanning vertical 0 <-> 1 transitions per col.
    for (std::size_t c = 0uz; c < cols; ++c) {
      int prev = 0;
      for (std::size_t r = 0uz; r < rows; ++r) {
        if (grid[r][c] != prev) {
          ++perimeter;
        }
        prev = grid[r][c];
      }
      // Close trailing land cell against the implicit bottom water border.
      if (prev != 0) {
        ++perimeter;
      }
    }

    return perimeter;
  }
};

LC_PROBLEM(463, "Island Perimeter", "hash", ctx) {
  ctx.test("example_standard_island", [&] {
    Solution sol;
    const std::vector<std::vector<int>> grid = {
        {0, 1, 0, 0},
        {1, 1, 1, 0},
        {0, 1, 0, 0},
        {1, 1, 0, 0},
    };
    ctx.expect_eq(sol.islandPerimeter(grid), 16);
    ctx.expect_eq(sol.islandPerimeterTransitions(grid), 16);
  });

  ctx.test("single_land_cell", [&] {
    Solution sol;
    const std::vector<std::vector<int>> grid = {{1}};
    ctx.expect_eq(sol.islandPerimeter(grid), 4);
    ctx.expect_eq(sol.islandPerimeterTransitions(grid), 4);
  });

  ctx.test("single_row_two_cells", [&] {
    Solution sol;
    const std::vector<std::vector<int>> grid = {{1, 0}};
    ctx.expect_eq(sol.islandPerimeter(grid), 4);
    ctx.expect_eq(sol.islandPerimeterTransitions(grid), 4);
  });

  ctx.test("solid_rectangle", [&] {
    Solution sol;
    const std::vector<std::vector<int>> grid = {
        {1, 1, 1},
        {1, 1, 1},
    };
    ctx.expect_eq(sol.islandPerimeter(grid), 10);
    ctx.expect_eq(sol.islandPerimeterTransitions(grid), 10);
  });

  ctx.test("empty_and_all_water", [&] {
    Solution sol;
    const std::vector<std::vector<int>> empty_grid;
    const std::vector<std::vector<int>> water = {{0, 0}, {0, 0}};
    ctx.expect_eq(sol.islandPerimeter(empty_grid), 0);
    ctx.expect_eq(sol.islandPerimeterTransitions(empty_grid), 0);
    ctx.expect_eq(sol.islandPerimeter(water), 0);
    ctx.expect_eq(sol.islandPerimeterTransitions(water), 0);
  });
}

}  // namespace
