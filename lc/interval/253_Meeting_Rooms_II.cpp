// LeetCode 253: Meeting Rooms II
// https://leetcode.com/problems/meeting-rooms-ii/
//
// Problem:
//   Given an array of meeting time intervals `intervals` where
//   `intervals[i] = [start_i, end_i]` (`start_i < end_i`), return the minimum
//   number of conference rooms required so that all meetings can be held
//   without conflict. A meeting ending at time `t` frees its room for a
//   meeting starting at time `t`.
//   Constraints: 1 <= intervals.size() <= 10^4 (empty input yields 0),
//   0 <= start_i < end_i <= 10^6.
//
// Algorithm:
//   Chronological two-pointer sweep over separately sorted start and end
//   times. Extract all start times into `starts` and all end times into
//   `ends`, then sort both arrays ascending. Iterate through each `start` in
//   `starts` while tracking the earliest unconsumed end time `ends[end_idx]`:
//   if `start < ends[end_idx]`, no active room has finished yet so increment
//   `rooms`; otherwise advance `end_idx` to reuse that freed room.
//   Key invariant: `rooms` never decreases and equals the high-water mark of
//   simultaneously active meetings encountered so far, which by Dilworth's
//   theorem on interval graphs equals the minimum room count.
//   Time: O(N log N) for sorting, Space: O(N) for `starts` and `ends`.
//
// Data Structures:
//   - `std::vector<int> starts`, `ends`: separately sorted timestamp arrays
//     traversed with an index pointer `end_idx`.
//
// Real-World Applications:
//   - Sizing conference rooms, airport gates, hospital operating rooms, or
//     vehicle fleets to satisfy peak concurrent demand.
//   - Computing peak thread-pool or worker-container concurrency needed for
//     scheduled batch workloads.
//   - Interval-graph coloring in compiler register allocation (linear scan
//     register count).
//
// Related LeetCode Problems:
//   - #252 Meeting Rooms (feasibility check: whether 1 room is sufficient)
//   - #1094 Car Pooling (weighted capacity version with passenger counts)
//   - #2402 Meeting Rooms III (numbered room assignment with delayed meetings)
//   - #732 My Calendar III (online peak overlap count via boundary events)
//   - #1854 Maximum Population Year (peak simultaneous count over intervals)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Chronological two-pointer sweep over separately sorted start and end times:
  // when the next meeting starts before the earliest active meeting ends,
  // allocate a new room; otherwise reuse the freed room.
  // Time: O(N log N), Space: O(N).
  int minMeetingRooms(const std::vector<std::vector<int>>& intervals) {
    const std::size_t n = intervals.size();
    std::vector<int> starts;
    std::vector<int> ends;
    starts.reserve(n);
    ends.reserve(n);

    for (const auto& interval : intervals) {
      starts.push_back(interval[0]);
      ends.push_back(interval[1]);
    }

    std::ranges::sort(starts);
    std::ranges::sort(ends);

    int rooms = 0;
    std::size_t end_idx = 0uz;
    for (const int start : starts) {
      if (start < ends[end_idx]) {
        ++rooms;
      } else {
        ++end_idx;
      }
    }
    return rooms;
  }
};

LC_PROBLEM(253, "Meeting Rooms II", "interval", ctx) {
  ctx.test("leetcode_example_1", [&] {
    Solution sol;
    ctx.expect_eq(sol.minMeetingRooms({{0, 30}, {5, 10}, {15, 20}}), 2);
  });

  ctx.test("leetcode_example_2", [&] {
    Solution sol;
    ctx.expect_eq(sol.minMeetingRooms({{7, 10}, {2, 4}}), 1);
  });

  ctx.test("back_to_back_reuses_room", [&] {
    Solution sol;
    ctx.expect_eq(sol.minMeetingRooms({{0, 10}, {10, 20}, {20, 30}}), 1);
  });

  ctx.test("all_overlapping", [&] {
    Solution sol;
    ctx.expect_eq(sol.minMeetingRooms({{1, 5}, {2, 6}, {3, 7}, {4, 8}}), 4);
  });

  ctx.test("empty_and_single", [&] {
    Solution sol;
    ctx.expect_eq(sol.minMeetingRooms({}), 0);
    ctx.expect_eq(sol.minMeetingRooms({{1, 10}}), 1);
  });
}

}  // namespace
