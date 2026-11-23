// LeetCode 992: Subarrays With K Different Integers
// https://leetcode.com/problems/subarrays-with-k-different-integers/
//
// Problem:
//   Given an integer array `nums` and an integer `k`, return the number of
//   contiguous subarrays of `nums` that contain exactly `k` distinct integers.
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 2, 1, 2, 3], k = 2
//     Output: 7
//     Explanation:
//       - In countAtMost(nums, 2), right = 0..3 keep left = 0 with freq keys {1, 2}
//         (adding 1 + 2 + 3 + 4 = 10), and right = 4 (3) shrinks left to 3 ({2, 3},
//         adding 2) for a total of 12.
//       - In countAtMost(nums, 1), adjacent elements differ at every step so left
//         advances to right at each index, adding 1 + 1 + 1 + 1 + 1 = 5.
//       - Subtracting countAtMost(nums, 2) - countAtMost(nums, 1) gives 12 - 5 = 7.
//
//   Example 2:
//     Input:  nums = [1, 2, 1, 3, 4], k = 3
//     Output: 3
//     Explanation:
//       - In countAtMost(nums, 3), right = 0..3 keep left = 0 with at most three
//         distinct keys {1, 2, 3} (adding 1 + 2 + 3 + 4 = 10), and right = 4 (4)
//         shrinks left to 2 ({1, 3, 4}, adding 3) for a total of 13.
//       - In countAtMost(nums, 2), right = 0..2 add 1 + 2 + 3 = 6, right = 3 (3)
//         shrinks left to 2 ({1, 3}, adding 2), and right = 4 (4) shrinks left to 3
//         ({3, 4}, adding 2), totaling 10.
//       - Subtracting 13 - 10 returns 3.
//
//   Example 3:
//     Input:  nums = [1, 1, 1, 1], k = 2
//     Output: 0
//     Explanation:
//       - In countAtMost(nums, 2), freq.size() stays 1 <= 2 so left remains 0 and
//         all 1 + 2 + 3 + 4 = 10 subarrays are counted.
//       - In countAtMost(nums, 1), freq.size() also stays 1 <= 1 so left remains 0
//         and the same 10 subarrays are counted.
//       - Subtracting 10 - 10 yields 0 because no subarray contains 2 distinct
//         integers.
//
// Algorithm:
//   Exact-K via At-Most-K inclusion-exclusion:
//     exact(k) = atMost(k) - atMost(k - 1).
//   Counting subarrays with *at most* `m` distinct integers is monotonic as
//   the window expands, so a single sliding window suffices:
//     - Expand `right` and increment the frequency of `nums[right]`.
//     - While the map holds more than `m` distinct keys, decrement the
//       frequency of `nums[left]`, erase the key if its count hits 0, and
//       increment `left`.
//     - Every subarray ending at `right` with start in `[left, right]` has at
//       most `m` distinct integers, contributing `(right - left) + 1` valid
//       subarrays.
//   Subtracting `atMost(k - 1)` from `atMost(k)` leaves only subarrays with
//   exactly `k` distinct integers.
//   Time Complexity: O(n) average, where n = nums.size() (two linear passes).
//   Space Complexity: O(k) for the frequency hash map.
//
// Data Structures:
//   - `std::unordered_map<int, int>` tracking active element frequencies inside
//     the sliding window so distinct-count queries are O(1) via `size()`.
//
// Real-World Applications:
//   - Clickstream and user-session analytics: counting contiguous browsing
//     windows where a user interacted with an exact number of distinct product
//     categories or features.
//   - Network intrusion detection: counting packet windows that contact
//     exactly `k` unique destination hosts or ports.
//   - NLP and text mining: counting token spans containing an exact vocabulary
//     diversity count.
//
// Related LeetCode Problems:
//   - #1248 Count Number of Nice Subarrays [done] (same `atMost(k) - atMost(k - 1)` reduction for k odd numbers)
//   - #340 Longest Substring with At Most K Distinct Characters [done] (at-most-k distinct sliding window maximizing length)
//   - #159 Longest Substring with At Most Two Distinct Characters [done] (special case k = 2 of distinct-element sliding window)
//   - #930 Binary Subarrays With Sum [new] (exact-sum subarray count via `atMost(goal) - atMost(goal - 1)`)
//   - #904 Fruit Into Baskets [new] (at-most-2 distinct elements sliding window)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the number of contiguous subarrays with exactly `k` distinct
  // integers using `countAtMost(k) - countAtMost(k - 1)`.
  // Time: O(n) average, Space: O(k).
  int subarraysWithKDistinct(const std::vector<int>& nums, int k) {
    if (k <= 0) {
      return 0;
    }
    return static_cast<int>(countAtMost(nums, k) - countAtMost(nums, k - 1));
  }

 private:
  // Counts contiguous subarrays containing at most `max_distinct` distinct
  // values via a standard shrinking sliding window.
  static std::int64_t countAtMost(const std::vector<int>& nums,
                                  int max_distinct) {
    if (max_distinct <= 0) {
      return 0;
    }

    const auto limit = static_cast<std::size_t>(max_distinct);
    std::unordered_map<int, int> freq;
    std::size_t left = 0uz;
    std::int64_t total = 0;

    for (std::size_t right = 0uz; right < nums.size(); ++right) {
      ++freq[nums[right]];

      // Shrink from the left until the window has at most `limit` distinct
      // integers; erase zero-count keys so `freq.size()` is exact.
      while (freq.size() > limit) {
        const auto it = freq.find(nums[left]);
        --it->second;
        if (it->second == 0) {
          freq.erase(it);
        }
        ++left;
      }

      // All subarrays [start, right] with start in [left, right] are valid.
      total += static_cast<std::int64_t>((right - left) + 1uz);
    }

    return total;
  }
};

LC_PROBLEM(992, "Subarrays With K Different Integers", "sliding_window", ctx) {
  ctx.test("leetcode example 1 and python build", [&] {
    Solution sol;
    ctx.expect_eq(sol.subarraysWithKDistinct({1, 2, 1, 2, 3}, 2), 7);
  });

  ctx.test("leetcode example 2", [&] {
    Solution sol;
    ctx.expect_eq(sol.subarraysWithKDistinct({1, 2, 1, 3, 4}, 3), 3);
  });

  ctx.test("all identical elements k equals 1", [&] {
    Solution sol;
    ctx.expect_eq(sol.subarraysWithKDistinct({1, 1, 1, 1}, 1), 10);
  });

  ctx.test("k exceeds total distinct values", [&] {
    Solution sol;
    ctx.expect_eq(sol.subarraysWithKDistinct({1, 1, 1, 1}, 2), 0);
  });

  ctx.test("single element", [&] {
    Solution sol;
    ctx.expect_eq(sol.subarraysWithKDistinct({7}, 1), 1);
  });

  ctx.test("empty array", [&] {
    Solution sol;
    ctx.expect_eq(sol.subarraysWithKDistinct({}, 1), 0);
  });
}

}  // namespace
