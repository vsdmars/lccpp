// LeetCode 689: Maximum Sum Of 3 Non Overlapping Subarrays
// https://leetcode.com/problems/maximum-sum-of-3-non-overlapping-subarrays/
//
// Problem:
//   Given an integer array `nums` of positive integers and a window length `k`,
//   find three non-overlapping contiguous subarrays of length `k` whose total
//   sum is maximized. Return the 0-indexed starting positions of the three
//   subarrays in ascending order. If multiple answers achieve the maximum sum,
//   return the lexicographically smallest triple of indices.
//
// Algorithm:
//   Single-pass three-window sliding greedy / DP. Slide three non-overlapping
//   windows of length `k` in lockstep across `nums`:
//     - window 1 at `i1` in `[0, n - 3k]`
//     - window 2 at `i2 = i1 + k`
//     - window 3 at `i3 = i1 + 2k`
//   At each step, update each window sum in O(1) and maintain the best prefix
//   choices seen so far:
//     - `best_sum1` and `best_idx1` for the best single window ending on or
//       before `i1 + k - 1`,
//     - `best_sum2` and `best_idx2` for the best pair of non-overlapping
//       windows where the second starts at or before `i2`,
//     - `best_sum3` and `best_idx3` for the best triple of non-overlapping
//       windows where the third starts at or before `i3`.
//   Using strict `>` comparisons preserves the lexicographically smallest
//   indices on ties, and updating `best_idx2` / `best_idx3` atomically from the
//   current `best_idx1` / `best_idx2` guarantees non-overlapping indices.
//   Time Complexity: O(n), where n = nums.size().
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - Fixed-size `std::array<int, 2>` and `std::array<int, 3>` tuples to track
//     optimal starting indices without dynamic allocations per step.
//   - Scalar 64-bit integer accumulators for the three sliding window sums and
//     running DP maxima.
//
// Real-World Applications:
//   - Time-series event detection: selecting k-minute non-overlapping peak
//     traffic or revenue bursts in telemetry streams.
//   - Financial trading and backtesting: identifying a fixed number of
//     non-overlapping fixed-horizon investment windows with maximum return.
//   - Ad-slot and broadcast scheduling: placing fixed-duration commercial
//     breaks to maximize cumulative audience engagement without overlap.
//
// Related LeetCode Problems:
//   - #53 Maximum Subarray [done] (classic single maximum-sum contiguous subarray)
//   - #123 Best Time to Buy and Sell Stock III [new] (at most two non-overlapping profit intervals)
//   - #1031 Maximum Sum of Two Non-Overlapping Subarrays [new] (two non-overlapping windows of lengths L and M)
//   - #1477 Find Two Non-overlapping Sub-arrays Each With Target Sum [new] (prefix-best tracking for non-overlapping subarrays)
//   - #209 Minimum Size Subarray Sum [done] (sliding window subarray sum optimization)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the lexicographically smallest starting indices of three
  // non-overlapping subarrays of length `k` with maximum combined sum.
  // Time: O(n), Space: O(1).
  std::vector<int> maxSumOfThreeSubarrays(const std::vector<int>& nums, int k) {
    if (k <= 0) {
      return {};
    }
    const auto window = static_cast<std::size_t>(k);
    if (nums.size() < (3uz * window)) {
      return {};
    }

    // Compute the initial sums for the three adjacent windows:
    // [0, k), [k, 2k), and [2k, 3k).
    std::int64_t sum1 = 0;
    std::int64_t sum2 = 0;
    std::int64_t sum3 = 0;
    for (std::size_t offset = 0uz; offset < window; ++offset) {
      sum1 += nums[offset];
      sum2 += nums[window + offset];
      sum3 += nums[(2uz * window) + offset];
    }

    // Running optimal sums and decoupled starting-index snapshots for 1, 2,
    // and 3 non-overlapping windows.
    std::int64_t best_sum1 = sum1;
    std::int64_t best_sum2 = best_sum1 + sum2;
    std::int64_t best_sum3 = best_sum2 + sum3;

    int best_idx1 = 0;
    std::array<int, 2> best_idx2 = {0, k};
    std::array<int, 3> best_idx3 = {0, k, 2 * k};

    // Slide all three windows right by one position per iteration.
    // Loop invariant: at the start of iteration (i1, i2, i3),
    //   - `best_idx1 <= i1 - 1` achieves `best_sum1`,
    //   - `best_idx2` has two non-overlapping windows with second start <= i2 - 1,
    //   - `best_idx3` has three non-overlapping windows with third start <= i3 - 1.
    std::size_t idx1 = 1uz;
    std::size_t idx2 = window + 1uz;
    std::size_t idx3 = (2uz * window) + 1uz;
    const std::size_t last_idx3 = nums.size() - window;

    while (idx3 <= last_idx3) {
      // Advance window 1 and update best single-window choice (strict `>`
      // preserves the lexicographically smallest index on ties).
      sum1 = sum1 - nums[idx1 - 1uz] + nums[(idx1 + window) - 1uz];
      if (sum1 > best_sum1) {
        best_sum1 = sum1;
        best_idx1 = static_cast<int>(idx1);
      }

      // Advance window 2. Pairing `best_idx1` (at most `idx1`) with `idx2`
      // (`idx1 + window`) guarantees non-overlap.
      sum2 = sum2 - nums[idx2 - 1uz] + nums[(idx2 + window) - 1uz];
      if (best_sum1 + sum2 > best_sum2) {
        best_sum2 = best_sum1 + sum2;
        best_idx2 = {best_idx1, static_cast<int>(idx2)};
      }

      // Advance window 3. Pairing `best_idx2` (second window at most `idx2`)
      // with `idx3` (`idx2 + window`) guarantees all three are non-overlapping.
      sum3 = sum3 - nums[idx3 - 1uz] + nums[(idx3 + window) - 1uz];
      if (best_sum2 + sum3 > best_sum3) {
        best_sum3 = best_sum2 + sum3;
        best_idx3 = {best_idx2[0], best_idx2[1], static_cast<int>(idx3)};
      }

      ++idx1;
      ++idx2;
      ++idx3;
    }

    return {best_idx3[0], best_idx3[1], best_idx3[2]};
  }
};

LC_PROBLEM(689, "Maximum Sum Of 3 Non Overlapping Subarrays", "sliding_window",
           ctx) {
  ctx.test("leetcode example 1", [&] {
    Solution sol;
    ctx.expect_eq(sol.maxSumOfThreeSubarrays({1, 2, 1, 2, 6, 7, 5, 1}, 2),
                  std::vector<int>{0, 3, 5});
  });

  ctx.test("python build second case", [&] {
    Solution sol;
    ctx.expect_eq(
        sol.maxSumOfThreeSubarrays({7, 13, 20, 19, 19, 2, 10, 1, 1, 19}, 3),
        std::vector<int>{1, 4, 7});
  });

  ctx.test("lexicographical tie breaking", [&] {
    Solution sol;
    ctx.expect_eq(sol.maxSumOfThreeSubarrays({1, 2, 1, 2, 1, 2, 1, 2, 1}, 2),
                  std::vector<int>{0, 2, 4});
  });

  ctx.test("exact minimum array length 3k", [&] {
    Solution sol;
    ctx.expect_eq(sol.maxSumOfThreeSubarrays({1, 2, 3, 4, 5, 6}, 2),
                  std::vector<int>{0, 2, 4});
  });

  ctx.test("window size k equals 1", [&] {
    Solution sol;
    ctx.expect_eq(
        sol.maxSumOfThreeSubarrays({4, 5, 10, 6, 11, 17, 4, 11, 1, 3}, 1),
        std::vector<int>{4, 5, 7});
  });
}

}  // namespace
