// https://leetcode.com/problems/car-pooling/
//
// A vehicle with `capacity` empty seats drives only east. Given trips where
// trips[i] = [num_passengers, from, to], return true if and only if it is
// possible to pick up and drop off all passengers for all trips without ever
// exceeding `capacity`.

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
