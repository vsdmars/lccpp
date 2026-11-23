// LeetCode 300: Longest Increasing Subsequence
// https://leetcode.com/problems/longest-increasing-subsequence/
//
// Problem:
//   Given an unsorted integer array nums, return the length of the longest
//   strictly increasing subsequence. A subsequence preserves relative order
//   of elements from nums without requiring them to be contiguous.
//
// Algorithm:
//   1. Patience Sorting with Binary Search (optimal, O(n log n) time, O(n) space):
//      Maintain a vector `tails` where `tails[k]` stores the smallest possible
//      ending value of any strictly increasing subsequence of length k + 1
//      discovered so far. Because `tails` is always strictly increasing:
//        - For each number `num` in nums, binary-search (`std::ranges::lower_bound`)
//          for the first element in `tails` that is >= num.
//        - If no such element exists, `num` extends the longest subsequence so
//          far, so append it to `tails`.
//        - Otherwise, replace that element with `num` to tighten the tail for
//          that length, making it easier for future values to extend it.
//      Note that `tails` itself may not form an actual subsequence, but its
//      length is always the exact LIS length.
//   2. Classic Dynamic Programming (O(n^2) time, O(n) space):
//      Included as a reference implementation: `dp[i]` is the length of the
//      longest strictly increasing subsequence ending at index i.
//
// Data Structures:
//   - `std::vector<int> tails`: sorted array of minimal subsequence tail
//     values supporting O(log n) binary search via `std::ranges::lower_bound`.
//   - `std::vector<int> dp` (in the O(n^2) variant): 1D DP table.
//
// Real-World Applications:
//   - Patience diff algorithms (used by Git and Bazaar) to compute cleaner,
//     more human-readable file diffs by anchoring on unique matching lines.
//   - Bioinformatics sequence alignment and synteny block chaining (identifying
//     longest collinear sets of matching anchors between genomes).
//   - Job scheduling and box-stacking / envelope-nesting optimization problems.
//
// Related LeetCode Problems:
//   - #334 Increasing Triplet Subsequence [done] (specialized LIS check for length >= 3 in O(n) time, O(1) space)
//   - #354 Russian Doll Envelopes [done] (2D LIS via sorting width ascending and height descending)
//   - #673 Number of Longest Increasing Subsequence [done] (counting all LIS instances)
//   - #674 Longest Continuous Increasing Subsequence [done] (contiguous subarray variant in O(n))
//   - #1964 Find the Longest Valid Obstacle Course at Each Position [new] (non-decreasing LIS prefix lengths via upper_bound)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the length of the longest strictly increasing subsequence using
  // patience sorting + binary search.
  //
  // Invariant: `tails` is strictly increasing and `tails[k]` is the minimum
  // possible tail value among all strictly increasing subsequences of length
  // k + 1 over the processed prefix of `nums`.
  // Time: O(n log n), Space: O(n).
  int lengthOfLIS(const std::vector<int>& nums) {
    std::vector<int> tails;
    tails.reserve(nums.size());

    for (const int num : nums) {
      // Find the leftmost pile whose top is >= num (strictly increasing requires
      // lower_bound so duplicates replace rather than extend a pile).
      const auto it = std::ranges::lower_bound(tails, num);
      if (it == tails.end()) {
        // num is strictly greater than every current tail, extending the LIS.
        tails.push_back(num);
      } else {
        // Replace *it with the smaller (or equal) num to keep future options wide.
        *it = num;
      }
    }

    return static_cast<int>(tails.size());
  }

  // Reference O(n^2) dynamic programming solution mentioned in the problem
  // statement. Useful for cross-checking against the O(n log n) algorithm.
  // Time: O(n^2), Space: O(n).
  int lengthOfLISDP(const std::vector<int>& nums) {
    if (nums.empty()) {
      return 0;
    }

    const std::size_t n = nums.size();
    std::vector<int> dp(n, 1);
    int best = 1;

    for (std::size_t i = 1uz; i < n; ++i) {
      for (std::size_t j = 0uz; j < i; ++j) {
        if (nums[j] < nums[i]) {
          dp[i] = std::max(dp[i], dp[j] + 1);
        }
      }
      best = std::max(best, dp[i]);
    }

    return best;
  }
};

LC_PROBLEM(300, "Longest Increasing Subsequence", "binary_search", ctx) {
  ctx.test("docstring_and_build_examples", [&] {
    Solution sol;
    const std::vector<int> ex1 = {10, 9, 2, 5, 3, 7, 101, 18};
    const std::vector<int> ex2 = {5, 7, 3, 8};
    const std::vector<int> ex3 = {7, 7, 7, 7};
    const std::vector<int> ex4 = {2, 2, 2};
    const std::vector<int> ex5 = {0, 1, 0, 3, 2, 3};

    ctx.expect_eq(sol.lengthOfLIS(ex1), 4);
    ctx.expect_eq(sol.lengthOfLIS(ex2), 3);
    ctx.expect_eq(sol.lengthOfLIS(ex3), 1);
    ctx.expect_eq(sol.lengthOfLIS(ex4), 1);
    ctx.expect_eq(sol.lengthOfLIS(ex5), 4);

    ctx.expect_eq(sol.lengthOfLISDP(ex1), 4);
    ctx.expect_eq(sol.lengthOfLISDP(ex2), 3);
    ctx.expect_eq(sol.lengthOfLISDP(ex3), 1);
    ctx.expect_eq(sol.lengthOfLISDP(ex5), 4);
  });

  ctx.test("edge_cases_and_monotonic_arrays", [&] {
    Solution sol;
    const std::vector<int> empty_vec;
    const std::vector<int> single = {42};
    const std::vector<int> strictly_inc = {-10, -5, 0, 5, 10};
    const std::vector<int> strictly_dec = {10, 5, 0, -5, -10};

    ctx.expect_eq(sol.lengthOfLIS(empty_vec), 0);
    ctx.expect_eq(sol.lengthOfLIS(single), 1);
    ctx.expect_eq(sol.lengthOfLIS(strictly_inc), 5);
    ctx.expect_eq(sol.lengthOfLIS(strictly_dec), 1);

    ctx.expect_eq(sol.lengthOfLISDP(empty_vec), 0);
    ctx.expect_eq(sol.lengthOfLISDP(strictly_inc), 5);
    ctx.expect_eq(sol.lengthOfLISDP(strictly_dec), 1);
  });
}

}  // namespace
