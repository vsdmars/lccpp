// LeetCode 11: Container With Most Water
// https://leetcode.com/problems/container-with-most-water/
//
// Problem:
//   Given an array `height` of n non-negative integers where index `i`
//   represents a vertical line from `(i, 0)` to `(i, height[i])`, find two
//   lines that together with the x-axis form a container holding the maximum
//   amount of water `min(height[i], height[j]) * (j - i)`. The container may
//   not be slanted.
//
// Algorithm:
//   Greedy opposite-direction two pointers. Start with the widest possible
//   container at `left = 0` and `right = n - 1`. At each step, the limiting
//   wall is `min_height = min(height[left], height[right])`, yielding area
//   `min_height * (right - left)`. Because shrinking the width can only
//   improve the area if the limiting height strictly increases, advance
//   `left` past all lines `<= min_height` and retreat `right` past all lines
//   `<= min_height`. No optimal pair is ever skipped because any pair sharing
//   a discarded wall with a narrower width has strictly smaller area.
//   Time Complexity:  O(n), where n = height.size() (each pointer moves
//                     monotonically inward).
//   Space Complexity: O(1).
//
// Data Structures:
//   - Two index pointers (`left`, `right`): converge inward from both ends of
//     the input array in constant extra memory.
//
// Real-World Applications:
//   - Resource allocation and bandwidth scheduling (choosing two endpoints
//     maximizing `min(capacity_i, capacity_j) * span`).
//   - Financial time-series analysis (identifying widest high-floor intervals
//     bounded by two support levels).
//   - Geometry and civil/hydraulic basin capacity estimation over discrete
//     elevation profiles.
//
// Related LeetCode Problems:
//   - #42 Trapping Rain Water [done] (two-pointer water accumulation across all bars)
//   - #84 Largest Rectangle in Histogram [done] (monotonic stack maximum rectangular area)
//   - #167 Two Sum II [done] - Input Array Is Sorted (converging two-pointer search)
//   - #2517 Maximum Tastiness of Candy Basket [new] (maximizing minimum gap over sorted points)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the maximum water volume bounded by any two vertical lines in
  // `height` using greedy inward two pointers.
  //
  // Invariant: every pair `(a, b)` where either `a < left` or `b > right` has
  // area <= `max_area` (or was already evaluated), because discarding a wall
  // with height <= `min_height` can never discard a strictly better container.
  //
  // Time: O(n), Space: O(1).
  int maxArea(const std::vector<int>& height) {
    if (height.size() < 2uz) {
      return 0;
    }

    std::size_t left = 0uz;
    std::size_t right = height.size() - 1uz;
    int max_area = 0;

    while (left < right) {
      const int min_height = std::min(height[left], height[right]);
      const int width = static_cast<int>(right - left);
      max_area = std::max(max_area, min_height * width);

      // Skip all lines that are no taller than the current limiting wall;
      // with a strictly smaller width, they cannot produce a larger area.
      while (left < right && height[left] <= min_height) {
        ++left;
      }
      while (left < right && height[right] <= min_height) {
        --right;
      }
    }

    return max_area;
  }
};

LC_PROBLEM(11, "Container With Most Water", "two_pointers", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    ctx.expect_eq(sol.maxArea({1, 8, 6, 2, 5, 4, 8, 3, 7}), 49);
  });

  ctx.test("two_equal_lines", [&] {
    ctx.expect_eq(sol.maxArea({1, 1}), 1);
  });

  ctx.test("tall_outer_walls", [&] {
    ctx.expect_eq(sol.maxArea({4, 3, 2, 1, 4}), 16);
  });

  ctx.test("peak_in_middle", [&] {
    ctx.expect_eq(sol.maxArea({1, 2, 1}), 2);
    ctx.expect_eq(sol.maxArea({1, 2, 4, 3}), 4);
  });

  ctx.test("zero_heights_and_short_inputs", [&] {
    ctx.expect_eq(sol.maxArea({0, 0, 0}), 0);
    ctx.expect_eq(sol.maxArea({5}), 0);
    ctx.expect_eq(sol.maxArea({}), 0);
  });
}

}  // namespace
