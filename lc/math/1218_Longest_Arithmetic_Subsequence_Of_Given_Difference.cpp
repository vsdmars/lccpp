// LeetCode 1218: Longest Arithmetic Subsequence Of Given Difference
// https://leetcode.com/problems/longest-arithmetic-subsequence-of-given-difference/
//
// Problem:
//   Given an integer array `arr` and an integer `difference`, return the length
//   of the longest subsequence of `arr` that forms an arithmetic progression
//   whose consecutive elements differ by exactly `difference`
//   (i.e., `subseq[i + 1] - subseq[i] == difference`). A subsequence preserves
//   relative order from `arr` while allowing arbitrary deletions.
//   Constraints: 1 <= arr.length <= 10^5, -10^4 <= arr[i], difference <= 10^4.
//
// Algorithm:
//   Use single-pass dynamic programming indexed by element value rather than
//   array position. Define `dp[v]` as the maximum length of a valid arithmetic
//   subsequence ending at value `v` among elements processed so far.
//   For each `num` in `arr`:
//     1. Look up its required predecessor `prev = num - difference` in `dp`.
//     2. The best subsequence ending at this occurrence of `num` has length
//        `dp[prev] + 1` (or `1` if `prev` has not been seen yet).
//     3. Update `dp[num]` to this length and track the running maximum.
//   Invariant: Because every `dp[x]` is monotonically non-decreasing over the
//   scan, overwriting `dp[num]` with `dp[num - difference] + 1` never loses a
//   longer earlier chain ending at `num`, and correctly handles `difference == 0`
//   by extending the existing count of `num` by 1.
//   Time Complexity:  O(n) average-case, where n = arr.size().
//   Space Complexity: O(n) in the worst case for the hash table.
//
// Data Structures:
//   - Hash map (`std::unordered_map<int, int>`): maps each encountered element
//     value `v` to the length of the longest arithmetic subsequence ending at
//     `v`, enabling O(1) average-time predecessor lookups.
//
// Real-World Applications:
//   - Event-log and telemetry correlation: finding the longest chain of
//     regularly spaced periodic events (e.g., heartbeats or cron ticks with a
//     fixed interval) embedded inside noisy timestamp streams.
//   - Packet sequence analysis: detecting the longest stride-regular sequence
//     of packet IDs or memory addresses for hardware prefetcher pattern
//     detection.
//   - Signal and audio processing: tracking constant-step harmonic or rhythm
//     progressions in discrete quantized frames.
//
// Related LeetCode Problems:
//   - #1027 Longest Arithmetic Subsequence [done] (generalizes to unknown common difference via 2D DP)
//   - #300 Longest Increasing Subsequence [done] (classic subsequence DP without a fixed step)
//   - #446 Arithmetic Slices II [new] - Subsequence (counts all arithmetic subsequences of length >= 3)
//   - #873 Length of Longest Fibonacci Subsequence [new] (predecessor-lookup subsequence DP using a hash map)
//   - #1048 Longest String Chain [done] (hash-map DP extending chains from single-step predecessors)

#include "framework/lc_framework.h"

#include <algorithm>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the length of the longest subsequence of `arr` with constant step
  // `difference`.
  // Time: O(n) average, Space: O(n).
  int longestSubsequence(const std::vector<int>& arr, int difference) {
    if (arr.empty()) {
      return 0;
    }

    // `dp[v]` stores the length of the longest valid arithmetic subsequence
    // ending with value `v` observed in the processed prefix of `arr`.
    std::unordered_map<int, int> dp;
    dp.reserve(arr.size());

    int longest = 0;
    for (const int num : arr) {
      // Query the predecessor value without default-inserting missing keys,
      // avoiding unnecessary hash table growth.
      const auto it = dp.find(num - difference);
      const int chain_len = (it != dp.end()) ? (it->second + 1) : 1;

      // Update the best chain ending at `num`. When `difference == 0`, `it`
      // points to the previous count of `num`, naturally incrementing it by 1.
      dp[num] = chain_len;
      longest = std::max(longest, chain_len);
    }

    return longest;
  }
};

LC_PROBLEM(1218, "Longest Arithmetic Subsequence Of Given Difference", "math",
           ctx) {
  Solution sol;

  ctx.test("example_1_strictly_increasing", [&] {
    ctx.expect_eq(sol.longestSubsequence({1, 2, 3, 4}, 1), 4);
  });

  ctx.test("example_2_no_adjacent_step_match", [&] {
    ctx.expect_eq(sol.longestSubsequence({1, 3, 5, 7}, 1), 1);
  });

  ctx.test("example_3_negative_difference_with_duplicates", [&] {
    ctx.expect_eq(sol.longestSubsequence({1, 5, 7, 8, 5, 3, 4, 2, 1}, -2), 4);
  });

  ctx.test("zero_difference_counts_most_frequent", [&] {
    ctx.expect_eq(sol.longestSubsequence({1, 2, 1, 3, 1, 2, 1}, 0), 4);
  });

  ctx.test("negative_values_and_order_sensitivity", [&] {
    ctx.expect_eq(sol.longestSubsequence({-5, -3, -1, 1, 3}, 2), 5);
    // Out-of-order elements cannot extend earlier predecessors; [4, 7] gives 2.
    ctx.expect_eq(sol.longestSubsequence({3, 0, -3, 4, -4, 7, 6}, 3), 2);
  });

  ctx.test("edge_cases_single_and_empty", [&] {
    ctx.expect_eq(sol.longestSubsequence({42}, 5), 1);
    ctx.expect_eq(sol.longestSubsequence({}, 1), 0);
  });
}

}  // namespace
