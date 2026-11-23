// LeetCode 322: Coin Change
// https://leetcode.com/problems/coin-change/
//
// Problem:
//   Given an integer array `coins` representing distinct positive coin
//   denominations (each available in infinite supply) and a non-negative
//   integer `amount`, return the fewest number of coins required to make up
//   `amount`. If `amount` cannot be formed by any combination of the coins,
//   return -1.
//
// Examples:
//   Example 1:
//     Input:  coins = [1, 2, 5], amount = 11
//     Output: 3
//     Explanation:
//       - Allocate dp of size 12 initialized to kUnreachable = 12 with dp[0] = 0.
//       - Fill cur = 1..5 to obtain dp[1] = 1, dp[2] = 1, dp[3] = 2, dp[4] = 2, and
//         dp[5] = dp[0] + 1 = 1.
//       - Fill cur = 6..10 to obtain dp[6] = 2, dp[7] = 2, dp[8] = 3, dp[9] = 3,
//         and dp[10] = dp[5] + 1 = 2.
//       - At cur = 11, relaxing across coins 1, 2, and 5 takes min(dp[10] + 1,
//         dp[9] + 1, dp[6] + 1) = min(3, 4, 3) = 3, which is <= 11 and returned.
//
//   Example 2:
//     Input:  coins = [3, 4], amount = 6
//     Output: 2
//     Explanation:
//       - Allocate dp of size 7 initialized to kUnreachable = 7 with dp[0] = 0.
//       - Values cur = 1, 2, and 5 remain unreachable at 7, while cur = 3 and cur =
//         4 relax to dp[3] = 1 and dp[4] = 1.
//       - At cur = 6, coin 3 yields dp[3] + 1 = 2 whereas coin 4 yields dp[2] + 1 =
//         8, so dp[6] = 2 is returned rather than failing greedily.
//
//   Example 3:
//     Input:  coins = [2], amount = 3
//     Output: -1
//     Explanation:
//       - Initialize dp of size 4 with kUnreachable = 4 and dp[0] = 0.
//       - At cur = 1, coin 2 exceeds cur so dp[1] stays 4; at cur = 2, coin 2
//         updates dp[2] = dp[0] + 1 = 1.
//       - At cur = 3, coin 2 tests dp[1] + 1 = 5 so dp[3] stays 4, and because
//         dp[3] > 3 the function returns -1.
//
// Algorithm:
//   Bottom-Up Unbounded Knapsack Dynamic Programming. Define `dp[a]` as the
//   minimum number of coins needed to sum exactly to `a`. Initialize `dp[0] = 0`
//   and all other entries to `amount + 1` (a safe upper-bound sentinel since
//   the smallest positive coin is 1, so any valid solution uses at most `amount`
//   coins, avoiding integer overflow when adding 1). For each target value
//   `a` from 1 to `amount`, relax `dp[a]` across every denomination `c <= a`:
//     dp[a] = min(dp[a], dp[a - c] + 1)
//   After filling the table, `dp[amount]` holds the answer if it is <= `amount`,
//   otherwise -1.
//   Time Complexity:  O(amount * |coins|).
//   Space Complexity: O(amount) for the 1D DP table.
//
// Data Structures:
//   - 1D `std::vector<int>` DP table of length `amount + 1` storing optimal
//     coin counts for every sub-amount `0..amount`.
//
// Real-World Applications:
//   - Vending machine, ATM, and point-of-sale currency dispensing under
//     non-canonical denomination systems where greedy selection fails.
//   - Cloud resource bin-packing and container sizing (selecting minimal
//     fixed-size instance blocks to satisfy exact capacity demand).
//   - Packet framing and memory slab allocation minimizing block count for a
//     requested byte payload.
//
// Related LeetCode Problems:
//   - #518 Coin Change II [done] (counts total unordered combinations instead of minimum coins)
//   - #377 Combination Sum IV [done] (counts ordered sequences summing to target)
//   - #279 Perfect Squares [done] (coin change where denominations are square numbers)
//   - #983 Minimum Cost For Tickets [done] (1D DP minimizing cost over fixed pass durations)
//   - #416 Partition Equal Subset Sum [done] (0/1 subset-sum variant where each item is used once)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the fewest coins needed to make `amount`, or -1 if impossible.
  // Invariant: after outer iteration `cur`, `dp[cur]` equals the minimum coin
  // count for exact sum `cur` (or `kUnreachable` if unreachable).
  // Time: O(amount * coins.size()), Space: O(amount).
  int coinChange(const std::vector<int>& coins, int amount) {
    if (amount < 0) {
      return -1;
    }
    if (amount == 0) {
      return 0;
    }

    const auto target = static_cast<std::size_t>(amount);
    // Sentinel strictly larger than any valid coin count (at most `amount`),
    // while small enough that `kUnreachable + 1` never overflows `int`.
    const int kUnreachable = amount + 1;
    std::vector<int> dp(target + 1uz, kUnreachable);
    dp[0uz] = 0;

    // Build optimal coin counts for every sub-amount from 1 up to `amount`.
    for (std::size_t cur = 1uz; cur <= target; ++cur) {
      for (const int coin : coins) {
        if (coin <= 0) {
          continue;
        }
        const auto u_coin = static_cast<std::size_t>(coin);
        if (u_coin <= cur) {
          dp[cur] = std::min(dp[cur], dp[cur - u_coin] + 1);
        }
      }
    }

    return dp[target] > amount ? -1 : dp[target];
  }
};

LC_PROBLEM(322, "Coin Change", "dp", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    ctx.expect_eq(sol.coinChange({1, 2, 5}, 11), 3);
  });

  ctx.test("example_2_impossible", [&] {
    Solution sol;
    ctx.expect_eq(sol.coinChange({2}, 3), -1);
  });

  ctx.test("zero_amount", [&] {
    Solution sol;
    ctx.expect_eq(sol.coinChange({1, 2, 5}, 0), 0);
    ctx.expect_eq(sol.coinChange({}, 0), 0);
  });

  ctx.test("non_canonical_greedy_fails", [&] {
    Solution sol;
    // Greedy picks 4 -> remainder 2 -> impossible; optimal is 3 + 3 = 6.
    ctx.expect_eq(sol.coinChange({3, 4}, 6), 2);
    // Classic LeetCode stress case where greedy fails dramatically.
    ctx.expect_eq(sol.coinChange({186, 419, 83, 408}, 6249), 20);
  });

  ctx.test("empty_coins_or_all_too_large", [&] {
    Solution sol;
    ctx.expect_eq(sol.coinChange({}, 5), -1);
    ctx.expect_eq(sol.coinChange({5, 10}, 3), -1);
  });
}

}  // namespace
