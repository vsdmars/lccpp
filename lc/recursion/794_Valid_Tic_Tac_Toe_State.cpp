// LeetCode 794: Valid Tic-Tac-Toe State
// https://leetcode.com/problems/valid-tic-tac-toe-state/
//
// Problem:
//   Given a 3x3 Tic-Tac-Toe `board` as a vector of 3 strings of length 3
//   containing ' ', 'X', and 'O', return `true` if and only if the position is
//   reachable in a valid game. The game starts from an empty board, 'X' always
//   plays first, players alternate turns on empty cells, and play stops
//   immediately once either player forms three identical marks in a row,
//   column, or diagonal (or all 9 cells are filled).
//
// Examples:
//   Example 1:
//     Input:  board = ["XOX", "O O", "XOX"]
//     Output: true
//     Explanation:
//       - Count all marks on the 3x3 grid to get x_count = 4 and o_count = 4,
//         satisfying o_count <= x_count and x_count - o_count <= 1.
//       - Check all 3 rows, 3 columns, and 2 diagonals via hasWin for both 'X' and
//         'O', finding x_wins = false and o_wins = false.
//       - Since neither player has formed a winning line and move counts are valid,
//         return true.
//
//   Example 2:
//     Input:  board = ["XXX", "OOX", "OOX"]
//     Output: true
//     Explanation:
//       - Count the marks to find x_count = 5 and o_count = 4, which passes the
//         move-count invariant.
//       - Run hasWin to get x_wins = true (row 0 and column 2 both contain three
//         'X's) and o_wins = false.
//       - Verify the 'X'-win invariant x_count == o_count + 1 (5 == 4 + 1), which
//         holds and returns true.
//
//   Example 3:
//     Input:  board = ["O  ", "   ", "   "]
//     Output: false
//     Explanation:
//       - Scan the board to count x_count = 0 and o_count = 1.
//       - Evaluate o_count > x_count (1 > 0), which fails immediately and returns
//         false because 'X' must move first.
//
// Algorithm:
//   Checks the necessary and sufficient invariants of a reachable 3x3
//   Tic-Tac-Toe board directly without searching the game tree:
//     1. Count all 'X' and 'O' marks; because 'X' moves first and players
//        alternate, `x_count` must equal `o_count` or `o_count + 1`.
//     2. Check all 8 winning lines (3 rows, 3 columns, 2 diagonals) via
//        `hasWin` for both 'X' and 'O'.
//     3. If 'O' has a winning line, 'X' cannot also have won and 'O' must have
//        played the final move (`x_count == o_count`).
//     4. If 'X' has a winning line, 'X' must have played the final move
//        (`x_count == o_count + 1`). Note that 'X' may legally complete two
//        winning lines on its last move.
//   - Time Complexity:  O(1) (fixed 3x3 board and 8 winning lines).
//   - Space Complexity: O(1).
//
// Data Structures:
//   - `const std::vector<std::string>&`: Read-only 3x3 board representation.
//   - Scalar counters (`x_count`, `o_count`) and booleans (`x_wins`, `o_wins`,
//     `main_diag`, `anti_diag`) for constant-space invariant checks.
//
// Real-World Applications:
//   - Server-side anti-cheat and board-state validation in online turn-based
//     multiplayer games to reject tampered or unreachable client states.
//   - State-machine invariant verification and protocol sanity checks in
//     distributed workflows.
//   - Filtering invalid board configurations in automated test generators and
//     game-AI training pipelines.
//
// Related LeetCode Problems:
//   - #348 Design Tic-Tac-Toe [done] (O(1) incremental win detection on an n x n grid)
//   - #1275 Find Winner on a Tic Tac Toe Game [done] (simulating a move list to find
//     the winner or draw state)
//   - #36 Valid Sudoku [done] (verifying fixed-grid rule invariants without solving)
//   - #294 Flip Game II [new] (two-player turn-based game state analysis)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <vector>

namespace {

class Solution {
 public:
  // Checks the necessary and sufficient invariants of a reachable Tic-Tac-Toe
  // position:
  //   1. Move count: x_count == o_count or x_count == o_count + 1.
  //   2. If 'O' has a winning line, 'X' cannot also win and x_count == o_count.
  //   3. If 'X' has a winning line, x_count == o_count + 1.
  //
  // Time:  O(1)
  // Space: O(1)
  bool validTicTacToe(const std::vector<std::string>& board) {
    int x_count = 0;
    int o_count = 0;

    for (const auto& row : board) {
      for (char cell : row) {
        if (cell == 'X') {
          ++x_count;
        } else if (cell == 'O') {
          ++o_count;
        }
      }
    }

    if (o_count > x_count || (x_count - o_count) > 1) {
      return false;
    }

    const bool x_wins = hasWin(board, 'X');
    const bool o_wins = hasWin(board, 'O');

    if (o_wins) {
      return !x_wins && x_count == o_count;
    }
    if (x_wins) {
      return x_count == (o_count + 1);
    }
    return true;
  }

 private:
  static bool hasWin(const std::vector<std::string>& board, char player) {
    for (std::size_t i = 0uz; i < 3uz; ++i) {
      if (board[i][0] == player && board[i][1] == player &&
          board[i][2] == player) {
        return true;
      }
      if (board[0][i] == player && board[1][i] == player &&
          board[2][i] == player) {
        return true;
      }
    }

    const bool main_diag = board[0][0] == player && board[1][1] == player &&
                           board[2][2] == player;
    const bool anti_diag = board[0][2] == player && board[1][1] == player &&
                           board[2][0] == player;
    return main_diag || anti_diag;
  }
};

LC_PROBLEM(794, "Valid Tic Tac Toe State", "recursion", ctx) {
  Solution sol;

  ctx.test("example_1_o_goes_first", [&] {
    ctx.expect_false(sol.validTicTacToe({"O  ", "   ", "   "}));
  });

  ctx.test("example_2_x_plays_twice", [&] {
    ctx.expect_false(sol.validTicTacToe({"XOX", " X ", "   "}));
  });

  ctx.test("example_3_both_players_win", [&] {
    ctx.expect_false(sol.validTicTacToe({"XXX", "   ", "OOO"}));
  });

  ctx.test("example_4_valid_in_progress", [&] {
    ctx.expect_true(sol.validTicTacToe({"XOX", "O O", "XOX"}));
  });

  ctx.test("empty_and_winning_edge_cases", [&] {
    ctx.expect_true(sol.validTicTacToe({"   ", "   ", "   "}));
    // 'X' completes two lines simultaneously on its final move.
    ctx.expect_true(sol.validTicTacToe({"XXX", "OOX", "OOX"}));
    // 'X' already won, so 'O' could not have made an extra move.
    ctx.expect_false(sol.validTicTacToe({"XXX", "OO ", "O  "}));
    // 'O' wins validly on its turn (x_count == o_count).
    ctx.expect_true(sol.validTicTacToe({"XX ", "OOO", "X  "}));
    // 'O' won, so 'X' could not have continued playing.
    ctx.expect_false(sol.validTicTacToe({"XX ", "OOO", "XX "}));
  });
}

}  // namespace
