// LeetCode 53: Maximum Subarray
// https://leetcode.com/problems/maximum-subarray/
//
// Problem:
//   Given a non-empty integer array `nums`, find the contiguous non-empty
//   subarray with the largest sum and return its sum.
//
// Examples:
//   Example 1:
//     Input:  nums = [-2, 1, -3, 4, -1, 2, 1, -5, 4]
//     Output: 6
//     Explanation:
//       - Initialize `prefix_sum = 0`, `min_prefix = 0`, and `best` to the lowest
//         64-bit integer.
//       - At index 0 (`-2`), `prefix_sum` becomes `-2`, `best` updates to `-2 - 0 =
//         -2`, and `min_prefix` updates to `-2`.
//       - At indices 1 and 2 (`1`, `-3`), `prefix_sum` becomes `-1` then `-4`,
//         updating `best` to `-1 - (-2) = 1` and `min_prefix` to `-4`.
//       - At indices 3 through 6 (`4`, `-1`, `2`, `1`), `prefix_sum` reaches `0`,
//         `-1`, `1`, and `2`, updating `best` via `prefix_sum - min_prefix` (`2 -
//         (-4) = 6`) while `min_prefix` stays `-4`.
//       - At indices 7 and 8 (`-5`, `4`), `prefix_sum` becomes `-3` then `1`,
//         giving candidate sums `1` and `5` without exceeding `best = 6`, so `6` is
//         returned.
//
//   Example 2:
//     Input:  nums = [5, 4, -1, 7, 8]
//     Output: 23
//     Explanation:
//       - Initialize `prefix_sum = 0`, `min_prefix = 0`, and `best` to the lowest
//         64-bit integer.
//       - At indices 0 through 2 (`5`, `4`, `-1`), `prefix_sum` becomes `5`, `9`,
//         and `8` while `min_prefix` remains `0`, updating `best` to `9`.
//       - At indices 3 and 4 (`7`, `8`), `prefix_sum` increases to `15` and `23`,
//         updating `best` to `15 - 0 = 15` and finally returning `23`.
//
//   Example 3:
//     Input:  nums = [-5, -2, -8, -1, -4]
//     Output: -1
//     Explanation:
//       - Initialize `prefix_sum = 0`, `min_prefix = 0`, and `best` to the lowest
//         64-bit integer.
//       - At index 0 (`-5`), `prefix_sum = -5` updates `best = -5 - 0 = -5` before
//         `min_prefix` updates to `-5`; at index 1 (`-2`), `prefix_sum = -7`
//         improves `best` to `-7 - (-5) = -2` and sets `min_prefix = -7`.
//       - At index 2 (`-8`), `prefix_sum = -15` leaves `best = -2` and updates
//         `min_prefix = -15`.
//       - At index 3 (`-1`), `prefix_sum = -16` updates `best` to `-16 - (-15) =
//         -1` and `min_prefix` to `-16`; index 4 (`-4`) yields `-20 - (-16) = -4`,
//         so `-1` is returned.
//
// Algorithm:
//   1. Prefix-sum minimum tracking (`maxSubArray`, equivalent to Kadane's
//      algorithm): the sum of any subarray `nums[l..r]` equals
//      `prefix[r] - prefix[l - 1]` (with `prefix[-1] = 0`). Scanning left to
//      right, we maintain the running `prefix_sum` and the smallest prefix sum
//      `min_prefix` seen strictly before the current index. Updating
//      `best = max(best, prefix_sum - min_prefix)` BEFORE updating
//      `min_prefix = min(min_prefix, prefix_sum)` guarantees every candidate
//      subarray is non-empty (at least one element), even when all numbers are
//      negative. Using 64-bit accumulators prevents intermediate overflow.
//      Time complexity: O(n). Space complexity: O(1).
//   2. Divide and conquer (`maxSubArrayDivideAndConquer`, the problem's
//      follow-up): each segment [l, r] summarizes four values — total sum,
//      best prefix sum, best suffix sum, and best subarray sum — which merge
//      in O(1) across the midpoint (the exact monoid behind range-maximum
//      subarray segment trees).
//      Time complexity: O(n) via T(n) = 2T(n/2) + O(1).
//      Space complexity: O(log n) recursion stack.
//
// Data Structures:
//   - Scalar running accumulators (`prefix_sum`, `min_prefix`, `best`) for
//     the O(1)-space streaming pass.
//   - `SegmentStats` 4-field aggregate struct for the divide-and-conquer
//     monoid merge.
//
// Real-World Applications:
//   - Quantitative finance: finding the single highest-gain contiguous
//     holding period from daily return deltas.
//   - Bioinformatics / genomics: identifying high-scoring GC-rich or
//     conserved contiguous sequence segments (Smith-Waterman / Kadane).
//   - Computer vision and signal processing: locating brightest 1D/2D
//     contiguous regions in histograms, telemetry bursts, or anomaly windows.
//
// Related LeetCode Problems:
//   - #121 Best Time to Buy and Sell Stock [done] (maximum subarray of daily price diffs)
//   - #152 Maximum Product Subarray [done] (multiplicative analog tracking both max and min)
//   - #918 Maximum Sum Circular Subarray [new] (extends Kadane to wrap-around subarrays)
//   - #1186 Maximum Subarray Sum with One Deletion [new] (Kadane with one allowed skip)
//   - #1749 Maximum Absolute Sum of Any Subarray [new] (uses max_prefix - min_prefix directly)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

namespace {

// Monoid summary for a contiguous slice [l, r], enabling O(1) merging in
// divide-and-conquer and segment-tree queries.
struct SegmentStats {
  std::int64_t total = 0;         // Sum of all elements in the segment.
  std::int64_t max_prefix = 0;    // Maximum sum of a non-empty prefix.
  std::int64_t max_suffix = 0;    // Maximum sum of a non-empty suffix.
  std::int64_t max_subarray = 0;  // Maximum sum of any non-empty subarray.
};

class Solution {
 public:
  // Computes the maximum contiguous subarray sum using running prefix sum
  // minus the minimum earlier prefix sum (Kadane's algorithm in prefix form).
  //
  // Invariant: at the start of iteration i, `min_prefix` equals
  // min_{0 <= k <= i} prefix[k] (with prefix[0] = 0), so after adding nums[i],
  // `prefix_sum - min_prefix` is the maximum sum of any non-empty subarray
  // ending at index i.
  //
  // Time: O(n), Space: O(1).
  int maxSubArray(const std::vector<int>& nums) {
    std::int64_t prefix_sum = 0;
    std::int64_t min_prefix = 0;
    std::int64_t best = std::numeric_limits<std::int64_t>::lowest();

    for (const int value : nums) {
      prefix_sum += value;
      // Evaluate before updating `min_prefix` so the subarray has length >= 1.
      best = std::max(best, prefix_sum - min_prefix);
      min_prefix = std::min(min_prefix, prefix_sum);
    }

    return static_cast<int>(best);
  }

  // Follow-up divide-and-conquer solution using the 4-tuple segment monoid.
  //
  // Time: O(n), Space: O(log n) recursion stack.
  int maxSubArrayDivideAndConquer(const std::vector<int>& nums) {
    auto solve = [&](this auto&& self, std::size_t left,
                     std::size_t right) -> SegmentStats {
      if (left == right) {
        const auto val = static_cast<std::int64_t>(nums[left]);
        return SegmentStats{
            .total = val,
            .max_prefix = val,
            .max_suffix = val,
            .max_subarray = val,
        };
      }

      const std::size_t mid = left + ((right - left) / 2uz);
      const SegmentStats lhs = self(left, mid);
      const SegmentStats rhs = self(mid + 1uz, right);

      // A best subarray lies entirely in lhs, entirely in rhs, or crosses the
      // midpoint as a suffix of lhs concatenated with a prefix of rhs.
      return SegmentStats{
          .total = lhs.total + rhs.total,
          .max_prefix = std::max(lhs.max_prefix, lhs.total + rhs.max_prefix),
          .max_suffix = std::max(rhs.max_suffix, rhs.total + lhs.max_suffix),
          .max_subarray = std::max({lhs.max_subarray, rhs.max_subarray,
                                    lhs.max_suffix + rhs.max_prefix}),
      };
    };

    return static_cast<int>(solve(0uz, nums.size() - 1uz).max_subarray);
  }
};

LC_PROBLEM(53, "Maximum Subarray", "array", ctx) {
  Solution sol;

  ctx.test("example_mixed_signs", [&] {
    const std::vector<int> nums{-2, 1, -3, 4, -1, 2, 1, -5, 4};
    ctx.expect_eq(sol.maxSubArray(nums), 6);
    ctx.expect_eq(sol.maxSubArrayDivideAndConquer(nums), 6);
  });

  ctx.test("single_positive_and_single_negative", [&] {
    const std::vector<int> pos{1};
    ctx.expect_eq(sol.maxSubArray(pos), 1);
    ctx.expect_eq(sol.maxSubArrayDivideAndConquer(pos), 1);

    const std::vector<int> neg{-42};
    ctx.expect_eq(sol.maxSubArray(neg), -42);
    ctx.expect_eq(sol.maxSubArrayDivideAndConquer(neg), -42);
  });

  ctx.test("all_negative_picks_least_negative", [&] {
    const std::vector<int> nums1{-2, -1};
    ctx.expect_eq(sol.maxSubArray(nums1), -1);
    ctx.expect_eq(sol.maxSubArrayDivideAndConquer(nums1), -1);

    const std::vector<int> nums2{-5, -2, -8, -1, -4};
    ctx.expect_eq(sol.maxSubArray(nums2), -1);
    ctx.expect_eq(sol.maxSubArrayDivideAndConquer(nums2), -1);
  });

  ctx.test("whole_array_optimal", [&] {
    const std::vector<int> nums{5, 4, -1, 7, 8};
    ctx.expect_eq(sol.maxSubArray(nums), 23);
    ctx.expect_eq(sol.maxSubArrayDivideAndConquer(nums), 23);
  });

  ctx.test("all_zeros", [&] {
    const std::vector<int> nums{0, 0, 0};
    ctx.expect_eq(sol.maxSubArray(nums), 0);
    ctx.expect_eq(sol.maxSubArrayDivideAndConquer(nums), 0);
  });
}

}  // namespace
