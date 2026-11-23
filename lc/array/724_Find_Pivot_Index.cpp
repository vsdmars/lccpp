// LeetCode 724: Find Pivot Index
// https://leetcode.com/problems/find-pivot-index/
//
// Problem:
//   Given an array of integers `nums`, find the leftmost "pivot" index `i`
//   such that the sum of all elements strictly to the left of `i` equals the
//   sum of all elements strictly to the right of `i`. If `i == 0`, the left
//   sum is 0; if `i == nums.size() - 1`, the right sum is 0. Return `-1` if no
//   such index exists.
//   Constraints: 0 <= nums.size() <= 10,000; -1000 <= nums[i] <= 1000.
//
// Algorithm:
//   First compute the total sum `total` of all elements in `nums`. Then scan
//   left to right while maintaining the running prefix sum `left_sum` of
//   elements strictly before index `i`. At index `i`, the suffix sum strictly
//   after `i` is `right_sum = total - left_sum - nums[i]`. The first index
//   where `left_sum == right_sum` is guaranteed to be the leftmost pivot.
//   Using 64-bit accumulators prevents intermediate overflow even for inputs
//   beyond the nominal constraints.
//   Time Complexity:  O(n), two linear scans (total sum + prefix scan).
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - Scalar prefix/total accumulators (`std::int64_t`): replace an explicit
//     O(n) prefix-sum array with constant-space running arithmetic.
//
// Real-World Applications:
//   - Load balancing and data sharding: finding a split point in a weighted
//     sequence of tasks or keys so left and right partitions carry equal load.
//   - Financial ledger reconciliation: locating an inflection date where
//     cumulative debits before an event match net flows after it.
//   - Physics and signal processing: finding the discrete center of mass or
//     energy balance point along a 1D waveform or sensor array.
//
// Related LeetCode Problems:
//   - #1991 Find the Middle Index in Array [new] (identical equilibrium-index problem)
//   - #560 Subarray Sum Equals K [done] (prefix-sum reasoning over arrays with negatives)
//   - #238 Product of Array Except Self [done] (combining prefix and suffix aggregates)
//   - #2574 Left and Right Sum Differences [new] (comparing prefix and suffix sums at every index)
//   - #1480 Running Sum of 1d Array [done] (foundational prefix sum construction)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <numeric>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the leftmost index where sum(nums[0..i-1]) == sum(nums[i+1..n-1]),
  // or -1 if no pivot index exists.
  // Time: O(n), Space: O(1).
  int pivotIndex(const std::vector<int>& nums) {
    const std::int64_t total =
        std::accumulate(nums.begin(), nums.end(), std::int64_t{0});

    std::int64_t left_sum = 0;
    for (std::size_t idx = 0uz; idx < nums.size(); ++idx) {
      // Total splits cleanly into left_sum + nums[idx] + right_sum.
      const std::int64_t right_sum = total - left_sum - nums[idx];
      if (left_sum == right_sum) {
        return static_cast<int>(idx);
      }
      left_sum += nums[idx];
    }

    return -1;
  }
};

LC_PROBLEM(724, "Find Pivot Index", "array", ctx) {
  Solution sol;

  ctx.test("example_1_middle_pivot", [&] {
    ctx.expect_eq(sol.pivotIndex({1, 7, 3, 6, 5, 6}), 3);
  });

  ctx.test("example_2_no_pivot", [&] {
    ctx.expect_eq(sol.pivotIndex({1, 2, 3}), -1);
  });

  ctx.test("example_3_pivot_at_index_zero", [&] {
    ctx.expect_eq(sol.pivotIndex({2, 1, -1}), 0);
  });

  ctx.test("negatives_pivot_at_zero_from_python_build", [&] {
    ctx.expect_eq(sol.pivotIndex({-1, -1, -1, 0, 1, 1}), 0);
  });

  ctx.test("pivot_at_last_index", [&] {
    ctx.expect_eq(sol.pivotIndex({1, -1, 5}), 2);
  });

  ctx.test("multiple_pivots_returns_leftmost", [&] {
    ctx.expect_eq(sol.pivotIndex({0, 0, 0, 0}), 0);
  });

  ctx.test("single_element_and_empty", [&] {
    ctx.expect_eq(sol.pivotIndex({42}), 0);
    ctx.expect_eq(sol.pivotIndex({}), -1);
  });
}

}  // namespace
