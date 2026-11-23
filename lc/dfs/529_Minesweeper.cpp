// LeetCode 529: Minesweeper
// https://leetcode.com/problems/minesweeper/
//
// Problem:
//   Given an m x n Minesweeper board and a click position `[click_r, click_c]`
//   on an unrevealed square (`'M'` for an unrevealed mine or `'E'` for an
//   unrevealed empty square), return the updated board after applying the game
//   rules:
//     1. Clicking an unrevealed mine `'M'` changes it to `'X'` (game over).
//     2. Clicking an unrevealed empty square `'E'` with 1..8 adjacent mines
//        (across all 8 orthogonal and diagonal neighbors) replaces it with the
//        corresponding digit `'1'`..`'8'` and stops expanding.
//     3. Clicking an `'E'` with 0 adjacent mines replaces it with `'B'` and
//        recursively reveals all adjacent unrevealed squares.
//
// Examples:
//   Example 1:
//     Input:  board =
//             [['E','E','E','E','E'],['E','E','M','E','E'],['E','E','E','E','E'],['E','E','E','E','E']],
//             click = [3, 0]
//     Output: [['B','1','E','1','B'],['B','1','M','1','B'],['B','1','1','1','B'],['B','B','B','B','B']]
//     Explanation:
//       - Because board[3][0] is 'E' rather than 'M', updateBoard calls
//         dfsReveal(board, 3, 0).
//       - countAdjacentMines(board, 3, 0) finds 0 mines among the 8 neighbors in
//         kDirs8, so board[3][0] becomes 'B' and dfsReveal recurses into its
//         unrevealed neighbors.
//       - Blank cells with 0 adjacent mines in rows 0-3 (such as (3,1)..(3,4),
//         (2,0), (2,4), (1,0), (1,4), (0,0), and (0,4)) are marked 'B' and continue
//         expanding.
//       - At the 7 unrevealed neighbors surrounding the mine at (1, 2) — namely
//         (0,1), (0,3), (1,1), (1,3), (2,1), (2,2), and (2,3) — countAdjacentMines
//         returns 1, so each is set to '1' and stops expanding.
//       - Cell (0, 2) is shielded by '1' frontier squares and the 'M' at (1, 2), so
//         it is never visited and remains 'E'.
//
//   Example 2:
//     Input:  board =
//             [['B','1','E','1','B'],['B','1','M','1','B'],['B','1','1','1','B'],['B','B','B','B','B']],
//             click = [1, 2]
//     Output: [['B','1','E','1','B'],['B','1','X','1','B'],['B','1','1','1','B'],['B','B','B','B','B']]
//     Explanation:
//       - updateBoard inspects board[1][2] at the click coordinates [1, 2] and sees
//         an unrevealed mine 'M'.
//       - Rule 1 triggers immediately, replacing board[1][2] with 'X' and returning
//         the board without calling dfsReveal.
//
//   Example 3:
//     Input:  board = [['E','M'],['E','E']], click = [0, 0]
//     Output: [['1','M'],['E','E']]
//     Explanation:
//       - Because board[0][0] is 'E', updateBoard invokes dfsReveal(board, 0, 0).
//       - countAdjacentMines(board, 0, 0) checks valid neighbors in kDirs8 and
//         counts 1 adjacent mine at (0, 1).
//       - Since mines > 0, board[0][0] is written as '1' and dfsReveal returns
//         immediately, leaving (1, 0) and (1, 1) unrevealed as 'E'.
//
// Algorithm:
//   Handle the mine-click base case (`'M' -> 'X'`) immediately. Otherwise,
//   perform 8-directional flood-fill DFS (plus an equivalent BFS variant
//   matching the Python source) starting from `click`. At each unrevealed `'E'`
//   cell, count how many of its 8 neighbors contain `'M'`. If the count is
//   positive, write the digit character and stop; if zero, mark the cell `'B'`
//   and expand to all valid `'E'` neighbors. In the BFS variant, mark each
//   discovered `'E'` cell as `'B'` upon enqueueing so no cell is enqueued
//   more than once (overwriting with a digit when popped if adjacent mines
//   exist).
//   Time Complexity:  O(m * n), since each cell is revealed at most once and
//                     checks at most 8 neighbors.
//   Space Complexity: O(m * n) worst case for the DFS recursion stack or BFS
//                     queue when the entire board is blank.
//
// Data Structures:
//   - 2D character grid (`std::vector<std::vector<char>>`): updated in-place
//     to serve as both the game state and the visited marker (`'E'` -> `'B'`
//     or `'1'`..`'8'`).
//   - Fixed 8-direction offset table (`std::array<std::pair<int, int>, 8>`).
//   - FIFO queue (`std::queue<std::pair<int, int>>`): used in the secondary
//     BFS implementation.
//
// Real-World Applications:
//   - Interactive grid games & puzzle engines: cascade-revealing connected
//     empty regions bordered by frontier clues.
//   - Image editing flood-fill / magic-wand selection: expanding across
//     uniform-color pixels while stopping at boundary edges.
//   - Geospatial hazard zoning: delineating safe contiguous zones bounded by
//     buffer cells adjacent to hazard points.
//
// Related LeetCode Problems:
//   - #733 Flood Fill [done] (4-directional region recoloring via DFS/BFS)
//   - #200 Number of Islands [done] (grid flood-fill component traversal)
//   - #130 Surrounded Regions [done] (boundary-constrained grid DFS)
//   - #695 Max Area of Island [done] (DFS area accumulation on a 2D grid)
//   - #286 Walls and Gates [done] (multi-source grid BFS)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <queue>
#include <utility>
#include <vector>

namespace {

// All 8 orthogonal and diagonal neighbor offsets.
constexpr std::array<std::pair<int, int>, 8> kDirs8 = {
    {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}}};

class Solution {
 public:
  // Reveals the board starting from `click` using recursive 8-directional DFS.
  // Time: O(m * n) | Space: O(m * n) worst-case recursion depth.
  std::vector<std::vector<char>> updateBoard(
      std::vector<std::vector<char>>& board, const std::vector<int>& click) {
    const int r = click[0];
    const int c = click[1];
    const auto ur = static_cast<std::size_t>(r);
    const auto uc = static_cast<std::size_t>(c);

    // Rule 1: clicking a mine ends the game immediately.
    if (board[ur][uc] == 'M') {
      board[ur][uc] = 'X';
      return board;
    }

    dfsReveal(board, r, c);
    return board;
  }

  // Secondary iterative BFS approach (matching the Python `updateBoard` BFS).
  // Time: O(m * n) | Space: O(m * n).
  std::vector<std::vector<char>> updateBoardBfs(
      std::vector<std::vector<char>>& board, const std::vector<int>& click) {
    const int start_r = click[0];
    const int start_c = click[1];
    const auto sr = static_cast<std::size_t>(start_r);
    const auto sc = static_cast<std::size_t>(start_c);

    if (board[sr][sc] == 'M') {
      board[sr][sc] = 'X';
      return board;
    }

    std::queue<std::pair<int, int>> q;
    q.emplace(start_r, start_c);
    // Mark as visited immediately upon enqueueing so duplicates never enter `q`.
    board[sr][sc] = 'B';

    while (!q.empty()) {
      const auto [r, c] = q.front();
      q.pop();

      const int mines = countAdjacentMines(board, r, c);
      if (mines > 0) {
        board[static_cast<std::size_t>(r)][static_cast<std::size_t>(c)] =
            static_cast<char>('0' + mines);
        continue;
      }

      enqueueEmptyNeighbors(board, r, c, q);
    }

    return board;
  }

 private:
  // Returns true when `(r, c)` lies inside the `board` bounds.
  static bool inBounds(const std::vector<std::vector<char>>& board, int r,
                       int c) {
    return r >= 0 && std::cmp_less(r, board.size()) && c >= 0 &&
           std::cmp_less(c, board[0].size());
  }

  // Counts unrevealed mines `'M'` among the 8 neighbors of `(r, c)`.
  static int countAdjacentMines(const std::vector<std::vector<char>>& board,
                                int r, int c) {
    int mines = 0;
    for (const auto& [dr, dc] : kDirs8) {
      const int nr = r + dr;
      const int nc = c + dc;
      if (inBounds(board, nr, nc) &&
          board[static_cast<std::size_t>(nr)][static_cast<std::size_t>(nc)] ==
              'M') {
        ++mines;
      }
    }
    return mines;
  }

  // Enqueues all adjacent unrevealed empty cells `'E'` and pre-marks them `'B'`.
  static void enqueueEmptyNeighbors(std::vector<std::vector<char>>& board,
                                    int r, int c,
                                    std::queue<std::pair<int, int>>& q) {
    for (const auto& [dr, dc] : kDirs8) {
      const int nr = r + dr;
      const int nc = c + dc;
      if (!inBounds(board, nr, nc)) {
        continue;
      }
      auto& cell =
          board[static_cast<std::size_t>(nr)][static_cast<std::size_t>(nc)];
      if (cell == 'E') {
        cell = 'B';
        q.emplace(nr, nc);
      }
    }
  }

  // Recursively reveals cell `(r, c)` and cascades through blank `'B'` zones.
  static void dfsReveal(std::vector<std::vector<char>>& board, int r, int c) {
    if (!inBounds(board, r, c)) {
      return;
    }
    auto& cell =
        board[static_cast<std::size_t>(r)][static_cast<std::size_t>(c)];
    if (cell != 'E') {
      return;
    }

    const int mines = countAdjacentMines(board, r, c);
    if (mines > 0) {
      // Frontier square: record mine count and stop expanding.
      cell = static_cast<char>('0' + mines);
      return;
    }

    // Blank square: reveal as `'B'` and recurse into all 8 neighbors.
    cell = 'B';
    for (const auto& [dr, dc] : kDirs8) {
      dfsReveal(board, r + dr, c + dc);
    }
  }
};

LC_PROBLEM(529, "Minesweeper", "dfs", ctx) {
  ctx.test("example_1_cascade_reveal", [&] {
    const std::vector<std::vector<char>> initial = {
        {'E', 'E', 'E', 'E', 'E'},
        {'E', 'E', 'M', 'E', 'E'},
        {'E', 'E', 'E', 'E', 'E'},
        {'E', 'E', 'E', 'E', 'E'},
    };
    const std::vector<std::vector<char>> expected = {
        {'B', '1', 'E', '1', 'B'},
        {'B', '1', 'M', '1', 'B'},
        {'B', '1', '1', '1', 'B'},
        {'B', 'B', 'B', 'B', 'B'},
    };

    Solution sol;
    auto board_dfs = initial;
    auto board_bfs = initial;
    ctx.expect_eq(sol.updateBoard(board_dfs, {3, 0}), expected);
    ctx.expect_eq(sol.updateBoardBfs(board_bfs, {3, 0}), expected);
  });

  ctx.test("example_2_click_mine", [&] {
    const std::vector<std::vector<char>> initial = {
        {'B', '1', 'E', '1', 'B'},
        {'B', '1', 'M', '1', 'B'},
        {'B', '1', '1', '1', 'B'},
        {'B', 'B', 'B', 'B', 'B'},
    };
    const std::vector<std::vector<char>> expected = {
        {'B', '1', 'E', '1', 'B'},
        {'B', '1', 'X', '1', 'B'},
        {'B', '1', '1', '1', 'B'},
        {'B', 'B', 'B', 'B', 'B'},
    };

    Solution sol;
    auto board_dfs = initial;
    auto board_bfs = initial;
    ctx.expect_eq(sol.updateBoard(board_dfs, {1, 2}), expected);
    ctx.expect_eq(sol.updateBoardBfs(board_bfs, {1, 2}), expected);
  });

  ctx.test("python_build_8x8_board", [&] {
    const std::vector<std::vector<char>> initial = {
        {'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E'},
        {'E', 'E', 'E', 'E', 'E', 'E', 'E', 'M'},
        {'E', 'E', 'M', 'E', 'E', 'E', 'E', 'E'},
        {'M', 'E', 'E', 'E', 'E', 'E', 'E', 'E'},
        {'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E'},
        {'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E'},
        {'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E'},
        {'E', 'E', 'M', 'M', 'E', 'E', 'E', 'E'},
    };
    const std::vector<std::vector<char>> expected = {
        {'B', 'B', 'B', 'B', 'B', 'B', '1', 'E'},
        {'B', '1', '1', '1', 'B', 'B', '1', 'M'},
        {'1', '2', 'M', '1', 'B', 'B', '1', '1'},
        {'M', '2', '1', '1', 'B', 'B', 'B', 'B'},
        {'1', '1', 'B', 'B', 'B', 'B', 'B', 'B'},
        {'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B'},
        {'B', '1', '2', '2', '1', 'B', 'B', 'B'},
        {'B', '1', 'M', 'M', '1', 'B', 'B', 'B'},
    };

    Solution sol;
    auto board_dfs = initial;
    auto board_bfs = initial;
    ctx.expect_eq(sol.updateBoard(board_dfs, {0, 0}), expected);
    ctx.expect_eq(sol.updateBoardBfs(board_bfs, {0, 0}), expected);
  });

  ctx.test("click_adjacent_to_mine_stops_immediately", [&] {
    const std::vector<std::vector<char>> initial = {
        {'E', 'M'},
        {'E', 'E'},
    };
    const std::vector<std::vector<char>> expected = {
        {'1', 'M'},
        {'E', 'E'},
    };

    Solution sol;
    auto board_dfs = initial;
    auto board_bfs = initial;
    ctx.expect_eq(sol.updateBoard(board_dfs, {0, 0}), expected);
    ctx.expect_eq(sol.updateBoardBfs(board_bfs, {0, 0}), expected);
  });

  ctx.test("single_cell_boards", [&] {
    Solution sol;
    std::vector<std::vector<char>> empty_cell = {{'E'}};
    ctx.expect_eq(sol.updateBoard(empty_cell, {0, 0}),
                  std::vector<std::vector<char>>{{'B'}});

    std::vector<std::vector<char>> mine_cell = {{'M'}};
    ctx.expect_eq(sol.updateBoard(mine_cell, {0, 0}),
                  std::vector<std::vector<char>>{{'X'}});
  });
}

}  // namespace
