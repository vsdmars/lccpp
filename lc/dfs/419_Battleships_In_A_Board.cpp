// LeetCode 419: Battleships In A Board
// https://leetcode.com/problems/battleships-in-a-board/
//
// Problem:
//   Given an m x n board where each cell is either a battleship piece ('X') or
//   empty water ('.'), return the number of battleships on the board. Every
//   battleship is a straight horizontal (1 x k) or vertical (k x 1) segment,
//   and at least one empty cell separates any two battleships horizontally or
//   vertically (no two ships are orthogonally adjacent).
//   Follow-up: Count the ships in a single pass with O(1) extra space and
//   without mutating the board.
//
// Algorithm:
//   1. Primary (DFS Flood-Fill): Iterate over every cell; whenever an unvisited
//      'X' is encountered, increment the ship count and launch a 4-directional
//      DFS that marks the entire connected ship component as '.' so it is never
//      recounted.
//      Time Complexity: O(m * n).
//      Space Complexity: O(max(m, n)) recursion stack depth (since valid ships
//      are 1 x k or k x 1 segments).
//   2. Follow-up (One-Pass Top-Left Head Counting): Because valid ships never
//      touch orthogonally and are axis-aligned rectangles of width 1, each ship
//      has a unique top-leftmost cell `(r, c)` where `board[r][c] == 'X'` and
//      neither `(r - 1, c)` nor `(r, c - 1)` is 'X'. Counting only these ship
//      heads solves the problem in one pass without modifying the board.
//      Time Complexity: O(m * n).
//      Space Complexity: O(1).
//
// Data Structures:
//   - std::vector<std::vector<char>>: 2D grid representing the board (mutated
//     in-place during DFS; read-only in the one-pass follow-up).
//   - std::array<std::pair<int, int>, 4>: compile-time cardinal direction
//     offsets for DFS traversal.
//
// Real-World Applications:
//   - Connected-component labeling in binary image processing and computer
//     vision (counting isolated objects or barcodes in scanned grids).
//   - PCB / chip-layout design-rule checking (counting non-overlapping wire
//     segments on a Manhattan routing grid).
//   - Spatial occupancy analysis in robotics and autonomous vehicle grid maps.
//
// Related LeetCode Problems:
//   - #200 Number of Islands [done] (general 2D connected-component counting via DFS)
//   - #695 Max Area of Island [done] (computing largest connected component via DFS)
//   - #463 Island Perimeter [done] (single-pass local neighbor inspection on a grid)
//   - #1254 Number of Closed Islands [done] (2D grid DFS component classification)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

// Cardinal neighbor offsets for 4-directional flood fill.
constexpr std::array<std::pair<int, int>, 4> kDirs = {
    {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};

class Solution {
 public:
  // Counts battleships via in-place DFS flood-fill, sinking each discovered
  // ship from 'X' to '.' so each connected component is counted once.
  //
  // Time: O(m * n); Space: O(max(m, n)) recursion stack depth.
  int countBattleships(std::vector<std::vector<char>>& board) {
    if (board.empty() || board[0].empty()) {
      return 0;
    }

    const std::size_t rows = board.size();
    const std::size_t cols = board[0].size();
    int ships = 0;

    for (std::size_t r = 0uz; r < rows; ++r) {
      for (std::size_t c = 0uz; c < cols; ++c) {
        if (board[r][c] == 'X') {
          ++ships;
          sinkShip(board, r, c);
        }
      }
    }
    return ships;
  }

  // Follow-up solution: counts battleships in one read-only pass using O(1)
  // auxiliary space by counting only each ship's unique top-leftmost cell.
  //
  // Invariant: A valid ship cell `(r, c)` is a ship head if and only if neither
  // the cell directly above `(r - 1, c)` nor the cell directly left `(r, c - 1)`
  // is 'X'.
  //
  // Time: O(m * n); Space: O(1).
  int countBattleshipsOnePass(const std::vector<std::vector<char>>& board) {
    if (board.empty() || board[0].empty()) {
      return 0;
    }

    const std::size_t rows = board.size();
    const std::size_t cols = board[0].size();
    int ships = 0;

    for (std::size_t r = 0uz; r < rows; ++r) {
      for (std::size_t c = 0uz; c < cols; ++c) {
        if (board[r][c] != 'X') {
          continue;
        }
        const bool has_above = (r > 0uz) && (board[r - 1uz][c] == 'X');
        const bool has_left = (c > 0uz) && (board[r][c - 1uz] == 'X');
        if (!has_above && !has_left) {
          ++ships;
        }
      }
    }
    return ships;
  }

 private:
  // Flood-fills an entire battleship starting at `(r, c)` by replacing 'X' with
  // '.'.
  static void sinkShip(std::vector<std::vector<char>>& board, std::size_t r,
                       std::size_t c) {
    const std::size_t rows = board.size();
    const std::size_t cols = board[0].size();
    board[r][c] = '.';

    for (const auto [dr, dc] : kDirs) {
      const int nr = static_cast<int>(r) + dr;
      const int nc = static_cast<int>(c) + dc;
      if (nr < 0 || nc < 0) {
        continue;
      }

      const auto ur = static_cast<std::size_t>(nr);
      const auto uc = static_cast<std::size_t>(nc);
      if (ur < rows && uc < cols && board[ur][uc] == 'X') {
        sinkShip(board, ur, uc);
      }
    }
  }
};

LC_PROBLEM(419, "Battleships In A Board", "dfs", ctx) {
  ctx.test("example1_two_ships", [&] {
    const std::vector<std::vector<char>> board = {
        {'X', '.', '.', 'X'}, {'.', '.', '.', 'X'}, {'.', '.', '.', 'X'}};
    std::vector<std::vector<char>> mutable_board = board;

    Solution sol;
    ctx.expect_eq(sol.countBattleships(mutable_board), 2);
    ctx.expect_eq(sol.countBattleshipsOnePass(board), 2);
  });

  ctx.test("diagonal_neighbors_are_separate_ships", [&] {
    // From the Python source build(): (1, 0) and (2, 1) only touch diagonally.
    const std::vector<std::vector<char>> board = {
        {'X', '.', '.', 'X'}, {'X', '.', '.', 'X'}, {'.', 'X', '.', 'X'}};
    std::vector<std::vector<char>> mutable_board = board;

    Solution sol;
    ctx.expect_eq(sol.countBattleships(mutable_board), 3);
    ctx.expect_eq(sol.countBattleshipsOnePass(board), 3);
  });

  ctx.test("empty_water_single_cell", [&] {
    const std::vector<std::vector<char>> board = {{'.'}};
    std::vector<std::vector<char>> mutable_board = board;

    Solution sol;
    ctx.expect_eq(sol.countBattleships(mutable_board), 0);
    ctx.expect_eq(sol.countBattleshipsOnePass(board), 0);
  });

  ctx.test("single_ship_cell", [&] {
    const std::vector<std::vector<char>> board = {{'X'}};
    std::vector<std::vector<char>> mutable_board = board;

    Solution sol;
    ctx.expect_eq(sol.countBattleships(mutable_board), 1);
    ctx.expect_eq(sol.countBattleshipsOnePass(board), 1);
  });

  ctx.test("horizontal_and_vertical_mix", [&] {
    const std::vector<std::vector<char>> board = {
        {'X', 'X', 'X', '.'}, {'.', '.', '.', 'X'}, {'X', '.', '.', 'X'}};
    std::vector<std::vector<char>> mutable_board = board;

    Solution sol;
    ctx.expect_eq(sol.countBattleships(mutable_board), 3);
    ctx.expect_eq(sol.countBattleshipsOnePass(board), 3);
  });
}

}  // namespace
