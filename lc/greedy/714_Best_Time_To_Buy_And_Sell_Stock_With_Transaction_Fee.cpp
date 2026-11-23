// LeetCode 714: Best Time To Buy And Sell Stock With Transaction Fee
// https://leetcode.com/problems/best-time-to-buy-and-sell-stock-with-transaction-fee/
//
// Problem:
//   Given an integer array `prices` where `prices[i]` is the price of a stock
//   on day `i`, and a non-negative integer `fee` charged once per completed
//   buy-and-sell transaction, return the maximum total profit achievable. You
//   may complete as many transactions as you like, but you may hold at most one
//   share at a time (you must sell before buying again).
//
// Algorithm:
//   Use a two-state space-optimized dynamic programming / greedy state machine
//   tracking the best net profit at the end of each day `i`:
//     - `hold`: maximum profit while currently holding 1 share (initialized to
//       `-prices[0]`, representing buying on day 0).
//     - `cash`: maximum profit while holding 0 shares (initialized to `0`).
//   On each day `i = 1 .. n - 1`, compute simultaneous transitions (charging
//   `fee` on sell):
//     - `next_hold = max(hold, cash - prices[i])`   (keep holding, or buy
//       today)
//     - `next_cash = max(cash, (hold + prices[i]) - fee)` (stay in cash, or
//       sell today and pay the transaction fee)
//   At the end of the horizon, holding a share cannot beat being in cash
//   (prices are non-negative), so return `cash`.
//
//   Time Complexity:  O(n), single pass over `prices`.
//   Space Complexity: O(1), storing only the two scalar states.
//
// Data Structures:
//   - Two scalar state variables (`hold`, `cash`) rolled forward across days in
//     O(1) space.
//
// Real-World Applications:
//   - Quantitative trading backtesting with brokerage commissions or bid-ask
//     spread slippage.
//   - Energy grid battery arbitrage (buy/charge low, sell/discharge high)
//     accounting for round-trip conversion losses or wear cost per cycle.
//   - Cloud spot-instance migration decisions where switching workloads incurs
//     a fixed checkpoint/restart overhead.
//
// Related LeetCode Problems:
//   - #121 Best Time to Buy and Sell Stock [done] (single transaction, no fee)
//   - #122 Best Time to Buy and Sell Stock II [done] (unlimited transactions, zero fee)
//   - #309 Best Time to Buy and Sell Stock with Cooldown [done] (same state machine with 1-day cooldown)
//   - #123 Best Time to Buy and Sell Stock III [new] (at most 2 transactions)
//   - #188 Best Time to Buy and Sell Stock IV [done] (at most k transactions)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the maximum achievable profit with unlimited single-share
  // transactions when each completed sell incurs `fee`.
  // Time: O(prices.size()), Space: O(1).
  int maxProfit(const std::vector<int>& prices, int fee) {
    if (prices.empty()) {
      return 0;
    }

    // Day 0 states: either buy the share (net profit -prices[0]) or hold cash.
    int hold = -prices[0];
    int cash = 0;

    for (std::size_t i = 1uz; i < prices.size(); ++i) {
      const int price = prices[i];
      // Evaluate both transitions from day i - 1 before updating states:
      // 1. Hold through day i, or buy today using previous cash.
      const int next_hold = std::max(hold, cash - price);
      // 2. Stay in cash, or sell the held share today and pay the fee.
      const int next_cash = std::max(cash, (hold + price) - fee);

      hold = next_hold;
      cash = next_cash;
    }

    return cash;
  }
};

LC_PROBLEM(714, "Best Time To Buy And Sell Stock With Transaction Fee",
           "greedy", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    ctx.expect_eq(sol.maxProfit({1, 3, 2, 8, 4, 9}, 2), 8);
  });

  ctx.test("example_2_merge_beats_split", [&] {
    Solution sol;
    // Holding from 1 to 10 (profit 6) beats trading 1->7 and 5->10 (profit 5).
    ctx.expect_eq(sol.maxProfit({1, 3, 7, 5, 10, 3}, 3), 6);
  });

  ctx.test("python_build_input", [&] {
    Solution sol;
    // Buy 1, sell 7 (+4); buy 2, sell 9 (+5) -> total 9.
    ctx.expect_eq(sol.maxProfit({1, 3, 7, 2, 9}, 2), 9);
  });

  ctx.test("zero_fee_matches_stock_ii", [&] {
    Solution sol;
    ctx.expect_eq(sol.maxProfit({1, 3, 2, 8, 4, 9}, 0), 13);
  });

  ctx.test("decreasing_or_prohibitive_fee", [&] {
    Solution sol;
    ctx.expect_eq(sol.maxProfit({9, 7, 5, 3, 1}, 1), 0);
    ctx.expect_eq(sol.maxProfit({1, 3, 2, 4}, 5), 0);
    ctx.expect_eq(sol.maxProfit({5}, 2), 0);
  });
}

}  // namespace
