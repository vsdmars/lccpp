// LeetCode 486: Predict The Winner
// https://leetcode.com/problems/predict-the-winner/
//
// Problem:
//   Given an array `nums` of non-negative integer scores, two players alternate
//   taking one number from either end of the remaining array until no numbers
//   remain. Both players play optimally to maximize their own total score.
//   Return `true` if Player 1 can guarantee a score greater than or equal to
//   Player 2's score (ties go to Player 1), and `false` otherwise.
//   Constraints: 1 <= nums.size() <= 20, 0 <= nums[i] <= 10^7.
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 5, 2]
//     Output: false
//     Explanation:
//       - Build prefix = [0, 1, 6, 8] (total_sum = 8) and initialize base cases
//         dp[0][0] = 1, dp[1][1] = 5, dp[2][2] = 2.
//       - Compute length-2 intervals: dp[0][1] = 6 - min(5, 1) = 5 and dp[1][2] = 7
//         - min(2, 5) = 5.
//       - Compute length-3 interval: dp[0][2] = 8 - min(dp[1][2], dp[0][1]) = 8 -
//         min(5, 5) = 3.
//       - Since 2 * dp[0][2] = 6 < total_sum = 8, Player 1 cannot force a tie or
//         win, so return false.
//
//   Example 2:
//     Input:  nums = [1, 5, 233, 7]
//     Output: true
//     Explanation:
//       - Build prefix = [0, 1, 6, 239, 246] (total_sum = 246) and set diagonal
//         base cases dp[i][i] = nums[i].
//       - Compute length-2 intervals: dp[0][1] = 5, dp[1][2] = 233, and dp[2][3] =
//         233.
//       - Compute length-3 intervals: dp[0][2] = 239 - min(233, 5) = 234 and
//         dp[1][3] = 245 - min(233, 233) = 12.
//       - Compute dp[0][3] = 246 - min(12, 234) = 234; since 2 * 234 = 468 >= 246,
//         return true.
//
//   Example 3:
//     Input:  nums = [5, 5]
//     Output: true
//     Explanation:
//       - Check n = nums.size() = 2 against the early-return guard n <= 2uz.
//       - Player 1 takes 5 on the first move, securing at least half of the total
//         score 10, so return true immediately.
//
// Algorithm:
//   Use bottom-up interval dynamic programming paired with a prefix-sum array
//   for O(1) subarray sum queries.
//   Let `dp[l][r]` be the maximum total score the player whose turn it is can
//   obtain from subarray `nums[l..r]`.
//     - Base case: `dp[l][l] = nums[l]`.
//     - Transition (`l < r`): if the current player picks `nums[l]`, the
//       opponent optimally takes `dp[l + 1][r]` from `nums[l + 1..r]`, leaving
//       the current player with `sum(l, r) - dp[l + 1][r]`. Symmetrically,
//       picking `nums[r]` yields `sum(l, r) - dp[l][r - 1]`. Hence:
//         dp[l][r] = sum(l, r) - min(dp[l + 1][r], dp[l][r - 1])
//   Fill the table in increasing order of interval length from 1 to n. Player 1
//   wins (or ties) iff `2 * dp[0][n - 1] >= sum(0, n - 1)`.
//   Time Complexity:  O(n^2) to populate all `(l, r)` intervals.
//   Space Complexity: O(n^2) for the interval DP table and O(n) for prefix sums.
//
// Data Structures:
//   - Prefix-sum array (`std::vector<std::int64_t>` of size `n + 1`): answers
//     arbitrary contiguous subarray sums in O(1) without integer overflow.
//   - 2D interval DP table (`std::vector<std::vector<std::int64_t>>`): stores
//     optimal first-mover scores for every subgame `[l, r]`.
//
// Real-World Applications:
//   - Game-tree minimax evaluation and endgame tablebase construction for
//     two-player zero-sum deterministic games.
//   - Competitive resource-claiming and alternating-draft strategy analysis
//     (e.g., auction/draft turn ordering in sports or bandwidth allocation).
//   - Optimal adversarial scheduling when two competing agents pick jobs from
//     a double-ended queue.
//
// Related LeetCode Problems:
//   - #877 Stone Game [new] (even-length specialization of this exact two-ended game)
//   - #1140 Stone Game II [new] (interval game DP with variable pick count and suffix sums)
//   - #1406 Stone Game III [new] (zero-sum minimax DP on linear array)
//   - #312 Burst Balloons [done] (classic bottom-up interval DP by subarray length)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Determines whether Player 1 can achieve at least half of the total score
  // under optimal play using bottom-up interval DP with prefix sums.
  // Time: O(n^2), Space: O(n^2).
  bool predictTheWinner(const std::vector<int>& nums) {
    const std::size_t n = nums.size();
    if (n <= 2uz) {
      // With 1 or 2 elements, Player 1 immediately picks the larger element.
      return true;
    }

    // `prefix[k]` stores the sum of `nums[0..k-1]` in 64-bit arithmetic so
    // sums up to 20 * 10^7 never risk signed overflow.
    std::vector<std::int64_t> prefix(n + 1uz, 0);
    for (std::size_t i = 0uz; i < n; ++i) {
      prefix[i + 1uz] = prefix[i] + static_cast<std::int64_t>(nums[i]);
    }

    // `dp[l][r]` is the maximum score the active player can secure from the
    // subgame `nums[l..r]`.
    std::vector<std::vector<std::int64_t>> dp(
        n, std::vector<std::int64_t>(n, 0));

    // Base cases: single-element subgames grant that sole element.
    for (std::size_t i = 0uz; i < n; ++i) {
      dp[i][i] = static_cast<std::int64_t>(nums[i]);
    }

    // Expand bottom-up by interval length so `dp[l + 1][r]` and `dp[l][r - 1]`
    // are always computed before `dp[l][r]`.
    for (std::size_t len = 2uz; len <= n; ++len) {
      for (std::size_t lhs = 0uz; (lhs + len) <= n; ++lhs) {
        const std::size_t rhs = (lhs + len) - 1uz;
        const std::int64_t range_sum = prefix[rhs + 1uz] - prefix[lhs];

        // Choosing `nums[lhs]` leaves the opponent with `dp[lhs + 1][rhs]`;
        // choosing `nums[rhs]` leaves the opponent with `dp[lhs][rhs - 1]`.
        // Maximizing our score is equivalent to minimizing the opponent's.
        const std::int64_t best_opponent =
            std::min(dp[lhs + 1uz][rhs], dp[lhs][rhs - 1uz]);
        dp[lhs][rhs] = range_sum - best_opponent;
      }
    }

    const std::int64_t total_sum = prefix[n];
    // Tie counts as a win for Player 1, so check 2 * player1_score >= total.
    return (dp[0uz][n - 1uz] * 2) >= total_sum;
  }
};

LC_PROBLEM(486, "Predict The Winner", "dp", ctx) {
  ctx.test("example_1_player1_loses", [&] {
    Solution sol;
    ctx.expect_false(sol.predictTheWinner({1, 5, 2}));
  });

  ctx.test("example_2_player1_wins", [&] {
    Solution sol;
    ctx.expect_true(sol.predictTheWinner({1, 5, 233, 7}));
  });

  ctx.test("source_build_even_length", [&] {
    Solution sol;
    ctx.expect_true(sol.predictTheWinner({1, 567, 1, 1, 99, 100}));
  });

  ctx.test("all_equal_odd_length", [&] {
    Solution sol;
    ctx.expect_true(sol.predictTheWinner({1, 1, 1}));
  });

  ctx.test("single_element_and_tie", [&] {
    Solution sol;
    ctx.expect_true(sol.predictTheWinner({0}));
    ctx.expect_true(sol.predictTheWinner({5, 5}));
  });

  ctx.test("dominant_middle_odd_length", [&] {
    Solution sol;
    ctx.expect_false(sol.predictTheWinner({1, 100, 1}));
  });
}

}  // namespace
