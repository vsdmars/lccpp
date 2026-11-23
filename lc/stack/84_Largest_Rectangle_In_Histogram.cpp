// LeetCode 84: Largest Rectangle In Histogram
// https://leetcode.com/problems/largest-rectangle-in-histogram/
//
// Problem:
//   Given an array of non-negative integers `heights` representing the bar
//   heights of a histogram where every bar has width 1, return the area of
//   the largest axis-aligned rectangle that fits completely inside the
//   histogram.
//
// Examples:
//   Example 1:
//     Input:  heights = [2, 1, 5, 6, 2, 3]
//     Output: 10
//     Explanation:
//       - Push idx = 0 (height 2), then at idx = 1 (cur_height = 1 < 2) pop 0 with
//         width 1 (max_area = 2 * 1 = 2) and push 1, 2, and 3 so inc_stack = [1, 2,
//         3].
//       - At idx = 4 (cur_height = 2 < 6): pop 3 (height 6, width 4 - 2 - 1 = 1,
//         area 6) and pop 2 (height 5, width 4 - 1 - 1 = 2, area 10), updating
//         max_area = 10 before pushing 4 (inc_stack = [1, 4]).
//       - Push idx = 5 (height 3, inc_stack = [1, 4, 5]), then at sentinel idx = 6
//         (cur_height = 0) pop 5 (area 3 * 1 = 3), 4 (area 2 * (6 - 1 - 1) = 8),
//         and 1 (area 1 * 6 = 6), returning 10.
//
//   Example 2:
//     Input:  heights = [2, 1, 2]
//     Output: 3
//     Explanation:
//       - Push idx = 0 (height 2), then at idx = 1 (cur_height = 1 < 2) pop 0 with
//         width 1 to set max_area = 2, and push 1 (inc_stack = [1]).
//       - Push idx = 2 (height 2 >= 1) so inc_stack = [1, 2].
//       - At sentinel idx = 3 (cur_height = 0), pop 2 (height 2, width 3 - 1 - 1 =
//         1, area 2) and then pop 1 (height 1, empty stack gives width 3, area 3),
//         updating max_area to 3.
//
//   Example 3:
//     Input:  heights = [1, 2, 3, 4, 5]
//     Output: 9
//     Explanation:
//       - For idx = 0 through 4, each bar height exceeds heights[inc_stack.back()],
//         so no bars pop and inc_stack becomes [0, 1, 2, 3, 4].
//       - At sentinel idx = 5 (cur_height = 0), bars pop in reverse order: 4 yields
//         5 * 1 = 5, 3 yields 4 * 2 = 8, 2 yields 3 * 3 = 9, 1 yields 2 * 4 = 8,
//         and 0 yields 1 * 5 = 5, returning max_area = 9.
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
