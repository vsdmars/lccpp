// https://leetcode.com/problems/meeting-rooms/description/
//
// Given an array of meeting time intervals `intervals` where
// `intervals[i] = [start_i, end_i]` (`start_i < end_i`), determine if a person
// could attend all meetings without any overlap.

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Sort by start time and verify every meeting starts at or after the
  // previous meeting ends.
  // Time: O(N log N), Space: O(N).
  bool canAttendMeetings(const std::vector<std::vector<int>>& intervals) {
    std::vector<std::pair<int, int>> meetings;
    meetings.reserve(intervals.size());
    for (const auto& interval : intervals) {
      meetings.emplace_back(interval[0], interval[1]);
    }
    std::ranges::sort(meetings);

    for (std::size_t i = 1uz; i < meetings.size(); ++i) {
      if (meetings[i].first < meetings[i - 1uz].second) {
        return false;
      }
    }
    return true;
  }
};

LC_PROBLEM(252, "Meeting Rooms", "interval", ctx) {
  ctx.test("leetcode_example_overlap", [&] {
    Solution sol;
    ctx.expect_false(sol.canAttendMeetings({{0, 30}, {5, 10}, {15, 20}}));
  });

  ctx.test("leetcode_example_no_overlap", [&] {
    Solution sol;
    ctx.expect_true(sol.canAttendMeetings({{7, 10}, {2, 4}}));
  });

  ctx.test("python_build_overlap", [&] {
    Solution sol;
    ctx.expect_false(
        sol.canAttendMeetings({{0, 15}, {16, 20}, {13, 20}, {21, 22}}));
  });

  ctx.test("back_to_back_allowed", [&] {
    Solution sol;
    ctx.expect_true(sol.canAttendMeetings({{10, 20}, {0, 10}, {20, 25}}));
  });

  ctx.test("empty_and_single", [&] {
    Solution sol;
    ctx.expect_true(sol.canAttendMeetings({}));
    ctx.expect_true(sol.canAttendMeetings({{1, 5}}));
  });
}

}  // namespace
