// LeetCode 723: Candy Crush
// https://leetcode.com/problems/candy-crush/
//
// Problem:
//   Given an `m x n` integer grid `board` representing candies (positive
//   integers indicate candy types; `0` represents an empty cell), repeatedly
//   stabilize the board according to two simultaneous rules until no more
//   crushes are possible:
//     1. Crush: any 3 or more identical non-zero candies adjacent horizontally
//        or vertically are eliminated simultaneously (set to `0`).
//     2. Gravity: in each column, remaining non-zero candies fall straight down
//        to fill empty spaces, leaving `0`s at the top of the column.
//   Return the final stable board.
//   Constraints: 3 <= m, n <= 50; initial board[r][c] in [1, 2000].
//
// Examples:
//   Example 1:
//     Input:  board = [[4, 8, 5], [1, 2, 3], [6, 2, 7], [8, 2, 8]]
//     Output: [[0, 0, 0], [4, 0, 5], [1, 0, 3], [6, 0, 7]]
//     Explanation:
//       - Round 1 markMatches scans all cells and detects a vertical triple of 2s
//         at (1, 1), (2, 1), and (3, 1), setting their entries in crush_mask to 1.
//       - Round 1 clearMarkedCells zeroes column 1 rows 1..3, and applyGravity
//         drops 8 from (0, 1) down to (3, 1), forming row 3 = [8, 8, 8].
//       - Round 2 markMatches finds the horizontal triple of 8s at (3, 0), (3, 1),
//         and (3, 2) and flags them in crush_mask.
//       - Round 2 clears row 3 and applyGravity compacts columns 0 and 2 downward
//         into rows 1..3 while filling row 0 with 0s.
//       - Round 3 markMatches finds no remaining horizontal or vertical triples,
//         terminating the loop and returning [[0, 0, 0], [4, 0, 5], [1, 0, 3], [6,
//         0, 7]].
//
//   Example 2:
//     Input:  board = [[5, 1, 2], [5, 3, 4], [5, 5, 5]]
//     Output: [[0, 0, 0], [0, 1, 2], [0, 3, 4]]
//     Explanation:
//       - Round 1 markMatches flags both the horizontal triple at (2, 0)..(2, 2)
//         and the vertical triple at (0, 0)..(2, 0) in crush_mask without mutating
//         board mid-scan.
//       - clearMarkedCells zeroes all five marked cells simultaneously, leaving
//         only (0, 1)=1, (0, 2)=2, (1, 1)=3, and (1, 2)=4 non-zero.
//       - applyGravity compacts column 1 ([1, 3]) and column 2 ([2, 4]) down to
//         rows 1..2 while zero-filling row 0 and keeping column 0 all zeros.
//       - Round 2 markMatches finds no runs of 3 identical candies and returns
//         false, returning the final board.
//
//   Example 3:
//     Input:  board = [[1, 2, 1], [2, 1, 2], [1, 2, 1]]
//     Output: [[1, 2, 1], [2, 1, 2], [1, 2, 1]]
//     Explanation:
//       - Allocate the 3 * 3 crush_mask initialized to 0 and invoke markMatches for
//         Round 1.
//       - Scanning every cell finds no horizontal triple (c >= 2) or vertical
//         triple (r >= 2) of identical non-zero candies, so markMatches returns
//         false immediately.
//       - Because the while loop never executes, the already stable board is
//         returned unmodified.
//
// Algorithm:
//   Run an iterative simulation loop where each round performs two decoupled
//   phases:
//     1. Simultaneous Match Marking (`markMatches`): scan every cell `(r, c)`.
//        If `board[r][c] != 0`, check whether it completes a horizontal triple
//        `(r, c-2..c)` or a vertical triple `(r-2..r, c)`. Record all matching
//        coordinates in a separate boolean mask so intersecting horizontal and
//        vertical runs (L-, T-, or +-shapes) do not erase each other mid-scan.
//        If no cell is marked, the board is stable and we terminate.
//     2. Clear & Gravity Compaction (`clearAndApplyGravity`): zero out all
//        marked cells, then compact each column bottom-up with a two-pointer
//        write cursor, pulling non-zero candies downward and zero-filling the
//        remaining upper rows.
//   Time Complexity:  O((m * n)^2) worst case, since each round removes at
//                     least 3 candies (so at most O(m * n) rounds), and each
//                     round scans the grid in O(m * n).
//   Space Complexity: O(m * n) for the per-round crush mask (reused across
//                     iterations to avoid repeated allocations).
//
// Data Structures:
//   - 2D Grid (`std::vector<std::vector<int>>`): stores the candy board and is
//     updated in place across simulation rounds.
//   - Flat Crush Mask (`std::vector<std::uint8_t>`): indexed by `(r * cols) + c`
//     to record cells scheduled for simultaneous elimination in the current
//     round without hash-set overhead.
//
// Real-World Applications:
//   - Match-3 and puzzle game engines: implementing deterministic cascading
//     tile-clearing and gravity physics loops.
//   - Cellular automata and physical grid simulations: synchronous state
//     transitions using double-buffered/masked updates before applying gravity
//     or compaction.
//   - Memory / storage defragmentation: column-wise in-place compaction using
//     read/write pointers to eliminate holes while preserving order.
//
// Related LeetCode Problems:
//   - #283 Move Zeroes [done] (two-pointer in-place array compaction preserving order)
//   - #289 Game of Life [done] (simultaneous 2D grid state updates)
//   - #1861 Rotating the Box [done] (2D grid gravity simulation)
//   - #73 Set Matrix Zeroes [done] (deferred marking before clearing a 2D matrix)
//   - #488 Zuma Game [new] (cascading 3-in-a-row elimination mechanics)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Repeatedly crushes all horizontal and vertical runs of >= 3 identical
  // candies and applies downward gravity until the board reaches stability.
  // Time: O((m * n)^2) worst case, Space: O(m * n).
  std::vector<std::vector<int>> candyCrush(
      std::vector<std::vector<int>> board) {
    if (board.empty() || board[0].empty()) {
      return board;
    }

    const std::size_t rows = board.size();
    const std::size_t cols = board[0].size();
    // Reuse a single flat mask buffer across rounds to avoid repeated heap
    // allocations.
    std::vector<std::uint8_t> crush_mask(rows * cols, 0);

    while (markMatches(board, crush_mask)) {
      clearMarkedCells(board, crush_mask);
      applyGravity(board);
    }

    return board;
  }

 private:
  // Scans the board for horizontal and vertical runs of at least 3 identical
  // non-zero candies and flags every participating cell in `crush_mask`.
  // Returns true if at least one candy will be crushed this round.
  static bool markMatches(const std::vector<std::vector<int>>& board,
                          std::vector<std::uint8_t>& crush_mask) {
    const std::size_t rows = board.size();
    const std::size_t cols = board[0].size();
    std::ranges::fill(crush_mask, static_cast<std::uint8_t>(0));

    bool has_crush = false;
    for (std::size_t r = 0uz; r < rows; ++r) {
      for (std::size_t c = 0uz; c < cols; ++c) {
        const int candy = board[r][c];
        if (candy == 0) {
          continue;
        }

        // Check horizontal triple ending at (r, c). Marking all overlapping
        // 3-cell windows automatically covers runs of length >= 3.
        if (c >= 2uz && board[r][c - 1] == candy && board[r][c - 2] == candy) {
          crush_mask[(r * cols) + c] = 1;
          crush_mask[(r * cols) + (c - 1)] = 1;
          crush_mask[(r * cols) + (c - 2)] = 1;
          has_crush = true;
        }

        // Check vertical triple ending at (r, c).
        if (r >= 2uz && board[r - 1][c] == candy && board[r - 2][c] == candy) {
          crush_mask[(r * cols) + c] = 1;
          crush_mask[((r - 1) * cols) + c] = 1;
          crush_mask[((r - 2) * cols) + c] = 1;
          has_crush = true;
        }
      }
    }

    return has_crush;
  }

  // Zeroes out every cell flagged in `crush_mask` simultaneously.
  static void clearMarkedCells(std::vector<std::vector<int>>& board,
                               const std::vector<std::uint8_t>& crush_mask) {
    const std::size_t rows = board.size();
    const std::size_t cols = board[0].size();

    for (std::size_t r = 0uz; r < rows; ++r) {
      for (std::size_t c = 0uz; c < cols; ++c) {
        if (crush_mask[(r * cols) + c] != 0) {
          board[r][c] = 0;
        }
      }
    }
  }

  // Compacts each column downward so non-zero candies settle to the bottom
  // while preserving their vertical order, and fills vacated top cells with 0.
  static void applyGravity(std::vector<std::vector<int>>& board) {
    const std::size_t rows = board.size();
    const std::size_t cols = board[0].size();

    for (std::size_t c = 0uz; c < cols; ++c) {
      // `write_row` is one past the next bottom-most slot to fill, keeping all
      // loop indices strictly unsigned without underflow.
      std::size_t write_row = rows;
      for (std::size_t r = rows; r > 0uz; --r) {
        if (board[r - 1][c] != 0) {
          --write_row;
          board[write_row][c] = board[r - 1][c];
        }
      }

      // Fill all remaining cells above the settled candies with empty (0).
      for (std::size_t r = 0uz; r < write_row; ++r) {
        board[r][c] = 0;
      }
    }
  }
};

LC_PROBLEM(723, "Candy Crush", "array", ctx) {
  Solution sol;

  ctx.test("example_1_cascading_crushes", [&] {
    const std::vector<std::vector<int>> board = {
        {110, 5, 112, 113, 114}, {210, 211, 5, 213, 214},
        {310, 311, 3, 313, 314}, {410, 411, 412, 5, 414},
        {5, 1, 512, 3, 3},       {610, 4, 1, 613, 614},
        {710, 1, 2, 713, 714},   {810, 1, 2, 1, 1},
        {1, 1, 2, 2, 2},         {4, 1, 4, 4, 1014},
    };
    const std::vector<std::vector<int>> expected = {
        {0, 0, 0, 0, 0},         {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},         {110, 0, 0, 0, 114},
        {210, 0, 0, 0, 214},     {310, 0, 0, 113, 314},
        {410, 0, 0, 213, 414},   {610, 211, 112, 313, 614},
        {710, 311, 412, 613, 714}, {810, 411, 512, 713, 1014},
    };
    ctx.expect_eq(sol.candyCrush(board), expected);
  });

  ctx.test("already_stable_board", [&] {
    const std::vector<std::vector<int>> board = {
        {1, 2, 1},
        {2, 1, 2},
        {1, 2, 1},
    };
    ctx.expect_eq(sol.candyCrush(board), board);
  });

  ctx.test("all_identical_clears_entire_board", [&] {
    const std::vector<std::vector<int>> board = {
        {7, 7, 7},
        {7, 7, 7},
        {7, 7, 7},
    };
    const std::vector<std::vector<int>> expected = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
    };
    ctx.expect_eq(sol.candyCrush(board), expected);
  });

  ctx.test("intersecting_l_shape_crushed_simultaneously", [&] {
    // Row 2 has three 5s and col 0 has three 5s sharing corner (2, 0).
    // Both runs must be cleared together in the same round.
    const std::vector<std::vector<int>> board = {
        {5, 1, 2},
        {5, 3, 4},
        {5, 5, 5},
    };
    const std::vector<std::vector<int>> expected = {
        {0, 0, 0},
        {0, 1, 2},
        {0, 3, 4},
    };
    ctx.expect_eq(sol.candyCrush(board), expected);
  });

  ctx.test("gravity_triggers_second_round_chain_reaction", [&] {
    // Round 1 crushes column 1's three 2s, allowing the 8 at (0, 1) to fall
    // to (3, 1), which creates a horizontal triple of 8s along row 3 that
    // crushes in Round 2.
    const std::vector<std::vector<int>> cascade = {
        {4, 8, 5},
        {1, 2, 3},
        {6, 2, 7},
        {8, 2, 8},
    };
    const std::vector<std::vector<int>> expected = {
        {0, 0, 0},
        {4, 0, 5},
        {1, 0, 3},
        {6, 0, 7},
    };
    ctx.expect_eq(sol.candyCrush(cascade), expected);
  });
}

}  // namespace
