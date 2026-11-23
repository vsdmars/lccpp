// LeetCode 568: Maximum Vacation Days
// https://leetcode.com/problems/maximum-vacation-days/
//
// Problem:
//   There are N cities (indexed 0..N-1) connected by directed Monday-morning
//   flights represented by an N x N binary matrix `flights` (flights[i][j] == 1
//   means a flight from city i to city j exists; flights[i][i] == 0, though
//   staying in the same city is always allowed). Over K weeks, an N x K matrix
//   `days` specifies `days[c][w]`, the maximum vacation days available in city
//   c during week w. Starting in city 0 on week 0's Monday (with the option to
//   fly immediately on Monday morning), return the maximum total vacation days
//   achievable over all K weeks.
//
// Algorithm:
//   Space-optimized bottom-up dynamic programming over weeks.
//   Maintain a 1D array `dp` of length N where `dp[c]` is the maximum total
//   vacation days accumulated after completing all weeks processed so far while
//   ending in city `c` (or a negative-infinity sentinel if city `c` is
//   unreachable).
//   - Initialization: before week 0, only city 0 is reachable (`dp[0] = 0`).
//   - Transition for each week `w` in 0..K-1: initialize `next_dp` to negative
//     infinity. For every reachable `from_city`, every valid destination
//     `to_city` (either `from_city == to_city` or `flights[from_city][to_city]
//     == 1`) relaxes:
//       next_dp[to_city] = max(next_dp[to_city],
//                              dp[from_city] + days[to_city][w]).
//   - Swap `dp` and `next_dp` after each week.
//   After K weeks, the answer is `max(dp)`.
//   Time Complexity: O(K * N^2).
//   Space Complexity: O(N) for two rolling city-state vectors.
//
// Data Structures:
//   - Two 1D rolling DP vectors (`std::vector<int>` of length N): `dp` holds
//     the previous week's best totals per city, and `next_dp` accumulates the
//     current week's transitions without reallocating each iteration.
//
// Real-World Applications:
//   - Multi-period workforce / field-technician routing and crew scheduling
//     across hubs with weekly site-value weights and connectivity constraints.
//   - Dynamic resource placement across geo-distributed data centers where
//     workload migration is constrained by link topology per scheduling epoch.
//   - Viterbi-style maximum-reward path decoding on time-expanded state graphs.
//
// Related LeetCode Problems:
//   - #787 Cheapest Flights Within K Stops [done] (bounded-step shortest path DP on flight graphs)
//   - #120 Triangle [done] (layered DAG path-sum dynamic programming)
//   - #931 Minimum Falling Path Sum [new] (rolling-row grid transition DP)
//   - #1575 Count All Possible Routes [new] (city-transition DP with resource budget)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <utility>
#include <vector>

namespace {

constexpr int kUnreachable = std::numeric_limits<int>::min();

class Solution {
 public:
  // Computes the maximum total vacation days across all weeks using rolling
  // layered DP over cities.
  // Time: O(K * N^2), Space: O(N).
  int maxVacationDays(const std::vector<std::vector<int>>& flights,
                      const std::vector<std::vector<int>>& days) {
    if (flights.empty() || days.empty() || days[0].empty()) {
      return 0;
    }

    const std::size_t num_cities = flights.size();
    const std::size_t num_weeks = days[0].size();

    // dp[c] stores the best vacation-day total ending in city `c` after the
    // previous week. Initially, we start at city 0 with 0 days accumulated.
    std::vector<int> dp(num_cities, kUnreachable);
    std::vector<int> next_dp(num_cities, kUnreachable);
    dp[0] = 0;

    for (std::size_t week = 0uz; week < num_weeks; ++week) {
      std::ranges::fill(next_dp, kUnreachable);

      for (std::size_t from_city = 0uz; from_city < num_cities; ++from_city) {
        // Skip cities that cannot be reached by the end of the previous week.
        if (dp[from_city] == kUnreachable) {
          continue;
        }
        relaxFromCity(from_city, week, dp[from_city], flights, days, next_dp);
      }

      // Advance the rolling state to the completed week in O(1) without
      // allocating a fresh vector.
      std::swap(dp, next_dp);
    }

    return std::ranges::max(dp);
  }

 private:
  // Relaxes all valid Monday transitions (staying or taking a direct flight)
  // out of `from_city` for the given `week`.
  static void relaxFromCity(std::size_t from_city, std::size_t week,
                            int base_days,
                            const std::vector<std::vector<int>>& flights,
                            const std::vector<std::vector<int>>& days,
                            std::vector<int>& next_dp) {
    const std::size_t num_cities = flights.size();
    for (std::size_t to_city = 0uz; to_city < num_cities; ++to_city) {
      if (from_city == to_city || flights[from_city][to_city] != 0) {
        const int candidate = base_days + days[to_city][week];
        next_dp[to_city] = std::max(next_dp[to_city], candidate);
      }
    }
  }
};

LC_PROBLEM(568, "Maximum Vacation Days", "dp", ctx) {
  ctx.test("example_1_full_connectivity", [&] {
    Solution sol;
    const std::vector<std::vector<int>> flights = {
        {0, 1, 1}, {1, 0, 1}, {1, 1, 0}};
    const std::vector<std::vector<int>> days = {
        {1, 3, 1}, {6, 0, 3}, {3, 3, 3}};
    ctx.expect_eq(sol.maxVacationDays(flights, days), 12);
  });

  ctx.test("example_2_no_flights", [&] {
    Solution sol;
    const std::vector<std::vector<int>> flights = {
        {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    const std::vector<std::vector<int>> days = {
        {1, 1, 1}, {7, 7, 7}, {7, 7, 7}};
    ctx.expect_eq(sol.maxVacationDays(flights, days), 3);
  });

  ctx.test("example_3_diagonal_schedule", [&] {
    Solution sol;
    const std::vector<std::vector<int>> flights = {
        {0, 1, 1}, {1, 0, 1}, {1, 1, 0}};
    const std::vector<std::vector<int>> days = {
        {7, 0, 0}, {0, 7, 0}, {0, 0, 7}};
    ctx.expect_eq(sol.maxVacationDays(flights, days), 21);
  });

  ctx.test("python_build_case", [&] {
    Solution sol;
    const std::vector<std::vector<int>> flights = {
        {0, 1, 1}, {1, 0, 1}, {1, 1, 0}};
    const std::vector<std::vector<int>> days = {
        {5, 0, 0}, {1, 7, 7}, {0, 0, 7}};
    // Week 0: stay at city 0 (5); Week 1: fly to city 1 (7); Week 2: stay or
    // fly to city 2 (7) -> 19.
    ctx.expect_eq(sol.maxVacationDays(flights, days), 19);
  });

  ctx.test("unreachable_high_value_city_ignored", [&] {
    Solution sol;
    // City 0 can reach city 1, but city 2 is completely isolated.
    const std::vector<std::vector<int>> flights = {
        {0, 1, 0}, {1, 0, 0}, {0, 0, 0}};
    const std::vector<std::vector<int>> days = {
        {2, 2}, {3, 4}, {7, 7}};
    ctx.expect_eq(sol.maxVacationDays(flights, days), 7);
  });

  ctx.test("single_city_single_week", [&] {
    Solution sol;
    const std::vector<std::vector<int>> flights = {{0}};
    const std::vector<std::vector<int>> days = {{4}};
    ctx.expect_eq(sol.maxVacationDays(flights, days), 4);
  });
}

}  // namespace
