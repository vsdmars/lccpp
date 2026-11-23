// LeetCode 778: Swim In Rising Water
// https://leetcode.com/problems/swim-in-rising-water/
//
// Problem:
//   You are given an n x n integer grid where `grid[r][c]` represents the
//   elevation at cell `(r, c)`. At time `t`, the water level everywhere is `t`,
//   and you can swim instantaneously between 4-directionally adjacent cells if
//   both cells have elevation <= t. Starting at `(0, 0)`, return the minimum
//   time `t` required to reach `(n - 1, n - 1)`.
//
// Algorithm:
//   Minimax Path Dijkstra / Best-First Search with a Min-Heap:
//   Finding the earliest reachable time is equivalent to finding a path from
//   `(0, 0)` to `(n - 1, n - 1)` that minimizes the maximum cell elevation
//   along the path.
//   1. Push `(grid[0][0], 0, 0)` into a min-heap ordered by elevation and mark
//      `(0, 0)` as visited.
//   2. Repeatedly pop the reachable frontier cell with the smallest elevation
//      and update `min_time = max(min_time, cell.elevation)`.
//   3. By the greedy Dijkstra/Prim cut property, the first time we pop
//      `(n - 1, n - 1)`, `min_time` is the globally optimal minimum bottleneck
//      elevation across all paths.
//   4. Otherwise, mark and push all unvisited 4-directional neighbors.
//   Time:  O(n^2 * log(n))
//   Space: O(n^2)
//
// Data Structures:
//   - Min-heap (`std::priority_queue` with `std::greater<>`) prioritizing
//     frontier cells by lowest elevation.
//   - Flat 1D visited table (`std::vector<std::uint8_t>`) indexed by
//     `(row * cols) + col` to prevent re-enqueuing cells.
//
// Real-World Applications:
//   - Network routing (widest-path / maximum-bandwidth bottleneck routing,
//     where path cost is the worst link rather than the sum of links).
//   - Robotics and autonomous vehicle path planning minimizing peak terrain
//     hazard or elevation grade.
//   - Percolation threshold analysis in physics and materials science.
//
// Related LeetCode Problems:
//   - #407 Trapping Rain Water II [done] (min-heap boundary expansion on 2D elevation grid)
//   - #1631 Path With Minimum Effort [new] (minimax Dijkstra on edge height differences)
//   - #743 Network Delay Time [new] (classic shortest-path Dijkstra with min-heap)
//   - #1102 Path With Maximum Minimum Value [done] (max-heap bottleneck path on grid)

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
  // Computes the minimum time required to swim from (0, 0) to (n - 1, n - 1)
  // via minimax Dijkstra expansion on a min-heap.
  // Time: O(n^2 * log(n)), Space: O(n^2).
  [[nodiscard]] int swimInWater(
      const std::vector<std::vector<int>>& grid) const {
    if (grid.empty() || grid[0].empty()) {
      return 0;
    }

    struct Cell {
      int elevation = 0;
      std::size_t row = 0;
      std::size_t col = 0;

      [[nodiscard]] bool operator>(const Cell& other) const noexcept {
        return elevation > other.elevation;
      }
    };

    const std::size_t rows = grid.size();
    const std::size_t cols = grid[0].size();
    std::vector<std::uint8_t> visited(rows * cols, 0);
    std::priority_queue<Cell, std::vector<Cell>, std::greater<>> min_heap;

    // Start at top-left cell; we must wait at least until grid[0][0] is
    // submerged.
    visited[0] = 1;
    min_heap.push(Cell{.elevation = grid[0][0], .row = 0uz, .col = 0uz});

    static constexpr std::array<std::pair<int, int>, 4> kDirs = {{
        {0, 1},
        {0, -1},
        {1, 0},
        {-1, 0},
    }};

    int min_time = grid[0][0];
    while (!min_heap.empty()) {
      const Cell curr = min_heap.top();
      min_heap.pop();

      // The water level must rise to at least every cell popped along the
      // minimum-bottleneck spanning frontier.
      min_time = std::max(min_time, curr.elevation);
      if (curr.row == rows - 1 && curr.col == cols - 1) {
        return min_time;
      }

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

        // Mark visited on push so each grid cell enters the heap at most once.
        visited[flat_idx] = 1;
        min_heap.push(Cell{
            .elevation = grid[unr][unc],
            .row = unr,
            .col = unc,
        });
      }
    }

    return -1;
  }
};

LC_PROBLEM(778, "Swim In Rising Water", "heap", ctx) {
  ctx.test("example_1_2x2", [&] {
    Solution sol;
    const std::vector<std::vector<int>> grid = {
        {0, 2},
        {1, 3},
    };
    ctx.expect_eq(sol.swimInWater(grid), 3);
  });

  ctx.test("example_2_5x5_spiral", [&] {
    Solution sol;
    const std::vector<std::vector<int>> grid = {
        {0, 1, 2, 3, 4},
        {24, 23, 22, 21, 5},
        {12, 13, 14, 15, 16},
        {11, 17, 18, 19, 20},
        {10, 9, 8, 7, 6},
    };
    ctx.expect_eq(sol.swimInWater(grid), 16);
  });

  ctx.test("single_cell", [&] {
    Solution sol;
    const std::vector<std::vector<int>> grid = {{7}};
    ctx.expect_eq(sol.swimInWater(grid), 7);
  });

  ctx.test("start_elevation_dominates_path", [&] {
    Solution sol;
    const std::vector<std::vector<int>> grid = {
        {5, 1, 2},
        {8, 7, 3},
        {6, 4, 0},
    };
    ctx.expect_eq(sol.swimInWater(grid), 5);
  });
}

}  // namespace
