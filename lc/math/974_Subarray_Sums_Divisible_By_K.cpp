// LeetCode 974: Subarray Sums Divisible By K
// https://leetcode.com/problems/subarray-sums-divisible-by-k/
//
// Problem:
//   Given an integer array `nums` (which may contain negative values, zero,
//   and positive values) and a positive integer `k`, return the total number
//   of non-empty contiguous subarrays whose sum is divisible by `k`.
//   Constraints: 1 <= nums.length <= 30000, -10000 <= nums[i] <= 10000,
//   2 <= k <= 10000.
//
// Algorithm:
//   Prefix sums modulo k with frequency counting.
//   Let P[i] be the sum of the first i elements (with P[0] = 0). A contiguous
//   subarray nums[l..r-1] has a sum divisible by k iff:
//       (P[r] - P[l]) % k == 0  <=>  P[r] mod k == P[l] mod k
//   where `mod k` denotes the canonical non-negative remainder in [0, k - 1].
//   Because C++'s `%` operator preserves the sign of the dividend (unlike
//   Python's floor modulo), we normalize each running remainder via:
//       rem = (((rem + num) % k) + k) % k
//   As we scan left to right, every earlier prefix sharing the same canonical
//   remainder `rem` forms a valid divisible subarray ending at the current
//   index. Initializing `remainder_counts[0] = 1` accounts for subarrays that
//   start at index 0.
//   Time Complexity:  O(N) where N = nums.size().
//   Space Complexity: O(k) for the direct-indexed remainder frequency table.
//
// Data Structures:
//   - std::vector<int> remainder_counts(k, 0): direct-addressed frequency
//     bucket storing how many prefix sums seen so far had each remainder in
//     [0, k - 1], giving guaranteed O(1) lookup/update without hashing overhead.
//
// Real-World Applications:
//   - Financial reconciliation & batch auditing: detecting contiguous ledger
//     transaction windows whose net delta is an exact multiple of a settlement
//     lot size or currency unit.
//   - Telemetry & signal processing: identifying contiguous time intervals
//     whose cumulative phase/offset aligns with a periodic frame boundary.
//   - Database query & streaming analytics: online counting of sliding windows
//     satisfying modular conservation constraints in a single O(N) pass.
//
// Related LeetCode Problems:
//   - #523 Continuous Subarray Sum [done] (checking if any length >= 2 subarray sum is a multiple of k)
//   - #560 Subarray Sum Equals K [done] (prefix-sum frequency map for exact target sum k)
//   - #1010 Pairs of Songs With Total Durations Divisible by 60 [done] (modular remainder counting for pairs)
//   - #1590 Make Sum Divisible by P [new] (minimum subarray removal using prefix sums modulo p)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Counts the number of non-empty contiguous subarrays in `nums` whose sum
  // is divisible by `k`.
  // Time: O(N), Space: O(k).
  [[nodiscard]] int subarraysDivByK(const std::vector<int>& nums, int k) const {
    if (k <= 0) {
      return 0;
    }

    // remainder_counts[r] holds the number of prefixes seen so far with
    // canonical remainder r in [0, k - 1]. Empty prefix has sum 0 => rem 0.
    std::vector<int> remainder_counts(static_cast<std::size_t>(k), 0);
    remainder_counts[0uz] = 1;

    int rem = 0;
    int total_subarrays = 0;

    for (const int num : nums) {
      // Normalize modulo into [0, k - 1] so negative running sums map to the
      // same congruence class as positive equivalents.
      rem = (((rem + num) % k) + k) % k;
      const auto bucket = static_cast<std::size_t>(rem);

      // Every prior prefix with the same remainder closes a valid subarray here.
      total_subarrays += remainder_counts[bucket];
      ++remainder_counts[bucket];
    }

    return total_subarrays;
  }
};

LC_PROBLEM(974, "Subarray Sums Divisible By K", "math", ctx) {
  const Solution sol;

  ctx.test("example_1_mixed_signs", [&] {
    ctx.expect_eq(sol.subarraysDivByK({4, 5, 0, -2, -3, 1}, 5), 7);
  });

  ctx.test("example_2_single_not_divisible", [&] {
    ctx.expect_eq(sol.subarraysDivByK({5}, 9), 0);
  });

  ctx.test("negative_prefix_normalization", [&] {
    // Valid subarrays for k = 2: [2] (sum 2) and [-1, 2, 9] (sum 10).
    ctx.expect_eq(sol.subarraysDivByK({-1, 2, 9}, 2), 2);
  });

  ctx.test("all_zeros", [&] {
    // All 3 * 4 / 2 = 6 non-empty subarrays have sum 0.
    ctx.expect_eq(sol.subarraysDivByK({0, 0, 0}, 5), 6);
  });

  ctx.test("all_negative_multiples", [&] {
    // Every non-empty subarray of [-3, -6, -9] is divisible by 3 => 6.
    ctx.expect_eq(sol.subarraysDivByK({-3, -6, -9}, 3), 6);
  });
}

}  // namespace
