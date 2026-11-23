// LeetCode 16: 3Sum Closest
// https://leetcode.com/problems/3sum-closest/
//
// Problem:
//   Given an integer array `nums` of length n >= 3 and an integer `target`,
//   find three integers in `nums` (at distinct indices) whose sum is closest
//   to `target`, and return that sum. You may assume each input has exactly
//   one closest sum.
//
// Examples:
//   Example 1:
//     Input:  nums = [-1, 2, 1, -4], target = 1
//     Output: 2
//     Explanation:
//       - Sort nums to [-4, -1, 1, 2] and initialize best_sum = -4 + (-1) + 1 = -4
//         with min_diff = |-4 - 1| = 5.
//       - For anchor i = 0 (-4), left = 1 (-1) and right = 3 (2) give sum = -3
//         (diff = 4 < 5), updating best_sum = -3; advancing left to 2 (1) gives sum
//         = -1 (diff = 2 < 4), updating best_sum = -1 and min_diff = 2.
//       - For anchor i = 1 (-1), left = 2 (1) and right = 3 (2) give sum = -1 + 1 +
//         2 = 2 (diff = |2 - 1| = 1 < 2), updating best_sum = 2 and returning 2
//         after the loop finishes.
//
//   Example 2:
//     Input:  nums = [4, 0, 5, -5, 3, 3, 0, -4, -5], target = -2
//     Output: -2
//     Explanation:
//       - Sort nums to [-5, -5, -4, 0, 0, 3, 3, 4, 5] and initialize best_sum = -5
//         + (-5) + (-4) = -14 with min_diff = 12.
//       - At anchor i = 0 (-5), left moves from 1 (-5) to 2 (-4) to 3 (0) against
//         right = 8 (5), narrowing best_sum to -4 (min_diff = 2).
//       - With left = 3 (0), right steps inward from 8 (5) to 7 (4) giving sum = -1
//         (best_sum = -1, min_diff = 1), and then to 6 (3) where -5 + 0 + 3 = -2 ==
//         target, returning -2 immediately.
//
//   Example 3:
//     Input:  nums = [1, 1, 1, 0], target = -100
//     Output: 2
//     Explanation:
//       - Sort nums to [0, 1, 1, 1] and initialize best_sum = 0 + 1 + 1 = 2 with
//         min_diff = |2 - (-100)| = 102.
//       - At anchor i = 0 (0), left = 1 (1) and right = 3 (1) give sum = 2 > -100,
//         so right skips duplicate 1s down to left without improving min_diff.
//       - At anchor i = 1 (1), left = 2 (1) and right = 3 (1) give sum = 3 > -100
//         (diff = 103), so the search finishes and returns best_sum = 2.
//
// Algorithm:
//   Sort-and-two-pointers. Sort `nums` ascending, initialize `best_sum` with
//   the first three elements, and iterate over each distinct anchor index `i`
//   from `0` to `n - 3`. For each anchor, sweep `left = i + 1` and
//   `right = n - 1` inward:
//   - If `sum == target`, return `target` immediately (distance 0 is optimal).
//   - Otherwise compare `|sum - target|` against `min_diff` (using 64-bit
//     arithmetic to avoid overflow) and update `best_sum` when closer.
//   - Move `left` right if `sum < target` or `right` left if `sum > target`,
//     skipping consecutive identical values to avoid redundant checks.
//   Time Complexity:  O(n^2) (O(n log n) sort + O(n^2) two-pointer sweep).
//   Space Complexity: O(log n) auxiliary for sorting.
//
// Data Structures:
//   - Sorted `std::vector<int>`: provides monotonic ordering so two pointers
//     can approach `target` in O(n) per fixed anchor.
//   - Scalar trackers (`best_sum`, `min_diff`): maintain the closest triplet
//     sum seen so far in O(1) space.
//
// Real-World Applications:
//   - Budget and resource allocation (picking three components or packages
//     whose combined cost/capacity most closely matches a target quota).
//   - Embedded/analog circuit design (selecting three standard resistor or
//     capacitor values whose combined value approximates a target spec).
//   - Logistics and bin packing (combining three shipments to closely hit a
//     container weight or volume limit).
//
// Related LeetCode Problems:
//   - #15 3Sum [done] (exact-zero triplet sum with the same sort + two-pointer pattern)
//   - #18 4Sum [done] (four-element extension of sorted two pointers)
//   - #259 3Sum Smaller [done] (counting triplets below a target bound)
//   - #167 Two Sum II [done] - Input Array Is Sorted (two-pointer search on a sorted array)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the sum of three distinct elements in `nums` that is closest to
  // `target`.
  //
  // Time: O(n^2), Space: O(log n) auxiliary for sorting.
  int threeSumClosest(std::vector<int> nums, int target) {
    if (nums.size() < 3uz) {
      return 0;
    }

    std::ranges::sort(nums);

    int best_sum = nums[0] + nums[1] + nums[2];
    std::int64_t min_diff =
        std::abs(static_cast<std::int64_t>(best_sum) - target);

    const std::size_t last_anchor = nums.size() - 2uz;
    for (std::size_t i = 0uz; i < last_anchor; ++i) {
      // Identical anchors explore a strict subset of already-checked pairs.
      if (i > 0uz && nums[i] == nums[i - 1uz]) {
        continue;
      }

      if (searchWithFixedFirst(nums, i, target, best_sum, min_diff)) {
        return target;
      }
    }

    return best_sum;
  }

 private:
  // Sweeps two pointers over `nums[anchor_idx + 1 .. n - 1]`, updating
  // `best_sum` and `min_diff` whenever a closer triplet is found. Returns
  // `true` immediately if an exact match (`sum == target`) is discovered.
  static bool searchWithFixedFirst(const std::vector<int>& nums,
                                   std::size_t anchor_idx, int target,
                                   int& best_sum, std::int64_t& min_diff) {
    const int anchor = nums[anchor_idx];
    std::size_t left = anchor_idx + 1uz;
    std::size_t right = nums.size() - 1uz;

    while (left < right) {
      const int sum = anchor + nums[left] + nums[right];
      if (sum == target) {
        return true;
      }

      const std::int64_t diff =
          std::abs(static_cast<std::int64_t>(sum) - target);
      if (diff < min_diff) {
        min_diff = diff;
        best_sum = sum;
      }

      // Advance past duplicate values on whichever pointer moves to skip
      // evaluating identical sums repeatedly.
      if (sum < target) {
        const int prev_left = nums[left];
        while (left < right && nums[left] == prev_left) {
          ++left;
        }
      } else {
        const int prev_right = nums[right];
        while (left < right && nums[right] == prev_right) {
          --right;
        }
      }
    }

    return false;
  }
};

LC_PROBLEM(16, "3Sum Closest", "two_pointers", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    ctx.expect_eq(sol.threeSumClosest({-1, 2, 1, -4}, 1), 2);
  });

  ctx.test("all_zeros", [&] {
    ctx.expect_eq(sol.threeSumClosest({0, 0, 0}, 1), 0);
  });

  ctx.test("far_negative_target", [&] {
    ctx.expect_eq(sol.threeSumClosest({1, 1, 1, 0}, -100), 2);
  });

  ctx.test("exact_hit_with_duplicates", [&] {
    ctx.expect_eq(sol.threeSumClosest({4, 0, 5, -5, 3, 3, 0, -4, -5}, -2), -2);
  });

  ctx.test("all_negative_numbers", [&] {
    ctx.expect_eq(sol.threeSumClosest({-100, -98, -2, -1}, -101), -101);
  });
}

}  // namespace
