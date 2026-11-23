// LeetCode 84: Largest Rectangle In Histogram
// https://leetcode.com/problems/largest-rectangle-in-histogram/
//
// Problem:
//   Given an array of non-negative integers `heights` representing the bar
//   heights of a histogram where every bar has width 1, return the area of
//   the largest axis-aligned rectangle that fits completely inside the
//   histogram.
//
// Algorithm:
//   Use a monotonic increasing stack of bar indices. For every bar `j`, the
//   widest rectangle whose height equals `heights[j]` extends leftward to the
//   nearest strictly shorter bar `L` and rightward to the nearest strictly
//   shorter bar `R`, giving area `heights[j] * (R - L - 1)`.
//   Iterate `idx` from `0` through `n` (treating `idx == n` as a virtual
//   sentinel bar of height `0` so all remaining bars on the stack are flushed
//   without mutating the input):
//   - While the stack is non-empty and `cur_height < heights[stack.back()]`,
//     the top bar `top` has just found its right boundary `R = idx`. Pop `top`.
//     Its left boundary `L` is the new `stack.back()` (or `-1` if the stack is
//     now empty, giving width `idx`). Update `max_area`.
//   - Push `idx` onto the stack.
//   Each index is pushed once and popped at most once.
//   Time complexity: O(n), where n = heights.size().
//   Space complexity: O(n) for the monotonic index stack.
//
// Data Structures:
//   - std::vector<std::size_t> used as a monotonic increasing index stack so
//     each popped bar immediately knows both its left and right strictly
//     shorter boundaries in O(1).
//
// Real-World Applications:
//   - Computer vision and document layout analysis: finding the largest empty
//     rectangular region or maximal foreground block in binary masks (also the
//     core subroutine for LeetCode #85 Maximal Rectangle).
//   - VLSI floorplanning and 2D bin/rectangle packing on skyline profiles.
//   - Database and memory-allocator fragmentation analysis (largest
//     contiguous free-block window over time).
//
// Related LeetCode Problems:
//   - #42 Trapping Rain Water [done] (monotonic stack over elevation bars)
//   - #85 Maximal Rectangle [done] (row-by-row reduction to histogram #84)
//   - #907 Sum of Subarray Minimums [done] (same nearest-smaller boundaries)
//   - #1793 Maximum Score of a Good Subarray [new] (histogram max with anchor)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the maximum rectangular area inside the histogram `heights`.
  // Time: O(n), Space: O(n).
  int largestRectangleArea(const std::vector<int>& heights) {
    const std::size_t n = heights.size();
    // Stack invariant: indices in `inc_stack` refer to bars with
    // non-decreasing heights from bottom to top.
    std::vector<std::size_t> inc_stack;
    inc_stack.reserve(n + 1uz);
    int max_area = 0;

    // Iterate to `idx == n` with a virtual height-0 sentinel to flush all
    // bars remaining on the stack without mutating `heights`.
    for (std::size_t idx = 0uz; idx <= n; ++idx) {
      const int cur_height = (idx == n) ? 0 : heights[idx];

      // Any bar taller than `cur_height` cannot extend past `idx`, so `idx`
      // is its exact right boundary.
      while (!inc_stack.empty() && cur_height < heights[inc_stack.back()]) {
        const int bar_height = heights[inc_stack.back()];
        inc_stack.pop_back();

        // After popping, `inc_stack.back()` is the nearest strictly shorter
        // bar on the left; if empty, the rectangle spans all the way from 0.
        const std::size_t width =
            inc_stack.empty() ? idx : ((idx - inc_stack.back()) - 1uz);
        max_area = std::max(max_area, bar_height * static_cast<int>(width));
      }

      inc_stack.push_back(idx);
    }

    return max_area;
  }
};

LC_PROBLEM(84, "Largest Rectangle In Histogram", "stack", ctx) {
  Solution sol;

  ctx.test("leetcode_examples", [&] {
    ctx.expect_eq(sol.largestRectangleArea({2, 1, 5, 6, 2, 3}), 10);
    ctx.expect_eq(sol.largestRectangleArea({2, 4}), 4);
  });

  ctx.test("source_build_and_small_cases", [&] {
    ctx.expect_eq(sol.largestRectangleArea({2}), 2);
    ctx.expect_eq(sol.largestRectangleArea({}), 0);
    ctx.expect_eq(sol.largestRectangleArea({0, 0, 0}), 0);
  });

  ctx.test("monotonic_and_equal_heights", [&] {
    ctx.expect_eq(sol.largestRectangleArea({1, 2, 3, 4, 5}), 9);
    ctx.expect_eq(sol.largestRectangleArea({5, 4, 3, 2, 1}), 9);
    ctx.expect_eq(sol.largestRectangleArea({4, 4, 4, 4}), 16);
    ctx.expect_eq(sol.largestRectangleArea({2, 1, 2}), 3);
  });
}

}  // namespace
