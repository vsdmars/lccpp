// LeetCode 518: Coin Change 2
// https://leetcode.com/problems/coin-change-ii/
//
// Problem:
//   Given an integer array `coins` of distinct positive coin denominations
//   (with an unlimited supply of each denomination) and an integer `amount`,
//   return the number of distinct combinations of coins that sum to `amount`.
//   Order does not matter (e.g., `1 + 2` and `2 + 1` count as one combination).
//   Constraints: 1 <= coins.size() <= 300, 1 <= coins[i] <= 5000,
//   0 <= amount <= 5000, answer fits in a signed 32-bit integer.
//
// Examples:
//   Example 1:
//     Input:  amount = 5, coins = [1, 2, 5]
//     Output: 4
//     Explanation:
//       - Allocate dp of size 6 initialized to [1, 0, 0, 0, 0, 0] with base case
//         dp[0] = 1.
//       - Process coin = 1 for a = 1..5 via dp[a] += dp[a - 1], updating dp to [1,
//         1, 1, 1, 1, 1].
//       - Process coin = 2 for a = 2..5: dp[2] = 2, dp[3] = 2, dp[4] = 3, dp[5] =
//         3, giving dp = [1, 1, 2, 2, 3, 3].
//       - Process coin = 5 at a = 5: dp[5] += dp[0] = 3 + 1 = 4, and return dp[5] =
//         4.
//
//   Example 2:
//     Input:  amount = 3, coins = [2]
//     Output: 0
//     Explanation:
//       - Initialize dp = [1, 0, 0, 0] for target amount 3.
//       - Process coin = 2 for a = 2..3: dp[2] += dp[0] sets dp[2] = 1, and dp[3]
//         += dp[1] adds 0 so dp[3] remains 0.
//       - Return dp[3] = 0 because 3 cannot be formed from denomination 2 alone.
//
//   Example 3:
//     Input:  amount = 0, coins = [1, 2, 5]
//     Output: 1
//     Explanation:
//       - Initialize dp of size 1 to [1] for target = 0.
//       - For each coin (1, 2, 5), the inner loop condition a <= 0 fails
//         immediately because every denomination exceeds target = 0.
//       - Return dp[0] = 1 representing the single empty combination.
//
// Algorithm:
//   Use bottom-up 1D unbounded-knapsack dynamic programming where `dp[a]`
//   stores the number of combinations summing to `a`, initialized with
//   `dp[0] = 1` (one empty combination for amount 0) and `dp[1..amount] = 0`.
//   Crucially, iterate `coins` in the OUTER loop and `a` from `coin` up to
//   `amount` in the INNER loop:
//     dp[a] += dp[a - coin]
//   Fixing coin order in the outer loop enforces canonical non-decreasing
//   denomination sequences, counting unordered combinations rather than
//   permutations (whereas putting `amount` outside and `coins` inside would
//   erroneously count permutations as in LeetCode #377). Sweeping `a` forwards
//   allows each coin denomination to be reused arbitrarily many times.
//   Time Complexity:  O(C * amount), where C = coins.size().
//   Space Complexity: O(amount) for the 1D DP table.
//
// Data Structures:
//   - 1D DP table (`std::vector<std::uint64_t>` of size `amount + 1`):
//     accumulates combination counts for every intermediate sum while avoiding
//     signed-overflow undefined behavior on unreachable high-count states.
//
// Real-World Applications:
//   - Payment/ATM dispensing analysis and currency denomination design
//     evaluating ways to compose transaction values.
//   - Combinatorial integer-partition counting in resource-packaging systems
//     (e.g., counting ways to assemble fixed-size storage/network bundles from
//     standardized block sizes).
//   - Generating-function coefficient computation in actuarial and pricing
//     models over discrete unit increments.
//
// Related LeetCode Problems:
//   - #322 Coin Change [done] (unbounded knapsack minimizing number of coins)
//   - #377 Combination Sum IV [done] (swapped loop order counting ordered permutations)
//   - #494 Target Sum [done] (0/1 knapsack counting ways with backward inner loop)
//   - #39 Combination Sum [done] (enumerating all unbounded combinations via backtracking)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Counts unordered combinations of `coins` summing to `amount` via 1D
  // unbounded-knapsack DP.
  // Time: O(C * amount), Space: O(amount).
  int change(int amount, const std::vector<int>& coins) {
    if (amount < 0) {
      return 0;
    }

    const auto target = static_cast<std::size_t>(amount);

    // Use unsigned 64-bit accumulators to avoid signed overflow UB on
    // intermediate subproblems whose counts exceed 32-bit int even when the
    // final answer fits in `int`.
    std::vector<std::uint64_t> dp(target + 1uz, 0);
    // Base case: exactly one combination (choosing no coins) sums to 0.
    dp[0uz] = 1;

    // Outer loop over coins guarantees each combination is counted once in
    // canonical coin order (avoiding duplicate permutations).
    for (const int coin : coins) {
      if (coin <= 0) {
        continue;
      }
      const auto denom = static_cast<std::size_t>(coin);
      // Forward sweep lets the current denomination be chosen multiple times.
      for (std::size_t a = denom; a <= target; ++a) {
        dp[a] += dp[a - denom];
      }
    }

    return static_cast<int>(dp[target]);
  }
};

LC_PROBLEM(518, "Coin Change 2", "dp", ctx) {
  ctx.test("example_1_amount_5", [&] {
    Solution sol;
    ctx.expect_eq(sol.change(5, {1, 2, 5}), 4);
  });

  ctx.test("example_2_impossible", [&] {
    Solution sol;
    ctx.expect_eq(sol.change(3, {2}), 0);
  });

  ctx.test("example_3_exact_single_coin", [&] {
    Solution sol;
    ctx.expect_eq(sol.change(10, {10}), 1);
  });

  ctx.test("zero_amount", [&] {
    Solution sol;
    ctx.expect_eq(sol.change(0, {1, 2, 5}), 1);
    ctx.expect_eq(sol.change(0, {}), 1);
  });

  ctx.test("combinations_not_permutations", [&] {
    Solution sol;
    // Ways to form 3 from {1, 2}: (1+1+1) and (1+2) => 2 (not 3).
    ctx.expect_eq(sol.change(3, {1, 2}), 2);
  });

  ctx.test("coins_larger_than_amount", [&] {
    Solution sol;
    ctx.expect_eq(sol.change(4, {5, 10}), 0);
  });
}

}  // namespace
