// LeetCode 454: 4Sum II
// https://leetcode.com/problems/4sum-ii/
//
// Problem:
//   Given four integer arrays `nums1`, `nums2`, `nums3`, and `nums4`, each of
//   length n, return the number of index tuples (i, j, k, l) with
//   0 <= i, j, k, l < n such that:
//     nums1[i] + nums2[j] + nums3[k] + nums4[l] == 0.
//   Constraints: 1 <= n <= 200, -2^28 <= numsX[i] <= 2^28.
//
// Examples:
//   Example 1:
//     Input:  nums1 = [1, 2], nums2 = [-2, -1], nums3 = [-1, 2], nums4 = [0, 2]
//     Output: 2
//     Explanation:
//       - Phase 1 records all four pairwise sums from nums1 and nums2 into
//         pair_sum_counts: 1 + (-2) = -1, 1 + (-1) = 0, 2 + (-2) = 0, and 2 + (-1)
//         = 1, giving {-1: 1, 0: 2, 1: 1}.
//       - Phase 2 iterates over pairs (c, d) from nums3 and nums4 and looks up
//         target = -(c + d) in pair_sum_counts.
//       - Pair (-1, 0) has target = 1, which hits pair_sum_counts[1] = 1
//         (tuple_count = 1), and pair (-1, 2) has target = -1, which hits
//         pair_sum_counts[-1] = 1 (tuple_count = 2).
//       - Pairs (2, 0) and (2, 2) produce targets -2 and -4, neither of which is in
//         pair_sum_counts, so fourSumCount returns 2.
//
//   Example 2:
//     Input:  nums1 = [1, 1], nums2 = [-1, -1], nums3 = [0, 0], nums4 = [0, 0]
//     Output: 16
//     Explanation:
//       - Phase 1 evaluates all 4 pairs (1, -1) from nums1 and nums2, each summing
//         to 0, so pair_sum_counts becomes {0: 4}.
//       - Phase 2 evaluates all 4 pairs (0, 0) from nums3 and nums4, each querying
//         target = -(0 + 0) = 0 and adding pair_sum_counts[0] = 4 to tuple_count.
//       - Accumulating 4 across all 4 pairs from nums3 x nums4 returns tuple_count
//         = 16.
//
//   Example 3:
//     Input:  nums1 = [1, 2], nums2 = [3, 4], nums3 = [5, 6], nums4 = [7, 8]
//     Output: 0
//     Explanation:
//       - Phase 1 inserts the 4 pair sums from nums1 and nums2 into
//         pair_sum_counts: 1 + 3 = 4, 1 + 4 = 5, 2 + 3 = 5, and 2 + 4 = 6, giving
//         {4: 1, 5: 2, 6: 1}.
//       - Phase 2 checks target = -(c + d) for pairs (5, 7), (5, 8), (6, 7), and
//         (6, 8), yielding targets -12, -13, -13, and -14.
//       - None of those negative targets appear in pair_sum_counts, so tuple_count
//         remains 0.
//
// Algorithm:
//   Meet-in-the-middle decomposition. Rather than enumerating all O(n^4)
//   quadruples, rewrite the condition as:
//     nums1[i] + nums2[j] == -(nums3[k] + nums4[l]).
//   Phase 1 enumerates every pair (a, b) from (nums1, nums2) and records the
//   frequency of each sum (a + b) in a hash map. Phase 2 enumerates every pair
//   (c, d) from (nums3, nums4) and adds the stored frequency of -(c + d) to the
//   answer.
//   Time Complexity:  O(n^2) average (two nested loops of size n * n).
//   Space Complexity: O(n^2) worst case to store up to n^2 distinct pair sums.
//
// Data Structures:
//   - `std::unordered_map<std::int64_t, int>`: maps each pair sum (a + b) from
//     the first two arrays to its multiplicity, enabling O(1) average-time
//     complement lookups for -(c + d).
//
// Real-World Applications:
//   - Meet-in-the-middle cryptanalysis (e.g., Double-DES key-space reduction
//     from O(2^(2k)) to O(2^k) time and space).
//   - Financial reconciliation and multi-ledger settlement matching where four
//     independent transaction legs must net to zero.
//   - Database multi-way join optimization using hash-partitioned intermediate
//     aggregates to avoid full Cartesian products.
//
// Related LeetCode Problems:
//   - #1 Two Sum [done] (1-array complement lookup with a hash map)
//   - #15 3Sum [done] (three-element zero-sum tuples in a single array)
//   - #18 4Sum [done] (four-element target-sum tuples in a single array without duplicate values)
//   - #1679 Max Number of K-Sum Pairs [new] (counting complementary pairs via frequency map)

#include "framework/lc_framework.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Counts all index tuples (i, j, k, l) satisfying
  // nums1[i] + nums2[j] + nums3[k] + nums4[l] == 0 using meet-in-the-middle.
  //
  // Time: O(n^2) average, Space: O(n^2).
  int fourSumCount(const std::vector<int>& nums1, const std::vector<int>& nums2,
                   const std::vector<int>& nums3,
                   const std::vector<int>& nums4) {
    // Phase 1: Record the frequency of every pairwise sum from nums1 x nums2.
    // Using 64-bit keys eliminates signed overflow risk for extreme inputs.
    std::unordered_map<std::int64_t, int> pair_sum_counts;
    pair_sum_counts.reserve(nums1.size() * nums2.size());

    for (const int a : nums1) {
      for (const int b : nums2) {
        const std::int64_t sum = static_cast<std::int64_t>(a) + b;
        ++pair_sum_counts[sum];
      }
    }

    // Phase 2: For each pair from nums3 x nums4, accumulate how many (a, b)
    // pairs match the required additive inverse -(c + d).
    int tuple_count = 0;
    for (const int c : nums3) {
      for (const int d : nums4) {
        const std::int64_t target = -(static_cast<std::int64_t>(c) + d);
        if (const auto it = pair_sum_counts.find(target);
            it != pair_sum_counts.end()) {
          tuple_count += it->second;
        }
      }
    }

    return tuple_count;
  }
};

LC_PROBLEM(454, "4Sum II", "math", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    ctx.expect_eq(sol.fourSumCount({1, 2}, {-2, -1}, {-1, 2}, {0, 2}), 2);
  });

  ctx.test("example_2_single_zero", [&] {
    ctx.expect_eq(sol.fourSumCount({0}, {0}, {0}, {0}), 1);
  });

  ctx.test("all_combinations_match", [&] {
    ctx.expect_eq(sol.fourSumCount({1, 1}, {-1, -1}, {0, 0}, {0, 0}), 16);
  });

  ctx.test("no_matching_tuples", [&] {
    ctx.expect_eq(sol.fourSumCount({1, 2}, {3, 4}, {5, 6}, {7, 8}), 0);
  });

  ctx.test("boundary_constraint_values", [&] {
    constexpr int kBound = 1 << 28;
    ctx.expect_eq(sol.fourSumCount({kBound}, {-kBound}, {kBound}, {-kBound}),
                  1);
  });
}

}  // namespace
