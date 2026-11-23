// LeetCode 407: Trapping Rain Water II
// https://leetcode.com/problems/trapping-rain-water-ii/
//
// Problem:
//   Given an m x n integer matrix `heightMap` representing the height of each
//   unit cell in a 2D elevation map, return the total volume of rainwater that
//   can be trapped after raining. Water flows 4-directionally and leaks off
//   the outer boundary of the grid.
//
// Examples:
//   Example 1:
//     Input:  heightMap = [[1, 4, 3, 1, 3, 2], [3, 2, 1, 3, 2, 4], [2, 3, 3, 2, 3,
//             1]]
//     Output: 4
//     Explanation:
//       - seed_boundary marks all 14 outer perimeter cells visited and pushes them
//         into the min-heap.
//       - Perimeter cells of height 1 and 2 pop first; only (1, 3) = 3 is
//         discovered from (1, 0, 3) with 0 trapped water and pushed as (3, 1, 3).
//       - When (3, 0, 2) pops, it visits (1, 2) = 1, traps 3 - 1 = 2 units of
//         water, and pushes effective wall (3, 1, 2).
//       - Expanding (3, 1, 2) visits (1, 1) = 2 (traps 3 - 2 = 1) and expanding (3,
//         1, 3) visits (1, 4) = 2 (traps 3 - 2 = 1), giving total trapped water 2 +
//         1 + 1 = 4.
//
//   Example 2:
//     Input:  heightMap = [[9, 9, 9, 9], [9, 1, 2, 5], [9, 9, 9, 9]]
//     Output: 7
//     Explanation:
//       - seed_boundary inserts all 10 perimeter cells: one cell (1, 3) at height 5
//         and nine cells at height 9.
//       - The lowest boundary cell (5, 1, 3) pops first and visits (1, 2) = 2,
//         trapping 5 - 2 = 3 units and pushing (5, 1, 2).
//       - Popping (5, 1, 2) visits (1, 1) = 1, trapping 5 - 1 = 4 units and pushing
//         (5, 1, 1).
//       - All interior cells are now visited, so remaining pops find no unvisited
//         neighbors and return 3 + 4 = 7.
//
//   Example 3:
//     Input:  heightMap = [[5, 1, 5], [5, 1, 5]]
//     Output: 0
//     Explanation:
//       - Check grid dimensions: height_map.size() is 2, which is less than 3.
//       - The early-return guard triggers immediately because every cell lies on
//         the outer boundary, returning 0.
//
// Algorithm:
//   Multi-source Best-First Search (Priority-Flood / Dijkstra-style boundary
//   contraction) using a min-heap:
//   1. No cell on the outer perimeter can hold water because it drains off the
//      grid. Insert all perimeter cells into a min-heap ordered by height and
//      mark them visited.
//   2. Repeatedly pop the cell with the minimum effective boundary height `h`.
//      By the min-heap invariant, `h` is the lowest spill-point height of the
//      entire unvisited interior's enclosing wall.
//   3. For each unvisited 4-directional neighbor `(nr, nc)` with terrain height
//      `nh`, it can trap `max(0, h - nh)` units of water. Push it into the
//      min-heap with effective wall height `max(h, nh)` (either water fills it
//      up to `h` or its own taller terrain becomes a higher retaining wall)
//      and mark it visited.
//   Time:  O(m * n * log(m * n))
//   Space: O(m * n)
//
// Data Structures:
//   - Min-heap (`std::priority_queue` with `std::greater<>`) storing boundary
//     cells `(effective_height, row, col)` to always expand from the lowest
//     spill point.
//   - Flat 1D visited array (`std::vector<std::uint8_t>`) indexed by
//     `(row * cols) + col` to ensure each cell is processed exactly once.
//
// Real-World Applications:
//   - GIS / hydrological terrain modeling (Priority-Flood depression filling
//     for digital elevation models and watershed delineation).
//   - Flood risk simulation and urban stormwater drainage capacity estimation.
//   - Computer vision watershed segmentation on grayscale height surfaces.
//
// Related LeetCode Problems:
//   - #42 Trapping Rain Water [done] (1D two-pointer / monotonic stack version)
//   - #778 Swim in Rising Water [done] (min-heap minimax path on 2D elevation grid)
//   - #11 Container With Most Water [done] (two-pointer water volume maximization)
//   - #1631 Path With Minimum Effort [new] (Dijkstra over 2D grid elevations)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <queue>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the total volume of water trapped in a 2D height map by shrinking
  // the enclosing boundary inward from the lowest spill point.
  // Time: O(m * n * log(m * n)), Space: O(m * n).
  int trapRainWater(const std::vector<std::vector<int>>& height_map) {
    // Grids narrower than 3 in either dimension have no interior cells.
    if (height_map.size() < 3 || height_map[0].size() < 3) {
      return 0;
    }

    const std::size_t rows = height_map.size();
    const std::size_t cols = height_map[0].size();
    std::vector<std::uint8_t> visited(rows * cols, 0);
    MinHeap boundary = seed_boundary(height_map, visited);

    static constexpr std::array<std::pair<int, int>, 4> kDirs = {{
        {1, 0},
        {-1, 0},
        {0, 1},
        {0, -1},
    }};

    int trapped_water = 0;
    while (!boundary.empty()) {
      const Cell curr = boundary.top();
      boundary.pop();

      for (const auto& [dr, dc] : kDirs) {
        const int nr = static_cast<int>(curr.row) + dr;
        const int nc = static_cast<int>(curr.col) + dc;
        if (nr < 0 || nc < 0 || std::cmp_greater_equal(nr, rows) ||
            std::cmp_greater_equal(nc, cols)) {
          continue;
        }

        const auto unr = static_cast<std::size_t>(nr);
        const auto unc = static_cast<std::size_t>(nc);
        const std::size_t flat_idx = (unr * cols) + unc;
        if (visited[flat_idx] != 0) {
          continue;
        }
        visited[flat_idx] = 1;

        const int neighbor_height = height_map[unr][unc];
        // Any deficit below the current minimum boundary height traps water;
        // once filled, this cell acts as a new boundary wall at max height.
        if (curr.height > neighbor_height) {
          trapped_water += curr.height - neighbor_height;
        }
        boundary.push(Cell{
            .height = std::max(curr.height, neighbor_height),
            .row = unr,
            .col = unc,
        });
      }
    }

    return trapped_water;
  }

 private:
  struct Cell {
    int height = 0;
    std::size_t row = 0;
    std::size_t col = 0;

    [[nodiscard]] bool operator>(const Cell& other) const noexcept {
      return height > other.height;
    }
  };

  using MinHeap = std::priority_queue<Cell, std::vector<Cell>, std::greater<>>;

  // Pushes all outer perimeter cells into the min-heap and marks them visited.
  [[nodiscard]] static MinHeap seed_boundary(
      const std::vector<std::vector<int>>& height_map,
      std::vector<std::uint8_t>& visited) {
    const std::size_t rows = height_map.size();
    const std::size_t cols = height_map[0].size();
    std::vector<Cell> initial;
    initial.reserve((2uz * rows) + (2uz * cols));

    auto add_cell = [&](std::size_t r, std::size_t c) {
      const std::size_t flat_idx = (r * cols) + c;
      if (visited[flat_idx] == 0) {
        visited[flat_idx] = 1;
        initial.push_back(Cell{.height = height_map[r][c], .row = r, .col = c});
      }
    };

    for (std::size_t c = 0uz; c < cols; ++c) {
      add_cell(0uz, c);
      add_cell(rows - 1, c);
    }
    for (std::size_t r = 1uz; r + 1 < rows; ++r) {
      add_cell(r, 0uz);
      add_cell(r, cols - 1);
    }

    return MinHeap(std::greater<>{}, std::move(initial));
  }
};

LC_PROBLEM(407, "Trapping Rain Water II", "heap", ctx) {
  ctx.test("example_1_3x6_map", [&] {
    Solution sol;
    const std::vector<std::vector<int>> height_map = {
        {1, 4, 3, 1, 3, 2},
        {3, 2, 1, 3, 2, 4},
        {2, 3, 3, 2, 3, 1},
    };
    ctx.expect_eq(sol.trapRainWater(height_map), 4);
  });

  ctx.test("example_2_concentric_basin", [&] {
    Solution sol;
    const std::vector<std::vector<int>> height_map = {
        {3, 3, 3, 3, 3},
        {3, 2, 2, 2, 3},
        {3, 2, 1, 2, 3},
        {3, 2, 2, 2, 3},
        {3, 3, 3, 3, 3},
    };
    ctx.expect_eq(sol.trapRainWater(height_map), 10);
  });

  ctx.test("too_small_grids_trap_nothing", [&] {
    Solution sol;
    ctx.expect_eq(sol.trapRainWater({}), 0);
    ctx.expect_eq(sol.trapRainWater({{5, 1, 5}, {5, 1, 5}}), 0);
    ctx.expect_eq(sol.trapRainWater({{5, 5}, {5, 1}, {5, 5}}), 0);
  });

  ctx.test("leaky_rim_limits_water_level", [&] {
    Solution sol;
    const std::vector<std::vector<int>> height_map = {
        {9, 9, 9, 9},
        {9, 1, 2, 5},
        {9, 9, 9, 9},
    };
    // Lowest rim cell is 5 -> cell (1,1) traps 4, cell (1,2) traps 3 => 7.
    ctx.expect_eq(sol.trapRainWater(height_map), 7);
  });
}

}  // namespace
