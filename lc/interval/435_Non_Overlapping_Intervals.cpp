// https://leetcode.com/problems/non-overlapping-intervals/
//
// Given a collection of intervals where intervals[i] = [start_i, end_i], return
// the minimum number of intervals you need to remove to make the rest of the
// intervals non-overlapping. Intervals that only touch at an endpoint (e.g.,
// [1, 2] and [2, 3]) are considered non-overlapping.

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
