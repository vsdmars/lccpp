// LeetCode 325: Maximum Size Subarray Sum Equals K
// https://leetcode.com/problems/maximum-size-subarray-sum-equals-k/
//
// Problem:
//   Given an integer array `nums` (which may contain positive, negative, and
//   zero values) and a target integer `k`, return the maximum length of a
//   contiguous subarray whose elements sum to `k`. If no such subarray exists,
//   return `0`.
//
// Algorithm:
//   Use running prefix sums paired with a hash map storing the *earliest*
//   index where each prefix sum value was observed. Seed the map with
//   `{0: -1}` to account for subarrays starting at index 0.
//   At index `i` with running sum `prefix = sum(nums[0..i])`, any earlier
//   index `j` with `prefix_sum[j] == prefix - k` yields a valid subarray
//   `nums[j+1 .. i]` of sum `k` and length `i - j`. To maximize length, we
//   want the smallest possible `j`, so we insert `prefix` into the map only on
//   its first occurrence (`try_emplace`). Running sums are tracked in
//   `std::int64_t` to avoid 32-bit overflow during `prefix - k`.
//   Time Complexity: O(N) average time in a single pass.
//   Space Complexity: O(N) to store up to N + 1 distinct prefix sums.
//
// Data Structures:
//   - `std::unordered_map<std::int64_t, int>`: maps each distinct prefix sum
//     to its earliest occurrence index (`-1` for the empty prefix).
//
// Real-World Applications:
//   - Financial ledger and cash-flow analysis: finding the longest balanced
//     or target-net-change trading window across positive/negative deltas.
//   - Telemetry and energy-grid monitoring: identifying the longest time
//     interval whose cumulative net power or resource drift hits a threshold.
//   - Bioinformatics sequence profiling: locating the longest genomic segment
//     matching an exact net GC-skew or score balance.
//
// Related LeetCode Problems:
//   - #560 Subarray Sum Equals K [done] (counts all subarrays summing to k using prefix-sum frequencies)
//   - #525 Contiguous Array [done] (special case k = 0 after mapping 0 -> -1)
//   - #523 Continuous Subarray Sum [done] (earliest prefix sum modulo k)
//   - #209 Minimum Size Subarray Sum [done] (sliding window when all elements are positive)
//   - #363 Max Sum of Rectangle No Larger Than K [done] (2D prefix sums with ordered search)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the length of the longest contiguous subarray of `nums` that sums
  // to `k`, or 0 if no such subarray exists.
  // Time: O(N) average, Space: O(N).
  int maxSubArrayLen(const std::vector<int>& nums, int k) {
    // Maps prefix sum -> earliest index where that prefix sum occurred.
    // The empty prefix before index 0 has sum 0 at virtual index -1.
    std::unordered_map<std::int64_t, int> first_index;
    first_index.reserve(nums.size() + 1uz);
    first_index.emplace(0, -1);

    std::int64_t prefix = 0;
    const auto target = static_cast<std::int64_t>(k);
    int max_len = 0;

    for (std::size_t i = 0uz; i < nums.size(); ++i) {
      prefix += nums[i];
      const auto idx = static_cast<int>(i);

      // If `prefix - k` was seen earlier at `j`, then `nums[j+1 .. i]` sums to
      // `k` with length `i - j`.
      if (const auto it = first_index.find(prefix - target);
          it != first_index.end()) {
        max_len = std::max(max_len, idx - it->second);
      }

      // Only record the earliest index for `prefix` so future lookups achieve
      // the maximum possible subarray length.
      first_index.try_emplace(prefix, idx);
    }

    return max_len;
  }
};

LC_PROBLEM(325, "Maximum Size Subarray Sum Equals K", "hash", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    ctx.expect_eq(sol.maxSubArrayLen({1, -1, 5, -2, 3}, 3), 4);
  });

  ctx.test("example_2", [&] {
    Solution sol;
    ctx.expect_eq(sol.maxSubArrayLen({-2, -1, 2, 1}, 1), 2);
  });

  ctx.test("empty_and_no_match", [&] {
    Solution sol;
    ctx.expect_eq(sol.maxSubArrayLen({}, 0), 0);
    ctx.expect_eq(sol.maxSubArrayLen({1, 2, 3}, 7), 0);
  });

  ctx.test("all_zeros_and_negative_target", [&] {
    Solution sol;
    ctx.expect_eq(sol.maxSubArrayLen({0, 0, 0, 0}, 0), 4);
    ctx.expect_eq(sol.maxSubArrayLen({-1, -2, -3, 1}, -5), 4);
  });

  ctx.test("large_values_avoid_32bit_overflow", [&] {
    Solution sol;
    ctx.expect_eq(sol.maxSubArrayLen({2000000000, -2000000000, 1}, -2000000000),
                  1);
    ctx.expect_eq(sol.maxSubArrayLen({2000000000, 2000000000, -2000000000},
                                     2000000000),
                  3);
  });
}

}  // namespace
