// LeetCode 33: Search In Rotated Sorted Array
// https://leetcode.com/problems/search-in-rotated-sorted-array/
//
// Problem:
//   An ascending sorted integer array `nums` with distinct values is possibly
//   rotated at an unknown pivot index (for example, `[0,1,2,4,5,6,7]` rotated
//   to `[4,5,6,7,0,1,2]`). Given a `target` integer, return its index in
//   `nums` if present, or `-1` otherwise, in O(log n) time.
//
// Examples:
//   Example 1:
//     Input:  nums = [4, 5, 6, 7, 0, 1, 2], target = 0
//     Output: 4
//     Explanation:
//       - Start on [left, right) = [0, 7) with mid = 3 (nums[3] = 7 != 0).
//       - Since mid_in_left is true (4 <= 7) and target_in_left is false (0 < 4),
//         discard the left half by setting left = 4.
//       - On [4, 7), compute mid = 5 (nums[5] = 1 != 0); both mid_in_left (0 <= 1)
//         and target_in_left (0 >= 0) are true with 0 < 1, so set right = 5.
//       - On [4, 5), compute mid = 4, match nums[4] == 0, and return 4.
//
//   Example 2:
//     Input:  nums = [4, 5, 6, 7, 0, 1, 2], target = 3
//     Output: -1
//     Explanation:
//       - Start on [left, right) = [0, 7) with mid = 3 (nums[3] = 7); mid_in_left
//         is true (4 <= 7) and target_in_left is false (3 < 4), so set left = 4.
//       - On [4, 7), compute mid = 5 (nums[5] = 1); both mid_in_left (0 <= 1) and
//         target_in_left (3 >= 0) are true with 3 > 1, so set left = 6.
//       - On [6, 7), compute mid = 6 (nums[6] = 2); both sides match (2 <= 2 and 3
//         >= 2) with 3 > 2, so set left = 7.
//       - Exit the loop because left == right == 7 and return -1.
//
//   Example 3:
//     Input:  nums = [1, 2, 3, 4, 5], target = 5
//     Output: 4
//     Explanation:
//       - Start on [left, right) = [0, 5) with mid = 2 (nums[2] = 3 != 5).
//       - Both mid_in_left (1 <= 3) and target_in_left (5 >= 1) are true with
//         target > nums[2], so set left = 3.
//       - On [3, 5), compute mid = 4, find nums[4] == 5, and return 4.
//
// Algorithm:
//   Use half-open binary search over `[left, right)`. Because all values are
//   distinct, comparing `nums[mid]` and `target` against `nums[left]` tells us
//   which side of the rotation pivot each one occupies within the active
//   subarray:
//   - `mid_in_left = (nums[left] <= nums[mid])`: `mid` lies in the left
//     (higher-valued) sorted segment.
//   - `target_in_left = (target >= nums[left])`: if `target` exists in the
//     active range, it must lie in the left sorted segment.
//   If `mid` and `target` lie on opposite sides of the pivot, move straight
//   toward `target`'s side (`left = mid + 1` if `mid` is on the left, or
//   `right = mid` if `mid` is on the right). If they lie on the same monotonic
//   segment, perform standard binary search comparing `target` to `nums[mid]`.
//   Time Complexity: O(log n).
//   Space Complexity: O(1).
//
// Data Structures:
//   Two `std::size_t` boundary indices (`left`, `right`) over a contiguous
//   `std::vector<int>`.
//
// Real-World Applications:
//   Searching timestamped records in ring/circular log buffers without
//   re-indexing, querying wrap-around sequence numbers in network protocols,
//   and binary-searching cyclic time-series partitions.
//
// Related LeetCode Problems:
//   - #81 Search In Rotated Sorted Array II [done] (allows duplicate values, O(n) worst case)
//   - #153 Find Minimum In Rotated Sorted Array [done] (locates the pivot minimum in O(log n))
//   - #154 Find Minimum In Rotated Sorted Array II [done] (pivot minimum with duplicates)
//   - #35 Search Insert Position [done] (standard lower-bound binary search)
//   - #1095 Find in Mountain Array [new] (binary search on piecewise-monotonic arrays)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Searches for `target` in a rotated ascending array of distinct integers.
  //
  // Invariant: If `target` exists in `nums`, its index lies inside the
  // half-open interval `[left, right)`. Each iteration shrinks the interval by
  // at least half.
  //
  // Time: O(log n), Space: O(1).
  int search(const std::vector<int>& nums, int target) {
    std::size_t left = 0uz;
    std::size_t right = nums.size();

    while (left < right) {
      // Overflow-safe midpoint inside `[left, right)`.
      const std::size_t mid = left + ((right - left) / 2);
      if (nums[mid] == target) {
        return static_cast<int>(mid);
      }

      // Classify whether `mid` and `target` belong to the left (unwrapped)
      // sorted segment relative to `nums[left]`.
      const bool mid_in_left = nums[left] <= nums[mid];
      const bool target_in_left = target >= nums[left];

      if (mid_in_left != target_in_left) {
        // `mid` and `target` are on opposite sides of the pivot; discard `mid`'s
        // side completely.
        if (mid_in_left) {
          left = mid + 1;
        } else {
          right = mid;
        }
      } else if (target < nums[mid]) {
        // Both lie on the same strictly increasing segment, with `target` left.
        right = mid;
      } else {
        // Both lie on the same strictly increasing segment, with `target` right.
        left = mid + 1;
      }
    }

    return -1;
  }
};

LC_PROBLEM(33, "Search In Rotated Sorted Array", "binary_search", ctx) {
  Solution sol;

  ctx.test("example_rotated_present_and_absent", [&] {
    const std::vector<int> nums = {4, 5, 6, 7, 0, 1, 2};
    ctx.expect_eq(sol.search(nums, 5), 1);
    ctx.expect_eq(sol.search(nums, 0), 4);
    ctx.expect_eq(sol.search(nums, 4), 0);
    ctx.expect_eq(sol.search(nums, 2), 6);
    ctx.expect_eq(sol.search(nums, 3), -1);
    ctx.expect_eq(sol.search(nums, -5), -1);
    ctx.expect_eq(sol.search(nums, 10), -1);
  });

  ctx.test("unrotated_sorted_array", [&] {
    const std::vector<int> nums = {1, 2, 3, 4, 5};
    ctx.expect_eq(sol.search(nums, 1), 0);
    ctx.expect_eq(sol.search(nums, 3), 2);
    ctx.expect_eq(sol.search(nums, 5), 4);
    ctx.expect_eq(sol.search(nums, 0), -1);
    ctx.expect_eq(sol.search(nums, 6), -1);
  });

  ctx.test("edge_cases_small_and_empty", [&] {
    ctx.expect_eq(sol.search({}, 1), -1);
    ctx.expect_eq(sol.search({1}, 1), 0);
    ctx.expect_eq(sol.search({1}, 0), -1);
    ctx.expect_eq(sol.search({3, 1}, 3), 0);
    ctx.expect_eq(sol.search({3, 1}, 1), 1);
    ctx.expect_eq(sol.search({3, 1}, 2), -1);
  });
}

}  // namespace
