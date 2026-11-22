// https://leetcode.com/problems/valid-tic-tac-toe-state/
//
// Given a 3x3 Tic-Tac-Toe board as an array of 3 strings containing ' ', 'X',
// and 'O', return true if and only if it is possible to reach this board state
// during a valid game where 'X' plays first, players alternate, and the game
// stops immediately once either player gets three in a row.

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
