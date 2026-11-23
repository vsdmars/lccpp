// LeetCode 983: Minimum Cost For Tickets
// https://leetcode.com/problems/minimum-cost-for-tickets/
//
// Problem:
//   You are given a strictly increasing array `days` of travel days within a
//   year (1..365) and a 3-element array `costs = [c1, c7, c30]` representing
//   the prices of 1-day, 7-day, and 30-day consecutive travel passes. Return
//   the minimum total cost required to cover every day in `days`.
//
// Examples:
//   Example 1:
//     Input:  days = [1, 4, 6, 7, 8, 20], costs = [2, 7, 15]
//     Output: 11
//     Explanation:
//       - Allocate dp of size 21 with dp[0] = 0 and set day_idx = 0; at travel day
//         d = 1, dp[1] = min(2 + dp[0], 7 + dp[0], 15 + dp[0]) = 2.
//       - Non-travel days d = 2 and d = 3 carry forward dp[2] = dp[3] = 2, then
//         travel days d = 4 and d = 6 extend via 1-day passes to dp[4] = 4 and
//         dp[6] = 6 (with dp[5] = 4).
//       - At d = 7, the 7-day pass beats a 1-day pass (7 + dp[0] = 7 vs 2 + dp[6] =
//         8), so dp[7] = 7, and at d = 8, dp[8] = min(2 + dp[7], 7 + dp[1], 15 +
//         dp[0]) = 9.
//       - Carry dp[8] = 9 across non-travel days 9..19, then at d = 20 pick min(2 +
//         dp[19], 7 + dp[13], 15 + dp[0]) = min(11, 16, 15) = 11 and return dp[20]
//         = 11.
//
//   Example 2:
//     Input:  days = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 30, 31], costs = [2, 7, 15]
//     Output: 17
//     Explanation:
//       - For d = 1..3, 1-day passes are cheapest (dp[1] = 2, dp[2] = 4, dp[3] =
//         6); for d = 4..7, buying a 7-day pass at dp[0] gives dp[4..7] = 7.
//       - For d = 8..10, 1-day or shifted 7-day passes yield dp[8] = 9, dp[9] = 11,
//         and dp[10] = 13, which carries unchanged through non-travel days d =
//         11..29.
//       - At d = 30, min(2 + dp[29], 7 + dp[23], 15 + dp[0]) = min(15, 20, 15) =
//         15, covering days 1..30 with one 30-day pass.
//       - At d = 31, min(2 + dp[30], 7 + dp[24], 15 + dp[1]) = min(17, 20, 17) =
//         17, so the method returns dp[31] = 17.
//
//   Example 3:
//     Input:  days = [1, 10, 25], costs = [10, 10, 5]
//     Output: 5
//     Explanation:
//       - At d = 1, compare 1-day (10 + dp[0] = 10), 7-day (10 + dp[0] = 10), and
//         30-day (5 + dp[0] = 5) passes, setting dp[1] = 5.
//       - Carry dp[1] = 5 through non-travel days 2..9; at d = 10, 5 + dp[0] = 5
//         still beats 10 + dp[9] = 15 and 10 + dp[3] = 15, giving dp[10] = 5.
//       - Carry 5 through days 11..24 and at d = 25 select 5 + dp[0] = 5 over 15,
//         returning dp[25] = 5.
//
// Algorithm:
//   Bottom-up calendar dynamic programming. Let `dp[d]` be the minimum cost to
//   cover all planned travel days in `[1, d]`, with `dp[0] = 0`. Iterate day
//   `d` from `1` through `days.back()` while advancing a single index pointer
//   `day_idx` through the sorted `days` array:
//     - If `d` is not a travel day, no ticket needs to expire on `d`, so
//       `dp[d] = dp[d - 1]`.
//     - If `d` is a travel day, choose the cheapest pass ending on `d`:
//         `dp[d] = min(dp[max(0, d - 1)]  + costs[0],`
//                     `dp[max(0, d - 7)]  + costs[1],`
//                     `dp[max(0, d - 30)] + costs[2])`.
//   Time complexity: O(D), where D = days.back() <= 365.
//   Space complexity: O(D).
//
// Data Structures:
//   - std::vector<int>: 1D DP table `dp` of length `days.back() + 1` indexed
//     directly by calendar day for O(1) lookbacks at `d - 1`, `d - 7`, `d - 30`.
//   - Single `std::size_t` pointer (`day_idx`) exploiting `days`'s sortedness
//     for O(1) travel-day membership checks without a hash set.
//
// Real-World Applications:
//   - Cloud compute reservation planning (choosing on-demand vs. 1-week vs.
//     1-month committed-use discounts for bursty workloads).
//   - Transit fare capping and subscription tier optimization.
//   - Interval-covering cost minimization in telecommunications and API
//     billing plans.
//
// Related LeetCode Problems:
//   - #322 Coin Change [done] (unbounded minimum-cost 1D DP)
//   - #746 Min Cost Climbing Stairs [done] (1D DP with fixed lookback steps)
//   - #1335 Minimum Difficulty of a Job Schedule [new] (partitioning days DP)
//   - #279 Perfect Squares [done] (minimum-cost lookback 1D DP)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

constexpr std::size_t kWeekPassDays = 7uz;
constexpr std::size_t kMonthPassDays = 30uz;

class Solution {
 public:
  // Computes the minimum dollar cost to cover every travel day in `days` using
  // 1-day (`costs[0]`), 7-day (`costs[1]`), and 30-day (`costs[2]`) passes.
  //
  // Invariant: After iteration `d`, `dp[d]` stores the minimum cost covering
  // all travel days `<= d`, and `day_idx` points to the first travel day > `d`.
  // Time: O(D); Space: O(D), where D = days.back().
  int mincostTickets(const std::vector<int>& days,
                     const std::vector<int>& costs) {
    if (days.empty()) {
      return 0;
    }

    const auto last_day = static_cast<std::size_t>(days.back());
    std::vector<int> dp(last_day + 1uz, 0);
    std::size_t day_idx = 0uz;

    for (std::size_t d = 1uz; d <= last_day; ++d) {
      // Non-travel days carry forward the prior minimum cost unchanged.
      if (std::cmp_not_equal(d, days[day_idx])) {
        dp[d] = dp[d - 1uz];
        continue;
      }

      ++day_idx;
      const int buy_one = costs[0] + dp[d - 1uz];
      const int buy_seven =
          costs[1] + dp[d > kWeekPassDays ? d - kWeekPassDays : 0uz];
      const int buy_thirty =
          costs[2] + dp[d > kMonthPassDays ? d - kMonthPassDays : 0uz];
      dp[d] = std::min({buy_one, buy_seven, buy_thirty});
    }

    return dp[last_day];
  }
};

LC_PROBLEM(983, "Minimum Cost For Tickets", "dp", ctx) {
  Solution sol;

  ctx.test("example_1_mixed_passes", [&] {
    ctx.expect_eq(sol.mincostTickets({1, 4, 6, 7, 8, 20}, {2, 7, 15}), 11);
  });

  ctx.test("example_2_month_plus_day_pass", [&] {
    ctx.expect_eq(
        sol.mincostTickets({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 30, 31}, {2, 7, 15}),
        17);
  });

  ctx.test("python_build_week_beats_three_days", [&] {
    ctx.expect_eq(sol.mincostTickets({1, 2, 3}, {3, 7, 15}), 7);
  });

  ctx.test("non_monotonic_pass_prices", [&] {
    // Longer pass cheaper than shorter pass on a single or sparse schedule.
    ctx.expect_eq(sol.mincostTickets({15}, {5, 3, 10}), 3);
    ctx.expect_eq(sol.mincostTickets({1, 10, 25}, {10, 10, 5}), 5);
  });

  ctx.test("empty_schedule", [&] {
    ctx.expect_eq(sol.mincostTickets({}, {2, 7, 15}), 0);
  });
}

}  // namespace
