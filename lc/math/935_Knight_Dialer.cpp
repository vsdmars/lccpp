// LeetCode 935: Knight Dialer
// https://leetcode.com/problems/knight-dialer/
//
// Problem:
//   Place a chess knight on any numbered key (0..9) of a standard phone keypad:
//     1 2 3
//     4 5 6
//     7 8 9
//     * 0 #
//   The knight makes `n - 1` valid L-shaped hops between numbered keys (never
//   landing on '*' or '#'), pressing `n` digits in total (1 <= n <= 5000).
//   Return the number of distinct `n`-digit numbers dialable, modulo 10^9 + 7.
//
// Examples:
//   Example 1:
//     Input:  n = 1
//     Output: 10
//     Explanation:
//       - Initialize dp[0..9] to 1, representing one valid 1-digit sequence ending
//         at each keypad key.
//       - Because n = 1, the hop loop (for step = 1; step < n; ++step) executes
//         zero times.
//       - Sum all 10 entries of dp to return 10.
//
//   Example 2:
//     Input:  n = 2
//     Output: 20
//     Explanation:
//       - Start with length-1 counts dp = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}.
//       - On hop step = 1, sum incoming knight moves per key: next[0] = next[1] =
//         next[2] = next[3] = next[7] = next[8] = next[9] = 2, next[4] = next[6] =
//         3, and next[5] = 0.
//       - Sum next[0..9] (2*7 + 3*2 + 0) to obtain 20.
//
//   Example 3:
//     Input:  n = 3
//     Output: 46
//     Explanation:
//       - After hop 1, dp = {2, 2, 2, 2, 3, 0, 3, 2, 2, 2}.
//       - On hop step = 2, compute next[0] = dp[4] + dp[6] = 6, next[1] = dp[6] +
//         dp[8] = 5, next[2] = dp[7] + dp[9] = 4, next[3] = dp[4] + dp[8] = 5, and
//         next[4] = dp[0] + dp[3] + dp[9] = 6.
//       - Compute the remaining keys: next[5] = 0, next[6] = dp[0] + dp[1] + dp[7]
//         = 6, next[7] = dp[2] + dp[6] = 5, next[8] = dp[1] + dp[3] = 4, and
//         next[9] = dp[2] + dp[4] = 5.
//       - Sum next[0..9] (6 + 5 + 4 + 5 + 6 + 0 + 6 + 5 + 4 + 5) to return 46.
//
// Algorithm:
//   1. Bottom-up rolling Dynamic Programming (`knightDialer`):
//      Let `dp[d]` be the number of valid sequences of the current length ending
//      at digit `d` (modulo 10^9 + 7). Initialize `dp[d] = 1` for all 10 digits
//      (length 1). On each of the `n - 1` hops, compute `next[d]` as the sum of
//      `dp[prev]` over all keys `prev` a knight's move away from `d` (key 5 has
//      no legal moves, so `next[5] = 0`).
//      Time: O(n), Space: O(1).
//   2. Symmetry-compressed Matrix Exponentiation (`knightDialerFast`):
//      For n >= 2, the 9 reachable keys collapse into 4 symmetry classes:
//        A = {1, 3, 7, 9}, B = {2, 8}, C = {4, 6}, D = {0}.
//      Exponentiating the 4x4 transition matrix via binary exponentiation yields
//      the answer in O(log n) time and O(1) space.
//
// Data Structures:
//   - `std::array<std::int64_t, 10>` for the rolling DP state vector.
//   - `std::array<std::array<std::int64_t, 4>, 4>` for the symmetry-compressed
//     transition matrix.
//
// Real-World Applications:
//   - Counting bounded-length walks on sparse state-transition graphs (Markov
//     chains, regular-language word counting, and finite-automaton path counts).
//   - Password/PIN policy entropy estimation under adjacency transition rules.
//
// Related LeetCode Problems:
//   - #688 Knight Probability in Chessboard [done] (finite-step knight walk DP on an N x N board)
//   - #1197 Minimum Knight Moves [done] (shortest-path knight reachability on a 2D grid)
//   - #70 Climbing Stairs [done] (linear recurrence solvable by rolling DP or matrix exponentiation)
//   - #509 Fibonacci Number [done] (constant-space recurrence and O(log n) matrix powering)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace {

class Solution {
 public:
  // Computes the number of distinct `n`-digit numbers dialable by a chess knight
  // on a phone keypad, modulo 10^9 + 7, using O(n) rolling DP.
  //
  // Invariant: After `step` iterations, `dp[d]` equals the count (mod kMod) of
  // valid knight sequences of length `step + 1` ending at digit `d`.
  //
  // Time: O(n), Space: O(1).
  int knightDialer(int n) {
    if (n <= 0) {
      return 0;
    }

    // Length 1: every digit 0..9 has exactly 1 valid sequence (just itself).
    std::array<std::int64_t, kNumKeys> dp{};
    dp.fill(1);

    // Perform `n - 1` hops, reducing modulo kMod at every step to prevent
    // 64-bit overflow for large `n` (up to 5000).
    for (int step = 1; step < n; ++step) {
      std::array<std::int64_t, kNumKeys> next{};
      next[0] = (dp[4] + dp[6]) % kMod;
      next[1] = (dp[6] + dp[8]) % kMod;
      next[2] = (dp[7] + dp[9]) % kMod;
      next[3] = (dp[4] + dp[8]) % kMod;
      next[4] = (dp[0] + dp[3] + dp[9]) % kMod;
      next[5] = 0;  // Key 5 has no valid knight moves.
      next[6] = (dp[0] + dp[1] + dp[7]) % kMod;
      next[7] = (dp[2] + dp[6]) % kMod;
      next[8] = (dp[1] + dp[3]) % kMod;
      next[9] = (dp[2] + dp[4]) % kMod;
      dp = next;
    }

    std::int64_t total = 0;
    for (const std::int64_t count : dp) {
      total = (total + count) % kMod;
    }
    return static_cast<int>(total);
  }

  // Secondary O(log n) solution using 4x4 matrix exponentiation over the four
  // keypad symmetry groups: A={1,3,7,9}, B={2,8}, C={4,6}, D={0}.
  //
  // Time: O(log n), Space: O(1).
  int knightDialerFast(int n) {
    if (n <= 0) {
      return 0;
    }
    if (n == 1) {
      return 10;
    }

    // Group totals at length 1 (excluding isolated key 5, which dies on hop 1):
    //   A={1,3,7,9} -> 4, B={2,8} -> 2, C={4,6} -> 2, D={0} -> 1.
    // Transition per hop:
    //   A' = 2*B + 2*C,  B' = A,  C' = A + 2*D,  D' = C.
    Matrix4x4 base = {
        Row4{0, 2, 2, 0},
        Row4{1, 0, 0, 0},
        Row4{1, 0, 0, 2},
        Row4{0, 0, 1, 0},
    };

    Matrix4x4 power = identity4x4();
    int exp = n - 1;
    while (exp > 0) {
      if ((exp & 1) != 0) {
        power = multiply4x4(power, base);
      }
      base = multiply4x4(base, base);
      exp >>= 1;
    }

    const Row4 initial = {4, 2, 2, 1};
    std::int64_t total = 0;
    for (std::size_t r = 0uz; r < kSymGroups; ++r) {
      std::int64_t group_sum = 0;
      for (std::size_t c = 0uz; c < kSymGroups; ++c) {
        group_sum = (group_sum + (power[r][c] * initial[c])) % kMod;
      }
      total = (total + group_sum) % kMod;
    }
    return static_cast<int>(total);
  }

 private:
  static constexpr std::int64_t kMod = 1'000'000'007;
  static constexpr std::size_t kNumKeys = 10uz;
  static constexpr std::size_t kSymGroups = 4uz;

  using Row4 = std::array<std::int64_t, kSymGroups>;
  using Matrix4x4 = std::array<Row4, kSymGroups>;

  // Returns the 4x4 multiplicative identity matrix.
  static Matrix4x4 identity4x4() {
    Matrix4x4 id{};
    for (std::size_t i = 0uz; i < kSymGroups; ++i) {
      id[i][i] = 1;
    }
    return id;
  }

  // Multiplies two 4x4 matrices modulo kMod.
  static Matrix4x4 multiply4x4(const Matrix4x4& lhs, const Matrix4x4& rhs) {
    Matrix4x4 out{};
    for (std::size_t i = 0uz; i < kSymGroups; ++i) {
      for (std::size_t k = 0uz; k < kSymGroups; ++k) {
        if (lhs[i][k] == 0) {
          continue;
        }
        for (std::size_t j = 0uz; j < kSymGroups; ++j) {
          out[i][j] = (out[i][j] + (lhs[i][k] * rhs[k][j])) % kMod;
        }
      }
    }
    return out;
  }
};

LC_PROBLEM(935, "Knight Dialer", "math", ctx) {
  Solution sol;

  ctx.test("example_1_n1", [&] {
    ctx.expect_eq(sol.knightDialer(1), 10);
    ctx.expect_eq(sol.knightDialerFast(1), 10);
  });

  ctx.test("example_2_n2", [&] {
    ctx.expect_eq(sol.knightDialer(2), 20);
    ctx.expect_eq(sol.knightDialerFast(2), 20);
  });

  ctx.test("example_3_n3", [&] {
    ctx.expect_eq(sol.knightDialer(3), 46);
    ctx.expect_eq(sol.knightDialerFast(3), 46);
  });

  ctx.test("small_verified_values", [&] {
    ctx.expect_eq(sol.knightDialer(4), 104);
    ctx.expect_eq(sol.knightDialerFast(4), 104);
    ctx.expect_eq(sol.knightDialer(5), 240);
    ctx.expect_eq(sol.knightDialerFast(5), 240);
    // Matches Python build() input N = 10.
    ctx.expect_eq(sol.knightDialer(10), 14'912);
    ctx.expect_eq(sol.knightDialerFast(10), 14'912);
  });

  ctx.test("large_n_dp_matches_matrix_exp", [&] {
    // Verify modulo arithmetic and exact agreement between O(n) DP and O(log n)
    // matrix exponentiation up to the constraint ceiling n = 5000.
    ctx.expect_eq(sol.knightDialer(3131), 136'006'598);
    ctx.expect_eq(sol.knightDialerFast(3131), 136'006'598);
    ctx.expect_eq(sol.knightDialer(5000), sol.knightDialerFast(5000));
  });
}

}  // namespace
