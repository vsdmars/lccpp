// LeetCode 121: Best Time To Buy And Sell Stock
// https://leetcode.com/problems/best-time-to-buy-and-sell-stock/
//
// Problem:
//   Given an array `prices` where `prices[i]` is the price of a stock on day i,
//   maximize profit by choosing a single day to buy one share and a strictly
//   later day to sell that share. If no profitable transaction is possible,
//   return 0.
//   Constraints: 1 <= prices.length <= 10^5; 0 <= prices[i] <= 10^4.
//
// Examples:
//   Example 1:
//     Input:  prices = [7, 1, 5, 3, 6, 4]
//     Output: 5
//     Explanation:
//       - Initialize min_price = 7 (day 0) and max_profit = 0.
//       - At i = 1 (price 1), max_profit stays 0 and min_price updates to 1.
//       - At i = 2 (price 5) and i = 3 (price 3), max_profit updates to 4 and
//         min_price remains 1.
//       - At i = 4 (price 6), max_profit updates to max(4, 6 - 1) = 5.
//       - At i = 5 (price 4), 4 - 1 = 3 does not beat 5, so maxProfit returns 5.
//
//   Example 2:
//     Input:  prices = [7, 6, 4, 3, 1]
//     Output: 0
//     Explanation:
//       - Initialize min_price = 7 and max_profit = 0.
//       - Across i = 1..4 (prices 6, 4, 3, 1), each prices[i] - min_price is
//         negative so max_profit remains 0.
//       - Meanwhile min_price decreases at each step from 7 down to 6, 4, 3, and 1,
//         returning 0.
//
//   Example 3:
//     Input:  prices = [2, 4, 1]
//     Output: 2
//     Explanation:
//       - Initialize min_price = 2 and max_profit = 0.
//       - At i = 1 (price 4), max_profit updates to 4 - 2 = 2 while min_price stays
//         2.
//       - At i = 2 (price 1), min_price updates to 1 without overwriting the
//         previously recorded max_profit of 2, so the function returns 2.
//
// Algorithm:
//   Two equivalent O(n)-time, O(1)-space single-pass DP approaches are provided:
//   1. Prefix Minimum Tracking (`maxProfit`):
//      - Maintain `min_price` = lowest price seen strictly before day i.
//      - At day i, the best profit from selling today is `prices[i] - min_price`.
//      - Update `max_profit` and `min_price` in one forward scan.
//   2. Kadane's Algorithm on Daily Differences (`maxProfitKadane`):
//      - Because telescoping daily price differences sum to total profit
//        (prices[j] - prices[i] = sum_{t=i+1..j} (prices[t] - prices[t-1])),
//        finding the best single buy-sell pair reduces to Maximum Subarray Sum
//        on the adjacent-difference array.
//      - Maintain `running_profit` ending at day i; reset to 0 whenever it
//        drops below 0 (equivalent to resetting the buy day to day i).
//   - Time Complexity: O(n) for both approaches.
//   - Space Complexity: O(1) for both approaches.
//
// Data Structures:
//   - Two scalar DP state variables (`min_price`/`max_profit` or
//     `running_profit`/`max_profit`), requiring constant auxiliary memory.
//
// Real-World Applications:
//   - Computing maximum drawdown / maximum run-up metrics in financial time
//     series and portfolio backtesting engines.
//   - Telemetry anomaly detection (measuring largest upward drift or latency
//     spike over a sliding or unbounded window).
//   - Energy arbitrage planning for single-cycle battery charge/discharge.
//
// Related LeetCode Problems:
//   - #188 Best Time To Buy And Sell Stock IV [done] (generalizes to at most k transactions)
//   - #122 Best Time to Buy and Sell Stock II [done] (unlimited transactions via greedy positive diffs)
//   - #123 Best Time to Buy and Sell Stock III [new] (at most 2 transactions)
//   - #53 Maximum Subarray [done] (exact same Kadane recurrence on raw values)
//   - #2016 Maximum Difference Between Increasing Elements [new] (identical prefix-min formulation)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the maximum single-transaction profit by tracking the lowest buy
  // price seen so far.
  //
  // Invariant: before processing day i, `min_price` is min(prices[0..i-1]) and
  // `max_profit` is the best profit achievable within prices[0..i-1].
  // Time: O(n), Space: O(1).
  int maxProfit(const std::vector<int>& prices) {
    if (prices.size() < 2) {
      return 0;
    }

    int min_price = prices[0];
    int max_profit = 0;

    for (std::size_t i = 1uz; i < prices.size(); ++i) {
      // Evaluate selling on day i against the cheapest prior buy day.
      max_profit = std::max(max_profit, prices[i] - min_price);
      // Record day i as a candidate buy day for future days.
      min_price = std::min(min_price, prices[i]);
    }

    return max_profit;
  }

  // Finds the maximum single-transaction profit using Kadane's algorithm over
  // telescoping daily price differences.
  //
  // Invariant: `running_profit` is the maximum profit of any transaction ending
  // on day i (clamped at 0 if starting fresh on day i is better).
  // Time: O(n), Space: O(1).
  int maxProfitKadane(const std::vector<int>& prices) {
    if (prices.size() < 2) {
      return 0;
    }

    int running_profit = 0;
    int max_profit = 0;

    for (std::size_t i = 1uz; i < prices.size(); ++i) {
      const int daily_diff = prices[i] - prices[i - 1];
      // If accumulated gain falls below 0, prices[i] is cheaper than our
      // previous buy point, so reset the buy anchor to day i.
      running_profit = std::max(0, running_profit + daily_diff);
      max_profit = std::max(max_profit, running_profit);
    }

    return max_profit;
  }
};

LC_PROBLEM(121, "Best Time To Buy And Sell Stock", "dp", ctx) {
  Solution sol;

  auto check_both = [&](const std::vector<int>& prices, int expected) {
    ctx.expect_eq(sol.maxProfit(prices), expected);
    ctx.expect_eq(sol.maxProfitKadane(prices), expected);
  };

  ctx.test("example_1_standard", [&] {
    check_both({7, 1, 5, 3, 6, 4}, 5);
  });

  ctx.test("example_2_monotonically_decreasing", [&] {
    check_both({7, 6, 4, 3, 1}, 0);
  });

  ctx.test("additional_python_cases", [&] {
    check_both({7, 1, 3}, 2);
    check_both({1, 3, 7, 1}, 6);
    check_both({1, 9, 2, 10}, 9);
  });

  ctx.test("edge_cases_small_and_flat", [&] {
    check_both({}, 0);
    check_both({5}, 0);
    check_both({3, 3, 3, 3}, 0);
    check_both({2, 4, 1}, 2);
  });
}

}  // namespace
