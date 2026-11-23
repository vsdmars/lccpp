// LeetCode 35: Search Insert Position
// https://leetcode.com/problems/search-insert-position/
//
// Problem:
//   Given a sorted array of distinct integers `nums` and a `target` value,
//   return the index of `target` if it is found. Otherwise, return the index
//   where `target` would be inserted to keep `nums` sorted in ascending order.
//   The solution must run in O(log n) time.
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
