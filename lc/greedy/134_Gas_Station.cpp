// LeetCode 134: Gas Station
// https://leetcode.com/problems/gas-station/
//
// Problem:
//   There are `n` gas stations arranged along a circular route. Station `i`
//   supplies `gas[i]` units of fuel, and traveling clockwise from station `i`
//   to `(i + 1) % n` consumes `cost[i]` units of fuel. Starting with an empty
//   tank at one station (tank capacity is unlimited), return the unique
//   starting station index that allows completing one full clockwise circuit,
//   or `-1` if no such starting station exists.
//
// Algorithm:
//   Single-pass greedy prefix-reset algorithm based on two observations:
//   1. Global feasibility: A full circuit exists if and only if
//      `sum(gas[i] - cost[i]) >= 0`. If total fuel is less than total cost, no
//      starting point can finish.
//   2. Segment elimination: Suppose starting from `start` keeps `tank >= 0`
//      through stations `start .. i - 1`, and first drops to `tank < 0` upon
//      leaving station `i`. Then NO station `k` in `[start, i]` can be a valid
//      starting point—because arriving at `k` from `start` had `tank >= 0`,
//      starting fresh at `k` with `tank == 0` yields an equal or lower balance
//      at every step and will also run dry at or before `i`.
//   Therefore, whenever `tank < 0` at index `i`, we discard `[start, i]`, set
//   `start = i + 1`, and reset `tank = 0`. When the pass completes, if
//   `total_surplus >= 0`, the remaining candidate `start` (which finishes the
//   suffix `[start, n - 1]` with enough surplus to cover the deficit of
//   `[0, start - 1]`) is guaranteed to be the unique valid answer.
//
//   Time Complexity:  O(n) single linear pass over `gas` and `cost`.
//   Space Complexity: O(1) constant extra memory.
//
// Data Structures:
//   - Three scalar accumulators (`total_surplus`, `tank`, `start`) — no
//     auxiliary containers needed.
//
// Real-World Applications:
//   - Circular route and logistics planning for delivery vehicles, electric
//     shuttles, or satellites with cyclic energy harvesting and discharge.
//   - Ring-buffer / cyclic producer-consumer scheduling: finding a starting
//     phase in a periodic workload where buffer occupancy never underflows.
//   - Financial cash-flow cycle analysis to locate the start date of a
//     periodic budget cycle that never dips below zero liquidity.
//
// Related LeetCode Problems:
//   - #55 Jump Game [done] (single-pass greedy reachability)
//   - #53 Maximum Subarray [done] (Kadane's prefix-reset greedy invariant)
//   - #918 Maximum Sum Circular Subarray [new] (circular prefix-sum analysis)
//   - #871 Minimum Number of Refueling Stops [new] (greedy refueling with max-heap)
//   - #135 Candy [done] (greedy allocation along an array)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the unique starting station index that completes the circular
  // route, or `-1` if no valid starting station exists.
  //
  // Invariant: Every station in `[0, start - 1]` has been proven infeasible,
  // and starting at `start` keeps the running fuel balance `tank >= 0` across
  // `[start, i]`.
  // Time: O(n), Space: O(1).
  int canCompleteCircuit(const std::vector<int>& gas,
                         const std::vector<int>& cost) {
    if (gas.empty() || gas.size() != cost.size()) {
      return -1;
    }

    std::int64_t total_surplus = 0;  // Net fuel across the entire circle.
    std::int64_t tank = 0;           // Fuel from current candidate `start`.
    std::size_t start = 0;

    for (std::size_t i = 0; i < gas.size(); ++i) {
      const auto delta =
          static_cast<std::int64_t>(gas[i]) - static_cast<std::int64_t>(cost[i]);
      total_surplus += delta;
      tank += delta;

      if (tank < 0) {
        // No station in `[start, i]` can reach `i + 1`; advance the candidate
        // start past the deficit segment and reset the local tank.
        start = i + 1;
        tank = 0;
      }
    }

    // If overall fuel covers overall cost, `start`'s suffix surplus is
    // guaranteed to absorb the prefix deficit `[0, start - 1]`.
    if (total_surplus < 0) {
      return -1;
    }
    return static_cast<int>(start);
  }
};

LC_PROBLEM(134, "Gas Station", "greedy", ctx) {
  ctx.test("example_1_valid_start_at_3", [&] {
    Solution sol;
    ctx.expect_eq(sol.canCompleteCircuit({1, 2, 3, 4, 5}, {3, 4, 5, 1, 2}), 3);
  });

  ctx.test("example_2_impossible_circuit", [&] {
    Solution sol;
    ctx.expect_eq(sol.canCompleteCircuit({2, 3, 4}, {3, 4, 3}), -1);
  });

  ctx.test("python_build_case_net_deficit", [&] {
    Solution sol;
    ctx.expect_eq(
        sol.canCompleteCircuit({4, 5, 2, 6, 5, 3}, {3, 2, 7, 3, 2, 9}), -1);
  });

  ctx.test("single_station_cases", [&] {
    Solution sol;
    ctx.expect_eq(sol.canCompleteCircuit({5}, {4}), 0);
    ctx.expect_eq(sol.canCompleteCircuit({0}, {0}), 0);
    ctx.expect_eq(sol.canCompleteCircuit({2}, {3}), -1);
  });

  ctx.test("exact_zero_surplus_wrap_around", [&] {
    Solution sol;
    ctx.expect_eq(sol.canCompleteCircuit({5, 1, 2, 3, 4}, {4, 4, 1, 5, 1}), 4);
    ctx.expect_eq(sol.canCompleteCircuit({3, 1, 1}, {1, 2, 2}), 0);
  });
}

}  // namespace
