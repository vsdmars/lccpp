// LeetCode 452: Minimum Number Of Arrows To Burst Balloons
// https://leetcode.com/problems/minimum-number-of-arrows-to-burst-balloons/
//
// Problem:
//   Balloons are represented by closed horizontal intervals [x_start, x_end] on
//   the x-axis (where coordinates may span the full 32-bit signed integer
//   range). An arrow shot vertically at coordinate x bursts every balloon
//   satisfying x_start <= x <= x_end. Return the minimum number of arrows
//   required to burst all balloons.
//
// Algorithm:
//   This is the classic greedy interval-stabbing problem (dual of interval
//   scheduling):
//     1. Copy the intervals into a flat array of (start, end) pairs and sort
//        them in ascending order of their right endpoint `end`.
//     2. Shoot the first arrow at the rightmost point of the first balloon,
//        `arrow_x = intervals[0].end`, because placing the arrow as far right
//        as possible maximizes overlap with subsequent balloons while still
//        bursting the earliest-ending balloon.
//     3. Scan the remaining intervals in sorted order. Whenever an interval has
//        `start <= arrow_x`, it already contains `arrow_x` (since `arrow_x <=
//        interval.end` by sorting). Otherwise (`start > arrow_x`), shoot a new
//        arrow at `interval.end` and increment the arrow count.
//
//   Time Complexity:  O(n log n) due to sorting the n intervals.
//   Space Complexity: O(n) for the flat interval buffer (avoiding mutation of
//                     the caller's nested vectors and improving cache locality
//                     during sorting).
//
// Data Structures:
//   - std::vector<Interval> (flat contiguous struct array) sorted by right
//     endpoint via std::ranges::sort.
//
// Real-World Applications:
//   - Sensor and camera placement: choosing the fewest trigger timestamps or
//     guard positions to monitor all active time windows.
//   - Batch job coalescing and timer-wheel coalescing in OS kernels to wake
//     the CPU as infrequently as possible while meeting all deadline windows.
//   - Wireless channel and broadcast scheduling over overlapping client
//     availability intervals.
//
// Related LeetCode Problems:
//   - #435 Non-overlapping Intervals [done] (same sort-by-end greedy invariant for disjoint intervals)
//   - #253 Meeting Rooms II [done] (interval overlap counting via sweep line or min-heap)
//   - #56 Merge Intervals [done] (merging overlapping intervals after sorting)
//   - #757 Set Intersection Size At Least Two [new] (generalization requiring two arrows per interval)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <vector>

namespace {

struct BalloonInterval {
  int start = 0;
  int end = 0;
};

class Solution {
 public:
  // Returns the minimum number of vertical arrows needed to pierce all closed
  // intervals in `points`.
  // Time: O(n log n), Space: O(n).
  int findMinArrowShots(const std::vector<std::vector<int>>& points) {
    if (points.empty()) {
      return 0;
    }

    // Flatten into a compact value-type vector for cache-friendly sorting.
    std::vector<BalloonInterval> balloons;
    balloons.reserve(points.size());
    for (const auto& pt : points) {
      balloons.emplace_back(pt[0], pt[1]);
    }

    // Sort by right endpoint ascending so the first unburst balloon always
    // dictates the rightmost safe coordinate for the next arrow.
    std::ranges::sort(balloons, [](const BalloonInterval& lhs,
                                   const BalloonInterval& rhs) {
      return lhs.end < rhs.end;
    });

    // Initializing directly from the first balloon avoids sentinel issues when
    // coordinates equal INT_MIN.
    int arrows = 1;
    int arrow_x = balloons[0].end;

    for (std::size_t i = 1uz; i < balloons.size(); ++i) {
      // By sort order, arrow_x <= balloons[i].end already holds.
      // A new arrow is needed iff this balloon starts strictly after arrow_x.
      if (balloons[i].start > arrow_x) {
        arrow_x = balloons[i].end;
        ++arrows;
      }
    }

    return arrows;
  }
};

LC_PROBLEM(452, "Minimum Number Of Arrows To Burst Balloons", "greedy", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    const std::vector<std::vector<int>> points = {
        {10, 16}, {2, 8}, {1, 6}, {7, 12}};
    ctx.expect_eq(sol.findMinArrowShots(points), 2);
  });

  ctx.test("empty_input", [&] {
    Solution sol;
    const std::vector<std::vector<int>> points = {};
    ctx.expect_eq(sol.findMinArrowShots(points), 0);
  });

  ctx.test("python_build_large_coordinates", [&] {
    Solution sol;
    const std::vector<std::vector<int>> points = {
        {77171087, 133597895}, {45117276, 135064454}, {80695788, 90089372},
        {91705403, 110208054}, {52392754, 127005153}, {53999932, 118094992},
        {11549676, 55543044},  {43947739, 128157751}, {55636226, 105334812},
        {69348094, 125645633}};
    ctx.expect_eq(sol.findMinArrowShots(points), 3);
  });

  ctx.test("python_build_overlapping_clusters", [&] {
    Solution sol;
    const std::vector<std::vector<int>> points = {
        {0, 9},  {1, 8},  {7, 8}, {1, 6}, {9, 16},
        {7, 13}, {7, 10}, {6, 11}, {6, 9}, {9, 13}};
    ctx.expect_eq(sol.findMinArrowShots(points), 3);
  });

  ctx.test("python_build_single_common_point", [&] {
    Solution sol;
    const std::vector<std::vector<int>> points = {
        {4, 12}, {7, 8}, {7, 9}, {7, 9}, {2, 8}, {6, 7}, {5, 14}, {4, 13}};
    ctx.expect_eq(sol.findMinArrowShots(points), 1);
  });

  ctx.test("int32_extreme_bounds", [&] {
    Solution sol;
    constexpr int kMin = std::numeric_limits<int>::min();
    constexpr int kMax = std::numeric_limits<int>::max();
    const std::vector<std::vector<int>> disjoint = {
        {kMin + 2, kMin + 3}, {kMax - 1, kMax}};
    const std::vector<std::vector<int>> full_span = {{kMin, kMax}};
    ctx.expect_eq(sol.findMinArrowShots(disjoint), 2);
    ctx.expect_eq(sol.findMinArrowShots(full_span), 1);
  });
}

}  // namespace
