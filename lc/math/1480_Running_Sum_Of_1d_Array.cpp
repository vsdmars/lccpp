// LeetCode 1480: Running Sum Of 1d Array
// https://leetcode.com/problems/running-sum-of-1d-array/
//
// Problem:
//   Given an integer array `nums`, return its running sum (inclusive prefix
//   sum) array `runningSum`, where
//     `runningSum[i] = nums[0] + nums[1] + ... + nums[i]`
//   for every `0 <= i < nums.size()`.
//   Constraints: 1 <= nums.length <= 1000, -10^6 <= nums[i] <= 10^6.
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 2, 3, 4]
//     Output: [1, 3, 6, 10]
//     Explanation:
//       - Copy nums into result = [1, 2, 3, 4], leaving result[0] = 1.
//       - At i = 1, compute result[1] += result[0] to get 2 + 1 = 3.
//       - At i = 2, compute result[2] += result[1] to get 3 + 3 = 6.
//       - At i = 3, compute result[3] += result[2] to get 4 + 6 = 10 and return [1,
//         3, 6, 10].
//
//   Example 2:
//     Input:  nums = [-2, 5, -3, 0, -4]
//     Output: [-2, 3, 0, 0, -4]
//     Explanation:
//       - Initialize result = [-2, 5, -3, 0, -4] with result[0] = -2.
//       - Update i = 1 to result[1] = 5 + (-2) = 3 and i = 2 to result[2] = -3 + 3
//         = 0.
//       - Update i = 3 to result[3] = 0 + 0 = 0 and i = 4 to result[4] = -4 + 0 =
//         -4, returning [-2, 3, 0, 0, -4].
//
//   Example 3:
//     Input:  nums = []
//     Output: []
//     Explanation:
//       - Copy the empty input vector into result = [].
//       - Because result.size() == 0, the loop starting at i = 1 executes zero
//         times and returns [].
//
// Algorithm:
//   Copy `nums` into `result` and perform a single left-to-right scan starting
//   at index `1`, applying the recurrence:
//     `result[i] = result[i - 1] + nums[i]` (stored directly as
//     `result[i] += result[i - 1]`).
//   Invariant: At the end of iteration `i`, `result[i]` equals the exact sum of
//   `nums[0..i]`. Because `|nums[i]| <= 10^6` and `nums.size() <= 1000`, the
//   cumulative sum is bounded by `+/- 10^9`, well within 32-bit signed `int`.
//   Time Complexity:  O(n), where n = nums.size().
//   Space Complexity: O(1) auxiliary space beyond the returned output vector.
//
// Data Structures:
//   - Contiguous array (`std::vector<int>`): stores the inclusive prefix sums
//     with cache-friendly sequential memory access.
//
// Real-World Applications:
//   - Financial ledgers and accounting: computing running account balances and
//     cumulative cash flows from a sequence of transactions.
//   - Probability and weighted sampling: building a cumulative distribution
//     function (CDF) table to draw weighted random samples via binary search.
//   - Parallel computing and GPU kernels: inclusive prefix-scan primitives
//     used in stream compaction, radix sort, and histogram offset tables.
//   - Analytics and time-series dashboards: rendering cumulative metrics
//     (year-to-date revenue, cumulative active users, bandwidth totals).
//
// Related LeetCode Problems:
//   - #303 Range Sum Query [done] - Immutable (uses prefix sums to answer range sum queries in O(1))
//   - #724 Find Pivot Index [done] (compares left and right prefix sums across indices)
//   - #560 Subarray Sum Equals K [done] (combines running prefix sums with a frequency hash map)
//   - #1732 Find the Highest Altitude [new] (tracks the peak value of a running sum)
//   - #238 Product of Array Except Self [done] (multiplicative prefix and suffix scan analogue)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the inclusive prefix sum vector of `nums`.
  // Time: O(n), Space: O(1) auxiliary (O(n) including the output vector).
  std::vector<int> runningSum(const std::vector<int>& nums) {
    std::vector<int> result = nums;

    // Accumulate each prefix sum from the immediately preceding prefix sum.
    for (std::size_t i = 1; i < result.size(); ++i) {
      result[i] += result[i - 1];
    }

    return result;
  }
};

LC_PROBLEM(1480, "Running Sum Of 1d Array", "math", ctx) {
  Solution sol;

  ctx.test("example_1_increasing", [&] {
    const std::vector<int> expected = {1, 3, 6, 10};
    ctx.expect_eq(sol.runningSum({1, 2, 3, 4}), expected);
  });

  ctx.test("example_2_all_ones", [&] {
    const std::vector<int> expected = {1, 2, 3, 4, 5};
    ctx.expect_eq(sol.runningSum({1, 1, 1, 1, 1}), expected);
  });

  ctx.test("example_3_mixed_values", [&] {
    const std::vector<int> expected = {3, 4, 6, 16, 17};
    ctx.expect_eq(sol.runningSum({3, 1, 2, 10, 1}), expected);
  });

  ctx.test("negative_and_zero_values", [&] {
    const std::vector<int> expected = {-2, 3, 0, 0, -4};
    ctx.expect_eq(sol.runningSum({-2, 5, -3, 0, -4}), expected);
  });

  ctx.test("single_and_empty", [&] {
    const std::vector<int> single_expected = {7};
    const std::vector<int> empty_expected = {};
    ctx.expect_eq(sol.runningSum({7}), single_expected);
    ctx.expect_eq(sol.runningSum({}), empty_expected);
  });
}

}  // namespace
