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
// Examples:
//   Example 1:
//     Input:  board = [['X', '.', '.', 'X'], ['.', '.', '.', 'X'], ['.', '.', '.',
//             'X']]
//     Output: 2
//     Explanation:
//       - Scanning row-major, countBattleships finds 'X' at (0, 0), increments
//         ships to 1, and sinkShip replaces (0, 0) with '.' with no adjacent 'X'
//         neighbors.
//       - At (0, 3), countBattleships increments ships to 2 and sinkShip
//         flood-fills downward through (0, 3) -> (1, 3) -> (2, 3), sinking all
//         three cells to '.' so (1, 3) and (2, 3) are not recounted.
//       - Equivalently, countBattleshipsOnePass increments ships only at (0, 0) and
//         (0, 3) where neither the cell above nor the cell to the left is 'X',
//         returning 2.
//
//   Example 2:
//     Input:  board = [['X', '.', '.', 'X'], ['X', '.', '.', 'X'], ['.', 'X', '.',
//             'X']]
//     Output: 3
//     Explanation:
//       - At (0, 0), ships becomes 1 and sinkShip visits only 4-directional
//         neighbors, flipping (0, 0) and (1, 0) to '.' while leaving diagonal (2,
//         1) untouched.
//       - At (0, 3), ships becomes 2 and sinkShip sinks the vertical ship across
//         (0, 3), (1, 3), and (2, 3).
//       - When the scan reaches (2, 1), it is still 'X' so ships increments to 3,
//         matching countBattleshipsOnePass which finds ship heads at (0, 0), (0,
//         3), and (2, 1).
//
//   Example 3:
//     Input:  board = [['.']]
//     Output: 0
//     Explanation:
//       - Both countBattleships and countBattleshipsOnePass inspect cell (0, 0) and
//         observe '.' instead of 'X'.
//       - No flood-fill or ship-head increment occurs, so both methods return 0.
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
