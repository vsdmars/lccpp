// LeetCode 688: Knight Probability In Chessboard
// https://leetcode.com/problems/knight-probability-in-chessboard/
//
// Problem:
//   On an n x n chessboard (0-indexed, 1 <= n <= 25), a chess knight begins at
//   square `(row, column)` and attempts to make `k` moves (0 <= k <= 100). On
//   each move, the knight selects one of its 8 L-shaped offsets uniformly at
//   random (probability 1/8 each), even if the destination lies off the board.
//   Once the knight leaves the board it stops moving. Return the probability
//   that the knight remains on the board after all `k` moves.
//
// Examples:
//   Example 1:
//     Input:  n = 3, k = 2, row = 0, column = 0
//     Output: 0.0625
//     Explanation:
//       - Initialize a 3x3 dp grid with dp[0][0] = 1.0 and all other cells 0.0.
//       - On step 1, advanceOneStep distributes 1.0 * 0.125 = 0.125 from (0, 0) to
//         the two in-bounds targets (1, 2) and (2, 1), discarding the six off-board
//         jumps.
//       - On step 2, (1, 2) adds 0.125 * 0.125 = 0.015625 to (0, 0) and (2, 0),
//         while (2, 1) adds 0.015625 to (0, 0) and (0, 2).
//       - Sum the remaining non-zero probabilities dp[0][0] = 0.03125, dp[0][2] =
//         0.015625, and dp[2][0] = 0.015625 to return 0.0625.
//
//   Example 2:
//     Input:  n = 1, k = 0, row = 0, column = 0
//     Output: 1.0
//     Explanation:
//       - Verify that starting square (0, 0) lies inside the 1x1 board.
//       - Trigger the k == 0 check and return 1.0 immediately because no moves are
//         attempted.
//
//   Example 3:
//     Input:  n = 3, k = 1, row = 1, column = 1
//     Output: 0.0
//     Explanation:
//       - Initialize dp[1][1] = 1.0 at the center of the 3x3 board.
//       - In the single step, every offset in kKnightMoves shifts either row or
//         column by +-2 from 1 to -1 or 3, which is outside [0, 3).
//       - All cells in next_dp stay 0.0, so summing the grid returns 0.0.
//
// Algorithm:
//   Bottom-up Markov-chain dynamic programming with two rolling n x n grids.
//   Let `dp[r][c]` be the probability that the knight is still on the board
//   and occupies square `(r, c)` after the current step.
//   - Initialization (step 0): `dp[row][column] = 1.0`, all other squares 0.0.
//   - Transition (repeated `k` times): clear `next_dp` to 0.0. For every cell
//     `(r, c)` with non-zero probability `p`, distribute `p / 8.0` to each of
//     its 8 knight neighbors `(nr, nc)` that lie inside `[0, n) x [0, n)`.
//     (Probability mass landing outside the board is implicitly absorbed as
//     off-board loss.) Swap `dp` and `next_dp`.
//   - Result: sum of `dp[r][c]` over all cells on the board.
//   Time Complexity: O(k * n^2) (8 transitions per cell per step).
//   Space Complexity: O(n^2) for the two rolling probability boards.
//
// Data Structures:
//   - Two 2D rolling probability grids (`std::vector<std::vector<double>>`):
//     store the Markov state distribution before and after each move without
//     reallocating memory per step.
//   - Fixed `std::array<std::pair<int, int>, 8>`: holds the 8 L-shaped knight
//     displacement vectors.
//
// Real-World Applications:
//   - Finite-horizon absorbing Markov chain survival analysis in reliability
//     engineering, network packet TTL survival modeling, and credit-risk
//     transition matrices.
//   - Stochastic grid-based robotics localization and bounded-domain random
//     walk simulations in physics and computational finance.
//
// Related LeetCode Problems:
//   - #576 Out of Boundary Paths [new] (counts off-board paths on a grid via the same layered DP)
//   - #935 Knight Dialer [done] (counts k-step knight walks on a keypad graph)
//   - #837 New 21 Game [new] (1D probability DP over fixed-distribution transitions)
//   - #808 Soup Servings [new] (2D absorbing-state probability DP)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

constexpr double kEpsilon = 1e-5;
constexpr double kMoveProb = 0.125;  // Exact binary representation of 1 / 8.

constexpr std::array<std::pair<int, int>, 8> kKnightMoves = {{
    {-2, -1},
    {-2, 1},
    {-1, -2},
    {-1, 2},
    {1, -2},
    {1, 2},
    {2, -1},
    {2, 1},
}};

class Solution {
 public:
  // Computes the probability that a knight starting at `(row, column)` on an
  // `n x n` board remains on the board after `k` uniformly random moves.
  // Time: O(k * n^2), Space: O(n^2).
  double knightProbability(int n, int k, int row, int column) {
    if (n <= 0 || row < 0 || row >= n || column < 0 || column >= n) {
      return 0.0;
    }
    if (k == 0) {
      return 1.0;
    }

    const auto size = static_cast<std::size_t>(n);
    std::vector<std::vector<double>> dp(size, std::vector<double>(size, 0.0));
    std::vector<std::vector<double>> next_dp(size,
                                             std::vector<double>(size, 0.0));

    // At step 0, all probability mass sits at the knight's starting square.
    dp[static_cast<std::size_t>(row)][static_cast<std::size_t>(column)] = 1.0;

    for (int step = 0; step < k; ++step) {
      advanceOneStep(n, dp, next_dp);
      std::swap(dp, next_dp);
    }

    // Total survival probability is the sum of probabilities over all on-board
    // squares after `k` steps.
    double remaining_prob = 0.0;
    for (const auto& grid_row : dp) {
      for (const double cell_prob : grid_row) {
        remaining_prob += cell_prob;
      }
    }
    return remaining_prob;
  }

 private:
  // Propagates one Markov transition step from `current` into `next`,
  // discarding probability mass that jumps off the `n x n` board.
  static void advanceOneStep(int n,
                             const std::vector<std::vector<double>>& current,
                             std::vector<std::vector<double>>& next) {
    for (auto& grid_row : next) {
      std::ranges::fill(grid_row, 0.0);
    }

    const auto size = static_cast<std::size_t>(n);
    for (std::size_t r = 0uz; r < size; ++r) {
      for (std::size_t c = 0uz; c < size; ++c) {
        const double prob = current[r][c];
        if (prob == 0.0) {
          continue;
        }
        distributeFromCell(n, static_cast<int>(r), static_cast<int>(c),
                           prob * kMoveProb, next);
      }
    }
  }

  // Adds `branch_prob` to each valid in-bounds knight destination from `(r, c)`.
  static void distributeFromCell(int n, int r, int c, double branch_prob,
                                 std::vector<std::vector<double>>& next) {
    for (const auto& [dr, dc] : kKnightMoves) {
      const int nr = r + dr;
      const int nc = c + dc;
      if (nr >= 0 && nr < n && nc >= 0 && nc < n) {
        next[static_cast<std::size_t>(nr)][static_cast<std::size_t>(nc)] +=
            branch_prob;
      }
    }
  }
};

// Floating-point comparison helper within standard LeetCode tolerance.
bool approxEqual(double actual, double expected) {
  return std::fabs(actual - expected) <= kEpsilon;
}

LC_PROBLEM(688, "Knight Probability In Chessboard", "dp", ctx) {
  ctx.test("example_1_3x3_two_moves", [&] {
    Solution sol;
    ctx.expect_true(approxEqual(sol.knightProbability(3, 2, 0, 0), 0.0625));
  });

  ctx.test("example_2_zero_moves", [&] {
    Solution sol;
    ctx.expect_true(approxEqual(sol.knightProbability(1, 0, 0, 0), 1.0));
  });

  ctx.test("guaranteed_off_board", [&] {
    Solution sol;
    // On a 1x1 board or from the center of a 3x3 board, every knight move
    // lands outside the board.
    ctx.expect_true(approxEqual(sol.knightProbability(1, 1, 0, 0), 0.0));
    ctx.expect_true(approxEqual(sol.knightProbability(3, 1, 1, 1), 0.0));
  });

  ctx.test("single_move_corner_and_center", [&] {
    Solution sol;
    // Corner of 3x3 or 8x8 has exactly 2 legal moves -> 2 / 8 = 0.25.
    ctx.expect_true(approxEqual(sol.knightProbability(3, 1, 0, 0), 0.25));
    ctx.expect_true(approxEqual(sol.knightProbability(8, 1, 0, 0), 0.25));
    // Center of 8x8 has all 8 moves in bounds -> 1.0.
    ctx.expect_true(approxEqual(sol.knightProbability(8, 1, 3, 3), 1.0));
  });

  ctx.test("larger_horizon_valid_probability_range", [&] {
    Solution sol;
    const double p = sol.knightProbability(8, 30, 6, 4);
    ctx.expect_true(p > 0.0 && p < 1.0);
  });
}

}  // namespace
