// LeetCode 983: Minimum Cost For Tickets
// https://leetcode.com/problems/minimum-cost-for-tickets/
//
// Problem:
//   You are given a strictly increasing array `days` of travel days within a
//   year (1..365) and a 3-element array `costs = [c1, c7, c30]` representing
//   the prices of 1-day, 7-day, and 30-day consecutive travel passes. Return
//   the minimum total cost required to cover every day in `days`.
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
