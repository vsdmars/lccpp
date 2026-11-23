// LeetCode 1498: Number Of Subsequences That Satisfy The Given Sum Condition
// https://leetcode.com/problems/number-of-subsequences-that-satisfy-the-given-sum-condition/
//
// Problem:
//   Given an integer array `nums` and an integer `target`, return the number of
//   non-empty subsequences of `nums` such that the sum of the minimum element
//   and the maximum element in the subsequence is less than or equal to
//   `target`. Because the count may be very large, return it modulo 10^9 + 7.
//   Constraints: 1 <= nums.length <= 10^5, 1 <= nums[i], target <= 10^6.
//
// Examples:
//   Example 1:
//     Input:  nums = [3, 5, 6, 7], target = 9
//     Output: 4
//     Explanation:
//       - Sort a copy of nums to [3, 5, 6, 7], precompute pow2 = [1, 2, 4, 8], and
//         initialize left = 0, right = 3, result = 0.
//       - At left = 0, right = 3, sorted[0] + sorted[3] = 3 + 7 = 10 > 9, so
//         decrement right to 2.
//       - At left = 0, right = 2, sorted[0] + sorted[2] = 3 + 6 = 9 <= 9, so add
//         pow2[2 - 0] = 4 to result (result = 4) and increment left to 1.
//       - At left = 1, right = 2, sorted[1] + sorted[2] = 5 + 6 = 11 > 9,
//         decrementing right to 1; at left = 1, right = 1, 5 + 5 = 10 > 9
//         decrements right to 0, ending the loop and returning 4.
//
//   Example 2:
//     Input:  nums = [3, 3, 6, 8], target = 10
//     Output: 6
//     Explanation:
//       - Sort nums to [3, 3, 6, 8], precompute pow2 = [1, 2, 4, 8], and start with
//         left = 0, right = 3, result = 0.
//       - At left = 0, right = 3, 3 + 8 = 11 > 10, so decrement right to 2.
//       - At left = 0, right = 2, 3 + 6 = 9 <= 10, so add pow2[2] = 4 (result = 4)
//         and advance left to 1.
//       - At left = 1, right = 2, 3 + 6 = 9 <= 10, so add pow2[1] = 2 (result = 6)
//         and advance left to 2; at left = 2, right = 2, 6 + 6 = 12 > 10 decrements
//         right to 1 and exits, returning 6.
//
//   Example 3:
//     Input:  nums = [5, 6, 7], target = 9
//     Output: 0
//     Explanation:
//       - Sort nums to [5, 6, 7], precompute pow2 = [1, 2, 4], and initialize left
//         = 0, right = 2, result = 0.
//       - At left = 0, right = 2, 5 + 7 = 12 > 9 decrements right to 1, and at left
//         = 0, right = 1, 5 + 6 = 11 > 9 decrements right to 0.
//       - At left = 0, right = 0, 5 + 5 = 10 > 9 triggers the right == 0 guard to
//         break out of the loop and return 0.
//
// Algorithm:
//   Key insight: Whether a subsequence satisfies `min + max <= target` depends
//   only on the chosen multiset of elements, not on their original order in
//   `nums` (every non-empty subset of indices induces exactly one subsequence).
//   1. Sort a copy of `nums` in non-decreasing order.
//   2. Precompute `pow2[k] = 2^k mod (10^9 + 7)` for `0 <= k < n`.
//   3. Maintain two pointers `left = 0` and `right = n - 1`:
//      - While `left <= right`:
//        * If `sorted[left] + sorted[right] <= target`, then fixing
//          `sorted[left]` as the minimum element allows any subset of the
//          `right - left` remaining elements in `(left, right]` to be included
//          or excluded freely (since every chosen element is in
//          `[sorted[left], sorted[right]]`). There are
//          `sum_{k=0}^{right-left} C(right - left, k) = 2^(right - left)` such
//          subsequences. Add `pow2[right - left]` modulo `10^9 + 7` and advance
//          `++left`.
//        * Otherwise `sorted[right]` is too large to pair with `sorted[left]`
//          (and therefore with any larger element to the right of `left`), so
//          decrement `right`.
//   Time Complexity:  O(n log n) due to sorting; precomputation and the
//                     two-pointer scan are each O(n).
//   Space Complexity: O(n) for the sorted copy and the `pow2` table.
//
// Data Structures:
//   - Sorted array (`std::vector<int>`): orders values so opposite-end two
//     pointers can identify the largest valid maximum for each minimum in O(n)
//     amortized steps.
//   - Modular power lookup table (`std::vector<int>`): stores `2^k mod M` for
//     O(1) combinatorial subset counting without exponentiation overhead or
//     integer overflow.
//
// Real-World Applications:
//   - Portfolio and risk-budget selection: counting valid asset subsets whose
//     extreme (min + max) risk or spread stays within a regulatory threshold.
//   - Hardware/bin-packing feasibility analysis: counting valid component
//     configurations whose combined minimum and maximum load or latency meets
//     an SLA envelope.
//   - Combinatorial database query cardinality estimation: bounding result set
//     sizes for range-constrained subset predicates.
//
// Related LeetCode Problems:
//   - #167 Two Sum II [done] - Input Array Is Sorted (opposite-ends two pointers on a sorted array)
//   - #15 3Sum [done] (sorting combined with two-pointer window shrinking)
//   - #259 3Sum Smaller [done] (counting combinations under an inequality bound via two pointers)
//   - #611 Valid Triangle Number [done] (sorting and two-pointer counting of valid subsets)
//   - #881 Boats to Save People [done] (pairing min and max elements under a target sum limit)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr int kMod = 1'000'000'007;

class Solution {
 public:
  // Counts non-empty subsequences of `nums` where min + max <= `target`,
  // modulo 10^9 + 7.
  // Time: O(n log n), Space: O(n).
  int numSubseq(const std::vector<int>& nums, int target) {
    if (nums.empty()) {
      return 0;
    }

    // Sorting does not change the count of valid subsequences because validity
    // depends solely on a subset's minimum and maximum values.
    std::vector<int> sorted = nums;
    std::ranges::sort(sorted);

    const std::size_t n = sorted.size();

    // Precompute powers of 2 modulo kMod so each valid window contributes in
    // O(1) time without overflow.
    std::vector<int> pow2(n, 1);
    for (std::size_t i = 1; i < n; ++i) {
      pow2[i] = (pow2[i - 1] * 2) % kMod;
    }

    std::size_t left = 0;
    std::size_t right = n - 1;
    int result = 0;

    // Two-pointer sweep: `sorted[left]` is the fixed minimum; shrink `right`
    // until `sorted[right]` is the largest valid partner for `sorted[left]`.
    while (left <= right) {
      const std::int64_t pair_sum =
          static_cast<std::int64_t>(sorted[left]) + sorted[right];
      if (pair_sum <= target) {
        // Every subset of indices in (left, right] forms a valid subsequence
        // together with `sorted[left]`, giving 2^(right - left) choices.
        result = (result + pow2[right - left]) % kMod;
        ++left;
      } else {
        // Guard against unsigned underflow when even sorted[0] + sorted[0] > target.
        if (right == 0) {
          break;
        }
        --right;
      }
    }

    return result;
  }
};

LC_PROBLEM(1498, "Number Of Subsequences That Satisfy The Given Sum Condition",
           "math", ctx) {
  Solution sol;

  ctx.test("example_1_basic", [&] {
    ctx.expect_eq(sol.numSubseq({3, 5, 6, 7}, 9), 4);
  });

  ctx.test("example_2_duplicates", [&] {
    ctx.expect_eq(sol.numSubseq({3, 3, 6, 8}, 10), 6);
  });

  ctx.test("example_3_mixed", [&] {
    ctx.expect_eq(sol.numSubseq({2, 3, 3, 4, 6, 7}, 12), 61);
  });

  ctx.test("example_4_all_subsequences_valid", [&] {
    // All 2^7 - 1 = 127 non-empty subsequences satisfy min + max <= 16.
    ctx.expect_eq(sol.numSubseq({5, 2, 4, 1, 7, 6, 8}, 16), 127);
  });

  ctx.test("build_case_and_edge_cases", [&] {
    // Valid subsequences: [3], [3,5], [3,6], [3,5,6], [5] -> 5.
    ctx.expect_eq(sol.numSubseq({3, 5, 6, 8}, 10), 5);
    ctx.expect_eq(sol.numSubseq({5, 6, 7}, 9), 0);
    ctx.expect_eq(sol.numSubseq({5}, 9), 0);
    ctx.expect_eq(sol.numSubseq({4}, 8), 1);
    ctx.expect_eq(sol.numSubseq({}, 10), 0);
  });

  ctx.test("large_modulo_wraparound", [&] {
    // 35 ones with target = 2: all (2^35 - 1) non-empty subsequences are valid.
    // (34359738368 - 1) % 1000000007 == 359738129.
    const std::vector<int> ones(35, 1);
    ctx.expect_eq(sol.numSubseq(ones, 2), 359738129);
  });
}

}  // namespace
