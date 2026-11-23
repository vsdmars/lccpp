// LeetCode 845: Longest Mountain In Array
// https://leetcode.com/problems/longest-mountain-in-array/
//
// Problem:
//   Given an integer array `arr`, return the length of the longest contiguous
//   subarray that forms a "mountain". A subarray `B` is a mountain iff:
//     - `B.length >= 3`, and
//     - there exists a peak index `0 < i < B.length - 1` such that
//       `B[0] < B[1] < ... < B[i] > B[i + 1] > ... > B[B.length - 1]`.
//   Return 0 if no mountain subarray exists.
//   Constraints: 1 <= arr.length <= 10^4, 0 <= arr[i] <= 10^4.
//   Follow-up: solve in a single pass with O(1) extra space.
//
// Algorithm:
//   Single-pass state tracking with two counters (`up`, `down`):
//   - Walk adjacent pairs `(prev, curr) = (arr[i - 1], arr[i])` from left to
//     right while tracking the length of the current strictly-increasing run
//     (`up`) and strictly-decreasing run (`down`).
//   - Reset both `up` and `down` to 0 whenever we hit a plateau (`prev == curr`)
//     or begin a fresh ascent after already descending (`down > 0 && prev < curr`),
//     since the previous mountain has ended at `prev` and a new candidate starts
//     from `prev`.
//   - Increment `up` if `prev < curr`, or `down` if `prev > curr`.
//   - Whenever both `up > 0` and `down > 0`, the current subarray is a valid
//     mountain of length `up + down + 1`; update `best`.
//   Time Complexity:  O(N) — single linear pass.
//   Space Complexity: O(1) — only scalar counters.
//
// Data Structures:
//   - Scalar integer state counters (`up`, `down`, `best`): encode the rising
//     slope length, falling slope length, and best mountain length seen so far
//     without auxiliary arrays.
//
// Real-World Applications:
//   - Time-series peak and pulse detection in telemetry, ECG/PPG biosignals,
//     and sensor data (identifying widest unimodal rise-and-fall episodes).
//   - Financial chart pattern scanning for rally-and-pullback intervals.
//   - Elevation profile analysis in GPS/GIS routing to identify longest
//     continuous climb-and-descent segments.
//
// Related LeetCode Problems:
//   - #941 Valid Mountain Array [done] (checks whether the entire array is one mountain)
//   - #852 Peak Index in a Mountain Array [done] (binary search for the peak of a mountain)
//   - #1671 Minimum Number of Removals to Make Mountain Array [new] (subsequence mountain via LIS/LDS)
//   - #1095 Find in Mountain Array [new] (interactive binary search on a unimodal array)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the length of the longest strictly-rising-then-strictly-falling
  // contiguous subarray of `arr`, or 0 if none exists.
  // Invariant: At index `i`, if `up > 0 && down > 0`, then the subarray ending
  // at `i` of length `up + down + 1` is a valid mountain whose peak is at
  // `i - down`.
  // Time: O(N), Space: O(1).
  int longestMountain(const std::vector<int>& arr) {
    if (arr.size() < 3uz) {
      return 0;
    }

    int up = 0;    // Length of strictly increasing slope of current candidate.
    int down = 0;  // Length of strictly decreasing slope of current candidate.
    int best = 0;

    for (std::size_t i = 1uz; i < arr.size(); ++i) {
      const int prev = arr[i - 1uz];
      const int curr = arr[i];

      // A plateau breaks any mountain; starting a new ascent after a descent
      // finishes the previous mountain at `prev`, so reset counters before
      // counting the new edge (prev -> curr).
      if ((down > 0 && prev < curr) || prev == curr) {
        up = 0;
        down = 0;
      }

      if (prev < curr) {
        ++up;
      } else if (prev > curr) {
        ++down;
      }

      // Only a non-empty ascent followed by a non-empty descent forms a valid
      // mountain (length = up edges + down edges + 1 peak/vertex offset).
      if (up > 0 && down > 0) {
        best = std::max(best, up + down + 1);
      }
    }

    return best;
  }
};

LC_PROBLEM(845, "Longest Mountain In Array", "two_pointers", ctx) {
  Solution sol;

  ctx.test("example_1_mountain_of_5", [&] {
    ctx.expect_eq(sol.longestMountain({2, 1, 4, 7, 3, 2, 5}), 5);
  });

  ctx.test("example_2_flat_array", [&] {
    ctx.expect_eq(sol.longestMountain({2, 2, 2}), 0);
  });

  ctx.test("python_build_cases", [&] {
    // Mountain is [2, 9, 8, 0] -> length 4.
    ctx.expect_eq(sol.longestMountain({4, 2, 9, 8, 0}), 4);
    // Mountain is [1, 4, 7, 3, 2, 1] -> length 6.
    ctx.expect_eq(sol.longestMountain({2, 1, 4, 7, 3, 2, 1}), 6);
    // Strictly decreasing -> no mountain.
    ctx.expect_eq(sol.longestMountain({5, 4, 3}), 0);
  });

  ctx.test("monotonic_and_plateau_edge_cases", [&] {
    ctx.expect_eq(sol.longestMountain({}), 0);
    ctx.expect_eq(sol.longestMountain({1, 2}), 0);
    ctx.expect_eq(sol.longestMountain({1, 2, 3, 4}), 0);
    // Plateau at peak disqualifies the mountain.
    ctx.expect_eq(sol.longestMountain({1, 2, 2, 1}), 0);
  });

  ctx.test("adjacent_mountains_sharing_valley", [&] {
    // [0, 1, 0] (len 3) followed by [0, 2, 3, 4, 2, 1] (len 6) sharing index 2.
    ctx.expect_eq(sol.longestMountain({0, 1, 0, 2, 3, 4, 2, 1}), 6);
  });
}

}  // namespace
