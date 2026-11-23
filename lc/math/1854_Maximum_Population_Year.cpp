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
// Examples:
//   Example 1:
//     Input:  logs = [[1950, 1961], [1960, 1971], [1970, 1981]]
//     Output: 1960
//     Explanation:
//       - Record +1 in delta at indices 0, 10, and 20 (years 1950, 1960, 1970) and
//         -1 at indices 11, 21, and 31 (years 1961, 1971, 1981).
//       - Sweeping forward, current_pop becomes 1 at 1950 (max_pop = 1, best_year =
//         1950) and rises to 2 at 1960 (max_pop = 2, best_year = 1960).
//       - At 1961 (delta[11] = -1) current_pop drops to 1, and at 1970 (delta[20] =
//         +1) current_pop reaches 2, which is not strictly greater than max_pop = 2
//         so best_year stays 1960.
//       - Decrements at 1971 and 1981 bring current_pop back to 0, returning 1960.
//
//   Example 2:
//     Input:  logs = [[1950, 1960], [1960, 1970], [1970, 1980]]
//     Output: 1950
//     Explanation:
//       - Record +1 at years 1950, 1960, 1970 (indices 0, 10, 20) and -1 at years
//         1960, 1970, 1980 (indices 10, 20, 30), causing delta[10] and delta[20] to
//         cancel to 0.
//       - At i = 0 (1950), current_pop increases to 1, setting max_pop = 1 and
//         best_year = 1950.
//       - From 1951 through 1979 current_pop stays at 1 (never exceeding max_pop)
//         and drops to 0 at 1980, so 1950 is returned.
//
//   Example 3:
//     Input:  logs = [[1950, 2050], [1960, 2040], [1970, 2030], [1980, 2020]]
//     Output: 1980
//     Explanation:
//       - Add +1 to delta at indices 0, 10, 20, and 30 (years 1950, 1960, 1970,
//         1980) and -1 at indices 70, 80, 90, and 100 (years 2020, 2030, 2040,
//         2050).
//       - During the prefix-sum sweep, current_pop and best_year step upward to 1
//         (1950), 2 (1960), 3 (1970), and peak at 4 in 1980.
//       - From 1981 to 2050, current_pop remains 4 until 2020 and then steps down
//         by 1 at 2020, 2030, 2040, and 2050, returning 1980.
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
