// LeetCode 496: Next Greater Element I
// https://leetcode.com/problems/next-greater-element-i/
//
// Problem:
//   You are given two distinct-element integer arrays `nums1` and `nums2`,
//   where `nums1` is a subset of `nums2`. For each element `x` in `nums1`,
//   find the first element strictly greater than `x` appearing to the right of
//   `x` in `nums2`, or `-1` if no such element exists.
//
// Algorithm:
//   Right-to-left monotonic decreasing stack + hash map lookup:
//   1. Traverse `nums2` in reverse order while maintaining a strictly
//      decreasing stack of candidate next-greater values to the right.
//   2. For each `value` in `nums2`:
//      - Pop all elements `<= value` from the stack, because `value` is closer
//        and larger, so those popped elements can never be the next greater
//        element for anything further left.
//      - Record `next_greater[value] = stack.empty() ? -1 : stack.back()`.
//      - Push `value` onto the stack.
//   3. Build the answer vector by looking up each element of `nums1` in
//      `next_greater`.
//   Time Complexity: O(N + M), where N = `nums1.size()` and M = `nums2.size()`
//   (each element of `nums2` is pushed and popped at most once).
//   Space Complexity: O(M) for the monotonic stack and hash map.
//
// Data Structures:
//   - `std::vector<int>` serving as a monotonic decreasing stack.
//   - `std::unordered_map<int, int>` mapping each value in `nums2` to its next
//     greater element to the right.
//
// Real-World Applications:
//   - Financial time-series analysis (finding the next higher stock price or
//     breakout point after each trading day).
//   - UI/layout and visibility algorithms (nearest taller barrier / horizon
//     detection in 1D elevation profiles).
//   - Static analysis and compiler liveness checks finding the next dominating
//     event in an ordered trace.
//
// Related LeetCode Problems:
//   - #503 Next Greater Element II [done] (monotonic stack extension to circular arrays)
//   - #556 Next Greater Element III [done] (next lexicographical permutation of decimal digits)
//   - #739 Daily Temperatures [done] (index distance to the next greater value via monotonic stack)
//   - #84 Largest Rectangle in Histogram [done] (nearest smaller elements on both sides via monotonic stack)
//   - #901 Online Stock Span [new] (streaming monotonic stack for consecutive span queries)

#include "framework/lc_framework.h"

#include <ranges>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // For every element in `nums1`, returns the first greater element to its
  // right in `nums2`, or -1 if none exists.
  // Time: O(N + M), Space: O(M).
  std::vector<int> nextGreaterElement(const std::vector<int>& nums1,
                                      const std::vector<int>& nums2) {
    std::unordered_map<int, int> next_greater;
    next_greater.reserve(nums2.size());

    // Monotonic decreasing stack holding candidate next-greater values to the
    // right of the current position in `nums2`.
    std::vector<int> stack;
    stack.reserve(nums2.size());

    for (const int value : std::views::reverse(nums2)) {
      // Discard elements <= `value`; they are shadowed by `value` for every
      // element further to the left.
      while (!stack.empty() && stack.back() <= value) {
        stack.pop_back();
      }

      next_greater[value] = stack.empty() ? -1 : stack.back();
      stack.push_back(value);
    }

    std::vector<int> result;
    result.reserve(nums1.size());
    for (const int query : nums1) {
      result.push_back(next_greater[query]);
    }
    return result;
  }
};

LC_PROBLEM(496, "Next Greater Element I", "stack", ctx) {
  Solution sol;

  ctx.test("leetcode_examples", [&] {
    ctx.expect_eq(sol.nextGreaterElement({4, 1, 2}, {1, 3, 4, 2}),
                  std::vector<int>{-1, 3, -1});
    ctx.expect_eq(sol.nextGreaterElement({2, 4}, {1, 2, 3, 4}),
                  std::vector<int>{3, -1});
  });

  ctx.test("monotonic_sequences", [&] {
    // Strictly decreasing: no element has a greater value to its right.
    ctx.expect_eq(sol.nextGreaterElement({5, 3, 1}, {5, 4, 3, 2, 1}),
                  std::vector<int>{-1, -1, -1});
    // Strictly increasing: every element except the last maps to its neighbor.
    ctx.expect_eq(sol.nextGreaterElement({1, 2, 3, 4}, {1, 2, 3, 4}),
                  std::vector<int>{2, 3, 4, -1});
  });

  ctx.test("edge_cases", [&] {
    ctx.expect_eq(sol.nextGreaterElement({42}, {42}), std::vector<int>{-1});
    ctx.expect_eq(sol.nextGreaterElement({}, {1, 2, 3}), std::vector<int>{});
    ctx.expect_eq(sol.nextGreaterElement({-3, -1, 0}, {-3, -2, -1, 0}),
                  std::vector<int>{-2, 0, -1});
  });
}

}  // namespace
