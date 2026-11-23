// LeetCode 35: Search Insert Position
// https://leetcode.com/problems/search-insert-position/
//
// Problem:
//   Given a sorted array of distinct integers `nums` and a `target` value,
//   return the index of `target` if it is found. Otherwise, return the index
//   where `target` would be inserted to keep `nums` sorted in ascending order.
//   The solution must run in O(log n) time.
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 3, 5, 6], target = 5
//     Output: 2
//     Explanation:
//       - Initialize [left, right) = [0, 4) (matching the range searched by
//         std::ranges::lower_bound).
//       - Compute mid = 2; since nums[2] = 5 >= 5, shrink the right bound to right
//         = 2.
//       - On [0, 2), compute mid = 1; since nums[1] = 3 < 5, advance left = 2.
//       - Terminate with left == right == 2 and return 2.
//
//   Example 2:
//     Input:  nums = [1, 3, 5, 6], target = 2
//     Output: 1
//     Explanation:
//       - Initialize [left, right) = [0, 4) to locate the smallest index where
//         nums[i] >= 2.
//       - Compute mid = 2 (nums[2] = 5 >= 2) to set right = 2, then mid = 1
//         (nums[1] = 3 >= 2) to set right = 1.
//       - On [0, 1), compute mid = 0 (nums[0] = 1 < 2), advancing left = 1.
//       - Terminate with left == right == 1 and return 1 as the insertion index.
//
//   Example 3:
//     Input:  nums = [1, 3, 5, 6], target = 7
//     Output: 4
//     Explanation:
//       - Initialize [left, right) = [0, 4) with target = 7 larger than every
//         element in nums.
//       - Compute mid = 2 (nums[2] = 5 < 7) and advance left = 3.
//       - On [3, 4), compute mid = 3 (nums[3] = 6 < 7) and advance left = 4.
//       - Terminate with left == right == 4 (matching nums.end() from lower_bound)
//         and return 4.
//
// Algorithm:
//   Compute the lower bound of `target` in `nums` — the smallest index `i` in
//   `[0, n]` such that `i == n` or `nums[i] >= target`.
//   - Primary method (`searchInsert`): uses `std::ranges::lower_bound`, the
//     idiomatic C++23 equivalent of Python's `bisect.bisect_left`.
//   - Secondary method (`searchInsertManual`): explicit half-open binary search
//     over `[left, right)` that maintains the invariant `nums[k] < target` for
//     all `k < left` and `nums[k] >= target` for all `k >= right`, converging to
//     `left == right`.
//   Time Complexity: O(log n).
//   Space Complexity: O(1).
//
// Data Structures:
//   Sorted `std::vector<int>` accessed with random-access iterators or two
//   `std::size_t` search bounds.
//
// Real-World Applications:
//   Maintaining sorted arrays/vectors with binary insertion, locating bucket
//   boundaries in histograms and load-balancing consistent hash rings, and
//   key insertion lookup inside B-tree nodes.
//
// Related LeetCode Problems:
//   - #34 Find First And Last Position Of Element In Sorted Array [done] (lower and upper bound pair)
//   - #278 First Bad Version [done] (binary search for the first index satisfying a monotonic predicate)
//   - #704 Binary Search [done] (exact-match binary search returning -1 on miss)
//   - #744 Find Smallest Letter Greater Than Target [done] (upper-bound search with wrap-around)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the index of `target` in sorted `nums`, or its sorted insertion
  // index if absent, using C++23 `std::ranges::lower_bound`.
  //
  // Time: O(log n), Space: O(1).
  int searchInsert(const std::vector<int>& nums, int target) {
    const auto it = std::ranges::lower_bound(nums, target);
    return static_cast<int>(std::distance(nums.begin(), it));
  }

  // Explicit half-open binary search implementation of lower bound.
  //
  // Invariant: For every index `k < left`, `nums[k] < target`; for every index
  // `k >= right`, `nums[k] >= target`. When `left == right`, `left` is the
  // first index where `target` belongs.
  //
  // Time: O(log n), Space: O(1).
  int searchInsertManual(const std::vector<int>& nums, int target) {
    std::size_t left = 0uz;
    std::size_t right = nums.size();

    while (left < right) {
      const std::size_t mid = left + ((right - left) / 2);
      if (nums[mid] < target) {
        // `mid` and everything to its left are strictly smaller than `target`.
        left = mid + 1;
      } else {
        // `mid` is a valid insertion candidate; keep it in the closed-left side.
        right = mid;
      }
    }

    return static_cast<int>(left);
  }
};

LC_PROBLEM(35, "Search Insert Position", "binary_search", ctx) {
  Solution sol;

  const auto check = [&](const std::vector<int>& nums, int target,
                         int expected) {
    ctx.expect_eq(sol.searchInsert(nums, target), expected);
    ctx.expect_eq(sol.searchInsertManual(nums, target), expected);
  };

  ctx.test("leetcode_examples", [&] {
    check({1, 3, 5, 6}, 5, 2);
    check({1, 3, 5, 6}, 2, 1);
    check({1, 3, 5, 6}, 7, 4);
    check({1, 3, 5, 6}, 0, 0);
    check({1}, 0, 0);
  });

  ctx.test("edge_cases_empty_single_and_negatives", [&] {
    check({}, 42, 0);
    check({1}, 1, 0);
    check({1}, 2, 1);
    check({-10, -3, 0, 4, 9}, -15, 0);
    check({-10, -3, 0, 4, 9}, -3, 1);
    check({-10, -3, 0, 4, 9}, 5, 4);
    check({-10, -3, 0, 4, 9}, 10, 5);
  });
}

}  // namespace
