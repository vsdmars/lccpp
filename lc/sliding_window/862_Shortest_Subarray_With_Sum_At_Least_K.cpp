// LeetCode 862: Shortest Subarray With Sum At Least K
// https://leetcode.com/problems/shortest-subarray-with-sum-at-least-k/
//
// Problem:
//   Given an integer array `nums` (which may contain positive, zero, and
//   negative integers) and a positive integer `k`, return the length of the
//   shortest non-empty contiguous subarray of `nums` whose sum is at least
//   `k`. If no such subarray exists, return `-1`.
//
// Algorithm:
//   Prefix sums combined with a monotonically increasing deque. Because
//   negative numbers break the monotonicity needed for a standard two-pointer
//   sliding window, express any subarray sum `nums[l..r-1]` as
//   `prefix[r] - prefix[l]`. Maintain a deque of candidate starting pairs
//   `(prefix_sum, index)` with strictly increasing `prefix_sum` (and strictly
//   increasing `index`):
//     1. Front pop (optimality): while `prefix[r] - deque.front().sum >= k`,
//        record `r - deque.front().index` and pop the front. Any future `r' > r`
//        paired with that same front can only form a longer subarray, so it is
//        never needed again.
//     2. Back pop (dominance): while `prefix[r] <= deque.back().sum`, pop the
//        back. For any future `r' > r`, starting at `r` yields both a larger
//        or equal sum difference (`prefix[r'] - prefix[r] >= prefix[r'] - old`)
//        AND a shorter length (`r' - r < r' - old`), so the back entry is
//        strictly dominated.
//     3. Push `(prefix[r], r)` to the back.
//   Each index is pushed once and popped at most once.
//   Time Complexity: O(n), where n = nums.size().
//   Space Complexity: O(n) for the monotonic deque.
//
// Data Structures:
//   - `std::deque<PrefixEntry>` maintaining a monotonically increasing sequence
//     of `(prefix_sum, index)` pairs with O(1) amortized front/back operations.
//   - 64-bit signed accumulator `std::int64_t` to prevent overflow when summing
//     up to 10^5 elements of magnitude 10^5.
//
// Real-World Applications:
//   - Streaming anomaly detection: finding the shortest burst of net-positive
//     resource spikes (with intermittent dips) exceeding a threshold.
//   - Financial risk and momentum analysis: detecting the quickest time window
//     in which cumulative profit-and-loss (containing both gains and losses)
//     reaches a target drawdown or gain threshold.
//   - Battery and power budgeting: identifying the shortest interval over which
//     net energy discharge (combining consumption and regenerative charging)
//     exceeds a safety limit.
//
// Related LeetCode Problems:
//   - #209 Minimum Size Subarray Sum [done] (non-negative-only version solvable with plain two pointers)
//   - #239 Sliding Window Maximum [done] (classic monotonic deque over a sliding window)
//   - #560 Subarray Sum Equals K [done] (prefix sums for exact subarray sum with negative numbers)
//   - #1425 Constrained Subsequence Sum [new] (DP accelerated with a monotonic deque)
//   - #1499 Max Value of Equation [new] (monotonic deque optimizing a linear prefix/suffix objective)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <limits>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the shortest non-empty subarray length with sum >= k, or -1 if
  // none exists. Handles negative elements via prefix sums + monotonic deque.
  // Time: O(n), Space: O(n).
  int shortestSubarray(const std::vector<int>& nums, int k) {
    struct PrefixEntry {
      std::int64_t sum = 0;
      std::size_t index = 0uz;
    };

    const auto target = static_cast<std::int64_t>(k);
    constexpr std::size_t kNotFound = std::numeric_limits<std::size_t>::max();
    std::size_t best_len = kNotFound;

    // Deque holds candidate start prefixes in strictly increasing order of
    // both `sum` and `index`. Seed with the empty prefix P[0] = 0 at index 0.
    std::deque<PrefixEntry> candidates;
    candidates.push_back({.sum = 0, .index = 0uz});

    std::int64_t running_sum = 0;
    for (std::size_t i = 0uz; i < nums.size(); ++i) {
      running_sum += nums[i];
      const std::size_t prefix_idx = i + 1uz;

      // Shrink from the front: any front entry satisfying the threshold gives
      // its shortest possible valid subarray right now, so discard it.
      while (!candidates.empty() &&
             running_sum - candidates.front().sum >= target) {
        best_len = std::min(best_len, prefix_idx - candidates.front().index);
        candidates.pop_front();
      }

      // Maintain strict monotonicity from the back: any prior prefix >= current
      // running_sum is strictly worse (smaller difference and longer span).
      while (!candidates.empty() && running_sum <= candidates.back().sum) {
        candidates.pop_back();
      }

      candidates.push_back({.sum = running_sum, .index = prefix_idx});
    }

    return best_len == kNotFound ? -1 : static_cast<int>(best_len);
  }
};

LC_PROBLEM(862, "Shortest Subarray With Sum At Least K", "sliding_window",
           ctx) {
  ctx.test("leetcode example 1 single element", [&] {
    Solution sol;
    ctx.expect_eq(sol.shortestSubarray({1}, 1), 1);
  });

  ctx.test("leetcode example 2 impossible", [&] {
    Solution sol;
    ctx.expect_eq(sol.shortestSubarray({1, 2}, 4), -1);
  });

  ctx.test("leetcode example 3 negative dip", [&] {
    Solution sol;
    ctx.expect_eq(sol.shortestSubarray({2, -1, 2}, 3), 3);
  });

  ctx.test("python build negative reset then surge", [&] {
    Solution sol;
    ctx.expect_eq(sol.shortestSubarray({84, -37, 32, 40, 95}, 167), 3);
  });

  ctx.test("all negative elements", [&] {
    Solution sol;
    ctx.expect_eq(sol.shortestSubarray({-5, -2, -8, -1}, 1), -1);
  });

  ctx.test("large sums avoiding 32-bit overflow", [&] {
    Solution sol;
    ctx.expect_eq(
        sol.shortestSubarray({1000000000, 1000000000, 1000000000}, 2000000000),
        2);
  });
}

}  // namespace
