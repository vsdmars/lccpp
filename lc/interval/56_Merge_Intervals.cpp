// LeetCode 56: Merge Intervals
// https://leetcode.com/problems/merge-intervals/
//
// Problem:
//   Given an array of intervals where `intervals[i] = [start_i, end_i]`, merge
//   all overlapping (or endpoint-touching) intervals and return an array of
//   mutually disjoint intervals that cover the exact same union as the input.
//   Constraints: 1 <= intervals.size() <= 10^4 (empty input returns empty),
//   0 <= start_i <= end_i <= 10^4 (implementation also supports negative
//   coordinates).
//
// Examples:
//   Example 1:
//     Input:  intervals = [[1, 3], [2, 6], [8, 10], [15, 18]]
//     Output: [[1, 6], [8, 10], [15, 18]]
//     Explanation:
//       - Sort intervals lexicographically (already ordered) and push [1, 3] onto
//         merged.
//       - For [2, 6], 2 <= merged.back()[1] (3), so extend merged.back()[1] to
//         max(3, 6) = 6, giving [[1, 6]].
//       - For [8, 10] (8 > 6) and [15, 18] (15 > 10), neither overlaps the back of
//         merged, so append both to return [[1, 6], [8, 10], [15, 18]].
//
//   Example 2:
//     Input:  intervals = [[4, 8], [1, 10], [2, 3]]
//     Output: [[1, 10]]
//     Explanation:
//       - Sort intervals lexicographically to [[1, 10], [2, 3], [4, 8]] and push
//         [1, 10] onto merged.
//       - For [2, 3], 2 <= 10, so merged.back()[1] becomes max(10, 3) = 10.
//       - For [4, 8], 4 <= 10, so merged.back()[1] stays max(10, 8) = 10, returning
//         [[1, 10]].
//
//   Example 3:
//     Input:  intervals = [[5, 6], [-1, 2], [-3, 0]]
//     Output: [[-3, 2], [5, 6]]
//     Explanation:
//       - Sort intervals lexicographically to [[-3, 0], [-1, 2], [5, 6]] and push
//         [-3, 0] onto merged.
//       - For [-1, 2], -1 <= 0, so extend merged.back()[1] to max(0, 2) = 2,
//         leaving merged = [[-3, 2]].
//       - For [5, 6], 5 > 2, so append [5, 6] to produce [[-3, 2], [5, 6]].
//
// Algorithm:
//   Sort-and-sweep greedy merge. Sort `intervals` lexicographically (by start
//   ascending, then end ascending). Iterate over the sorted intervals while
//   maintaining the merged output list: if `merged` is non-empty and the
//   current interval starts at or before `merged.back()[1]`, extend
//   `merged.back()[1]` to `max(merged.back()[1], interval[1])`; otherwise
//   append the new disjoint interval to `merged`.
//   Key invariant: every interval already in `merged` is strictly disjoint
//   from all earlier ones, and no future interval can start before
//   `merged.back()[0]`, so only the last merged interval can ever overlap.
//   Time: O(N log N) for sorting, Space: O(N) for the output vector (plus
//   O(log N) sort stack space).
//
// Data Structures:
//   - `std::vector<std::vector<int>> intervals`: input list sorted in place
//     via `std::ranges::sort`.
//   - `std::vector<std::vector<int>> merged`: output stack whose back element
//     tracks the active merged range.
//
// Real-World Applications:
//   - Coalescing adjacent or overlapping free memory blocks in heap allocators
//     and disk/extent managers.
//   - Merging overlapping time ranges in calendar availability views, uptime
//     SLA calculators, and log/trace timeline visualizations.
//   - Consolidating byte-range requests in HTTP caching proxies and storage
//     I/O schedulers (read/write coalescing).
//
// Related LeetCode Problems:
//   - #57 Insert Interval [done] (insert and merge into an already-sorted list)
//   - #986 Interval List Intersections [done] (intersect two disjoint interval lists)
//   - #759 Employee Free Time [done] (find complement gaps of merged busy schedules)
//   - #252 Meeting Rooms [done] (sort by start time to test for any overlap)
//   - #435 Non-overlapping Intervals [done] (minimum removals to avoid all overlaps)

#include "framework/lc_framework.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Sort intervals by start (then end), then greedily merge overlapping
  // intervals onto a result stack.
  // Time: O(N log N), Space: O(N) for the output.
  std::vector<std::vector<int>> merge(
      std::vector<std::vector<int>> intervals) {
    std::ranges::sort(intervals);

    std::vector<std::vector<int>> merged;
    merged.reserve(intervals.size());

    for (auto& interval : intervals) {
      if (!merged.empty() && interval[0] <= merged.back()[1]) {
        merged.back()[1] = std::max(merged.back()[1], interval[1]);
        continue;
      }
      merged.push_back(std::move(interval));
    }

    return merged;
  }
};

LC_PROBLEM(56, "Merge Intervals", "interval", ctx) {
  ctx.test("leetcode_example_1", [&] {
    Solution sol;
    const std::vector<std::vector<int>> expected = {
        {1, 6}, {8, 10}, {15, 18}};
    ctx.expect_eq(sol.merge({{1, 3}, {2, 6}, {8, 10}, {15, 18}}), expected);
  });

  ctx.test("touching_boundaries", [&] {
    Solution sol;
    const std::vector<std::vector<int>> expected = {{1, 5}};
    ctx.expect_eq(sol.merge({{1, 4}, {4, 5}}), expected);
  });

  ctx.test("unsorted_and_nested", [&] {
    Solution sol;
    const std::vector<std::vector<int>> expected = {{1, 10}};
    ctx.expect_eq(sol.merge({{4, 8}, {1, 10}, {2, 3}}), expected);
  });

  ctx.test("empty_and_single", [&] {
    Solution sol;
    ctx.expect_eq(sol.merge({}), std::vector<std::vector<int>>{});
    ctx.expect_eq(sol.merge({{3, 7}}), std::vector<std::vector<int>>{{3, 7}});
  });

  ctx.test("negative_coordinates", [&] {
    Solution sol;
    const std::vector<std::vector<int>> expected = {{-3, 2}, {5, 6}};
    ctx.expect_eq(sol.merge({{5, 6}, {-1, 2}, {-3, 0}}), expected);
  });
}

}  // namespace
