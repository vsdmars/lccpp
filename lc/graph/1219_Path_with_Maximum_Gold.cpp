// LeetCode 1219: Path with Maximum Gold
// https://leetcode.com/problems/path-with-maximum-gold/
//
// Problem:
//   Given an m x n integer matrix `grid` representing a gold mine where each
//   cell holds a non-negative amount of gold (`0` means empty), return the
//   maximum total gold collectible along a single simple path. You may start
//   and stop at any cell with positive gold, move 4-directionally (up, down,
//   left, right), never revisit a cell, and never step onto a 0-gold cell.
//   Constraints: 1 <= m, n <= 15, 0 <= grid[r][c] <= 100, and at most 25 cells
//   contain gold.
//
// Examples:
//   Example 1:
//     Input:  grid = [[0, 6, 0], [5, 8, 7], [0, 9, 0]]
//     Output: 24
//     Explanation:
//       - The outer loop starts DFS from each positive-gold cell in the 3x3 grid,
//         temporarily setting each visited cell to 0 and restoring it on return.
//       - Starting at (2, 1) with gold 9, the only positive-gold neighbor is (1, 1)
//         with gold 8.
//       - From (1, 1), setting it to 0 leaves three unvisited positive branches:
//         (0, 1) returning 6, (1, 0) returning 5, and (1, 2) returning 7.
//       - Backtracking restores the cells and takes the maximum branch: 9 + 8 + 7 =
//         24, which exceeds all other starting cells.
//
//   Example 2:
//     Input:  grid = [[1, 0, 7], [2, 0, 6], [3, 4, 5], [0, 3, 0], [9, 0, 20]]
//     Output: 28
//     Explanation:
//       - Cells (4, 0) = 9 and (4, 2) = 20 have no positive-gold neighbors, so
//         their DFS runs return 9 and 20 respectively.
//       - Starting at (0, 0) = 1, DFS follows the single corridor (0, 0) -> (1, 0)
//         -> (2, 0) -> (2, 1), accumulating 1 + 2 + 3 + 4 = 10.
//       - At (2, 1) = 4, the downward branch to (3, 1) = 3 returns 3, whereas the
//         rightward branch explores (2, 2) = 5 -> (1, 2) = 6 -> (0, 2) = 7 and
//         returns 18.
//       - Combining the prefix with the better branch yields 10 + 18 = 28, which
//         becomes the global maximum.
//
//   Example 3:
//     Input:  grid = [[0, 0], [0, 0]]
//     Output: 0
//     Explanation:
//       - The double loop checks grid[r][c] > 0 for every cell in the 2x2 grid.
//       - Because every cell holds 0, no DFS call is launched and best remains at
//         its initial value of 0.
//
// Algorithm:
//   Backtracking Depth-First Search (DFS) initiated from every cell containing
//   gold. On entering `(r, c)`, the cell's gold value is saved and temporarily
//   set to `0` in-place to mark it visited on the active path, preventing
//   revisits without a separate visited matrix. The search explores all four
//   cardinal neighbors, records the highest-yielding continuation, restores the
//   cell's original gold value upon backtracking, and returns `gold +
//   best_branch`. Taking the maximum over all non-zero starting cells yields
//   the globally optimal simple path.
//   Time Complexity: O(m * n + G * 3^G) (at most O(m * n * 3^G)), where
//     G <= 25 is the number of non-zero gold cells (after the first step, each
//     move has at most 3 unvisited neighbors).
//   Space Complexity: O(G) for the DFS recursion call stack and O(1) auxiliary
//     heap space via in-place marking.
//
// Data Structures:
//   - `std::vector<std::vector<int>>& grid`: The 2D gold mine mutated in-place
//     (zeroed on visit, restored on backtrack) to track active path membership.
//   - `std::array<std::pair<int, int>, 4> kDirs`: Fixed compile-time array of
//     4-directional `(dr, dc)` neighbor offsets.
//   - Recursive call stack (`dfs` lambda): Stores active coordinates and saved
//     gold amounts needed to restore state while unwinding.
//
// Real-World Applications:
//   - Autonomous drone or robot foraging and inspection path planning across a
//     2D spatial grid with no-revisit and obstacle constraints.
//   - Open-pit or underground mine excavation routing to maximize resource
//     yield along a continuous non-overlapping path.
//   - Board-game and puzzle move-tree search (such as Boggle or tile-chaining
//     games) using in-place board mutation and undo during backtracking.
//
// Related LeetCode Problems:
//   - #79 Word Search [done] (2D grid backtracking DFS with in-place visited marking)
//   - #980 Unique Paths III [new] (grid backtracking visiting cells without revisits)
//   - #212 Word Search II [done] (multi-path 2D grid backtracking with Trie pruning)
//   - #200 Number of Islands [done] (4-directional DFS over non-zero grid regions)
//   - #329 Longest Increasing Path in a Matrix [done] (grid path optimization where
//     strict monotonicity allows memoization instead of full backtracking)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

constexpr std::array<std::pair<int, int>, 4> kDirs = {
    {{0, 1}, {0, -1}, {1, 0}, {-1, 0}}};

class Solution {
 public:
  // Backtracking DFS starting from every non-zero cell.
  // Each visited cell is temporarily zeroed in-place and restored upon return
  // so no separate visited array is needed.
  // Time: O(m * n * 3^G) where G <= 25 is the number of gold cells.
  // Space: O(G) recursion depth.
  int getMaximumGold(std::vector<std::vector<int>>& grid) {
    if (grid.empty() || grid[0].empty()) {
      return 0;
    }

    const int rows = static_cast<int>(grid.size());
    const int cols = static_cast<int>(grid[0].size());

    auto dfs = [&](this auto&& self, int r, int c) -> int {
      if (r < 0 || r >= rows || c < 0 || c >= cols) {
        return 0;
      }
      int& cell = grid[static_cast<std::size_t>(r)][static_cast<std::size_t>(c)];
      if (cell == 0) {
        return 0;
      }

      const int gold = cell;
      cell = 0;  // Mark cell visited for the current path.

      int best_branch = 0;
      for (const auto& [dr, dc] : kDirs) {
        best_branch = std::max(best_branch, self(r + dr, c + dc));
      }

      cell = gold;  // Restore cell while backtracking.
      return gold + best_branch;
    };

    int best = 0;
    for (int r = 0; r < rows; ++r) {
      for (int c = 0; c < cols; ++c) {
        if (grid[static_cast<std::size_t>(r)][static_cast<std::size_t>(c)] > 0) {
          best = std::max(best, dfs(r, c));
        }
      }
    }
    return best;
  }
};

LC_PROBLEM(1219, "Path with Maximum Gold", "graph", ctx) {
  Solution sol;

  ctx.test("example 1: path 9 -> 8 -> 7 gives 24", [&] {
    std::vector<std::vector<int>> grid = {
        {0, 6, 0},
        {5, 8, 7},
        {0, 9, 0},
    };
    ctx.expect_eq(sol.getMaximumGold(grid), 24);
  });

  ctx.test("example 2: snake path 1..7 gives 28", [&] {
    std::vector<std::vector<int>> grid = {
        {1, 0, 7}, {2, 0, 6}, {3, 4, 5}, {0, 3, 0}, {9, 0, 20},
    };
    ctx.expect_eq(sol.getMaximumGold(grid), 28);
  });

  ctx.test("all zero grid gives 0", [&] {
    std::vector<std::vector<int>> grid = {{0, 0}, {0, 0}};
    ctx.expect_eq(sol.getMaximumGold(grid), 0);
  });
}

}  // namespace
