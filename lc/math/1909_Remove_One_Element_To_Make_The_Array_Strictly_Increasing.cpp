// LeetCode 1909: Remove One Element To Make The Array Strictly Increasing
// https://leetcode.com/problems/remove-one-element-to-make-the-array-strictly-increasing/
//
// Problem:
//   Given a 0-indexed integer array `nums`, return `true` if `nums` can be
//   made strictly increasing (`nums[i - 1] < nums[i]` for all valid `i`) by
//   removing at most one element (removing exactly one element is always
//   possible when the array is already strictly increasing). Return `false`
//   otherwise.
//   Constraints: 2 <= nums.length <= 1000, 1 <= nums[i] <= 1000.
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 2, 10, 5, 7]
//     Output: true
//     Explanation:
//       - Initialize removals = 0 and prev = nums[0] = 1, then iterate from i = 1.
//       - At i = 1 and i = 2, nums[1] = 2 > 1 and nums[2] = 10 > 2, advancing prev
//         to 2 and then 10.
//       - At i = 3, nums[3] = 5 <= 10 increments removals to 1; since nums[3] >
//         nums[1] (5 > 2), logically drop nums[2] = 10 and set prev = 5.
//       - At i = 4, nums[4] = 7 > 5 advances prev to 7, so the loop finishes and
//         returns true.
//
//   Example 2:
//     Input:  nums = [2, 3, 1, 2]
//     Output: false
//     Explanation:
//       - Initialize removals = 0 and prev = nums[0] = 2, then advance prev = 3 at
//         i = 1 because 3 > 2.
//       - At i = 2, nums[2] = 1 <= 3 increments removals to 1; since i > 1 and
//         nums[2] <= nums[0] (1 <= 2), discard nums[2] via continue and keep prev =
//         3.
//       - At i = 3, nums[3] = 2 <= 3 increments removals to 2 (> 1), immediately
//         returning false.
//
//   Example 3:
//     Input:  nums = [105, 924, 32, 968]
//     Output: true
//     Explanation:
//       - Initialize removals = 0 and prev = 105, then update prev = 924 at i = 1
//         since 924 > 105.
//       - At i = 2, nums[2] = 32 <= 924 increments removals to 1; because nums[2]
//         <= nums[0] (32 <= 105), discard 32 and retain prev = 924.
//       - At i = 3, nums[3] = 968 > 924 updates prev = 968, and the scan finishes
//         returning true.
//
// Algorithm:
//   Single-pass greedy scan tracking the effective previous kept value `prev`
//   and the number of order violations `removals`. Whenever `nums[i] <= prev`,
//   one removal is consumed (if a second violation occurs, return `false`
//   immediately). At the first violation, we prefer to logically remove
//   `nums[i - 1]` so the kept tail becomes the smaller value `nums[i]`, which
//   maximizes room for subsequent elements. However, if `i > 1` and
//   `nums[i] <= nums[i - 2]`, removing `nums[i - 1]` would leave a violation
//   with `nums[i - 2]`, so we must logically remove `nums[i]` instead and keep
//   `prev` unchanged (without mutating the input array).
//   Time Complexity:  O(n), where n = nums.size().
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - Two scalar state variables (`prev` and `removals`) maintained during a
//     single linear traversal of the input vector.
//
// Real-World Applications:
//   - Sensor-stream outlier tolerance (checking whether a monotonic timestamp
//     or odometer sequence has at most one corrupted reading).
//   - Data quality validation in time-series ingestion pipelines before
//     triggering full out-of-order re-sorting.
//   - Version/sequence-number sanity checks in distributed log replication.
//
// Related LeetCode Problems:
//   - #665 Non-decreasing Array [done] (modifying at most one element for non-decreasing order)
//   - #300 Longest Increasing Subsequence [done] (minimum removals equals n - LIS length)
//   - #674 Longest Continuous Increasing Subsequence [done] (linear scan over strictly increasing runs)
//   - #2970 Count the Number of Incremovable Subarrays I [new] (removing a contiguous subarray for strict increase)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Determines whether `nums` becomes strictly increasing after deleting at
  // most one element, without mutating the input array.
  // Time: O(n), Space: O(1).
  bool canBeIncreasing(const std::vector<int>& nums) {
    // Any array of length <= 2 leaves at most one element after a deletion,
    // which is vacuously strictly increasing.
    if (nums.size() <= 2uz) {
      return true;
    }

    int removals = 0;
    int prev = nums[0];

    for (std::size_t i = 1uz; i < nums.size(); ++i) {
      if (nums[i] <= prev) {
        ++removals;
        if (removals > 1) {
          return false;
        }
        // Prefer removing nums[i - 1] (shrinking the tail to nums[i]), unless
        // nums[i] also violates strict increase against nums[i - 2]; in that
        // case we must discard nums[i] and retain `prev`.
        if (i > 1uz && nums[i] <= nums[i - 2uz]) {
          continue;
        }
      }
      prev = nums[i];
    }

    return true;
  }
};

LC_PROBLEM(1909, "Remove One Element To Make The Array Strictly Increasing",
           "math", ctx) {
  ctx.test("example_1_remove_peak", [&] {
    Solution sol;
    // Removing 10 yields [1, 2, 5, 7].
    ctx.expect_true(sol.canBeIncreasing({1, 2, 10, 5, 7}));
  });

  ctx.test("example_2_build_impossible", [&] {
    Solution sol;
    ctx.expect_false(sol.canBeIncreasing({2, 3, 1, 2}));
  });

  ctx.test("example_3_all_equal", [&] {
    Solution sol;
    ctx.expect_false(sol.canBeIncreasing({1, 1, 1}));
  });

  ctx.test("remove_dip_instead_of_previous", [&] {
    Solution sol;
    // Removing 32 yields [105, 924, 968]; removing 924 would fail at 105 >= 32.
    ctx.expect_true(sol.canBeIncreasing({105, 924, 32, 968}));
  });

  ctx.test("remove_first_element", [&] {
    Solution sol;
    // Removing index 0 yields [21, 100].
    ctx.expect_true(sol.canBeIncreasing({100, 21, 100}));
  });

  ctx.test("already_strictly_increasing", [&] {
    Solution sol;
    ctx.expect_true(sol.canBeIncreasing({1, 2, 3, 4}));
  });

  ctx.test("length_two_decreasing", [&] {
    Solution sol;
    ctx.expect_true(sol.canBeIncreasing({5, 1}));
  });
}

}  // namespace
