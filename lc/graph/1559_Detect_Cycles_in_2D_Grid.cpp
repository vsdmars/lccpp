// LeetCode 1559: Detect Cycles in 2D Grid
// https://leetcode.com/problems/detect-cycles-in-2d-grid/
//
// Problem:
//   Given an m x n 2D grid of lowercase English letters `grid`, return `true`
//   if there exists a cycle of length >= 4 consisting entirely of identical
//   characters, and `false` otherwise. A valid cycle moves 4-directionally (up,
//   down, left, right) through distinct same-character cells without
//   immediately stepping back to the previous cell, returning to its start.
//   Constraints: 1 <= m, n <= 500, `grid[i][j]` is a lowercase English letter.
//
// Algorithm:
//   Undirected Depth-First Search (DFS) with parent-coordinate tracking across
//   same-character connected components. Key graph-theoretic invariant: any
//   4-connected 2D grid is bipartite (cells 2-color by `(r + c) % 2`), so it
//   contains no odd-length cycles. Because DFS skips the immediate parent cell
//   `(pr, pc)` (preventing trivial length-2 backtracks), encountering any
//   already-visited same-character neighbor guarantees a simple cycle of even
//   length >= 4. The solver scans all cells, launches DFS on each unvisited
//   component, and returns `true` on the first detected cycle.
//   Time Complexity: O(m * n), since each cell is visited at most once and
//     inspects at most 4 neighbors.
//   Space Complexity: O(m * n) for the `visited` matrix and worst-case DFS
//     recursion stack depth.
//
// Data Structures:
//   - `std::vector<std::vector<std::uint8_t>> visited`: 2D byte-flag matrix
//     recording cells explored across all DFS traversals.
//   - `std::array<std::pair<int, int>, 4> kDirs`: Fixed compile-time array of
//     4-directional `(dr, dc)` neighbor offsets.
//   - Recursive call stack (`dfs` passing `(r, c)` and parent `(pr, pc)`):
//     Tracks active traversal state while suppressing immediate backtracking.
//
// Real-World Applications:
//   - Electronic Design Automation (EDA) and PCB layout checking: detecting
//     unintended closed conductive loops (ground loops or shorts) on a grid
//     routing layer.
//   - Computer vision and image segmentation: testing whether a connected
//     region of matching pixels forms a closed contour enclosing a hole.
//   - Tile-based puzzle and game engines (e.g., circuit/pipe-connection or
//     match-loop games) validating closed loops of matching tiles.
//
// Related LeetCode Problems:
//   - #200 Number of Islands [done] (4-directional DFS connected-component search on
//     a 2D grid)
//   - #261 Graph Valid Tree [done] (undirected cycle detection using parent tracking)
//   - #684 Redundant Connection [done] (detecting cycles in an undirected graph)
//   - #130 Surrounded Regions [done] (2D grid connected-component boundary analysis)
//   - #1034 Coloring A Border [done] (4-directional same-value component DFS on a
//     2D grid)

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
