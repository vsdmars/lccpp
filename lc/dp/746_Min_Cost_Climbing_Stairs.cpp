// LeetCode 746: Min Cost Climbing Stairs
// https://leetcode.com/problems/min-cost-climbing-stairs/
//
// Problem:
//   Given an integer array `cost` where `cost[i]` is the cost of stepping on
//   the i-th stair, paying `cost[i]` lets you climb either 1 or 2 steps. You
//   may start at index 0 or index 1 with zero initial cost. Return the minimum
//   cost to reach the top of the floor (one position past the last stair).
//
// Algorithm:
//   Bottom-up linear dynamic programming with O(1) rolling state. Define
//   `dp[i]` as the minimum cost to arrive at stair `i` (before paying
//   `cost[i]`), with base cases `dp[0] = 0` and `dp[1] = 0`. For each position
//   `i` from 2 to `n` (where `n` is the top of the floor):
//     `dp[i] = min(dp[i - 1] + cost[i - 1], dp[i - 2] + cost[i - 2])`.
//   Because each state depends only on the previous two states, two integer
//   variables (`prev2`, `prev1`) suffice.
//   Time complexity: O(N), where N = cost.size().
//   Space complexity: O(1).
//
// Data Structures:
//   - Two scalar rolling DP variables (`prev2`, `prev1`) storing `dp[i - 2]`
//     and `dp[i - 1]` without allocating an O(N) DP table.
//
// Real-World Applications:
//   - Shortest-path / minimum-toll routing over linear multi-hop pipelines with
//     bounded skip distances.
//   - Dynamic power/checkpoint placement in streaming jobs where checkpoints
//     may skip at most one stage.
//   - Optimal text-wrapping and line-break cost minimization with bounded
//     lookback windows.
//
// Related LeetCode Problems:
//   - #70 Climbing Stairs [done] (unweighted count of 1-or-2-step paths)
//   - #198 House Robber [done] (linear DP with two-step lookback)
//   - #983 Minimum Cost For Tickets [done] (1D DP with variable jump windows)
//   - #1696 Jump Game VI [done] (sliding-window maximum over bounded step jumps)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the minimum total cost to reach the top of the staircase (index n)
  // starting from either step 0 or step 1.
  //
  // Invariant: At iteration `i`, `prev2 == dp[i - 2]` and `prev1 == dp[i - 1]`,
  // representing the minimum costs to arrive at steps `i - 2` and `i - 1`.
  // Time: O(N); Space: O(1).
  int minCostClimbingStairs(const std::vector<int>& cost) {
    const std::size_t n = cost.size();
    if (n < 2uz) {
      return 0;
    }

    // Starting at index 0 or index 1 incurs no entry cost.
    int prev2 = 0;  // dp[i - 2]
    int prev1 = 0;  // dp[i - 1]

    for (std::size_t i = 2uz; i <= n; ++i) {
      const int one_step = prev1 + cost[i - 1uz];
      const int two_steps = prev2 + cost[i - 2uz];
      const int curr = std::min(one_step, two_steps);
      prev2 = prev1;
      prev1 = curr;
    }

    return prev1;
  }
};

LC_PROBLEM(746, "Min Cost Climbing Stairs", "dp", ctx) {
  Solution sol;

  ctx.test("example_1_three_steps", [&] {
    ctx.expect_eq(sol.minCostClimbingStairs({10, 15, 20}), 15);
  });

  ctx.test("example_2_skip_expensive_steps", [&] {
    ctx.expect_eq(
        sol.minCostClimbingStairs({1, 100, 1, 1, 1, 100, 1, 1, 100, 1}), 6);
  });

  ctx.test("two_steps_minimum", [&] {
    ctx.expect_eq(sol.minCostClimbingStairs({5, 10}), 5);
    ctx.expect_eq(sol.minCostClimbingStairs({10, 5}), 5);
  });

  ctx.test("zero_and_equal_costs", [&] {
    ctx.expect_eq(sol.minCostClimbingStairs({0, 0, 0, 0}), 0);
    ctx.expect_eq(sol.minCostClimbingStairs({10, 10, 10, 10}), 20);
  });
}

}  // namespace
