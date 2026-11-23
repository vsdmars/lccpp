// LeetCode 130: Surrounded Regions
// https://leetcode.com/problems/surrounded-regions/
//
// Problem:
//   Given an m x n matrix `board` containing 'X' and 'O', capture all regions
//   of 4-directionally connected 'O' cells that are completely surrounded by
//   'X' by flipping them to 'X' in-place. Any 'O' cell on the border of the
//   board — or connected to a border 'O' — is NOT surrounded and must remain
//   'O'.
//
// Examples:
//   Example 1:
//     Input:  board = [['X', 'X', 'X', 'X'], ['X', 'O', 'O', 'X'], ['X', 'X', 'O',
//             'X'], ['X', 'O', 'X', 'X']]
//     Output: [['X', 'X', 'X', 'X'], ['X', 'X', 'X', 'X'], ['X', 'X', 'X', 'X'],
//             ['X', 'O', 'X', 'X']]
//     Explanation:
//       - Phase 1 scans top and bottom rows and calls markBorderConnected on (3, 1)
//         == 'O', marking it as '#' (neighbor (2, 1) is 'X', so DFS stops).
//       - Phase 2 checks left and right border columns for rows 1..2, all of which
//         are 'X', leaving interior cells (1, 1), (1, 2), and (2, 2) as 'O'.
//       - Phase 3 sweeps the 4x4 grid, flipping unreached 'O's at (1, 1), (1, 2),
//         and (2, 2) to 'X' and restoring '#' at (3, 1) back to 'O'.
//
//   Example 2:
//     Input:  board = [['X', 'O', 'X'], ['X', 'O', 'X'], ['X', 'O', 'X']]
//     Output: [['X', 'O', 'X'], ['X', 'O', 'X'], ['X', 'O', 'X']]
//     Explanation:
//       - Phase 1 encounters border cell (0, 1) == 'O' and invokes
//         markBorderConnected.
//       - DFS traverses downward along column 1 and marks (0, 1), (1, 1), and (2,
//         1) as '#'.
//       - Phase 3 restores all three '#' cells back to 'O', leaving board
//         unchanged.
//
//   Example 3:
//     Input:  board = [['O']]
//     Output: [['O']]
//     Explanation:
//       - With rows = 1 and cols = 1, Phase 1 runs markBorderConnected at (0, 0)
//         and marks it '#'.
//       - Phase 2 skips the interior column loop (1 < 0 is false), and Phase 3
//         restores '#' at (0, 0) back to 'O'.
//
// Algorithm:
//   Reverse-thinking border-seeded Depth-First Search (DFS) flood fill.
//   Instead of testing whether every interior 'O' component reaches the border,
//   seed DFS from every 'O' located on the four edges of the board and mark all
//   reachable 'O' cells in-place with a temporary safe sentinel '#'. Once all
//   border-connected components are protected, make a single full-grid pass:
//     - Remaining 'O' cells are strictly interior (surrounded) -> flip to 'X'.
//     - Protected '#' cells are border-escaped -> restore to 'O'.
//   Using an in-place sentinel eliminates the auxiliary hash set while keeping
//   the exact same border-seeded DFS traversal.
//   Time Complexity:  O(m * n), as every cell is visited at most once by DFS
//                     and once during the final sweep.
//   Space Complexity: O(m * n) worst-case recursion depth (e.g., a snake-shaped
//                     region of 'O's covering the board), O(1) extra grid memory.
//
// Data Structures:
//   - 2D character matrix (`std::vector<std::vector<char>>`): input board that
//     also records safe border-connected cells in-place via sentinel '#'.
//   - Fixed 4-direction offset table (`std::array<std::pair<int, int>, 4>`).
//   - Implicit DFS call stack.
//
// Real-World Applications:
//   - Game AI for Go (Weiqi): detecting captured stone groups with zero
//     liberties / no path to open eyes or board edges.
//   - Image processing and GIS hole-filling: distinguishing enclosed interior
//     holes from background regions connected to the image boundary.
//   - Flood/drainage modeling on elevation grids (identifying endorheic basins
//     that cannot drain to the map perimeter).
//
// Related LeetCode Problems:
//   - #200 Number Of Islands [done] (4-directional grid DFS connected components)
//   - #417 Pacific Atlantic Water Flow [done] (border-seeded reverse grid DFS)
//   - #1020 Number of Enclaves [new] (count interior land cells not on border)
//   - #1254 Number of Closed Islands [done] (count islands not touching border)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

constexpr std::array<std::pair<int, int>, 4> kDirs = {
    {{1, 0}, {-1, 0}, {0, -1}, {0, 1}}};

constexpr char kSafeMarker = '#';

class Solution {
 public:
  // Captures all surrounded 'O' regions in `board` in-place by protecting
  // border-connected 'O' components first via DFS.
  // Time: O(m * n), Space: O(m * n) worst-case DFS stack.
  void solve(std::vector<std::vector<char>>& board) {
    if (board.empty() || board[0].empty()) {
      return;
    }

    const auto rows = static_cast<int>(board.size());
    const auto cols = static_cast<int>(board[0].size());

    // Phase 1: Flood-fill from top and bottom border rows.
    for (int c = 0; c < cols; ++c) {
      markBorderConnected(board, 0, c, rows, cols);
      markBorderConnected(board, rows - 1, c, rows, cols);
    }

    // Phase 2: Flood-fill from left and right border columns.
    for (int r = 1; r < (rows - 1); ++r) {
      markBorderConnected(board, r, 0, rows, cols);
      markBorderConnected(board, r, cols - 1, rows, cols);
    }

    // Phase 3: Flip unreached (surrounded) 'O' -> 'X' and restore '#' -> 'O'.
    for (auto& row : board) {
      for (char& cell : row) {
        if (cell == 'O') {
          cell = 'X';
        } else if (cell == kSafeMarker) {
          cell = 'O';
        }
      }
    }
  }

 private:
  // Recursively marks every 'O' reachable from (r, c) with `kSafeMarker`.
  static void markBorderConnected(std::vector<std::vector<char>>& board, int r,
                                  int c, int rows, int cols) {
    const auto ur = static_cast<std::size_t>(r);
    const auto uc = static_cast<std::size_t>(c);
    if (board[ur][uc] != 'O') {
      return;
    }

    board[ur][uc] = kSafeMarker;
    for (const auto& [dr, dc] : kDirs) {
      const int nr = r + dr;
      const int nc = c + dc;
      if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
        markBorderConnected(board, nr, nc, rows, cols);
      }
    }
  }
};

LC_PROBLEM(130, "Surrounded Regions", "dfs", ctx) {
  Solution sol;

  ctx.test("example_4x4", [&] {
    std::vector<std::vector<char>> board = {
        {'X', 'X', 'X', 'X'},
        {'X', 'O', 'O', 'X'},
        {'X', 'X', 'O', 'X'},
        {'X', 'O', 'X', 'X'},
    };
    const std::vector<std::vector<char>> expected = {
        {'X', 'X', 'X', 'X'},
        {'X', 'X', 'X', 'X'},
        {'X', 'X', 'X', 'X'},
        {'X', 'O', 'X', 'X'},
    };
    sol.solve(board);
    ctx.expect_eq(board, expected);
  });

  ctx.test("python_build_5x5", [&] {
    std::vector<std::vector<char>> board = {
        {'O', 'X', 'X', 'O', 'X'},
        {'X', 'O', 'O', 'X', 'O'},
        {'X', 'O', 'X', 'O', 'X'},
        {'O', 'X', 'O', 'O', 'O'},
        {'X', 'X', 'O', 'X', 'O'},
    };
    const std::vector<std::vector<char>> expected = {
        {'O', 'X', 'X', 'O', 'X'},
        {'X', 'X', 'X', 'X', 'O'},
        {'X', 'X', 'X', 'O', 'X'},
        {'O', 'X', 'O', 'O', 'O'},
        {'X', 'X', 'O', 'X', 'O'},
    };
    sol.solve(board);
    ctx.expect_eq(board, expected);
  });

  ctx.test("vertical_bridge_all_escaped", [&] {
    std::vector<std::vector<char>> board = {
        {'X', 'O', 'X'},
        {'X', 'O', 'X'},
        {'X', 'O', 'X'},
    };
    const auto expected = board;
    sol.solve(board);
    ctx.expect_eq(board, expected);
  });

  ctx.test("single_cell_and_empty", [&] {
    std::vector<std::vector<char>> single_o = {{'O'}};
    sol.solve(single_o);
    ctx.expect_eq(single_o, std::vector<std::vector<char>>{{'O'}});

    std::vector<std::vector<char>> empty_board;
    sol.solve(empty_board);
    ctx.expect_true(empty_board.empty());
  });
}

}  // namespace
