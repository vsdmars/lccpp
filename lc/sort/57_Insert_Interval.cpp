// LeetCode 57: Insert Interval
// https://leetcode.com/problems/insert-interval/
//
// Problem:
//   Given an array of non-overlapping intervals sorted in ascending order by
//   start time, where intervals[i] = [start_i, end_i], and a new interval
//   new_interval = [start, end], insert new_interval into intervals such that
//   the resulting list remains sorted by start time and contains no
//   overlapping intervals (merging overlapping intervals as necessary).
//
// Algorithm:
//   Single-pass three-phase linear sweep taking advantage of the input's
//   existing sorted, non-overlapping invariant:
//   1. Left phase: Copy every interval that ends strictly before
//      new_interval starts (interval[1] < merged[0]).
//   2. Merge phase: For every subsequent interval that overlaps merged
//      (interval[0] <= merged[1]), absorb it by expanding
//      merged[0] = min(merged[0], interval[0]) and
//      merged[1] = max(merged[1], interval[1]). Then append merged.
//   3. Right phase: Copy all remaining intervals, which start strictly after
//      merged ends.
//   Time Complexity: O(n), where n = intervals.size().
//   Space Complexity: O(n) for the output vector (O(1) auxiliary space).
//
// Data Structures:
//   std::vector<std::vector<int>> for the sorted interval sequence and output,
//   plus a 2-element working interval tracking the running merged span.
//
// Real-World Applications:
//   - Calendar / meeting-room booking engines inserting a new reservation and
//     coalescing busy time blocks.
//   - OS virtual-memory managers (VMAs) and filesystem extent trees merging
//     adjacent or overlapping allocated address ranges.
//   - Time-series database compaction and genomic BED-file interval merging.
//
// Related LeetCode Problems:
//   - #56 Merge Intervals [done] (general unsorted interval merging)
//   - #252 Meeting Rooms [done] (checking whether any intervals overlap)
//   - #253 Meeting Rooms II [done] (maximum concurrent overlapping intervals)
//   - #435 Non-overlapping Intervals [done] (minimum removals to eliminate overlaps)
//   - #715 Range Module [done] (dynamic online interval add/query/remove)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Inserts new_interval into a sorted list of non-overlapping intervals,
  // merging any overlapping spans into a single interval.
  // Time: O(n), Space: O(1) auxiliary (O(n) output).
  std::vector<std::vector<int>> insert(
      const std::vector<std::vector<int>>& intervals,
      const std::vector<int>& new_interval) {
    std::vector<std::vector<int>> result;
    result.reserve(intervals.size() + 1uz);

    const std::size_t n = intervals.size();
    std::size_t i = 0uz;
    int merged_start = new_interval[0];
    int merged_end = new_interval[1];

    // Phase 1: Append all intervals ending strictly before new_interval starts.
    while (i < n && intervals[i][1] < merged_start) {
      result.push_back(intervals[i]);
      ++i;
    }

    // Phase 2: Merge all intervals overlapping [merged_start, merged_end].
    // Because intervals are sorted and phase 1 exhausted all intervals with
    // end < merged_start, an interval overlaps iff its start <= merged_end.
    while (i < n && intervals[i][0] <= merged_end) {
      merged_start = std::min(merged_start, intervals[i][0]);
      merged_end = std::max(merged_end, intervals[i][1]);
      ++i;
    }
    result.push_back({merged_start, merged_end});

    // Phase 3: Append all remaining intervals starting strictly after merged.
    while (i < n) {
      result.push_back(intervals[i]);
      ++i;
    }

    return result;
  }
};

LC_PROBLEM(57, "Insert Interval", "sort", ctx) {
  Solution sol;
  using Intervals = std::vector<std::vector<int>>;

  ctx.test("example_1_merge_single", [&] {
    const Intervals expected = {{1, 5}, {6, 9}};
    ctx.expect_eq(sol.insert({{1, 3}, {6, 9}}, {2, 5}), expected);
  });

  ctx.test("example_2_merge_multiple", [&] {
    const Intervals input = {{1, 2}, {3, 5}, {6, 7}, {8, 10}, {12, 16}};
    const Intervals expected = {{1, 2}, {3, 10}, {12, 16}};
    ctx.expect_eq(sol.insert(input, {4, 9}), expected);
  });

  ctx.test("edge_cases_empty_and_non_overlapping", [&] {
    // Empty existing intervals.
    ctx.expect_eq(sol.insert({}, {5, 7}), Intervals{{5, 7}});

    // Strictly before all existing intervals.
    ctx.expect_eq(sol.insert({{3, 5}, {8, 10}}, {1, 2}),
                  Intervals{{1, 2}, {3, 5}, {8, 10}});

    // Strictly after all existing intervals.
    ctx.expect_eq(sol.insert({{1, 2}, {3, 5}}, {6, 8}),
                  Intervals{{1, 2}, {3, 5}, {6, 8}});
  });

  ctx.test("edge_cases_touching_and_encompassing", [&] {
    // Touching endpoints merge because intervals are closed [start, end].
    ctx.expect_eq(sol.insert({{1, 2}, {5, 6}}, {2, 5}), Intervals{{1, 6}});

    // New interval subsumes every existing interval.
    ctx.expect_eq(sol.insert({{2, 3}, {4, 5}, {6, 7}}, {1, 10}),
                  Intervals{{1, 10}});

    // New interval already contained inside an existing interval.
    ctx.expect_eq(sol.insert({{1, 10}}, {3, 5}), Intervals{{1, 10}});
  });
}

}  // namespace
