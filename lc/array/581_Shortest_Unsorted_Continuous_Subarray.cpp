// LeetCode 581: Shortest Unsorted Continuous Subarray
// https://leetcode.com/problems/shortest-unsorted-continuous-subarray/
//
// Problem:
//   Given an integer array `nums`, find one continuous subarray such that if
//   you sort only this subarray in non-decreasing order, the entire array
//   becomes sorted in non-decreasing order. Return the length of the shortest
//   such subarray, or 0 if `nums` is already sorted.
//
// Algorithm:
//   1. Optimal Two-Pointer Boundary Expansion (O(n) time, O(1) space):
//      - Advance `left` from the start while `nums[left] <= nums[left + 1]` to
//        locate the first out-of-order position. If `left` reaches the end, the
//        array is already sorted and the answer is 0.
//      - Retreat `right` from the end while `nums[right] >= nums[right - 1]` to
//        locate the last out-of-order position.
//      - Compute the minimum (`sub_min`) and maximum (`sub_max`) values within
//        the candidate window `nums[left..right]`.
//      - Expand `left` leftward while `nums[left - 1] > sub_min`, because any
//        prefix element strictly greater than `sub_min` must move rightward.
//      - Expand `right` rightward while `nums[right + 1] < sub_max`, because
//        any suffix element strictly smaller than `sub_max` must move leftward.
//      - The minimal unsorted window length is `right - left + 1`.
//   2. Sort-and-Compare Baseline (O(n log n) time, O(n) space):
//      - Copy and sort `nums`, then find the leftmost and rightmost indices
//        where `nums[i] != sorted_nums[i]`.
//
// Data Structures:
//   - Optimal method: scalar index pointers (`left`, `right`) and scalar
//     extrema (`sub_min`, `sub_max`) operating directly on the input vector.
//   - Baseline method: an auxiliary sorted `std::vector<int>` copy for element-
//     wise comparison.
//
// Real-World Applications:
//   - Incremental database index repair and log-structured merge (LSM) compaction
//     identifying the minimal out-of-order slice needing re-sorting.
//   - Financial time-series ingestion and telemetry pipelines repairing small
//     late-arriving out-of-order windows without re-sorting entire partitions.
//   - Adaptive sorting algorithms (such as Timsort) detecting already-sorted
//     prefix/suffix runs to minimize comparison work.
//
// Related LeetCode Problems:
//   - #665 Non-decreasing Array [done] (checks if an array is nearly sorted within one edit)
//   - #674 Longest Continuous Increasing Subsequence [done] (scans contiguous monotonic runs)
//   - #768 Max Chunks To Make Sorted II [new] (partitions an array using prefix max and suffix min)
//   - #769 Max Chunks To Make Sorted [new] (simpler permutation variant of sorted chunk boundaries)
//   - #1574 Shortest Subarray to be Removed to Make Array Sorted [new] (removes instead of sorts a minimal window)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the length of the shortest continuous subarray whose sorting makes
  // the entire array non-decreasing using two-pointer boundary expansion.
  //
  // Invariant:
  //   After expanding `left` and `right`, every element in `nums[0..left-1]`
  //   is <= every element in `nums[left..n-1]`, and every element in
  //   `nums[right+1..n-1]` is >= every element in `nums[0..right]`.
  //
  // Time Complexity: O(n) — each index is visited at most twice.
  // Space Complexity: O(1) — only constant scalar state is maintained.
  int findUnsortedSubarray(const std::vector<int>& nums) {
    const std::size_t n = nums.size();
    if (n <= 1) {
      return 0;
    }

    // Phase 1: Locate the first index from the left that violates ascending
    // order. Reaching `n - 1` confirms the entire array is already sorted.
    std::size_t left = 0;
    while (left + 1 < n && nums[left] <= nums[left + 1]) {
      ++left;
    }
    if (left == n - 1) {
      return 0;
    }

    // Phase 2: Locate the first index from the right that violates ascending
    // order. Guaranteed to stop before `left` because an inversion exists.
    std::size_t right = n - 1;
    while (right > 0 && nums[right - 1] <= nums[right]) {
      --right;
    }

    // Phase 3: Compute the minimum and maximum within the unsorted core
    // `nums[left..right]` so we know how far values must shift outward.
    int sub_min = nums[left];
    int sub_max = nums[left];
    for (std::size_t i = left + 1; i <= right; ++i) {
      sub_min = std::min(sub_min, nums[i]);
      sub_max = std::max(sub_max, nums[i]);
    }

    // Phase 4: Expand `left` leftward over any prefix elements larger than
    // `sub_min` (they must appear after `sub_min` in the sorted result).
    while (left > 0 && nums[left - 1] > sub_min) {
      --left;
    }

    // Phase 5: Expand `right` rightward over any suffix elements smaller than
    // `sub_max` (they must appear before `sub_max` in the sorted result).
    while (right + 1 < n && nums[right + 1] < sub_max) {
      ++right;
    }

    return static_cast<int>(right - left + 1);
  }

  // Reference sort-and-compare approach matching the original Python solution.
  //
  // Time Complexity: O(n log n) due to sorting a full copy of `nums`.
  // Space Complexity: O(n) for the sorted copy.
  int findUnsortedSubarraySort(const std::vector<int>& nums) {
    const std::size_t n = nums.size();
    if (n <= 1) {
      return 0;
    }

    std::vector<int> sorted_nums = nums;
    std::ranges::sort(sorted_nums);

    // Find the leftmost mismatch between the original and sorted arrays.
    std::size_t left = 0;
    while (left < n && nums[left] == sorted_nums[left]) {
      ++left;
    }
    if (left == n) {
      return 0;
    }

    // Find the rightmost mismatch between the original and sorted arrays.
    std::size_t right = n - 1;
    while (right > left && nums[right] == sorted_nums[right]) {
      --right;
    }

    return static_cast<int>(right - left + 1);
  }
};

LC_PROBLEM(581, "Shortest Unsorted Continuous Subarray", "array", ctx) {
  Solution sol;

  ctx.test("example_1_middle_window", [&] {
    const std::vector<int> nums = {2, 6, 4, 8, 10, 9, 15};
    ctx.expect_eq(sol.findUnsortedSubarray(nums), 5);
    ctx.expect_eq(sol.findUnsortedSubarraySort(nums), 5);
  });

  ctx.test("already_sorted", [&] {
    const std::vector<int> nums = {1, 2, 3, 4};
    ctx.expect_eq(sol.findUnsortedSubarray(nums), 0);
    ctx.expect_eq(sol.findUnsortedSubarraySort(nums), 0);
  });

  ctx.test("single_and_empty", [&] {
    ctx.expect_eq(sol.findUnsortedSubarray({1}), 0);
    ctx.expect_eq(sol.findUnsortedSubarray({}), 0);
    ctx.expect_eq(sol.findUnsortedSubarraySort({1}), 0);
    ctx.expect_eq(sol.findUnsortedSubarraySort({}), 0);
  });

  ctx.test("fully_reversed", [&] {
    const std::vector<int> nums = {5, 4, 3, 2, 1};
    ctx.expect_eq(sol.findUnsortedSubarray(nums), 5);
    ctx.expect_eq(sol.findUnsortedSubarraySort(nums), 5);
  });

  ctx.test("duplicates_expand_boundary", [&] {
    // `3` must move past all trailing `2`s, so the window is [3, 2, 2, 2].
    const std::vector<int> nums = {1, 3, 2, 2, 2};
    ctx.expect_eq(sol.findUnsortedSubarray(nums), 4);
    ctx.expect_eq(sol.findUnsortedSubarraySort(nums), 4);
  });

  ctx.test("late_small_value_expands_left", [&] {
    // `3` at the end is smaller than both `4` and `5`, expanding left to 4.
    const std::vector<int> nums = {1, 2, 4, 5, 3};
    ctx.expect_eq(sol.findUnsortedSubarray(nums), 3);
    ctx.expect_eq(sol.findUnsortedSubarraySort(nums), 3);
  });

  ctx.test("negative_values", [&] {
    const std::vector<int> nums = {-5, -1, -3, -2, 0};
    ctx.expect_eq(sol.findUnsortedSubarray(nums), 3);
    ctx.expect_eq(sol.findUnsortedSubarraySort(nums), 3);
  });
}

}  // namespace
