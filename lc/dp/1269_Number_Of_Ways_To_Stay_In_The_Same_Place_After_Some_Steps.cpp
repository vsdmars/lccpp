// LeetCode 1269: Number Of Ways To Stay In The Same Place After Some Steps
// https://leetcode.com/problems/number-of-ways-to-stay-in-the-same-place-after-some-steps/
//
// Problem:
//   A pointer starts at index 0 of an array of length `arrLen` (`1 <= arrLen <=
//   10^6`). In each of `steps` (`1 <= steps <= 500`) moves, the pointer may
//   move 1 step left, move 1 step right, or stay at its current index without
//   ever leaving `[0, arrLen - 1]`. Return the number of distinct move
//   sequences that leave the pointer at index 0 after exactly `steps` moves,
//   modulo `10^9 + 7`.
//
// Algorithm:
//   Bottom-up rolling-array dynamic programming over step count and reachable
//   positions. Notice two key pruning insights:
//     1. The pointer can never venture farther right than `steps / 2` and still
//        return to index 0 within `steps` moves. Thus we only need to track
//        positions `0 .. max_cols - 1`, where
//        `max_cols = min(steps / 2 + 1, arrLen)`.
//     2. Starting from step 0 with `dp[0] = 1` (and all other positions 0)
//        cleanly handles `arrLen == 1` and `steps == 1` without special cases.
//   At each step `s` from `1` to `steps`, for each position `pos`:
//     `next[pos] = (dp[pos] + dp[pos - 1] (if pos > 0)
//                           + dp[pos + 1] (if pos + 1 < max_cols)) % (10^9 + 7)`
//   Time Complexity:  O(steps * min(steps, arrLen)).
//   Space Complexity: O(min(steps, arrLen)) using two rolling 1D arrays.
//
// Data Structures:
//   - Two 1D DP vectors (`dp` and `next_dp` of `std::int64_t`, length
//     `min(steps / 2 + 1, arrLen)`) storing path counts at the previous and
//     current step counts.
//
// Real-World Applications:
//   - Discrete-time Markov chain return-probability analysis and bounded 1D
//     random walks (Motzkin path enumeration).
//   - Queueing theory state-occupancy modeling for bounded buffers and token
//     buckets.
//   - Quantitative finance trinomial lattice pricing for options with reflecting
//     or absorbing barriers.
//
// Related LeetCode Problems:
//   - #70 Climbing Stairs [done] (1D path-counting DP)
//   - #576 Out of Boundary Paths [new] (bounded-grid step-count DP modulo 10^9 + 7)
//   - #688 Knight Probability in Chessboard [done] (fixed-step transition DP on a bounded domain)
//   - #2400 Number of Ways to Reach a Position After Exactly k Steps [new] (unbounded 1D random walk counting)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the number of ways to start at index 0 and return to index 0 after
  // exactly `steps` moves inside `[0, arr_len - 1]`, modulo 10^9 + 7.
  // Time: O(steps * min(steps, arr_len)), Space: O(min(steps, arr_len)).
  int numWays(int steps, int arr_len) {
    static constexpr std::int64_t kMod = 1'000'000'007;

    // Any index strictly greater than `steps / 2` cannot return to 0 in time,
    // so clamping the state width prevents wasting time/memory when `arr_len`
    // is up to 10^6.
    const std::size_t max_cols =
        static_cast<std::size_t>(std::min((steps / 2) + 1, arr_len));

    // Base state at step 0: 1 way to be at index 0, 0 ways elsewhere.
    std::vector<std::int64_t> dp(max_cols, 0);
    std::vector<std::int64_t> next_dp(max_cols, 0);
    dp[0uz] = 1;

    for (int step = 0; step < steps; ++step) {
      for (std::size_t pos = 0uz; pos < max_cols; ++pos) {
        // Stay at `pos`.
        std::int64_t ways = dp[pos];
        // Arrived from `pos - 1` by moving right.
        if (pos > 0uz) {
          ways += dp[pos - 1uz];
        }
        // Arrived from `pos + 1` by moving left.
        if (pos + 1uz < max_cols) {
          ways += dp[pos + 1uz];
        }
        next_dp[pos] = ways % kMod;
      }
      std::swap(dp, next_dp);
    }

    return static_cast<int>(dp[0uz]);
  }
};

LC_PROBLEM(1269, "Number Of Ways To Stay In The Same Place After Some Steps",
           "dp", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    ctx.expect_eq(sol.numWays(3, 2), 4);
  });

  ctx.test("example_2", [&] {
    ctx.expect_eq(sol.numWays(2, 4), 2);
  });

  ctx.test("example_3_python_build", [&] {
    ctx.expect_eq(sol.numWays(4, 2), 8);
  });

  ctx.test("edge_cases_single_cell_and_single_step", [&] {
    // When arrLen == 1, only staying at index 0 is legal on every step.
    ctx.expect_eq(sol.numWays(1, 1), 1);
    ctx.expect_eq(sol.numWays(5, 1), 1);
    ctx.expect_eq(sol.numWays(1, 100), 1);
  });

  ctx.test("unconstrained_motzkin_and_modulo_boundary", [&] {
    // 4th Motzkin number M_4 = 9 when arrLen >= steps/2 + 1 = 3.
    ctx.expect_eq(sol.numWays(4, 3), 9);
    ctx.expect_eq(sol.numWays(4, 1'000'000), 9);
    ctx.expect_eq(sol.numWays(5, 3), 21);
    ctx.expect_eq(sol.numWays(6, 3), 50);
    // For arrLen == 2, ways(steps, 2) = 2^(steps - 1) mod (10^9 + 7):
    // 2^30 = 1,073,741,824 === 73,741,817 (mod 1,000,000,007).
    ctx.expect_eq(sol.numWays(31, 2), 73'741'817);
  });
}

}  // namespace
