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
// Examples:
//   Example 1:
//     Input:  arr = [2, 1, 4, 7, 3, 2, 5]
//     Output: 5
//     Explanation:
//       - Initialize up = 0, down = 0, and best = 0 before scanning adjacent pairs
//         from index 1.
//       - At i = 1 (2 -> 1), prev > curr increments down to 1 while up remains 0,
//         so no valid mountain exists yet.
//       - At i = 2 (1 -> 4), down > 0 && prev < curr resets up = 0 and down = 0
//         before incrementing up to 1; at i = 3 (4 -> 7), up becomes 2.
//       - At i = 4 (7 -> 3), down increments to 1, and since up > 0 && down > 0,
//         best updates to 2 + 1 + 1 = 4.
//       - At i = 5 (3 -> 2), down increments to 2, updating best to 2 + 2 + 1 = 5
//         for the subarray [1, 4, 7, 3, 2].
//       - At i = 6 (2 -> 5), down > 0 && prev < curr resets both counters and sets
//         up = 1, leaving best = 5 as the final return value.
//
//   Example 2:
//     Input:  arr = [0, 1, 0, 2, 3, 4, 2, 1]
//     Output: 6
//     Explanation:
//       - Pairs (0 -> 1) at i = 1 and (1 -> 0) at i = 2 set up = 1 and down = 1,
//         recording best = 1 + 1 + 1 = 3 for [0, 1, 0].
//       - At i = 3 (0 -> 2), down > 0 && prev < curr resets up = 0 and down = 0
//         before incrementing up to 1, starting a new mountain from the shared
//         valley 0.
//       - Ascending pairs (2 -> 3) at i = 4 and (3 -> 4) at i = 5 advance up to 3
//         while down stays 0.
//       - Descending pairs (4 -> 2) at i = 6 and (2 -> 1) at i = 7 increment down
//         to 1 (best = 5) and then 2, updating best to 3 + 2 + 1 = 6 for [0, 2, 3,
//         4, 2, 1].
//
//   Example 3:
//     Input:  arr = [1, 2, 2, 1]
//     Output: 0
//     Explanation:
//       - At i = 1 (1 -> 2), up increments to 1 while down remains 0.
//       - At i = 2 (2 -> 2), prev == curr resets both up and down to 0 because a
//         plateau breaks any mountain.
//       - At i = 3 (2 -> 1), down increments to 1 while up stays 0, so up > 0 &&
//         down > 0 is never satisfied and 0 is returned.
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
