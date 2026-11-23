// LeetCode 560: Subarray Sum Equals K
// https://leetcode.com/problems/subarray-sum-equals-k/
//
// Problem:
//   Given an integer array `nums` (which may contain negative values and zeros)
//   and an integer `k`, return the total number of non-empty contiguous
//   subarrays whose elements sum to `k`.
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 2, 3], k = 3
//     Output: 2
//     Explanation:
//       - Initialize prefix_counts = {0: 1}, prefix_sum = 0, and total_subarrays =
//         0.
//       - At value = 1: prefix_sum becomes 1, needed = 1 - 3 = -2 is absent from
//         prefix_counts, and prefix_counts[1] becomes 1.
//       - At value = 2: prefix_sum becomes 3, needed = 3 - 3 = 0 matches count 1 in
//         prefix_counts (total_subarrays = 1, subarray [1, 2]), and
//         prefix_counts[3] becomes 1.
//       - At value = 3: prefix_sum becomes 6, needed = 6 - 3 = 3 matches count 1 in
//         prefix_counts (total_subarrays = 2, subarray [3]), and prefix_counts[6]
//         becomes 1.
//
//   Example 2:
//     Input:  nums = [2, 3, -3, 3], k = 3
//     Output: 3
//     Explanation:
//       - Initialize prefix_counts = {0: 1}; processing 2 updates prefix_sum = 2,
//         finds no match for needed = -1, and sets prefix_counts[2] = 1.
//       - Processing 3 updates prefix_sum = 5, finds prefix_counts[5 - 3] =
//         prefix_counts[2] = 1 (total_subarrays = 1), and sets prefix_counts[5] =
//         1.
//       - Processing -3 brings prefix_sum back to 2, finds no match for needed =
//         -1, and increments prefix_counts[2] to 2.
//       - Processing the final 3 updates prefix_sum = 5 and finds prefix_counts[2]
//         = 2 (for [3, -3, 3] and [3]), raising total_subarrays to 1 + 2 = 3.
//
//   Example 3:
//     Input:  nums = [0, 0, 0], k = 0
//     Output: 6
//     Explanation:
//       - Initialize prefix_counts = {0: 1}; at index 0 (value = 0), prefix_sum = 0
//         queries needed = 0 before incrementing, adding 1 to total_subarrays and
//         updating prefix_counts[0] to 2.
//       - At index 1 (value = 0), prefix_sum = 0 finds prefix_counts[0] = 2
//         (total_subarrays = 3), then increments prefix_counts[0] to 3.
//       - At index 2 (value = 0), prefix_sum = 0 finds prefix_counts[0] = 3
//         (total_subarrays = 6), then increments prefix_counts[0] to 4 and returns
//         6.
//
// Algorithm:
//   Prefix Sum + Frequency Hash Map:
//     Let P[i] = nums[0] + ... + nums[i - 1] with P[0] = 0. A contiguous
//     subarray nums[l..r] sums to k iff P[r + 1] - P[l] == k, which rearranges
//     to P[l] == P[r + 1] - k.
//     As we scan `nums` left-to-right maintaining the running prefix sum
//     `prefix_sum`, we query `prefix_counts.find(prefix_sum - k)` to count how
//     many earlier prefix sums form a valid subarray ending at the current
//     index, then increment `prefix_counts[prefix_sum]`. Seeding
//     `prefix_counts[0] = 1` accounts for subarrays starting at index 0.
//     Unlike a sliding window, this works seamlessly with negative numbers.
//   Time Complexity:  O(n) expected time (single linear scan).
//   Space Complexity: O(n) worst-case space for distinct prefix sums.
//
// Data Structures:
//   - std::unordered_map<std::int64_t, int> prefix_counts: stores how many
//     times each 64-bit prefix sum has occurred before the current index.
//
// Real-World Applications:
//   - Financial ledger and fraud detection identifying contiguous transaction
//     windows matching a target net balance or zero-sum wash trades.
//   - Time-series anomaly detection locating intervals whose cumulative metric
//     delta hits a threshold.
//   - Bioinformatics sequence analysis finding contiguous genomic regions with
//     a target GC-skew or cumulative score.
//
// Related LeetCode Problems:
//   - #1 Two Sum [done] (complement lookup in a hash map during a single pass)
//   - #523 Continuous Subarray Sum [done] (prefix sums modulo k in a hash map)
//   - #974 Subarray Sums Divisible by K [done] (prefix-sum remainder frequency counting)
//   - #325 Maximum Size Subarray Sum Equals k [done] (earliest index per prefix sum)
//   - #525 Contiguous Array [done] (transforming 0/1 balance into zero-sum prefix matching)

#include "framework/lc_framework.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Counts all contiguous subarrays of `nums` whose sum equals `k`.
  // Time: O(n) expected, Space: O(n).
  int subarraySum(const std::vector<int>& nums, int k) {
    // Maps each prefix sum seen so far to the number of times it occurred.
    // Seed sum 0 with frequency 1 to represent the empty prefix before index 0.
    std::unordered_map<std::int64_t, int> prefix_counts;
    prefix_counts.reserve(nums.size() + 1uz);
    prefix_counts[0] = 1;

    std::int64_t prefix_sum = 0;
    int total_subarrays = 0;

    for (int value : nums) {
      prefix_sum += value;

      // Any prior prefix sum equal to (prefix_sum - k) defines a valid
      // contiguous subarray ending at the current element.
      const std::int64_t needed = prefix_sum - static_cast<std::int64_t>(k);
      if (const auto it = prefix_counts.find(needed);
          it != prefix_counts.end()) {
        total_subarrays += it->second;
      }

      // Record the current prefix sum AFTER querying so k == 0 does not match
      // an empty subarray at the current index.
      ++prefix_counts[prefix_sum];
    }

    return total_subarrays;
  }
};

LC_PROBLEM(560, "Subarray Sum Equals K", "hash", ctx) {
  ctx.test("example_ones", [&] {
    Solution sol;
    ctx.expect_eq(sol.subarraySum({1, 1, 1}, 2), 2);
  });

  ctx.test("example_1_2_3", [&] {
    Solution sol;
    ctx.expect_eq(sol.subarraySum({1, 2, 3}, 3), 2);
  });

  ctx.test("negative_numbers_canceling_out", [&] {
    Solution sol;
    // Subarrays summing to 3: [3] at idx 1, [3, -3, 3], [3] at idx 3.
    ctx.expect_eq(sol.subarraySum({2, 3, -3, 3}, 3), 3);
    ctx.expect_eq(sol.subarraySum({1, -1, 0}, 0), 3);
  });

  ctx.test("all_zeros_target_zero", [&] {
    Solution sol;
    // n * (n + 1) / 2 = 3 * 4 / 2 = 6 non-empty subarrays.
    ctx.expect_eq(sol.subarraySum({0, 0, 0}, 0), 6);
  });

  ctx.test("empty_and_single_element", [&] {
    Solution sol;
    ctx.expect_eq(sol.subarraySum({}, 0), 0);
    ctx.expect_eq(sol.subarraySum({5}, 5), 1);
    ctx.expect_eq(sol.subarraySum({5}, 0), 0);
  });
}

}  // namespace
