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
