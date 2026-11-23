// LeetCode 377: Combination Sum IV
// https://leetcode.com/problems/combination-sum-iv/
//
// Problem:
//   Given an array of distinct positive integers `nums` and a target integer
//   `target`, return the number of possible ordered combinations (sequences)
//   that add up to `target`. Elements from `nums` may be reused unlimited
//   times, and different orderings are counted as distinct combinations (the
//   final answer is guaranteed to fit in a 32-bit signed integer).
//
// Algorithm:
//   Bottom-Up 1D Dynamic Programming (Ordered Unbounded Knapsack). Define
//   `dp[t]` as the number of ordered sequences summing to `t`, with base case
//   `dp[0] = 1` (the empty sequence). Because order matters, the outer loop
//   iterates over target sums `t = 1..target` and the inner loop iterates over
//   candidate last elements `num` in `nums`:
//     dp[t] = sum_{num in nums, num <= t} dp[t - num]
//   Sorting a local copy of `nums` allows breaking out of the inner loop early
//   once `num > t`. To guard against 32-bit/64-bit signed overflow in
//   unreachable intermediate states (e.g., `nums = [3, 33, 333], target = 10000`
//   where `dp[9999]` overflows even 64-bit integers while `dp[10000] = 0`),
//   intermediate sums are accumulated in `std::uint64_t` and saturated at
//   `INT_MAX + 1` (no state exceeding `INT_MAX` can ever contribute to a
//   valid answer that fits in `int`).
//   Time Complexity:  O(target * |nums| + |nums| log |nums|).
//   Space Complexity: O(target + |nums|).
//
// Data Structures:
//   - `std::vector<int> sorted_nums`: sorted copy of `nums` enabling early
//     inner-loop termination.
//   - `std::vector<std::uint64_t> dp`: 1D table of length `target + 1` storing
//     saturated sequence counts for sums `0..target`.
//
// Real-World Applications:
//   - Counting ordered packet/instruction sequences of fixed total latency or
//     byte length in protocol verification and code-generation analyzers.
//   - Computing ordered stair-climbing / step-composition counts in queueing
//     and discrete-time Markov chain path enumeration.
//   - Counting rhythmic/melodic measure compositions from fixed note durations
//     in algorithmic music and scheduling engines.
//
// Related LeetCode Problems:
//   - #518 Coin Change II [done] (swaps loop order to count unordered combinations)
//   - #322 Coin Change [done] (unbounded knapsack minimizing coin count)
//   - #39 Combination Sum [done] (backtracking to enumerate actual unordered combinations)
//   - #70 Climbing Stairs [done] (special case with `nums = [1, 2]`)
//   - #1155 Number of Dice Rolls With Target Sum [new] (fixed-length ordered sum counting DP)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

namespace {

class Solution {
 public:
  // Counts the number of ordered sequences from `nums` that sum to `target`.
  // Invariant: after outer iteration `t`, `dp[t]` equals the exact count of
  // ordered sequences summing to `t` (saturated at `kCap` if unreachable and
  // larger than `INT_MAX`).
  // Time: O(target * nums.size() + nums.size() log nums.size()),
  // Space: O(target + nums.size()).
  int combinationSum4(const std::vector<int>& nums, int target) {
    if (target < 0) {
      return 0;
    }

    // Sort a local copy ascending so the inner loop can exit early as soon as
    // `num > t`.
    std::vector<int> sorted_nums = nums;
    std::ranges::sort(sorted_nums);

    const auto u_target = static_cast<std::size_t>(target);
    // Cap unreachable intermediate counts at INT_MAX + 1 to prevent signed or
    // unsigned overflow while remaining exact whenever `dp[target] <= INT_MAX`.
    constexpr std::uint64_t kCap =
        static_cast<std::uint64_t>(std::numeric_limits<int>::max()) + 1ULL;

    std::vector<std::uint64_t> dp(u_target + 1uz, 0ULL);
    dp[0uz] = 1ULL;

    for (std::size_t t = 1uz; t <= u_target; ++t) {
      for (const int num : sorted_nums) {
        if (num <= 0) {
          continue;
        }
        const auto u_num = static_cast<std::size_t>(num);
        if (u_num > t) {
          break;
        }
        // Saturating addition avoids overflow in intermediate states that
        // never feed into `dp[target]`.
        const std::uint64_t add = dp[t - u_num];
        dp[t] = (dp[t] >= kCap - add) ? kCap : (dp[t] + add);
      }
    }

    return static_cast<int>(dp[u_target]);
  }
};

LC_PROBLEM(377, "Combination Sum IV", "dp", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    ctx.expect_eq(sol.combinationSum4({1, 2, 3}, 4), 7);
    // Unsorted input yields identical count.
    ctx.expect_eq(sol.combinationSum4({3, 2, 1}, 4), 7);
  });

  ctx.test("example_2_unreachable", [&] {
    Solution sol;
    ctx.expect_eq(sol.combinationSum4({9}, 3), 0);
  });

  ctx.test("zero_target_and_empty", [&] {
    Solution sol;
    ctx.expect_eq(sol.combinationSum4({1, 2, 3}, 0), 1);
    ctx.expect_eq(sol.combinationSum4({}, 5), 0);
  });

  ctx.test("even_nums_odd_target", [&] {
    Solution sol;
    ctx.expect_eq(sol.combinationSum4({2, 4, 6}, 7), 0);
  });

  ctx.test("unreachable_intermediate_overflow_guard", [&] {
    Solution sol;
    // dp[9999] is astronomical, yet dp[10000] == 0 because 3 does not divide
    // 10000; saturating accumulation handles this cleanly without UB.
    ctx.expect_eq(sol.combinationSum4({3, 33, 333}, 10000), 0);
  });
}

}  // namespace
