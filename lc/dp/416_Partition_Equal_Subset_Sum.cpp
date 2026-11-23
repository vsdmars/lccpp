// LeetCode 416: Partition Equal Subset Sum
// https://leetcode.com/problems/partition-equal-subset-sum/
//
// Problem:
//   Given a non-empty array `nums` of positive integers, determine whether it
//   can be partitioned into two subsets with equal sum.
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 5, 11, 5]
//     Output: true
//     Explanation:
//       - Total sum is 22 (even), giving target = 11; initialize reachable[0] = 1
//         and reachable[1..11] = 0.
//       - Processing 1 sets reachable[1] = 1 (reachable sums: {0, 1}), and
//         processing 5 downward from 11 to 5 sets reachable[6] = 1 and reachable[5]
//         = 1 ({0, 1, 5, 6}).
//       - Processing 11 sets reachable[11] = reachable[0] = 1, triggering the early
//         return of true.
//
//   Example 2:
//     Input:  nums = [1, 2, 3, 5]
//     Output: false
//     Explanation:
//       - Sum all elements to get total = 11.
//       - Because total % 2 != 0, two subsets cannot share an equal integer sum, so
//         the function returns false immediately.
//
//   Example 3:
//     Input:  nums = [2, 2, 3, 5]
//     Output: false
//     Explanation:
//       - Total sum is 12 (even), so target = 6 and reachable starts with only
//         index 0 set to 1.
//       - Processing 2 and 2 sets reachable[2] = 1 and then reachable[4] = 1
//         (reachable sums: {0, 2, 4}).
//       - Processing 3 downward from 6 to 3 sets reachable[5] = 1 and reachable[3]
//         = 1, while reachable[6] stays 0 because reachable[3] was 0 before the
//         sweep.
//       - Processing 5 downward from 6 to 5 checks reachable[1] == 0 and
//         reachable[0] == 1, leaving reachable[6] = 0 and returning false.
//
// Algorithm:
//   Space-Optimized 0/1 Knapsack (Subset Sum) Dynamic Programming. Let `total`
//   be the sum of all elements in `nums`:
//   1. If `total` is odd, an equal split is mathematically impossible (`false`).
//   2. Otherwise set `target = total / 2`. The problem reduces to deciding
//      whether any subset of `nums` sums to `target`.
//   3. Maintain a 1D boolean table `reachable[0..target]` where `reachable[s]`
//      is true iff some subset of the elements processed so far sums to `s`,
//      initialized with `reachable[0] = true`.
//   4. For each `num` in `nums`:
//      - If `num > target`, return `false` immediately (no partition can place
//        `num` in a half of sum `target`).
//      - Sweep `s` *downward* from `target` to `num`:
//          reachable[s] = reachable[s] || reachable[s - num]
//        Sweeping downward guarantees each element is considered at most once
//        (preventing reuse within the same iteration).
//      - Exit early with `true` as soon as `reachable[target]` becomes true.
//   Time Complexity:  O(n * target) where `target = sum(nums) / 2`.
//   Space Complexity: O(target) for the 1D reachability table.
//
// Data Structures:
//   - `std::vector<std::uint8_t> reachable` of length `target + 1` (using
//     byte-backed flags instead of bit-packed `std::vector<bool>` for direct,
//     fast indexing).
//
// Real-World Applications:
//   - Two-way workload balancing across dual compute nodes or storage shards
//     to equalize total job weight or data volume.
//   - Dual-rail power distribution and phase load balancing in electrical
//     engineering and datacenter rack planning.
//   - Fair division of indivisible assets or tasks in auction and scheduling
//     systems.
//
// Related LeetCode Problems:
//   - #494 Target Sum [done] (transforms +/- sign assignment into subset-sum counting DP)
//   - #1049 Last Stone Weight II [new] (minimizes |sum(A) - sum(B)| via the same 0/1 subset-sum DP)
//   - #698 Partition to K Equal Sum Subsets [done] (generalization from 2 to k equal subsets)
//   - #322 Coin Change [done] (unbounded knapsack counterpart sweeping upward)
//   - #474 Ones and Zeroes [done] (2D 0/1 knapsack sweeping both dimensions downward)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Returns true iff `nums` can be partitioned into two equal-sum subsets.
  // Invariant: after processing the first `i` elements, `reachable[s] != 0`
  // iff some subset of `nums[0..i-1]` sums exactly to `s`.
  // Time: O(n * (sum / 2)), Space: O(sum / 2).
  bool canPartition(const std::vector<int>& nums) {
    int total = 0;
    for (const int num : nums) {
      total += num;
    }

    // An odd total cannot be split into two equal integer sums.
    if ((total % 2) != 0) {
      return false;
    }

    const int half = total / 2;
    const auto target = static_cast<std::size_t>(half);
    std::vector<std::uint8_t> reachable(target + 1uz, 0U);
    reachable[0uz] = 1U;

    for (const int num : nums) {
      // Any single element larger than half the total makes equal partitioning
      // impossible immediately.
      if (num > half) {
        return false;
      }

      const auto u_num = static_cast<std::size_t>(num);
      // Iterate downward so `reachable[s - u_num]` reflects only prior items
      // (the 0/1 knapsack single-use invariant).
      for (std::size_t s = target; s >= u_num; --s) {
        if (reachable[s - u_num] != 0U) {
          reachable[s] = 1U;
        }
        if (s == u_num) {
          break;
        }
      }

      // Early exit as soon as half the total sum becomes achievable.
      if (reachable[target] != 0U) {
        return true;
      }
    }

    return reachable[target] != 0U;
  }
};

LC_PROBLEM(416, "Partition Equal Subset Sum", "dp", ctx) {
  ctx.test("example_1_true", [&] {
    Solution sol;
    ctx.expect_true(sol.canPartition({1, 5, 11, 5}));
  });

  ctx.test("example_2_false", [&] {
    Solution sol;
    ctx.expect_false(sol.canPartition({1, 2, 3, 5}));
  });

  ctx.test("python_build_case", [&] {
    Solution sol;
    // [3, 3, 3] and [4, 5] both sum to 9.
    ctx.expect_true(sol.canPartition({3, 3, 3, 4, 5}));
  });

  ctx.test("edge_cases", [&] {
    Solution sol;
    ctx.expect_false(sol.canPartition({1}));
    ctx.expect_false(sol.canPartition({100}));
    ctx.expect_true(sol.canPartition({2, 2}));
    // Single element 5 > half (4) triggers immediate false exit.
    ctx.expect_false(sol.canPartition({1, 2, 5}));
    // Even total (12), max element <= 6, but no subset sums to 6.
    ctx.expect_false(sol.canPartition({2, 2, 3, 5}));
  });
}

}  // namespace
