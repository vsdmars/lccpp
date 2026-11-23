// LeetCode 152: Maximum Product Subarray
// https://leetcode.com/problems/maximum-product-subarray/
//
// Problem:
//   Given an integer array `nums` containing at least one number, find the
//   contiguous non-empty subarray that has the largest product and return that
//   product. Test cases are generated so that the final answer fits in a 32-bit
//   signed integer.
//   Constraints: 1 <= nums.length <= 2 * 10^4; -10 <= nums[i] <= 10.
//
// Algorithm:
//   Single-pass O(1)-space Dynamic Programming tracking both maximum and
//   minimum subarray products ending at the current index:
//   - Unlike Maximum Subarray Sum (Kadane's), multiplying by a negative number
//     flips signs: the smallest (most negative) product ending at i - 1 can
//     become the largest positive product at i when multiplied by nums[i] < 0.
//   - Maintain two rolling states for subarrays ending at index i:
//       * `max_prod` = largest product of a subarray ending at i.
//       * `min_prod` = smallest product of a subarray ending at i.
//   - At each element `val = nums[i]`, compute candidates `max_prod * val` and
//     `min_prod * val`, then update:
//       * `max_prod = max(val, max_prod * val, min_prod * val)`
//       * `min_prod = min(val, max_prod * val, min_prod * val)`
//       * `best = max(best, max_prod)`
//   - Including `val` itself cleanly handles restarting after a 0 or dropping a
//     harmful prefix (e.g. starting fresh at -100 in [-3, 1, -100]).
//   - Time Complexity: O(n).
//   - Space Complexity: O(1).
//
// Data Structures:
//   - Three scalar 64-bit integer DP states (`max_prod`, `min_prod`, `best`)
//     for constant space and protection against intermediate 32-bit overflow.
//
// Real-World Applications:
//   - Finding the peak compounded return window in multiplicative financial
//     series (growth factors / exchange-rate chains).
//   - Signal-amplification chain analysis where stage gains can invert phase
//     (negative gain multipliers) or attenuate to zero.
//   - Probabilistic sequence modeling (finding highest-likelihood contiguous
//     segments before log-transforming).
//
// Related LeetCode Problems:
//   - #121 Best Time To Buy And Sell Stock [done] (Kadane's single-pass state compression)
//   - #53 Maximum Subarray [done] (additive counterpart using only a single running maximum)
//   - #628 Maximum Product of Three Numbers [new] (same sign-flip principle tracking max and min extremes)
//   - #713 Subarray Product Less Than K [new] (contiguous subarray product with positive integers)
//   - #1567 Maximum Length of Subarray With Positive Product [new] (tracks positive/negative product lengths)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the largest product of any non-empty contiguous subarray of `nums`.
  //
  // Invariant: after step i, `max_prod` and `min_prod` equal the maximum and
  // minimum products of all subarrays ending at index i, and `best` is the
  // global maximum across all subarrays within nums[0..i].
  // Time: O(n), Space: O(1).
  int maxProduct(const std::vector<int>& nums) {
    if (nums.empty()) {
      return 0;
    }

    // Use 64-bit integers internally to avoid signed 32-bit overflow on
    // intermediate products that are later discarded or flipped.
    std::int64_t best = nums[0];
    std::int64_t max_prod = nums[0];
    std::int64_t min_prod = nums[0];

    for (std::size_t i = 1uz; i < nums.size(); ++i) {
      const std::int64_t val = nums[i];
      // Snapshot both extended products before mutating `max_prod`/`min_prod`.
      const std::int64_t ext_max = max_prod * val;
      const std::int64_t ext_min = min_prod * val;

      // Either start a new subarray at `val` or extend the best/worst previous
      // subarray (a negative `val` swaps the roles of max and min).
      max_prod = std::max({val, ext_max, ext_min});
      min_prod = std::min({val, ext_max, ext_min});

      best = std::max(best, max_prod);
    }

    return static_cast<int>(best);
  }
};

LC_PROBLEM(152, "Maximum Product Subarray", "dp", ctx) {
  Solution sol;

  ctx.test("example_1_standard", [&] {
    ctx.expect_eq(sol.maxProduct({2, 3, -2, 4}), 6);
  });

  ctx.test("example_2_with_zero", [&] {
    ctx.expect_eq(sol.maxProduct({-2, 0, -1}), 0);
  });

  ctx.test("python_build_four_negatives", [&] {
    // Whole array has 4 negatives -> product = 3*1*2*2*3*2*4*1 = 288.
    ctx.expect_eq(sol.maxProduct({3, -1, -2, 2, 3, -2, 4, -1}), 288);
  });

  ctx.test("python_docstring_cases", [&] {
    // [-2, 2, 3, -2, 4] -> 96.
    ctx.expect_eq(sol.maxProduct({3, -1, -2, 2, 3, -2, 4}), 96);
    // [-3, 1, -100] -> 300 (preserving current value as candidate min/max).
    ctx.expect_eq(sol.maxProduct({-3, 1, -100}), 300);
  });

  ctx.test("single_element", [&] {
    ctx.expect_eq(sol.maxProduct({-5}), -5);
    ctx.expect_eq(sol.maxProduct({0}), 0);
    ctx.expect_eq(sol.maxProduct({7}), 7);
  });

  ctx.test("zeros_resetting_segments", [&] {
    ctx.expect_eq(sol.maxProduct({0, 2}), 2);
    ctx.expect_eq(sol.maxProduct({-2, 0, -3, -4}), 12);
  });
}

}  // namespace
