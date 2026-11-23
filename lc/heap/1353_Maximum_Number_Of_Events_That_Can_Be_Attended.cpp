// LeetCode 1353: Maximum Number Of Events That Can Be Attended
// https://leetcode.com/problems/maximum-number-of-events-that-can-be-attended/
//
// Problem:
//   Given an array `events` where `events[i] = [startDay_i, endDay_i]`
//   (1 <= N <= 10^5, 1 <= startDay_i <= endDay_i <= 10^5), you may attend
//   event `i` on any single day `d` with `startDay_i <= d <= endDay_i`. At
//   most one event can be attended per day. Return the maximum number of
//   events you can attend.
//
// Algorithm:
//   Greedy Earliest-Deadline-First (EDF) with Min-Heap:
//   1. Copy events into a compact struct array and sort ascending by start day.
//   2. Sweep `current_day` forward while either unprocessed events remain or
//      the min-heap of active end days is non-empty:
//      - If the min-heap is empty, jump `current_day` directly to the next
//        event's start day (skipping idle gaps in O(1)).
//      - Push the `end_day` of every event whose `start_day <= current_day`
//        into the min-heap.
//      - Pop the event with the smallest `end_day` (earliest deadline) to
//        attend on `current_day`, increment the attended counter, and advance
//        `current_day` by 1.
//      - Purge any remaining heap entries whose `end_day < current_day`
//        because they have expired and can no longer be attended.
//   Exchange-Argument Invariant: Among all events available on `current_day`,
//   attending the one that expires soonest never reduces the set of events
//   attendable on future days compared to any alternative choice.
//   Time Complexity:  O(N log N) — O(N log N) for sorting, and each event is
//                     pushed into and popped from the min-heap at most once.
//   Space Complexity: O(N) for the sorted event copy and min-heap.
//
// Data Structures:
//   - Sorted `std::vector<Event>` (compact 8-byte structs): streams newly
//     available events in chronological order of start day.
//   - Min-heap (`std::priority_queue<int, std::vector<int>, std::greater<>>`):
//     tracks the end days of currently active events so the soonest-expiring
//     event can be selected (and expired events pruned) in O(log N) time.
//
// Real-World Applications:
//   - Real-time operating system (RTOS) task scheduling using Earliest
//     Deadline First (EDF) to maximize on-time job completion.
//   - Conference, webinar, or interview slot allocation where each session has
//     a flexible availability window.
//   - Cloud batch-job admission control with release times and hard SLAs.
//
// Related LeetCode Problems:
//   - #1751 Maximum Number of Events That Can Be Attended II [new] (weighted events with k limit via DP)
//   - #253 Meeting Rooms II [done] (interval sweep with min-heap of end times)
//   - #630 Course Schedule III [new] (greedy deadline scheduling with max-heap)
//   - #1834 Single Threaded Cpu [done] (event-driven release-time + min-heap scheduling)
//   - #435 Non-overlapping Intervals [done] (greedy earliest-end interval selection)

#include "framework/lc_framework.h"

#include <algorithm>
#include <compare>
#include <cstddef>
#include <functional>
#include <queue>
#include <vector>

namespace {

// Compact representation of one event interval [start, end].
struct Event {
  int start = 0;
  int end = 0;

  auto operator<=>(const Event&) const = default;
};

class Solution {
 public:
  // Returns the maximum number of events that can be attended (at most one per
  // day) using Earliest-Deadline-First greedy selection.
  //
  // Time Complexity:  O(N log N)
  // Space Complexity: O(N)
  [[nodiscard]] int maxEvents(const std::vector<std::vector<int>>& events) {
    if (events.empty()) {
      return 0;
    }

    // Copy into contiguous Event structs and sort by start day ascending.
    std::vector<Event> sorted_events;
    sorted_events.reserve(events.size());
    for (const auto& ev : events) {
      sorted_events.push_back(Event{.start = ev[0], .end = ev[1]});
    }
    std::ranges::sort(sorted_events);

    // Min-heap of end days for all events that have started and not yet ended.
    std::priority_queue<int, std::vector<int>, std::greater<>> active_ends;
    std::size_t next_idx = 0uz;
    int current_day = 0;
    int attended = 0;

    while (next_idx < sorted_events.size() || !active_ends.empty()) {
      // Fast-forward over idle days when no event is currently active.
      if (active_ends.empty()) {
        current_day = sorted_events[next_idx].start;
      }

      // Enqueue all events that become available on or before `current_day`.
      while (next_idx < sorted_events.size() &&
             sorted_events[next_idx].start <= current_day) {
        active_ends.push(sorted_events[next_idx].end);
        ++next_idx;
      }

      // Attend the active event that expires earliest (greedy EDF choice).
      active_ends.pop();
      ++attended;
      ++current_day;

      // Discard any remaining events whose deadline has already passed.
      while (!active_ends.empty() && active_ends.top() < current_day) {
        active_ends.pop();
      }
    }

    return attended;
  }
};

LC_PROBLEM(1353, "Maximum Number Of Events That Can Be Attended", "heap", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    const std::vector<std::vector<int>> events = {{1, 2}, {2, 3}, {3, 4}};
    ctx.expect_eq(sol.maxEvents(events), 3);
  });

  ctx.test("example_2", [&] {
    Solution sol;
    const std::vector<std::vector<int>> events = {
        {1, 2}, {2, 3}, {3, 4}, {1, 2}};
    ctx.expect_eq(sol.maxEvents(events), 4);
  });

  ctx.test("mixed_short_and_long_windows", [&] {
    Solution sol;
    const std::vector<std::vector<int>> events = {
        {1, 4}, {4, 4}, {2, 2}, {3, 4}, {1, 1}};
    ctx.expect_eq(sol.maxEvents(events), 4);
  });

  ctx.test("same_day_conflict_drops_expired", [&] {
    Solution sol;
    const std::vector<std::vector<int>> events = {{1, 1}, {1, 1}, {1, 1}};
    ctx.expect_eq(sol.maxEvents(events), 1);
  });

  ctx.test("large_idle_gap_fast_forward", [&] {
    Solution sol;
    const std::vector<std::vector<int>> events = {{1, 1}, {100000, 100000}};
    ctx.expect_eq(sol.maxEvents(events), 2);
  });

  ctx.test("nested_expanding_deadlines", [&] {
    Solution sol;
    const std::vector<std::vector<int>> events = {
        {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}, {1, 7}};
    ctx.expect_eq(sol.maxEvents(events), 7);
  });

  ctx.test("empty_events", [&] {
    Solution sol;
    ctx.expect_eq(sol.maxEvents({}), 0);
  });
}

}  // namespace
