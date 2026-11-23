// LeetCode 51: N Queens
// https://leetcode.com/problems/n-queens/
//
// Problem:
//   The n-queens puzzle asks to place n queens on an n x n chessboard so that
//   no two queens attack each other (no shared row, column, or diagonal).
//   Given an integer n (1 <= n <= 9), return all distinct valid board
//   configurations, where each configuration is a list of n strings of length
//   n using 'Q' for a queen and '.' for an empty square.
//
// Examples:
//   Example 1:
//     Input:  n = 4
//     Output: [[".Q..", "...Q", "Q...", "..Q."], ["..Q.", "Q...", "...Q", ".Q.."]]
//     Explanation:
//       - Initialize board to four "...." rows, col_used of size 4, and
//         anti_diag_used / main_diag_used of size 7 to 0.
//       - Row 0 at col = 0 finds no valid completion across rows 1-3 and
//         backtracks; placing at (0, 1) (anti_diag = 1, main_diag = 2) succeeds at
//         (1, 3), (2, 0), and (3, 2) to emit [".Q..", "...Q", "Q...", "..Q."].
//       - Backtracking to (0, 2) (anti_diag = 2, main_diag = 1) places queens at
//         (1, 0), (2, 3), and (3, 1) to emit ["..Q.", "Q...", "...Q", ".Q.."],
//         while (0, 3) fails to complete.
//
//   Example 2:
//     Input:  n = 1
//     Output: [["Q"]]
//     Explanation:
//       - Initialize board = ["."] and single-entry conflict arrays col_used,
//         anti_diag_used, and main_diag_used to 0.
//       - At row = 0, col = 0, all three conflict flags are 0, so 'Q' is placed at
//         (0, 0) and dfs recurses to row = 1.
//       - Because row == size (1 == 1), ["Q"] is appended to results and
//         backtracking clears the flags before returning [["Q"]].
//
//   Example 3:
//     Input:  n = 3
//     Output: []
//     Explanation:
//       - Initialize board to three "..." rows and diagonal conflict tables of size
//         5.
//       - In row 0, placing at (0, 0) forces (1, 2) at row 1, which blocks every
//         column in row 2; placing at (0, 1) attacks all three columns of row 1
//         immediately; placing at (0, 2) symmetrically forces (1, 0) and blocks row
//         2.
//       - No branch reaches row == 3, so all placements backtrack and solveNQueens
//         returns [].
//
// Algorithm:
//   Row-by-row backtracking DFS with O(1) conflict lookup tables:
//     - Because each row must contain exactly one queen, recursion advances
//       one row at a time (`row = 0 .. n - 1`), guaranteeing no row conflicts.
//     - Track attacked lines in three flat boolean arrays:
//         1. `col_used[col]` for column `col`.
//         2. `anti_diag_used[row + col]` for `/` anti-diagonals (all cells on
//            the same `/` diagonal share constant `row + col` in [0, 2n - 2]).
//         3. `main_diag_used[(row + (n - 1)) - col]` for `\` main diagonals
//            (all cells on the same `\` diagonal share constant `row - col`,
//            shifted by `n - 1` into [0, 2n - 2] to stay non-negative).
//     - Place `'Q'` at `(row, col)` when all three flags are clear, recurse to
//       `row + 1`, and restore `'.'` and clear flags on backtrack.
//     - When `row == n`, append a copy of the completed `board` to `results`.
//   Time Complexity:  O(n!) upper bound — at most n choices in row 0, n-1 in
//                     row 1, etc., pruned sharply by diagonal conflicts.
//   Space Complexity: O(n^2) for the mutable board plus O(n) for the conflict
//                     tables and recursion stack (excluding output).
//
// Data Structures:
//   - Three `std::vector<std::uint8_t>` conflict tables (`col_used`,
//     `anti_diag_used`, `main_diag_used`): O(1) attack detection without
//     scanning previously placed queens or allocating hash sets.
//   - Mutable board (`std::vector<std::string>`): updated in place via single
//     character toggles (`'Q'` / `'.'`) and snapshotted on each complete
//     placement.
//
// Real-World Applications:
//   - Constraint Satisfaction Problem (CSP) benchmarking and exact-cover
//     solver design (Knuth's Dancing Links / Algorithm X).
//   - VLSI chip floorplanning and non-interfering sensor/antenna placement on
//     2D grids.
//   - Parallel memory-bank skewing and conflict-free data-layout design for
//     SIMD / matrix hardware (row, column, and diagonal bank access).
//   - Scheduling jobs with multi-dimensional mutual-exclusion constraints.
//
// Related LeetCode Problems:
//   - #52 N-Queens II [new] (returns only the count of distinct n-queens boards)
//   - #37 Sudoku Solver [done] (grid backtracking with O(1) constraint masks)
//   - #46 Permutations [done] (column placement without diagonal restrictions)
//   - #1001 Grid Illumination [done] (same row/col/diagonal line-indexing math)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace {

class Solution {
 public:
  // Returns all distinct valid solutions to the n-queens puzzle.
  // Time: O(n!), Space: O(n^2) working board + O(n) conflict tables.
  std::vector<std::vector<std::string>> solveNQueens(int n) {
    if (n <= 0) {
      return {};
    }

    const auto size = static_cast<std::size_t>(n);
    const std::size_t diag_count = (2uz * size) - 1uz;

    std::vector<std::vector<std::string>> results;
    std::vector<std::string> board(size, std::string(size, '.'));
    std::vector<std::uint8_t> col_used(size, 0);
    std::vector<std::uint8_t> anti_diag_used(diag_count, 0);
    std::vector<std::uint8_t> main_diag_used(diag_count, 0);

    // DFS invariant: rows `0 .. row - 1` each contain one non-attacking queen
    // whose column and both diagonals are marked in the three lookup tables.
    auto dfs = [&](this auto&& self, std::size_t row) -> void {
      if (row == size) {
        results.push_back(board);
        return;
      }

      for (std::size_t col = 0uz; col < size; ++col) {
        // `/` anti-diagonal index is `row + col` in [0, 2*size - 2].
        // `\` main-diagonal index is `(row - col) + (size - 1)` in [0, 2*size - 2],
        // written as `(row + (size - 1)) - col` to stay unsigned without underflow.
        const std::size_t anti_diag = row + col;
        const std::size_t main_diag = (row + (size - 1uz)) - col;

        if (col_used[col] != 0 || anti_diag_used[anti_diag] != 0 ||
            main_diag_used[main_diag] != 0) {
          continue;
        }

        // Place queen and mark column + both diagonals as attacked.
        board[row][col] = 'Q';
        col_used[col] = 1;
        anti_diag_used[anti_diag] = 1;
        main_diag_used[main_diag] = 1;

        self(row + 1uz);

        // Backtrack: remove queen and release column + diagonal flags.
        board[row][col] = '.';
        col_used[col] = 0;
        anti_diag_used[anti_diag] = 0;
        main_diag_used[main_diag] = 0;
      }
    };

    dfs(0uz);
    return results;
  }
};

// Validates that a candidate n x n board has exactly one queen per row and
// column and no two queens share any diagonal.
bool isValidBoard(const std::vector<std::string>& board, std::size_t size) {
  if (board.size() != size) {
    return false;
  }
  const std::size_t diag_count = (2uz * size) - 1uz;
  std::vector<std::uint8_t> col_used(size, 0);
  std::vector<std::uint8_t> anti_diag_used(diag_count, 0);
  std::vector<std::uint8_t> main_diag_used(diag_count, 0);

  for (std::size_t r = 0uz; r < size; ++r) {
    if (board[r].size() != size) {
      return false;
    }
    int queens_in_row = 0;
    for (std::size_t c = 0uz; c < size; ++c) {
      if (board[r][c] == '.') {
        continue;
      }
      if (board[r][c] != 'Q') {
        return false;
      }
      ++queens_in_row;
      const std::size_t anti_diag = r + c;
      const std::size_t main_diag = (r + (size - 1uz)) - c;
      if (col_used[c] != 0 || anti_diag_used[anti_diag] != 0 ||
          main_diag_used[main_diag] != 0) {
        return false;
      }
      col_used[c] = 1;
      anti_diag_used[anti_diag] = 1;
      main_diag_used[main_diag] = 1;
    }
    if (queens_in_row != 1) {
      return false;
    }
  }
  return true;
}

LC_PROBLEM(51, "N Queens", "dfs", ctx) {
  Solution sol;

  ctx.test("example_1_n_4", [&] {
    const std::vector<std::vector<std::string>> expected = {
        {".Q..", "...Q", "Q...", "..Q."},
        {"..Q.", "Q...", "...Q", ".Q.."},
    };
    ctx.expect_eq(sol.solveNQueens(4), expected);
  });

  ctx.test("example_2_n_1", [&] {
    const std::vector<std::vector<std::string>> expected = {{"Q"}};
    ctx.expect_eq(sol.solveNQueens(1), expected);
  });

  ctx.test("no_solutions_for_n_2_and_n_3", [&] {
    ctx.expect_true(sol.solveNQueens(2).empty());
    ctx.expect_true(sol.solveNQueens(3).empty());
  });

  ctx.test("n_5_has_10_valid_solutions", [&] {
    const auto solutions = sol.solveNQueens(5);
    ctx.expect_eq(solutions.size(), 10uz);
    for (const auto& board : solutions) {
      ctx.expect_true(isValidBoard(board, 5uz));
    }
  });

  ctx.test("classic_8_queens_has_92_valid_solutions", [&] {
    const auto solutions = sol.solveNQueens(8);
    ctx.expect_eq(solutions.size(), 92uz);
    for (const auto& board : solutions) {
      ctx.expect_true(isValidBoard(board, 8uz));
    }
  });
}

}  // namespace
