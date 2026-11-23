// LeetCode 154: Find Minimum In Rotated Sorted Array II
// https://leetcode.com/problems/find-minimum-in-rotated-sorted-array-ii/
//
// Problem:
//   Given an ascending sorted array `nums` that may contain duplicate values
//   and has been rotated between 1 and `n` times, return the minimum element
//   in the array while minimizing the number of comparisons.
//
// Examples:
//   Example 1:
//     Input:  nums = [2, 2, 2, 0, 1]
//     Output: 0
//     Explanation:
//       - Initialize closed search bounds left = 0 and right = 4 over nums.
//       - At mid = 2, nums[2] = 2 > nums[4] = 1, so the rotation drop lies in [3,
//         4] and left advances to 3.
//       - At mid = 3, nums[3] = 0 < nums[4] = 1, so [3, 4] is non-decreasing and
//         right shrinks to 3.
//       - The loop terminates with left == right == 3 and returns nums[3] = 0.
//
//   Example 2:
//     Input:  nums = [3, 3, 1, 3]
//     Output: 1
//     Explanation:
//       - Initialize closed search bounds left = 0 and right = 3 over nums.
//       - At mid = 1, nums[1] = 3 equals nums[3] = 3, so neither half can be
//         eliminated and right decrements to 2 while nums[1] preserves a copy of 3.
//       - At mid = 1 again, nums[1] = 3 > nums[2] = 1, so the minimum must lie in
//         [2, 2] and left advances to 2.
//       - The loop terminates with left == right == 2 and returns nums[2] = 1.
//
//   Example 3:
//     Input:  nums = [-3, -3, -5, -5, -3]
//     Output: -5
//     Explanation:
//       - Initialize closed search bounds left = 0 and right = 4 over nums.
//       - At mid = 2, nums[2] = -5 < nums[4] = -3, so right shrinks to 2.
//       - At mid = 1, nums[1] = -3 > nums[2] = -5, so left advances to mid + 1 = 2.
//       - The loop terminates with left == right == 2 and returns nums[2] = -5.
//
// Algorithm:
//   Use closed-interval binary search over `[left, right]` comparing
//   `nums[mid]` against `nums[right]`:
//   - If `nums[mid] > nums[right]`: the rotation drop must lie strictly inside
//     `[mid + 1, right]`, so set `left = mid + 1`.
//   - If `nums[mid] < nums[right]`: `[mid, right]` is non-decreasing and
//     cannot contain a smaller element strictly after `mid`, so set
//     `right = mid`.
//   - If `nums[mid] == nums[right]`: duplicates make it ambiguous which side
//     holds the minimum (e.g., `[3, 3, 1, 3]` vs. `[3, 1, 3, 3]`). Because
//     `mid < right` and `nums[mid]` retains an equal copy of `nums[right]`,
//     decrementing `right` by 1 safely discards index `right` without ever
//     losing the minimum value.
//   Time Complexity: O(log n) average (including unrotated arrays); O(n) worst
//   case when nearly all elements are identical.
//   Space Complexity: O(1).
//
// Data Structures:
//   Two `std::size_t` inclusive indices (`left`, `right`) over a contiguous
//   `std::vector<int>`.
//
// Real-World Applications:
//   Locating the earliest timestamp in a circular buffer when multiple entries
//   can share identical timestamps, and minimum-finding in rotated telemetry
//   buffers with coarse quantization.
//
// Related LeetCode Problems:
//   - #153 Find Minimum In Rotated Sorted Array [done] (distinct elements, guaranteed O(log n))
//   - #81 Search In Rotated Sorted Array II [done] (rotated target search with duplicates)
//   - #33 Search In Rotated Sorted Array [done] (rotated target search without duplicates)
//   - #162 Find Peak Element [done] (binary search via neighbor comparison)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the minimum element in a rotated non-decreasing array that may
  // contain duplicate values.
  //
  // Invariant: At least one instance of the global minimum value always lies
  // inside the closed interval `[left, right]`.
  //
  // Time: O(log n) average, O(n) worst case; Space: O(1).
  int findMin(const std::vector<int>& nums) {
    if (nums.empty()) {
      return 0;
    }

    std::size_t left = 0uz;
    std::size_t right = nums.size() - 1;

    while (left < right) {
      const std::size_t mid = left + ((right - left) / 2);

      if (nums[mid] > nums[right]) {
        // `mid` is strictly above `nums[right]`, so the rotation drop and
        // minimum must lie in `[mid + 1, right]`.
        left = mid + 1;
      } else if (nums[mid] < nums[right]) {
        // `[mid, right]` is sorted non-decreasing, so no element strictly
        // after `mid` can be smaller than `nums[mid]`.
        right = mid;
      } else {
        // `nums[mid] == nums[right]`: we cannot discard either half (e.g.,
        // `[3, 3, 1, 3]` vs `[3, 1, 3, 3]`), so shrink the right boundary by
        // one; `nums[mid]` preserves an equal copy if that value is minimal.
        --right;
      }
    }

    return nums[left];
  }
};

LC_PROBLEM(154, "Find Minimum In Rotated Sorted Array II", "binary_search",
           ctx) {
  Solution sol;

  ctx.test("leetcode_examples", [&] {
    ctx.expect_eq(sol.findMin({1, 3, 5}), 1);
    ctx.expect_eq(sol.findMin({2, 2, 2, 0, 1}), 0);
  });

  ctx.test("ambiguous_duplicate_rotations", [&] {
    ctx.expect_eq(sol.findMin({3, 3, 1, 3}), 1);
    ctx.expect_eq(sol.findMin({3, 1, 3, 3}), 1);
    ctx.expect_eq(sol.findMin({5, 5, 5, 5, 5, 1}), 1);
    ctx.expect_eq(sol.findMin({1, 5, 5, 5, 5, 5}), 1);
    ctx.expect_eq(sol.findMin({4, 5, 6, 7, 0, 1, 4}), 0);
  });

  ctx.test("edge_cases_uniform_and_small", [&] {
    ctx.expect_eq(sol.findMin({7}), 7);
    ctx.expect_eq(sol.findMin({2, 2, 2, 2}), 2);
    ctx.expect_eq(sol.findMin({1, 1}), 1);
    ctx.expect_eq(sol.findMin({2, 1}), 1);
    ctx.expect_eq(sol.findMin({-3, -3, -5, -5, -3}), -5);
  });
}

}  // namespace
