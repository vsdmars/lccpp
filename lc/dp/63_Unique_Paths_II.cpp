// LeetCode 63: Unique Paths II
// https://leetcode.com/problems/unique-paths-ii/
//
// Problem:
//   Given an m x n binary grid obstacleGrid (1 <= m, n <= 100) where 0 marks
//   an open cell and 1 marks an obstacle, a robot starts at (0, 0) and may move
//   only right or down. Return the number of unique paths to (m - 1, n - 1)
//   that never step onto an obstacle cell.
//
// Examples:
//   Example 1:
//     Input:  obstacleGrid = [[0, 0, 0], [0, 1, 0], [0, 0, 0]]
//     Output: 2
//     Explanation:
//       - Both start (0, 0) and end (2, 2) are open, so initialize dp = [1, 0, 0].
//       - Row 0 ([0, 0, 0]) propagates left-to-right to dp = [1, 1, 1].
//       - Row 1 ([0, 1, 0]) keeps dp[0] = 1, zeroes dp[1] = 0 at the obstacle, and
//         updates dp[2] = 1 + 0 = 1.
//       - Row 2 ([0, 0, 0]) updates dp[1] = 0 + 1 = 1 and dp[2] = 1 + 1 = 2,
//         returning dp[2] = 2.
//
//   Example 2:
//     Input:  obstacleGrid = [[0, 1], [0, 0]]
//     Output: 1
//     Explanation:
//       - Initialize dp = [1, 0] since both corners are open.
//       - Row 0 ([0, 1]) keeps dp[0] = 1 and zeroes dp[1] = 0 at obstacle (0, 1).
//       - Row 1 ([0, 0]) keeps dp[0] = 1 and sets dp[1] = dp[1] + dp[0] = 0 + 1 =
//         1, returning dp[1] = 1.
//
//   Example 3:
//     Input:  obstacleGrid = [[0, 0, 0], [1, 1, 1], [0, 0, 0]]
//     Output: 0
//     Explanation:
//       - Initialize dp = [1, 0, 0] and process row 0 ([0, 0, 0]) to get dp = [1,
//         1, 1].
//       - Row 1 ([1, 1, 1]) has kObstacle in every column, resetting dp to [0, 0,
//         0].
//       - Row 2 ([0, 0, 0]) adds only zeros across dp, leaving dp[2] = 0 as the
//         returned count.
//
// Algorithm:
//   1. Bottom-up rolling-row DP (`uniquePathsWithObstacles`): Maintain a 1D
//      array dp of length n representing path counts for the current row,
//      seeded with dp[0] = 1. For each cell (r, c): if obstacleGrid[r][c] == 1,
//      no path may pass through it so set dp[c] = 0; otherwise, if c > 0, add
//      the left neighbor dp[c - 1] to the top neighbor already in dp[c].
//      Intermediate sums use int64_t so unreachable open sub-grids cannot cause
//      32-bit signed overflow.
//      Time Complexity:  O(m * n).
//      Space Complexity: O(n).
//   2. Top-down memoized DFS (`uniquePathsTopDown`): Included to mirror the
//      recursive memoization approach in the Python source. Each state (r, c)
//      sums the valid paths from (r + 1, c) and (r, c + 1) and caches the
//      result in a flat memo table.
//      Time Complexity:  O(m * n).
//      Space Complexity: O(m * n).
//
// Data Structures:
//   - std::vector<std::int64_t> dp: 1D rolling DP table (bottom-up method).
//   - Flat std::vector<std::int64_t> memo indexed by (r * cols) + c (top-down
//     memoized DFS method).
//
// Real-World Applications:
//   - Autonomous robot and warehouse AGV grid routing around static obstacles.
//   - Circuit board / FPGA routing path counting around reserved keep-out zones.
//   - DAG reachability counting when specific intermediate nodes are offline.
//
// Related LeetCode Problems:
//   - #62 Unique Paths [done] (obstacle-free version of this grid DP)
//   - #64 Minimum Path Sum [done] (weighted shortest path on the same grid DAG)
//   - #980 Unique Paths III [new] (4-directional paths visiting all open cells)
//   - #2087 Minimum Cost Homecoming of a Robot in a Grid [new] (grid movement)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr int kObstacle = 1;
constexpr std::int64_t kUnvisited = -1;

class Solution {
 public:
  // Counts unique right/down paths avoiding obstacles using a 1D rolling DP.
  // Time: O(m * n), Space: O(n).
  int uniquePathsWithObstacles(const std::vector<std::vector<int>>& grid) {
    if (grid.empty() || grid[0].empty()) {
      return 0;
    }

    const std::size_t rows = grid.size();
    const std::size_t cols = grid[0].size();

    // If start or destination is blocked, no valid path exists.
    if (grid[0][0] == kObstacle || grid[rows - 1][cols - 1] == kObstacle) {
      return 0;
    }

    // Use 64-bit accumulators to avoid signed overflow in open sub-regions
    // whose paths are later blocked before the exit.
    std::vector<std::int64_t> dp(cols, 0);
    dp[0] = 1;

    for (std::size_t r = 0uz; r < rows; ++r) {
      for (std::size_t c = 0uz; c < cols; ++c) {
        if (grid[r][c] == kObstacle) {
          // Zero out dp[c] so neither the cell to the right nor below inherits
          // paths through this obstacle.
          dp[c] = 0;
        } else if (c > 0uz) {
          dp[c] += dp[c - 1];
        }
      }
    }

    return static_cast<int>(dp[cols - 1]);
  }

  // Reference top-down memoized DFS matching the Python source's first method.
  // Time: O(m * n), Space: O(m * n).
  int uniquePathsTopDown(const std::vector<std::vector<int>>& grid) {
    if (grid.empty() || grid[0].empty()) {
      return 0;
    }

    const std::size_t rows = grid.size();
    const std::size_t cols = grid[0].size();
    if (grid[0][0] == kObstacle || grid[rows - 1][cols - 1] == kObstacle) {
      return 0;
    }

    std::vector<std::int64_t> memo(rows * cols, kUnvisited);
    memo[((rows - 1uz) * cols) + (cols - 1uz)] = 1;

    const auto dfs = [&](this auto&& self, std::size_t r,
                         std::size_t c) -> std::int64_t {
      const std::size_t flat = (r * cols) + c;
      if (memo[flat] != kUnvisited) {
        return memo[flat];
      }

      std::int64_t ways = 0;
      if (r + 1uz < rows && grid[r + 1uz][c] != kObstacle) {
        ways += self(r + 1uz, c);
      }
      if (c + 1uz < cols && grid[r][c + 1uz] != kObstacle) {
        ways += self(r, c + 1uz);
      }

      memo[flat] = ways;
      return ways;
    };

    return static_cast<int>(dfs(0uz, 0uz));
  }
};

LC_PROBLEM(63, "Unique Paths II", "dp", ctx) {
  Solution sol;

  ctx.test("example_1_center_obstacle", [&] {
    const std::vector<std::vector<int>> grid = {
        {0, 0, 0}, {0, 1, 0}, {0, 0, 0}};
    ctx.expect_eq(sol.uniquePathsWithObstacles(grid), 2);
    ctx.expect_eq(sol.uniquePathsTopDown(grid), 2);
  });

  ctx.test("example_2_small_grid", [&] {
    const std::vector<std::vector<int>> grid = {{0, 1}, {0, 0}};
    ctx.expect_eq(sol.uniquePathsWithObstacles(grid), 1);
    ctx.expect_eq(sol.uniquePathsTopDown(grid), 1);
  });

  ctx.test("blocked_start_or_end", [&] {
    const std::vector<std::vector<int>> single_blocked = {{1}};
    ctx.expect_eq(sol.uniquePathsWithObstacles(single_blocked), 0);
    ctx.expect_eq(sol.uniquePathsTopDown(single_blocked), 0);

    const std::vector<std::vector<int>> end_blocked = {{0, 0}, {0, 1}};
    ctx.expect_eq(sol.uniquePathsWithObstacles(end_blocked), 0);
    ctx.expect_eq(sol.uniquePathsTopDown(end_blocked), 0);
  });

  ctx.test("single_open_cell", [&] {
    const std::vector<std::vector<int>> single_open = {{0}};
    ctx.expect_eq(sol.uniquePathsWithObstacles(single_open), 1);
    ctx.expect_eq(sol.uniquePathsTopDown(single_open), 1);
  });

  ctx.test("completely_walled_row", [&] {
    const std::vector<std::vector<int>> walled = {
        {0, 0, 0}, {1, 1, 1}, {0, 0, 0}};
    ctx.expect_eq(sol.uniquePathsWithObstacles(walled), 0);
    ctx.expect_eq(sol.uniquePathsTopDown(walled), 0);
  });

  ctx.test("single_row_obstacle_blocks_tail", [&] {
    const std::vector<std::vector<int>> row = {{0, 0, 1, 0}};
    ctx.expect_eq(sol.uniquePathsWithObstacles(row), 0);
    ctx.expect_eq(sol.uniquePathsTopDown(row), 0);
  });
}

}  // namespace
