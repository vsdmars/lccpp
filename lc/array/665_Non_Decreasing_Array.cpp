// LeetCode 665: Non Decreasing Array
// https://leetcode.com/problems/non-decreasing-array/
//
// Problem:
//   Given an integer array `nums` of length `n`, determine whether it can be
//   made non-decreasing (`nums[i] <= nums[i + 1]` for every `0 <= i < n - 1`)
//   by modifying at most one element.
//
// Algorithm:
//   1. Optimal Single-Pass Greedy (O(n) time, O(1) space):
//      Scan left to right while maintaining the effective last two values
//      (`prev_prev` and `prev`) of the repaired prefix, plus a boolean
//      `modified` recording whether we have already spent our single edit.
//      Whenever we encounter an inversion `nums[i] < prev`:
//        - If `modified` is already true, a second edit is required -> false.
//        - Otherwise set `modified = true` and choose the greedy repair:
//          * Prefer lowering `nums[i - 1]` down to `nums[i]` whenever doing so
//            does not violate `prev_prev <= nums[i]` (or when `i == 1`, where
//            no `prev_prev` exists). Lowering keeps the prefix ceiling as small
//            as possible, giving future elements maximum room.
//          * Otherwise (`prev_prev > nums[i]`), lowering `nums[i - 1]` would
//            break `nums[i - 2] <= nums[i - 1]`, so we must raise `nums[i]` up
//            to `prev`.
//   2. Two-Candidate Verification (O(n) time, O(n) space, from Python source):
//      Locate the first inversion `nums[i] > nums[i + 1]`, test both single-
//      element repairs (`one[i] = nums[i + 1]` and `two[i + 1] = nums[i]`),
//      and check whether either candidate is sorted via `std::ranges::is_sorted`.
//
// Data Structures:
//   - Optimal method: constant scalar variables (`prev_prev`, `prev`,
//     `modified`) with zero heap allocations and no mutation of the input.
//   - Reference method: two auxiliary `std::vector<int>` copies.
//
// Real-World Applications:
//   - Sensor and time-series outlier sanitization (detecting whether a monotonic
//     counter or timestamp stream has at most one single-point glitch).
//   - Data quality validation in ETL pipelines checking near-monotonic sequence
//     numbers or version IDs before triggering a full re-index.
//   - Distributed clock / Lamport timestamp anomaly detection permitting a
//     single transient skew event.
//
// Related LeetCode Problems:
//   - #581 Shortest Unsorted Continuous Subarray [done] (measures extent of unsorted region)
//   - #674 Longest Continuous Increasing Subsequence [done] (tracks monotonic runs)
//   - #1909 Remove One Element to Make the Array Strictly Increasing [done] (strict deletion variant)
//   - #2111 Minimum Operations to Make the Array K-Increasing [new] (generalized edit distance to non-decreasing)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Determines in a single non-mutating pass whether `nums` can become
  // non-decreasing after modifying at most one element.
  //
  // Invariant:
  //   At the end of iteration `i`, the repaired prefix `nums[0..i]` is
  //   non-decreasing, ends with values `(prev_prev, prev)` (for `i >= 1`), and
  //   has the smallest achievable trailing value `prev` using at most one edit.
  //
  // Time Complexity: O(n).
  // Space Complexity: O(1).
  bool checkPossibility(const std::vector<int>& nums) {
    const std::size_t n = nums.size();
    if (n <= 2) {
      return true;
    }

    bool modified = false;
    int prev = nums[0];
    int prev_prev = 0;  // Valid whenever `i >= 2`.

    for (std::size_t i = 1; i < n; ++i) {
      if (nums[i] < prev) {
        if (modified) {
          return false;
        }
        modified = true;

        // Greedy choice: prefer lowering `nums[i - 1]` to `nums[i]` if it
        // preserves `prev_prev <= nums[i]`, because a smaller trailing value
        // makes it easier for subsequent elements to stay non-decreasing.
        if (i == 1 || prev_prev <= nums[i]) {
          prev_prev = nums[i];
          prev = nums[i];
        } else {
          // Lowering `nums[i - 1]` would break the previous pair, so we must
          // raise `nums[i]` up to `prev` (`prev` stays unchanged).
          prev_prev = prev;
        }
      } else {
        prev_prev = prev;
        prev = nums[i];
      }
    }

    return true;
  }

  // Reference two-candidate approach adapted from the Python solution, using
  // linear `std::ranges::is_sorted` checks instead of sorting.
  //
  // Time Complexity: O(n).
  // Space Complexity: O(n).
  bool checkPossibilityTwoCandidates(const std::vector<int>& nums) {
    const std::size_t n = nums.size();
    if (n <= 2) {
      return true;
    }

    std::vector<int> lower_left = nums;
    std::vector<int> raise_right = nums;

    for (std::size_t i = 0; i + 1 < n; ++i) {
      if (nums[i] > nums[i + 1]) {
        lower_left[i] = nums[i + 1];
        raise_right[i + 1] = nums[i];
        break;
      }
    }

    return std::ranges::is_sorted(lower_left) ||
           std::ranges::is_sorted(raise_right);
  }
};

LC_PROBLEM(665, "Non Decreasing Array", "array", ctx) {
  Solution sol;

  ctx.test("example_1_lower_first_element", [&] {
    const std::vector<int> nums = {4, 2, 3};
    ctx.expect_true(sol.checkPossibility(nums));
    ctx.expect_true(sol.checkPossibilityTwoCandidates(nums));
  });

  ctx.test("example_2_strictly_decreasing_false", [&] {
    const std::vector<int> nums = {4, 2, 1};
    ctx.expect_false(sol.checkPossibility(nums));
    ctx.expect_false(sol.checkPossibilityTwoCandidates(nums));
  });

  ctx.test("tricky_cannot_fix_both_sides", [&] {
    // At (4, 2), lowering 4 to 2 breaks (3, 2), and raising 2 to 4 breaks (4, 3).
    const std::vector<int> nums = {3, 4, 2, 3};
    ctx.expect_false(sol.checkPossibility(nums));
    ctx.expect_false(sol.checkPossibilityTwoCandidates(nums));
  });

  ctx.test("raise_second_element_succeeds", [&] {
    // Raising 2 to 3 yields [2, 3, 3, 3, 4].
    const std::vector<int> nums = {2, 3, 3, 2, 4};
    ctx.expect_true(sol.checkPossibility(nums));
    ctx.expect_true(sol.checkPossibilityTwoCandidates(nums));
  });

  ctx.test("lower_middle_element_succeeds", [&] {
    // Lowering 4 to 2 yields [1, 2, 2, 3].
    const std::vector<int> nums = {1, 4, 2, 3};
    ctx.expect_true(sol.checkPossibility(nums));
    ctx.expect_true(sol.checkPossibilityTwoCandidates(nums));
  });

  ctx.test("already_sorted_and_small_arrays", [&] {
    ctx.expect_true(sol.checkPossibility({1, 2, 3}));
    ctx.expect_true(sol.checkPossibility({1}));
    ctx.expect_true(sol.checkPossibility({5, 1}));
    ctx.expect_true(sol.checkPossibility({}));
  });

  ctx.test("raise_deep_drop_in_middle", [&] {
    // Raising 1 to 7 yields [5, 7, 7, 8].
    const std::vector<int> nums = {5, 7, 1, 8};
    ctx.expect_true(sol.checkPossibility(nums));
    ctx.expect_true(sol.checkPossibilityTwoCandidates(nums));
  });
}

}  // namespace
