// LeetCode 605: Can Place Flowers
// https://leetcode.com/problems/can-place-flowers/
//
// Problem:
//   Given an integer array `flowerbed` containing 0s (empty plots) and 1s
//   (planted plots) where no two existing 1s are adjacent, and an integer `n`,
//   return true if `n` new flowers can be planted in `flowerbed` without
//   violating the no-adjacent-flowers rule, and false otherwise.
//
// Examples:
//   Example 1:
//     Input:  flowerbed = [1, 0, 0, 0, 1], n = 1
//     Output: true
//     Explanation:
//       - Initialize remaining = 1 and left_empty = true.
//       - At i = 0, flowerbed[0] == 1 sets left_empty = false; at i = 1,
//         flowerbed[1] == 0 sees left_empty = false, skips planting, and sets
//         left_empty = true.
//       - At i = 2, flowerbed[2] == 0 has left_empty = true and right_empty = true
//         (flowerbed[3] == 0), so plant here, decrement remaining to 0, and return
//         true immediately.
//
//   Example 2:
//     Input:  flowerbed = [1, 0, 0, 0, 1], n = 2
//     Output: false
//     Explanation:
//       - Initialize remaining = 2 and left_empty = true.
//       - Indices 0 and 1 leave remaining = 2 and set left_empty = true entering
//         index 2.
//       - At i = 2 (0), both left_empty and right_empty (flowerbed[3] == 0) are
//         true, so plant virtually, decrement remaining to 1, and set left_empty =
//         false.
//       - At i = 3 (0), left_empty is false (and flowerbed[4] == 1), so no flower
//         is planted; at i = 4 (1), left_empty becomes false.
//       - After scanning all plots with remaining = 1 > 0, return false.
//
//   Example 3:
//     Input:  flowerbed = [0, 0, 1, 0, 0], n = 2
//     Output: true
//     Explanation:
//       - Initialize remaining = 2 and left_empty = true because index 0 has no
//         left neighbor.
//       - At i = 0 (0), left_empty = true and right_empty = true (flowerbed[1] ==
//         0), so plant at index 0, decrement remaining to 1, and set left_empty =
//         false.
//       - At i = 1 (0), left_empty = false prevents planting and resets left_empty
//         = true; at i = 2 (1), left_empty is set to false.
//       - At i = 3 (0), left_empty = false prevents planting and resets left_empty
//         = true; at i = 4 (0), left_empty = true and right_empty = true (i + 1 ==
//         5), so plant at index 4, decrement remaining to 0, and return true.
//
// Algorithm:
//   Scan the flowerbed from left to right and greedily plant a flower at the
//   earliest legal plot. By a standard exchange argument, planting at index `i`
//   instead of delaying to `i + 1` never reduces the remaining capacity of the
//   suffix `flowerbed[i+2..]`.
//   To avoid mutating the input array while keeping O(1) space, maintain a
//   single boolean `left_empty` tracking whether plot `i - 1` is empty (true
//   at index 0 because there is no left neighbor). For each empty plot `i`:
//     - Check `right_empty` (true at the last index or when `flowerbed[i+1] ==
//       0`).
//     - If both `left_empty` and `right_empty` hold, plant here: decrement
//       `remaining`, return true immediately once `remaining == 0`, and set
//       `left_empty = false` for the next plot.
//     - Otherwise, leave plot `i` empty and set `left_empty = true`.
//
//   Time Complexity:  O(m), where m = flowerbed.size(), with early exit once
//                     `n` flowers are planted.
//   Space Complexity: O(1) auxiliary space (does not mutate input).
//
// Data Structures:
//   - Single-pass linear scan over `const std::vector<int>&` with scalar state
//     (`left_empty` and `remaining`).
//
// Real-World Applications:
//   - Seating and physical-distancing layout planning (maximizing non-adjacent
//     seats in theaters, flights, or exam halls).
//   - Radio-frequency / cellular tower channel assignment along linear paths
//     where adjacent cells cannot share a channel.
//   - Guard-interval insertion in hardware buffers and memory banks to avoid
//     adjacent-cell crosstalk.
//
// Related LeetCode Problems:
//   - #198 House Robber [done] (maximum-weight non-adjacent selection on a path)
//   - #495 Teemo Attacking [new] (linear greedy interval scan on 1D timeline)
//   - #849 Maximize Distance to Closest Person [new] (optimal empty slot placement between occupied seats)
//   - #213 House Robber II [done] (circular extension of non-adjacent selection)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Returns true iff `n` non-adjacent flowers can be planted in `flowerbed`.
  // Time: O(flowerbed.size()), Space: O(1).
  bool canPlaceFlowers(const std::vector<int>& flowerbed, int n) {
    if (n <= 0) {
      return true;
    }

    int remaining = n;
    // Boundary condition: index 0 has no left neighbor, so left is "empty".
    bool left_empty = true;
    const std::size_t size = flowerbed.size();

    for (std::size_t i = 0uz; i < size; ++i) {
      if (flowerbed[i] == 1) {
        // Plot is already occupied; next plot cannot be planted.
        left_empty = false;
        continue;
      }

      // Boundary condition: the last plot has no right neighbor.
      const bool right_empty = (i + 1uz == size) || (flowerbed[i + 1uz] == 0);
      if (left_empty && right_empty) {
        --remaining;
        if (remaining == 0) {
          return true;
        }
        // Record that we virtually planted at index `i`.
        left_empty = false;
      } else {
        // Plot `i` remains empty for plot `i + 1`.
        left_empty = true;
      }
    }

    return false;
  }
};

LC_PROBLEM(605, "Can Place Flowers", "greedy", ctx) {
  ctx.test("example_1_true", [&] {
    Solution sol;
    ctx.expect_true(sol.canPlaceFlowers({1, 0, 0, 0, 1}, 1));
  });

  ctx.test("example_2_false", [&] {
    Solution sol;
    ctx.expect_false(sol.canPlaceFlowers({1, 0, 0, 0, 1}, 2));
  });

  ctx.test("python_build_cases", [&] {
    Solution sol;
    ctx.expect_true(sol.canPlaceFlowers({1}, 0));
    ctx.expect_true(sol.canPlaceFlowers({1, 0, 0, 0, 1, 0, 0}, 2));
    ctx.expect_false(sol.canPlaceFlowers({1, 0, 0, 0, 1, 0, 0}, 3));
  });

  ctx.test("single_empty_plot", [&] {
    Solution sol;
    ctx.expect_true(sol.canPlaceFlowers({0}, 1));
    ctx.expect_false(sol.canPlaceFlowers({0}, 2));
  });

  ctx.test("boundary_and_all_empty", [&] {
    Solution sol;
    ctx.expect_true(sol.canPlaceFlowers({0, 0, 1, 0, 0}, 2));
    ctx.expect_false(sol.canPlaceFlowers({0, 0, 1, 0, 0}, 3));
    ctx.expect_true(sol.canPlaceFlowers({0, 0, 0, 0, 0}, 3));
    ctx.expect_false(sol.canPlaceFlowers({0, 0, 0, 0, 0}, 4));
  });
}

}  // namespace
