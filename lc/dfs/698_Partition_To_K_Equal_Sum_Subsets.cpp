// LeetCode 698: Partition To K Equal Sum Subsets
// https://leetcode.com/problems/partition-to-k-equal-sum-subsets/
//
// Problem:
//   Given an integer array `nums` of positive integers and a positive integer
//   `k`, determine whether it is possible to divide `nums` into `k` non-empty
//   subsets whose element sums are all equal.
//   Constraints: 1 <= k <= nums.size() <= 16, 1 <= nums[i] <= 10^4.
//
// Examples:
//   Example 1:
//     Input:  nums = [4, 3, 2, 3, 5, 2, 1], k = 4
//     Output: true
//     Explanation:
//       - The total sum is 20 and 20 % 4 == 0, giving target = 5; sorting
//         descending yields nums = [5, 4, 3, 3, 2, 2, 1] and buckets = [5, 5, 5,
//         5].
//       - Elements idx = 0..3 (values 5, 4, 3, 3) are placed into buckets 0, 1, 2,
//         and 3 respectively, leaving remaining capacities [0, 1, 2, 2].
//       - Next, idx = 4 (val = 2) fills bucket 2 to 0, idx = 5 (val = 2) fills
//         bucket 3 to 0, and idx = 6 (val = 1) fills bucket 1 to 0.
//       - When idx reaches 7 (nums.size()), all elements have been placed without
//         negative capacities, so backtrack returns true.
//
//   Example 2:
//     Input:  nums = [1, 2, 3, 4], k = 3
//     Output: false
//     Explanation:
//       - canPartitionKSubsets confirms k = 3 > 0 and nums.size() = 4 >= 3, then
//         sums nums to total = 10.
//       - Because total % k = 10 % 3 = 1 != 0, no partition into 3 equal-sum
//         subsets is possible and the function immediately returns false.
//
//   Example 3:
//     Input:  nums = [2, 2, 2, 2, 3, 4, 5], k = 4
//     Output: false
//     Explanation:
//       - Summing nums gives total = 20 and target = 5; descending sort yields [5,
//         4, 3, 2, 2, 2, 2] with buckets = [5, 5, 5, 5].
//       - Backtracking places 5 into bucket 0, 4 into bucket 1, and 3 into bucket
//         2, leaving buckets = [0, 1, 2, 5] with no value 1 available to complete
//         bucket 1.
//       - Every way of placing the first three 2s into buckets 2 and 3 reduces
//         buckets to [0, 1, 0, 1], leaving the final 2 unable to fit anywhere.
//       - has_same_capacity_before and empty-bucket pruning (buckets[bucket] ==
//         target) skip symmetric states until all branches fail and false is
//         returned.
//
// Algorithm:
//   Compute the total sum of `nums`. If `total % k != 0`, an equal partition
//   is impossible; otherwise each subset must sum to `target = total / k`.
//   Sort `nums` in descending order so large elements constrain bucket choices
//   immediately (fail-fast ordering). If `nums[0] > target`, return false.
//   Run backtracking DFS over element indices `0..n-1`, assigning `nums[idx]`
//   to one of `k` buckets that track remaining capacity (initialized to
//   `target`). Two symmetry-breaking rules prune redundant search branches:
//     1. Duplicate-capacity pruning: skip bucket `j` if an earlier bucket
//        `0..j-1` currently has the exact same remaining capacity, because
//        placing `nums[idx]` into either bucket yields an isomorphic state.
//     2. Empty-bucket pruning: after backtracking from placing `nums[idx]`
//        into an empty bucket (`buckets[j] == target`), break out of the loop
//        because all remaining empty buckets are symmetric.
//   When `idx == nums.size()`, every element was placed without overfilling
//   any bucket; since `sum(nums) == k * target`, all remaining capacities are
//   guaranteed to be zero.
//   Time Complexity: O(k * 2^n) worst-case upper bound (vastly faster in
//     practice with descending sort and symmetry-breaking pruning).
//   Space Complexity: O(k + n) for the bucket capacities and recursion stack.
//
// Data Structures:
//   - std::vector<int> `nums`: sorted descending for fail-fast backtracking.
//   - std::vector<int> `buckets`: length-`k` array tracking remaining target
//     sum needed by each subset.
//
// Real-World Applications:
//   - Multi-processor load balancing: partitioning jobs with known runtimes
//     across `k` identical workers so all machines finish at the same time.
//   - Distributed storage sharding: evenly balancing data partitions by byte
//     size across `k` storage nodes.
//   - Bin packing and resource allocation in cloud schedulers and logistics.
//
// Related LeetCode Problems:
//   - #416 Partition Equal Subset Sum [done] (special case k = 2 via subset-sum DP)
//   - #473 Matchsticks to Square [new] (special case k = 4 forming four equal sides)
//   - #2305 Fair Distribution of Cookies [new] (minimize maximum subset sum across k buckets)
//   - #1723 Find Minimum Time to Finish All Jobs [new] (identical bucket-pruning backtracking pattern)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <span>
#include <vector>

namespace {

class Solution {
 public:
  // Determines whether `nums` can be partitioned into `k` subsets of equal sum.
  // Time: O(k * 2^n) worst case with heavy pruning; Space: O(k + n).
  bool canPartitionKSubsets(std::vector<int> nums, int k) {
    if (k <= 0 || nums.size() < static_cast<std::size_t>(k)) {
      return false;
    }

    // Compute total sum to derive the exact per-bucket target sum.
    int total = 0;
    for (const int num : nums) {
      total += num;
    }
    if ((total % k) != 0) {
      return false;
    }
    const int target = total / k;

    // Sort descending so larger items are placed first and prune dead branches
    // as early as possible in the search tree.
    std::ranges::sort(nums, std::greater<>{});
    if (nums[0] > target) {
      return false;
    }

    // Track remaining capacity required by each of the `k` subsets.
    std::vector<int> buckets(static_cast<std::size_t>(k), target);
    return backtrack(nums, 0uz, target, buckets);
  }

 private:
  // Returns true if any bucket before `bucket_idx` currently holds the same
  // remaining capacity, meaning trying `bucket_idx` would explore an
  // isomorphic state already tested earlier.
  static bool has_same_capacity_before(const std::vector<int>& buckets,
                                       std::size_t bucket_idx) {
    const int capacity = buckets[bucket_idx];
    return std::ranges::any_of(
        std::span<const int>(buckets).first(bucket_idx),
        [capacity](int prev_capacity) { return prev_capacity == capacity; });
  }

  // Recursively assigns `nums[idx]` to a valid bucket until all elements are
  // placed. Invariant: `sum(buckets) == sum(nums[idx..n-1])` and every bucket
  // capacity stays non-negative.
  static bool backtrack(const std::vector<int>& nums, std::size_t idx,
                        int target, std::vector<int>& buckets) {
    if (idx == nums.size()) {
      // Because total == k * target and no bucket went negative, every bucket
      // must now have remaining capacity 0.
      return true;
    }

    const int val = nums[idx];
    for (std::size_t bucket = 0uz; bucket < buckets.size(); ++bucket) {
      if (buckets[bucket] < val) {
        continue;
      }
      if (has_same_capacity_before(buckets, bucket)) {
        continue;
      }

      // Place `val` into `bucket` and recurse on the next element.
      buckets[bucket] -= val;
      if (backtrack(nums, idx + 1uz, target, buckets)) {
        return true;
      }
      // Undo placement to backtrack.
      buckets[bucket] += val;

      // If placing `val` into an empty bucket failed, placing it into any
      // subsequent empty bucket will fail identically.
      if (buckets[bucket] == target) {
        break;
      }
    }
    return false;
  }
};

LC_PROBLEM(698, "Partition To K Equal Sum Subsets", "dfs", ctx) {
  ctx.test("example_1_four_subsets", [&] {
    Solution sol;
    ctx.expect_true(sol.canPartitionKSubsets({4, 3, 2, 3, 5, 2, 1}, 4));
  });

  ctx.test("example_2_not_divisible", [&] {
    Solution sol;
    ctx.expect_false(sol.canPartitionKSubsets({1, 2, 3, 4}, 3));
  });

  ctx.test("single_subset_k_equals_1", [&] {
    Solution sol;
    ctx.expect_true(sol.canPartitionKSubsets({5, 10, 15}, 1));
  });

  ctx.test("element_exceeds_target", [&] {
    Solution sol;
    ctx.expect_false(sol.canPartitionKSubsets({2, 2, 2, 2, 3, 4, 5}, 4));
  });

  ctx.test("equal_pairs_partition", [&] {
    Solution sol;
    ctx.expect_true(sol.canPartitionKSubsets({1, 1, 1, 1, 2, 2, 2, 2}, 4));
  });

  ctx.test("pruning_stress_impossible_partition", [&] {
    Solution sol;
    // Sum is 32, k = 8 => target = 4, but two 3s cannot combine with 2s to
    // reach 4. Without symmetry pruning this explores an exponential tree.
    ctx.expect_false(sol.canPartitionKSubsets(
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3}, 8));
  });
}

}  // namespace
