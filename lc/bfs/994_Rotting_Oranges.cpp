// LeetCode 994: Rotting Oranges
// https://leetcode.com/problems/rotting-oranges/
//
// Problem:
//   Given an m x n grid where 0 = empty cell, 1 = fresh orange, and 2 = rotten
//   orange, every minute any fresh orange 4-directionally adjacent to a rotten
//   orange becomes rotten. Return the minimum number of minutes that must
//   elapse until no fresh oranges remain, or -1 if at least one fresh orange
//   can never become rotten.
//   Constraints: 1 <= m, n <= 10, grid[i][j] in {0, 1, 2}.
//
// Examples:
//   Example 1:
//     Input:  grid = [[2, 1, 1], [1, 1, 0], [0, 1, 1]]
//     Output: 4
//     Explanation:
//       - The initial grid scan counts fresh_count = 6 and enqueues the sole rotten
//         source (0, 0) into frontier with minutes = 0.
//       - Minute 1 pops (0, 0), infects fresh neighbors (0, 1) and (1, 0) in-place,
//         decrements fresh_count to 4, and increments minutes to 1.
//       - Minute 2 drains [(0, 1), (1, 0)], infecting (0, 2) and (1, 1)
//         (fresh_count = 2, minutes = 2), and minute 3 spreads from (1, 1) to (2,
//         1) (fresh_count = 1, minutes = 3).
//       - Minute 4 spreads from (2, 1) to (2, 2), dropping fresh_count to 0 and
//         setting minutes = 4; the loop exits immediately because fresh_count == 0,
//         returning 4.
//
//   Example 2:
//     Input:  grid = [[2, 1, 1], [0, 1, 1], [1, 0, 1]]
//     Output: -1
//     Explanation:
//       - The initial scan counts fresh_count = 6 and enqueues (0, 0) into
//         frontier.
//       - Four BFS waves infect (0, 1), (1, 1), (0, 2), (1, 2), and (2, 2),
//         reducing fresh_count from 6 to 1.
//       - The remaining fresh orange at (2, 0) is blocked by empty 0 cells at (1,
//         0) and (2, 1), so frontier empties with fresh_count = 1 and
//         orangesRotting returns -1.
//
//   Example 3:
//     Input:  grid = [[0, 2]]
//     Output: 0
//     Explanation:
//       - The initial scan finds fresh_count = 0 and pushes (0, 1) into frontier.
//       - Because the loop condition checks !frontier.empty() && fresh_count > 0,
//         zero waves execute, minutes stays 0, and the function returns 0.
//
// Algorithm:
//   Multi-source level-by-level BFS (simultaneous wave propagation):
//   1. Initial Scan: Traverse the grid once to count all fresh oranges
//      (`fresh_count`) and enqueue every initially rotten orange's coordinates
//      as the minute-0 frontier.
//   2. Wave Simulation: While the queue is non-empty AND `fresh_count > 0`,
//      process all cells in the current wave (exactly `level_size` cells).
//      Each cell infects its 4-directional fresh neighbors, marking them rotten
//      (value 2) in-place, decrementing `fresh_count`, and enqueueing them for
//      the next minute. After finishing the wave, increment `minutes`.
//   3. Termination: Stopping as soon as `fresh_count == 0` avoids counting an
//      extra empty trailing minute. If `fresh_count == 0`, return `minutes`;
//      otherwise unreachable fresh oranges remain, so return -1.
//   Time Complexity:  O(m * n) — every cell is scanned once and enqueued at
//                     most once.
//   Space Complexity: O(m * n) — worst-case BFS queue size.
//
// Data Structures:
//   - FIFO queue (`std::queue<std::pair<int, int>>`): holds the active frontier
//     of newly rotten oranges for level-synchronous minute-by-minute BFS.
//   - 2D grid (`std::vector<std::vector<int>>`): updated in-place from 1 to 2
//     to serve as the visited state without extra memory.
//
// Real-World Applications:
//   - Epidemiological & wildfire spread modeling: simulating discrete-time
//     contagion or fire propagation from multiple simultaneous ignition points.
//   - Distributed cache / gossip-protocol convergence: computing the minimum
//     number of communication rounds needed for an update seeded at multiple
//     nodes to reach every connected replica.
//   - Agricultural & supply-chain quality simulation: estimating spoilage
//     propagation timelines across compartmentalized storage grids.
//
// Related LeetCode Problems:
//   - #286 Walls and Gates [done] (multi-source BFS computing shortest grid distance from all gates)
//   - #542 01 Matrix [done] (multi-source BFS shortest distance to the nearest zero)
//   - #934 Shortest Bridge [done] (wave-by-wave multi-source BFS expanding from an island)
//   - #1091 Shortest Path in Binary Matrix [done] (8-directional grid BFS shortest path)
//   - #1162 As Far from Land as Possible [new] (maximum distance wave in multi-source grid BFS)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <queue>
#include <utility>
#include <vector>

namespace {

// Four orthogonal directions: right, left, down, up.
constexpr std::array<std::pair<int, int>, 4> kDirs = {
    {{0, 1}, {0, -1}, {1, 0}, {-1, 0}},
};

class Solution {
 public:
  // Returns the minimum number of minutes until all fresh oranges rot, or -1
  // if any fresh orange is unreachable from all rotten sources.
  // Time: O(R * C), Space: O(R * C).
  int orangesRotting(std::vector<std::vector<int>>& grid) {
    if (grid.empty() || grid[0].empty()) {
      return 0;
    }

    const int rows = static_cast<int>(grid.size());
    const int cols = static_cast<int>(grid[0].size());

    // Phase 1: Seed the multi-source BFS queue with all initially rotten
    // oranges (minute 0) and count how many fresh oranges must be infected.
    std::queue<std::pair<int, int>> frontier;
    int fresh_count = 0;
    for (int r = 0; r < rows; ++r) {
      for (int c = 0; c < cols; ++c) {
        const int cell =
            grid[static_cast<std::size_t>(r)][static_cast<std::size_t>(c)];
        if (cell == 1) {
          ++fresh_count;
        } else if (cell == 2) {
          frontier.emplace(r, c);
        }
      }
    }

    // Phase 2: Propagate rot one minute (BFS layer) at a time. The loop stops
    // immediately once `fresh_count` hits 0 so no extra minute is added for
    // draining the final layer of already-rotten oranges.
    int minutes = 0;
    while (!frontier.empty() && fresh_count > 0) {
      const std::size_t wave_size = frontier.size();
      for (std::size_t i = 0uz; i < wave_size; ++i) {
        const auto [r, c] = frontier.front();
        frontier.pop();
        infectNeighbors(grid, r, c, frontier, fresh_count);
      }
      ++minutes;
    }

    return fresh_count == 0 ? minutes : -1;
  }

 private:
  // Infects all 4-directional fresh neighbors of `(r, c)`, marking them rotten
  // in-place, decrementing `fresh_count`, and enqueueing them for the next
  // minute's wave.
  static void infectNeighbors(std::vector<std::vector<int>>& grid, int r, int c,
                              std::queue<std::pair<int, int>>& frontier,
                              int& fresh_count) {
    const int rows = static_cast<int>(grid.size());
    const int cols = static_cast<int>(grid[0].size());

    for (const auto [dr, dc] : kDirs) {
      const int nr = r + dr;
      const int nc = c + dc;
      if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) {
        continue;
      }

      auto& neighbor =
          grid[static_cast<std::size_t>(nr)][static_cast<std::size_t>(nc)];
      if (neighbor == 1) {
        neighbor = 2;
        --fresh_count;
        frontier.emplace(nr, nc);
      }
    }
  }
};

LC_PROBLEM(994, "Rotting Oranges", "bfs", ctx) {
  Solution sol;

  ctx.test("example1_all_rot_in_4_minutes", [&] {
    std::vector<std::vector<int>> grid = {
        {2, 1, 1},
        {1, 1, 0},
        {0, 1, 1},
    };
    ctx.expect_eq(sol.orangesRotting(grid), 4);
  });

  ctx.test("example2_isolated_fresh_orange_returns_minus_1", [&] {
    std::vector<std::vector<int>> grid = {
        {2, 1, 1},
        {0, 1, 1},
        {1, 0, 1},
    };
    ctx.expect_eq(sol.orangesRotting(grid), -1);
  });

  ctx.test("example3_no_fresh_oranges_returns_0", [&] {
    std::vector<std::vector<int>> grid = {{0, 2}};
    ctx.expect_eq(sol.orangesRotting(grid), 0);
  });

  ctx.test("all_empty_and_no_rotten_sources", [&] {
    std::vector<std::vector<int>> empty_grid = {{0, 0}, {0, 0}};
    std::vector<std::vector<int>> fresh_only = {{1, 1}};
    ctx.expect_eq(sol.orangesRotting(empty_grid), 0);
    ctx.expect_eq(sol.orangesRotting(fresh_only), -1);
  });

  ctx.test("multiple_rotten_sources_accelerate_spread", [&] {
    // Rotten sources at opposite corners meet in the middle in 2 minutes.
    std::vector<std::vector<int>> grid = {
        {2, 1, 1},
        {1, 1, 1},
        {1, 1, 2},
    };
    ctx.expect_eq(sol.orangesRotting(grid), 2);
  });
}

}  // namespace
