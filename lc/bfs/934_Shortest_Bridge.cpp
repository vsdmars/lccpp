// LeetCode 934: Shortest Bridge
// https://leetcode.com/problems/shortest-bridge/
//
// Problem:
//   Given an n x n binary matrix `grid` containing exactly two 4-directionally
//   connected islands (groups of 1s separated by 0s), you may flip 0s to 1s to
//   connect the two islands into a single island. Return the minimum number of
//   0 cells that must be flipped.
//   Constraints: 2 <= n <= 100, grid[i][j] in {0, 1}, exactly two islands.
//
// Examples:
//   Example 1:
//     Input:  grid = [[0, 1], [1, 0]]
//     Output: 1
//     Explanation:
//       - `findFirstLand` scans row-major and finds the first `1` at `(0, 1)`.
//       - `collectFirstIsland` marks `(0, 1)` as `2` and seeds `frontier` with `(0,
//         1)`.
//       - At `flips = 0`, expanding `(0, 1)` marks water neighbors `(0, 0)` and
//         `(1, 1)` as `2`, enqueues them, and increments `flips` to `1`.
//       - At `flips = 1`, popping `(0, 0)` inspects neighbor `(1, 0)` which holds
//         `1`, returning `1`.
//
//   Example 2:
//     Input:  grid = [[0, 1, 0], [0, 0, 0], [0, 0, 1]]
//     Output: 2
//     Explanation:
//       - `findFirstLand` locates `(0, 1)` and `collectFirstIsland` marks it `2`,
//         initializing `frontier = [(0, 1)]`.
//       - At `flips = 0`, `(0, 1)` marks water cells `(1, 1)`, `(0, 2)`, and `(0,
//         0)` as `2` and enqueues them before incrementing `flips` to `1`.
//       - At `flips = 1`, the wave expands to water cells `(2, 1)`, `(1, 0)`, and
//         `(1, 2)`, marking them `2` and incrementing `flips` to `2`.
//       - At `flips = 2`, popping `(2, 1)` sees `grid[2][2] == 1` (the second
//         island), immediately returning `2`.
//
//   Example 3:
//     Input:  grid = [[1, 1, 1, 1, 1], [1, 0, 0, 0, 1], [1, 0, 1, 0, 1], [1, 0, 0,
//             0, 1], [1, 1, 1, 1, 1]]
//     Output: 1
//     Explanation:
//       - `findFirstLand` finds `(0, 0)`, and `collectFirstIsland` marks all 16
//         outer-ring `1`s as `2` and places them into `frontier` while leaving the
//         center `(2, 2)` as `1`.
//       - At `flips = 0`, expanding the 16 ring cells marks the 8 inner water cells
//         around `(2, 2)` as `2`, enqueues them, and increments `flips` to `1`.
//       - At `flips = 1`, popping `(1, 2)` reaches neighbor `(2, 2)` with value
//         `1`, returning `1`.
//
// Algorithm:
//   Two-stage BFS (Component Discovery + Multi-Source Shortest-Path Expansion):
//   1. Discover First Island: Scan the grid for the first land cell (value 1)
//      and run a BFS to find all cells belonging to this island. Mark every
//      visited cell with value 2 in-place and record all of its coordinates as
//      the initial multi-source BFS frontier.
//   2. Multi-Source Wave Expansion: Expand outward from the entire first island
//      simultaneously, one ring (distance layer) at a time. Any 4-directional
//      neighbor that is water (0) is marked 2 and queued for the next layer.
//      The very first time a neighbor with value 1 is encountered, we have
//      reached the untouched second island, so the current number of expanded
//      water layers equals the exact minimum bridge length.
//   Time Complexity:  O(n^2) — every cell is marked and expanded at most once.
//   Space Complexity: O(n^2) — worst-case BFS queue storing island/water cells.
//
// Data Structures:
//   - 2D grid (`std::vector<std::vector<int>>`): mutated in-place (value 2 =
//     visited first island or already-expanded water) to avoid a separate
//     visited matrix.
//   - FIFO queue (`std::queue<std::pair<int, int>>`): drives both the initial
//     island discovery BFS and the subsequent multi-source wave expansion.
//
// Real-World Applications:
//   - Infrastructure & utility engineering: finding the shortest bridge, cable
//     trench, or pipeline corridor linking two disconnected service zones on a
//     rasterized terrain map.
//   - Computer vision & medical imaging: computing the minimum pixel gap
//     between two segmented regions or anatomical structures.
//   - VLSI physical design: estimating minimum Manhattan wirelength required to
//     connect two disconnected routing nets on a grid.
//
// Related LeetCode Problems:
//   - #200 Number of Islands [done] (4-directional island flood fill on a binary grid)
//   - #542 01 Matrix [done] (multi-source BFS shortest distance on a 2D grid)
//   - #827 Making A Large Island [done] (labeling grid islands and bridging them via water cells)
//   - #1162 As Far from Land as Possible [new] (multi-source BFS expanding outward from all land cells)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <queue>
#include <utility>
#include <vector>

namespace {

// Four orthogonal directions: down, up, right, left.
constexpr std::array<std::pair<int, int>, 4> kDirs = {
    {{1, 0}, {-1, 0}, {0, 1}, {0, -1}},
};

class Solution {
 public:
  // Returns the minimum number of 0 cells to flip to connect the two islands
  // in `grid`.
  // Time: O(R * C), Space: O(R * C).
  int shortestBridge(std::vector<std::vector<int>>& grid) {
    if (grid.empty() || grid[0].empty()) {
      return 0;
    }

    const int rows = static_cast<int>(grid.size());
    const int cols = static_cast<int>(grid[0].size());

    // Phase 1: Locate any land cell of the first island and BFS-collect the
    // entire island into the multi-source expansion queue.
    const auto [start_r, start_c] = findFirstLand(grid);
    std::queue<std::pair<int, int>> frontier =
        collectFirstIsland(grid, start_r, start_c);

    // Phase 2: Expand outward from the first island one water layer per step.
    // `flips` equals the number of water cells crossed before touching any
    // remaining `1` (which belongs to the second island).
    int flips = 0;
    while (!frontier.empty()) {
      const std::size_t layer_size = frontier.size();
      for (std::size_t i = 0uz; i < layer_size; ++i) {
        const auto [r, c] = frontier.front();
        frontier.pop();

        for (const auto [dr, dc] : kDirs) {
          const int nr = r + dr;
          const int nc = c + dc;
          if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) {
            continue;
          }

          auto& cell =
              grid[static_cast<std::size_t>(nr)][static_cast<std::size_t>(nc)];
          if (cell == 1) {
            return flips;
          }
          if (cell == 0) {
            // Mark water as visited (2) immediately upon enqueueing.
            cell = 2;
            frontier.emplace(nr, nc);
          }
        }
      }
      ++flips;
    }

    return -1;
  }

 private:
  // Scans `grid` in row-major order and returns the coordinates of the first
  // land cell (value 1).
  static std::pair<int, int> findFirstLand(
      const std::vector<std::vector<int>>& grid) {
    const int rows = static_cast<int>(grid.size());
    const int cols = static_cast<int>(grid[0].size());
    for (int r = 0; r < rows; ++r) {
      for (int c = 0; c < cols; ++c) {
        if (grid[static_cast<std::size_t>(r)][static_cast<std::size_t>(c)] ==
            1) {
          return {r, c};
        }
      }
    }
    return {-1, -1};
  }

  // Flood-fills the island containing `(start_r, start_c)`, marks all of its
  // cells as 2 in-place, and returns a queue containing every cell in that
  // island for subsequent multi-source BFS.
  static std::queue<std::pair<int, int>> collectFirstIsland(
      std::vector<std::vector<int>>& grid, int start_r, int start_c) {
    const int rows = static_cast<int>(grid.size());
    const int cols = static_cast<int>(grid[0].size());

    std::queue<std::pair<int, int>> discover;
    std::queue<std::pair<int, int>> island_cells;
    if (start_r < 0 || start_c < 0) {
      return island_cells;
    }

    grid[static_cast<std::size_t>(start_r)]
        [static_cast<std::size_t>(start_c)] = 2;
    discover.emplace(start_r, start_c);
    island_cells.emplace(start_r, start_c);

    while (!discover.empty()) {
      const auto [r, c] = discover.front();
      discover.pop();

      for (const auto [dr, dc] : kDirs) {
        const int nr = r + dr;
        const int nc = c + dc;
        if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) {
          continue;
        }
        auto& cell =
            grid[static_cast<std::size_t>(nr)][static_cast<std::size_t>(nc)];
        if (cell == 1) {
          cell = 2;
          discover.emplace(nr, nc);
          island_cells.emplace(nr, nc);
        }
      }
    }

    return island_cells;
  }
};

LC_PROBLEM(934, "Shortest Bridge", "bfs", ctx) {
  Solution sol;

  ctx.test("example1_diagonal_two_by_two", [&] {
    std::vector<std::vector<int>> grid = {{0, 1}, {1, 0}};
    ctx.expect_eq(sol.shortestBridge(grid), 1);
  });

  ctx.test("example2_three_by_three_gap_2", [&] {
    std::vector<std::vector<int>> grid = {
        {0, 1, 0},
        {0, 0, 0},
        {0, 0, 1},
    };
    ctx.expect_eq(sol.shortestBridge(grid), 2);
  });

  ctx.test("example3_surrounding_ring_island", [&] {
    std::vector<std::vector<int>> grid = {
        {1, 1, 1, 1, 1}, {1, 0, 0, 0, 1}, {1, 0, 1, 0, 1},
        {1, 0, 0, 0, 1}, {1, 1, 1, 1, 1},
    };
    ctx.expect_eq(sol.shortestBridge(grid), 1);
  });

  ctx.test("opposite_corners_of_four_by_four", [&] {
    // Manhattan distance between (0, 0) and (3, 3) is 6, so 5 water cells sit
    // strictly between the two single-cell islands.
    std::vector<std::vector<int>> grid = {
        {1, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 1},
    };
    ctx.expect_eq(sol.shortestBridge(grid), 5);
  });
}

}  // namespace
