// LeetCode 1254: Number Of Closed Islands
// https://leetcode.com/problems/number-of-closed-islands/
//
// Problem:
//   Given a 2D binary grid of `0`s (land) and `1`s (water) with dimensions
//   1 <= m, n <= 100, an island is a maximal 4-directionally connected group
//   of `0`s. A closed island is an island that is completely surrounded
//   (top, bottom, left, right) by `1`s—meaning none of its cells touch the
//   outer boundary of the grid. Return the total number of closed islands.
//
// Examples:
//   Example 1:
//     Input:  grid = [[1, 1, 1, 1, 1, 1, 1, 0], [1, 0, 0, 0, 0, 1, 1, 0], [1, 0, 1,
//             0, 1, 1, 1, 0], [1, 0, 0, 0, 0, 1, 0, 1], [1, 1, 1, 1, 1, 1, 1, 0]]
//     Output: 2
//     Explanation:
//       - Since rows = 5 >= 3 and cols = 8 >= 3, Phase 1 runs flood_fill from the
//         border cells and sinks the border-touching 0s at (0, 7), (1, 7), (2, 7),
//         and (4, 7) to 1.
//       - Phase 2 scans interior cells [1..3] x [1..6] and encounters 0 at (1, 1),
//         incrementing count to 1 and sinking its entire 4-connected component
//         spanning rows 1..3 and cols 1..4 (including (2, 3)) to 1.
//       - Continuing the interior scan finds the remaining 0 at (3, 6),
//         incrementing count to 2 and sinking (3, 6) to 1.
//       - All remaining interior cells are 1, so closedIsland returns 2.
//
//   Example 2:
//     Input:  grid = [[0, 0, 1, 0, 0], [0, 1, 0, 1, 0], [0, 1, 1, 1, 0]]
//     Output: 1
//     Explanation:
//       - Phase 1 flood-fills from all border cells, sinking the left border land
//         {(0, 0), (0, 1), (1, 0), (2, 0)} and right border land {(0, 3), (0, 4),
//         (1, 4), (2, 4)} from 0 to 1.
//       - Phase 2 scans the single interior row r = 1 across columns 1..3, skipping
//         1s at (1, 1) and (1, 3).
//       - At (1, 2) == 0, count increments to 1 and flood_fill flips (1, 2) to 1,
//         returning 1.
//
//   Example 3:
//     Input:  grid = [[0, 0, 0], [0, 0, 0], [0, 0, 0]]
//     Output: 0
//     Explanation:
//       - Phase 1 starts flood_fill from border cell (0, 0) == 0 and sinks the
//         entire 4-connected 3x3 component (including center cell (1, 1)) to 1.
//       - Phase 2 checks interior cell (1, 1), finds it is already 1, and returns
//         0.
//
// Algorithm:
//   Use a two-phase flood-fill DFS (mutating visited land cells from `0` to
//   `1` in place so no separate visited matrix is required):
//     1. Boundary elimination: any land cell on the first/last row or
//        first/last column cannot belong to a closed island. Flood-fill from
//        every boundary `0` to sink all boundary-connected land into `1`.
//     2. Interior counting: scan the strict interior `[1, m - 2] x [1, n - 2]`.
//        Every remaining `0` belongs to a closed island—increment the count
//        and flood-fill its entire connected component to `1`.
//   Grids with fewer than 3 rows or fewer than 3 columns have no interior
//   cells capable of being surrounded on all 4 sides, so return 0 early.
//
//   Time Complexity:  O(m * n), as each cell is flipped at most once.
//   Space Complexity: O(m * n) worst-case recursion depth on a meandering
//                     land component (no extra visited buffer allocated).
//
// Data Structures:
//   - 2D `std::vector<std::vector<int>>` (`grid`) used both as the input graph
//     and in-place visited state (`0` -> `1`).
//   - Fixed `std::array<std::pair<int, int>, 4>` for the 4 cardinal offsets.
//
// Real-World Applications:
//   - Image processing and morphological hole filling: counting enclosed
//     cavities/voids inside a segmented binary mask (e.g., X-ray porosity or
//     semiconductor wafer defect detection).
//   - Procedural terrain and game-map generation: identifying enclosed lakes
//     or valleys isolated from the map edge.
//   - PCB / CAD geometry checks: detecting trapped copper islands isolated
//     from the board perimeter.
//
// Related LeetCode Problems:
//   - #130 Surrounded Regions [done] (identical boundary-first flood-fill pattern)
//   - #200 Number of Islands [done] (counting all 4-connected components)
//   - #1020 Number of Enclaves [new] (counting interior land cells instead of components)
//   - #1034 Coloring A Border [done] (grid boundary/component DFS)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

constexpr std::array<std::pair<int, int>, 4> kDirs = {
    {{0, 1}, {0, -1}, {1, 0}, {-1, 0}}};

class Solution {
 public:
  // Counts the number of maximal 4-connected components of 0s (land) that do
  // not touch the outer boundary of `grid`.
  //
  // Complexity: O(m * n) time, O(m * n) worst-case recursion stack space.
  int closedIsland(std::vector<std::vector<int>> grid) {
    const std::size_t rows = grid.size();
    if (rows < 3uz || grid[0].size() < 3uz) {
      // Every cell in a grid with < 3 rows or < 3 cols touches the boundary.
      return 0;
    }
    const std::size_t cols = grid[0].size();

    // Phase 1: Flood-fill all land touching the 4 borders of the grid so that
    // only genuinely enclosed islands remain as 0s.
    for (std::size_t c = 0uz; c < cols; ++c) {
      flood_fill(grid, 0, static_cast<int>(c));
      flood_fill(grid, static_cast<int>(rows - 1uz), static_cast<int>(c));
    }
    for (std::size_t r = 1uz; r + 1uz < rows; ++r) {
      flood_fill(grid, static_cast<int>(r), 0);
      flood_fill(grid, static_cast<int>(r), static_cast<int>(cols - 1uz));
    }

    // Phase 2: Every remaining 0 in the interior seeds a distinct closed
    // island; count it and sink its entire component.
    int count = 0;
    for (std::size_t r = 1uz; r + 1uz < rows; ++r) {
      for (std::size_t c = 1uz; c + 1uz < cols; ++c) {
        if (grid[r][c] == 0) {
          ++count;
          flood_fill(grid, static_cast<int>(r), static_cast<int>(c));
        }
      }
    }
    return count;
  }

 private:
  // Recursively marks the 4-connected land component containing `(r, c)` as
  // water (`1`) in place.
  static void flood_fill(std::vector<std::vector<int>>& grid, int r, int c) {
    const std::size_t rows = grid.size();
    const std::size_t cols = grid[0].size();
    if (r < 0 || std::cmp_greater_equal(r, rows) || c < 0 ||
        std::cmp_greater_equal(c, cols)) {
      return;
    }

    const auto ru = static_cast<std::size_t>(r);
    const auto cu = static_cast<std::size_t>(c);
    if (grid[ru][cu] != 0) {
      return;
    }

    grid[ru][cu] = 1;
    for (const auto& [dr, dc] : kDirs) {
      flood_fill(grid, r + dr, c + dc);
    }
  }
};

LC_PROBLEM(1254, "Number Of Closed Islands", "dfs", ctx) {
  Solution sol;

  ctx.test("example_1_two_closed_islands", [&] {
    const std::vector<std::vector<int>> grid = {
        {1, 1, 1, 1, 1, 1, 1, 0},
        {1, 0, 0, 0, 0, 1, 1, 0},
        {1, 0, 1, 0, 1, 1, 1, 0},
        {1, 0, 0, 0, 0, 1, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 0},
    };
    ctx.expect_eq(sol.closedIsland(grid), 2);
  });

  ctx.test("example_2_one_closed_island", [&] {
    const std::vector<std::vector<int>> grid = {
        {0, 0, 1, 0, 0},
        {0, 1, 0, 1, 0},
        {0, 1, 1, 1, 0},
    };
    ctx.expect_eq(sol.closedIsland(grid), 1);
  });

  ctx.test("example_3_concentric_rings", [&] {
    const std::vector<std::vector<int>> grid = {
        {1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 1},
        {1, 0, 1, 0, 1, 0, 1},
        {1, 0, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1},
    };
    ctx.expect_eq(sol.closedIsland(grid), 2);
  });

  ctx.test("small_grids_cannot_have_closed_islands", [&] {
    ctx.expect_eq(sol.closedIsland({{0}}), 0);
    ctx.expect_eq(sol.closedIsland({{1}}), 0);
    ctx.expect_eq(sol.closedIsland({{0}, {0}, {0}}), 0);
    ctx.expect_eq(sol.closedIsland({{1, 0}, {0, 1}}), 0);
  });

  ctx.test("all_water_and_all_land", [&] {
    ctx.expect_eq(sol.closedIsland({{1, 1, 1}, {1, 1, 1}, {1, 1, 1}}), 0);
    ctx.expect_eq(sol.closedIsland({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}), 0);
  });
}

}  // namespace
