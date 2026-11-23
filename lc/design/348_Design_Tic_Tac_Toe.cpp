// LeetCode 348: Design Tic Tac Toe
// https://leetcode.com/problems/design-tic-tac-toe/
//
// Problem:
//   Design a Tic-Tac-Toe game played between two players (player 1 and
//   player 2) on an n x n board. Every `move(row, col, player)` places a
//   guaranteed-valid mark on an empty cell and returns:
//     - 0 if no one has won yet,
//     - 1 if player 1 wins on this move,
//     - 2 if player 2 wins on this move.
//   A player wins by completing any full row, column, main diagonal, or
//   anti-diagonal of length n. Follow-up: do better than O(n^2) per move.
//
// Examples:
//   Example 1:
//     Input:  TicTacToe(3); moves: (0,0,1), (0,2,2), (2,2,1), (1,1,2), (2,0,1),
//             (1,0,2), (2,1,1)
//     Output: [0, 0, 0, 0, 0, 0, 1]
//     Explanation:
//       - TicTacToe(3) zero-initializes rows_ = [0, 0, 0], cols_ = [0, 0, 0], diag_
//         = 0, and anti_diag_ = 0.
//       - Moves (0,0,1) and (0,2,2) cancel in row 0 (rows_[0] = 0) while setting
//         diag_ = 1 and anti_diag_ = -1; moves (2,2,1), (1,1,2), (2,0,1), and
//         (1,0,2) leave rows_ = [0, -2, 2], cols_ = [1, -1, 0], diag_ = 1, and
//         anti_diag_ = -1, all with absolute value < 3 so each returns 0.
//       - Final move (2,1,1) adds +1 to rows_[2] (reaching 3) and cols_[1]
//         (reaching 0); std::abs(rows_[2]) == 3 triggers and returns player 1.
//
//   Example 2:
//     Input:  TicTacToe(3); moves: (0,0,1), (0,2,2), (1,0,1), (1,1,2), (2,2,1),
//             (2,0,2)
//     Output: [0, 0, 0, 0, 0, 2]
//     Explanation:
//       - Player 1 adds +1 at (0,0), (1,0), and (2,2), while player 2 adds -1 at
//         (0,2) and (1,1) (both on row + col == 2), bringing anti_diag_ to -2 after
//         five moves with all outputs 0.
//       - Move (2,0,2) satisfies 2 + 0 == 2, updating rows_[2] to 0, cols_[0] to 1,
//         and anti_diag_ to -3.
//       - Because std::abs(anti_diag_) == 3, the move returns winning player 2.
//
//   Example 3:
//     Input:  TicTacToe(3); moves: (0,0,1), (0,1,2), (0,2,1), (1,1,2), (1,0,1),
//             (1,2,2), (2,1,1), (2,0,2), (2,2,1)
//     Output: [0, 0, 0, 0, 0, 0, 0, 0, 0]
//     Explanation:
//       - Each move adds +1 for player 1 or -1 for player 2 to its row, column, and
//         any matching diagonal (row == col or row + col == 2).
//       - After all nine cells are filled, the final counters are rows_ = [1, -1,
//         1], cols_ = [1, -1, 1], diag_ = 1, and anti_diag_ = -1.
//       - No counter ever reaches absolute value 3, so every move returns 0 for a
//         draw.
//
// Algorithm:
//   Two implementations are provided:
//   1. `TicTacToe` (Optimal O(1) time per move, O(n) space):
//      Assign player 1 a delta of +1 and player 2 a delta of -1. Maintain
//      running sums for each row, each column, the main diagonal (row == col),
//      and the anti-diagonal (row + col == n - 1). Because moves never
//      overwrite existing cells, a line reaches absolute sum `n` if and only
//      if all `n` cells belong to the same player.
//      Time Complexity: O(1) per `move()`, O(n) constructor.
//      Space Complexity: O(n).
//   2. `GridTicTacToe` (Direct grid directional scan matching the Python
//      reference): stores the n x n board and counts contiguous matching marks
//      along all 4 axes through `(row, col)`.
//      Time Complexity: O(n) per `move()`, O(n^2) constructor.
//      Space Complexity: O(n^2).
//
// Data Structures:
//   - `std::vector<int>` row and column counters (`rows_`, `cols_`) plus two
//     scalar diagonal counters (`diag_`, `anti_diag_`) for the O(1) design.
//   - 2D `std::vector<std::vector<int>>` board and `std::array` axis offsets
//     for the direct grid-scan implementation.
//
// Real-World Applications:
//   - Incremental invariant maintenance in game engines and rule validators
//     (avoiding full board re-scans on every state transition).
//   - Streaming aggregation and threshold monitoring (tracking per-bucket
//     counters in O(1) rather than recomputing over full windows).
//   - Constraint propagation in puzzle solvers (Sudoku / N-Queens row, column,
//     and diagonal occupancy tracking).
//
// Related LeetCode Problems:
//   - #1275 Find Winner on a Tic Tac Toe Game [done] (3x3 batch move evaluation)
//   - #794 Valid Tic-Tac-Toe State [done] (board state reachability invariants)
//   - #51 N-Queens [done] (row/col/diagonal line indexing on an n x n grid)
//   - #36 Valid Sudoku [done] (O(1) line occupancy tracking on a fixed grid)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdlib>
#include <vector>

namespace {

// Optimal O(1)-per-move Tic-Tac-Toe engine using signed line counters.
class TicTacToe {
 public:
  // Initializes an empty n x n game with zeroed line counters.
  // Time: O(n), Space: O(n).
  explicit TicTacToe(int n)
      : n_(n),
        rows_(static_cast<std::size_t>(n), 0),
        cols_(static_cast<std::size_t>(n), 0) {}

  // Places `player`'s mark (1 or 2) at `(row, col)` and returns the winning
  // player (1 or 2), or 0 if no player has won yet.
  // Time: O(1), Space: O(1).
  int move(int row, int col, int player) {
    // Encode player 1 as +1 and player 2 as -1 so a unanimous line has
    // absolute sum equal to n_.
    const int delta = (player == 1) ? 1 : -1;
    const auto r = static_cast<std::size_t>(row);
    const auto c = static_cast<std::size_t>(col);

    rows_[r] += delta;
    cols_[c] += delta;
    if (row == col) {
      diag_ += delta;
    }
    if (row + col == n_ - 1) {
      anti_diag_ += delta;
    }

    // Only lines passing through (row, col) could have just reached length n_.
    if (std::abs(rows_[r]) == n_ || std::abs(cols_[c]) == n_ ||
        std::abs(diag_) == n_ || std::abs(anti_diag_) == n_) {
      return player;
    }
    return 0;
  }

 private:
  int n_ = 0;
  std::vector<int> rows_;
  std::vector<int> cols_;
  int diag_ = 0;
  int anti_diag_ = 0;
};

// Direct board-based Tic-Tac-Toe matching the Python directional-ray scan.
class GridTicTacToe {
 public:
  // Allocates an n x n board initialized to empty (-1).
  // Time: O(n^2), Space: O(n^2).
  explicit GridTicTacToe(int n)
      : n_(n),
        grid_(static_cast<std::size_t>(n),
              std::vector<int>(static_cast<std::size_t>(n), -1)) {}

  // Places `player` at `(row, col)` and checks the 4 axes passing through it.
  // Time: O(n), Space: O(1).
  int move(int row, int col, int player) {
    grid_[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)] =
        player;
    return checkWin(row, col, player) ? player : 0;
  }

 private:
  // Four bidirectional axes: vertical, horizontal, anti-diagonal, main
  // diagonal.
  static constexpr std::array<std::array<int, 2>, 4> kAxes = {{
      {1, 0},
      {0, 1},
      {-1, 1},
      {1, 1},
  }};

  // Counts contiguous cells owned by `player` starting one step from (r, c)
  // along direction (dr, dc).
  [[nodiscard]] int countRay(int r, int c, int dr, int dc, int player) const {
    int cnt = 0;
    int nr = r + dr;
    int nc = c + dc;
    while (nr >= 0 && nr < n_ && nc >= 0 && nc < n_ &&
           grid_[static_cast<std::size_t>(nr)][static_cast<std::size_t>(nc)] ==
               player) {
      ++cnt;
      nr += dr;
      nc += dc;
    }
    return cnt;
  }

  // Returns true if any axis through (row, col) contains n_ contiguous marks.
  [[nodiscard]] bool checkWin(int row, int col, int player) const {
    return std::ranges::any_of(
        kAxes, [this, row, col, player](const auto& axis) {
          const auto [dr, dc] = axis;
          const int total = 1 + countRay(row, col, dr, dc, player) +
                            countRay(row, col, -dr, -dc, player);
          return total == n_;
        });
  }

  int n_ = 0;
  std::vector<std::vector<int>> grid_;
};

// Runs a move sequence on both implementations and verifies identical outputs.
void verifySequence(lc::TestContext& ctx, int n,
                    const std::vector<std::array<int, 4>>& script) {
  TicTacToe fast_game(n);
  GridTicTacToe grid_game(n);
  for (const auto& [r, c, player, expected] : script) {
    ctx.expect_eq(fast_game.move(r, c, player), expected);
    ctx.expect_eq(grid_game.move(r, c, player), expected);
  }
}

LC_PROBLEM(348, "Design Tic Tac Toe", "design", ctx) {
  ctx.test("leetcode example 3x3 row win", [&] {
    verifySequence(ctx, 3,
                   {
                       {0, 0, 1, 0},
                       {0, 2, 2, 0},
                       {2, 2, 1, 0},
                       {1, 1, 2, 0},
                       {2, 0, 1, 0},
                       {1, 0, 2, 0},
                       {2, 1, 1, 1},
                   });
  });

  ctx.test("python build 4x4 row win", [&] {
    verifySequence(ctx, 4,
                   {
                       {0, 0, 2, 0},
                       {1, 1, 1, 0},
                       {0, 1, 2, 0},
                       {1, 0, 1, 0},
                       {0, 2, 2, 0},
                       {1, 2, 1, 0},
                       {0, 3, 2, 2},
                   });
  });

  ctx.test("main diagonal and anti-diagonal wins", [&] {
    // Player 1 wins on main diagonal.
    verifySequence(ctx, 3,
                   {
                       {0, 0, 1, 0},
                       {0, 1, 2, 0},
                       {1, 1, 1, 0},
                       {0, 2, 2, 0},
                       {2, 2, 1, 1},
                   });
    // Player 2 wins on anti-diagonal.
    verifySequence(ctx, 3,
                   {
                       {0, 0, 1, 0},
                       {0, 2, 2, 0},
                       {1, 0, 1, 0},
                       {1, 1, 2, 0},
                       {2, 2, 1, 0},
                       {2, 0, 2, 2},
                   });
  });

  ctx.test("column win and 1x1 board", [&] {
    verifySequence(ctx, 3,
                   {
                       {0, 1, 2, 0},
                       {0, 0, 1, 0},
                       {1, 1, 2, 0},
                       {2, 2, 1, 0},
                       {2, 1, 2, 2},
                   });
    verifySequence(ctx, 1, {{0, 0, 1, 1}});
  });

  ctx.test("full board draw with no winner", [&] {
    verifySequence(ctx, 3,
                   {
                       {0, 0, 1, 0},
                       {0, 1, 2, 0},
                       {0, 2, 1, 0},
                       {1, 1, 2, 0},
                       {1, 0, 1, 0},
                       {1, 2, 2, 0},
                       {2, 1, 1, 0},
                       {2, 0, 2, 0},
                       {2, 2, 1, 0},
                   });
  });
}

}  // namespace
