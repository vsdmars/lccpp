// LeetCode 1029: Two City Scheduling
// https://leetcode.com/problems/two-city-scheduling/
//
// Problem:
//   A company is interviewing `2n` candidates. Given an array `costs` of length
//   `2n` where `costs[i] = [a_cost, b_cost]` is the cost to fly candidate `i`
//   to City A or City B respectively, return the minimum total cost to fly all
//   candidates such that exactly `n` candidates arrive in City A and `n` arrive
//   in City B.
//   Constraints: costs.length == 2n, 2 <= costs.length <= 100,
//   1 <= a_cost, b_cost <= 1000.
//
// Examples:
//   Example 1:
//     Input:  costs = [[10, 20], [30, 200], [400, 50], [30, 20]]
//     Output: 110
//     Explanation:
//       - Sum all City A costs as the baseline: total_cost = 10 + 30 + 400 + 30 =
//         470, and record deltas (b_cost - a_cost) = [10, 170, -350, -10].
//       - Sort deltas ascending to [-350, -10, 10, 170].
//       - Add the first half = 4 / 2 = 2 deltas to switch those candidates to City
//         B: total_cost = 470 + (-350) + (-10) = 110.
//
//   Example 2:
//     Input:  costs = [[259, 770], [448, 54], [926, 667], [184, 139], [840, 118],
//             [577, 469]]
//     Output: 1859
//     Explanation:
//       - Compute the all-City-A baseline total_cost = 259 + 448 + 926 + 184 + 840
//         + 577 = 3234 and deltas = [511, -394, -259, -45, -722, -108].
//       - Sort deltas ascending to [-722, -394, -259, -108, -45, 511].
//       - Add the first half = 6 / 2 = 3 deltas (-722 + -394 + -259 = -1375) to
//         total_cost, returning 3234 - 1375 = 1859.
//
//   Example 3:
//     Input:  costs = [[100, 1], [1, 100]]
//     Output: 2
//     Explanation:
//       - Compute baseline total_cost = 100 + 1 = 101 and deltas = [-99, 99].
//       - Sort deltas ascending to [-99, 99].
//       - Add the first half = 2 / 2 = 1 delta (-99), yielding total_cost = 101 -
//         99 = 2.
//
// Algorithm:
//   Greedy opportunity-cost ("refund") minimization.
//   Start with the baseline assignment of flying all `2n` candidates to City A,
//   incurring `total_cost = sum(a_cost_i)`. We must then reassign exactly `n`
//   candidates from City A to City B. Switching candidate `i` changes the total
//   cost by `delta_i = b_cost_i - a_cost_i` (negative values represent net
//   savings / refunds; positive values represent extra cost).
//   Because the `n` chosen candidates contribute independently to the final sum
//   `sum(a_cost_i) + sum_{i in B}(b_cost_i - a_cost_i)`, the global optimum is
//   achieved by picking the `n` candidates with the smallest (most negative)
//   `delta_i` values. Sorting `deltas` ascending and adding its first `n`
//   entries yields the minimum total cost.
//   Time Complexity:  O(N log N) where N = 2n = costs.size() (can also be done
//                     in O(N) expected time with std::ranges::nth_element).
//   Space Complexity: O(N) for the `deltas` array.
//
// Data Structures:
//   - std::vector<int> deltas: stores the per-candidate switch cost
//     `b_cost - a_cost`, sorted ascending to select the `n` best reassignments.
//
// Real-World Applications:
//   - Cloud workload & data-center placement: partitioning jobs evenly across
//     two availability zones with differing egress/compute costs to minimize
//     total operational spend.
//   - Workforce & shift routing: assigning technicians evenly across two hubs
//     while minimizing combined travel expense.
//   - Network load balancing: splitting flows 50/50 across two peering links
//     based on per-flow latency or transit-cost differentials.
//
// Related LeetCode Problems:
//   - #134 Gas Station [done] (greedy net-delta reasoning over a sequence)
//   - #406 Queue Reconstruction by Height [done] (greedy sorting by key difference/attribute)
//   - #2611 Mice and Cheese [new] (exact same baseline + top-k delta greedy trick)
//   - #1383 Maximum Performance of a Team [new] (greedy selection with sorting on trade-off metrics)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the minimum total travel cost to send exactly half of the
  // candidates to City A and half to City B.
  // Time: O(N log N), Space: O(N).
  [[nodiscard]] int twoCitySchedCost(
      const std::vector<std::vector<int>>& costs) const {
    if (costs.empty()) {
      return 0;
    }

    int total_cost = 0;
    std::vector<int> deltas;
    deltas.reserve(costs.size());

    // Baseline: send everyone to City A and record the net cost delta
    // (b_cost - a_cost) incurred if we instead switch each candidate to City B.
    for (const auto& candidate : costs) {
      const int a_cost = candidate[0uz];
      const int b_cost = candidate[1uz];
      total_cost += a_cost;
      deltas.push_back(b_cost - a_cost);
    }

    // Sort deltas ascending so the n most favorable switches (largest savings /
    // smallest penalties) sit at the front.
    std::ranges::sort(deltas);

    const std::size_t half = costs.size() / 2uz;
    for (std::size_t i = 0uz; i < half; ++i) {
      total_cost += deltas[i];
    }

    return total_cost;
  }
};

LC_PROBLEM(1029, "Two City Scheduling", "math", ctx) {
  const Solution sol;

  ctx.test("example_1_basic", [&] {
    const std::vector<std::vector<int>> costs = {
        {10, 20}, {30, 200}, {400, 50}, {30, 20}};
    ctx.expect_eq(sol.twoCitySchedCost(costs), 110);
  });

  ctx.test("example_2_from_python_build", [&] {
    const std::vector<std::vector<int>> costs = {{259, 770}, {448, 54},
                                                 {926, 667}, {184, 139},
                                                 {840, 118}, {577, 469}};
    ctx.expect_eq(sol.twoCitySchedCost(costs), 1859);
  });

  ctx.test("example_3_eight_candidates", [&] {
    const std::vector<std::vector<int>> costs = {
        {515, 563}, {451, 713}, {537, 709}, {343, 819},
        {855, 779}, {457, 60},  {650, 359}, {631, 42}};
    ctx.expect_eq(sol.twoCitySchedCost(costs), 3086);
  });

  ctx.test("minimum_size_two_candidates", [&] {
    const std::vector<std::vector<int>> costs = {{100, 1}, {1, 100}};
    ctx.expect_eq(sol.twoCitySchedCost(costs), 2);
  });

  ctx.test("identical_costs_everywhere", [&] {
    const std::vector<std::vector<int>> costs = {
        {50, 50}, {50, 50}, {50, 50}, {50, 50}};
    ctx.expect_eq(sol.twoCitySchedCost(costs), 200);
  });
}

}  // namespace
