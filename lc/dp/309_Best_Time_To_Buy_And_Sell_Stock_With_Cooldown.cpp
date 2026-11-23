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
// Examples:
//   Example 1:
//     Input:  prices = [1, 2, 3, 0, 2]
//     Output: 3
//     Explanation:
//       - Initialize rolling states to free = 0, hold = -inf, and cool = -inf
//         before day 0.
//       - On day 0 (price = 1) and day 1 (price = 2), buy into hold = max(-inf, 0 -
//         1) = -1 on day 0 and sell on day 1 to reach cool = -1 + 2 = 1 while free
//         = 0.
//       - On day 2 (price = 3), exiting yesterday's cooldown updates free = max(0,
//         1) = 1 while hold stays -1 and cool becomes -1 + 3 = 2.
//       - On day 3 (price = 0), free updates to max(1, 2) = 2 and buying from
//         yesterday's free = 1 gives hold = max(-1, 1 - 0) = 1 with cool = -1.
//       - On day 4 (price = 2), selling the held share produces cool = 1 + 2 = 3
//         alongside free = 2, returning max(free, cool) = 3.
//
//   Example 2:
//     Input:  prices = [1, 5, 2, 2, 8]
//     Output: 10
//     Explanation:
//       - Start with free = 0, hold = -inf, cool = -inf, then buy on day 0 (price =
//         1) to set hold = -1 and sell on day 1 (price = 5) for cool = 4.
//       - On day 2 (price = 2), the mandatory cooldown moves yesterday's profit
//         into free = max(0, 4) = 4 while hold remains -1 and cool becomes 1.
//       - On day 3 (price = 2), buying from free = 4 raises hold = max(-1, 4 - 2) =
//         2 while free stays 4 and cool stays 1.
//       - On day 4 (price = 8), selling yields cool = 2 + 8 = 10, so max(free,
//         cool) = max(4, 10) = 10.
//
//   Example 3:
//     Input:  prices = [5, 4, 3, 2, 1]
//     Output: 0
//     Explanation:
//       - Begin with free = 0, hold = -inf, and cool = -inf; buying on day 0 (price
//         = 5) sets hold = -5.
//       - Across days 1 through 4 (prices 4, 3, 2, 1), buying later improves hold
//         from -5 up to -1 while every sale produces cool = -1 and free stays
//         max(0, -1) = 0.
//       - At the end of day 4, max(free, cool) = max(0, -1) = 0 because executing
//         no trades beats any losing buy-and-sell pair.
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
