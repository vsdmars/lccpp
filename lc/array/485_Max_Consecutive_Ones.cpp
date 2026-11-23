// LeetCode 485: Max Consecutive Ones
// https://leetcode.com/problems/max-consecutive-ones/
//
// Problem:
//   Given a binary array `nums` containing only `0`s and `1`s, return the
//   maximum number of consecutive `1`s in the array.
//
// Algorithm:
//   Perform a single linear scan while maintaining two counters:
//     - `current_streak`: length of the contiguous run of `1`s ending at the
//       current index.
//     - `max_streak`: longest run of `1`s observed so far.
//
//   When `nums[i] == 1`, increment `current_streak` and update `max_streak`.
//   When `nums[i] == 0`, reset `current_streak` to `0` because any subsequent
//   contiguous run of `1`s cannot cross a zero boundary.
//
//   Time Complexity:  O(n) — visits each element once.
//   Space Complexity: O(1) — uses only two integer counters.
//
// Data Structures:
//   - Two scalar integer counters (`current_streak`, `max_streak`): sufficient
//     to summarize all prior history in a single streaming pass.
//
// Real-World Applications:
//   - Measuring maximum contiguous service uptime streaks or consecutive SLA
//     compliance intervals from binary health-check time series.
//   - Finding the largest contiguous free-block run in bitmap memory/disk
//     allocators.
//   - Analyzing run-length statistics in binary signal processing and
//     run-length encoding (RLE) compression.
//
// Related LeetCode Problems:
//   - #487 Max Consecutive Ones II [new] (allows flipping at most one 0 to 1 via sliding window)
//   - #1004 Max Consecutive Ones III [done] (allows flipping at most k 0s to 1 via sliding window)
//   - #1446 Consecutive Characters [new] (longest run of any identical character in a string)
//   - #525 Contiguous Array [done] (longest binary subarray with equal counts of 0s and 1s)

#include "framework/lc_framework.h"

#include <algorithm>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the maximum length of a contiguous subarray of `1`s in `nums`.
  //
  // Invariant: After inspecting element `i`, `current_streak` equals the
  // length of the suffix of `nums[0..i]` consisting solely of `1`s, and
  // `max_streak` equals the maximum streak across `nums[0..i]`.
  //
  // Time: O(n), Space: O(1).
  int findMaxConsecutiveOnes(const std::vector<int>& nums) {
    int max_streak = 0;
    int current_streak = 0;

    for (const int bit : nums) {
      if (bit == 1) {
        ++current_streak;
        max_streak = std::max(max_streak, current_streak);
      } else {
        // A zero breaks the active run of consecutive ones.
        current_streak = 0;
      }
    }

    return max_streak;
  }
};

LC_PROBLEM(485, "Max Consecutive Ones", "array", ctx) {
  ctx.test("example_1_trailing_run", [&] {
    Solution sol;
    ctx.expect_eq(sol.findMaxConsecutiveOnes({1, 1, 0, 1, 1, 1}), 3);
  });

  ctx.test("python_build_case", [&] {
    Solution sol;
    ctx.expect_eq(sol.findMaxConsecutiveOnes({1, 1, 0, 1, 0, 1}), 2);
  });

  ctx.test("example_2_middle_run", [&] {
    Solution sol;
    ctx.expect_eq(sol.findMaxConsecutiveOnes({1, 0, 1, 1, 0, 1}), 2);
  });

  ctx.test("all_zeros", [&] {
    Solution sol;
    ctx.expect_eq(sol.findMaxConsecutiveOnes({0, 0, 0, 0}), 0);
  });

  ctx.test("all_ones", [&] {
    Solution sol;
    ctx.expect_eq(sol.findMaxConsecutiveOnes({1, 1, 1, 1, 1}), 5);
  });

  ctx.test("single_element_and_empty", [&] {
    Solution sol;
    ctx.expect_eq(sol.findMaxConsecutiveOnes({}), 0);
    ctx.expect_eq(sol.findMaxConsecutiveOnes({0}), 0);
    ctx.expect_eq(sol.findMaxConsecutiveOnes({1}), 1);
  });
}

}  // namespace
