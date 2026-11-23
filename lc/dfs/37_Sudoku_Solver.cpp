// LeetCode 37: Sudoku Solver
// https://leetcode.com/problems/sudoku-solver/
//
// Problem:
//   Write a program to solve a 9x9 Sudoku puzzle in place by filling all empty
//   cells (represented by '.'). A valid solution must place each digit '1'-'9'
//   exactly once in every row, every column, and every one of the nine 3x3
//   sub-boxes. The input board contains only digits '1'-'9' and '.', is always
//   9x9, and is guaranteed to have a single unique solution.
//
// Examples:
//   Example 1:
//     Input:  board =
//             [["5","3",".",".","7",".",".",".","."],["6",".",".","1","9","5",".",".","."],[".","9","8",".",".",".",".","6","."],["8",".",".",".","6",".",".",".","3"],["4",".",".","8",".","3",".",".","1"],["7",".",".",".","2",".",".",".","6"],[".","6",".",".",".",".","2","8","."],[".",".",".","4","1","9",".",".","5"],[".",".",".",".","8",".",".","7","9"]]
//     Output: [["5","3","4","6","7","8","9","1","2"],["6","7","2","1","9","5","3","4","8"],["1","9","8","3","4","2","5","6","7"],["8","5","9","7","6","1","4","2","3"],["4","2","6","8","5","3","7","9","1"],["7","1","3","9","2","4","8","5","6"],["9","6","1","5","3","7","2","8","4"],["2","8","7","4","1","9","6","3","5"],["3","4","5","2","8","6","1","7","9"]]
//     Explanation:
//       - Phase 1 scans the 30 initial clues into row_mask, col_mask, and box_mask
//         and collects all 51 '.' coordinates into empty_cells, starting at (0, 2).
//       - At idx = 0 for cell (0, 2) in box 0, used = row_mask[0] | col_mask[2] |
//         box_mask[0] forbids {3, 5, 6, 7, 8, 9}, leaving candidates '1', '2', and
//         '4'.
//       - backtrack tries '1' and '2' first, clearing board[0][2] and its mask bits
//         after downstream conflicts, before placing '4' and setting bit 3 in
//         row_mask[0], col_mask[2], and box_mask[0].
//       - Continuing recursively through all 51 empty cells reaches idx == 51 and
//         returns true immediately, leaving the solved 9x9 board in place.
//
//   Example 2:
//     Input:  board =
//             [[".",".","9","7","4","8",".",".","."],["7",".",".",".",".",".",".",".","."],[".","2",".","1",".","9",".",".","."],[".",".","7",".",".",".","2","4","."],[".","6","4",".","1",".","5","9","."],[".","9","8",".",".",".","3",".","."],[".",".",".","8",".","3",".","2","."],[".",".",".",".",".",".",".",".","6"],[".",".",".","2","7","5","9",".","."]]
//     Output: [["5","1","9","7","4","8","6","3","2"],["7","8","3","6","5","2","4","1","9"],["4","2","6","1","3","9","8","7","5"],["3","5","7","9","8","6","2","4","1"],["2","6","4","3","1","7","5","9","8"],["1","9","8","5","2","4","3","6","7"],["9","7","5","8","6","3","1","2","4"],["8","3","2","4","9","1","7","5","6"],["6","4","1","2","7","5","9","8","3"]]
//     Explanation:
//       - Phase 1 indexes the 27 initial clues into row_mask, col_mask, and
//         box_mask and records all 54 empty cells starting at (0, 0).
//       - At idx = 0 for cell (0, 0) in box 0, row_mask[0] blocks {4, 7, 8, 9},
//         col_mask[0] blocks {7}, and box_mask[0] blocks {2, 7, 9}, leaving
//         candidates '1', '3', '5', and '6'.
//       - backtrack prunes invalid branches using O(1) bitwise checks, unwinding
//         '1' and '3' on downstream dead ends before committing '5' at (0, 0).
//       - Once all 54 empty cells are assigned without bitmask conflict, idx == 54
//         returns true with the unique solved grid in board.
//
// Algorithm:
//   Backtracking DFS accelerated by O(1) bitmask constraint tracking:
//     1. Scan the 9x9 board once to record every pre-filled digit in three
//        9-bit occupancy masks (`row_mask[r]`, `col_mask[c]`, `box_mask[b]`),
//        where box index `b = (r / 3) * 3 + (c / 3)`, and collect the list of
//        empty cell coordinates.
//     2. Recursively assign the next empty cell `empty_cells[idx]`:
//        - Compute the union `used = row_mask[r] | col_mask[c] | box_mask[b]`.
//        - For each digit `d` in `0..8` whose bit is clear in `used`, place
//          `'1' + d` on the board, set bit `d` in all three masks, and recurse
//          to `idx + 1`.
//        - If recursion succeeds (`idx == empty_cells.size()`), unwind and
//          return `true` immediately, leaving the solved board in place.
//        - Otherwise restore `'.'` and clear bit `d` in all three masks
//          (backtrack).
//   Time Complexity:  O(9^E) worst-case upper bound where E <= 81 is the number
//                     of empty cells; bitmask pruning makes real puzzles solve
//                     in microseconds.
//   Space Complexity: O(E) = O(1) bounded space (at most 81 recursion frames
//                     and 27 bitmasks).
//
// Data Structures:
//   - Three fixed-size bitmask tables (`std::array<std::uint16_t, 9>`): track
//     which digits 1..9 are currently placed in each row, column, and 3x3 box
//     in O(1) time via bitwise operations.
//   - `std::vector<Cell>` (`empty_cells`): stores coordinates of empty cells so
//     each DFS level directly indexes the next cell without rescanning the grid.
//
// Real-World Applications:
//   - Constraint Satisfaction Problem (CSP) and SAT/SMT solvers used in formal
//     hardware/software verification and symbolic execution.
//   - Exam, classroom, and shift timetabling with hard resource-conflict
//     constraints.
//   - Wireless spectrum / frequency assignment and register allocation in
//     compilers (modeled as graph coloring with backtracking and pruning).
//   - Circuit layout and FPGA place-and-route conflict resolution.
//
// Related LeetCode Problems:
//   - #36 Valid Sudoku [done] (validation half of the same row/col/box masks)
//   - #51 N Queens [done] (classic backtracking with O(1) conflict bitmasks)
//   - #52 N-Queens II [new] (counting valid placements via bitmask backtracking)
//   - #79 Word Search [done] (grid backtracking with in-place state restoration)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr std::size_t kGridSize = 9;
constexpr std::size_t kBoxSize = 3;

// Computes the 3x3 sub-box index in [0, 8] for cell (row, col).
constexpr std::size_t boxIndex(std::size_t row, std::size_t col) {
  return ((row / kBoxSize) * kBoxSize) + (col / kBoxSize);
}

// Coordinate of an unassigned cell on the 9x9 Sudoku board.
struct Cell {
  std::size_t row = 0;
  std::size_t col = 0;
};

// Mutable bitmask and empty-cell state threaded through backtracking DFS.
struct SolverState {
  std::array<std::uint16_t, kGridSize> row_mask{};
  std::array<std::uint16_t, kGridSize> col_mask{};
  std::array<std::uint16_t, kGridSize> box_mask{};
  std::vector<Cell> empty_cells;
};

class Solution {
 public:
  // Solves the 9x9 Sudoku `board` in place using bitmask-accelerated DFS.
  // Time: O(9^E) worst case (E = empty cells), Space: O(E) = O(1) bounded.
  void solveSudoku(std::vector<std::vector<char>>& board) {
    SolverState state;
    state.empty_cells.reserve(kGridSize * kGridSize);

    // Phase 1: Index all initial clues into row/col/box bitmasks and gather
    // the coordinates of all empty cells to fill.
    for (std::size_t r = 0uz; r < kGridSize; ++r) {
      for (std::size_t c = 0uz; c < kGridSize; ++c) {
        if (board[r][c] == '.') {
          state.empty_cells.push_back(Cell{.row = r, .col = c});
          continue;
        }
        const auto digit = static_cast<unsigned>(board[r][c] - '1');
        const auto bit = static_cast<std::uint16_t>(1U << digit);
        state.row_mask[r] |= bit;
        state.col_mask[c] |= bit;
        state.box_mask[boxIndex(r, c)] |= bit;
      }
    }

    // Phase 2: Backtrack over `empty_cells` until all cells are validly filled.
    backtrack(board, state, 0uz);
  }

 private:
  // Attempts to fill `state.empty_cells[idx ..]` consistently with all masks.
  // Returns true as soon as the unique valid solution is found.
  static bool backtrack(std::vector<std::vector<char>>& board,
                        SolverState& state, std::size_t idx) {
    if (idx == state.empty_cells.size()) {
      return true;  // All empty cells assigned without conflict.
    }

    const std::size_t r = state.empty_cells[idx].row;
    const std::size_t c = state.empty_cells[idx].col;
    const std::size_t b = boxIndex(r, c);

    // Combined bitmask of digits already forbidden in row r, col c, or box b.
    const auto used = static_cast<std::uint16_t>(
        state.row_mask[r] | state.col_mask[c] | state.box_mask[b]);

    for (unsigned d = 0; d < kGridSize; ++d) {
      const auto bit = static_cast<std::uint16_t>(1U << d);
      if ((used & bit) != 0U) {
        continue;
      }

      // Place digit and mark occupancy across row, column, and 3x3 sub-box.
      board[r][c] = static_cast<char>('1' + static_cast<int>(d));
      state.row_mask[r] |= bit;
      state.col_mask[c] |= bit;
      state.box_mask[b] |= bit;

      if (backtrack(board, state, idx + 1)) {
        return true;
      }

      // Backtrack: clear digit and release bitmask bits.
      board[r][c] = '.';
      state.row_mask[r] &= static_cast<std::uint16_t>(~bit);
      state.col_mask[c] &= static_cast<std::uint16_t>(~bit);
      state.box_mask[b] &= static_cast<std::uint16_t>(~bit);
    }

    return false;
  }
};

// Verifies that `solved` is a complete, valid Sudoku preserving all clues from
// `original`.
bool isValidSolution(const std::vector<std::vector<char>>& original,
                     const std::vector<std::vector<char>>& solved) {
  constexpr std::uint16_t kAllNineDigits = (1U << kGridSize) - 1U;
  std::array<std::uint16_t, kGridSize> row_mask{};
  std::array<std::uint16_t, kGridSize> col_mask{};
  std::array<std::uint16_t, kGridSize> box_mask{};

  for (std::size_t r = 0uz; r < kGridSize; ++r) {
    for (std::size_t c = 0uz; c < kGridSize; ++c) {
      const char ch = solved[r][c];
      if (ch < '1' || ch > '9') {
        return false;
      }
      if (original[r][c] != '.' && original[r][c] != ch) {
        return false;
      }
      const auto bit =
          static_cast<std::uint16_t>(1U << static_cast<unsigned>(ch - '1'));
      row_mask[r] |= bit;
      col_mask[c] |= bit;
      box_mask[boxIndex(r, c)] |= bit;
    }
  }

  std::uint16_t combined = kAllNineDigits;
  for (std::size_t i = 0uz; i < kGridSize; ++i) {
    combined &= row_mask[i];
    combined &= col_mask[i];
    combined &= box_mask[i];
  }
  return combined == kAllNineDigits;
}

LC_PROBLEM(37, "Sudoku Solver", "dfs", ctx) {
  Solution sol;

  ctx.test("leetcode_example_1", [&] {
    const std::vector<std::vector<char>> original = {
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
    const std::vector<std::vector<char>> expected = {
        {'5', '3', '4', '6', '7', '8', '9', '1', '2'},
        {'6', '7', '2', '1', '9', '5', '3', '4', '8'},
        {'1', '9', '8', '3', '4', '2', '5', '6', '7'},
        {'8', '5', '9', '7', '6', '1', '4', '2', '3'},
        {'4', '2', '6', '8', '5', '3', '7', '9', '1'},
        {'7', '1', '3', '9', '2', '4', '8', '5', '6'},
        {'9', '6', '1', '5', '3', '7', '2', '8', '4'},
        {'2', '8', '7', '4', '1', '9', '6', '3', '5'},
        {'3', '4', '5', '2', '8', '6', '1', '7', '9'},
    };

    auto board = original;
    sol.solveSudoku(board);
    ctx.expect_eq(board, expected);
    ctx.expect_true(isValidSolution(original, board));
  });

  ctx.test("hard_sparse_puzzle", [&] {
    const std::vector<std::vector<char>> original = {
        {'.', '.', '9', '7', '4', '8', '.', '.', '.'},
        {'7', '.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '2', '.', '1', '.', '9', '.', '.', '.'},
        {'.', '.', '7', '.', '.', '.', '2', '4', '.'},
        {'.', '6', '4', '.', '1', '.', '5', '9', '.'},
        {'.', '9', '8', '.', '.', '.', '3', '.', '.'},
        {'.', '.', '.', '8', '.', '3', '.', '2', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '6'},
        {'.', '.', '.', '2', '7', '5', '9', '.', '.'},
    };
    const std::vector<std::vector<char>> expected = {
        {'5', '1', '9', '7', '4', '8', '6', '3', '2'},
        {'7', '8', '3', '6', '5', '2', '4', '1', '9'},
        {'4', '2', '6', '1', '3', '9', '8', '7', '5'},
        {'3', '5', '7', '9', '8', '6', '2', '4', '1'},
        {'2', '6', '4', '3', '1', '7', '5', '9', '8'},
        {'1', '9', '8', '5', '2', '4', '3', '6', '7'},
        {'9', '7', '5', '8', '6', '3', '1', '2', '4'},
        {'8', '3', '2', '4', '9', '1', '7', '5', '6'},
        {'6', '4', '1', '2', '7', '5', '9', '8', '3'},
    };

    auto board = original;
    sol.solveSudoku(board);
    ctx.expect_eq(board, expected);
    ctx.expect_true(isValidSolution(original, board));
  });
}

}  // namespace
