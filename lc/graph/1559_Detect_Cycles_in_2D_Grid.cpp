// https://leetcode.com/problems/detect-cycles-in-2d-grid/
//
// Given an m x n grid of lowercase characters, determine whether there exists a
// cycle of length >= 4 consisting entirely of identical characters, moving
// 4-directionally without immediately stepping back to the previous cell.

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace {

constexpr std::array<std::pair<int, int>, 4> kDirs = {
    {{0, 1}, {0, -1}, {1, 0}, {-1, 0}}};

class Solution {
 public:
  // Runs DFS over each unvisited same-character connected component.
  // Because the 4-connected grid graph is bipartite (no odd cycles exist), any
  // cycle that avoids immediately backtracking to its parent automatically has
  // length >= 4.
  // Time: O(m * n), Space: O(m * n).
  bool containsCycle(const std::vector<std::vector<char>>& grid) {
    if (grid.empty() || grid[0].empty()) {
      return false;
    }

    const int rows = static_cast<int>(grid.size());
    const int cols = static_cast<int>(grid[0].size());
    std::vector<std::vector<std::uint8_t>> visited(
        grid.size(), std::vector<std::uint8_t>(grid[0].size(), 0));

    for (int r = 0; r < rows; ++r) {
      for (int c = 0; c < cols; ++c) {
        const auto ur = static_cast<std::size_t>(r);
        const auto uc = static_cast<std::size_t>(c);
        if (visited[ur][uc] == 0 && dfs(grid, visited, r, c, -1, -1)) {
          return true;
        }
      }
    }
    return false;
  }

 private:
  static bool dfs(const std::vector<std::vector<char>>& grid,
                  std::vector<std::vector<std::uint8_t>>& visited, int r, int c,
                  int pr, int pc) {
    const int rows = static_cast<int>(grid.size());
    const int cols = static_cast<int>(grid[0].size());
    const auto ur = static_cast<std::size_t>(r);
    const auto uc = static_cast<std::size_t>(c);
    visited[ur][uc] = 1;
    const char target = grid[ur][uc];

    for (const auto& [dr, dc] : kDirs) {
      const int nr = r + dr;
      const int nc = c + dc;
      if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) {
        continue;
      }
      if (nr == pr && nc == pc) {
        continue;  // Never immediately backtrack to the previous cell.
      }
      const auto unr = static_cast<std::size_t>(nr);
      const auto unc = static_cast<std::size_t>(nc);
      if (grid[unr][unc] != target) {
        continue;
      }
      if (visited[unr][unc] != 0 || dfs(grid, visited, nr, nc, r, c)) {
        return true;
      }
    }
    return false;
  }
};

LC_PROBLEM(1559, "Detect Cycles in 2D Grid", "graph", ctx) {
  Solution sol;

  ctx.test("example 1: concentric rectangular cycles", [&] {
    const std::vector<std::vector<char>> grid = {
        {'a', 'a', 'a', 'a'},
        {'a', 'b', 'b', 'a'},
        {'a', 'b', 'b', 'a'},
        {'a', 'a', 'a', 'a'},
    };
    ctx.expect_true(sol.containsCycle(grid));
  });

  ctx.test("example 2: irregular loop of 'c'", [&] {
    const std::vector<std::vector<char>> grid = {
        {'c', 'c', 'c', 'a'},
        {'c', 'd', 'c', 'c'},
        {'c', 'c', 'e', 'c'},
        {'f', 'c', 'c', 'c'},
    };
    ctx.expect_true(sol.containsCycle(grid));
  });

  ctx.test("example 3: no cycle exists", [&] {
    const std::vector<std::vector<char>> grid = {
        {'a', 'b', 'b'},
        {'b', 'z', 'b'},
        {'b', 'b', 'a'},
    };
    ctx.expect_false(sol.containsCycle(grid));
  });
}

}  // namespace
