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
