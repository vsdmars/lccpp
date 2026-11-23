// LeetCode 2256: Minimum Average Difference
// https://leetcode.com/problems/minimum-average-difference/
//
// Problem:
//   Given a 0-indexed integer array `nums` of length `n` (1 <= n <= 10^5,
//   0 <= nums[i] <= 10^5), define the average difference at index `i` as:
//     | floor(sum(nums[0..i]) / (i + 1)) -
//       floor(sum(nums[i+1..n-1]) / (n - i - 1)) |
//   where the average of 0 elements (when i == n - 1) is defined as 0.
//   Return the smallest index `i` that minimizes this average difference.
//
// Algorithm:
//   Two-pass running prefix/suffix sum scan:
//     1. First pass: compute `total_sum` across all elements in 64-bit integer
//        arithmetic (max sum is 10^10, which exceeds 32-bit `int`).
//     2. Second pass: scan `i` from `0` to `n - 1`, maintaining `prefix_sum`
//        incrementally and deriving `suffix_sum = total_sum - prefix_sum`.
//     3. Compute truncated integer averages for the prefix of length `i + 1`
//        and suffix of length `n - i - 1` (0 when suffix is empty), then take
//        their absolute difference.
//     4. Update `(min_diff, best_idx)` strictly when `diff < min_diff` so ties
//        automatically preserve the smallest index.
//   Time Complexity:  O(n).
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   Read-only view of the input `std::vector<int>` plus O(1) scalar 64-bit
//   accumulators (`total_sum`, `prefix_sum`, `min_diff`) and index tracker
//   (`best_idx`).
//
// Real-World Applications:
//   - Time-series change-point detection: locating the split timestamp where
//     historical and future moving averages diverge least (or most).
//   - Load balancing & stream partitioning: finding a contiguous split point
//     that equalizes mean latency or throughput between two processing stages.
//   - Financial & telemetry analytics: single-pass online prefix/suffix
//     aggregation without materializing auxiliary prefix-sum arrays.
//
// Related LeetCode Problems:
//   - #724 Find Pivot Index [done] (matches prefix and suffix sums in a single O(1)-space pass)
//   - #2270 Number of Ways to Split Array [new] (evaluates prefix vs suffix sums across all split indices)
//   - #1991 Find the Middle Index in Array [new] (finds the earliest prefix/suffix balance point)
//   - #2574 Left and Right Sum Differences [new] (computes absolute prefix/suffix sum differences per index)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the smallest index `i` minimizing the absolute difference between
  // the floored integer average of `nums[0..i]` and `nums[i+1..n-1]`.
  //
  // Invariant:
  //   At iteration `i`, `prefix_sum == sum(nums[0..i])`,
  //   `suffix_sum == sum(nums[i+1..n-1])`, and `(min_diff, best_idx)` stores
  //   the optimal average difference and earliest achieving index over `0..i-1`.
  //
  // Time:  O(n)
  // Space: O(1)
  int minimumAverageDifference(const std::vector<int>& nums) {
    const std::size_t n = nums.size();
    const auto n_count = static_cast<std::int64_t>(n);

    // Compute total sum in 64-bit integer arithmetic because up to 10^5
    // elements of value 10^5 can sum to 10^10 (> INT32_MAX).
    std::int64_t total_sum = 0;
    for (const int val : nums) {
      total_sum += val;
    }

    std::int64_t prefix_sum = 0;
    std::int64_t min_diff = std::numeric_limits<std::int64_t>::max();
    std::size_t best_idx = 0uz;

    for (std::size_t i = 0uz; i < n; ++i) {
      prefix_sum += nums[i];
      const std::int64_t suffix_sum = total_sum - prefix_sum;

      // All values are non-negative, so C++ integer division matches floor.
      const std::int64_t left_count = static_cast<std::int64_t>(i) + 1;
      const std::int64_t right_count = n_count - left_count;

      const std::int64_t left_avg = prefix_sum / left_count;
      // By problem definition, the average of 0 remaining elements is 0.
      const std::int64_t right_avg =
          (right_count > 0) ? (suffix_sum / right_count) : 0;

      const std::int64_t diff = std::abs(left_avg - right_avg);

      // Strict inequality ensures ties keep the smallest index.
      if (diff < min_diff) {
        min_diff = diff;
        best_idx = i;
      }
    }

    return static_cast<int>(best_idx);
  }
};

LC_PROBLEM(2256, "Minimum Average Difference", "math", ctx) {
  Solution sol;

  ctx.test("example1", [&] {
    ctx.expect_eq(sol.minimumAverageDifference({2, 5, 3, 9, 5, 3}), 3);
  });

  ctx.test("example2_single_zero", [&] {
    ctx.expect_eq(sol.minimumAverageDifference({0}), 0);
  });

  ctx.test("python_build_alternating_ties", [&] {
    // At index 0: |0/1 - 3/5| = |0 - 0| = 0; index 1 also has diff 0, so
    // tie-breaking must return the smallest index (0).
    ctx.expect_eq(sol.minimumAverageDifference({0, 1, 0, 1, 0, 1}), 0);
  });

  ctx.test("minimum_at_last_index", [&] {
    // diffs: i=0 -> |0-3|=3, i=1 -> |0-5|=5, i=2 -> |0-10|=10, i=3 -> |2-0|=2.
    ctx.expect_eq(sol.minimumAverageDifference({0, 0, 0, 10}), 3);
  });

  ctx.test("all_equal_elements", [&] {
    ctx.expect_eq(sol.minimumAverageDifference({5, 5, 5, 5}), 0);
  });

  ctx.test("large_values_64bit_sum_guard", [&] {
    // 25,000 zeros followed by 25,000 values of 100,000 -> total sum = 2.5e9
    // (> INT32_MAX). Only the final index (49,999) achieves diff = 50,000.
    std::vector<int> nums(25000uz, 0);
    nums.insert(nums.end(), 25000uz, 100000);
    ctx.expect_eq(sol.minimumAverageDifference(nums), 49999);
  });
}

}  // namespace
