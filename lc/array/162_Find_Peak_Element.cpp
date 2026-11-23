// LeetCode 162: Find Peak Element
// https://leetcode.com/problems/find-peak-element/
//
// Problem:
//   A peak element is an element that is strictly greater than its immediate
//   neighbors. Given a 0-indexed integer array `nums` where `nums[i] !=
//   nums[i + 1]` for all valid `i` and virtual sentinels `nums[-1] = nums[n] =
//   -infinity`, find any peak element and return its index.
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 2, 3, 1]
//     Output: 2
//     Explanation:
//       - Binary search initializes left = 0 and right = 3, then computes mid = 1
//         where nums[1] = 2 <= nums[2] = 3, moving left = 2.
//       - With left = 2 and right = 3, mid = 2 checks nums[2] = 3 > nums[3] = 1,
//         narrowing right = 2.
//       - Loop ends at left == right == 2 and returns 2 (the linear slope scan also
//         detects nums[3] < nums[2] and returns 2).
//
//   Example 2:
//     Input:  nums = [1, 2, 1, 3, 5, 6, 4]
//     Output: 5
//     Explanation:
//       - Binary search starts with left = 0, right = 6 and tests mid = 3 where
//         nums[3] = 3 <= nums[4] = 5, setting left = 4.
//       - At left = 4, right = 6, mid = 5 finds nums[5] = 6 > nums[6] = 4,
//         narrowing right = 5.
//       - At left = 4, right = 5, mid = 4 finds nums[4] = 5 <= nums[5] = 6, setting
//         left = 5 and returning valid peak index 5 (linear scan returns the
//         earlier valid peak index 1).
//
//   Example 3:
//     Input:  nums = [1, 2, 3, 4, 5]
//     Output: 4
//     Explanation:
//       - Binary search starts with left = 0, right = 4; at mid = 2, nums[2] = 3 <=
//         nums[3] = 4 moves left = 3.
//       - At left = 3, right = 4, mid = 3 sees nums[3] = 4 <= nums[4] = 5 and moves
//         left = 4.
//       - Loop exits at left == right == 4 and returns 4 (matching the linear scan
//         returning nums.size() - 1 = 4 after observing no drop).
//
// Algorithm:
//   1. Linear Slope Scan (`findPeakElement`):
//      Because `nums[-1] = -infinity`, the sequence starts on an upward slope.
//      Scanning left to right, the first index `i` where `nums[i] < nums[i - 1]`
//      marks the first downward step, meaning `i - 1` is strictly greater than
//      both its neighbors and is therefore a peak. If no downward step occurs,
//      the array is strictly increasing and the final index `n - 1` is a peak
//      (since `nums[n] = -infinity`).
//      Time Complexity: O(n), Space Complexity: O(1).
//   2. Binary Search on Slope (`findPeakElementBinarySearch`):
//      Maintain an invariant that a peak exists within the closed interval
//      `[left, right]` (because `nums[left - 1] < nums[left]` and
//      `nums[right] > nums[right + 1]`). Compare `nums[mid]` with
//      `nums[mid + 1]`: if `nums[mid] > nums[mid + 1]`, a peak must exist on
//      the left half `[left, mid]`; otherwise a peak must exist on the right
//      half `[mid + 1, right]`.
//      Time Complexity: O(log n), Space Complexity: O(1).
//
// Data Structures:
//   - Only scalar index variables (`std::size_t`); operates directly on the
//     read-only input `std::vector<int>` with no auxiliary allocation.
//
// Real-World Applications:
//   - Peak detection in 1D sensor telemetry, audio waveforms, and spectroscopy
//     (finding local maxima in sampled signals).
//   - Unimodal / hill-climbing parameter tuning in numerical optimization and
//     auto-focus lens positioning.
//   - Bisection search in hardware calibration curves where only local slope
//     comparisons are available.
//
// Related LeetCode Problems:
//   - #852 Peak Index in a Mountain Array [done] (guaranteed single peak via binary search)
//   - #1901 Find a Peak Element II [done] (2D grid generalization in O(m log n))
//   - #1095 Find in Mountain Array [new] (peak discovery followed by dual binary search)
//   - #33 Search in Rotated Sorted Array [done] (binary search on piecewise monotonic arrays)
//   - #153 Find Minimum in Rotated Sorted Array [done] (binary search for inflection point)

#include "framework/lc_framework.h"

#include <cstddef>
#include <limits>
#include <vector>

namespace {

class Solution {
 public:
  // Linear scan returning the index of the first local peak.
  // Invariant: prior to index `i`, nums[0..i-1] is strictly increasing, so the
  // first drop `nums[i] < nums[i - 1]` guarantees `i - 1` is a peak.
  // Time: O(n), Space: O(1).
  int findPeakElement(const std::vector<int>& nums) {
    if (nums.size() < 2uz) {
      return 0;
    }

    for (std::size_t i = 1uz; i < nums.size(); ++i) {
      if (nums[i] < nums[i - 1]) {
        return static_cast<int>(i - 1);
      }
    }

    // Strictly increasing throughout: the last element beats nums[n - 2] and
    // virtual sentinel nums[n] = -infinity.
    return static_cast<int>(nums.size() - 1);
  }

  // Optimal O(log n) binary search following the ascending slope toward a peak.
  // Invariant: a valid peak always lies inside [left, right].
  // Time: O(log n), Space: O(1).
  int findPeakElementBinarySearch(const std::vector<int>& nums) {
    if (nums.size() < 2uz) {
      return 0;
    }

    std::size_t left = 0uz;
    std::size_t right = nums.size() - 1;

    while (left < right) {
      // Midpoint strictly satisfies mid < right, so mid + 1 is always in range.
      const std::size_t mid = left + ((right - left) / 2);
      if (nums[mid] > nums[mid + 1]) {
        // Descending slope: a peak exists at mid or to its left.
        right = mid;
      } else {
        // Ascending slope: a peak exists strictly to the right of mid.
        left = mid + 1;
      }
    }

    return static_cast<int>(left);
  }
};

// Verifies that `idx` is a valid peak index under the -infinity boundary rules.
bool isPeakIndex(const std::vector<int>& nums, int idx) {
  if (idx < 0 || static_cast<std::size_t>(idx) >= nums.size()) {
    return false;
  }
  const auto u_idx = static_cast<std::size_t>(idx);
  const bool greater_than_left =
      (u_idx == 0uz) || (nums[u_idx] > nums[u_idx - 1]);
  const bool greater_than_right =
      (u_idx + 1 == nums.size()) || (nums[u_idx] > nums[u_idx + 1]);
  return greater_than_left && greater_than_right;
}

LC_PROBLEM(162, "Find Peak Element", "array", ctx) {
  Solution sol;

  ctx.test("example_1_single_peak", [&] {
    const std::vector<int> nums{1, 2, 3, 1};
    ctx.expect_eq(sol.findPeakElement(nums), 2);
    ctx.expect_eq(sol.findPeakElementBinarySearch(nums), 2);
  });

  ctx.test("example_2_multiple_peaks", [&] {
    const std::vector<int> nums{1, 2, 1, 3, 5, 6, 4};
    ctx.expect_true(isPeakIndex(nums, sol.findPeakElement(nums)));
    ctx.expect_true(isPeakIndex(nums, sol.findPeakElementBinarySearch(nums)));
  });

  ctx.test("small_arrays", [&] {
    ctx.expect_eq(sol.findPeakElement({1}), 0);
    ctx.expect_eq(sol.findPeakElementBinarySearch({1}), 0);
    ctx.expect_eq(sol.findPeakElement({1, 2}), 1);
    ctx.expect_eq(sol.findPeakElementBinarySearch({1, 2}), 1);
    ctx.expect_eq(sol.findPeakElement({2, 1}), 0);
    ctx.expect_eq(sol.findPeakElementBinarySearch({2, 1}), 0);
  });

  ctx.test("strictly_monotonic", [&] {
    const std::vector<int> asc{1, 2, 3, 4, 5};
    ctx.expect_eq(sol.findPeakElement(asc), 4);
    ctx.expect_eq(sol.findPeakElementBinarySearch(asc), 4);

    const std::vector<int> desc{5, 4, 3, 2, 1};
    ctx.expect_eq(sol.findPeakElement(desc), 0);
    ctx.expect_eq(sol.findPeakElementBinarySearch(desc), 0);
  });

  ctx.test("extreme_int_values", [&] {
    constexpr int kMin = std::numeric_limits<int>::min();
    constexpr int kMax = std::numeric_limits<int>::max();
    ctx.expect_eq(sol.findPeakElement({kMin}), 0);
    ctx.expect_eq(sol.findPeakElementBinarySearch({kMin}), 0);
    ctx.expect_eq(sol.findPeakElement({kMin, kMax}), 1);
    ctx.expect_eq(sol.findPeakElementBinarySearch({kMin, kMax}), 1);
  });
}

}  // namespace
