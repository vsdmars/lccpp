// LeetCode 1091: Shortest Path In Binary Matrix
// https://leetcode.com/problems/shortest-path-in-binary-matrix/
//
// Problem:
//   Given an n x n binary matrix `grid`, return the length of the shortest
//   clear path from the top-left cell `(0, 0)` to the bottom-right cell
//   `(n - 1, n - 1)`, or -1 if no such path exists. A clear path visits only
//   cells with value 0, moves 8-directionally (orthogonal or diagonal), and
//   its length is the total number of visited cells along the path (including
//   both endpoints).
//   Constraints: 1 <= n <= 100, grid[i][j] in {0, 1}.
//
// Examples:
//   Example 1:
//     Input:  grid = [[0, 1], [1, 0]]
//     Output: 2
//     Explanation:
//       - Confirm both start (0, 0) and target (1, 1) equal 0, then enqueue (0, 0,
//         dist=1) and mark grid[0][0] = 1.
//       - Pop (0, 0, 1) and inspect its 8 neighbors in kDirs8: (0, 1) and (1, 0)
//         are blocked by 1, while diagonal neighbor (1, 1) is 0.
//       - Mark grid[1][1] = 1, enqueue (1, 1, 2), and return 2 immediately when (1,
//         1, 2) is popped as the target cell.
//
//   Example 2:
//     Input:  grid = [[0, 0, 0], [1, 1, 0], [1, 1, 0]]
//     Output: 4
//     Explanation:
//       - Enqueue (0, 0, dist=1) and set grid[0][0] = 1 since both (0, 0) and
//         target (2, 2) are open (0).
//       - Pop (0, 0, 1): only (0, 1) is open, so mark grid[0][1] = 1 and push (0,
//         1, 2).
//       - Pop (0, 1, 2): mark its two open neighbors (0, 2) and (1, 2) as 1 and
//         push (0, 2, 3) and (1, 2, 3).
//       - Pop (0, 2, 3) with no remaining open neighbors, then pop (1, 2, 3), mark
//         (2, 2) = 1, push (2, 2, 4), and return 4 when (2, 2, 4) is popped.
//
//   Example 3:
//     Input:  grid = [[0, 1, 0], [0, 1, 0], [0, 1, 0]]
//     Output: -1
//     Explanation:
//       - Confirm endpoints (0, 0) and (2, 2) are 0, enqueue (0, 0, dist=1), and
//         mark grid[0][0] = 1.
//       - Pop (0, 0, 1) and push its only open neighbor (1, 0, 2), then pop (1, 0,
//         2) and push (2, 0, 3).
//       - Pop (2, 0, 3): column 1 is entirely 1s, leaving no unvisited open
//         neighbors, so the frontier empties and the function returns -1.
//
// Algorithm:
//   Single-source unweighted shortest-path BFS on an 8-connected grid:
//   1. Guard: If the grid is empty or either endpoint `(0, 0)` or
//      `(n - 1, n - 1)` is blocked (value != 0), no clear path exists -> -1.
//   2. BFS Exploration: Enqueue `(0, 0)` with path length `1` and immediately
//      mark `grid[0][0] = 1` so it is never revisited. Because every move has
//      uniform cost 1, FIFO BFS explores cells in non-decreasing distance
//      order; the first time `(n - 1, n - 1)` is popped, its recorded length is
//      guaranteed to be minimal.
//   3. For each popped cell, inspect all 8 neighboring offsets. Any in-bounds
//      cell with value 0 is marked 1 immediately upon enqueueing (critical to
//      prevent exponential duplicate queueing) with `dist + 1`.
//   Time Complexity:  O(n^2) — every cell is enqueued and expanded at most
//                     once, inspecting 8 neighbors in O(1).
//   Space Complexity: O(n^2) — worst-case BFS queue size.
//
// Data Structures:
//   - FIFO queue (`std::queue<Step>`): maintains frontier cells in exact
//     non-decreasing path-length order.
//   - 2D grid (`std::vector<std::vector<int>>`): reused in-place as the
//     visited matrix by flipping visited `0` cells to `1`.
//   - Compile-time direction table (`std::array<std::pair<int, int>, 8>`):
//     encodes all 8 orthogonal and diagonal step offsets cleanly.
//
// Real-World Applications:
//   - Autonomous robotics & game AI pathfinding: computing shortest collision-
//     free trajectories on 8-way occupancy grids (octile movement).
//   - Warehouse AGV routing: finding minimum-step routes between loading docks
//     and drop-off bays across obstacle-mapped floor grids.
//   - Image processing & maze solving: computing Chebyshev/8-connected geodesic
//     shortest paths through binary masks.
//
// Related LeetCode Problems:
//   - #994 Rotting Oranges [done] (4-directional wave BFS on a 2D grid)
//   - #542 01 Matrix [done] (shortest grid distances via multi-source BFS)
//   - #1293 Shortest Path in a Grid with Obstacles Elimination [new] (grid BFS with an extra remaining-eliminations state)
//   - #1730 Shortest Path to Get Food [new] (single-source shortest path on an obstacle grid)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <queue>
#include <utility>
#include <vector>

namespace {

// All 8 orthogonal and diagonal neighbor offsets.
constexpr std::array<std::pair<int, int>, 8> kDirs8 = {
    {{0, 1}, {0, -1}, {1, 0}, {-1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}},
};

struct Step {
  int row = 0;
  int col = 0;
  int dist = 0;

  Step(int r, int c, int d) : row(r), col(c), dist(d) {}
};

class Solution {
 public:
  // Returns the shortest 8-directional clear path length (in visited cells)
  // from `(0, 0)` to `(rows - 1, cols - 1)`, or -1 if none exists.
  // Time: O(R * C), Space: O(R * C).
  int shortestPathBinaryMatrix(std::vector<std::vector<int>>& grid) {
    if (grid.empty() || grid[0].empty()) {
      return -1;
    }

    const int rows = static_cast<int>(grid.size());
    const int cols = static_cast<int>(grid[0].size());
    const int target_r = rows - 1;
    const int target_c = cols - 1;

    // Both start and goal must be open (0) for any clear path to exist.
    if (grid[0][0] != 0 ||
        grid[static_cast<std::size_t>(target_r)]
            [static_cast<std::size_t>(target_c)] != 0) {
      return -1;
    }

    std::queue<Step> frontier;
    frontier.emplace(0, 0, 1);
    grid[0][0] = 1;  // Mark visited immediately on enqueue.

    while (!frontier.empty()) {
      const Step cur = frontier.front();
      frontier.pop();

      if (cur.row == target_r && cur.col == target_c) {
        return cur.dist;
      }

      for (const auto [dr, dc] : kDirs8) {
        const int nr = cur.row + dr;
        const int nc = cur.col + dc;
        if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) {
          continue;
        }

        auto& cell =
            grid[static_cast<std::size_t>(nr)][static_cast<std::size_t>(nc)];
        if (cell == 0) {
          // Flip to 1 before pushing so no other neighbor enqueues `(nr, nc)`.
          cell = 1;
          frontier.emplace(nr, nc, cur.dist + 1);
        }
      }
    }

    return -1;
  }
};

LC_PROBLEM(1091, "Shortest Path In Binary Matrix", "bfs", ctx) {
  Solution sol;

  ctx.test("example1_diagonal_two_by_two", [&] {
    std::vector<std::vector<int>> grid = {{0, 1}, {1, 0}};
    ctx.expect_eq(sol.shortestPathBinaryMatrix(grid), 2);
  });

  ctx.test("example2_detour_around_obstacles", [&] {
    std::vector<std::vector<int>> grid = {
        {0, 0, 0},
        {1, 1, 0},
        {1, 1, 0},
    };
    ctx.expect_eq(sol.shortestPathBinaryMatrix(grid), 4);
  });

  ctx.test("example3_blocked_start", [&] {
    std::vector<std::vector<int>> grid = {
        {1, 0, 0},
        {1, 1, 0},
        {1, 1, 0},
    };
    ctx.expect_eq(sol.shortestPathBinaryMatrix(grid), -1);
  });

  ctx.test("diagonal_straight_through", [&] {
    // From Python reference build(): (0,0) -> (1,1) -> (2,2) has length 3.
    std::vector<std::vector<int>> grid = {
        {0, 0, 0},
        {1, 0, 0},
        {1, 1, 0},
    };
    ctx.expect_eq(sol.shortestPathBinaryMatrix(grid), 3);
  });

  ctx.test("vertical_wall_blocks_all_paths", [&] {
    std::vector<std::vector<int>> grid = {
        {0, 1, 0},
        {0, 1, 0},
        {0, 1, 0},
    };
    ctx.expect_eq(sol.shortestPathBinaryMatrix(grid), -1);
  });

  ctx.test("single_cell_open_and_blocked", [&] {
    std::vector<std::vector<int>> open_grid = {{0}};
    std::vector<std::vector<int>> blocked_grid = {{1}};
    ctx.expect_eq(sol.shortestPathBinaryMatrix(open_grid), 1);
    ctx.expect_eq(sol.shortestPathBinaryMatrix(blocked_grid), -1);
  });
}

}  // namespace
