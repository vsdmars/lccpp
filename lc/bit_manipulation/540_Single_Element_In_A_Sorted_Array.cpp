// LeetCode 540: Single Element in a Sorted Array
// https://leetcode.com/problems/single-element-in-a-sorted-array/
//
// Problem:
//   Given a sorted array `nums` of integers in which every element appears
//   exactly twice except for one element that appears exactly once, return the
//   single non-duplicate element.
//   Constraints: 1 <= nums.length <= 10^5 (always odd), 0 <= nums[i] <= 10^5.
//   Target complexity: O(log n) time and O(1) space.
//
// Algorithm:
//   Primary (Binary Search with Bitwise Parity Flip `mid ^ 1uz`):
//   Before the unique element, every duplicate pair occupies indices
//   `(2k, 2k + 1)` (even followed by odd). Immediately after the unique
//   element, pair alignment shifts to `(2k + 1, 2k + 2)` (odd followed by
//   even). For any index `mid < high`, flipping its least significant bit
//   (`mid ^ 1uz`) produces its expected pre-shift partner (`mid + 1` if `mid`
//   is even, `mid - 1` if `mid` is odd):
//     - If `nums[mid] == nums[mid ^ 1uz]`, pair alignment is still intact up
//       through `mid`, so the single element must lie strictly to the right:
//       `low = mid + 1uz`.
//     - Otherwise alignment has already broken at or before `mid`, so narrow
//       to the left half inclusive of `mid`: `high = mid`.
//   Because `high` starts at the even index `nums.size() - 1` and `mid < high`,
//   `mid ^ 1uz` is guaranteed to remain in bounds `[0, nums.size() - 1]`.
//   Time Complexity:  O(log n).
//   Space Complexity: O(1).
//
//   Alternative (XOR Fold, matching the Python source): XOR all elements so
//   duplicate pairs cancel (`x ^ x == 0`), leaving the unique element.
//   Time Complexity:  O(n).
//   Space Complexity: O(1).
//
// Data Structures:
//   Read-only sorted array (`const std::vector<int>&`) searched in place using
//   two `std::size_t` binary-search bounds (`low`, `high`).
//
// Real-World Applications:
//   Locating a single unclosed or corrupted record in a sorted paired event
//   log (e.g., begin/commit transaction pairs or matched open/close audit
//   entries) in logarithmic time via random access.
//
// Related LeetCode Problems:
//   - #136 Single Number [done] (unsorted single-element lookup via O(n) XOR)
//   - #137 Single Number II [done] (elements appear 3x except one; bit counting)
//   - #260 Single Number III [done] (two unique elements partitioned by lowest set bit)
//   - #389 Find the Difference [done] (XOR cancellation across two sequences)
//   - #704 Binary Search [done] (foundational binary-search invariant template)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the unique element in a sorted array of pairs in O(log n) time using
  // binary search and the `mid ^ 1uz` parity-partner bit trick.
  // Invariant: the unique element always lies inside the closed index interval
  // `[low, high]`.
  // Time: O(log n), Space: O(1).
  int singleNonDuplicate(const std::vector<int>& nums) {
    std::size_t low = 0uz;
    std::size_t high = nums.size() - 1uz;

    while (low < high) {
      const std::size_t mid = low + ((high - low) / 2uz);

      // Flipping bit 0 maps even `2k -> 2k + 1` and odd `2k + 1 -> 2k`.
      // Because `nums.size()` is odd, `mid < nums.size() - 1` guarantees
      // `mid ^ 1uz < nums.size()`.
      if (nums[mid] == nums[mid ^ 1uz]) {
        // Pair alignment is intact at `mid`; the unique element lies to the right.
        low = mid + 1uz;
      } else {
        // Alignment is broken at or before `mid`; keep `mid` in the search range.
        high = mid;
      }
    }

    return nums[low];
  }

  // Alternative O(n) XOR reduction (directly porting the Python solution).
  // Every paired value cancels via `x ^ x == 0`, leaving the single element.
  // Time: O(n), Space: O(1).
  int singleNonDuplicateXor(const std::vector<int>& nums) {
    std::uint32_t acc = 0U;
    for (const int num : nums) {
      acc ^= static_cast<std::uint32_t>(num);
    }
    return static_cast<int>(acc);
  }
};

LC_PROBLEM(540, "Single Element In A Sorted Array", "bit_manipulation", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    const std::vector<int> nums = {1, 1, 2, 3, 3, 4, 4, 8, 8};
    ctx.expect_eq(sol.singleNonDuplicate(nums), 2);
    ctx.expect_eq(sol.singleNonDuplicateXor(nums), 2);
  });

  ctx.test("example_2", [&] {
    const std::vector<int> nums = {3, 3, 7, 7, 10, 11, 11};
    ctx.expect_eq(sol.singleNonDuplicate(nums), 10);
    ctx.expect_eq(sol.singleNonDuplicateXor(nums), 10);
  });

  ctx.test("single_element_array", [&] {
    const std::vector<int> nums = {42};
    ctx.expect_eq(sol.singleNonDuplicate(nums), 42);
    ctx.expect_eq(sol.singleNonDuplicateXor(nums), 42);
  });

  ctx.test("unique_at_boundaries", [&] {
    const std::vector<int> at_front = {1, 2, 2, 3, 3};
    ctx.expect_eq(sol.singleNonDuplicate(at_front), 1);
    ctx.expect_eq(sol.singleNonDuplicateXor(at_front), 1);

    const std::vector<int> at_back = {1, 1, 2, 2, 9};
    ctx.expect_eq(sol.singleNonDuplicate(at_back), 9);
    ctx.expect_eq(sol.singleNonDuplicateXor(at_back), 9);
  });

  ctx.test("negative_numbers", [&] {
    const std::vector<int> nums = {-5, -5, -3, -1, -1, 0, 0};
    ctx.expect_eq(sol.singleNonDuplicate(nums), -3);
    ctx.expect_eq(sol.singleNonDuplicateXor(nums), -3);
  });
}

}  // namespace
