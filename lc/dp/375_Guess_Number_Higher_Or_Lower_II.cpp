// LeetCode 375: Guess Number Higher Or Lower II
// https://leetcode.com/problems/guess-number-higher-or-lower-ii/
//
// Problem:
//   We play a guessing game on integers `1..n`. A secret number is chosen in
//   `[1, n]`. Whenever you guess a wrong number `x`, you pay `$x` and are told
//   whether the secret number is higher or lower. When you guess the secret
//   number, you pay `$0` and win. Given `n >= 1`, return the minimum amount of
//   money required to guarantee a win regardless of which number was picked.
//
// Algorithm:
//   Bottom-Up Minimax Interval Dynamic Programming. Define `dp[lo][hi]` as the
//   minimum worst-case cost to identify any secret number in the inclusive
//   range `[lo, hi]`.
//   - Base cases: `dp[lo][hi] = 0` for `lo >= hi` (a single remaining number
//     requires no risky guess; empty intervals have 0 cost).
//   - Recurrence for `lo < hi`: choosing first guess `x` in `[lo, hi - 1]`
//     incurs cost `x` in the worst case plus the harder of the two resulting
//     sub-intervals `[lo, x - 1]` and `[x + 1, hi]` (adversary picks the
//     branch that maximizes our cost, while we choose `x` to minimize it):
//       dp[lo][hi] = min_{lo <= x < hi} (x + max(dp[lo][x - 1], dp[x + 1][hi]))
//     Note that guessing `x = hi` is never strictly better than guessing
//     `x = hi - 1` because `hi + dp[lo][hi - 1] >= (hi - 1) + dp[lo][hi - 2]`
//     and `dp[hi][hi] = 0`.
//   Filling intervals in increasing length `len = 2..n` ensures all strictly
//   shorter sub-intervals are ready before they are queried.
//   Time Complexity:  O(n^3) — O(n^2) intervals, each testing O(n) pivots.
//   Space Complexity: O(n^2) for the 2D DP table.
//
// Data Structures:
//   - 2D `std::vector<std::vector<int>>` DP table of size `(n + 1) x (n + 1)`
//     indexed 1-based by inclusive range endpoints `[lo, hi]`, with row/column
//     0 naturally serving as empty-interval zero entries.
//
// Real-World Applications:
//   - Non-uniform cost decision-tree construction (e.g., medical diagnostics
//     or hardware fault isolation where each test has a different monetary or
//     latency cost and we want to minimize worst-case diagnostic cost).
//   - Minimax adversarial game-tree evaluation and optimal worst-case
//     probing strategies in network security / SLA budgeting.
//   - Cost-aware binary search over variable-latency storage tiers.
//
// Related LeetCode Problems:
//   - #374 Guess Number Higher or Lower [done] (uniform-cost binary search version)
//   - #887 Super Egg Drop [new] (minimax DP minimizing worst-case trials with limited eggs)
//   - #312 Burst Balloons [done] (interval DP over contiguous subranges)
//   - #486 Predict the Winner [done] (minimax interval DP for two-player zero-sum game)
//   - #1130 Minimum Cost Tree From Leaf Values [new] (interval DP choosing optimal split points)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the minimum money needed to guarantee winning the guessing game
  // on `[1, n]`.
  // Invariant: after outer iteration `len`, `dp[lo][hi]` holds the optimal
  // minimax cost for every inclusive interval `[lo, hi]` with `hi - lo + 1 <= len`.
  // Time: O(n^3), Space: O(n^2).
  int getMoneyAmount(int n) {
    if (n <= 1) {
      return 0;
    }

    const auto size = static_cast<std::size_t>(n);
    // Index 0 acts as a convenient 0-cost empty-range sentinel when `x == lo`.
    std::vector<std::vector<int>> dp(size + 1uz,
                                     std::vector<int>(size + 1uz, 0));

    // Solve intervals in increasing length from 2 up to `n`.
    for (std::size_t len = 2uz; len <= size; ++len) {
      for (std::size_t lo = 1uz; lo + len - 1uz <= size; ++lo) {
        const std::size_t hi = lo + len - 1uz;
        int best = std::numeric_limits<int>::max();

        // Try each candidate first guess `x` in `[lo, hi - 1]`. Guessing `hi`
        // is dominated by `hi - 1` because paying `hi` with empty right side
        // is always >= paying `hi - 1` with 0-cost right singleton `[hi, hi]`.
        for (std::size_t x = lo; x < hi; ++x) {
          const int worst_branch = std::max(dp[lo][x - 1uz], dp[x + 1uz][hi]);
          const int candidate_cost = static_cast<int>(x) + worst_branch;
          best = std::min(best, candidate_cost);
        }

        dp[lo][hi] = best;
      }
    }

    return dp[1uz][size];
  }
};

LC_PROBLEM(375, "Guess Number Higher Or Lower II", "dp", ctx) {
  ctx.test("base_cases", [&] {
    Solution sol;
    // 1 number: no wrong guess possible => $0.
    ctx.expect_eq(sol.getMoneyAmount(1), 0);
    // [1, 2]: guess 1 => worst case $1.
    ctx.expect_eq(sol.getMoneyAmount(2), 1);
    // [1, 2, 3]: guess middle 2 => worst case $2.
    ctx.expect_eq(sol.getMoneyAmount(3), 2);
  });

  ctx.test("medium_ranges", [&] {
    Solution sol;
    ctx.expect_eq(sol.getMoneyAmount(4), 4);
    ctx.expect_eq(sol.getMoneyAmount(5), 6);
    ctx.expect_eq(sol.getMoneyAmount(6), 8);
  });

  ctx.test("example_n_10", [&] {
    Solution sol;
    // Official LeetCode example: optimal first guess 7 gives 7 + max(8, 9) = 16.
    ctx.expect_eq(sol.getMoneyAmount(10), 16);
  });
}

}  // namespace
