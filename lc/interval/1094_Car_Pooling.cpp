// LeetCode 1094: Car Pooling
// https://leetcode.com/problems/car-pooling/
//
// Problem:
//   A vehicle with `capacity` empty seats travels strictly east along a line.
//   Given `trips` where `trips[i] = [num_passengers, from, to]` (`from < to`),
//   return true if and only if the vehicle can pick up and drop off all
//   passengers for every trip without ever exceeding `capacity` seats.
//   Passengers dropped off at a location free their seats before pickups at
//   that same location. Constraints: 1 <= trips.size() <= 1000,
//   0 <= from < to <= 1000, 1 <= capacity <= 10^5.
//
// Examples:
//   Example 1:
//     Input:  trips = [[2, 1, 5], [3, 3, 7]], capacity = 4
//     Output: false
//     Explanation:
//       - Build and sort pickups = [(1, 2), (3, 3)] and dropoffs = [(5, 2), (7, 3)]
//         with available = 4.
//       - At pickup_idx = 0, dropoffs[0].location (5) > pickups[0].location (1), so
//         board 2 passengers: available becomes 4 - 2 = 2.
//       - At pickup_idx = 1, dropoffs[0].location (5) > pickups[1].location (3), so
//         board 3 passengers: available becomes 2 - 3 = -1.
//       - Because available < 0, carPooling returns false immediately.
//
//   Example 2:
//     Input:  trips = [[2, 1, 5], [3, 5, 7]], capacity = 3
//     Output: true
//     Explanation:
//       - Build and sort pickups = [(1, 2), (5, 3)] and dropoffs = [(5, 2), (7, 3)]
//         with available = 3.
//       - At pickup_idx = 0, dropoffs[0].location (5) > pickups[0].location (1), so
//         board 2 passengers: available becomes 3 - 2 = 1.
//       - At pickup_idx = 1, dropoffs[0].location (5) <= pickups[1].location (5)
//         triggers first, reclaiming 2 seats so available becomes 1 + 2 = 3 and
//         dropoff_idx advances to 1.
//       - With dropoffs[1].location (7) > pickups[1].location (5), board 3
//         passengers so available becomes 3 - 3 = 0, then return true as pickup_idx
//         reaches 2.
//
//   Example 3:
//     Input:  trips = [[7, 5, 6], [6, 7, 8], [10, 1, 6]], capacity = 16
//     Output: false
//     Explanation:
//       - Sort pickups by from to [(1, 10), (5, 7), (7, 6)] and dropoffs by to to
//         [(6, 7), (6, 10), (8, 6)] with available = 16.
//       - At pickup_idx = 0, dropoffs[0].location (6) > pickups[0].location (1), so
//         board 10 passengers and update available = 16 - 10 = 6.
//       - At pickup_idx = 1, dropoffs[0].location (6) > pickups[1].location (5), so
//         board 7 passengers and update available = 6 - 7 = -1.
//       - Since available < 0, return false.
//
// Algorithm:
//   Sweep-line with two sorted event arrays and two pointers. Split each trip
//   into a pickup stop `(from, num_passengers)` and a drop-off stop
//   `(to, num_passengers)`, then sort both arrays by `location` ascending.
//   Walk through pickups in order, always processing all drop-offs with
//   `dropoff.location <= pickup.location` first to reclaim seats before
//   boarding. Subtract boarded passengers from `available`; if `available < 0`
//   at any point, return false.
//   Key invariant: before any pickup at coordinate `x`, `available` reflects
//   all drop-offs at coordinates <= `x` and all prior pickups.
//   Time: O(N log N) for sorting, Space: O(N) for the stop arrays.
//
// Data Structures:
//   - `struct Stop`: pairs a road coordinate (`location`) with a passenger
//     delta (`passengers`).
//   - `std::vector<Stop> pickups`, `dropoffs`: separately sorted event lists
//     scanned with two index pointers (`pickup_idx`, `dropoff_idx`).
//
// Real-World Applications:
//   - Ride-sharing, shuttle, and railway seat-inventory validation along a
//     linear route with intermediate stops.
//   - Network bandwidth and link-capacity admission control for scheduled
//     data transfers over shared links.
//   - Cloud compute and memory quota enforcement across overlapping batch jobs.
//
// Related LeetCode Problems:
//   - #253 Meeting Rooms II [done] (unit-weight peak overlap via two-pointer sweep)
//   - #1109 Corporate Flight Bookings [new] (range-addition accumulation on indices)
//   - #731 My Calendar II [done] (online interval booking with capacity bound of 2)
//   - #732 My Calendar III [new] (dynamic peak overlap count via boundary events)
//   - #1854 Maximum Population Year [done] (peak simultaneous count over intervals)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

struct Stop {
  int location = 0;
  int passengers = 0;
};

class Solution {
 public:
  // Sweep-line via two sorted event arrays (pickups sorted by `from`, drop-offs
  // sorted by `to`) and two pointers. Drop-offs at or before the next pickup
  // location are always processed first to free seats before boarding.
  //
  // Time:  O(n log n)
  // Space: O(n)
  bool carPooling(const std::vector<std::vector<int>>& trips, int capacity) {
    std::vector<Stop> pickups;
    std::vector<Stop> dropoffs;
    pickups.reserve(trips.size());
    dropoffs.reserve(trips.size());

    for (const auto& trip : trips) {
      pickups.push_back(Stop{.location = trip[1], .passengers = trip[0]});
      dropoffs.push_back(Stop{.location = trip[2], .passengers = trip[0]});
    }

    std::ranges::sort(pickups, {}, &Stop::location);
    std::ranges::sort(dropoffs, {}, &Stop::location);

    std::size_t pickup_idx = 0uz;
    std::size_t dropoff_idx = 0uz;
    int available = capacity;

    while (pickup_idx < pickups.size()) {
      if (dropoff_idx < dropoffs.size() &&
          dropoffs[dropoff_idx].location <= pickups[pickup_idx].location) {
        available += dropoffs[dropoff_idx].passengers;
        ++dropoff_idx;
        continue;
      }

      available -= pickups[pickup_idx].passengers;
      if (available < 0) {
        return false;
      }
      ++pickup_idx;
    }

    return true;
  }
};

LC_PROBLEM(1094, "Car Pooling", "interval", ctx) {
  Solution sol;

  ctx.test("example_1_exceeds_capacity", [&] {
    ctx.expect_false(sol.carPooling({{2, 1, 5}, {3, 3, 7}}, 4));
  });

  ctx.test("example_2_exact_capacity", [&] {
    ctx.expect_true(sol.carPooling({{2, 1, 5}, {3, 3, 7}}, 5));
  });

  ctx.test("example_3_touching_endpoints", [&] {
    ctx.expect_true(sol.carPooling({{2, 1, 5}, {3, 5, 7}}, 3));
  });

  ctx.test("example_4_multiple_overlaps", [&] {
    ctx.expect_true(sol.carPooling({{3, 2, 7}, {3, 7, 9}, {8, 3, 9}}, 11));
  });

  ctx.test("build_case_exceeds_by_one", [&] {
    ctx.expect_false(sol.carPooling({{7, 5, 6}, {6, 7, 8}, {10, 1, 6}}, 16));
    ctx.expect_true(sol.carPooling({{7, 5, 6}, {6, 7, 8}, {10, 1, 6}}, 17));
  });

  ctx.test("edge_cases", [&] {
    ctx.expect_true(sol.carPooling({}, 1));
    ctx.expect_false(sol.carPooling({{5, 0, 1}}, 4));
    ctx.expect_true(sol.carPooling({{2, 1, 4}, {2, 1, 4}}, 4));
  });
}

}  // namespace
