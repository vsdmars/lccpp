// LeetCode 1877: Minimize Maximum Pair Sum In Array
// https://leetcode.com/problems/minimize-maximum-pair-sum-in-array/
//
// Problem:
//   Given an integer array `nums` of even length `n`, partition all elements
//   into `n / 2` disjoint pairs `(a, b)` such that the maximum pair sum
//   `max(a + b)` across all pairs is as small as possible. Return that
//   minimized maximum pair sum.
//   Constraints: n is even, 2 <= n <= 10^5, 1 <= nums[i] <= 10^5.
//
// Algorithm:
//   Sort `nums` in non-decreasing order and pair the i-th smallest element with
//   the i-th largest element: `(nums[i], nums[n - 1 - i])` for `0 <= i < n / 2`.
//   Exchange-argument proof of optimality:
//     Consider the largest element `M = nums[n - 1]` and smallest element
//     `m = nums[0]`. Suppose an optimal pairing pairs `M` with `x` and `m` with
//     `y` (`m <= x, y <= M`). Re-pairing them as `(M, m)` and `(x, y)` satisfies
//     `M + m <= M + x` and `x + y <= M + x`, so `max(M + m, x + y) <= max(M + x, m + y)`.
//     Thus pairing `m` with `M` never worsens the objective; induction on the
//     remaining `n - 2` elements proves opposite-end pairing is globally optimal.
//   Time Complexity:  O(n log n) for sorting, O(n) for the single pairing scan.
//   Space Complexity: O(log n) auxiliary stack space for in-place introsort.
//
// Data Structures:
//   - `std::vector<int>` sorted in place, indexed symmetrically from both ends.
//
// Real-World Applications:
//   - Load balancing pairs of tasks (e.g., pairing a heavy compute job with a
//     light job on each dual-slot server to minimize peak server load).
//   - Bin-packing / two-item container loading where each truck carries two
//     shipments and maximum axle load must be minimized.
//   - Pairing players or reviewers of complementary skill/workload levels to
//     keep maximum team workload balanced.
//
// Related LeetCode Problems:
//   - #881 Boats To Save People [done] (opposite-end two-pointer greedy pairing after sorting)
//   - #561 Array Partition [new] (sorting + greedy pairing to optimize pair mins)
//   - #2491 Divide Players Into Teams Of Equal Skill [new] (opposite-end pairing requiring equal sums)
//   - #1679 Max Number Of K Sum Pairs [new] (two-pointer opposite-end pair matching)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the minimized maximum pair sum after optimally pairing the even
  // number of elements in `nums`.
  // Time: O(n log n), Space: O(log n).
  int minPairSum(std::vector<int> nums) {
    // Sort ascending so opposite ends pair the smallest with the largest.
    std::ranges::sort(nums);

    int max_pair_sum = 0;
    const std::size_t n = nums.size();
    const std::size_t half = n / 2uz;

    // Pair nums[i] with its mirror element nums[(n - 1) - i].
    for (std::size_t i = 0uz; i < half; ++i) {
      const int pair_sum = nums[i] + nums[(n - 1uz) - i];
      max_pair_sum = std::max(max_pair_sum, pair_sum);
    }

    return max_pair_sum;
  }
};

LC_PROBLEM(1877, "Minimize Maximum Pair Sum In Array", "sort", ctx) {
  Solution sol;

  ctx.test("example1_four_elements", [&] {
    // Optimal pairs: (2, 5) and (3, 3) -> max(7, 6) = 7.
    ctx.expect_eq(sol.minPairSum({3, 5, 2, 3}), 7);
  });

  ctx.test("example2_six_elements", [&] {
    // Optimal pairs: (2, 6), (3, 5), (4, 4) -> max(8, 8, 8) = 8.
    ctx.expect_eq(sol.minPairSum({3, 5, 4, 2, 4, 6}), 8);
  });

  ctx.test("minimal_two_elements", [&] {
    ctx.expect_eq(sol.minPairSum({1, 100}), 101);
  });

  ctx.test("all_equal_and_bimodal", [&] {
    ctx.expect_eq(sol.minPairSum({4, 4, 4, 4}), 8);
    ctx.expect_eq(sol.minPairSum({1, 1, 1, 100, 100, 100}), 101);
  });
}

}  // namespace
