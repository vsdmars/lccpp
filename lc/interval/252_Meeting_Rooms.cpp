// LeetCode 252: Meeting Rooms
// https://leetcode.com/problems/meeting-rooms/
//
// Problem:
//   Given an array of meeting time intervals `intervals` where
//   `intervals[i] = [start_i, end_i]` (`start_i < end_i`), return true if a
//   single person can attend all meetings without any time conflict, and false
//   otherwise. Back-to-back meetings that share only an endpoint (e.g.,
//   `[0, 10]` and `[10, 20]`) do not overlap.
//   Constraints: 0 <= intervals.size() <= 10^4, 0 <= start_i < end_i <= 10^6.
//
// Algorithm:
//   Copy the intervals into pairs `(start, end)` and sort them in ascending
//   order by start time (breaking ties by end time). Scan adjacent pairs from
//   left to right: if any meeting starts strictly before the previous meeting
//   ends (`meetings[i].first < meetings[i - 1].second`), an overlap exists and
//   we return false; otherwise all meetings are pairwise disjoint.
//   Key invariant: once sorted by start time, any overlap among intervals must
//   appear between some adjacent pair `(i - 1, i)`.
//   Time: O(N log N) for sorting, Space: O(N) for the sorted copy.
//
// Data Structures:
//   - `std::vector<std::pair<int, int>> meetings`: contiguous array of
//     `(start, end)` pairs sorted in place with `std::ranges::sort`.
//
// Real-World Applications:
//   - Calendar conflict detection for personal schedulers, doctor clinics, and
//     single-operator booking systems.
//   - Single-machine or single-core job feasibility validation in real-time
//     task schedulers.
//   - Time-division channel allocation checks verifying non-interfering
//     transmission windows.
//
// Related LeetCode Problems:
//   - #253 Meeting Rooms II (minimum rooms needed when overlaps are allowed)
//   - #56 Merge Intervals (sort by start time and coalesce overlapping ranges)
//   - #435 Non-overlapping Intervals (minimum removals to eliminate overlaps)
//   - #729 My Calendar I (online overlap detection as bookings arrive)

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
