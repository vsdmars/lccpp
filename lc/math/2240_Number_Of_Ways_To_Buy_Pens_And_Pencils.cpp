// LeetCode 2240: Number Of Ways To Buy Pens And Pencils
// https://leetcode.com/problems/number-of-ways-to-buy-pens-and-pencils/
//
// Problem:
//   Given an integer budget `total` and two unit prices `cost1` (pen) and
//   `cost2` (pencil), determine the number of distinct non-negative integer
//   pairs (pens, pencils) such that:
//     pens * cost1 + pencils * cost2 <= total
//   Buying zero pens and/or zero pencils is allowed. Constraints:
//   1 <= total, cost1, cost2 <= 10^6. Because the answer can reach ~5 * 10^11
//   (when total = 10^6 and cost1 = cost2 = 1), the return type must be 64-bit.
//
// Examples:
//   Example 1:
//     Input:  total = 20, cost1 = 10, cost2 = 5
//     Output: 9
//     Explanation:
//       - Keep step = 10 and other = 5 since cost1 >= cost2, with budget = 20 and
//         ways = 0.
//       - At spend = 0, remaining = 20 allows (20 / 5) + 1 = 5 pencil counts
//         (0..4), updating ways = 5.
//       - At spend = 10, remaining = 10 allows (10 / 5) + 1 = 3 pencil counts
//         (0..2), updating ways = 8.
//       - At spend = 20, remaining = 0 allows (0 / 5) + 1 = 1 pencil count (0),
//         updating ways = 9 before spend = 30 exits the loop.
//
//   Example 2:
//     Input:  total = 10, cost1 = 3, cost2 = 4
//     Output: 8
//     Explanation:
//       - Swap cost1 and cost2 since 3 < 4, giving step = 4, other = 3, budget =
//         10, and ways = 0.
//       - At spend = 0, remaining = 10 contributes (10 / 3) + 1 = 4 ways, setting
//         ways = 4.
//       - At spend = 4, remaining = 6 contributes (6 / 3) + 1 = 3 ways, setting
//         ways = 7.
//       - At spend = 8, remaining = 2 contributes (2 / 3) + 1 = 1 way, setting ways
//         = 8 before spend = 12 exits the loop.
//
//   Example 3:
//     Input:  total = 5, cost1 = 10, cost2 = 10
//     Output: 1
//     Explanation:
//       - Initialize step = 10, other = 10, budget = 5, and ways = 0.
//       - At spend = 0, remaining = 5 contributes (5 / 10) + 1 = 1 way (0 pens, 0
//         pencils), updating ways = 1.
//       - Advance spend to 10 > 5, terminating the loop and returning 1.
//
// Algorithm:
//   Fix the amount spent on one utensil and count all feasible quantities of
//   the other in O(1) arithmetic:
//     1. Swap `cost1` and `cost2` if `cost1 < cost2` so we step by the larger
//        unit price. Since the inequality is symmetric in (cost1, cost2), this
//        preserves the exact count while minimizing loop iterations.
//     2. Iterate `spend = 0, cost1, 2 * cost1, ...` while `spend <= total`.
//     3. With `remaining = total - spend`, we can purchase any integer quantity
//        of the second utensil in `[0, remaining / cost2]`, contributing
//        `(remaining / cost2) + 1` distinct combinations (the `+ 1` accounts
//        for buying 0 units).
//   Time Complexity:  O(total / max(cost1, cost2)).
//   Space Complexity: O(1).
//
// Data Structures:
//   Scalar 64-bit integer accumulators (`std::int64_t`) only; no auxiliary
//   containers are required.
//
// Real-World Applications:
//   - Cloud resource capacity planning: counting feasible combinations of two
//     VM instance classes or storage tiers within a fixed cost/memory budget.
//   - Financial portfolio & retail bundling: enumerating valid two-product
//     promotional bundles or integer lot allocations under a spending cap.
//   - Integer linear programming & knapsack feasibility counting in operations
//     research (2D lattice-point counting inside a right triangle).
//
// Related LeetCode Problems:
//   - #518 Coin Change II [done] (generalizes counting combinations to arbitrary coin sets with DP)
//   - #322 Coin Change [done] (optimizes minimum item count rather than counting feasible pairs)
//   - #279 Perfect Squares [done] (bounded integer decomposition under a target sum)
//   - #1925 Count Square Sum Triples [new] (bounded integer enumeration over an arithmetic constraint)

#include "framework/lc_framework.h"

#include <cstdint>
#include <utility>

namespace {

class Solution {
 public:
  // Counts the number of distinct non-negative pairs (pens, pencils) whose
  // combined cost does not exceed `total`.
  //
  // Invariant:
  //   At each iteration, `spend` equals `k * step` for some integer `k >= 0`
  //   with `spend <= budget`, and `ways` holds the exact count of valid pairs
  //   for all quantities `0, 1, ..., k - 1` of the pricier utensil.
  //
  // Time:  O(total / max(cost1, cost2))
  // Space: O(1)
  std::int64_t waysToBuyPensPencils(int total, int cost1, int cost2) {
    // The feasibility condition is symmetric in (cost1, cost2). Stepping by
    // the pricier utensil minimizes the number of loop iterations.
    if (cost1 < cost2) {
      std::swap(cost1, cost2);
    }

    // Use 64-bit arithmetic throughout to prevent overflow when `total` is
    // large and both unit prices are small (answer up to ~5 * 10^11).
    const auto budget = static_cast<std::int64_t>(total);
    const auto step = static_cast<std::int64_t>(cost1);
    const auto other = static_cast<std::int64_t>(cost2);

    std::int64_t ways = 0;

    // Enumerate every affordable expenditure on the first utensil (including 0).
    for (std::int64_t spend = 0; spend <= budget; spend += step) {
      const std::int64_t remaining = budget - spend;
      // Can buy 0, 1, ..., (remaining / other) units of the second utensil.
      ways += (remaining / other) + 1;
    }

    return ways;
  }
};

LC_PROBLEM(2240, "Number Of Ways To Buy Pens And Pencils", "math", ctx) {
  Solution sol;

  ctx.test("example1", [&] {
    ctx.expect_eq(sol.waysToBuyPensPencils(20, 10, 5), std::int64_t{9});
  });

  ctx.test("example1_swapped_costs", [&] {
    ctx.expect_eq(sol.waysToBuyPensPencils(20, 5, 10), std::int64_t{9});
  });

  ctx.test("example2_cannot_afford_either", [&] {
    ctx.expect_eq(sol.waysToBuyPensPencils(5, 10, 10), std::int64_t{1});
  });

  ctx.test("python_build_case", [&] {
    // Sum_{k=0}^{100000} (100000 - k + 1) = 100001 * 100002 / 2 = 5000150001.
    ctx.expect_eq(sol.waysToBuyPensPencils(100000, 1, 1),
                  std::int64_t{5000150001});
  });

  ctx.test("minimum_constraints", [&] {
    // Pairs: (0,0), (0,1), (1,0) -> 3 ways.
    ctx.expect_eq(sol.waysToBuyPensPencils(1, 1, 1), std::int64_t{3});
  });

  ctx.test("non_divisible_costs", [&] {
    // 3x + 4y <= 10: y=0 -> x in [0..3] (4), y=1 -> x in [0..2] (3),
    // y=2 -> x in [0..0] (1). Total = 8.
    ctx.expect_eq(sol.waysToBuyPensPencils(10, 3, 4), std::int64_t{8});
  });

  ctx.test("max_constraints_64bit_overflow_guard", [&] {
    // Sum_{k=0}^{10^6} (10^6 - k + 1) = 1000001 * 1000002 / 2 = 500001500001.
    ctx.expect_eq(sol.waysToBuyPensPencils(1000000, 1, 1),
                  std::int64_t{500001500001});
  });
}

}  // namespace
