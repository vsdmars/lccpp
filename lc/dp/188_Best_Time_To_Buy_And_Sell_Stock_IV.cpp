// LeetCode 188: Best Time To Buy And Sell Stock IV
// https://leetcode.com/problems/best-time-to-buy-and-sell-stock-iv/
//
// Problem:
//   Given an integer k and an array `prices` where `prices[j]` is the price of
//   a stock on day j, find the maximum profit achievable with at most k
//   completed transactions (each transaction is one buy followed by one sell).
//   You cannot hold more than one share at a time (must sell before buying
//   again, though same-day sell-and-rebuy is permitted).
//   Constraints: 0 <= k <= 100; 0 <= prices.length <= 1000;
//   0 <= prices[j] <= 1000.
//
// Algorithm:
//   1. Unlimited-Transaction Fast Path:
//      - Because each profitable transaction requires at least 2 distinct days
//        (a buy day and a later sell day), when k >= n / 2 the transaction cap
//        is never binding. Sum every positive daily increase
//        max(0, prices[j] - prices[j - 1]) in O(n) time and O(1) space.
//   2. Bottom-Up Local/Global DP with Rolling Transaction Rows (for k < n / 2):
//      - For transaction count i in 1..k:
//          * `local_max[j]` = max profit using at most i transactions where the
//            i-th transaction sells on day j.
//          * `curr_global[j]` = max profit using at most i transactions up
//            through day j (not required to sell on day j).
//      - Letting `diff = prices[j] - prices[j - 1]`:
//          * To sell on day j, either:
//            (a) Finish i - 1 transactions by day j - 1 and execute a fresh
//                1-day trade [j - 1 -> j] (or 0-profit same-day trade [j -> j]):
//                `prev_global[j - 1] + max(diff, 0)`.
//            (b) Extend the i-th transaction that sold on day j - 1 to sell on
//                day j instead (telescoping difference adds `diff` without
//                consuming an extra transaction): `local_max[j - 1] + diff`.
//          * `curr_global[j] = max(curr_global[j - 1], local_max[j])`.
//      - Rolling `prev_global` and `curr_global` keeps auxiliary space O(n).
//   - Time Complexity: O(n) when k >= n / 2; O(k * n) otherwise.
//   - Space Complexity: O(1) when k >= n / 2; O(n) otherwise.
//
// Data Structures:
//   - Three 1D DP vectors (`prev_global`, `curr_global`, `local_max`, each
//     std::vector<int> of size n) storing rolling transaction-layer states.
//
// Real-World Applications:
//   - Quantitative trading strategy optimization under strict turnover or
//     transaction-count budgets (limiting brokerage fees and market impact).
//   - Energy storage scheduling where battery cycle life limits the maximum
//     number of charge/discharge cycles per horizon.
//   - Cloud spot-instance / resource reservation switching with a cap on
//     reconfiguration events.
//
// Related LeetCode Problems:
//   - #121 Best Time To Buy And Sell Stock [done] (special case k = 1)
//   - #122 Best Time to Buy and Sell Stock II [done] (unlimited transactions, used as the k >= n / 2 fast path)
//   - #123 Best Time to Buy and Sell Stock III [new] (special case k = 2)
//   - #309 Best Time to Buy and Sell Stock with Cooldown [done] (state-machine stock DP with 1-day cooldown)
//   - #714 Best Time to Buy and Sell Stock with Transaction Fee [done] (stock DP with per-trade fee penalty)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the maximum stock profit with at most `k` transactions.
  //
  // Invariant: after outer iteration `i`, `prev_global[j]` equals the maximum
  // profit achievable within days [0..j] using at most `i` transactions.
  // Time: O(k * n) (O(n) when k >= n / 2), Space: O(n) (O(1) when k >= n / 2).
  int maxProfit(int k, const std::vector<int>& prices) {
    const std::size_t n = prices.size();
    if (k <= 0 || n < 2) {
      return 0;
    }

    // Fast path: with at least n / 2 transactions available, we can capture
    // every upward price movement independently.
    const auto max_transactions = static_cast<std::size_t>(k);
    if (max_transactions >= n / 2) {
      return maxProfitUnlimited(prices);
    }

    // Rolling DP arrays over transaction layers:
    // - prev_global[j]: best profit in [0..j] using <= i - 1 transactions.
    // - curr_global[j]: best profit in [0..j] using <= i transactions.
    // - local_max[j]:   best profit in [0..j] using <= i transactions with the
    //                   final sell occurring on day j.
    std::vector<int> prev_global(n, 0);
    std::vector<int> curr_global(n, 0);
    std::vector<int> local_max(n, 0);

    for (std::size_t i = 1uz; i <= max_transactions; ++i) {
      local_max[0] = 0;
      curr_global[0] = 0;

      for (std::size_t j = 1uz; j < n; ++j) {
        const int diff = prices[j] - prices[j - 1];

        // Start a new i-th transaction on day j - 1 (or buy+sell on day j for
        // 0 gain) vs. defer the i-th sale from day j - 1 to day j.
        const int start_new = prev_global[j - 1] + std::max(diff, 0);
        const int extend_prev = local_max[j - 1] + diff;
        local_max[j] = std::max(start_new, extend_prev);

        // Best overall by day j either skips selling on day j or sells today.
        curr_global[j] = std::max(curr_global[j - 1], local_max[j]);
      }

      std::swap(prev_global, curr_global);
    }

    return prev_global[n - 1];
  }

 private:
  // Greedy O(n) solver for unlimited transactions (equivalent to LC #122).
  static int maxProfitUnlimited(const std::vector<int>& prices) {
    int profit = 0;
    for (std::size_t j = 1uz; j < prices.size(); ++j) {
      if (prices[j] > prices[j - 1]) {
        profit += prices[j] - prices[j - 1];
      }
    }
    return profit;
  }
};

LC_PROBLEM(188, "Best Time To Buy And Sell Stock IV", "dp", ctx) {
  Solution sol;

  ctx.test("example_1_two_transactions_one_needed", [&] {
    ctx.expect_eq(sol.maxProfit(2, {2, 4, 1}), 2);
  });

  ctx.test("example_2_two_transactions", [&] {
    ctx.expect_eq(sol.maxProfit(2, {3, 2, 6, 5, 0, 3}), 7);
  });

  ctx.test("python_build_monotonic_increase", [&] {
    ctx.expect_eq(sol.maxProfit(2, {1, 3, 4, 7, 9, 10}), 9);
  });

  ctx.test("single_transaction_matches_lc121", [&] {
    ctx.expect_eq(sol.maxProfit(1, {7, 1, 5, 3, 6, 4}), 5);
  });

  ctx.test("large_k_unlimited_fast_path", [&] {
    // Upward steps: (2-1) + (3-2) + (5-2) = 5.
    ctx.expect_eq(sol.maxProfit(100, {1, 2, 3, 2, 5}), 5);
  });

  ctx.test("edge_cases_zero_k_small_or_decreasing", [&] {
    ctx.expect_eq(sol.maxProfit(0, {1, 5, 10}), 0);
    ctx.expect_eq(sol.maxProfit(2, {}), 0);
    ctx.expect_eq(sol.maxProfit(2, {5}), 0);
    ctx.expect_eq(sol.maxProfit(3, {9, 7, 4, 2, 1}), 0);
  });
}

}  // namespace
