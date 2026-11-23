// LeetCode 896: Monotonic Array
// https://leetcode.com/problems/monotonic-array/
//
// Problem:
//   Given an integer array `nums`, determine whether it is monotonic. An array
//   is monotonic if it is either monotone increasing (for all i <= j,
//   nums[i] <= nums[j]) or monotone decreasing (for all i <= j,
//   nums[i] >= nums[j]). Return true if `nums` is monotonic and false
//   otherwise.
//   Constraints: 1 <= nums.length <= 10^5, -10^5 <= nums[i] <= 10^5.
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 2, 2, 3]
//     Output: true
//     Explanation:
//       - Initialize `non_decreasing = true` and `non_increasing = true` before
//         scanning adjacent pairs.
//       - At `i = 1`, `nums[1] = 2 > nums[0] = 1`, so set `non_increasing = false`
//         while `non_decreasing` stays `true`.
//       - At `i = 2`, `nums[2] = 2 == nums[1] = 2`, so neither flag changes.
//       - At `i = 3`, `nums[3] = 3 > nums[2] = 2`, keeping `non_decreasing = true`,
//         and return `true` after the loop finishes.
//
//   Example 2:
//     Input:  nums = [1, 3, 2]
//     Output: false
//     Explanation:
//       - Initialize `non_decreasing = true` and `non_increasing = true`.
//       - At `i = 1`, `nums[1] = 3 > nums[0] = 1`, so set `non_increasing = false`.
//       - At `i = 2`, `nums[2] = 2 < nums[1] = 3`, so set `non_decreasing = false`.
//       - Since `!non_decreasing && !non_increasing` now holds, return `false`
//         immediately.
//
//   Example 3:
//     Input:  nums = [-10, -10, -5, 0]
//     Output: true
//     Explanation:
//       - Initialize `non_decreasing = true` and `non_increasing = true`.
//       - At `i = 1`, `nums[1] = -10 == nums[0] = -10`, so both flags remain
//         `true`.
//       - At `i = 2`, `nums[2] = -5 > nums[1] = -10`, so set `non_increasing =
//         false`.
//       - At `i = 3`, `nums[3] = 0 > nums[2] = -5`, keeping `non_decreasing =
//         true`, and return `true` after completing the pass.
//
// Algorithm:
//   Single-Pass Dual-Flag Scan (`isMonotonic`):
//   Maintain two boolean invariants, `non_decreasing` and `non_increasing`,
//   both initially true. Scan adjacent pairs `(nums[i - 1], nums[i])`:
//     - If `nums[i] > nums[i - 1]`, the array cannot be non-increasing.
//     - If `nums[i] < nums[i - 1]`, the array cannot be non-decreasing.
//   If both flags become false, exit immediately with false. Otherwise, if at
//   least one flag survives to the end of the array, the sequence is monotonic.
//   Time Complexity:  O(n) single pass with early termination.
//   Space Complexity: O(1) auxiliary space.
//
//   Sort-Comparison Alternative (`isMonotonicBySort`):
//   Mirrors the reference Python `rewrite()` approach by checking whether
//   `nums` equals either its ascending or descending sorted copy.
//   Time Complexity:  O(n log n).
//   Space Complexity: O(n).
//
// Data Structures:
//   - Two scalar boolean state flags (`non_decreasing`, `non_increasing`) for
//     the optimal O(n) pass.
//   - Temporary `std::vector<int>` copies for the O(n log n) sort-based check.
//
// Real-World Applications:
//   - Time-series telemetry validation (verifying monotonic counters, sequence
//     numbers, or hardware clock timestamps that must never move backward).
//   - Database query planners and storage engines deciding whether a column
//     chunk or index segment can use binary search or range-skipping metadata.
//   - Financial and sensor anomaly detection for non-decreasing cumulative
//     metrics such as metered billing usage or odometer readings.
//
// Related LeetCode Problems:
//   - #941 Valid Mountain Array [done] (strictly increasing then strictly decreasing pass)
//   - #665 Non-decreasing Array [done] (monotonicity with at most one modification)
//   - #1752 Check if Array Is Sorted and Rotated [new] (circular monotonicity check)
//   - #674 Longest Continuous Increasing Subsequence [done] (longest monotone run)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <vector>

namespace {

class Solution {
 public:
  // Checks whether `nums` is monotone non-decreasing or monotone
  // non-increasing in a single pass with early exit.
  // Time: O(n), Space: O(1).
  bool isMonotonic(const std::vector<int>& nums) {
    bool non_decreasing = true;
    bool non_increasing = true;

    for (std::size_t i = 1uz; i < nums.size(); ++i) {
      if (nums[i] > nums[i - 1uz]) {
        non_increasing = false;
      } else if (nums[i] < nums[i - 1uz]) {
        non_decreasing = false;
      }

      // Once both directions are violated, the array cannot be monotonic.
      if (!non_decreasing && !non_increasing) {
        return false;
      }
    }

    return true;
  }

  // Alternative approach matching the Python `rewrite()` method: verifies
  // monotonicity by comparing against ascending and descending sorted copies.
  // Time: O(n log n), Space: O(n).
  bool isMonotonicBySort(const std::vector<int>& nums) {
    std::vector<int> asc = nums;
    std::ranges::sort(asc);
    if (nums == asc) {
      return true;
    }

    std::vector<int> desc = nums;
    std::ranges::sort(desc, std::ranges::greater{});
    return nums == desc;
  }
};

LC_PROBLEM(896, "Monotonic Array", "array", ctx) {
  Solution sol;

  ctx.test("example_1_non_decreasing", [&] {
    const std::vector<int> nums = {1, 2, 2, 3};
    ctx.expect_true(sol.isMonotonic(nums));
    ctx.expect_true(sol.isMonotonicBySort(nums));
  });

  ctx.test("example_2_non_increasing", [&] {
    const std::vector<int> nums = {6, 5, 4, 4};
    ctx.expect_true(sol.isMonotonic(nums));
    ctx.expect_true(sol.isMonotonicBySort(nums));
  });

  ctx.test("example_3_non_monotonic", [&] {
    const std::vector<int> nums = {1, 3, 2};
    ctx.expect_false(sol.isMonotonic(nums));
    ctx.expect_false(sol.isMonotonicBySort(nums));
  });

  ctx.test("example_4_strictly_increasing", [&] {
    const std::vector<int> nums = {1, 2, 4, 5};
    ctx.expect_true(sol.isMonotonic(nums));
    ctx.expect_true(sol.isMonotonicBySort(nums));
  });

  ctx.test("example_5_all_equal", [&] {
    const std::vector<int> nums = {1, 1, 1, 1, 1};
    ctx.expect_true(sol.isMonotonic(nums));
    ctx.expect_true(sol.isMonotonicBySort(nums));
  });

  ctx.test("build_variants", [&] {
    ctx.expect_false(sol.isMonotonic({1, 2, 5, 4, 4}));
    ctx.expect_true(sol.isMonotonic({1, 2, 3, 4, 4}));
    ctx.expect_false(sol.isMonotonic({1, 2, 3, 2, 1}));
    ctx.expect_true(sol.isMonotonic({4, 3, 2, 1, 1}));
  });

  ctx.test("edge_cases_short_and_negative", [&] {
    ctx.expect_true(sol.isMonotonic({}));
    ctx.expect_true(sol.isMonotonic({42}));
    ctx.expect_true(sol.isMonotonic({-10, -10, -5, 0}));
    ctx.expect_false(sol.isMonotonic({-1, -5, -2}));
  });
}

}  // namespace
