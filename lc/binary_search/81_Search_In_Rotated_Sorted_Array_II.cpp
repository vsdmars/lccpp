// LeetCode 81: Search In Rotated Sorted Array II
// https://leetcode.com/problems/search-in-rotated-sorted-array-ii/
//
// Problem:
//   Given an integer array `nums` sorted in non-decreasing order (duplicates
//   allowed) that has been rotated at an unknown pivot index, and an integer
//   `target`, return `true` if `target` is in `nums`, or `false` otherwise.
//
// Examples:
//   Example 1:
//     Input:  nums = [2, 5, 6, 0, 0, 1, 2], target = 3
//     Output: false
//     Explanation:
//       - Initialize the half-open search range left = 0 and right = 7.
//       - At mid = 3, nums[3] = 0 != 3 and nums[0] = 2 > nums[3] = 0 gives
//         mid_in_left = false while target_in_left = (3 >= 2) = true, so set right
//         = 3.
//       - At mid = 1, nums[1] = 5 != 3 and nums[0] = 2 < nums[1] = 5 gives
//         mid_in_left = true while target_in_left = true; since 3 < 5, set right =
//         1.
//       - At mid = 0, nums[0] = 2 != 3 and nums[left] == nums[mid] triggers ++left
//         to 1, ending the loop and returning false.
//
//   Example 2:
//     Input:  nums = [1, 3, 1, 1], target = 3
//     Output: true
//     Explanation:
//       - Initialize left = 0 and right = 4.
//       - At mid = 2, nums[2] = 1 != 3 and nums[0] == nums[2] == 1 cannot identify
//         the sorted half, so increment left to 1.
//       - At mid = 2 with left = 1 and right = 4, nums[1] = 3 > nums[2] = 1 sets
//         mid_in_left = false while target_in_left = (3 >= 3) = true, so set right
//         = 2.
//       - At mid = 1 with left = 1 and right = 2, nums[1] = 3 == target, returning
//         true.
//
//   Example 3:
//     Input:  nums = [5, 5, 5, 5, 5, 5, 1, 2, 3, 5], target = 2
//     Output: true
//     Explanation:
//       - Initialize left = 0 and right = 10.
//       - On the first two iterations (mid = 5, then mid = 5 again), nums[left] ==
//         nums[mid] == 5 != 2 advances left from 0 to 2.
//       - At left = 2, right = 10, mid = 6 has nums[6] = 1 < nums[2] = 5
//         (mid_in_left = false) and target_in_left = (2 >= 5) = false; since 2 > 1,
//         set left = 7.
//       - At left = 7, right = 10, mid = 8 has nums[8] = 3 > 2 on the sorted [7,
//         10) segment so right = 8; then mid = 7 finds nums[7] = 2 == target and
//         returns true.
//
// Algorithm:
//   Extend LeetCode #33's half-open binary search over `[left, right)` to
//   handle duplicate values. At each step:
//   1. If `nums[mid] == target`, return `true`.
//   2. If `nums[left] == nums[mid]`, duplicate boundary values make it
//      impossible to know which side of `mid` contains the rotation pivot
//      (for example, `[1, 1, 1, 3, 1]` vs. `[1, 3, 1, 1, 1]`). Because we
//      already checked `nums[mid] != target`, `nums[left]` is also guaranteed
//      not to equal `target`, so increment `left` by 1 to safely shrink the
//      search space and retry.
//   3. Otherwise `nums[left] != nums[mid]`, so `nums[left] < nums[mid]`
//      guarantees `[left, mid]` is sorted (no pivot inside), while
//      `nums[left] > nums[mid]` guarantees `[mid, right)` is sorted. Compare
//      `mid_in_left` vs. `target_in_left` just as in #33 to discard half the
//      remaining range.
//   Time Complexity: O(log n) average; O(n) worst case when all or nearly all
//   elements are equal duplicates.
//   Space Complexity: O(1).
//
// Data Structures:
//   Two `std::size_t` search bounds (`left`, `right`) over a contiguous
//   `std::vector<int>`.
//
// Real-World Applications:
//   Searching circular telemetry or event logs where multiple events share the
//   exact same timestamp (coarse clock granularity), and degraded-to-linear
//   safe fallback binary search in rotated partitioned datasets.
//
// Related LeetCode Problems:
//   - #33 Search In Rotated Sorted Array [done] (distinct elements, guaranteed O(log n))
//   - #153 Find Minimum In Rotated Sorted Array [done] (pivot minimum without duplicates)
//   - #154 Find Minimum In Rotated Sorted Array II [done] (pivot minimum with duplicates)
//   - #34 Find First And Last Position Of Element In Sorted Array [done] (binary search with duplicates)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Determines whether `target` exists in a rotated non-decreasing array that
  // may contain duplicate values.
  //
  // Invariant: If `target` exists in `nums`, at least one copy of `target`
  // remains within the half-open interval `[left, right)`.
  //
  // Time: O(log n) average, O(n) worst case; Space: O(1).
  bool search(const std::vector<int>& nums, int target) {
    std::size_t left = 0uz;
    std::size_t right = nums.size();

    while (left < right) {
      const std::size_t mid = left + ((right - left) / 2);
      if (nums[mid] == target) {
        return true;
      }

      // When `nums[left] == nums[mid]`, duplicates obscure which half holds
      // the rotation drop. Since `nums[mid] != target`, `nums[left]` is not
      // `target` either, so advancing `left` preserves the invariant.
      if (nums[left] == nums[mid]) {
        ++left;
        continue;
      }

      // Now `nums[left] != nums[mid]`, so `nums[left] < nums[mid]` proves `mid`
      // sits on the left sorted segment, whereas `nums[left] > nums[mid]`
      // proves `mid` sits on the right sorted segment.
      const bool mid_in_left = nums[left] < nums[mid];
      const bool target_in_left = target >= nums[left];

      if (mid_in_left != target_in_left) {
        if (mid_in_left) {
          left = mid + 1;
        } else {
          right = mid;
        }
      } else if (target < nums[mid]) {
        right = mid;
      } else {
        left = mid + 1;
      }
    }

    return false;
  }
};

LC_PROBLEM(81, "Search In Rotated Sorted Array II", "binary_search", ctx) {
  Solution sol;

  ctx.test("leetcode_examples", [&] {
    const std::vector<int> nums = {2, 5, 6, 0, 0, 1, 2};
    ctx.expect_true(sol.search(nums, 0));
    ctx.expect_false(sol.search(nums, 3));
  });

  ctx.test("duplicate_ambiguous_pivots", [&] {
    ctx.expect_true(sol.search({1, 1, 1, 1, 1, 3, 1, 1, 1, 1}, 3));
    ctx.expect_false(sol.search({1, 1, 1, 1, 1, 3, 1, 1, 1, 1}, 2));
    ctx.expect_true(sol.search({1, 3, 1, 1}, 3));
    ctx.expect_true(sol.search({5, 5, 5, 5, 5, 5, 1, 2, 3, 5}, 2));
    ctx.expect_true(sol.search({2, 2, 0, 1, 2}, 2));
    ctx.expect_true(sol.search({2, 0, 1, 2, 2}, 2));
  });

  ctx.test("edge_cases_small_and_uniform", [&] {
    ctx.expect_false(sol.search({}, 1));
    ctx.expect_true(sol.search({1}, 1));
    ctx.expect_false(sol.search({1}, 0));
    ctx.expect_true(sol.search({3, 1}, 3));
    ctx.expect_true(sol.search({3, 1}, 1));
    ctx.expect_false(sol.search({3, 1}, 0));
    ctx.expect_true(sol.search({1, 3}, 1));
    ctx.expect_true(sol.search({4, 4, 4, 4}, 4));
    ctx.expect_false(sol.search({4, 4, 4, 4}, 5));
  });
}

}  // namespace
