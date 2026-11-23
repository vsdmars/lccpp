// LeetCode 1027: Longest Arithmetic Sequence
// https://leetcode.com/problems/longest-arithmetic-subsequence/
//
// Problem:
//   Given an integer array `nums`, return the length of the longest arithmetic
//   subsequence in `nums`. A subsequence is formed by deleting zero or more
//   elements without changing the relative order of the remaining elements, and
//   a sequence `B` is arithmetic if `B[i + 1] - B[i]` is constant for all valid
//   `i`.
//   Constraints: 2 <= nums.length <= 1000, 0 <= nums[i] <= 500 (the algorithm
//   handles arbitrary signed 32-bit integers).
//
// Examples:
//   Example 1:
//     Input:  nums = [3, 6, 9, 12]
//     Output: 4
//     Explanation:
//       - Initialize dp with 4 empty maps and best = 2.
//       - At right = 1 (val 6), left = 0 (val 3) yields diff = 3; dp[0] has no key
//         3, so dp[1][3] = 2 and best = 2.
//       - At right = 2 (val 9), left = 0 sets dp[2][6] = 2, and left = 1 (val 6)
//         yields diff = 3, extending dp[1][3] = 2 to dp[2][3] = 3 with best = 3.
//       - At right = 3 (val 12), left = 2 (val 9) yields diff = 3, extending
//         dp[2][3] = 3 to dp[3][3] = 4 and updating best = 4.
//
//   Example 2:
//     Input:  nums = [20, 1, 15, 3, 10, 5, 8]
//     Output: 4
//     Explanation:
//       - At right = 2 (val 15), left = 0 (val 20) computes diff = -5 and records
//         dp[2][-5] = 2.
//       - At right = 4 (val 10), left = 2 (val 15) computes diff = -5 and extends
//         dp[2][-5] = 2 to dp[4][-5] = 3, raising best to 3.
//       - At right = 5 (val 5), left = 4 (val 10) computes diff = -5 and extends
//         dp[4][-5] = 3 to dp[5][-5] = 4, raising best to 4 for [20, 15, 10, 5].
//       - At right = 6 (val 8), none of the differences against indices 0..5 match
//         prior keys in dp[0..5], so every candidate has length 2 and best remains
//         4.
//
//   Example 3:
//     Input:  nums = [1, 4, 7, 4, 10]
//     Output: 4
//     Explanation:
//       - At right = 1 (val 4), left = 0 (val 1) sets dp[1][3] = 2; at right = 2
//         (val 7), left = 1 extends diff = 3 to dp[2][3] = 3 and sets best = 3.
//       - At right = 3 (val 4), pairing with indices 0, 1, 2 records dp[3][3] = 2,
//         dp[3][0] = 2, and dp[3][-3] = 2 without disrupting dp[2][3] = 3.
//       - At right = 4 (val 10), left = 2 (val 7) yields diff = 3 and extends
//         dp[2][3] = 3 to dp[4][3] = 4, while left = 3 (val 4) only starts dp[4][6]
//         = 2.
//       - The global maximum best updates to 4 for the subsequence [1, 4, 7, 10]
//         and is returned.
//
// Algorithm:
//   2D Dynamic Programming Indexed by `(end_index, common_difference)`:
//   Let `dp[right][diff]` be the length of the longest arithmetic subsequence
//   ending at index `right` with step size `diff`.
//   For every pair of indices `0 <= left < right < n`:
//   - Compute `diff = nums[right] - nums[left]`.
//   - If `dp[left]` already tracks a subsequence with difference `diff`, we can
//     extend it by appending `nums[right]` to get length `dp[left][diff] + 1`;
//     otherwise `{nums[left], nums[right]}` forms a new arithmetic subsequence
//     of length `2`.
//   - Update `dp[right][diff] = max(dp[right][diff], candidate_len)` and keep a
//     running global maximum `best`.
//   Time Complexity: O(n^2) expected, as each `(left, right)` pair performs
//   O(1) expected hash-table lookup and update.
//   Space Complexity: O(n^2) worst case for storing per-index difference maps.
//
// Data Structures:
//   - Vector of hash maps (`std::vector<std::unordered_map<int, int>>`):
//     `dp[i]` maps each achievable common difference `diff` to the maximum
//     arithmetic subsequence length ending at index `i`. Using hash maps avoids
//     fixed value-range assumptions and handles negative or large differences.
//
// Real-World Applications:
//   - Signal processing and telemetry analysis: detecting periodic beacon
//     pulses or regularly spaced events buried inside noisy timestamp streams.
//   - Time-series anomaly & pattern mining: identifying recurring fixed-cadence
//     user actions, cron jobs, or C2 heartbeats in network flow logs.
//   - Computational biology & genomics: finding regularly spaced tandem motifs
//     along a sequence.
//
// Related LeetCode Problems:
//   - #1218 Longest Arithmetic Subsequence of Given Difference [done] (fixed-step specialization solvable in O(n) with one hash map)
//   - #446 Arithmetic Slices II [new] - Subsequence (counts all arithmetic subsequences of length >= 3 using the same `dp[i][diff]` table)
//   - #413 Arithmetic Slices [new] (contiguous arithmetic subarrays in O(n) time)
//   - #300 Longest Increasing Subsequence [done] (classic subsequence DP pattern)
//   - #873 Length of Longest Fibonacci Subsequence [new] (2D subsequence DP over value pairs)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the length of the longest arithmetic subsequence in `nums`.
  // Time: O(n^2) expected, Space: O(n^2).
  int longestArithSeqLength(const std::vector<int>& nums) {
    const std::size_t n = nums.size();
    // Base cases: 0 or 1 element forms an arithmetic sequence of length `n`.
    if (n <= 1uz) {
      return static_cast<int>(n);
    }

    // `dp[i][diff]` stores the longest arithmetic subsequence ending at `i`
    // with common difference `diff`.
    std::vector<std::unordered_map<int, int>> dp(n);
    int best = 2;

    for (std::size_t right = 1uz; right < n; ++right) {
      auto& right_map = dp[right];
      for (std::size_t left = 0uz; left < right; ++left) {
        const int diff = nums[right] - nums[left];
        // Extend an existing chain ending at `left`, or start a new length-2
        // subsequence `{nums[left], nums[right]}`.
        const auto prev_it = dp[left].find(diff);
        const int candidate =
            (prev_it != dp[left].end()) ? (prev_it->second + 1) : 2;

        int& current = right_map[diff];
        current = std::max(current, candidate);
        best = std::max(best, current);
      }
    }

    return best;
  }
};

LC_PROBLEM(1027, "Longest Arithmetic Sequence", "hash", ctx) {
  Solution sol;

  ctx.test("example_1_whole_array", [&] {
    ctx.expect_eq(sol.longestArithSeqLength({3, 6, 9, 12}), 4);
  });

  ctx.test("example_2_non_contiguous", [&] {
    // Longest arithmetic subsequence: [4, 7, 10].
    ctx.expect_eq(sol.longestArithSeqLength({9, 4, 7, 2, 10}), 3);
  });

  ctx.test("example_3_negative_step", [&] {
    // Longest arithmetic subsequence: [20, 15, 10, 5].
    ctx.expect_eq(sol.longestArithSeqLength({20, 1, 15, 3, 10, 5, 8}), 4);
  });

  ctx.test("zero_difference_all_equal", [&] {
    ctx.expect_eq(sol.longestArithSeqLength({5, 5, 5, 5}), 4);
  });

  ctx.test("duplicate_value_interleaved", [&] {
    // [1, 4, 7, 10] has length 4 despite the extra 4 at index 3.
    ctx.expect_eq(sol.longestArithSeqLength({1, 4, 7, 4, 10}), 4);
  });

  ctx.test("negative_values", [&] {
    ctx.expect_eq(sol.longestArithSeqLength({-5, -2, 1, 4}), 4);
  });

  ctx.test("minimal_inputs", [&] {
    ctx.expect_eq(sol.longestArithSeqLength({7, 100}), 2);
    ctx.expect_eq(sol.longestArithSeqLength({42}), 1);
    ctx.expect_eq(sol.longestArithSeqLength({}), 0);
  });
}

}  // namespace
