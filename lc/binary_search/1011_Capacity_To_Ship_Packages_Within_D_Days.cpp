// LeetCode 1011: Capacity To Ship Packages Within D Days
// https://leetcode.com/problems/capacity-to-ship-packages-within-d-days/
//
// Problem:
//   A conveyor belt holds `n` packages with positive integer weights given by
//   `weights`. Packages must be loaded onto a ship strictly in the given order
//   (no reordering). Each day, the ship carries a contiguous batch of packages
//   whose total weight does not exceed the ship's capacity. Return the minimum
//   ship weight capacity needed to ship all packages within `days` days.
//   Constraints: 1 <= days <= weights.length <= 50,000, 1 <= weights[i] <= 500.
//
// Algorithm:
//   Binary search on the ship capacity paired with a greedy linear feasibility
//   check:
//   - Feasibility is monotonic: if capacity `C` suffices to ship within `days`,
//     any capacity `C' > C` also suffices.
//   - Tight bounds: `left = max(weights)` (the heaviest single package must fit
//     on one day) and `right = sum(weights)` (all packages shipped in 1 day).
//   - For a candidate capacity `mid`, greedily pack as many consecutive
//     packages as fit into the current day before starting a new day. Because
//     packing earlier can never worsen the remaining suffix, this greedy rule
//     yields the exact minimum number of days needed for capacity `mid`.
//   - If required days exceed `days`, increase `left = mid + 1`; otherwise
//     tighten `right = mid`.
//   Time Complexity:  O(n log(S - M)), where n = weights.size(),
//                     S = sum(weights), and M = max(weights).
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - `std::vector<int>` (input sequence of package weights).
//   - 64-bit scalar bounds and accumulators (`std::int64_t`) for overflow-safe
//     capacity arithmetic.
//
// Real-World Applications:
//   - Freight, logistics, and container fleet sizing under fixed delivery SLAs.
//   - Partitioning ordered log/data streams into fixed-count shards with
//     minimized maximum shard size (e.g., distributed map-reduce splitting).
//   - Memory buffer sizing for sequential packet/chunk transmission under
//     bounded transmission windows.
//
// Related LeetCode Problems:
//   - #875 Koko Eating Bananas [done] (binary search on minimum feasible rate)
//   - #1231 Divide Chocolate [done] (dual: maximize minimum contiguous subarray sum)
//   - #1552 Magnetic Force Between Two Balls [done] (binary search with greedy placement)
//   - #1891 Cutting Ribbons [done] (binary search on answer with linear check)
//   - #410 Split Array Largest Sum [done] (mathematically identical problem)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the minimum ship capacity required to deliver all `weights` in
  // order within at most `days` days.
  // Time: O(n log(sum - max)), Space: O(1).
  int shipWithinDays(const std::vector<int>& weights, int days) {
    if (weights.empty() || days <= 0) {
      return 0;
    }

    // Establish tight capacity bounds:
    // - Lower bound: the heaviest single package must fit on the ship.
    // - Upper bound: shipping all packages in a single day.
    std::int64_t left = 0;
    std::int64_t right = 0;
    for (int weight : weights) {
      left = std::max<std::int64_t>(left, weight);
      right += weight;
    }

    // Invariant: capacities < left require > days; capacity `right` is always
    // feasible (since days >= 1). Shrink until left == right.
    while (left < right) {
      const std::int64_t mid = left + ((right - left) / 2);
      if (daysRequired(weights, mid) > days) {
        left = mid + 1;
      } else {
        right = mid;
      }
    }

    return static_cast<int>(left);
  }

 private:
  // Computes the minimum number of days needed to ship all `weights` in order
  // with a ship of the given `capacity` (requires capacity >= max(weights)).
  // Time: O(n), Space: O(1).
  static int daysRequired(const std::vector<int>& weights,
                          std::int64_t capacity) {
    int needed_days = 1;
    std::int64_t current_load = 0;
    for (int weight : weights) {
      if (current_load + weight > capacity) {
        // Start a fresh day with the current package.
        ++needed_days;
        current_load = weight;
      } else {
        current_load += weight;
      }
    }
    return needed_days;
  }
};

LC_PROBLEM(1011, "Capacity To Ship Packages Within D Days", "binary_search",
           ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    ctx.expect_eq(sol.shipWithinDays({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, 5), 15);
  });

  ctx.test("example_2", [&] {
    ctx.expect_eq(sol.shipWithinDays({3, 2, 2, 4, 1, 4}, 3), 6);
  });

  ctx.test("example_3", [&] {
    ctx.expect_eq(sol.shipWithinDays({1, 2, 3, 1, 1}, 4), 3);
  });

  ctx.test("single_day_equals_total_weight", [&] {
    ctx.expect_eq(sol.shipWithinDays({1, 2, 3, 4, 5}, 1), 15);
  });

  ctx.test("one_package_per_day_equals_max_weight", [&] {
    ctx.expect_eq(sol.shipWithinDays({3, 2, 8, 1, 4}, 5), 8);
  });

  ctx.test("single_package", [&] {
    ctx.expect_eq(sol.shipWithinDays({42}, 1), 42);
  });
}

}  // namespace
