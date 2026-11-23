// LeetCode 741: Cherry Pickup
// https://leetcode.com/problems/cherry-pickup/
//
// Problem:
//   Given an N x N grid where 0 is an empty cell, 1 contains a cherry, and -1
//   is an impassable thorn, walk from (0, 0) to (N - 1, N - 1) moving only
//   right or down, then return to (0, 0) moving only left or up. Each cherry
//   can be picked at most once. Return the maximum cherries collectible, or 0
//   if no valid round-trip path exists.
//
// Examples:
//   Example 1:
//     Input:  grid = [[0, 1, -1], [1, 0, -1], [1, 1, 1]]
//     Output: 5
//     Explanation:
//       - Verify start (0, 0) and end (2, 2) are not thorns (-1) and call dfs(grid,
//         memo, 0, 0, 0).
//       - At step 1, split the walkers to (r1, c1) = (1, 0) and (r2, c2) = (0, 1),
//         collecting grid[1][0] + grid[0][1] = 2 cherries.
//       - At step 2, advance to (2, 0) and (1, 1) (avoiding the -1 thorns at column
//         2) to collect grid[2][0] + grid[1][1] = 1 + 0 = 1 cherry.
//       - At step 3 both walkers converge at (2, 1) (crediting grid[2][1] = 1
//         once), then hit the base case at (2, 2) for grid[2][2] = 1, returning
//         std::max(0, 5) = 5.
//
//   Example 2:
//     Input:  grid = [[1, 1, -1], [1, -1, 1], [-1, 1, 1]]
//     Output: 0
//     Explanation:
//       - Pass the endpoint check (grid[0][0] = 1 and grid[2][2] = 1) and invoke
//         dfs(grid, memo, 0, 0, 0).
//       - At step 1, explore moving walkers to (0, 1) or (1, 0), the only non-thorn
//         cells reachable from (0, 0).
//       - At step 2, every cell with r + c = 2 is a thorn (grid[0][2] = -1,
//         grid[1][1] = -1, grid[2][0] = -1), so all four transitions return
//         kImpossible (-1).
//       - Propagate kImpossible back to (0, 0, 0) and return std::max(0, -1) = 0.
//
//   Example 3:
//     Input:  grid = [[1]]
//     Output: 1
//     Explanation:
//       - Verify n = 1 and grid[0][0] = 1 != -1, then call dfs(grid, memo, 0, 0,
//         0).
//       - Hit the base case r1 == n - 1 && c1 == n - 1 at (0, 0), returning
//         grid[0][0] = 1 so cherryPickup returns std::max(0, 1) = 1.
//
// Algorithm:
//   A round trip (down/right then up/left) is equivalent to two walkers
//   simultaneously traversing from (0, 0) to (N - 1, N - 1) moving only down
//   or right. Because both walkers take the same number of steps at every
//   moment (`r1 + c1 == r2 + c2`), `c2 = r1 + c1 - r2` is uniquely determined,
//   collapsing the 4D state `(r1, c1, r2, c2)` to 3D `(r1, c1, r2)`. Top-down
//   DFS with memoization evaluates all 4 joint step combinations (DD, DR, RD,
//   RR) and credits `grid[r1][c1]` once when both walkers occupy the same cell
//   or `grid[r1][c1] + grid[r2][c2]` when on distinct cells.
//   Time complexity: O(N^3) states with O(1) transition work per state.
//   Space complexity: O(N^3) for the flat memoization table and O(N) recursion
//   depth.
//
// Data Structures:
//   - std::vector<int>: Flat N x N x N memoization table indexed by
//     `(((r1 * n) + c1) * n) + r2`, using distinct sentinels for unvisited and
//     unreachable states.
//
// Real-World Applications:
//   - Dual-agent / multi-robot warehouse picking and routing on constrained
//     grids without double-counting collected items.
//   - Two-path network flow / disjoint routing maximization in DAGs and
//     Manhattan-grid VLSI routing.
//   - Round-trip delivery drone route planning with single-pickup rewards.
//
// Related LeetCode Problems:
//   - #1463 Cherry Pickup II [new] (two simultaneous top-to-bottom robots)
//   - #64 Minimum Path Sum [done] (single-path grid DP)
//   - #63 Unique Paths II [done] (grid DP with obstacles)
//   - #174 Dungeon Game [new] (grid DP with backward feasibility constraints)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <vector>

namespace {

constexpr int kUnvisited = std::numeric_limits<int>::min();
constexpr int kImpossible = -1;

class Solution {
 public:
  // Returns the maximum number of cherries collectible on a valid round trip
  // from (0, 0) to (N - 1, N - 1) and back, or 0 if no valid path exists.
  //
  // Invariant: Both simulated walkers are always at Manhattan distance
  // `t = r1 + c1 = r2 + c2` from (0, 0), so they can meet only at the exact
  // same step `t`, ensuring cherries are never double-counted.
  // Time: O(N^3); Space: O(N^3).
  int cherryPickup(const std::vector<std::vector<int>>& grid) {
    const std::size_t n = grid.size();
    if (n == 0uz || grid[0][0] == -1 || grid[n - 1uz][n - 1uz] == -1) {
      return 0;
    }

    // Flat 3D memoization table for state (r1, c1, r2), where c2 = r1 + c1 - r2.
    std::vector<int> memo(n * n * n, kUnvisited);
    return std::max(0, dfs(grid, memo, 0uz, 0uz, 0uz));
  }

 private:
  // Memoized top-down solver for two walkers at `(r1, c1)` and `(r2, c2)`
  // where `c2 = (r1 + c1) - r2`.
  static int dfs(const std::vector<std::vector<int>>& grid,
                 std::vector<int>& memo, std::size_t r1, std::size_t c1,
                 std::size_t r2) {
    const std::size_t n = grid.size();
    const std::size_t step = r1 + c1;
    if (r1 >= n || c1 >= n || r2 >= n || r2 > step) {
      return kImpossible;
    }
    const std::size_t c2 = step - r2;
    if (c2 >= n || grid[r1][c1] == -1 || grid[r2][c2] == -1) {
      return kImpossible;
    }

    // When walker 1 reaches (n - 1, n - 1), step == 2*(n - 1), which forces
    // walker 2 to also be at (n - 1, n - 1); count that final cell once.
    if (r1 == n - 1uz && c1 == n - 1uz) {
      return grid[r1][c1];
    }

    const std::size_t key = (((r1 * n) + c1) * n) + r2;
    if (memo[key] != kUnvisited) {
      return memo[key];
    }

    // Explore all four joint (down/right) x (down/right) transitions.
    const int best_next = std::max({
        dfs(grid, memo, r1 + 1uz, c1, r2 + 1uz),
        dfs(grid, memo, r1 + 1uz, c1, r2),
        dfs(grid, memo, r1, c1 + 1uz, r2 + 1uz),
        dfs(grid, memo, r1, c1 + 1uz, r2),
    });

    int out = kImpossible;
    if (best_next != kImpossible) {
      // Credit shared cell once; otherwise collect cherries from both cells.
      const int gained =
          grid[r1][c1] + ((r1 == r2 && c1 == c2) ? 0 : grid[r2][c2]);
      out = best_next + gained;
    }

    memo[key] = out;
    return out;
  }
};

LC_PROBLEM(741, "Cherry Pickup", "dp", ctx) {
  Solution sol;

  ctx.test("example_1_round_trip_collects_5", [&] {
    const std::vector<std::vector<int>> grid = {
        {0, 1, -1},
        {1, 0, -1},
        {1, 1, 1},
    };
    ctx.expect_eq(sol.cherryPickup(grid), 5);
  });

  ctx.test("blocked_grid_returns_zero", [&] {
    const std::vector<std::vector<int>> grid = {
        {1, 1, -1},
        {1, -1, 1},
        {-1, 1, 1},
    };
    ctx.expect_eq(sol.cherryPickup(grid), 0);
  });

  ctx.test("all_cherries_2x2", [&] {
    const std::vector<std::vector<int>> grid = {
        {1, 1},
        {1, 1},
    };
    ctx.expect_eq(sol.cherryPickup(grid), 4);
  });

  ctx.test("single_cell_cases", [&] {
    ctx.expect_eq(sol.cherryPickup({{1}}), 1);
    ctx.expect_eq(sol.cherryPickup({{0}}), 0);
    ctx.expect_eq(sol.cherryPickup({{-1}}), 0);
  });

  ctx.test("two_paths_needed_for_optimum", [&] {
    // Greedy single pass would miss a cherry achievable only by splitting paths.
    const std::vector<std::vector<int>> grid = {
        {1, 1, 1, 0, 0},
        {0, 0, 1, 0, 1},
        {1, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 1, 1},
    };
    ctx.expect_eq(sol.cherryPickup(grid), 11);
  });
}

}  // namespace
