// LeetCode 309: Best Time To Buy And Sell Stock With Cooldown
// https://leetcode.com/problems/best-time-to-buy-and-sell-stock-with-cooldown/
//
// Problem:
//   Given an integer array `prices` where `prices[i]` is the price of a stock
//   on day i, find the maximum profit achievable from any number of buy/sell
//   transactions subject to two constraints:
//     1. You may hold at most one share at a time (must sell before buying).
//     2. After selling a share on day i, you cannot buy on day i + 1 (a
//        mandatory 1-day cooldown).
//
// Algorithm:
//   Bottom-up 3-state finite-state-machine DP with O(1) space.
//   At the end of each day, any valid strategy sits in one of three mutually
//   exclusive states:
//     - `free`: holding no stock and NOT in cooldown (ready to buy tomorrow).
//     - `hold`: currently holding one share (can keep holding or sell).
//     - `cool`: just sold a share today (forced into `free` tomorrow).
//   On each day with stock price `p`, the simultaneous transitions are:
//     next_free = max(free, cool)       // stay free or exit cooldown
//     next_hold = max(hold, free - p)   // keep share or buy from free state
//     next_cool = hold + p              // sell held share today
//   Initially `free = 0` and `hold = cool = -inf` (since we cannot have already
//   bought or sold before day 0). After processing all days, optimal profit
//   cannot end holding an unsold share, so we return `max(free, cool)`.
//   Time Complexity:  O(n), where n = prices.size().
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   Three rolling scalar DP state variables (`free`, `hold`, `cool`) encoding
//   the finite-state automaton of stock ownership and cooldown.
//
// Real-World Applications:
//   - Algorithmic trading / energy-grid battery arbitrage with switching
//     dead-time or settlement-delay constraints (e.g., T+1 settlement rules).
//   - Cloud spot-instance or hardware-accelerator lifecycle management with
//     mandatory cool-down/spin-up periods between reallocation cycles.
//   - Industrial equipment duty-cycle optimization with thermal cooldown windows.
//
// Related LeetCode Problems:
//   - #121 Best Time to Buy and Sell Stock [done] (at most 1 transaction)
//   - #122 Best Time to Buy and Sell Stock II [done] (unlimited transactions without cooldown)
//   - #123 Best Time to Buy and Sell Stock III [new] (at most 2 transactions)
//   - #188 Best Time to Buy and Sell Stock IV [done] (at most k transactions via 2D state DP)
//   - #714 Best Time to Buy and Sell Stock with Transaction Fee [done] (2-state DP with per-trade fee)

#include "framework/lc_framework.h"

#include <algorithm>
#include <limits>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the maximum achievable profit from `prices` with unlimited
  // non-overlapping transactions and a 1-day cooldown after every sale.
  //
  // Invariant: After day i, `free`, `hold`, and `cool` store the maximum net
  // profit achievable ending day i in the ready-to-buy, holding-stock, and
  // just-sold (cooldown) states respectively.
  //
  // Time: O(n), Space: O(1).
  int maxProfit(const std::vector<int>& prices) {
    // Use half of INT_MIN as -infinity so `hold + price` cannot underflow or
    // overflow when prices are non-negative.
    static constexpr int kNegInf = std::numeric_limits<int>::min() / 2;

    int free = 0;
    int hold = kNegInf;
    int cool = kNegInf;

    for (const int price : prices) {
      // Compute all three next states simultaneously from yesterday's states so
      // a share cannot be bought and sold on the same day or immediately after
      // a sale.
      const int next_free = std::max(free, cool);
      const int next_hold = std::max(hold, free - price);
      const int next_cool = hold + price;

      free = next_free;
      hold = next_hold;
      cool = next_cool;
    }

    // Optimal final state holds no stock (either resting or just sold).
    return std::max(free, cool);
  }
};

LC_PROBLEM(309, "Best Time To Buy And Sell Stock With Cooldown", "dp", ctx) {
  Solution sol;

  ctx.test("leetcode_example_and_build", [&] {
    // Buy at 1, sell at 2 (+1), cooldown at 3, buy at 0, sell at 2 (+2) => 3.
    ctx.expect_eq(sol.maxProfit({1, 2, 3, 0, 2}), 3);
  });

  ctx.test("edge_cases", [&] {
    ctx.expect_eq(sol.maxProfit({}), 0);
    ctx.expect_eq(sol.maxProfit({1}), 0);
    ctx.expect_eq(sol.maxProfit({5, 4, 3, 2, 1}), 0);
    ctx.expect_eq(sol.maxProfit({3, 3, 3, 3}), 0);
  });

  ctx.test("monotone_increasing_and_cooldown_tradeoff", [&] {
    // Better to hold through day 1 (buy at 1, sell at 4) than sell at 2.
    ctx.expect_eq(sol.maxProfit({1, 2, 4}), 3);
    // Buy 1, sell 5 (+4), cooldown at 2, buy 2, sell 8 (+6) => 10.
    ctx.expect_eq(sol.maxProfit({1, 5, 2, 2, 8}), 10);
    // Buy 1, sell 6 (+5), cooldown at 4, skip 3, buy 0, sell 2 (+2) => 7.
    ctx.expect_eq(sol.maxProfit({6, 1, 6, 4, 3, 0, 2}), 7);
  });
}

}  // namespace
