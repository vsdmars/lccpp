// LeetCode 410: Split Array Largest Sum
// https://leetcode.com/problems/split-array-largest-sum/
//
// Problem:
//   Given an array `nums` of non-negative integers and an integer `k`, split
//   `nums` into `k` non-empty contiguous subarrays such that the largest sum
//   among the `k` subarrays is minimized. Return that minimized largest sum.
//
// Algorithm:
//   Binary Search on the Answer Space + Greedy Feasibility Check:
//   Because every element is non-negative, feasibility is monotonic in the
//   maximum allowed subarray sum `limit`:
//     - Lower bound `lo = max(nums)` (every subarray must accommodate at least
//       the single largest element).
//     - Upper bound `hi = sum(nums)` (the whole array as 1 subarray).
//     - Feasibility predicate `canSplit(nums, k, limit)`: greedily extend the
//       current subarray until adding the next element would exceed `limit`,
//       at which point start a new subarray. Return true iff the total number
//       of subarrays needed is <= k.
//     - Binary-search for the smallest `limit` in [lo, hi] for which
//       `canSplit` returns true (using 64-bit integers to prevent sum
//       overflow).
//   Time: O(n log(sum(nums) - max(nums))), Space: O(1).
//
// Data Structures:
//   Only O(1) 64-bit integer accumulators and binary-search bounds operating
//   directly on the input `std::vector<int>`.
//
// Real-World Applications:
//   - Partitioning ordered workloads or video/audio frames into k contiguous
//     shards across workers to minimize the bottleneck worker's processing time
//     (painter's partition / book allocation problem).
//   - Database range-sharding of sorted keys into k partitions to balance peak
//     shard size or query load.
//   - Splitting long text or print jobs across k pages/columns to minimize the
//     heaviest page.
//
// Related LeetCode Problems:
//   - #1011 Capacity To Ship Packages Within D Days [done] (identical binary-search-on-answer + greedy partitioning)
//   - #875 Koko Eating Bananas [done] (binary search on minimum feasible rate)
//   - #1231 Divide Chocolate [done] (dual max-min contiguous partition via binary search)
//   - #774 Minimize Max Distance to Gas Station [done] (continuous binary search on max gap)
//   - #2064 Minimized Maximum of Products Distributed to Any Store [new] (parametric binary search minimizing peak load)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the minimum possible largest subarray sum when partitioning `nums`
  // into `k` non-empty contiguous subarrays.
  //
  // Invariant: The optimal minimized largest sum lies in [lo, hi], where
  // `canSplit(hi)` is always true and `canSplit(x)` is false for all x < lo.
  // Time: O(n log(sum(nums))), Space: O(1).
  int splitArray(const std::vector<int>& nums, int k) {
    // Establish tight search bounds in 64-bit arithmetic to avoid sum overflow.
    std::int64_t lo = 0;
    std::int64_t hi = 0;
    for (const int num : nums) {
      lo = std::max<std::int64_t>(lo, num);
      hi += num;
    }

    while (lo < hi) {
      const std::int64_t mid = lo + ((hi - lo) / 2);

      if (canSplit(nums, k, mid)) {
        // `mid` is feasible; try to tighten the upper bound.
        hi = mid;
      } else {
        // `mid` forces more than k subarrays; raise the lower bound.
        lo = mid + 1;
      }
    }

    return static_cast<int>(lo);
  }

 private:
  // Greedily checks whether `nums` can be partitioned into at most `k`
  // contiguous subarrays with each subarray sum <= `max_subarray_sum`.
  static bool canSplit(const std::vector<int>& nums, int k,
                       std::int64_t max_subarray_sum) {
    int subarrays = 1;
    std::int64_t current_sum = 0;

    for (const int num : nums) {
      if (current_sum + num > max_subarray_sum) {
        // Start a new subarray beginning at `num`.
        ++subarrays;
        if (subarrays > k) {
          return false;
        }
        current_sum = num;
      } else {
        current_sum += num;
      }
    }

    return true;
  }
};

LC_PROBLEM(410, "Split Array Largest Sum", "binary_search", ctx) {
  ctx.test("docstring_and_leetcode_examples", [&] {
    Solution sol;
    ctx.expect_eq(sol.splitArray({7, 2, 5, 10, 8}, 2), 18);
    ctx.expect_eq(sol.splitArray({1, 2, 3, 4, 5}, 2), 9);
    ctx.expect_eq(sol.splitArray({1, 4, 4}, 3), 4);
  });

  ctx.test("boundary_k_values_and_zeros", [&] {
    Solution sol;
    // k == 1 -> whole array sum.
    ctx.expect_eq(sol.splitArray({7, 2, 5, 10, 8}, 1), 32);
    // k == n -> maximum single element.
    ctx.expect_eq(sol.splitArray({7, 2, 5, 10, 8}, 5), 10);
    // Single element.
    ctx.expect_eq(sol.splitArray({42}, 1), 42);
    // All zeros.
    ctx.expect_eq(sol.splitArray({0, 0, 0, 0}, 2), 0);
  });

  ctx.test("large_values_no_overflow", [&] {
    Solution sol;
    ctx.expect_eq(sol.splitArray({1'000'000, 1'000'000, 1'000'000, 1'000'000}, 2),
                  2'000'000);
    ctx.expect_eq(sol.splitArray({1'000'000'000, 1'000'000'000}, 2),
                  1'000'000'000);
  });
}

}  // namespace
