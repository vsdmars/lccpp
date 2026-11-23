// LeetCode 704: Binary Search
// https://leetcode.com/problems/binary-search/
//
// Problem:
//   Given an array of unique integers `nums` sorted in strictly ascending order
//   and an integer `target`, return the 0-based index of `target` if it exists
//   in `nums`, or `-1` otherwise. The algorithm must run in O(log n) time.
//   Constraints: 1 <= nums.length <= 10^4, -10^4 < nums[i], target < 10^4.
//
// Algorithm:
//   Maintain a half-open search interval `[lhs, rhs)` initialized to `[0, n)`
//   representing the first index where `nums[i] >= target` (lower bound).
//   In each step compute `mid = lhs + ((rhs - lhs) / 2)` (overflow-safe):
//     - If `target <= nums[mid]`, `mid` or an earlier index is the lower bound,
//       so shrink to `[lhs, mid)`.
//     - Otherwise `nums[mid] < target`, so discard `mid` and shrink to
//       `[mid + 1, rhs)`.
//   When `lhs == rhs`, check whether `lhs < n` and `nums[lhs] == target`; if so
//   return `lhs`, otherwise return `-1`.
//   Time Complexity: O(log n).
//   Space Complexity: O(1).
//
// Data Structures:
//   - Contiguous sorted `std::vector<int>` indexed via two `std::size_t`
//     boundary pointers (`lhs`, `rhs`).
//
// Real-World Applications:
//   - Searching sorted database index pages (B-tree / SSTable block lookups).
//   - Symbol table and debug-info (DWARF) address-to-line lookup in linkers and
//     profilers.
//   - Git bisect and monotonic parameter threshold discovery in systems tuning.
//
// Related LeetCode Problems:
//   - #33 Search in Rotated Sorted Array [done] (binary search with rotated pivot)
//   - #34 Find First and Last Position of Element in Sorted Array [done] (lower and upper bound search)
//   - #35 Search Insert Position [done] (returns lower-bound insertion index directly)
//   - #278 First Bad Version [done] (lower-bound binary search over monotonic predicate)
//   - #744 Find Smallest Letter Greater Than Target [done] (upper-bound binary search with wraparound)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Searches for `target` in strictly ascending `nums` using half-open
  // lower-bound binary search.
  // Time: O(log n), Space: O(1).
  int search(const std::vector<int>& nums, int target) {
    // Half-open interval [lhs, rhs) tracks candidate lower-bound positions.
    // Invariant: for all i < lhs, nums[i] < target;
    //            for all i >= rhs, nums[i] >= target.
    std::size_t lhs = 0uz;
    std::size_t rhs = nums.size();

    while (lhs < rhs) {
      const std::size_t mid = lhs + ((rhs - lhs) / 2uz);
      if (target <= nums[mid]) {
        rhs = mid;
      } else {
        lhs = mid + 1uz;
      }
    }

    if (lhs < nums.size() && nums[lhs] == target) {
      return static_cast<int>(lhs);
    }
    return -1;
  }
};

LC_PROBLEM(704, "Binary Search", "binary_search", ctx) {
  ctx.test("example_1_found", [&] {
    Solution sol;
    ctx.expect_eq(sol.search({-1, 0, 3, 5, 9, 12}, 9), 4);
  });

  ctx.test("example_2_missing_interior", [&] {
    Solution sol;
    ctx.expect_eq(sol.search({-1, 0, 3, 5, 9, 12}, 2), -1);
  });

  ctx.test("python_build_greater_than_all", [&] {
    Solution sol;
    ctx.expect_eq(sol.search({-1, 0, 3, 5, 9, 12}, 13), -1);
  });

  ctx.test("boundary_elements_and_below_min", [&] {
    Solution sol;
    const std::vector<int> nums = {-1, 0, 3, 5, 9, 12};
    ctx.expect_eq(sol.search(nums, -1), 0);
    ctx.expect_eq(sol.search(nums, 12), 5);
    ctx.expect_eq(sol.search(nums, -10), -1);
  });

  ctx.test("single_and_empty_arrays", [&] {
    Solution sol;
    ctx.expect_eq(sol.search({5}, 5), 0);
    ctx.expect_eq(sol.search({5}, -5), -1);
    ctx.expect_eq(sol.search({}, 1), -1);
  });
}

}  // namespace
