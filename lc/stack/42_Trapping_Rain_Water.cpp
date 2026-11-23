// LeetCode 42: Trapping Rain Water
// https://leetcode.com/problems/trapping-rain-water/
//
// Problem:
//   Given `n` non-negative integers `height` representing an elevation map
//   where the width of each bar is 1, compute the total volume of rainwater
//   that can be trapped between the bars after raining.
//
// Algorithm:
//   Primary (Two Pointers, O(1) space):
//     Place pointers `left = 0` and `right = n - 1` at the two ends and track
//     `water_level` (the maximum of all boundary bars processed so far). At
//     each step, choose the shorter of `height[left]` and `height[right]` as
//     `lower` and advance that pointer inward. Because the opposite end holds
//     a bar at least as tall as `lower` (and hence the opposite side's running
//     maximum is at least `water_level`), the trapped water directly above the
//     stepped bar is determined solely by `max(water_level, lower) - lower`.
//     Update `water_level = max(water_level, lower)` and accumulate
//     `water_level - lower`.
//     Time complexity: O(n). Space complexity: O(1).
//
//   Secondary (Monotonic Decreasing Stack, O(n) space):
//     Maintain a stack of bar indices with non-increasing heights. When a bar
//     taller than the stack top arrives, pop the valley floor `bottom`; if a
//     left wall remains on the stack, add the horizontal water layer bounded
//     by `min(height[left], height[i]) - height[bottom]` across width
//     `i - left - 1`.
//     Time complexity: O(n). Space complexity: O(n).
//
// Data Structures:
//   - Two index pointers (`left`, `right`) and scalar water-level tracker for
//     the O(1)-space solution.
//   - std::vector<std::size_t> as a monotonic decreasing index stack for the
//     stack-based solution.
//
// Real-World Applications:
//   - Hydrology, civil engineering, and GIS terrain analysis for computing
//     surface depression storage and watershed ponding volume.
//   - Image processing morphological watershed segmentation and flood-fill
//     basins.
//   - 2D skyline/elevation capacity modeling in architectural drainage design.
//
// Related LeetCode Problems:
//   - #11 Container With Most Water [done] (two-pointer max single container)
//   - #84 Largest Rectangle In Histogram [done] (monotonic stack bar areas)
//   - #407 Trapping Rain Water II [done] (3D elevation map with min-heap BFS)
//   - #238 Product of Array Except Self [done] (prefix/suffix array pattern)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Computes trapped rainwater using the O(1)-space converging two-pointer
  // scan. Time: O(n), Space: O(1).
  int trap(const std::vector<int>& height) {
    // Fewer than 3 bars cannot enclose a basin between a left and right wall.
    if (height.size() < 3uz) {
      return 0;
    }

    std::size_t left = 0uz;
    std::size_t right = height.size() - 1uz;
    // Highest bounding wall seen along the already-processed shorter side.
    int water_level = 0;
    int total_water = 0;

    while (left < right) {
      // Advance whichever side currently has the shorter bar; the opposite
      // side is guaranteed to hold a wall >= this bar, so `water_level` is a
      // valid upper bound on trapped water at this position.
      int lower = 0;
      if (height[left] < height[right]) {
        lower = height[left];
        ++left;
      } else {
        lower = height[right];
        --right;
      }

      water_level = std::max(water_level, lower);
      total_water += water_level - lower;
    }

    return total_water;
  }

  // Alternative monotonic decreasing stack implementation that accumulates
  // trapped water layer by layer horizontally. Time: O(n), Space: O(n).
  int trapMonotonicStack(const std::vector<int>& height) {
    // Stack invariant: indices stored in `desc_stack` have non-increasing
    // heights from bottom to top.
    std::vector<std::size_t> desc_stack;
    int total_water = 0;

    for (std::size_t right = 0uz; right < height.size(); ++right) {
      // Current bar acts as a right wall for every strictly shorter bar on top
      // of the stack.
      while (!desc_stack.empty() && height[right] > height[desc_stack.back()]) {
        const std::size_t floor_idx = desc_stack.back();
        desc_stack.pop_back();

        // Without a left wall behind the floor, water spills out to the left.
        if (desc_stack.empty()) {
          break;
        }

        const std::size_t left = desc_stack.back();
        const int width = static_cast<int>((right - left) - 1uz);
        const int bounded_height =
            std::min(height[left], height[right]) - height[floor_idx];
        total_water += width * bounded_height;
      }
      desc_stack.push_back(right);
    }

    return total_water;
  }
};

LC_PROBLEM(42, "Trapping Rain Water", "stack", ctx) {
  Solution sol;

  auto expect_both = [&](const std::vector<int>& bars, int expected) {
    ctx.expect_eq(sol.trap(bars), expected);
    ctx.expect_eq(sol.trapMonotonicStack(bars), expected);
  };

  ctx.test("leetcode_examples", [&] {
    expect_both({0, 1, 0, 2, 1, 0, 1, 3, 2, 1, 2, 1}, 6);
    expect_both({4, 2, 0, 3, 2, 5}, 9);
  });

  ctx.test("flat_and_monotonic_terrains", [&] {
    expect_both({}, 0);
    expect_both({5}, 0);
    expect_both({3, 3}, 0);
    expect_both({1, 2, 3, 4, 5}, 0);
    expect_both({5, 4, 3, 2, 1}, 0);
    expect_both({2, 2, 2, 2}, 0);
  });

  ctx.test("deep_basins_and_plateaus", [&] {
    expect_both({5, 0, 0, 0, 5}, 15);
    expect_both({5, 2, 1, 2, 1, 5}, 14);
    expect_both({3, 0, 2, 0, 4}, 7);
  });
}

}  // namespace
