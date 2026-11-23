// LeetCode 695: Max Area Of Island
// https://leetcode.com/problems/max-area-of-island/
//
// Problem:
//   Given an m x n binary matrix `grid` where `1` represents land and `0`
//   represents water, an island is a group of `1`s connected 4-directionally
//   (horizontally or vertically, not diagonally). The area of an island is the
//   number of land cells it contains. Return the maximum area of an island in
//   `grid`, or `0` if no island exists.
//
// Examples:
//   Example 1:
//     Input:  grid = [[0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 0,
//             0, 1, 1, 1, 0, 0, 0], [0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0], [0, 1,
//             0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0], [0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1,
//             0, 0], [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0], [0, 0, 0, 0, 0, 0, 0,
//             1, 1, 1, 0, 0, 0], [0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0]]
//     Output: 6
//     Explanation:
//       - Row-major scanning finds the single land cell at (0, 2); dfsArea zeroes
//         it and returns 1, setting max_area = 1.
//       - At (0, 7), dfsArea sinks (0, 7), (1, 7), (1, 8), and (1, 9), updating
//         max_area = 4; subsequent islands at (2, 1) and (2, 4) yield areas 4 and
//         5, raising max_area = 5.
//       - At (3, 8), dfsArea sinks (3, 8), (4, 8), (4, 9), (4, 10), (3, 10), and
//         (5, 10) for area 6, updating max_area = 6.
//       - The remaining island starting at (6, 7) spans 5 cells ((6, 7), (6, 8),
//         (6, 9), (7, 7), (7, 8)), so maxAreaOfIsland returns 6.
//
//   Example 2:
//     Input:  grid = [[1, 0, 1], [0, 1, 0], [1, 0, 1]]
//     Output: 1
//     Explanation:
//       - Scanning row 0 encounters grid[0][0] == 1 and invokes dfsArea(grid, 0,
//         0), which sets grid[0][0] = 0.
//       - kDirs4 checks only the four orthogonal neighbors (0, 1), (0, -1), (1, 0),
//         and (-1, 0), each of which is 0 or out of bounds, so dfsArea returns 1
//         without reaching the diagonal 1 at (1, 1).
//       - Later iterations visit the remaining isolated 1s at (0, 2), (1, 1), (2,
//         0), and (2, 2), each returning area 1 and leaving max_area = 1.
//
//   Example 3:
//     Input:  grid = [[0, 0, 0, 0, 0, 0, 0, 0]]
//     Output: 0
//     Explanation:
//       - maxAreaOfIsland verifies grid and grid[0] are non-empty and initializes
//         max_area = 0.
//       - The outer loops scan row 0 across columns 0..7, finding grid[0][c] == 0
//         at every cell so dfsArea is never invoked.
//       - max_area remains 0 and is returned.
//
// Algorithm:
//   Iterate over every cell `(r, c)` of the grid. Whenever an unvisited land
//   cell (`grid[r][c] == 1`) is encountered, run a 4-directional flood-fill
//   DFS that sinks each visited land cell (`grid[r][c] = 0`) and returns
//   `1` plus the summed areas returned by its four neighbors. Track the
//   maximum area discovered across all connected components.
//   Time Complexity:  O(m * n), because each cell is scanned once in the outer
//                     loop and visited at most once by DFS before being zeroed.
//   Space Complexity: O(m * n) worst-case recursion depth if the entire grid
//                     forms a single snake-like island.
//
// Data Structures:
//   - 2D integer grid (`std::vector<std::vector<int>>`): mutated in-place
//     (`1 -> 0`) to mark visited land cells with zero extra visited memory.
//   - Fixed 4-direction offset array (`std::array<std::pair<int, int>, 4>`):
//     cleanly drives orthogonal neighbor expansion.
//
// Real-World Applications:
//   - Medical imaging & pathology segmentation: computing the largest
//     contiguous lesion or tumor cross-section in a binary mask.
//   - Geospatial & environmental monitoring: measuring the largest contiguous
//     wildfire burn scar, deforestation patch, or flood zone from raster maps.
//   - Semiconductor wafer defect analysis: finding the largest clustered die
//     failure region to diagnose equipment anomalies.
//
// Related LeetCode Problems:
//   - #200 Number of Islands [done] (count 4-connected land components)
//   - #694 Number of Distinct Islands [done] (deduplicate island shapes via DFS)
//   - #463 Island Perimeter [done] (compute perimeter of an island)
//   - #1254 Number of Closed Islands [done] (islands not touching grid borders)
//   - #827 Making A Large Island [done] (max island area after flipping one 0 to 1)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

// Four orthogonal neighbor offsets (Right, Left, Down, Up).
constexpr std::array<std::pair<int, int>, 4> kDirs4 = {
    {{0, 1}, {0, -1}, {1, 0}, {-1, 0}}};

class Solution {
 public:
  // Returns the area of the largest 4-connected island of `1`s in `grid`.
  // Time: O(m * n) | Space: O(m * n) worst-case recursion stack.
  int maxAreaOfIsland(std::vector<std::vector<int>>& grid) {
    if (grid.empty() || grid[0].empty()) {
      return 0;
    }

    int max_area = 0;
    for (std::size_t r = 0uz; r < grid.size(); ++r) {
      for (std::size_t c = 0uz; c < grid[0].size(); ++c) {
        if (grid[r][c] == 1) {
          max_area = std::max(
              max_area,
              dfsArea(grid, static_cast<int>(r), static_cast<int>(c)));
        }
      }
    }
    return max_area;
  }

 private:
  // Flood-fills the connected component containing `(r, c)`, zeroing each
  // visited land cell and returning the component's total cell count.
  static int dfsArea(std::vector<std::vector<int>>& grid, int r, int c) {
    if (r < 0 || std::cmp_greater_equal(r, grid.size()) || c < 0 ||
        std::cmp_greater_equal(c, grid[0].size())) {
      return 0;
    }

    auto& cell =
        grid[static_cast<std::size_t>(r)][static_cast<std::size_t>(c)];
    if (cell == 0) {
      return 0;
    }

    // Sink this land cell so it is never double-counted by later calls.
    cell = 0;
    int area = 1;
    for (const auto& [dr, dc] : kDirs4) {
      area += dfsArea(grid, r + dr, c + dc);
    }
    return area;
  }
};

LC_PROBLEM(695, "Max Area Of Island", "dfs", ctx) {
  ctx.test("example_1_from_python_build", [&] {
    std::vector<std::vector<int>> grid = {
        {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},
        {0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0},
        {0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    };
    Solution sol;
    ctx.expect_eq(sol.maxAreaOfIsland(grid), 6);
  });

  ctx.test("example_2_no_island", [&] {
    std::vector<std::vector<int>> grid = {{0, 0, 0, 0, 0, 0, 0, 0}};
    Solution sol;
    ctx.expect_eq(sol.maxAreaOfIsland(grid), 0);
  });

  ctx.test("diagonal_cells_not_connected", [&] {
    std::vector<std::vector<int>> grid = {
        {1, 0, 1},
        {0, 1, 0},
        {1, 0, 1},
    };
    Solution sol;
    ctx.expect_eq(sol.maxAreaOfIsland(grid), 1);
  });

  ctx.test("all_land_full_grid", [&] {
    std::vector<std::vector<int>> grid = {
        {1, 1, 1},
        {1, 1, 1},
        {1, 1, 1},
    };
    Solution sol;
    ctx.expect_eq(sol.maxAreaOfIsland(grid), 9);
  });

  ctx.test("single_cell_cases", [&] {
    Solution sol;
    std::vector<std::vector<int>> land = {{1}};
    ctx.expect_eq(sol.maxAreaOfIsland(land), 1);

    std::vector<std::vector<int>> water = {{0}};
    ctx.expect_eq(sol.maxAreaOfIsland(water), 0);
  });
}

}  // namespace
