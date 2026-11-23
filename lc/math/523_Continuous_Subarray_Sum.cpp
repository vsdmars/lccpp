// LeetCode 523: Continuous Subarray Sum
// https://leetcode.com/problems/continuous-subarray-sum/
//
// Problem:
//   Given an array of non-negative integers `nums` and an integer `k`, return
//   `true` if `nums` contains a contiguous subarray of length at least 2 whose
//   elements sum to a multiple of `k` (i.e., sum == m * k for some integer m,
//   where 0 is always a multiple of k). Otherwise return `false`.
//   Constraints: 1 <= nums.size() <= 10^5, 0 <= nums[i] <= 10^9,
//   1 <= k <= 2^31 - 1 (legacy inputs with k == 0 are also handled safely).
//
// Examples:
//   Example 1:
//     Input:  nums = [23, 2, 4, 6, 7], k = 6
//     Output: true
//     Explanation:
//       - Initialize running_sum = 0, prev_mod = 0, and seen_mods = {} to enforce a
//         subarray length of at least 2 via delayed insertion.
//       - At num = 23: running_sum = 23, curr_mod = 23 % 6 = 5 (not in {}), then
//         insert prev_mod = 0 so seen_mods = {0} and update prev_mod = 5.
//       - At num = 2: running_sum = 25, curr_mod = 25 % 6 = 1 (not in {0}), then
//         insert prev_mod = 5 so seen_mods = {0, 5} and update prev_mod = 1.
//       - At num = 4: running_sum = 29, curr_mod = 29 % 6 = 5 matches 5 in
//         seen_mods (corresponding to subarray [2, 4] summing to 6), so the
//         function returns true.
//
//   Example 2:
//     Input:  nums = [23, 2, 6, 4, 7], k = 13
//     Output: false
//     Explanation:
//       - Start with running_sum = 0, prev_mod = 0, and seen_mods = {}.
//       - Scanning 23, 2, 6, 4, 7 yields running_sum values 23, 25, 31, 35, 42 and
//         remainders curr_mod = running_sum % 13 equal to 10, 12, 5, 9, 3.
//       - Before each lookup, seen_mods holds {0}, {0, 10}, {0, 10, 12}, {0, 10,
//         12, 5}, and {0, 10, 12, 5, 9}, so seen_mods.contains(curr_mod) is false
//         at every step.
//       - All elements are processed without a modular collision at distance >= 2,
//         returning false.
//
//   Example 3:
//     Input:  nums = [10, 0, 0], k = 0
//     Output: true
//     Explanation:
//       - Initialize running_sum = 0, prev_mod = 0, and seen_mods = {}; because k
//         == 0, curr_mod is set directly to running_sum.
//       - At num = 10: running_sum = 10, curr_mod = 10 (not in {}), then seen_mods
//         becomes {0} and prev_mod becomes 10.
//       - At the first 0: running_sum = 10, curr_mod = 10 is absent from seen_mods
//         = {0} because prev_mod = 10 was delayed, then seen_mods becomes {0, 10}
//         and prev_mod = 10.
//       - At the second 0: running_sum = 10, curr_mod = 10 is found in seen_mods =
//         {0, 10} (subarray [0, 0] of length 2 sums to 0), returning true.
//
// Algorithm:
//   Prefix sums with modular congruence and delayed hash-set insertion.
//   Let P[i] be the sum of the first i elements (with P[0] = 0). A subarray
//   nums[i..j-1] of length (j - i) >= 2 has sum divisible by k if and only if:
//     (P[j] - P[i]) % k == 0  <=>  P[j] % k == P[i] % k.
//   When k == 0, divisibility reduces to P[j] == P[i] (subarray sum == 0).
//   To enforce the length >= 2 constraint without storing indices, we delay
//   inserting each prefix remainder by one iteration: `prev_mod` holds the
//   immediately preceding prefix remainder (initially P[0] = 0), while
//   `seen_mods` holds all prefix remainders at distance >= 2 before the
//   current index. If the current remainder already appears in `seen_mods`, a
//   valid subarray of length >= 2 exists.
//   Time Complexity:  O(n) average.
//   Space Complexity: O(min(n, |k|)) when k != 0, or O(n) when k == 0.
//
// Data Structures:
//   - `std::unordered_set<std::int64_t>`: stores prefix remainders from
//     positions at least 2 indices earlier for O(1) average membership checks.
//
// Real-World Applications:
//   - Packet framing and periodic boundary detection in streaming telemetry
//     where contiguous windows must align to fixed block/frame sizes.
//   - Financial batching and ledger auditing to detect contiguous sequences of
//     transactions that reconcile to exact multiples of a settlement unit.
//   - Scheduling and time-slot alignment when contiguous task durations must
//     pack evenly into fixed-size quantum cycles.
//
// Related LeetCode Problems:
//   - #560 Subarray Sum Equals K [done] (prefix-sum hash map counting exact target sums)
//   - #974 Subarray Sums Divisible by K [done] (counting all subarrays divisible by k via remainder frequencies)
//   - #1590 Make Sum Divisible by P [new] (shortest subarray removal using modular prefix sums)
//   - #325 Maximum Size Subarray Sum Equals k [done] (earliest prefix-sum index tracking)

#include "framework/lc_framework.h"

#include <cstdint>
#include <unordered_set>
#include <vector>

namespace {

class Solution {
 public:
  // Checks whether `nums` contains a contiguous subarray of length >= 2 whose
  // sum is a multiple of `k`.
  //
  // Invariant: Before checking index j (0-based), `seen_mods` contains all
  // prefix remainders P[0..j-1] % k (distance >= 2 from P[j+1]), and
  // `prev_mod` holds P[j] % k (distance 1 from P[j+1]).
  // Time: O(n) average, Space: O(min(n, |k|)).
  bool checkSubarraySum(const std::vector<int>& nums, int k) {
    // At least two elements are required to form a valid subarray.
    if (nums.size() < 2uz) {
      return false;
    }

    std::unordered_set<std::int64_t> seen_mods;
    std::int64_t running_sum = 0;
    // Represents the empty prefix P[0] = 0; delayed by one step so that a
    // single-element multiple of k cannot match P[0].
    std::int64_t prev_mod = 0;

    for (const int num : nums) {
      running_sum += num;

      // When k == 0, a "multiple of 0" means the subarray sum must equal 0,
      // which occurs iff two prefix sums at distance >= 2 are identical.
      const std::int64_t curr_mod =
          (k != 0) ? (running_sum % static_cast<std::int64_t>(k)) : running_sum;

      if (seen_mods.contains(curr_mod)) {
        return true;
      }

      // Commit the previous prefix remainder now that it will be >= 2 steps
      // behind the next iteration's prefix.
      seen_mods.insert(prev_mod);
      prev_mod = curr_mod;
    }

    return false;
  }
};

LC_PROBLEM(523, "Continuous Subarray Sum", "math", ctx) {
  Solution sol;

  ctx.test("example_1_length_two_match", [&] {
    ctx.expect_true(sol.checkSubarraySum({23, 2, 4, 6, 7}, 6));
  });

  ctx.test("example_2_full_array_match", [&] {
    ctx.expect_true(sol.checkSubarraySum({23, 2, 6, 4, 7}, 6));
  });

  ctx.test("no_valid_subarray", [&] {
    ctx.expect_false(sol.checkSubarraySum({23, 2, 6, 4, 7}, 13));
  });

  ctx.test("single_zero_rejected", [&] {
    ctx.expect_false(sol.checkSubarraySum({0}, 1));
  });

  ctx.test("single_multiple_not_length_two", [&] {
    ctx.expect_false(sol.checkSubarraySum({6}, 6));
    ctx.expect_false(sol.checkSubarraySum({1, 0}, 2));
  });

  ctx.test("consecutive_zeros_always_multiple", [&] {
    ctx.expect_true(sol.checkSubarraySum({5, 0, 0, 0}, 3));
    ctx.expect_true(sol.checkSubarraySum({10, 0, 0}, 0));
  });

  ctx.test("zero_k_without_zero_subarray", [&] {
    ctx.expect_false(sol.checkSubarraySum({1, 2, 3}, 0));
  });
}

}  // namespace
