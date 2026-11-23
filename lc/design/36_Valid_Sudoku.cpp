// LeetCode 36: Valid Sudoku
// https://leetcode.com/problems/valid-sudoku/
//
// Problem:
//   Determine whether a partially filled 9 x 9 Sudoku board is valid according
//   to the three standard uniqueness rules (only filled cells '1'-'9' are
//   validated; empty cells '.' are ignored, and solvability is not required):
//     1) Each row contains digits 1-9 without repetition.
//     2) Each column contains digits 1-9 without repetition.
//     3) Each of the nine 3 x 3 sub-boxes contains digits 1-9 without
//        repetition.
//
// Examples:
//   Example 1:
//     Input:  board =
//             [['5','3','.','.','7','.','.','.','.'],['6','.','.','1','9','5','.','.','.'],['.','9','8','.','.','.','.','6','.'],['8','.','.','.','6','.','.','.','3'],['4','.','.','8','.','3','.','.','1'],['7','.','.','.','2','.','.','.','6'],['.','6','.','.','.','.','2','8','.'],['.','.','.','4','1','9','.','.','5'],['.','.','.','.','8','.','.','7','9']]
//     Output: true
//     Explanation:
//       - Initializes three 9-element bitmask arrays row_masks, col_masks, and
//         box_masks to 0 and scans each cell (r, c) once, skipping '.' cells.
//       - For each digit ch, computes bit = 1U << (ch - '1') and box = (r / 3) * 3
//         + (c / 3); for instance, (0, 0) = '5' sets bit 4 (16) in row_masks[0],
//         col_masks[0], and box_masks[0].
//       - Across all 30 filled cells, (row_masks[r] | col_masks[c] |
//         box_masks[box]) & bit is always 0, so the loop completes and returns
//         true.
//
//   Example 2:
//     Input:  board = same valid 9x9 board above except board[0][0] = '8'
//     Output: false
//     Explanation:
//       - At (0, 0) = '8', bit = 1U << 7 (128) is recorded in row_masks[0],
//         col_masks[0], and box_masks[0].
//       - Scanning proceeds until (2, 2) = '8', which maps to box = (2 / 3) * 3 +
//         (2 / 3) = 0.
//       - Because bit 7 is already set in box_masks[0], (row_masks[2] |
//         col_masks[2] | box_masks[0]) & (1U << 7) is non-zero and isValidSudoku
//         returns false immediately.
//
//   Example 3:
//     Input:  board = 9x9 empty '.' grid with board[3][3] = '2' and board[5][5] =
//             '2'
//     Output: false
//     Explanation:
//       - Cell (3, 3) = '2' sets bit 1 (1U << 1 = 2) in row_masks[3], col_masks[3],
//         and box_masks[4] since (3 / 3) * 3 + (3 / 3) = 4.
//       - All intermediate '.' cells are skipped until (5, 5) = '2', which shares
//         neither row nor column with (3, 3) but maps to the same box = (5 / 3) * 3
//         + (5 / 3) = 4.
//       - Checking (row_masks[5] | col_masks[5] | box_masks[4]) & 2 detects the bit
//         already set in box_masks[4] and returns false.
//
// Algorithm:
//   Single-Pass Bitmask Validation:
//   - Maintain three 9-element bitmask arrays: `row_masks`, `col_masks`, and
//     `box_masks`, where bit `d` (0 <= d < 9) records whether digit `d + 1`
//     has already appeared in that row, column, or 3 x 3 sub-box.
//   - Scan every cell (r, c) once. Skip empty '.' cells. For a digit cell,
//     validate that the character is in ['1', '9'], compute its bit mask
//     `1u << (ch - '1')` and sub-box index `(r / 3) * 3 + (c / 3)`.
//   - If that bit is already set in any of the three corresponding masks,
//     a duplicate exists and the board is invalid; otherwise set the bit in
//     all three masks.
//   - Time Complexity: O(1) (at most 81 cells inspected for a fixed 9 x 9
//     board; O(N^2) for a generalized N x N Sudoku).
//   - Space Complexity: O(1) (27 fixed 16-bit bitmasks on the stack; O(N) for
//     generalized N x N).
//
// Data Structures:
//   - Three stack-allocated `std::array<std::uint16_t, 9>` bitsets acting as
//     ultra-compact O(1) membership sets for rows, columns, and 3 x 3 boxes.
//
// Real-World Applications:
//   - Constraint satisfaction pre-checks (arc/unit consistency) in SAT/CSP
//     solvers and scheduling engines (timetabling, exam/shift assignment).
//   - Fast board-state validation in puzzle editors and interactive games.
//   - Bitmask-based duplicate/conflict detection across multiple simultaneous
//     grouping dimensions in databases and compilers (register allocation).
//
// Related LeetCode Problems:
//   - #37 Sudoku Solver [done] (backtracking solver extending bitmask constraint tracking)
//   - #2133 Check if Every Row and Column Contains All Numbers [new] (row/column Latin-square validation)
//   - #51 N-Queens [done] (multi-axis bitmask conflict checking on a grid)
//   - #217 Contains Duplicate [done] (fundamental duplicate detection)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr std::size_t kBoardSize = 9uz;
constexpr std::size_t kBoxSize = 3uz;

class Solution {
 public:
  // Validates that all filled cells in a 9 x 9 Sudoku board obey row, column,
  // and 3 x 3 sub-box uniqueness.
  // Time: O(1) (81 cells). Space: O(1) (27 bitmasks).
  bool isValidSudoku(const std::vector<std::vector<char>>& board) {
    if (board.size() != kBoardSize) {
      return false;
    }

    std::array<std::uint16_t, kBoardSize> row_masks{};
    std::array<std::uint16_t, kBoardSize> col_masks{};
    std::array<std::uint16_t, kBoardSize> box_masks{};

    for (std::size_t r = 0uz; r < kBoardSize; ++r) {
      if (board[r].size() != kBoardSize) {
        return false;
      }

      for (std::size_t c = 0uz; c < kBoardSize; ++c) {
        const char ch = board[r][c];
        if (ch == '.') {
          continue;
        }
        if (ch < '1' || ch > '9') {
          return false;
        }

        // Convert digit '1'..'9' into a single-bit flag (bits 0..8).
        const auto shift = static_cast<std::uint32_t>(
            static_cast<unsigned char>(ch) - static_cast<unsigned char>('1'));
        const auto bit = static_cast<std::uint16_t>(1U << shift);
        const std::size_t box = ((r / kBoxSize) * kBoxSize) + (c / kBoxSize);

        // Reject immediately if this digit already exists in the same row,
        // column, or 3 x 3 sub-box.
        const auto combined = static_cast<std::uint16_t>(
            row_masks[r] | col_masks[c] | box_masks[box]);
        if ((combined & bit) != 0U) {
          return false;
        }

        row_masks[r] = static_cast<std::uint16_t>(row_masks[r] | bit);
        col_masks[c] = static_cast<std::uint16_t>(col_masks[c] | bit);
        box_masks[box] = static_cast<std::uint16_t>(box_masks[box] | bit);
      }
    }

    return true;
  }
};

LC_PROBLEM(36, "Valid Sudoku", "design", ctx) {
  const std::vector<std::vector<char>> kValidBoard = {
      {'5', '3', '.', '.', '7', '.', '.', '.', '.'},
      {'6', '.', '.', '1', '9', '5', '.', '.', '.'},
      {'.', '9', '8', '.', '.', '.', '.', '6', '.'},
      {'8', '.', '.', '.', '6', '.', '.', '.', '3'},
      {'4', '.', '.', '8', '.', '3', '.', '.', '1'},
      {'7', '.', '.', '.', '2', '.', '.', '.', '6'},
      {'.', '6', '.', '.', '.', '.', '2', '8', '.'},
      {'.', '.', '.', '4', '1', '9', '.', '.', '5'},
      {'.', '.', '.', '.', '8', '.', '.', '7', '9'},
  };

  ctx.test("example_1_valid_board", [&] {
    Solution sol;
    ctx.expect_true(sol.isValidSudoku(kValidBoard));
  });

  ctx.test("example_2_duplicate_in_box_and_col", [&] {
    auto board = kValidBoard;
    // Changing (0,0) from '5' to '8' creates duplicates in column 0 and box 0.
    board[0][0] = '8';
    Solution sol;
    ctx.expect_false(sol.isValidSudoku(board));
  });

  ctx.test("duplicate_in_row_only", [&] {
    std::vector<std::vector<char>> board(kBoardSize,
                                         std::vector<char>(kBoardSize, '.'));
    board[0][0] = '4';
    board[0][8] = '4';
    Solution sol;
    ctx.expect_false(sol.isValidSudoku(board));
  });

  ctx.test("duplicate_in_col_only", [&] {
    std::vector<std::vector<char>> board(kBoardSize,
                                         std::vector<char>(kBoardSize, '.'));
    board[1][5] = '9';
    board[7][5] = '9';
    Solution sol;
    ctx.expect_false(sol.isValidSudoku(board));
  });

  ctx.test("duplicate_in_box_only", [&] {
    std::vector<std::vector<char>> board(kBoardSize,
                                         std::vector<char>(kBoardSize, '.'));
    board[3][3] = '2';
    board[5][5] = '2';
    Solution sol;
    ctx.expect_false(sol.isValidSudoku(board));
  });

  ctx.test("completely_empty_board_is_valid", [&] {
    const std::vector<std::vector<char>> board(
        kBoardSize, std::vector<char>(kBoardSize, '.'));
    Solution sol;
    ctx.expect_true(sol.isValidSudoku(board));
  });
}

}  // namespace
