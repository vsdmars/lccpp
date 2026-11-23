// LeetCode 153: Find Minimum In Rotated Sorted Array
// https://leetcode.com/problems/find-minimum-in-rotated-sorted-array/
//
// Problem:
//   Given an ascending sorted array `nums` of unique integers that has been
//   rotated between 1 and `n` times, return the minimum element in O(log n)
//   time.
//
// Algorithm:
//   Use closed-interval binary search over `[left, right]` with `right`
//   initialized to `nums.size() - 1` so `nums[right]` is always a valid
//   reference element inside the active window. At each step with
//   `mid = left + ((right - left) / 2)`:
//   - If `nums[mid] > nums[right]`, the rotation drop (and therefore the
//     minimum element) must lie strictly to the right of `mid`, so set
//     `left = mid + 1`.
//   - Otherwise (`nums[mid] <= nums[right]`), the subarray `[mid, right]` is
//     strictly sorted, so no element strictly after `mid` can be smaller than
//     `nums[mid]`; set `right = mid` (keeping `mid` as a candidate).
//   When `left == right`, `nums[left]` is the global minimum.
//   Time Complexity: O(log n).
//   Space Complexity: O(1).
//
// Data Structures:
//   Two `std::size_t` inclusive indices (`left`, `right`) over a contiguous
//   `std::vector<int>`.
//
// Real-World Applications:
//   Finding the oldest entry (head/start point) in a fixed-capacity circular
//   ring buffer or wrap-around log file without maintaining an external head
//   pointer, and locating rotation offsets in cyclic time-series storage.
//
// Related LeetCode Problems:
//   - #154 Find Minimum In Rotated Sorted Array II [done] (allows duplicate elements, O(n) worst case)
//   - #33 Search In Rotated Sorted Array [done] (searches a target in a rotated distinct array)
//   - #81 Search In Rotated Sorted Array II [done] (rotated target search with duplicates)
//   - #162 Find Peak Element [done] (O(log n) binary search on local slope comparisons)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the minimum element in a rotated ascending array of distinct
  // integers.
  //
  // Invariant: The global minimum always resides inside the closed interval
  // `[left, right]`. Because `left < right` implies `mid < right`, every
  // iteration strictly shrinks `right - left`.
  //
  // Time: O(log n), Space: O(1).
  int findMin(const std::vector<int>& nums) {
    if (nums.empty()) {
      return 0;
    }

    std::size_t left = 0uz;
    std::size_t right = nums.size() - 1;

    while (left < right) {
      const std::size_t mid = left + ((right - left) / 2);

      if (nums[mid] > nums[right]) {
        // `mid` is on the larger (pre-pivot) plateau, so the minimum must be
        // strictly to the right of `mid`.
        left = mid + 1;
      } else {
        // `[mid, right]` is strictly increasing, so `mid` itself may be the
        // minimum and everything to its right can be discarded.
        right = mid;
      }
    }

    return nums[left];
  }
};

LC_PROBLEM(153, "Find Minimum In Rotated Sorted Array", "binary_search", ctx) {
  Solution sol;

  ctx.test("leetcode_examples", [&] {
    ctx.expect_eq(sol.findMin({3, 4, 5, 1, 2}), 1);
    ctx.expect_eq(sol.findMin({4, 5, 6, 7, 0, 1, 2}), 0);
    ctx.expect_eq(sol.findMin({11, 13, 15, 17}), 11);
  });

  ctx.test("edge_cases_rotations_and_sizes", [&] {
    ctx.expect_eq(sol.findMin({42}), 42);
    ctx.expect_eq(sol.findMin({2, 1}), 1);
    ctx.expect_eq(sol.findMin({1, 2}), 1);
    ctx.expect_eq(sol.findMin({2, 3, 4, 5, 1}), 1);
    ctx.expect_eq(sol.findMin({-5, -2, 0, 3, -10, -8}), -10);
  });
}

}  // namespace
