// LeetCode 1275: Find Winner On A Tic Tac Toe Game
// https://leetcode.com/problems/find-winner-on-a-tic-tac-toe-game/
//
// Problem:
//   Two players A ('X', moves first) and B ('O', moves second) take turns
//   playing valid moves on a 3x3 Tic-Tac-Toe board. Given the ordered list
//   `moves` of `[row, col]` coordinates, return `"A"` or `"B"` if a player has
//   completed any row, column, or diagonal of length 3; `"Draw"` if all 9
//   squares are filled with no winner; or `"Pending"` if moves remain.
//
// Algorithm:
//   Track signed net counts for every winning line (3 rows, 3 columns, main
//   diagonal `r == c`, and anti-diagonal `r + c == 2`). Assign player A weight
//   `+1` and player B weight `-1`. On each move `(r, c)`:
//     - Add `player` to `rows[r]` and `cols[c]`.
//     - If `r == c`, add `player` to `diag`.
//     - If `r + c == 2`, add `player` to `anti_diag`.
//     - Because every square is played at most once, a line reaches absolute
//       value 3 iff all 3 squares on that line belong to the current player.
//       If so, return `"A"` (for +1) or `"B"` (for -1) immediately.
//   If the loop finishes without a winner, return `"Draw"` when 9 moves were
//   played and `"Pending"` otherwise.
//   Time Complexity:  O(M) where M <= 9 is the number of moves (O(1) per move).
//   Space Complexity: O(1) — fixed-size counters for the 3x3 board (O(N) for a
//                     generalized N x N board).
//
// Data Structures:
//   - `std::array<int, 3> rows`, `cols`: signed line counters per row/column.
//   - Two scalar integers `diag` and `anti_diag` for the two diagonals.
//
// Real-World Applications:
//   - O(1) incremental win/condition detection in turn-based board-game engines
//     and online multiplayer game servers.
//   - Streaming threshold/quorum counters across overlapping partitions (row,
//     column, and diagonal parity checks in RAID / erasure coding).
//   - Real-time rule-evaluation engines avoiding full-board rescanning per step.
//
// Related LeetCode Problems:
//   - #348 Design Tic-Tac-Toe [done] (online N x N Tic-Tac-Toe using the same O(1) counters)
//   - #794 Valid Tic-Tac-Toe State [done] (validate reachability of a 3x3 board state)
//   - #1222 Queens That Can Attack the King [new] (grid line/diagonal scanning)
//   - #36 Valid Sudoku [done] (row, column, and region constraint tracking)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <cstdlib>
#include <string>
#include <vector>

namespace {

constexpr std::size_t kBoardSize = 3uz;
constexpr int kBoardSizeInt = 3;
constexpr std::size_t kMaxMoves = kBoardSize * kBoardSize;

class Solution {
 public:
  // Evaluates the outcome ("A", "B", "Draw", or "Pending") after playing the
  // given sequence of alternating Tic-Tac-Toe moves.
  // Time: O(M) where M <= 9, Space: O(1).
  std::string tictactoe(const std::vector<std::vector<int>>& moves) {
    std::array<int, kBoardSize> rows{};
    std::array<int, kBoardSize> cols{};
    int diag = 0;
    int anti_diag = 0;

    for (std::size_t i = 0uz; i < moves.size(); ++i) {
      // Even-indexed moves belong to A (+1); odd-indexed moves belong to B (-1).
      const int player = (i % 2uz == 0uz) ? 1 : -1;
      const auto r = static_cast<std::size_t>(moves[i][0]);
      const auto c = static_cast<std::size_t>(moves[i][1]);

      rows[r] += player;
      cols[c] += player;
      if (r == c) {
        diag += player;
      }
      if ((r + c) == (kBoardSize - 1uz)) {
        anti_diag += player;
      }

      // A line's magnitude reaches 3 iff the current player owns all 3 cells.
      if (std::abs(rows[r]) == kBoardSizeInt ||
          std::abs(cols[c]) == kBoardSizeInt ||
          std::abs(diag) == kBoardSizeInt ||
          std::abs(anti_diag) == kBoardSizeInt) {
        return player == 1 ? "A" : "B";
      }
    }

    return moves.size() == kMaxMoves ? "Draw" : "Pending";
  }
};

LC_PROBLEM(1275, "Find Winner On A Tic Tac Toe Game", "design", ctx) {
  ctx.test("example_1_a_wins_main_diagonal", [&] {
    Solution sol;
    const std::vector<std::vector<int>> moves = {
        {0, 0}, {2, 0}, {1, 1}, {2, 1}, {2, 2},
    };
    ctx.expect_eq(sol.tictactoe(moves), "A");
  });

  ctx.test("example_2_b_wins_anti_diagonal", [&] {
    Solution sol;
    const std::vector<std::vector<int>> moves = {
        {0, 0}, {1, 1}, {0, 1}, {0, 2}, {1, 0}, {2, 0},
    };
    ctx.expect_eq(sol.tictactoe(moves), "B");
  });

  ctx.test("example_3_draw", [&] {
    Solution sol;
    const std::vector<std::vector<int>> moves = {
        {0, 0}, {1, 1}, {2, 0}, {1, 0}, {1, 2},
        {2, 1}, {0, 1}, {0, 2}, {2, 2},
    };
    ctx.expect_eq(sol.tictactoe(moves), "Draw");
  });

  ctx.test("example_4_pending", [&] {
    Solution sol;
    const std::vector<std::vector<int>> moves = {{0, 0}, {1, 1}};
    ctx.expect_eq(sol.tictactoe(moves), "Pending");
  });

  ctx.test("row_win_and_ninth_move_win", [&] {
    Solution sol;
    // B wins along row 2.
    const std::vector<std::vector<int>> b_row_win = {
        {0, 0}, {2, 0}, {0, 1}, {2, 1}, {1, 1}, {2, 2},
    };
    ctx.expect_eq(sol.tictactoe(b_row_win), "B");

    // A wins on the very last (9th) move along column 2 — must return "A", not
    // "Draw".
    const std::vector<std::vector<int>> last_move_win = {
        {0, 2}, {0, 0}, {1, 2}, {0, 1}, {2, 0},
        {1, 0}, {1, 1}, {2, 1}, {2, 2},
    };
    ctx.expect_eq(sol.tictactoe(last_move_win), "A");
  });
}

}  // namespace
