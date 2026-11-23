// LeetCode 1438: Longest Continuous Subarray With Absolute Diff Less Than Or Equal To Limit
// https://leetcode.com/problems/longest-continuous-subarray-with-absolute-diff-less-than-or-equal-to-limit/
//
// Problem:
//   Given an integer array `nums` and a non-negative integer `limit`, return
//   the length of the longest non-empty contiguous subarray such that the
//   absolute difference between any two elements in the subarray is at most
//   `limit`. Because the maximum absolute difference between any pair of
//   elements in a subarray equals `max(subarray) - min(subarray)`, the
//   condition reduces to maintaining a window whose range does not exceed
//   `limit`.
//   Constraints: 1 <= nums.length <= 10^5, 1 <= nums[i] <= 10^9,
//   0 <= limit <= 10^9.
//
// Examples:
//   Example 1:
//     Input:  nums = [8, 2, 4, 7], limit = 4
//     Output: 2
//     Explanation:
//       - At right = 0 (8), max_dq = [0] and min_dq = [0], spread = 0 <= 4, so
//         best_len becomes 1.
//       - At right = 1 (2), max_dq = [0, 1] and min_dq = [1] give spread = 8 - 2 =
//         6 > 4, so left shifts to 1 and index 0 is evicted from max_dq while
//         best_len stays 1.
//       - At right = 2 (4), max_dq = [2] and min_dq = [1, 2] give spread = 4 - 2 =
//         2 <= 4, expanding best_len to (2 - 1) + 1 = 2.
//       - At right = 3 (7), max_dq = [3] and min_dq = [1, 2, 3] give spread = 7 - 2
//         = 5 > 4, shifting left to 2 and evicting index 1 from min_dq; best_len
//         remains 2 and is returned.
//
//   Example 2:
//     Input:  nums = [10, 1, 2, 4, 7, 2], limit = 5
//     Output: 4
//     Explanation:
//       - At right = 0 (10) and right = 1 (1), spread = 10 - 1 = 9 > 5 shifts left
//         to 1 and evicts index 0 from max_dq, keeping best_len = 1.
//       - For right = 2 (2) and right = 3 (4), max_dq holds [3] and min_dq holds
//         [1, 2, 3], keeping spread <= 3 <= 5 and growing best_len to 3.
//       - At right = 4 (7), spread = 7 - 1 = 6 > 5 advances left to 2 and evicts
//         index 1 from min_dq, keeping best_len = 3.
//       - At right = 5 (2), max_dq = [4, 5] and min_dq = [2, 5] (popping 3 and 4
//         since 2 < 4, 7) yield spread = 7 - 2 = 5 <= 5, updating best_len to (5 -
//         2) + 1 = 4 for [2, 4, 7, 2].
//
//   Example 3:
//     Input:  nums = [4, 2, 2, 2, 4, 4, 2, 2], limit = 0
//     Output: 3
//     Explanation:
//       - At right = 0 (4) and right = 1 (2), spread = 4 - 2 = 2 > 0 shifts left to
//         1 and evicts index 0 from max_dq, leaving best_len = 1.
//       - At right = 2 and right = 3 (both 2), spread = 2 - 2 = 0 <= 0 expands the
//         window [1..3] = [2, 2, 2] and updates best_len to 3.
//       - At right = 4 (4) through right = 7 (2), every candidate window retains
//         both a 4 (in max_dq) and a 2 (in min_dq) with spread = 2 > 0, advancing
//         left once per step up to 5 and returning best_len = 3.
//
// Algorithm:
//   Use a non-shrinking sliding window `[left, right]` backed by two monotonic
//   deques storing indices:
//     1. `max_dq` maintains candidate indices in monotonically non-increasing
//        order of `nums[idx]`, so `nums[max_dq.front()]` is always the current
//        window maximum.
//     2. `min_dq` maintains candidate indices in monotonically non-decreasing
//        order of `nums[idx]`, so `nums[min_dq.front()]` is always the current
//        window minimum.
//   As `right` advances by one, pop dominated indices from the back of each
//   deque and push `right`. If `nums[max_dq.front()] - nums[min_dq.front()]`
//   exceeds `limit`, advance `left` by one and evict any front index that has
//   fallen behind `left`. Because `left` advances by at most one per step, the
//   window size never shrinks below the best valid length discovered so far,
//   and every valid window of larger size updates `best_len`.
//   Time Complexity:  O(n) — each index is pushed and popped from each deque
//                     at most once.
//   Space Complexity: O(n) — each deque holds at most `n` indices.
//
// Data Structures:
//   - Two `std::deque<std::size_t>` monotonic queues (`max_dq`, `min_dq`):
//     track the sliding window's running maximum and minimum indices in O(1)
//     amortized time per element.
//
// Real-World Applications:
//   - Time-series anomaly and stability detection: finding the longest period
//     during which a sensor reading, latency metric, or stock price stays
//     within a bounded fluctuation band.
//   - Audio/signal processing: locating the longest stationary segment whose
//     amplitude spread remains below a noise threshold.
//   - Resource scheduling and SLA monitoring: identifying the longest workload
//     interval with bounded jitter between peak and trough utilization.
//
// Related LeetCode Problems:
//   - #239 Sliding Window Maximum [done] (monotonic deque for fixed-width window maximum)
//   - #480 Sliding Window Median [done] (dynamic order-statistic maintenance over a sliding window)
//   - #2024 Maximize the Confusion of an Exam [done] (same non-shrinking sliding window pattern)
//   - #2398 Maximum Number of Robots Within Budget [new] (combines a max monotonic deque with a variable sliding window)
//   - #2762 Continuous Subarrays [new] (counts all subarrays with max - min <= 2 using two monotonic deques)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the length of the longest contiguous subarray whose maximum and
  // minimum elements differ by at most `limit`.
  //
  // Invariant:
  //   At the end of each iteration `right`, `max_dq` and `min_dq` contain only
  //   indices in `[left, right]`, with their fronts pointing to the exact
  //   maximum and minimum values of `nums[left..right]`.
  //
  // Time: O(n), Space: O(n).
  [[nodiscard]] int longestSubarray(const std::vector<int>& nums, int limit) {
    // Guard against empty input or an impossible negative difference bound.
    if (nums.empty() || limit < 0) {
      return 0;
    }

    // `max_dq` stores indices with monotonically non-increasing values;
    // `min_dq` stores indices with monotonically non-decreasing values.
    std::deque<std::size_t> max_dq;
    std::deque<std::size_t> min_dq;

    std::size_t left = 0;
    std::size_t best_len = 0;
    const auto limit64 = static_cast<std::int64_t>(limit);

    for (std::size_t right = 0; right < nums.size(); ++right) {
      const int current = nums[right];

      // Discard indices whose values are strictly smaller than `current`, as
      // they can never be the window maximum while `right` is in the window.
      while (!max_dq.empty() && current > nums[max_dq.back()]) {
        max_dq.pop_back();
      }
      // Discard indices whose values are strictly larger than `current`, as
      // they can never be the window minimum while `right` is in the window.
      while (!min_dq.empty() && current < nums[min_dq.back()]) {
        min_dq.pop_back();
      }

      max_dq.push_back(right);
      min_dq.push_back(right);

      // Compute the window spread in 64-bit arithmetic to avoid signed overflow
      // if large negative and positive values appear in `nums`.
      const std::int64_t spread =
          static_cast<std::int64_t>(nums[max_dq.front()]) -
          static_cast<std::int64_t>(nums[min_dq.front()]);

      // If the window is invalid, shift `left` forward by one (non-shrinking
      // window) and evict any stale front indices that fell outside `[left, right]`.
      if (spread > limit64) {
        ++left;
        while (!max_dq.empty() && max_dq.front() < left) {
          max_dq.pop_front();
        }
        while (!min_dq.empty() && min_dq.front() < left) {
          min_dq.pop_front();
        }
      }

      // Window length is either a newly achieved valid maximum (if spread <= limit)
      // or equal to the previously achieved `best_len` (if `left` was shifted).
      best_len = std::max(best_len, (right - left) + 1uz);
    }

    return static_cast<int>(best_len);
  }
};

LC_PROBLEM(
    1438,
    "Longest Continuous Subarray With Absolute Diff Less Than Or Equal To Limit",
    "sliding_window", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    ctx.expect_eq(sol.longestSubarray({8, 2, 4, 7}, 4), 2);
  });

  ctx.test("example_2", [&] {
    ctx.expect_eq(sol.longestSubarray({10, 1, 2, 4, 7, 2}, 5), 4);
  });

  ctx.test("example_3_zero_limit_duplicates", [&] {
    ctx.expect_eq(sol.longestSubarray({4, 2, 2, 2, 4, 4, 2, 2}, 0), 3);
  });

  ctx.test("single_element", [&] {
    ctx.expect_eq(sol.longestSubarray({42}, 0), 1);
  });

  ctx.test("all_elements_within_limit", [&] {
    ctx.expect_eq(sol.longestSubarray({1, 5, 6, 7, 8, 10, 6, 5, 6}, 9), 9);
  });

  ctx.test("strictly_increasing_large_gaps", [&] {
    ctx.expect_eq(sol.longestSubarray({1, 10, 20, 30, 40}, 5), 1);
  });

  ctx.test("all_equal_elements", [&] {
    ctx.expect_eq(sol.longestSubarray({7, 7, 7, 7, 7}, 0), 5);
  });

  ctx.test("empty_array", [&] {
    ctx.expect_eq(sol.longestSubarray({}, 4), 0);
  });
}

}  // namespace
