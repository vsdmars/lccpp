// LeetCode 1034: Coloring A Border
// https://leetcode.com/problems/coloring-a-border/
//
// Problem:
//   Given an m x n integer matrix `grid` (1 <= m, n <= 50) representing cell
//   colors, a starting coordinate `(row, col)`, and a target `color`, identify
//   the 4-directionally connected component of cells sharing the initial color
//   of `grid[row][col]`. Recolor only the border of this connected component
//   with `color`—where a cell is on the border if it lies on the outer boundary
//   of `grid` or is 4-directionally adjacent to at least one cell not in the
//   component—and return the modified grid.
//
// Algorithm:
//   Record `orig_color = grid[row][col]` and run a DFS from `(row, col)` using
//   a 2D `visited` table. Each DFS call returns 1 if `(r, c)` belongs to the
//   connected component and 0 otherwise:
//     - Out-of-bounds cells return 0.
//     - Already-visited cells return 1 immediately (they are guaranteed to be
//       part of the component even if their color was already updated in
//       post-order).
//     - Unvisited cells whose color differs from `orig_color` return 0.
//     - Otherwise, mark `(r, c)` visited, sum the return values of its 4
//       neighbors, and if `same_neighbors < 4`, recolor `grid[r][c] = color`
//       because at least one side touches the grid edge or another color.
//
//   Time Complexity:  O(m * n), visiting each cell at most once.
//   Space Complexity: O(m * n) for the `visited` grid and DFS recursion stack.
//
// Data Structures:
//   - 2D `std::vector<std::vector<int>>` (`grid`) modified in place.
//   - 2D `std::vector<std::vector<std::uint8_t>>` (`visited`) recording
//     membership in the target connected component in O(1) per cell.
//   - Fixed `std::array<std::pair<int, int>, 4>` for the 4 cardinal offsets.
//
// Real-World Applications:
//   - Raster image editors (e.g., Photoshop "Magic Wand" selection outline and
//     stroke-border rendering around a contiguous pixel region).
//   - GIS / satellite mapping: extracting polygon perimeters from labeled
//     land-use rasters without altering interior tiles.
//   - PCB / VLSI layout verification: highlighting perimeter pads of a copper
//     pour region for clearance inspection.
//
// Related LeetCode Problems:
//   - #733 Flood Fill [done] (recoloring the entire connected component instead of just its border)
//   - #200 Number of Islands [done] (4-directional connected-component DFS on grids)
//   - #463 Island Perimeter [done] (counting boundary edges of a grid component)
//   - #1254 Number of Closed Islands [done] (distinguishing boundary-touching vs interior components)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace {

constexpr std::array<std::pair<int, int>, 4> kDirs = {
    {{0, 1}, {0, -1}, {1, 0}, {-1, 0}}};

class Solution {
 public:
  // Recolors the border of the 4-connected component containing `(row, col)`
  // to `color` and returns the updated grid.
  //
  // Complexity: O(m * n) time, O(m * n) space.
  std::vector<std::vector<int>> colorBorder(std::vector<std::vector<int>> grid,
                                            int row, int col, int color) {
    if (grid.empty() || grid[0].empty()) {
      return grid;
    }

    const std::size_t rows = grid.size();
    const std::size_t cols = grid[0].size();
    const int orig_color =
        grid[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)];

    // `visited[r][c] == 1` iff (r, c) has been confirmed part of the starting
    // connected component. Keeping this separate from `grid` avoids confusion
    // when a border cell is recolored in post-order.
    std::vector<std::vector<std::uint8_t>> visited(
        rows, std::vector<std::uint8_t>(cols, 0));

    // Returns 1 if (r, c) belongs to the connected component, 0 otherwise.
    auto dfs = [&](this auto&& self, int r, int c) -> int {
      if (r < 0 || std::cmp_greater_equal(r, rows) || c < 0 ||
          std::cmp_greater_equal(c, cols)) {
        return 0;
      }

      const auto ru = static_cast<std::size_t>(r);
      const auto cu = static_cast<std::size_t>(c);

      // Must check `visited` before `grid[ru][cu]` because an already-finished
      // border cell in the component has been overwritten with `color`.
      if (visited[ru][cu] != 0) {
        return 1;
      }
      if (grid[ru][cu] != orig_color) {
        return 0;
      }

      visited[ru][cu] = 1;
      int same_neighbors = 0;
      for (const auto& [dr, dc] : kDirs) {
        same_neighbors += self(r + dr, c + dc);
      }

      // Fewer than 4 in-component neighbors means (r, c) is on the component's
      // border (either touching the grid boundary or an adjacent other color).
      if (same_neighbors < 4) {
        grid[ru][cu] = color;
      }
      return 1;
    };

    dfs(row, col);
    return grid;
  }
};

LC_PROBLEM(1034, "Coloring A Border", "dfs", ctx) {
  Solution sol;

  ctx.test("example_1_2x2_grid", [&] {
    const std::vector<std::vector<int>> grid = {{1, 1}, {1, 2}};
    const std::vector<std::vector<int>> expected = {{3, 3}, {3, 2}};
    ctx.expect_eq(sol.colorBorder(grid, 0, 0, 3), expected);
  });

  ctx.test("example_2_rectangular_component", [&] {
    const std::vector<std::vector<int>> grid = {{1, 2, 2}, {2, 3, 2}};
    const std::vector<std::vector<int>> expected = {{1, 3, 3}, {2, 3, 3}};
    ctx.expect_eq(sol.colorBorder(grid, 0, 1, 3), expected);
  });

  ctx.test("example_3_interior_preserved", [&] {
    // Center cell (1, 1) has all 4 neighbors in the component, so it stays 1.
    const std::vector<std::vector<int>> grid = {
        {1, 1, 1}, {1, 1, 1}, {1, 1, 1}};
    const std::vector<std::vector<int>> expected = {
        {2, 2, 2}, {2, 1, 2}, {2, 2, 2}};
    ctx.expect_eq(sol.colorBorder(grid, 1, 1, 2), expected);
  });

  ctx.test("single_cell_grid", [&] {
    const std::vector<std::vector<int>> grid = {{5}};
    const std::vector<std::vector<int>> expected = {{9}};
    ctx.expect_eq(sol.colorBorder(grid, 0, 0, 9), expected);
  });

  ctx.test("new_color_matches_neighbor_color", [&] {
    // Recoloring border to 1 (which already appears adjacent to component 2)
    // must not accidentally leak into neighboring 1-cells.
    const std::vector<std::vector<int>> grid = {
        {1, 2, 1, 2, 1, 2},
        {2, 2, 2, 2, 1, 2},
        {1, 2, 2, 2, 1, 2},
    };
    const std::vector<std::vector<int>> expected = {
        {1, 1, 1, 1, 1, 2},
        {1, 2, 1, 1, 1, 2},
        {1, 1, 1, 1, 1, 2},
    };
    ctx.expect_eq(sol.colorBorder(grid, 1, 3, 1), expected);
  });
}

}  // namespace
