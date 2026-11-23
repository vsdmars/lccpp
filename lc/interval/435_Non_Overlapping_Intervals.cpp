// LeetCode 435: Non-overlapping Intervals
// https://leetcode.com/problems/non-overlapping-intervals/
//
// Problem:
//   Given an array of intervals where `intervals[i] = [start_i, end_i]`,
//   return the minimum number of intervals to remove so that the remaining
//   intervals are mutually non-overlapping. Intervals that only touch at a
//   shared endpoint (e.g., `[1, 2]` and `[2, 3]`) do not overlap.
//   Constraints: 1 <= intervals.size() <= 10^5, -5*10^4 <= start_i < end_i
//   <= 5*10^4 (empty input returns 0).
//
// Algorithm:
//   Greedy earliest-finish interval scheduling. Minimizing removals is
//   equivalent to maximizing the number of mutually compatible intervals kept.
//   Sort all intervals by `end` coordinate ascending, select the first
//   interval (`last_end = spans[0].end`, `kept = 1`), and scan the rest:
//   whenever `spans[i].start >= last_end`, keep it and update `last_end`.
//   Return `spans.size() - kept`.
//   Key invariant (exchange argument): choosing the compatible interval with
//   the smallest end time leaves the maximum possible remaining timeline for
//   subsequent intervals, guaranteeing an optimal solution.
//   Time: O(N log N) for sorting, Space: O(N) for the copied interval array.
//
// Data Structures:
//   - `struct Interval`: lightweight `(start, end)` record for cache-friendly
//     sorting by `end`.
//   - `std::vector<Interval> spans`: contiguous array sorted via
//     `std::ranges::sort` using the `&Interval::end` projection.
//
// Real-World Applications:
//   - Activity selection and CPU/machine job scheduling to maximize the number
//     of completed non-overlapping tasks on a single resource.
//   - Conflict resolution in calendar apps and broadcast/ad slot allocation
//     when maximizing accepted bookings.
//   - Genomic sequence alignment chaining to pick the largest set of
//     non-overlapping candidate exons or motifs.
//
// Related LeetCode Problems:
//   - #452 Minimum Number of Arrows to Burst Balloons (dual earliest-end greedy)
//   - #646 Maximum Length of Pair Chain (maximum non-overlapping chain length)
//   - #56 Merge Intervals (sort-and-scan coalescing of overlapping intervals)
//   - #252 Meeting Rooms (detecting whether zero removals are required)
//   - #1235 Maximum Profit in Job Scheduling (weighted DP generalization)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

struct Interval {
  int start = 0;
  int end = 0;
};

class Solution {
 public:
  // Greedy interval scheduling: sort by end coordinate ascending and always
  // keep the earliest-finishing interval that does not overlap the previously
  // kept one. The minimum removals equals total intervals minus max kept.
  //
  // Time:  O(n log n)
  // Space: O(n)
  int eraseOverlapIntervals(const std::vector<std::vector<int>>& intervals) {
    if (intervals.empty()) {
      return 0;
    }

    std::vector<Interval> spans;
    spans.reserve(intervals.size());
    for (const auto& iv : intervals) {
      spans.push_back(Interval{.start = iv[0], .end = iv[1]});
    }

    std::ranges::sort(spans, {}, &Interval::end);

    int kept = 1;
    int last_end = spans[0].end;
    for (std::size_t i = 1uz; i < spans.size(); ++i) {
      if (spans[i].start >= last_end) {
        last_end = spans[i].end;
        ++kept;
      }
    }

    return static_cast<int>(spans.size()) - kept;
  }
};

LC_PROBLEM(435, "Non Overlapping Intervals", "interval", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    ctx.expect_eq(sol.eraseOverlapIntervals({{1, 2}, {2, 3}, {3, 4}, {1, 3}}),
                  1);
  });

  ctx.test("example_2_all_identical", [&] {
    ctx.expect_eq(sol.eraseOverlapIntervals({{1, 2}, {1, 2}, {1, 2}}), 2);
  });

  ctx.test("example_3_already_non_overlapping", [&] {
    ctx.expect_eq(sol.eraseOverlapIntervals({{1, 2}, {2, 3}}), 0);
  });

  ctx.test("wide_spanning_interval", [&] {
    ctx.expect_eq(
        sol.eraseOverlapIntervals({{1, 2}, {2, 3}, {3, 4}, {1, 100}}), 1);
  });

  ctx.test("empty_and_single", [&] {
    ctx.expect_eq(sol.eraseOverlapIntervals({}), 0);
    ctx.expect_eq(sol.eraseOverlapIntervals({{1, 5}}), 0);
  });

  ctx.test("negative_coordinates", [&] {
    ctx.expect_eq(
        sol.eraseOverlapIntervals({{-5, -2}, {-3, 0}, {0, 2}, {-2, 1}}), 2);
  });
}

}  // namespace
