// LeetCode 1004: Max Consecutive Ones III
// https://leetcode.com/problems/max-consecutive-ones-iii/
//
// Problem:
//   Given a binary array `nums` (containing only 0s and 1s) and a non-negative
//   integer `k`, return the maximum length of a contiguous subarray that
//   contains only 1s after flipping at most `k` zeros to ones.
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0], k = 2
//     Output: 6
//     Explanation:
//       - Initialize left = 0 and remaining_flips = 2, then advance through indices
//         0..4 ([1, 1, 1, 0, 0]) where the two zeros drop remaining_flips to 0 and
//         form a valid window of length 5.
//       - At index 5 (val = 0), remaining_flips becomes -1 < 0; since nums[0] == 1
//         does not refund a flip, left shifts to 1 while keeping remaining_flips =
//         -1.
//       - At indices 6 and 7 (both 1), remaining_flips stays -1 and outgoing ones
//         at nums[1] and nums[2] advance left to 3; at index 8 (1), outgoing
//         nums[3] == 0 refunds remaining_flips to 0 and advances left to 4.
//       - At index 9 (1), remaining_flips = 0 >= 0 so left stays 4 (window [4..9] =
//         [0, 0, 1, 1, 1, 1] of width 6); at index 10 (0), remaining_flips drops to
//         -1 and outgoing nums[4] == 0 restores it to 0 as left moves to 5.
//       - Return nums.size() - left = 11 - 5 = 6.
//
//   Example 2:
//     Input:  nums = [1, 0, 1, 1, 0, 0], k = 2
//     Output: 5
//     Explanation:
//       - Initialize left = 0 and remaining_flips = 2, scanning indices 0..4 ([1,
//         0, 1, 1, 0]) consumes both flips so remaining_flips reaches 0 with left
//         still at 0 (window length 5).
//       - At index 5 (val = 0), remaining_flips drops to -1 < 0, and because
//         outgoing nums[0] == 1 does not restore a flip, left advances to 1.
//       - Return nums.size() - left = 6 - 1 = 5.
//
//   Example 3:
//     Input:  nums = [0, 0, 0, 0], k = 0
//     Output: 0
//     Explanation:
//       - Start with left = 0 and remaining_flips = 0; at index 0 (0),
//         remaining_flips becomes -1 < 0, outgoing nums[0] == 0 restores
//         remaining_flips to 0, and left advances to 1.
//       - At each subsequent index 1, 2, and 3 (all 0), remaining_flips drops to
//         -1, the outgoing 0 at nums[left] refunds it to 0, and left advances to 2,
//         3, and 4.
//       - Return nums.size() - left = 4 - 4 = 0.
//
// Algorithm:
//   Non-shrinking sliding window. Maintain a left pointer `left` and a budget
//   `remaining_flips = k` representing how many additional zeros the current
//   window can absorb. For each element visited by `right`:
//     - If `nums[right] == 0`, consume one flip (`--remaining_flips`).
//     - If `remaining_flips < 0`, the window has too many zeros, so shift the
//       entire window right by 1 (`++left`), restoring one flip if the outgoing
//       element `nums[left]` was 0.
//   Because `left` advances by at most 1 per step, the window width never
//   shrinks: it expands whenever a longer valid run is discovered and slides
//   at the best width seen so far otherwise. At loop termination,
//   `nums.size() - left` equals the exact maximum valid window length.
//   Time Complexity: O(n), where n = nums.size().
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - Two index cursors (`left` and implicit `right`) plus a single integer
//     counter (`remaining_flips`); no auxiliary containers required.
//
// Real-World Applications:
//   - SLA and uptime analysis: finding the longest continuous healthy service
//     streak while tolerating up to `k` transient health-check failures.
//   - Storage and error-correcting streams: locating the longest contiguous
//     usable sector run given a budget of `k` bad-block remappings.
//   - Workforce / habit streak calculators: computing the longest consecutive
//     active-day streak when a user has `k` "streak freeze" tokens.
//
// Related LeetCode Problems:
//   - #485 Max Consecutive Ones [done] (base case with k = 0 flips allowed)
//   - #487 Max Consecutive Ones II [new] (special case allowing at most k = 1 flip)
//   - #424 Longest Repeating Character Replacement [done] (same non-shrinking sliding window over up to k replacements)
//   - #1493 Longest Subarray of 1's After Deleting One Element [new] (equivalent to k = 1 minus one deleted element)
//   - #2024 Maximize the Confusion of an Exam [done] (applying this exact technique to both 'T' and 'F')

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the longest contiguous run of 1s achievable by flipping at most
  // `k` zeros using a non-shrinking sliding window.
  // Time: O(n), Space: O(1).
  int longestOnes(const std::vector<int>& nums, int k) {
    std::size_t left = 0uz;
    int remaining_flips = k;

    for (const int val : nums) {
      if (val == 0) {
        --remaining_flips;
      }

      // Instead of shrinking with a `while` loop, shift `left` by 1 when over
      // budget so the window width monotonically tracks the maximum valid span.
      if (remaining_flips < 0) {
        if (nums[left] == 0) {
          ++remaining_flips;
        }
        ++left;
      }
    }

    return static_cast<int>(nums.size() - left);
  }
};

LC_PROBLEM(1004, "Max Consecutive Ones III", "sliding_window", ctx) {
  ctx.test("leetcode example 1", [&] {
    Solution sol;
    ctx.expect_eq(sol.longestOnes({1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0}, 2), 6);
  });

  ctx.test("leetcode example 2", [&] {
    Solution sol;
    ctx.expect_eq(
        sol.longestOnes(
            {0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1}, 3),
        10);
  });

  ctx.test("python build case", [&] {
    Solution sol;
    ctx.expect_eq(sol.longestOnes({1, 0, 1, 1, 0, 0}, 2), 5);
  });

  ctx.test("zero flips allowed all zeros", [&] {
    Solution sol;
    ctx.expect_eq(sol.longestOnes({0, 0, 0, 0}, 0), 0);
  });

  ctx.test("k covers entire array", [&] {
    Solution sol;
    ctx.expect_eq(sol.longestOnes({0, 0, 1, 0}, 4), 4);
  });

  ctx.test("empty array", [&] {
    Solution sol;
    ctx.expect_eq(sol.longestOnes({}, 2), 0);
  });
}

}  // namespace
