// LeetCode 79: Word Search
// https://leetcode.com/problems/word-search/
//
// Problem:
//   Given an m x n grid of characters `board` and a target string `word`,
//   determine whether `word` can be formed by a sequence of horizontally or
//   vertically adjacent cells in `board`. Each grid cell may be visited at
//   most once within a single path.
//
// Algorithm:
//   Depth-First Search (DFS) with backtracking.
//   Iterate over every cell (r, c) in the grid as a potential starting point.
//   Whenever board[r][c] == word[0], launch a recursive DFS that tries to
//   match word[idx] by exploring the four cardinal neighbors. To prevent
//   reusing a cell along the active path without allocating a separate
//   visited matrix, temporarily overwrite board[r][c] with a sentinel '\0'
//   before recursing and restore the original character on exit (both on
//   success and failure, leaving the input board intact).
//   Time Complexity:  O(m * n * 3^L), where L = word.size() (after the first
//                     character, each step has at most 3 unvisited neighbors).
//   Space Complexity: O(L) recursion call-stack depth (O(1) auxiliary space
//                     since visited state is tracked in-place on `board`).
//
// Data Structures:
//   - 2D character grid (`std::vector<std::vector<char>>`): stores the board
//     and doubles as an in-place visited-cell marker via a '\0' sentinel.
//   - Fixed 4-direction offset table (`std::array<std::pair<int, int>, 4>`):
//     enumerates orthogonal neighbors cleanly without heap allocation.
//   - Implicit DFS call stack: tracks the active path and backtracking points.
//
// Real-World Applications:
//   - Word-puzzle solvers (Boggle, Scrabble-adjacent path finding) and
//     constraint-based pattern search on 2D sensor/pixel grids.
//   - Path-constrained routing in PCB / VLSI layout grids where traces cannot
//     self-intersect.
//   - Subgraph / motif matching in biological contact maps and spatial graphs.
//
// Related LeetCode Problems:
//   - #212 Word Search II [done] (multi-word search on a grid using Trie + DFS)
//   - #200 Number Of Islands [done] (4-directional grid DFS flood fill)
//   - #130 Surrounded Regions [done] (border-seeded grid DFS)
//   - #980 Unique Paths III [new] (backtracking DFS visiting all empty cells)
//   - #1219 Path with Maximum Gold [done] (in-place backtracking DFS on a grid)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <string_view>
#include <utility>
#include <vector>

namespace {

// Orthogonal neighbor deltas: right, left, down, up.
constexpr std::array<std::pair<int, int>, 4> kDirs = {
    {{0, 1}, {0, -1}, {1, 0}, {-1, 0}}};

// Sentinel written into `board` while a cell is on the current DFS path.
constexpr char kVisited = '\0';

class Solution {
 public:
  // Returns true if `word` can be traced along a simple orthogonal path in
  // `board`. Temporarily mutates `board` during backtracking and always
  // restores every modified cell before returning.
  // Time: O(m * n * 3^L), Space: O(L) stack depth.
  bool exist(std::vector<std::vector<char>>& board, std::string_view word) {
    // Reject empty inputs and words longer than the total cell count.
    if (word.empty() || board.empty() || board[0].empty()) {
      return false;
    }
    const auto rows = board.size();
    const auto cols = board[0].size();
    if (word.size() > (rows * cols)) {
      return false;
    }

    const auto row_count = static_cast<int>(rows);
    const auto col_count = static_cast<int>(cols);

    // Try every cell matching word[0] as the start of a candidate path.
    for (int r = 0; r < row_count; ++r) {
      for (int c = 0; c < col_count; ++c) {
        if (dfs(board, word, r, c, 0uz, row_count, col_count)) {
          return true;
        }
      }
    }
    return false;
  }

 private:
  // Attempts to match word[widx..] starting at cell (r, c).
  // Invariant: 0 <= r < rows, 0 <= c < cols, and widx < word.size().
  static bool dfs(std::vector<std::vector<char>>& board, std::string_view word,
                  int r, int c, std::size_t widx, int rows, int cols) {
    const auto ur = static_cast<std::size_t>(r);
    const auto uc = static_cast<std::size_t>(c);
    if (board[ur][uc] != word[widx]) {
      return false;
    }

    // All characters of `word` have been matched along a valid simple path.
    if ((widx + 1uz) == word.size()) {
      return true;
    }

    // Mark cell visited in-place so deeper calls cannot reuse it.
    const char saved = board[ur][uc];
    board[ur][uc] = kVisited;

    bool found = false;
    for (const auto& [dr, dc] : kDirs) {
      const int nr = r + dr;
      const int nc = c + dc;
      if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
        if (dfs(board, word, nr, nc, widx + 1uz, rows, cols)) {
          found = true;
          break;
        }
      }
    }

    // Always restore the cell before unwinding so `board` stays unmodified.
    board[ur][uc] = saved;
    return found;
  }
};

LC_PROBLEM(79, "Word Search", "dfs", ctx) {
  Solution sol;

  ctx.test("example_abcced", [&] {
    std::vector<std::vector<char>> board = {
        {'A', 'B', 'C', 'E'},
        {'S', 'F', 'C', 'S'},
        {'A', 'D', 'E', 'E'},
    };
    const auto original = board;
    ctx.expect_true(sol.exist(board, "ABCCED"));
    ctx.expect_eq(board, original);
  });

  ctx.test("example_see", [&] {
    std::vector<std::vector<char>> board = {
        {'A', 'B', 'C', 'E'},
        {'S', 'F', 'C', 'S'},
        {'A', 'D', 'E', 'E'},
    };
    ctx.expect_true(sol.exist(board, "SEE"));
  });

  ctx.test("example_abcb_no_cell_reuse", [&] {
    std::vector<std::vector<char>> board = {
        {'A', 'B', 'C', 'E'},
        {'S', 'F', 'C', 'S'},
        {'A', 'D', 'E', 'E'},
    };
    ctx.expect_false(sol.exist(board, "ABCB"));
  });

  ctx.test("single_cell_and_edge_cases", [&] {
    std::vector<std::vector<char>> single = {{'A'}};
    ctx.expect_true(sol.exist(single, "A"));
    ctx.expect_false(sol.exist(single, "B"));
    ctx.expect_false(sol.exist(single, "AA"));
    ctx.expect_false(sol.exist(single, ""));

    std::vector<std::vector<char>> empty_board;
    ctx.expect_false(sol.exist(empty_board, "A"));
  });

  ctx.test("backtrack_across_shared_prefix", [&] {
    std::vector<std::vector<char>> board = {
        {'A', 'B', 'C', 'E'},
        {'S', 'F', 'E', 'S'},
        {'A', 'D', 'E', 'E'},
    };
    ctx.expect_true(sol.exist(board, "ABCESEEEFS"));
  });
}

}  // namespace
