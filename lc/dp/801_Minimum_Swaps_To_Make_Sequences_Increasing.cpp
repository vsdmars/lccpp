// LeetCode 801: Minimum Swaps To Make Sequences Increasing
// https://leetcode.com/problems/minimum-swaps-to-make-sequences-increasing/
//
// Problem:
//   Given two equal-length integer arrays `nums1` and `nums2`, you may swap
//   `nums1[i]` and `nums2[i]` at any index `i`. Return the minimum number of
//   swaps needed so that both `nums1` and `nums2` become strictly increasing.
//   It is guaranteed that at least one valid sequence of swaps exists.
//
// Examples:
//   Example 1:
//     Input:  nums1 = [1, 3, 5, 4], nums2 = [1, 2, 3, 7]
//     Output: 1
//     Explanation:
//       - Initialize at index 0 with keep = 0 and swap = 1.
//       - At index 1, both natural (1 < 3, 1 < 2) and crossed (1 < 2, 1 < 3)
//         transitions hold, yielding keep = min(0, 1) = 0 and swap = min(2, 1) = 1.
//       - At index 2, only natural ordering (3 < 5, 2 < 3) holds since 3 < 3 is
//         false, updating keep = 0 and swap = 1 + 1 = 2.
//       - At index 3, natural ordering fails (5 < 4 is false) and only crossed
//         ordering (5 < 7, 3 < 4) holds, updating keep = swap = 2 and swap = keep +
//         1 = 1.
//       - Return min(keep, swap) = min(2, 1) = 1.
//
//   Example 2:
//     Input:  nums1 = [1, 20, 3, 40, 5], nums2 = [10, 2, 30, 4, 50]
//     Output: 2
//     Explanation:
//       - Initialize at index 0 with keep = 0 and swap = 1.
//       - At index 1, only crossed ordering (1 < 2, 10 < 20) holds, updating keep =
//         1 and swap = 0 + 1 = 1.
//       - At index 2, only crossed ordering (20 < 30, 2 < 3) holds, updating keep =
//         1 and swap = 1 + 1 = 2.
//       - At index 3, only crossed ordering (3 < 4, 30 < 40) holds, updating keep =
//         2 and swap = 1 + 1 = 2.
//       - At index 4, only crossed ordering (40 < 50, 4 < 5) holds, updating keep =
//         2 and swap = 2 + 1 = 3, then return min(2, 3) = 2.
//
//   Example 3:
//     Input:  nums1 = [5], nums2 = [2]
//     Output: 0
//     Explanation:
//       - Check the input length n = 1 against the early guard n <= 1.
//       - Return 0 immediately without entering the DP loop because single-element
//         sequences are already strictly increasing.
//
// Algorithm:
//   Two-state linear dynamic programming (state machine DP). At each index `i`,
//   maintain two values:
//     - `keep`: minimum swaps on prefix `0..i` when index `i` is NOT swapped.
//     - `swap`: minimum swaps on prefix `0..i` when index `i` IS swapped.
//   Between consecutive indices `i - 1` and `i`, two independent ordering
//   conditions determine valid transitions:
//     1. `natural_ok`: `nums1[i-1] < nums1[i] && nums2[i-1] < nums2[i]`
//        -> index `i` can preserve the same swap parity as `i - 1`
//           (`keep -> next_keep`, `swap + 1 -> next_swap`).
//     2. `crossed_ok`: `nums1[i-1] < nums2[i] && nums2[i-1] < nums1[i]`
//        -> index `i` can flip swap parity relative to `i - 1`
//           (`swap -> next_keep`, `keep + 1 -> next_swap`).
//   Taking the minimum across all valid transitions yields the global optimum
//   in a single pass.
//   Time complexity: O(N), where N = nums1.size().
//   Space complexity: O(1).
//
// Data Structures:
//   - Two scalar rolling DP states (`keep`, `swap`) representing the two
//     possible configurations at the current index.
//
// Real-World Applications:
//   - Dual-lane / parallel-track signal alignment where paired sensor channels
//     occasionally cross over and must be untangled with minimal swaps.
//   - Two-processor task assignment minimizing channel switches subject to
//     monotonic timestamp or precedence constraints.
//   - Viterbi-style 2-state trellis decoding with transition feasibility masks.
//
// Related LeetCode Problems:
//   - #300 Longest Increasing Subsequence [done] (monotonic subsequence DP)
//   - #309 Best Time to Buy and Sell Stock with Cooldown [done] (state-machine DP)
//   - #714 Best Time to Buy and Sell Stock with Transaction Fee [done] (2-state rolling DP)
//   - #1187 Make Array Strictly Increasing [new] (strictly increasing DP with replacements)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <vector>

namespace {

constexpr int kInf = std::numeric_limits<int>::max() / 2;

class Solution {
 public:
  // Returns the minimum number of index-aligned swaps required to make both
  // `nums1` and `nums2` strictly increasing.
  //
  // Invariant: After processing index `i`, `keep` and `swap` hold the minimum
  // swap counts for `0..i` ending in the unswapped and swapped states.
  // Time: O(N); Space: O(1).
  int minSwap(const std::vector<int>& nums1, const std::vector<int>& nums2) {
    const std::size_t n = nums1.size();
    if (n <= 1uz) {
      return 0;
    }

    // At index 0, keeping costs 0 swaps and swapping costs 1 swap.
    int keep = 0;
    int swap = 1;

    for (std::size_t i = 1uz; i < n; ++i) {
      int next_keep = kInf;
      int next_swap = kInf;

      // Same parity as index i - 1 preserves strict ordering on both arrays.
      if (nums1[i - 1uz] < nums1[i] && nums2[i - 1uz] < nums2[i]) {
        next_keep = std::min(next_keep, keep);
        next_swap = std::min(next_swap, swap + 1);
      }

      // Opposite parity from index i - 1 preserves strict ordering on both.
      if (nums1[i - 1uz] < nums2[i] && nums2[i - 1uz] < nums1[i]) {
        next_keep = std::min(next_keep, swap);
        next_swap = std::min(next_swap, keep + 1);
      }

      keep = next_keep;
      swap = next_swap;
    }

    return std::min(keep, swap);
  }
};

LC_PROBLEM(801, "Minimum Swaps To Make Sequences Increasing", "dp", ctx) {
  Solution sol;

  ctx.test("example_1_single_tail_swap", [&] {
    ctx.expect_eq(sol.minSwap({1, 3, 5, 4}, {1, 2, 3, 7}), 1);
  });

  ctx.test("example_2_single_head_swap", [&] {
    ctx.expect_eq(sol.minSwap({0, 3, 5, 8, 9}, {2, 1, 4, 6, 9}), 1);
  });

  ctx.test("already_strictly_increasing", [&] {
    ctx.expect_eq(sol.minSwap({1, 2, 3, 4}, {5, 6, 7, 8}), 0);
  });

  ctx.test("multiple_swaps_required", [&] {
    // Swapping indices 1 and 3 restores [1,2,3,4,5] and [10,20,30,40,50].
    ctx.expect_eq(sol.minSwap({1, 20, 3, 40, 5}, {10, 2, 30, 4, 50}), 2);
  });

  ctx.test("single_element", [&] {
    ctx.expect_eq(sol.minSwap({5}, {2}), 0);
  });
}

}  // namespace
