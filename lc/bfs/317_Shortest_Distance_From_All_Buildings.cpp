// LeetCode 317: Shortest Distance From All Buildings
// https://leetcode.com/problems/shortest-distance-from-all-buildings/
//
// Problem:
//   Given an `m x n` grid where each cell is:
//     - `0`: passable empty land,
//     - `1`: a building you cannot pass through,
//     - `2`: an obstacle you cannot pass through,
//   find an empty land cell that minimizes the sum of 4-directional shortest
//   travel distances to ALL buildings in the grid. Return that minimum total
//   distance, or `-1` if no empty land can reach every building.
//
// Algorithm:
//   Pruned per-building Breadth-First Search:
//   Maintain two `m x n` grids: `total_dist` (cumulative shortest distance from
//   all processed buildings) and `reach_count` (how many prior buildings could
//   reach each empty cell). Process buildings sequentially with index
//   `b = 0, 1, ..., B - 1`:
//   - Launch a BFS from building `b`, stepping ONLY onto empty cells (`0`)
//     whose `reach_count == b`.
//   - Why this pruning is exact: because BFS only walks through `0` cells, any
//     cell reachable from all prior buildings lies in a connected component of
//     `0`s shared with them. A cell with `reach_count < b` belongs to a
//     disconnected component and can never lie on a path to any cell reachable
//     by all buildings.
//   - Incrementing `reach_count` from `b` to `b + 1` simultaneously acts as
//     the per-BFS visited set (avoiding per-BFS visited allocations) and
//     maintains the reachability invariant for building `b + 1`.
//   - Track the minimum `total_dist` observed during each building's BFS; if a
//     building cannot reach ANY cell with `reach_count == b`, immediately
//     return `-1`. After the final building completes, its tracked minimum is
//     the answer.
//   Time Complexity:  O(B * m * n), where B is the number of buildings.
//   Space Complexity: O(m * n) for `total_dist`, `reach_count`, and the queue.
//
// Data Structures:
//   - `std::vector<std::vector<int>>`: Two 2D matrices for `total_dist` and
//     `reach_count` (doubling as the visited state for each building's BFS).
//   - `std::queue<std::pair<int, int>>`: Level-order BFS frontier per building.
//
// Real-World Applications:
//   - Urban planning and warehouse / fulfillment-center siting to minimize
//     aggregate delivery distance to all customer hubs around blocked terrain.
//   - Emergency-service station placement (firehouses, ambulances) minimizing
//     combined response time across neighborhoods separated by obstacles.
//   - Distributed-systems coordinator / relay placement minimizing total hop
//     latency to all cluster replicas.
//
// Related LeetCode Problems:
//   - #286 Walls And Gates [done] (nearest single gate via multi-source BFS)
//   - #296 Best Meeting Point [done] (obstacle-free Manhattan median variant)
//   - #542 01 Matrix [done] (grid BFS distance transform)
//   - #1162 As Far from Land as Possible [new] (multi-source grid BFS)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

namespace {

constexpr int kEmpty = 0;
constexpr int kBuilding = 1;
constexpr int kInf = std::numeric_limits<int>::max();

constexpr std::array<std::pair<int, int>, 4> kDirs = {
    {{0, 1}, {0, -1}, {1, 0}, {-1, 0}},
};

class Solution {
 public:
  // Computes the minimum total shortest-path distance from a single empty land
  // cell to all buildings in `grid`, or `-1` if no such cell exists.
  // Time: O(B * m * n), Space: O(m * n).
  int shortestDistance(const std::vector<std::vector<int>>& grid) {
    if (grid.empty() || grid[0].empty()) {
      return -1;
    }

    const std::size_t rows = grid.size();
    const std::size_t cols = grid[0].size();
    std::vector<std::vector<int>> total_dist(rows, std::vector<int>(cols, 0));
    std::vector<std::vector<int>> reach_count(rows, std::vector<int>(cols, 0));

    int building_idx = 0;
    int best_dist = -1;

    for (std::size_t r = 0uz; r < rows; ++r) {
      for (std::size_t c = 0uz; c < cols; ++c) {
        if (grid[r][c] != kBuilding) {
          continue;
        }
        best_dist = bfs_from_building(grid, static_cast<int>(r),
                                      static_cast<int>(c), building_idx,
                                      total_dist, reach_count);
        // If this building cannot reach any cell reachable by all previous
        // buildings, no empty land can reach every building.
        if (best_dist == kInf) {
          return -1;
        }
        ++building_idx;
      }
    }

    return building_idx == 0 ? -1 : best_dist;
  }

 private:
  // Runs BFS from building `(start_r, start_c)`, stepping only onto empty land
  // cells with `reach_count == expected_reach`. Accumulates distances into
  // `total_dist`, increments `reach_count` to mark visited, and returns the
  // minimum cumulative distance among cells reached during this pass (or `kInf`
  // if none were reached).
  static int bfs_from_building(const std::vector<std::vector<int>>& grid,
                               int start_r, int start_c, int expected_reach,
                               std::vector<std::vector<int>>& total_dist,
                               std::vector<std::vector<int>>& reach_count) {
    const int rows = static_cast<int>(grid.size());
    const int cols = static_cast<int>(grid[0].size());

    std::queue<std::pair<int, int>> frontier;
    frontier.emplace(start_r, start_c);

    int step_dist = 0;
    int min_total = kInf;

    while (!frontier.empty()) {
      ++step_dist;
      const std::size_t level_size = frontier.size();

      for (std::size_t i = 0uz; i < level_size; ++i) {
        const auto [r, c] = frontier.front();
        frontier.pop();

        for (const auto& [dr, dc] : kDirs) {
          const int nr = r + dr;
          const int nc = c + dc;
          if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) {
            continue;
          }
          const auto ur = static_cast<std::size_t>(nr);
          const auto uc = static_cast<std::size_t>(nc);
          if (grid[ur][uc] != kEmpty || reach_count[ur][uc] != expected_reach) {
            continue;
          }

          reach_count[ur][uc] = expected_reach + 1;
          total_dist[ur][uc] += step_dist;
          min_total = std::min(min_total, total_dist[ur][uc]);
          frontier.emplace(nr, nc);
        }
      }
    }

    return min_total;
  }
};

LC_PROBLEM(317, "Shortest Distance From All Buildings", "bfs", ctx) {
  Solution sol;

  ctx.test("example_1_three_buildings_with_obstacle", [&] {
    const std::vector<std::vector<int>> grid = {
        {1, 0, 2, 0, 1},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
    };
    ctx.expect_eq(sol.shortestDistance(grid), 7);
  });

  ctx.test("blocked_by_obstacle", [&] {
    const std::vector<std::vector<int>> grid = {{1, 2, 0}};
    ctx.expect_eq(sol.shortestDistance(grid), -1);
  });

  ctx.test("single_building_no_empty_land", [&] {
    const std::vector<std::vector<int>> grid = {{1}};
    ctx.expect_eq(sol.shortestDistance(grid), -1);
  });

  ctx.test("isolated_building_cannot_reach_any_land", [&] {
    const std::vector<std::vector<int>> grid = {
        {0, 2, 1},
        {1, 0, 2},
        {0, 1, 0},
    };
    ctx.expect_eq(sol.shortestDistance(grid), -1);
  });

  ctx.test("dense_six_buildings_from_python_build", [&] {
    // Optimal empty land is (2, 1) with distances 3+5+1+1+2+2 = 14.
    const std::vector<std::vector<int>> grid = {
        {1, 0, 2, 0, 1},
        {0, 0, 0, 0, 0},
        {1, 0, 1, 0, 0},
        {1, 0, 1, 0, 0},
    };
    ctx.expect_eq(sol.shortestDistance(grid), 14);
  });

  ctx.test("single_building_adjacent_empty_and_empty_grid", [&] {
    ctx.expect_eq(sol.shortestDistance({{1, 0}}), 1);
    ctx.expect_eq(sol.shortestDistance({}), -1);
    ctx.expect_eq(sol.shortestDistance({{0, 0}}), -1);
  });
}

}  // namespace
