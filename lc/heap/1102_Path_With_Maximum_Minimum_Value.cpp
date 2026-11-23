// LeetCode 1102: Path With Maximum Minimum Value
// https://leetcode.com/problems/path-with-maximum-minimum-value/
//
// Problem:
//   Given an R x C grid of non-negative integers (1 <= R, C <= 100,
//   0 <= grid[r][c] <= 10^9), find the maximum possible score of a 4-connected
//   simple path starting at (0, 0) and ending at (R - 1, C - 1), where the
//   score of a path is the minimum cell value visited along that path.
//
// Algorithm:
//   Best-First Search / Modified Dijkstra (Max-Heap):
//   1. Maintain a max-heap of frontier cells ordered by the best bottleneck
//      score achievable on a path from (0, 0) to that cell.
//   2. Start by marking (0, 0) visited and pushing `(grid[0][0], 0, 0)`.
//   3. Repeatedly pop the cell `(score, r, c)` with the highest bottleneck
//      score:
//      - If `(r, c)` is `(R - 1, C - 1)`, return `score` immediately.
//      - For each unvisited 4-directional neighbor `(nr, nc)`, mark it visited
//        and push `(min(score, grid[nr][nc]), nr, nc)`.
//   Invariant: Cells are popped in non-increasing order of path bottleneck
//   score. Therefore the very first time any cell `(nr, nc)` is discovered from
//   a popped neighbor `p` with score `S`, every subsequently popped cell has
//   score `S' <= S`, so `min(S', grid[nr][nc]) <= min(S, grid[nr][nc])`. Hence
//   marking visited upon push is optimal and avoids redundant heap entries.
//   Time Complexity:  O(R * C * log(R * C)), since each cell is pushed and
//                     popped at most once.
//   Space Complexity: O(R * C) for the visited matrix and max-heap.
//
// Data Structures:
//   - Max-heap (`std::priority_queue<CellState>`): extracts the frontier cell
//     with the highest bottleneck score in O(log(R * C)) time.
//   - Flat visited bitmap (`std::vector<std::uint8_t>`): O(1) check preventing
//     cycles without mutating the caller's input grid.
//   - Fixed `std::array` of 4 cardinal direction deltas.
//
// Real-World Applications:
//   - Widest-path (maximum-bottleneck bandwidth) routing in computer networks
//     and software-defined WANs.
//   - Heavy-haul logistics and bridge-load-limit routing (maximizing the
//     minimum weight capacity along a road route).
//   - Terrain navigation and autonomous vehicle path planning avoiding low
//     clearance or low-traction cells.
//
// Related LeetCode Problems:
//   - #1631 Path With Minimum Effort [new] (minimax path on a 2D grid via Dijkstra)
//   - #778 Swim in Rising Water [done] (minimax bottleneck path using a min-heap)
//   - #743 Network Delay Time [new] (standard Dijkstra shortest path with heap)
//   - #1514 Path with Maximum Probability [new] (multiplicative max-heap Dijkstra)
//   - #2812 Find the Safest Path in a Grid [new] (multi-source BFS + max-min bottleneck path)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <queue>
#include <utility>
#include <vector>

namespace {

// Four cardinal grid directions: right, left, down, up.
constexpr std::array<std::pair<int, int>, 4> kDirs = {{
    {0, 1},
    {0, -1},
    {1, 0},
    {-1, 0},
}};

// Frontier entry in the max-heap; ordered primarily by `min_val` descending.
struct CellState {
  int min_val = 0;
  std::size_t row = 0uz;
  std::size_t col = 0uz;

  auto operator<=>(const CellState&) const = default;
};

class Solution {
 public:
  // Returns the maximum possible minimum cell value along any 4-connected path
  // from top-left (0, 0) to bottom-right (rows - 1, cols - 1).
  //
  // Time Complexity:  O(R * C * log(R * C))
  // Space Complexity: O(R * C)
  [[nodiscard]] int maximumMinimumPath(
      const std::vector<std::vector<int>>& grid) {
    if (grid.empty() || grid[0].empty()) {
      return 0;
    }

    const std::size_t rows = grid.size();
    const std::size_t cols = grid[0].size();
    const std::size_t target_r = rows - 1uz;
    const std::size_t target_c = cols - 1uz;

    // Flat visited array avoids mutating the input grid and improves cache
    // locality over a 2D vector of vectors.
    std::vector<std::uint8_t> visited(rows * cols, 0U);
    std::priority_queue<CellState> max_heap;

    visited[0uz] = 1U;
    max_heap.push(CellState{.min_val = grid[0][0], .row = 0uz, .col = 0uz});

    while (!max_heap.empty()) {
      const CellState curr = max_heap.top();
      max_heap.pop();

      // Because we expand cells in non-increasing order of bottleneck score,
      // the first pop of the destination is guaranteed to be optimal.
      if (curr.row == target_r && curr.col == target_c) {
        return curr.min_val;
      }

      for (const auto& [dr, dc] : kDirs) {
        const int next_r_signed = static_cast<int>(curr.row) + dr;
        const int next_c_signed = static_cast<int>(curr.col) + dc;
        if (next_r_signed < 0 || next_c_signed < 0) {
          continue;
        }

        const auto nr = static_cast<std::size_t>(next_r_signed);
        const auto nc = static_cast<std::size_t>(next_c_signed);
        if (nr >= rows || nc >= cols) {
          continue;
        }

        const std::size_t flat_idx = (nr * cols) + nc;
        if (visited[flat_idx] != 0U) {
          continue;
        }

        // First discovery from the max-heap already yields the highest
        // achievable bottleneck score for (nr, nc).
        visited[flat_idx] = 1U;
        const int next_score = std::min(curr.min_val, grid[nr][nc]);
        max_heap.push(CellState{.min_val = next_score, .row = nr, .col = nc});
      }
    }

    return 0;
  }
};

LC_PROBLEM(1102, "Path With Maximum Minimum Value", "heap", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    const std::vector<std::vector<int>> grid = {
        {5, 4, 5},
        {1, 2, 6},
        {7, 4, 6},
    };
    ctx.expect_eq(sol.maximumMinimumPath(grid), 4);
  });

  ctx.test("example_2", [&] {
    Solution sol;
    const std::vector<std::vector<int>> grid = {
        {2, 2, 1, 2, 2, 2},
        {1, 2, 2, 2, 1, 2},
    };
    ctx.expect_eq(sol.maximumMinimumPath(grid), 2);
  });

  ctx.test("example_3", [&] {
    Solution sol;
    const std::vector<std::vector<int>> grid = {
        {3, 4, 6, 3, 4},
        {0, 2, 1, 1, 7},
        {8, 8, 3, 2, 7},
        {3, 2, 4, 9, 8},
        {4, 1, 2, 0, 0},
        {4, 6, 5, 4, 3},
    };
    ctx.expect_eq(sol.maximumMinimumPath(grid), 3);
  });

  ctx.test("single_cell_and_zero_bottleneck", [&] {
    Solution sol;
    ctx.expect_eq(sol.maximumMinimumPath({{42}}), 42);
    ctx.expect_eq(sol.maximumMinimumPath({{5, 0}, {0, 5}}), 0);
  });

  ctx.test("large_values_and_backtracking_detour", [&] {
    Solution sol;
    // Optimal path snakes up and around the 1s to keep bottleneck at 10^9.
    constexpr int kHigh = 1'000'000'000;
    const std::vector<std::vector<int>> grid = {
        {kHigh, 1, kHigh, kHigh, kHigh},
        {kHigh, 1, kHigh, 1, kHigh},
        {kHigh, kHigh, kHigh, 1, kHigh},
    };
    ctx.expect_eq(sol.maximumMinimumPath(grid), kHigh);
  });
}

}  // namespace
