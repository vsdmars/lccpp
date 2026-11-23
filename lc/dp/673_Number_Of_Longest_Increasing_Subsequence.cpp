// LeetCode 673: Number Of Longest Increasing Subsequence
// https://leetcode.com/problems/number-of-longest-increasing-subsequence/
//
// Problem:
//   Given an unsorted integer array `nums` (1 <= nums.size() <= 2000), return
//   the number of distinct-index strictly longest increasing subsequences
//   (LIS) of `nums`. The answer is guaranteed to fit in a 32-bit signed
//   integer.
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 3, 5, 4, 7]
//     Output: 2
//     Explanation:
//       - Initialize length = [1, 1, 1, 1, 1] and count = [1, 1, 1, 1, 1].
//       - For i = 1 (3) and i = 2 (5), strictly smaller predecessors extend
//         length[1] = 2 (count 1) and length[2] = 3 (count 1).
//       - At i = 3 (4), predecessors j = 0 (1) and j = 1 (3) update length[3] =
//         length[1] + 1 = 3 with count[3] = 1, while j = 2 (5) is skipped.
//       - At i = 4 (7), j = 2 (5) sets length[4] = 4 and count[4] = 1, and j = 3
//         (4) ties length 4 to add count[3] = 1 so count[4] = 2.
//       - The global maximum length is max_len = 4 at index 4 alone, so summing
//         count[i] for length[i] == 4 returns 2.
//
//   Example 2:
//     Input:  nums = [2, 2, 2, 2, 2]
//     Output: 5
//     Explanation:
//       - Initialize length = [1, 1, 1, 1, 1] and count = [1, 1, 1, 1, 1].
//       - Every pair j < i has nums[j] >= nums[i] (2 >= 2), so no inner transition
//         fires and all length[i] and count[i] stay 1.
//       - With max_len = 1, all five indices match length[i] == 1, summing
//         count[0..4] to return 5.
//
//   Example 3:
//     Input:  nums = []
//     Output: 0
//     Explanation:
//       - Check nums.empty() at the start of findNumberOfLIS.
//       - Return 0 immediately because an empty input contains no subsequences.
//
// Algorithm:
//   Bottom-up dynamic programming tracking both the best length and the
//   multiplicity at every ending index.
//   For each index `i`:
//     - `length[i]`: length of the longest strictly increasing subsequence
//       ending at `nums[i]` (initialized to 1, representing `[nums[i]]`).
//     - `count[i]`: number of strictly increasing subsequences of length
//       `length[i]` ending at `nums[i]` (initialized to 1).
//   For each `i` from 1 to n - 1, inspect every predecessor `j < i` with
//   `nums[j] < nums[i]`:
//     - If `length[j] + 1 > length[i]`, we found a strictly longer chain:
//       update `length[i] = length[j] + 1` and reset `count[i] = count[j]`.
//     - Else if `length[j] + 1 == length[i]`, we found additional distinct
//       chains tying the current best length: add `count[i] += count[j]`.
//   Finally, compute `max_len = max(length)` and sum `count[i]` over all `i`
//   achieving `length[i] == max_len`.
//   Time Complexity: O(n^2).
//   Space Complexity: O(n) for the `length` and `count` arrays.
//
// Data Structures:
//   - Two parallel 1D DP vectors (`std::vector<int>` `length` and `count`):
//     pair the optimal subsequence length with its exact path count at each
//     ending element.
//
// Real-World Applications:
//   - Counting alternative optimal dependency chains in DAG task scheduling
//     and critical-path analysis (identifying how many tied critical paths
//     exist in a build or project graph).
//   - Measuring tie multiplicity in bioinformatics sequence ordering and
//     time-series monotonic trend extraction.
//
// Related LeetCode Problems:
//   - #300 Longest Increasing Subsequence [done] (computes only the maximum LIS length)
//   - #354 Russian Doll Envelopes [done] (2D LIS via sort + patience sorting)
//   - #368 Largest Divisible Subset [done] (predecessor-chain DP with divisibility relation)
//   - #674 Longest Continuous Increasing Subsequence [done] (contiguous subarray variant)
//   - #1048 Longest String Chain [done] (DAG longest-path DP on predecessor relations)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Counts how many strictly longest increasing subsequences exist in `nums`.
  // Time: O(n^2), Space: O(n).
  int findNumberOfLIS(const std::vector<int>& nums) {
    if (nums.empty()) {
      return 0;
    }

    const std::size_t n = nums.size();
    // Every single element forms an increasing subsequence of length 1 in
    // exactly 1 way.
    std::vector<int> length(n, 1);
    std::vector<int> count(n, 1);

    for (std::size_t i = 1uz; i < n; ++i) {
      for (std::size_t j = 0uz; j < i; ++j) {
        if (nums[j] >= nums[i]) {
          continue;
        }
        const int candidate_len = length[j] + 1;
        if (candidate_len > length[i]) {
          // Extending from `j` strictly improves the LIS ending at `i`.
          length[i] = candidate_len;
          count[i] = count[j];
        } else if (candidate_len == length[i]) {
          // Extending from `j` matches the best length; accumulate its ways.
          count[i] += count[j];
        }
      }
    }

    // Sum the counts of all indices that achieve the global maximum LIS length.
    const int max_len = std::ranges::max(length);
    int total_ways = 0;
    for (std::size_t i = 0uz; i < n; ++i) {
      if (length[i] == max_len) {
        total_ways += count[i];
      }
    }
    return total_ways;
  }
};

LC_PROBLEM(673, "Number Of Longest Increasing Subsequence", "dp", ctx) {
  ctx.test("example_1_two_lis", [&] {
    Solution sol;
    // [1, 3, 5, 7] and [1, 3, 4, 7]
    ctx.expect_eq(sol.findNumberOfLIS({1, 3, 5, 4, 7}), 2);
  });

  ctx.test("example_2_all_equal", [&] {
    Solution sol;
    // Every element alone is an LIS of length 1.
    ctx.expect_eq(sol.findNumberOfLIS({2, 2, 2, 2, 2}), 5);
  });

  ctx.test("python_build_cases", [&] {
    Solution sol;
    ctx.expect_eq(sol.findNumberOfLIS({6, 7, 8, 2, 3, 4}), 2);
    ctx.expect_eq(sol.findNumberOfLIS({1, 3, 5, 4}), 2);
    ctx.expect_eq(sol.findNumberOfLIS({1, 3, 5}), 1);
    ctx.expect_eq(sol.findNumberOfLIS({1, 2, 4, 3, 5, 4, 7, 2}), 3);
    ctx.expect_eq(sol.findNumberOfLIS({1, 3, 5, 7}), 1);
  });

  ctx.test("edge_cases_empty_single_decreasing", [&] {
    Solution sol;
    ctx.expect_eq(sol.findNumberOfLIS({}), 0);
    ctx.expect_eq(sol.findNumberOfLIS({42}), 1);
    ctx.expect_eq(sol.findNumberOfLIS({5, 4, 3, 2, 1}), 5);
  });
}

}  // namespace
