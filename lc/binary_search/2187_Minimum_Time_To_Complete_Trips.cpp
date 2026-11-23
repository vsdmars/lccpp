// LeetCode 2187: Minimum Time To Complete Trips
// https://leetcode.com/problems/minimum-time-to-complete-trips/
//
// Problem:
//   Given an integer array `time` where `time[i]` is the time the i-th bus
//   takes to finish one trip, and an integer `totalTrips`, return the minimum
//   total time required for all independently operating buses to collectively
//   complete at least `totalTrips` trips.
//   Constraints: 1 <= time.size() <= 10^5, 1 <= time[i], totalTrips <= 10^7.
//
// Examples:
//   Example 1:
//     Input:  time = [1, 2, 3], totalTrips = 5
//     Output: 3
//     Explanation:
//       - Find min_bus_time = 1 and initialize 64-bit bounds low = 1 and high = 1 *
//         5 = 5.
//       - At mid = 3, can_complete sums 3/1 + 3/2 + 3/3 = 3 + 1 + 1 = 5 >= 5, so
//         high shrinks to 3.
//       - At mid = 2, can_complete sums 2/1 + 2/2 + 2/3 = 2 + 1 + 0 = 3 < 5, so low
//         advances to 3.
//       - The loop ends with low == high == 3 and returns 3.
//
//   Example 2:
//     Input:  time = [5, 5, 5, 5], totalTrips = 13
//     Output: 20
//     Explanation:
//       - Find min_bus_time = 5 and set bounds low = 1 and high = 5 * 13 = 65.
//       - Probes at mid = 33 (can_complete reaches 18 >= 13 after three buses), mid
//         = 25, and mid = 21 all meet target 13, while mid = 17 yields 4 * (17 / 5)
//         = 12 < 13, narrowing [low, high] to [18, 21].
//       - At mid = 19, the four buses complete 4 * (19 / 5) = 12 < 13 trips,
//         raising low to 20.
//       - At mid = 20, the buses complete 4 * (20 / 5) = 16 >= 13 trips, lowering
//         high to 20 and returning 20.
//
//   Example 3:
//     Input:  time = [2], totalTrips = 1
//     Output: 2
//     Explanation:
//       - Find min_bus_time = 2 and initialize bounds low = 1 and high = 2 * 1 = 2.
//       - At mid = 1, can_complete computes 1 / 2 = 0 < 1, advancing low to mid + 1
//         = 2.
//       - The loop terminates with low == high == 2 and returns 2.
//
// Algorithm:
//   Binary search on the answer (parametric search over elapsed time `t`).
//   The total trips completed by time `t`, f(t) = sum_i floor(t / time[i]), is
//   monotonically non-decreasing in `t`. The minimum feasible time lies in
//   [1, min(time) * totalTrips], since even the single fastest bus alone can
//   complete `totalTrips` trips in `min(time) * totalTrips` time.
//   At each midpoint `mid`, check whether f(mid) >= totalTrips (early-exiting
//   once the threshold is reached to keep arithmetic fast and overflow-free):
//   - If feasible, shrink the upper bound `high = mid`.
//   - Otherwise, raise the lower bound `low = mid + 1`.
//   Time Complexity: O(n log(min(time) * totalTrips)), where n = time.size().
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - 64-bit integer search bounds (`std::int64_t`) to hold products up to
//     10^14 without 32-bit overflow.
//   - Read-only linear scan over the `std::vector<int>` bus durations.
//
// Real-World Applications:
//   - Capacity planning and job scheduling across heterogeneous worker pools
//     (e.g., finding the earliest completion SLA across servers of varying speed).
//   - Throughput estimation for parallel batch pipelines, manufacturing lines,
//     and fleet dispatching.
//
// Related LeetCode Problems:
//   - #875 Koko Eating Bananas [done] (binary search on minimum rate)
//   - #1011 Capacity To Ship Packages Within D Days [done] (binary search on capacity)
//   - #410 Split Array Largest Sum [done] (binary search on answer space)
//   - #1482 Minimum Number of Days to Make m Bouquets [new] (parametric time search)
//   - #2594 Minimum Time to Repair Cars [new] (heterogeneous parallel worker timing)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the minimum elapsed time for all buses in `time` to complete at
  // least `total_trips` trips in O(n log(min(time) * total_trips)) time and
  // O(1) space.
  std::int64_t minimumTime(const std::vector<int>& time, int total_trips) {
    if (time.empty() || total_trips <= 0) {
      return 0;
    }

    // Upper bound: the single fastest bus completes all `total_trips` on its
    // own. Cast to 64-bit before multiplying because the product reaches 10^14.
    const std::int64_t min_bus_time = *std::ranges::min_element(time);
    const std::int64_t target = total_trips;
    std::int64_t low = 1;
    std::int64_t high = min_bus_time * target;

    // Lower-bound binary search for the smallest `t` with completed(t) >= target.
    while (low < high) {
      const std::int64_t mid = low + ((high - low) / 2);
      if (can_complete(time, mid, target)) {
        high = mid;
      } else {
        low = mid + 1;
      }
    }

    return low;
  }

 private:
  // Checks whether all buses together complete at least `target` trips within
  // `elapsed` time. Early-exits as soon as the quota is met.
  static bool can_complete(const std::vector<int>& time, std::int64_t elapsed,
                           std::int64_t target) {
    std::int64_t trips = 0;
    for (const int bus_time : time) {
      trips += elapsed / bus_time;
      if (trips >= target) {
        return true;
      }
    }
    return false;
  }
};

LC_PROBLEM(2187, "Minimum Time To Complete Trips", "binary_search", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    ctx.expect_eq(sol.minimumTime({1, 2, 3}, 5), std::int64_t{3});
  });

  ctx.test("example_2_single_bus", [&] {
    ctx.expect_eq(sol.minimumTime({2}, 1), std::int64_t{2});
  });

  ctx.test("identical_buses", [&] {
    // 4 buses each taking 5 units complete 12 trips in 15 units (3 trips each).
    ctx.expect_eq(sol.minimumTime({5, 5, 5, 5}, 12), std::int64_t{15});
    // 13 trips requires one more cycle of 5 -> 20 units.
    ctx.expect_eq(sol.minimumTime({5, 5, 5, 5}, 13), std::int64_t{20});
  });

  ctx.test("large_64bit_answer", [&] {
    // 10^7 * 10^7 = 10^14, verifying no 32-bit overflow in search bounds.
    ctx.expect_eq(sol.minimumTime({10'000'000}, 10'000'000),
                  std::int64_t{100'000'000'000'000});
  });

  ctx.test("skewed_bus_speeds", [&] {
    // Fast bus (time = 1) does all 10 trips before the slow bus (time = 1000)
    // finishes even one.
    ctx.expect_eq(sol.minimumTime({1, 1000, 10000}, 10), std::int64_t{10});
  });
}

}  // namespace
