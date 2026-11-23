// LeetCode 1248: Count Number Of Nice Subarrays
// https://leetcode.com/problems/count-number-of-nice-subarrays/
//
// Problem:
//   Given an integer array `nums` and a non-negative integer `k`, a contiguous
//   subarray is called "nice" if it contains exactly `k` odd numbers. Return
//   the total number of nice subarrays in `nums`.
//
// Algorithm:
//   Exact-K via At-Most-K inclusion-exclusion:
//     exact(k) = atMost(k) - atMost(k - 1).
//   To compute `atMost(max_odds)` in one pass with O(1) space:
//     - Maintain a sliding window `[left, right]` and `odd_budget = max_odds`.
//     - On each new element `nums[right]`, subtract 1 from `odd_budget` if it
//       is odd.
//     - While `odd_budget < 0`, advance `left`, refunding 1 to `odd_budget`
//       whenever `nums[left]` is odd.
//     - Every subarray `[start, right]` with `left <= start <= right` contains
//       at most `max_odds` odd numbers, adding `(right - left) + 1` to the
//       total.
//   Time Complexity: O(n), where n = nums.size() (two linear passes).
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - Two sliding-window pointers (`left`, `right`) and scalar counters
//     (`odd_budget`, `total`); zero auxiliary containers needed.
//
// Real-World Applications:
//   - Event-log windowing: counting contiguous telemetry intervals that contain
//     an exact number of error or alert events amidst normal traffic.
//   - Manufacturing quality assurance: counting contiguous batch inspection
//     windows containing an exact defect count.
//   - Genomics and signal processing: counting sequence windows containing an
//     exact number of binary marker hits.
//
// Related LeetCode Problems:
//   - #992 Subarrays with K Different Integers [done] (same `atMost(k) - atMost(k - 1)` sliding-window pattern)
//   - #930 Binary Subarrays With Sum [new] (direct reduction after mapping odd->1 and even->0)
//   - #560 Subarray Sum Equals K [done] (alternative prefix-sum + hash-map approach for exact count)
//   - #1004 Max Consecutive Ones III [done] (budget-constrained sliding window over binary properties)
//   - #1358 Number of Substrings Containing All Three Characters [new] (subarray/substring counting via two pointers)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the number of contiguous subarrays with exactly `k` odd numbers
  // using `countAtMost(k) - countAtMost(k - 1)`.
  // Time: O(n), Space: O(1).
  int numberOfSubarrays(const std::vector<int>& nums, int k) {
    if (k < 0) {
      return 0;
    }
    return static_cast<int>(countAtMost(nums, k) - countAtMost(nums, k - 1));
  }

 private:
  // Counts contiguous subarrays containing at most `max_odds` odd integers.
  static std::int64_t countAtMost(const std::vector<int>& nums, int max_odds) {
    if (max_odds < 0) {
      return 0;
    }

    std::size_t left = 0uz;
    int odd_budget = max_odds;
    std::int64_t total = 0;

    for (std::size_t right = 0uz; right < nums.size(); ++right) {
      // Check parity via remainder != 0 so negative odd integers work too.
      if (nums[right] % 2 != 0) {
        --odd_budget;
      }

      // Shrink from the left until the window has at most `max_odds` odds.
      while (odd_budget < 0) {
        if (nums[left] % 2 != 0) {
          ++odd_budget;
        }
        ++left;
      }

      // Every suffix [start, right] for start in [left, right] is valid.
      total += static_cast<std::int64_t>((right - left) + 1uz);
    }

    return total;
  }
};

LC_PROBLEM(1248, "Count Number Of Nice Subarrays", "sliding_window", ctx) {
  ctx.test("leetcode example 1 and python build", [&] {
    Solution sol;
    ctx.expect_eq(sol.numberOfSubarrays({1, 1, 2, 1, 1}, 3), 2);
  });

  ctx.test("leetcode example 2 no odd numbers", [&] {
    Solution sol;
    ctx.expect_eq(sol.numberOfSubarrays({2, 4, 6}, 1), 0);
  });

  ctx.test("leetcode example 3 symmetric evens around odds", [&] {
    Solution sol;
    ctx.expect_eq(sol.numberOfSubarrays({2, 2, 2, 1, 2, 2, 1, 2, 2, 2}, 2), 16);
  });

  ctx.test("all odd numbers k equals 1", [&] {
    Solution sol;
    ctx.expect_eq(sol.numberOfSubarrays({1, 3, 5}, 1), 3);
  });

  ctx.test("zero odd numbers requested", [&] {
    Solution sol;
    ctx.expect_eq(sol.numberOfSubarrays({2, 4, 6}, 0), 6);
  });

  ctx.test("empty array", [&] {
    Solution sol;
    ctx.expect_eq(sol.numberOfSubarrays({}, 1), 0);
  });
}

}  // namespace
