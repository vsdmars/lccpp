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
// Examples:
//   Example 1:
//     Input:  intervals = [[0, 30], [5, 10], [15, 20]]
//     Output: false
//     Explanation:
//       - Copy intervals into meetings and sort by start time: [(0, 30), (5, 10),
//         (15, 20)].
//       - At i = 1, compare meetings[1].first (5) with meetings[0].second (30): 5 <
//         30 is true.
//       - Return false immediately upon finding the overlap between (0, 30) and (5,
//         10).
//
//   Example 2:
//     Input:  intervals = [[10, 20], [0, 10], [20, 25]]
//     Output: true
//     Explanation:
//       - Copy intervals into meetings and sort by start time: [(0, 10), (10, 20),
//         (20, 25)].
//       - At i = 1, meetings[1].first (10) < meetings[0].second (10) is false, so
//         back-to-back meetings (0, 10) and (10, 20) do not overlap.
//       - At i = 2, meetings[2].first (20) < meetings[1].second (20) is false, so
//         the loop completes and returns true.
//
//   Example 3:
//     Input:  intervals = [[0, 15], [16, 20], [13, 20], [21, 22]]
//     Output: false
//     Explanation:
//       - Copy intervals into meetings and sort by start time: [(0, 15), (13, 20),
//         (16, 20), (21, 22)].
//       - At i = 1, check meetings[1].first (13) < meetings[0].second (15): the
//         condition holds (13 < 15).
//       - Return false because (13, 20) starts before (0, 15) ends.
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
//   - #253 Meeting Rooms II [done] (minimum rooms needed when overlaps are allowed)
//   - #56 Merge Intervals [done] (sort by start time and coalesce overlapping ranges)
//   - #435 Non-overlapping Intervals [done] (minimum removals to eliminate overlaps)
//   - #729 My Calendar I [done] (online overlap detection as bookings arrive)

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
