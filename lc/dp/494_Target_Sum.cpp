// LeetCode 494: Target Sum
// https://leetcode.com/problems/target-sum/
//
// Problem:
//   Given an integer array `nums` of non-negative integers and an integer
//   `target`, build an expression by placing either `'+'` or `'-'` before each
//   integer in `nums` and concatenating all integers. Return the number of
//   distinct sign assignments whose evaluated sum equals `target`.
//   Constraints: 1 <= nums.size() <= 20, 0 <= nums[i] <= 1000,
//   0 <= sum(nums) <= 1000, -1000 <= target <= 1000.
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 1, 1, 1, 1], target = 3
//     Output: 5
//     Explanation:
//       - Compute total_sum = 5 (abs(3) <= 5) and allocate memo[5][11] with offset
//         = 5 initialized to -1.
//       - In dfs(idx, running_sum), branch on +1 and -1 at each index until idx ==
//         5, returning 1 when running_sum == 3 and 0 otherwise.
//       - Memoizing (idx, running_sum + 5) reuses shared subproblems such as (2,
//         2), (3, 1), and (4, 2) across the 4-plus/1-minus paths.
//       - Accumulating results back to dfs(0, 0) returns 5 (matching
//         findTargetSumWaysDp with subset_target = (5 + 3) / 2 = 4).
//
//   Example 2:
//     Input:  nums = [0, 0, 1], target = 1
//     Output: 4
//     Explanation:
//       - Compute total_sum = 1 (offset = 1, sum_span = 3) and begin top-down
//         search at dfs(0, 0).
//       - At idx = 2 (nums[2] = 1) with running_sum = 0, only the +1 branch reaches
//         target = 1, storing memo[2][1] = 1.
//       - At idx = 1 and idx = 0 (both 0), +0 and -0 both transition to running_sum
//         = 0, doubling counts to memo[1][1] = 2 and memo[0][1] = 4.
//       - Return 4 (equivalently in findTargetSumWaysDp, subset_target = 1, the two
//         0s double dp[0] to 4, and num = 1 sets dp[1] = 4).
//
//   Example 3:
//     Input:  nums = [1, 1, 1, 1, 1], target = 2
//     Output: 0
//     Explanation:
//       - Compute total_sum = 5 and pass the magnitude check abs(2) <= 5.
//       - In findTargetSumWays, five +/-1 steps always yield an odd final sum in
//         {-5, -3, -1, 1, 3, 5}, so no leaf matches 2 and dfs(0, 0) returns 0.
//       - In findTargetSumWaysDp, (total_sum + target) % 2 = 7 % 2 = 1 fails the
//         parity check and returns 0 immediately.
//
// Algorithm:
//   1. Top-Down Memoized DFS (`findTargetSumWays`):
//      Explore assigning `+nums[idx]` or `-nums[idx]` at each index while
//      caching `(idx, current_sum)` in a 2D table offset by `total_sum` so
//      negative running sums map cleanly to `[0, 2 * total_sum]`.
//      Time Complexity:  O(n * S), where S = sum(nums).
//      Space Complexity: O(n * S) for the memo table plus O(n) recursion stack.
//   2. Bottom-Up Subset-Sum DP (`findTargetSumWaysDp`):
//      Let `P` be the sum of elements given `'+'` and `N` the sum given `'-'`.
//      Since `P + N = S` and `P - N = target`, we need `2 * P = S + target`,
//      so no solution exists if `|target| > S` or `(S + target)` is odd.
//      Otherwise count subsets summing to `P = (S + target) / 2` with a 1D
//      0/1 knapsack DP traversed backwards per item (which naturally doubles
//      counts for zero-valued elements because `+0` and `-0` are distinct).
//      Time Complexity:  O(n * P) <= O(n * S).
//      Space Complexity: O(P).
//
// Data Structures:
//   - 2D offset memoization table (`std::vector<std::vector<int>>`): caches
//     subproblem results for `(index, shifted_sum)` in the top-down DFS.
//   - 1D subset-sum DP array (`std::vector<int>`): tracks the number of
//     subsets achieving each sum `0..P` in the bottom-up reduction.
//
// Real-World Applications:
//   - Financial ledger reconciliation finding credit/debit sign combinations
//     that explain an observed net balance discrepancy.
//   - Combinatorial circuit / signal polarity assignment to meet a target
//     aggregate voltage or phase offset.
//   - Cryptographic subset-sum counting and balanced load partitioning across
//     two opposing capacity channels.
//
// Related LeetCode Problems:
//   - #416 Partition Equal Subset Sum [done] (subset-sum feasibility reduction)
//   - #474 Ones And Zeroes [done] (2D 0/1 knapsack DP)
//   - #518 Coin Change 2 [done] (unbounded knapsack counting combinations)
//   - #282 Expression Add Operators [done] (DFS expression building to hit a target)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Counts sign assignments summing to `target` via top-down memoized DFS.
  // Time: O(n * S), Space: O(n * S), where S = sum(nums).
  int findTargetSumWays(const std::vector<int>& nums, int target) {
    std::int64_t total_sum = 0;
    for (const int num : nums) {
      total_sum += num;
    }

    const auto target64 = static_cast<std::int64_t>(target);
    const std::int64_t abs_target = (target64 < 0) ? -target64 : target64;
    if (abs_target > total_sum) {
      return 0;
    }

    const auto n = nums.size();
    const auto offset = static_cast<std::size_t>(total_sum);
    const std::size_t sum_span = (2uz * offset) + 1uz;

    // `memo[idx][sum + offset]` stores the number of ways from `idx` onward
    // given running sum `sum`; `-1` marks uncomputed states.
    std::vector<std::vector<int>> memo(n, std::vector<int>(sum_span, -1));

    auto dfs = [&](this auto&& self, std::size_t idx,
                   std::int64_t running_sum) -> int {
      if (idx == n) {
        return (running_sum == target64) ? 1 : 0;
      }

      const auto sum_col = static_cast<std::size_t>(running_sum + total_sum);
      if (memo[idx][sum_col] != -1) {
        return memo[idx][sum_col];
      }

      // Branch on assigning '+' or '-' to `nums[idx]`.
      const int plus_ways = self(idx + 1uz, running_sum + nums[idx]);
      const int minus_ways = self(idx + 1uz, running_sum - nums[idx]);
      memo[idx][sum_col] = plus_ways + minus_ways;
      return memo[idx][sum_col];
    };

    return dfs(0uz, 0);
  }

  // Space-optimized bottom-up reduction to subset-sum counting:
  // P = (total_sum + target) / 2.
  // Time: O(n * P), Space: O(P).
  int findTargetSumWaysDp(const std::vector<int>& nums, int target) {
    std::int64_t total_sum = 0;
    for (const int num : nums) {
      total_sum += num;
    }

    const auto target64 = static_cast<std::int64_t>(target);
    const std::int64_t abs_target = (target64 < 0) ? -target64 : target64;
    if (abs_target > total_sum || ((total_sum + target64) % 2) != 0) {
      return 0;
    }

    const auto subset_target =
        static_cast<std::size_t>((total_sum + target64) / 2);
    std::vector<int> dp(subset_target + 1uz, 0);
    // Empty subset uniquely achieves sum 0.
    dp[0uz] = 1;

    for (const int num : nums) {
      const auto val = static_cast<std::size_t>(num);
      // Reverse iteration ensures each element is considered at most once;
      // when `val == 0`, `dp[s] += dp[s]` cleanly doubles ways (+0 vs -0).
      for (std::size_t si = subset_target + 1uz; si > val; --si) {
        const std::size_t s = si - 1uz;
        dp[s] += dp[s - val];
      }
    }

    return dp[subset_target];
  }
};

LC_PROBLEM(494, "Target Sum", "dp", ctx) {
  ctx.test("example_1_five_ones", [&] {
    Solution sol;
    const std::vector<int> nums = {1, 1, 1, 1, 1};
    ctx.expect_eq(sol.findTargetSumWays(nums, 3), 5);
    ctx.expect_eq(sol.findTargetSumWaysDp(nums, 3), 5);
  });

  ctx.test("single_element", [&] {
    Solution sol;
    const std::vector<int> nums = {1};
    ctx.expect_eq(sol.findTargetSumWays(nums, 1), 1);
    ctx.expect_eq(sol.findTargetSumWaysDp(nums, 1), 1);
    ctx.expect_eq(sol.findTargetSumWays(nums, -1), 1);
    ctx.expect_eq(sol.findTargetSumWaysDp(nums, -1), 1);
  });

  ctx.test("all_zeros_doubles_each_step", [&] {
    Solution sol;
    const std::vector<int> nums = {0, 0, 0, 0, 0};
    // Each 0 can take '+' or '-', producing 2^5 = 32 distinct assignments.
    ctx.expect_eq(sol.findTargetSumWays(nums, 0), 32);
    ctx.expect_eq(sol.findTargetSumWaysDp(nums, 0), 32);
  });

  ctx.test("negative_target_and_unreachable", [&] {
    Solution sol;
    const std::vector<int> nums = {1, 1, 1, 1, 1};
    ctx.expect_eq(sol.findTargetSumWays(nums, -3), 5);
    ctx.expect_eq(sol.findTargetSumWaysDp(nums, -3), 5);
    ctx.expect_eq(sol.findTargetSumWays(nums, 7), 0);
    ctx.expect_eq(sol.findTargetSumWaysDp(nums, 7), 0);
    ctx.expect_eq(sol.findTargetSumWays(nums, 2), 0);
    ctx.expect_eq(sol.findTargetSumWaysDp(nums, 2), 0);
  });

  ctx.test("mixed_values_with_zeros", [&] {
    Solution sol;
    const std::vector<int> nums = {0, 0, 1};
    ctx.expect_eq(sol.findTargetSumWays(nums, 1), 4);
    ctx.expect_eq(sol.findTargetSumWaysDp(nums, 1), 4);
  });
}

}  // namespace
