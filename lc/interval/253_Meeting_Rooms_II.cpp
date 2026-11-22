// https://leetcode.com/problems/meeting-rooms-ii/description/
//
// Given an array of meeting time intervals `intervals` where
// `intervals[i] = [start_i, end_i]` (`start_i < end_i`), return the minimum
// number of conference rooms required.

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
