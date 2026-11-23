// LeetCode 34: Find First And Last Position Of Element In Sorted Array
// https://leetcode.com/problems/find-first-and-last-position-of-element-in-sorted-array/
//
// Problem:
//   Given an array of integers `nums` sorted in non-decreasing order and a
//   `target` value, find the starting and ending index of `target` in `nums`.
//   If `target` is not found, return `{-1, -1}`. The algorithm must run in
//   O(log n) time.
//
// Algorithm:
//   Perform two binary searches corresponding to lower and upper bounds:
//   1. `std::ranges::lower_bound(nums, target)` finds the first iterator `first`
//      with `*first >= target`. If `first == nums.end()` or `*first != target`,
//      `target` is absent and we immediately return `{-1, -1}`.
//   2. `std::ranges::upper_bound(first, nums.end(), target)` finds the first
//      iterator `after_last` strictly greater than `target` within the remaining
//      suffix `[first, nums.end())`. The ending index of `target` is then
//      `(after_last - nums.begin()) - 1`.
//   Time Complexity: O(log n).
//   Space Complexity: O(1).
//
// Data Structures:
//   Random-access iterators over a sorted `std::vector<int>` queried with
//   `std::ranges::lower_bound` and `std::ranges::upper_bound`.
//
// Real-World Applications:
//   Range scans on sorted B-tree index leaves and SSTables in databases,
//   slicing all log entries matching an exact timestamp in time-series stores,
//   and inverted-index posting list lookups.
//
// Related LeetCode Problems:
//   - #35 Search Insert Position [done] (single lower-bound lookup)
//   - #278 First Bad Version [done] (binary search for the first true predicate)
//   - #704 Binary Search [done] (classic exact-match binary search)
//   - #2089 Find Target Indices After Sorting Array [new] (same range after sorting)

#include "framework/lc_framework.h"

#include <algorithm>
#include <iterator>
#include <vector>

namespace {

class Solution {
 public:
  // Returns `{first_index, last_index}` for all occurrences of `target` in
  // sorted `nums`, or `{-1, -1}` if `target` is not present.
  //
  // Invariant: `first` points to the leftmost element `>= target`, and
  // `after_last` points to the leftmost element `> target`, so every element
  // in `[first, after_last)` equals `target`.
  //
  // Time: O(log n), Space: O(1).
  std::vector<int> searchRange(const std::vector<int>& nums, int target) {
    // Phase 1: Locate the leftmost position where `target` could appear.
    const auto first = std::ranges::lower_bound(nums, target);
    if (first == nums.end() || *first != target) {
      return {-1, -1};
    }

    // Phase 2: Locate the first element strictly greater than `target` in the
    // suffix starting at `first` so we avoid re-searching `[begin, first)`.
    const auto after_last =
        std::ranges::upper_bound(first, nums.end(), target);

    const int start_idx = static_cast<int>(std::distance(nums.begin(), first));
    const int end_idx =
        static_cast<int>(std::distance(nums.begin(), after_last)) - 1;
    return {start_idx, end_idx};
  }
};

LC_PROBLEM(34, "Find First And Last Position Of Element In Sorted Array",
           "binary_search", ctx) {
  Solution sol;

  ctx.test("example_found_multiple", [&] {
    ctx.expect_eq(sol.searchRange({5, 7, 7, 8, 8, 10}, 8),
                  std::vector<int>{3, 4});
  });

  ctx.test("example_not_found_and_empty", [&] {
    ctx.expect_eq(sol.searchRange({5, 7, 7, 8, 8, 10}, 6),
                  std::vector<int>{-1, -1});
    ctx.expect_eq(sol.searchRange({}, 0), std::vector<int>{-1, -1});
  });

  ctx.test("build_cases_boundaries", [&] {
    ctx.expect_eq(sol.searchRange({5, 7, 7, 8}, 5), std::vector<int>{0, 0});
    ctx.expect_eq(sol.searchRange({2, 2}, 3), std::vector<int>{-1, -1});
    ctx.expect_eq(sol.searchRange({5, 7, 7, 8}, 8), std::vector<int>{3, 3});
  });

  ctx.test("edge_cases_all_same_and_out_of_bounds", [&] {
    ctx.expect_eq(sol.searchRange({7, 7, 7, 7}, 7), std::vector<int>{0, 3});
    ctx.expect_eq(sol.searchRange({5, 7, 7, 8}, 1), std::vector<int>{-1, -1});
    ctx.expect_eq(sol.searchRange({1}, 1), std::vector<int>{0, 0});
    ctx.expect_eq(sol.searchRange({1}, 2), std::vector<int>{-1, -1});
  });
}

}  // namespace
