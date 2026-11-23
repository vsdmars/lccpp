// LeetCode 1854: Maximum Population Year
// https://leetcode.com/problems/maximum-population-year/
//
// Problem:
//   Given a 2D integer array `logs` where `logs[i] = [birth_i, death_i]`
//   records the birth and death years of the i-th person, the person is
//   counted as alive during every year in the half-open interval
//   `[birth_i, death_i)` (not counted in their death year). Return the
//   earliest year with the maximum population.
//   Constraints: 1 <= logs.length <= 100, 1950 <= birth_i < death_i <= 2050.
//
// Algorithm:
//   Line sweep via a fixed-size difference array over the bounded year range
//   `[1950, 2050]`. Each person contributes `+1` at `birth_i - 1950` and `-1`
//   at `death_i - 1950`. A single forward prefix-sum pass reconstructs the
//   exact population in every year from 1950 to 2050. Using a strict greater-
//   than check (`current_pop > max_pop`) automatically breaks ties in favor
//   of the earliest year.
//   Time Complexity:  O(n + Y), where n = logs.size() and Y = 101 years.
//   Space Complexity: O(Y) = O(1) fixed-size stack array.
//
// Data Structures:
//   - `std::array<int, 101>`: difference array storing net population change
//     for each year offset in `[1950, 2050]`.
//
// Real-World Applications:
//   - Peak concurrency analysis for active user sessions, database locks, or
//     cloud VM reservations over discrete time buckets.
//   - Hotel / flight / conference-room capacity planning from check-in and
//     check-out intervals.
//   - Network bandwidth peak utilization aggregation across time-stamped flows.
//
// Related LeetCode Problems:
//   - #253 Meeting Rooms II [done] (maximum simultaneous active intervals via line sweep)
//   - #1094 Car Pooling [done] (difference array over bounded stop coordinates)
//   - #1109 Corporate Flight Bookings [new] (range-addition difference array and prefix sum)
//   - #2406 Divide Intervals Into Minimum Number of Groups [new] (maximum interval overlap count)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the earliest calendar year in [1950, 2050] with peak population.
  // Uses a difference array over the 101-year domain followed by a prefix sum.
  // Time: O(n + Y), Space: O(Y) where Y = 101.
  int maximumPopulation(const std::vector<std::vector<int>>& logs) {
    // delta[y - kMinYear] holds the net population change entering year y.
    std::array<int, kYearSpan> delta{};

    for (const auto& entry : logs) {
      const auto birth_idx = static_cast<std::size_t>(entry[0] - kMinYear);
      const auto death_idx = static_cast<std::size_t>(entry[1] - kMinYear);
      ++delta[birth_idx];
      --delta[death_idx];
    }

    int current_pop = 0;
    int max_pop = 0;
    int best_year = kMinYear;

    // Sweep chronologically so strict `>` naturally preserves the earliest tie.
    for (std::size_t i = 0uz; i < kYearSpan; ++i) {
      current_pop += delta[i];
      if (current_pop > max_pop) {
        max_pop = current_pop;
        best_year = kMinYear + static_cast<int>(i);
      }
    }

    return best_year;
  }

 private:
  static constexpr int kMinYear = 1950;
  static constexpr int kMaxYear = 2050;
  static constexpr std::size_t kYearSpan =
      (static_cast<std::size_t>(kMaxYear) -
       static_cast<std::size_t>(kMinYear)) +
      1uz;
};

LC_PROBLEM(1854, "Maximum Population Year", "math", ctx) {
  ctx.test("example_1_build", [&] {
    Solution sol;
    ctx.expect_eq(sol.maximumPopulation({{1993, 1999}, {2000, 2010}}), 1993);
  });

  ctx.test("example_2_tie_breaks_earliest", [&] {
    Solution sol;
    // Population peaks at 2 in both 1960 and 1970; 1960 is earliest.
    ctx.expect_eq(
        sol.maximumPopulation({{1950, 1961}, {1960, 1971}, {1970, 1981}}),
        1960);
  });

  ctx.test("death_year_not_counted", [&] {
    Solution sol;
    // Adjacent half-open intervals do not overlap; peak population is 1.
    ctx.expect_eq(
        sol.maximumPopulation({{1950, 1960}, {1960, 1970}, {1970, 1980}}),
        1950);
  });

  ctx.test("upper_boundary_year", [&] {
    Solution sol;
    ctx.expect_eq(sol.maximumPopulation({{2049, 2050}, {2049, 2050}}), 2049);
  });

  ctx.test("nested_intervals", [&] {
    Solution sol;
    ctx.expect_eq(sol.maximumPopulation({{1950, 2050},
                                         {1960, 2040},
                                         {1970, 2030},
                                         {1980, 2020}}),
                  1980);
  });
}

}  // namespace
