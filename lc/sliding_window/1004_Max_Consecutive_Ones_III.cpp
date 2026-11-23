// LeetCode 1004: Max Consecutive Ones III
// https://leetcode.com/problems/max-consecutive-ones-iii/
//
// Problem:
//   Given a binary array `nums` (containing only 0s and 1s) and a non-negative
//   integer `k`, return the maximum length of a contiguous subarray that
//   contains only 1s after flipping at most `k` zeros to ones.
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
