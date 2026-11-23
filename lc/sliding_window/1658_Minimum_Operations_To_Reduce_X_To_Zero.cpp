// LeetCode 1658: Minimum Operations To Reduce X To Zero
// https://leetcode.com/problems/minimum-operations-to-reduce-x-to-zero/
//
// Problem:
//   Given an array of positive integers `nums` and a positive integer `x`, in
//   each operation you may remove either the leftmost or the rightmost element
//   of `nums` and subtract its value from `x`. Return the minimum number of
//   operations required to reduce `x` to exactly `0`, or `-1` if it is not
//   possible.
//   Constraints: 1 <= nums.length <= 10^5, 1 <= nums[i] <= 10^4,
//   1 <= x <= 10^9.
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 1, 4, 2, 3], x = 5
//     Output: 2
//     Explanation:
//       - Compute total_sum = 11 and middle subarray target = 11 - 5 = 6.
//       - Expand right across indices 0..2 until window_sum = 1 + 1 + 4 = 6 ==
//         target, recording max_middle_len = 3.
//       - At right = 3, window_sum = 8 > 6, so advance left to 2 to leave [4, 2]
//         with sum 6 (length 2, max_middle_len remains 3).
//       - At right = 4, window_sum = 9 > 6 and advancing left to 3 leaves [2, 3]
//         with sum 5, so return nums.size() - max_middle_len = 5 - 3 = 2.
//
//   Example 2:
//     Input:  nums = [3, 2, 20, 1, 1, 3], x = 10
//     Output: 5
//     Explanation:
//       - Compute total_sum = 30 and middle subarray target = 30 - 10 = 20.
//       - At right = 2, window_sum reaches 3 + 2 + 20 = 25 > 20, so advance left to
//         2 to leave [20] with sum 20 and max_middle_len = 1.
//       - As right advances through [1, 1, 3], window_sum reaches 25 at right = 5
//         and shrinking past index 2 drops the sum to 5 < 20.
//       - Return nums.size() - max_middle_len = 6 - 1 = 5 (removing prefix [3, 2]
//         and suffix [1, 1, 3]).
//
//   Example 3:
//     Input:  nums = [5, 6, 7, 8, 9], x = 4
//     Output: -1
//     Explanation:
//       - Compute total_sum = 35 and middle subarray target = 35 - 4 = 31.
//       - Expanding right across indices 0..3 yields sums 5, 11, 18, and 26, all
//         strictly below 31.
//       - At right = 4, window_sum = 35 > 31 and advancing left to 1 subtracts 5 to
//         leave 30 < 31, so window_sum never equals target.
//       - Since found remains false, return -1.
//
// Algorithm:
//   Reframe the problem by taking its complement: removing a prefix and a
//   suffix whose combined sum equals `x` is equivalent to leaving behind a
//   contiguous middle subarray whose sum equals `target = sum(nums) - x`.
//   Minimizing the number of removed elements is therefore equivalent to
//   maximizing the length of a contiguous subarray summing to `target`.
//   Because all elements of `nums` are positive, subarray sums grow
//   monotonically as the right endpoint advances and shrink monotonically as
//   the left endpoint advances. This allows a standard variable-size sliding
//   window `[left, right]`:
//     1. If `target < 0`, `x` exceeds the total sum, so return `-1`.
//     2. If `target == 0`, every element must be removed, so return `n`.
//     3. Expand `right`, adding `nums[right]` to `window_sum`. While
//        `window_sum > target`, subtract `nums[left]` and advance `left`.
//     4. Whenever `window_sum == target`, record the longest valid window
//        length.
//   Return `n - max_middle_len` if a valid middle subarray was found, or `-1`.
//   Time Complexity:  O(n) — both `left` and `right` traverse the array at
//                     most once.
//   Space Complexity: O(1) — uses only constant scalar state.
//
// Data Structures:
//   - Two index pointers (`left`, `right`) and a 64-bit running sum
//     (`window_sum`) implementing an in-place sliding window over `nums`.
//
// Real-World Applications:
//   - Ring-buffer and circular-log trimming: discarding the fewest head/tail
//     records to free a target byte budget while preserving the longest
//     contiguous retained log segment.
//   - Bandwidth / budget allocation: reclaiming an exact quota from the two
//     ends of an ordered job queue while minimizing disrupted tasks.
//   - Video/audio boundary clipping: trimming minimal intro/outro frames whose
//     combined duration hits a target reduction.
//
// Related LeetCode Problems:
//   - #209 Minimum Size Subarray Sum [done] (variable-size sliding window on positive integers for a sum target)
//   - #560 Subarray Sum Equals K [done] (prefix-sum hash map generalization when negative numbers break monotonicity)
//   - #1423 Maximum Points You Can Obtain from Cards [new] (same complementary middle-subarray reduction for prefix/suffix picks)
//   - #523 Continuous Subarray Sum [done] (contiguous subarray sum matching via prefix sums)
//   - #2516 Take K of Each Character From Left and Right [new] (prefix/suffix removal converted into maximizing the retained middle window)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the minimum number of leftmost/rightmost removals needed to reduce
  // `x` to zero, or `-1` if no such sequence of removals exists.
  //
  // Invariant:
  //   During the sliding window loop (`target > 0`), after the inner `while`
  //   loop finishes, `window_sum <= target` holds for `[left, right]`.
  //   Consequently, whenever `window_sum == target`, `left <= right` is
  //   guaranteed because an empty window has sum `0 < target`.
  //
  // Time: O(n), Space: O(1).
  [[nodiscard]] int minOperations(const std::vector<int>& nums, int x) {
    if (nums.empty()) {
      return x == 0 ? 0 : -1;
    }

    // Use 64-bit integers for sums so large inputs cannot overflow 32-bit int.
    const std::int64_t total_sum =
        std::accumulate(nums.begin(), nums.end(), std::int64_t{0});
    const std::int64_t target = total_sum - static_cast<std::int64_t>(x);

    // If `x` exceeds the sum of all elements, reducing `x` to 0 is impossible.
    if (target < 0) {
      return -1;
    }
    // If `x` equals the entire array sum, every element must be removed.
    if (target == 0) {
      return static_cast<int>(nums.size());
    }

    // Find the longest contiguous middle subarray summing to `target`.
    std::int64_t window_sum = 0;
    std::size_t left = 0;
    std::size_t max_middle_len = 0;
    bool found = false;

    for (std::size_t right = 0; right < nums.size(); ++right) {
      window_sum += nums[right];

      // Shrink from the left until the window sum no longer exceeds `target`.
      while (left <= right && window_sum > target) {
        window_sum -= nums[left];
        ++left;
      }

      if (window_sum == target) {
        found = true;
        max_middle_len = std::max(max_middle_len, (right - left) + 1uz);
      }
    }

    if (!found) {
      return -1;
    }
    return static_cast<int>(nums.size() - max_middle_len);
  }
};

LC_PROBLEM(1658, "Minimum Operations To Reduce X To Zero", "sliding_window",
           ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    ctx.expect_eq(sol.minOperations({1, 1, 4, 2, 3}, 5), 2);
  });

  ctx.test("example_2_impossible", [&] {
    ctx.expect_eq(sol.minOperations({5, 6, 7, 8, 9}, 4), -1);
  });

  ctx.test("example_3_both_ends", [&] {
    ctx.expect_eq(sol.minOperations({3, 2, 20, 1, 1, 3}, 10), 5);
  });

  ctx.test("remove_all_elements", [&] {
    ctx.expect_eq(sol.minOperations({1, 2, 3}, 6), 3);
  });

  ctx.test("x_exceeds_total_sum", [&] {
    ctx.expect_eq(sol.minOperations({1, 2, 3}, 7), -1);
  });

  ctx.test("single_element_match_and_miss", [&] {
    ctx.expect_eq(sol.minOperations({5}, 5), 1);
    ctx.expect_eq(sol.minOperations({5}, 3), -1);
  });

  ctx.test("remove_left_prefix_only", [&] {
    ctx.expect_eq(sol.minOperations({2, 3, 1, 1, 1}, 5), 2);
  });

  ctx.test("zero_target_operations", [&] {
    ctx.expect_eq(sol.minOperations({1, 2, 3}, 0), 0);
  });
}

}  // namespace
