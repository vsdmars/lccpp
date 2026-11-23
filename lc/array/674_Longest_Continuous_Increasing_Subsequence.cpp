// LeetCode 674: Longest Continuous Increasing Subsequence
// https://leetcode.com/problems/longest-continuous-increasing-subsequence/
//
// Problem:
//   Given an unsorted array of integers `nums`, return the length of the
//   longest strictly increasing contiguous subarray (continuous increasing
//   subsequence). Return 0 if `nums` is empty.
//
// Algorithm:
//   Single-Pass Sliding Run-Length Scan (O(n) time, O(1) space):
//     - Unlike general Longest Increasing Subsequence (LC #300), contiguity
//       means any position where `nums[i] <= nums[i - 1]` immediately breaks
//       the current increasing run and starts a fresh run of length 1.
//     - Maintain `cur_len` (length of the strictly increasing contiguous run
//       ending at index `i`) and `best_len` (global maximum over all runs).
//     - For each `i` from `1` to `n - 1`:
//         * If `nums[i] > nums[i - 1]`, increment `cur_len` and update
//           `best_len = max(best_len, cur_len)`.
//         * Otherwise, reset `cur_len = 1`.
//
//   Time Complexity: O(n) — single linear scan.
//   Space Complexity: O(1) — two scalar counters.
//
// Data Structures:
//   - Two scalar integer counters (`cur_len`, `best_len`) scanning `nums`
//     directly without auxiliary allocations.
//
// Real-World Applications:
//   - Financial and quantitative analytics detecting the longest consecutive
//     winning streak or uninterrupted upward price trend.
//   - Telemetry and monitoring systems identifying sustained monotonic growth
//     (e.g., continuous memory-leak ramps or thermal runaway intervals).
//   - Adaptive sorting engines (Timsort / natural merge sort) measuring
//     natural ascending run lengths in input data.
//
// Related LeetCode Problems:
//   - #300 Longest Increasing Subsequence [done] (non-contiguous variant via DP / patience sorting)
//   - #581 Shortest Unsorted Continuous Subarray [done] (uses monotonic prefix/suffix runs)
//   - #665 Non-decreasing Array [done] (near-monotonic sequence validation)
//   - #673 Number of Longest Increasing Subsequence [done] (counts optimal non-contiguous LIS)
//   - #845 Longest Mountain in Array [done] (pairs an increasing contiguous run with a decreasing one)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the length of the longest strictly increasing contiguous subarray
  // in `nums`.
  //
  // Invariant:
  //   At the end of iteration `i`, `cur_len` equals the length of the longest
  //   strictly increasing contiguous subarray ending at index `i`, and
  //   `best_len` is the maximum `cur_len` observed over `0..i`.
  //
  // Time Complexity: O(n).
  // Space Complexity: O(1).
  int findLengthOfLCIS(const std::vector<int>& nums) {
    if (nums.empty()) {
      return 0;
    }

    int best_len = 1;
    int cur_len = 1;

    for (std::size_t i = 1; i < nums.size(); ++i) {
      if (nums[i] > nums[i - 1]) {
        ++cur_len;
        best_len = std::max(best_len, cur_len);
      } else {
        // Equal or decreasing step ends the current strictly increasing run.
        cur_len = 1;
      }
    }

    return best_len;
  }
};

LC_PROBLEM(674, "Longest Continuous Increasing Subsequence", "array", ctx) {
  Solution sol;

  ctx.test("example_1_standard", [&] {
    ctx.expect_eq(sol.findLengthOfLCIS({1, 3, 5, 4, 7}), 3);
  });

  ctx.test("example_2_all_equal", [&] {
    ctx.expect_eq(sol.findLengthOfLCIS({2, 2, 2, 2, 2}), 1);
  });

  ctx.test("python_build_cases", [&] {
    ctx.expect_eq(sol.findLengthOfLCIS({1, 7, 2, 9, 5}), 2);
    ctx.expect_eq(sol.findLengthOfLCIS({}), 0);
    ctx.expect_eq(
        sol.findLengthOfLCIS({1, 3, 5, 7, 9, 11, 12, 3, 7, 9, 10, 12}), 7);
  });

  ctx.test("single_and_monotonic_extremes", [&] {
    ctx.expect_eq(sol.findLengthOfLCIS({42}), 1);
    ctx.expect_eq(sol.findLengthOfLCIS({1, 2, 3, 4, 5}), 5);
    ctx.expect_eq(sol.findLengthOfLCIS({5, 4, 3, 2, 1}), 1);
  });

  ctx.test("longest_run_at_tail_with_negatives", [&] {
    ctx.expect_eq(sol.findLengthOfLCIS({10, -5, -3, -1, 0, 4}), 5);
  });
}

}  // namespace
