// LeetCode 64: Minimum Path Sum
// https://leetcode.com/problems/minimum-path-sum/
//
// Problem:
//   Given an m x n grid filled with non-negative integers, find a path from
//   the top-left cell (0, 0) to the bottom-right cell (m - 1, n - 1) that
//   minimizes the sum of all numbers along its path. From any cell, you may
//   move only either down or right.
//   Constraints: 1 <= m, n <= 200, 0 <= grid[i][j] <= 100.
//
// Algorithm:
//   Because moves are restricted to right and down, the grid forms a Directed
//   Acyclic Graph (DAG) with optimal substructure and overlapping subproblems:
//     1. Top-Down DFS with Memoization (`minPathSum`):
//        Define `dfs(r, c)` as the minimum path sum from `(r, c)` to
//        `(m - 1, n - 1)`. Base case: `dfs(m - 1, n - 1) = grid[m - 1][n - 1]`.
//        Transition: `dfs(r, c) = grid[r][c] + min(dfs(r + 1, c), dfs(r, c + 1))`
//        (skipping out-of-bounds neighbors). Results are cached in a 2D memo
//        table initialized to `-1` (fixing the zero-sum sentinel pitfall of
//        in-place negation when cells contain 0).
//        Time: O(m * n), Space: O(m * n) for memo table and recursion stack.
//     2. Bottom-Up Rolling 1D DP (`minPathSumDp`):
//        Maintain a single row `dp[c]` representing the minimum cost to reach
//        column `c` in the current row:
//          `dp[c] = min(dp[c], dp[c - 1]) + grid[r][c]`.
//        Time: O(m * n), Space: O(n).
//
// Data Structures:
//   - 2D memoization table (`std::vector<std::vector<int>>`): caches solved
//     `(r, c)` states with `-1` as the unvisited sentinel.
//   - 1D rolling DP array (`std::vector<int>`): compresses the bottom-up DP
//     state from O(m * n) to O(n) space.
//
// Real-World Applications:
//   - Seam carving for content-aware image resizing (finding minimum-energy
//     monotonic paths across pixel energy matrices).
//   - Dynamic Time Warping (DTW) for speech recognition, gesture alignment,
//     and financial/sensor time-series similarity.
//   - Biological sequence alignment (Needleman-Wunsch / Smith-Waterman grid
//     recurrences).
//   - Robot motion planning and warehouse AGV routing over directed grid
//     costmaps.
//
// Related LeetCode Problems:
//   - #62 Unique Paths [done] (counts right/down paths on the same grid DAG)
//   - #63 Unique Paths II [done] (grid DAG path counting with obstacle cells)
//   - #120 Triangle [done] (minimum path sum on a triangular DAG)
//   - #174 Dungeon Game [new] (reverse-direction grid DP with health floor)
//   - #931 Minimum Falling Path Sum [new] (3-way downward grid minimum path DP)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <vector>

namespace {

class Solution {
 public:
  // Top-down DFS with 2D memoization over the right/down grid DAG.
  // Time: O(m * n), Space: O(m * n).
  int minPathSum(const std::vector<std::vector<int>>& grid) {
    if (grid.empty() || grid[0].empty()) {
      return 0;
    }

    const std::size_t rows = grid.size();
    const std::size_t cols = grid[0].size();
    // Sentinel -1 denotes unvisited states (safe because all grid values >= 0).
    std::vector<std::vector<int>> memo(rows, std::vector<int>(cols, -1));

    // Returns the minimum path sum from cell (r, c) to (rows - 1, cols - 1).
    auto dfs = [&](this auto&& self, std::size_t r, std::size_t c) -> int {
      if (r == rows - 1uz && c == cols - 1uz) {
        return grid[r][c];
      }
      if (memo[r][c] != -1) {
        return memo[r][c];
      }

      int best_next = std::numeric_limits<int>::max();
      if (r + 1uz < rows) {
        best_next = std::min(best_next, self(r + 1uz, c));
      }
      if (c + 1uz < cols) {
        best_next = std::min(best_next, self(r, c + 1uz));
      }

      memo[r][c] = grid[r][c] + best_next;
      return memo[r][c];
    };

    return dfs(0uz, 0uz);
  }

  // Bottom-up 1D rolling-row DP alternative.
  // Time: O(m * n), Space: O(n).
  int minPathSumDp(const std::vector<std::vector<int>>& grid) {
    if (grid.empty() || grid[0].empty()) {
      return 0;
    }

    const std::size_t rows = grid.size();
    const std::size_t cols = grid[0].size();
    std::vector<int> dp(cols, 0);

    // Initialize row 0: cells along the top edge are reachable only from left.
    dp[0] = grid[0][0];
    for (std::size_t c = 1uz; c < cols; ++c) {
      dp[c] = dp[c - 1uz] + grid[0][c];
    }

    // Transition row by row: before updating `dp[c]`, `dp[c]` holds the best
    // cost from above (`r - 1, c`) and `dp[c - 1]` holds the best cost from
    // the left (`r, c - 1`).
    for (std::size_t r = 1uz; r < rows; ++r) {
      dp[0] += grid[r][0];
      for (std::size_t c = 1uz; c < cols; ++c) {
        dp[c] = std::min(dp[c], dp[c - 1uz]) + grid[r][c];
      }
    }

    return dp.back();
  }
};

LC_PROBLEM(64, "Minimum Path Sum", "dfs", ctx) {
  Solution sol;

  ctx.test("example_1_3x3", [&] {
    const std::vector<std::vector<int>> grid = {
        {1, 3, 1},
        {1, 5, 1},
        {4, 2, 1},
    };
    ctx.expect_eq(sol.minPathSum(grid), 7);
    ctx.expect_eq(sol.minPathSumDp(grid), 7);
  });

  ctx.test("example_2_2x3", [&] {
    const std::vector<std::vector<int>> grid = {
        {1, 2, 3},
        {4, 5, 6},
    };
    ctx.expect_eq(sol.minPathSum(grid), 12);
    ctx.expect_eq(sol.minPathSumDp(grid), 12);
  });

  ctx.test("single_cell", [&] {
    const std::vector<std::vector<int>> grid = {{5}};
    ctx.expect_eq(sol.minPathSum(grid), 5);
    ctx.expect_eq(sol.minPathSumDp(grid), 5);
  });

  ctx.test("single_row_and_single_column", [&] {
    const std::vector<std::vector<int>> row_grid = {{2, 4, 1, 3}};
    const std::vector<std::vector<int>> col_grid = {{2}, {4}, {1}, {3}};
    ctx.expect_eq(sol.minPathSum(row_grid), 10);
    ctx.expect_eq(sol.minPathSumDp(row_grid), 10);
    ctx.expect_eq(sol.minPathSum(col_grid), 10);
    ctx.expect_eq(sol.minPathSumDp(col_grid), 10);
  });

  ctx.test("all_zeros_memo_sentinel_safety", [&] {
    const std::vector<std::vector<int>> grid(6, std::vector<int>(6, 0));
    ctx.expect_eq(sol.minPathSum(grid), 0);
    ctx.expect_eq(sol.minPathSumDp(grid), 0);
  });
}

}  // namespace
