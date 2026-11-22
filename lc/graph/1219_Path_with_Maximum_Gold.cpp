// https://leetcode.com/problems/path-with-maximum-gold/
//
// In an m x n gold mine `grid`, each cell contains a non-negative amount of
// gold. Starting and stopping at any non-zero cell, move 4-directionally
// without revisiting cells or stepping onto 0-gold cells. Return the maximum
// gold collectible along a single simple path.

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
