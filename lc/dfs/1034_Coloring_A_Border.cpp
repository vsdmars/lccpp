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
// Examples:
//   Example 1:
//     Input:  grid = [[1, 1], [1, 2]], row = 0, col = 0, color = 3
//     Output: [[3, 3], [3, 2]]
//     Explanation:
//       - Record orig_color = grid[0][0] = 1 and start DFS at (0, 0), marking it
//         visited.
//       - Visit right neighbor (0, 1) (val 1): mark visited, then check its 4
//         neighbors—(0, 2) and (-1, 1) are out of bounds (0), (0, 0) is already
//         visited (1), and (1, 1) has val 2 != 1 (0)—giving same_neighbors = 1 < 4,
//         so set grid[0][1] = 3 and return 1.
//       - Visit down neighbor (1, 0) (val 1): mark visited, find only (0, 0)
//         in-component (same_neighbors = 1 < 4), set grid[1][0] = 3, and return 1.
//       - Back at (0, 0), sum the 4 neighbor results (1 + 0 + 1 + 0 = 2 < 4) and
//         recolor grid[0][0] = 3, leaving grid[1][1] = 2 untouched.
//
//   Example 2:
//     Input:  grid = [[1, 1, 1], [1, 1, 1], [1, 1, 1]], row = 1, col = 1, color = 2
//     Output: [[2, 2, 2], [2, 1, 2], [2, 2, 2]]
//     Explanation:
//       - Record orig_color = 1 at center (1, 1) and mark all 9 cells of the
//         uniform 3x3 component visited during DFS.
//       - All 8 outer-ring cells have at least one out-of-bounds neighbor
//         (same_neighbors = 2 at corners and 3 at edge midpoints, all < 4), so each
//         is recolored to 2 in post-order.
//       - Back at center (1, 1), all 4 neighbors returned 1 (same_neighbors = 4),
//         so same_neighbors < 4 is false and grid[1][1] stays 1.
//
//   Example 3:
//     Input:  grid = [[1, 2, 1, 2, 1, 2], [2, 2, 2, 2, 1, 2], [1, 2, 2, 2, 1, 2]],
//             row = 1, col = 3, color = 1
//     Output: [[1, 1, 1, 1, 1, 2], [1, 2, 1, 1, 1, 2], [1, 1, 1, 1, 1, 2]]
//     Explanation:
//       - Record orig_color = grid[1][3] = 2 and explore the 9-cell connected
//         component of 2s across columns 0..3; column 4 of 1s blocks DFS from
//         reaching the 2s in column 5.
//       - Mark each component cell in visited before checking neighbors so
//         already-recolored border cells (now 1) still return 1 when revisited,
//         preventing false boundary counts or leaks into adjacent 1s.
//       - Among the 9 visited cells, only (1, 1) has all 4 cardinal neighbors—(1,
//         2), (1, 0), (2, 1), and (0, 1)—in the component (same_neighbors = 4), so
//         (1, 1) remains 2 while the other 8 cells are recolored to 1.
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
