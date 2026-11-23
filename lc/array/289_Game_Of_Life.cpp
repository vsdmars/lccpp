// LeetCode 289: Game Of Life
// https://leetcode.com/problems/game-of-life/
//
// Problem:
//   Given an `m x n` grid `board` representing Conway's Game of Life where each
//   cell is initially live (`1`) or dead (`0`), compute the next generation
//   simultaneously for all cells according to the four rules using 8-directional
//   neighbors:
//     1. Live cell with < 2 live neighbors dies (under-population).
//     2. Live cell with 2 or 3 live neighbors stays live.
//     3. Live cell with > 3 live neighbors dies (over-population).
//     4. Dead cell with exactly 3 live neighbors becomes live (reproduction).
//   Follow-up: update `board` in-place using O(1) extra memory.
//
// Algorithm:
//   Two-pass in-place state encoding:
//   Because every cell update depends on its neighbors' *original* states, we
//   encode both the previous state and the next state in the same integer cell
//   during Pass 1 using four parity-preserving states:
//     - 0 (`kDead`):       was dead (0), stays dead (0)
//     - 1 (`kLive`):       was live (1), stays live (1)
//     - 2 (`kDeadToLive`): was dead (0), becomes live (1)  -> `2 % 2 == 0`
//     - 3 (`kLiveToDead`): was live (1), becomes dead (0)  -> `3 % 2 == 1`
//   Because `state % 2` always equals the cell's *original* liveness, Pass 1
//   can mutate cells freely without corrupting neighbor counts for unvisited
//   cells. Pass 2 decodes temporary states `2 -> 1` and `3 -> 0`.
//   Time Complexity:  O(m * n) — visits each cell twice and inspects up to 8
//                     neighbors per cell.
//   Space Complexity: O(1) extra space (modifies `board` in-place).
//
// Data Structures:
//   - `std::vector<std::vector<int>>&` (`board`): 2D grid storing both current
//     and transitional cell states in-place.
//   - `std::array<std::pair<int, int>, 8>` (`kDirections`): fixed-size lookup
//     table for the 8 Moore neighborhood offsets.
//
// Real-World Applications:
//   - Cellular automata and physical stencil simulations (heat diffusion,
//     fluid grids, wildfire spread) where double-buffering memory is costly and
//     bit-packed state transitions reduce memory bandwidth.
//   - Image morphology filters (erosion, dilation, thinning) executed in-place
//     over 8-connected pixel neighborhoods.
//   - Ecological population modeling and biological morphogenesis simulations.
//
// Related LeetCode Problems:
//   - #73 Set Matrix Zeroes [done] (in-place 2D grid marking to avoid O(m * n) copies)
//   - #48 Rotate Image [done] (in-place 2D matrix transformation)
//   - #957 Prison Cells After N Days [new] (1D cellular automaton state transitions)
//   - #661 Image Smoother [new] (8-neighbor convolution with in-place bit-packing)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Advances `board` by one generation of Conway's Game of Life in-place.
  //
  // Time Complexity:  O(m * n)
  // Space Complexity: O(1)
  void gameOfLife(std::vector<std::vector<int>>& board) {
    if (board.empty() || board[0].empty()) {
      return;
    }

    const std::size_t rows = board.size();
    const std::size_t cols = board[0].size();

    // Pass 1: Compute next generation while preserving original state in bit 0
    // (`cell % 2`).
    for (std::size_t r = 0uz; r < rows; ++r) {
      for (std::size_t c = 0uz; c < cols; ++c) {
        const int live_neighbors = countLiveNeighbors(board, r, c);
        if (board[r][c] == kLive) {
          if (live_neighbors < 2 || live_neighbors > 3) {
            board[r][c] = kLiveToDead;
          }
        } else if (live_neighbors == 3) {
          board[r][c] = kDeadToLive;
        }
      }
    }

    // Pass 2: Finalize transitional states into pure binary {0, 1} values.
    for (auto& row : board) {
      for (int& cell : row) {
        if (cell == kLiveToDead) {
          cell = kDead;
        } else if (cell == kDeadToLive) {
          cell = kLive;
        }
      }
    }
  }

 private:
  static constexpr int kDead = 0;
  static constexpr int kLive = 1;
  static constexpr int kDeadToLive = 2;  // 2 % 2 == 0 (originally dead)
  static constexpr int kLiveToDead = 3;  // 3 % 2 == 1 (originally live)

  static constexpr std::array<std::pair<int, int>, 8> kDirections = {{
      {0, 1},
      {0, -1},
      {1, 0},
      {-1, 0},
      {1, 1},
      {1, -1},
      {-1, -1},
      {-1, 1},
  }};

  // Counts how many of the up to 8 neighbors of `(row, col)` were live in the
  // original generation (identified by odd state value: `kLive` or
  // `kLiveToDead`).
  static int countLiveNeighbors(const std::vector<std::vector<int>>& board,
                                std::size_t row, std::size_t col) {
    const auto rows = static_cast<int>(board.size());
    const auto cols = static_cast<int>(board[0].size());
    int live_count = 0;

    for (const auto& [dr, dc] : kDirections) {
      const int nr = static_cast<int>(row) + dr;
      const int nc = static_cast<int>(col) + dc;
      if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
        const int neighbor =
            board[static_cast<std::size_t>(nr)][static_cast<std::size_t>(nc)];
        if ((neighbor % 2) == 1) {
          ++live_count;
        }
      }
    }
    return live_count;
  }
};

LC_PROBLEM(289, "Game Of Life", "array", ctx) {
  Solution sol;

  ctx.test("example_1_4x3_board", [&] {
    std::vector<std::vector<int>> board = {
        {0, 1, 0},
        {0, 0, 1},
        {1, 1, 1},
        {0, 0, 0},
    };
    const std::vector<std::vector<int>> expected = {
        {0, 0, 0},
        {1, 0, 1},
        {0, 1, 1},
        {0, 1, 0},
    };
    sol.gameOfLife(board);
    ctx.expect_eq(board, expected);
  });

  ctx.test("example_2_2x2_still_life_block", [&] {
    std::vector<std::vector<int>> board = {
        {1, 1},
        {1, 0},
    };
    const std::vector<std::vector<int>> expected = {
        {1, 1},
        {1, 1},
    };
    sol.gameOfLife(board);
    ctx.expect_eq(board, expected);
  });

  ctx.test("blinker_oscillator_period_2", [&] {
    const std::vector<std::vector<int>> horizontal = {
        {0, 0, 0},
        {1, 1, 1},
        {0, 0, 0},
    };
    const std::vector<std::vector<int>> vertical = {
        {0, 1, 0},
        {0, 1, 0},
        {0, 1, 0},
    };
    std::vector<std::vector<int>> board = horizontal;
    sol.gameOfLife(board);
    ctx.expect_eq(board, vertical);
    sol.gameOfLife(board);
    ctx.expect_eq(board, horizontal);
  });

  ctx.test("single_cell_dies_from_underpopulation", [&] {
    std::vector<std::vector<int>> board = {{1}};
    sol.gameOfLife(board);
    ctx.expect_eq(board, std::vector<std::vector<int>>{{0}});
  });

  ctx.test("empty_board_noop", [&] {
    std::vector<std::vector<int>> board;
    sol.gameOfLife(board);
    ctx.expect_true(board.empty());
  });
}

}  // namespace
