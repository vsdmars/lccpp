// LeetCode 503: Next Greater Element II
// https://leetcode.com/problems/next-greater-element-ii/
//
// Problem:
//   Given a circular integer array `nums` (where the element after the last
//   element wraps around to `nums[0]`), return an array `result` of the same
//   length where `result[i]` is the first strictly greater number encountered
//   when traversing forward circularly from index `i`, or `-1` if no strictly
//   greater number exists in `nums`.
//
// Algorithm:
//   Simulate concatenating a second copy of `nums` after itself (`[nums][nums]`)
//   without materializing the doubled array. Pre-seed a monotonic stack with
//   `nums` in reverse order so that `nums[0]` sits on top of the stack,
//   representing the wrapped-around suffix visible from `nums[n - 1]`. Then
//   scan `nums` from right to left (`n - 1` down to `0`):
//     1. Pop all stack entries `<= nums[i]`, since `nums[i]` blocks them from
//        being the next greater element for any earlier index.
//     2. If the stack is non-empty, its top is the nearest strictly greater
//        element to the right (circularly); otherwise the answer is `-1`.
//     3. Push `nums[i]` onto the stack to preserve the strictly-increasing
//        from top-to-bottom monotonic invariant.
//   Time Complexity:  O(n), as each element is pushed at most twice (once
//                     during seeding, once during the scan) and popped at most
//                     twice.
//   Space Complexity: O(n) for the monotonic stack (excluding the output array).
//
// Data Structures:
//   - Monotonic decreasing stack (`std::vector<int>`): stores candidate next
//     greater values to the right of the current index, with the closest
//     strictly greater candidate always at the top.
//
// Real-World Applications:
//   - Ring-buffer telemetry and cyclic time-series anomaly detection (finding
//     the next higher reading in a wrap-around 24-hour or periodic window).
//   - Round-robin job schedulers looking for the next higher-priority slot in
//     a circular dispatch table.
//   - Computational geometry operations on closed polygons (e.g., finding the
//     next taller vertex around a cyclic perimeter).
//
// Related LeetCode Problems:
//   - #496 Next Greater Element I [done] (non-circular next greater element)
//   - #556 Next Greater Element III [done] (next greater digit permutation)
//   - #739 Daily Temperatures [done] (distance to next greater element)
//   - #84 Largest Rectangle in Histogram [done] (monotonic stack boundaries)
//   - #901 Online Stock Span [new] (streaming monotonic stack queries)

#include "framework/lc_framework.h"

#include <cstddef>
#include <ranges>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the next strictly greater element for every position in the circular
  // array `nums`, returning `-1` when no greater element exists.
  // Time: O(n), Space: O(n).
  std::vector<int> nextGreaterElements(const std::vector<int>& nums) {
    const std::size_t n = nums.size();
    std::vector<int> result(n, -1);
    if (n == 0uz) {
      return result;
    }

    // Pre-seed the stack with a reversed copy of `nums` so `nums[0]` is on top.
    // This models the virtual second copy `[nums][nums]` for wrap-around lookup.
    std::vector<int> stack;
    stack.reserve(n * 2uz);
    for (int val : std::views::reverse(nums)) {
      stack.push_back(val);
    }

    // Scan right-to-left while maintaining the invariant that `stack` holds
    // strictly increasing values from top (back) to bottom (front).
    for (std::size_t i = n; i-- > 0uz;) {
      const int current = nums[i];

      // Discard candidates that cannot be strictly greater than `current`;
      // `current` itself will shadow them for all elements further left.
      while (!stack.empty() && current >= stack.back()) {
        stack.pop_back();
      }

      if (!stack.empty()) {
        result[i] = stack.back();
      }

      stack.push_back(current);
    }

    return result;
  }
};

LC_PROBLEM(503, "Next Greater Element II", "stack", ctx) {
  ctx.test("example_circular_wrap", [&] {
    Solution sol;
    ctx.expect_eq(sol.nextGreaterElements({1, 2, 1}),
                  std::vector<int>{2, -1, 2});
  });

  ctx.test("source_build_case", [&] {
    Solution sol;
    ctx.expect_eq(sol.nextGreaterElements({8, 1, 9, 7}),
                  std::vector<int>{9, 9, -1, 8});
  });

  ctx.test("strictly_descending", [&] {
    Solution sol;
    ctx.expect_eq(sol.nextGreaterElements({5, 4, 3, 2, 1}),
                  std::vector<int>{-1, 5, 5, 5, 5});
  });

  ctx.test("all_equal_elements", [&] {
    Solution sol;
    ctx.expect_eq(sol.nextGreaterElements({3, 3, 3}),
                  std::vector<int>{-1, -1, -1});
  });

  ctx.test("single_and_empty", [&] {
    Solution sol;
    ctx.expect_eq(sol.nextGreaterElements({42}), std::vector<int>{-1});
    ctx.expect_eq(sol.nextGreaterElements({}), std::vector<int>{});
  });

  ctx.test("negative_values", [&] {
    Solution sol;
    ctx.expect_eq(sol.nextGreaterElements({-1, 0, -2}),
                  std::vector<int>{0, -1, -1});
  });
}

}  // namespace
