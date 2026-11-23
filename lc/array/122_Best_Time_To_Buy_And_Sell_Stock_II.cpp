// LeetCode 122: Best Time To Buy And Sell Stock II
// https://leetcode.com/problems/best-time-to-buy-and-sell-stock-ii/
//
// Problem:
//   Given an integer array `prices` where `prices[i]` is the price of a stock
//   on day `i`, find the maximum total profit achievable when you may buy and
//   sell as many times as you like (holding at most one share at any time,
//   with zero transaction fees).
//
// Examples:
//   Example 1:
//     Input:  prices = [7, 1, 5, 3, 6, 4]
//     Output: 7
//     Explanation:
//       - Initialize profit = 0 and iterate i from 1 to 5, adding
//         std::max(prices[i] - prices[i - 1], 0) at each step.
//       - At i = 1 (1 - 7 = -6, add 0), i = 2 (5 - 1 = 4, profit = 4), and i = 3 (3
//         - 5 = -2, add 0).
//       - At i = 4 (6 - 3 = 3, profit = 7) and i = 5 (4 - 6 = -2, add 0), giving a
//         final profit of 7.
//
//   Example 2:
//     Input:  prices = [1, 2, 3, 4, 5]
//     Output: 4
//     Explanation:
//       - Every adjacent step from i = 1 to 4 has a positive gain of +1 (2 - 1, 3 -
//         2, 4 - 3, 5 - 4).
//       - Accumulating these four daily increases telescopes to profit = 4,
//         matching buying at 1 and selling at 5.
//
//   Example 3:
//     Input:  prices = [7, 6, 4, 3, 1]
//     Output: 0
//     Explanation:
//       - Each day-over-day difference (-1, -2, -1, -2) is negative, so
//         std::max(diff, 0) contributes 0 on every iteration.
//       - The accumulator stays at its initial value and returns 0.
//
// Algorithm:
//   Greedy positive-slope accumulation. Because there are no transaction fees
//   and you may sell and immediately re-buy on the same day, any rising
//   segment `prices[a] <= prices[a+1] <= ... <= prices[b]` telescopes cleanly:
//     (prices[a+1] - prices[a]) + ... + (prices[b] - prices[b-1])
//       == prices[b] - prices[a].
//   Summing `max(prices[i] - prices[i - 1], 0)` over every adjacent day pair
//   therefore captures the exact sum of all valley-to-peak gains while
//   skipping all declining days.
//   Time complexity: O(n) — single linear pass.
//   Space complexity: O(1) — single running profit accumulator.
//
// Data Structures:
//   - Input array `prices` (`std::vector<int>`) scanned sequentially with a
//     single scalar integer accumulator.
//
// Real-World Applications:
//   - Quantitative trading backtesting: computing the frictionless theoretical
//     upper bound ("clairvoyant benchmark") for an algorithmic strategy on a
//     historical price series.
//   - Energy arbitrage / battery storage optimization: idealized profit bound
//     for buying electricity during low-price intervals and dispatching during
//     price increases.
//   - Total positive variation computation in time-series analysis and
//     telemetry signal processing.
//
// Related LeetCode Problems:
//   - #121 Best Time to Buy and Sell Stock [done] (at most 1 transaction)
//   - #123 Best Time to Buy and Sell Stock III [new] (at most 2 transactions via state-machine DP)
//   - #188 Best Time to Buy and Sell Stock IV [done] (at most k transactions via DP)
//   - #309 Best Time to Buy and Sell Stock with Cooldown [done] (adds 1-day cooldown state)
//   - #714 Best Time to Buy and Sell Stock with Transaction Fee [done] (greedy fails; requires 2-state DP)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the maximum achievable profit with unlimited transactions by
  // harvesting every positive day-over-day price increase.
  //
  // Time: O(n), Space: O(1).
  int maxProfit(const std::vector<int>& prices) {
    int profit = 0;

    // Starting at index 1 handles empty and 1-element inputs safely while
    // summing every positive upward step.
    for (std::size_t i = 1uz; i < prices.size(); ++i) {
      profit += std::max(prices[i] - prices[i - 1uz], 0);
    }

    return profit;
  }
};

LC_PROBLEM(122, "Best Time To Buy And Sell Stock II", "array", ctx) {
  Solution sol;

  ctx.test("example_1_multiple_trades", [&] {
    ctx.expect_eq(sol.maxProfit({7, 1, 5, 3, 6, 4}), 7);
  });

  ctx.test("example_2_strictly_increasing", [&] {
    ctx.expect_eq(sol.maxProfit({1, 2, 3, 4, 5}), 4);
  });

  ctx.test("example_3_strictly_decreasing", [&] {
    ctx.expect_eq(sol.maxProfit({7, 6, 4, 3, 1}), 0);
  });

  ctx.test("python_build_case", [&] {
    // (5 - 1) + (6 - 3) + (7 - 6) + (10 - 7) = 4 + 7 = 11.
    ctx.expect_eq(sol.maxProfit({1, 5, 3, 6, 7, 10}), 11);
  });

  ctx.test("edge_cases_empty_single_flat_zigzag", [&] {
    ctx.expect_eq(sol.maxProfit({}), 0);
    ctx.expect_eq(sol.maxProfit({5}), 0);
    ctx.expect_eq(sol.maxProfit({3, 3, 3, 3}), 0);
    ctx.expect_eq(sol.maxProfit({1, 5, 1, 5, 1, 5}), 12);
  });
}

}  // namespace
