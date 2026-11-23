// LeetCode 731: My Calendar II
// https://leetcode.com/problems/my-calendar-ii/
//
// Problem:
//   Implement a `MyCalendarTwo` class that stores half-open time intervals
//   `[start, end)` and rejects any booking that would cause a triple booking
//   (three events sharing at least one common point in time). Each call to
//   `book(start, end)` returns `true` and records the event if adding it
//   creates at most double bookings; otherwise it returns `false` and leaves
//   the calendar unchanged.
//
// Examples:
//   Example 1:
//     Input:  MyCalendarTwo(), book(10, 20), book(50, 60), book(10, 40), book(5,
//             15), book(5, 10), book(25, 55)
//     Output: [null, true, true, true, false, true, true]
//     Explanation:
//       - book(10, 20) and book(50, 60) have no existing overlaps, so calendar_
//         stores [[10, 20), [50, 60)] and both return true.
//       - book(10, 40) intersects [10, 20) in calendar_, adding [10, 20) to
//         overlaps_ and [10, 40) to calendar_, returning true.
//       - book(5, 15) intersects [10, 20) in overlaps_ (5 < 20 && 15 > 10), so
//         Phase 1 immediately returns false without modifying calendar_ or
//         overlaps_.
//       - book(5, 10) touches 10 without intersecting [10, 20) or [10, 40) under
//         half-open bounds, so it appends to calendar_ and returns true.
//       - book(25, 55) avoids [10, 20) in overlaps_, records intersections [25, 40)
//         and [50, 55) in overlaps_, appends [25, 55) to calendar_, and returns
//         true.
//
//   Example 2:
//     Input:  MyCalendarTwo(), book(0, 5), book(5, 10), book(5, 10), book(5, 10),
//             book(0, 5), book(4, 6)
//     Output: [null, true, true, true, false, true, false]
//     Explanation:
//       - book(0, 5) and book(5, 10) do not overlap because intersects checks start
//         < other.end && end > other.start, so both commit to calendar_.
//       - The second book(5, 10) finds no hit in overlaps_, records [5, 10) in
//         overlaps_, and returns true; the third book(5, 10) hits [5, 10) in
//         overlaps_ and returns false.
//       - book(0, 5) avoids [5, 10) in overlaps_, intersects [0, 5) in calendar_,
//         and adds [0, 5) to overlaps_, returning true.
//       - book(4, 6) overlaps both [0, 5) and [5, 10) in overlaps_, so Phase 1
//         rejects it with false.
//
//   Example 3:
//     Input:  MyCalendarTwo(), book(1, 10), book(1, 10), book(5, 20), book(10, 20),
//             book(10, 20), book(15, 16)
//     Output: [null, true, true, false, true, true, false]
//     Explanation:
//       - The first two book(1, 10) calls succeed, recording [1, 10) in overlaps_
//         and two copies of [1, 10) in calendar_.
//       - book(5, 20) hits [1, 10) in overlaps_ during Phase 1 and returns false
//         before Phases 2 and 3, leaving [10, 20) unbooked.
//       - Two book(10, 20) calls then succeed because [10, 20) is disjoint from [1,
//         10), creating [10, 20) in overlaps_ on the second call.
//       - book(15, 16) intersects [10, 20) in overlaps_ and returns false.
//
// Algorithm:
//   Maintain two interval lists: `calendar_` stores all accepted bookings, and
//   `overlaps_` stores all double-booked intersections produced by pairs of
//   accepted bookings. When `book(start, end)` is called:
//     1. Scan `overlaps_`. Two half-open intervals `[a, b)` and `[c, d)`
//        intersect iff `a < d && b > c`. If the new interval intersects any
//        already double-booked interval, adding it would form a triple booking,
//        so return `false` immediately without modifying state.
//     2. Scan `calendar_`. For every existing event intersecting `[start, end)`,
//        record the intersection `[max(start, ev.start), min(end, ev.end))` in
//        `overlaps_`.
//     3. Append `[start, end)` to `calendar_` and return `true`.
//   Because no point may ever be covered three times, all intervals stored in
//   `overlaps_` are pairwise disjoint and their endpoints are drawn from the
//   2N endpoints of `calendar_`, bounding `overlaps_.size()` by O(N).
//   Time Complexity:  O(N) per `book()` call, O(N^2) across N bookings.
//   Space Complexity: O(N) to store accepted events and double-booked segments.
//
// Data Structures:
//   - `std::vector<Interval> calendar_`: append-only list of all accepted
//     bookings.
//   - `std::vector<Interval> overlaps_`: list of pairwise-disjoint intervals
//     that are currently double-booked.
//
// Real-World Applications:
//   - Resource and meeting-room reservation systems with fixed concurrency
//     quotas (e.g., two available conference rooms, GPU slots, or test rigs).
//   - Hotel/equipment rental overbooking guards enforcing a hard overlap cap.
//   - Admission control in time-slotted network or cloud compute schedulers.
//
// Related LeetCode Problems:
//   - #729 My Calendar I [done] (forbid double bookings; single overlap check)
//   - #732 My Calendar III [new] (track maximum k-booking via sweep-line / map)
//   - #253 Meeting Rooms II [done] (compute peak concurrent overlapping intervals)
//   - #56 Merge Intervals [done] (interval intersection and union primitives)

#include "framework/lc_framework.h"

#include <algorithm>
#include <vector>

namespace {

// Calendar that permits single and double bookings on half-open intervals
// [start, end), while rejecting any booking that would cause a triple booking.
class MyCalendarTwo {
 public:
  // Attempts to add [start, end) to the calendar.
  // Returns true and commits the interval if no triple booking occurs;
  // otherwise returns false and leaves the calendar unmodified.
  // Time: O(N), Space: O(1) amortized (O(N) total across N bookings).
  bool book(int start, int end) {
    // Phase 1: Reject if [start, end) overlaps any existing double-booked
    // segment, which would immediately create a triple booking.
    for (const Interval& ov : overlaps_) {
      if (intersects(start, end, ov)) {
        return false;
      }
    }

    // Phase 2: Safe to commit. Record all new double-booked intersections
    // created between [start, end) and previously accepted events.
    for (const Interval& ev : calendar_) {
      if (intersects(start, end, ev)) {
        overlaps_.push_back(Interval{
            .start = std::max(start, ev.start),
            .end = std::min(end, ev.end),
        });
      }
    }

    // Phase 3: Commit the newly accepted event.
    calendar_.push_back(Interval{.start = start, .end = end});
    return true;
  }

 private:
  // Half-open time interval [start, end).
  struct Interval {
    int start = 0;
    int end = 0;
  };

  // Returns true iff half-open intervals [start, end) and [other.start,
  // other.end) share a non-empty intersection.
  static bool intersects(int start, int end, const Interval& other) {
    return start < other.end && end > other.start;
  }

  std::vector<Interval> calendar_;
  std::vector<Interval> overlaps_;
};

LC_PROBLEM(731, "My Calendar II", "design", ctx) {
  ctx.test("example_1", [&] {
    MyCalendarTwo cal;
    ctx.expect_true(cal.book(10, 20));
    ctx.expect_true(cal.book(50, 60));
    ctx.expect_true(cal.book(10, 40));
    ctx.expect_false(cal.book(5, 15));
    ctx.expect_true(cal.book(5, 10));
    ctx.expect_true(cal.book(25, 55));
  });

  ctx.test("adjacent_half_open_boundaries", [&] {
    MyCalendarTwo cal;
    // Touching endpoints [0, 5) and [5, 10) do not overlap.
    ctx.expect_true(cal.book(0, 5));
    ctx.expect_true(cal.book(5, 10));
    ctx.expect_true(cal.book(5, 10));   // Double booking on [5, 10).
    ctx.expect_false(cal.book(5, 10));  // Triple booking on [5, 10).
    ctx.expect_true(cal.book(0, 5));    // Double booking on [0, 5).
    ctx.expect_false(cal.book(4, 6));   // Overlaps both double-booked ranges.
  });

  ctx.test("failed_booking_does_not_corrupt_state", [&] {
    MyCalendarTwo cal;
    ctx.expect_true(cal.book(1, 10));
    ctx.expect_true(cal.book(1, 10));
    // This wide interval overlaps [1, 10) and must be rejected cleanly without
    // leaving a phantom booking on [10, 20).
    ctx.expect_false(cal.book(5, 20));
    ctx.expect_true(cal.book(10, 20));
    ctx.expect_true(cal.book(10, 20));
    ctx.expect_false(cal.book(15, 16));
  });

  ctx.test("single_interval_overlaps_multiple_disjoint_events", [&] {
    MyCalendarTwo cal;
    ctx.expect_true(cal.book(10, 20));
    ctx.expect_true(cal.book(30, 40));
    ctx.expect_true(cal.book(50, 60));
    // Spans all three disjoint single-booked events cleanly.
    ctx.expect_true(cal.book(15, 55));
    // Gaps [20, 30) and [40, 50) are only single-booked, so they accept one
    // more overlap, while [15, 20), [30, 40), and [50, 55) are double-booked.
    ctx.expect_true(cal.book(20, 30));
    ctx.expect_true(cal.book(40, 50));
    ctx.expect_false(cal.book(18, 25));
    ctx.expect_false(cal.book(35, 36));
    ctx.expect_false(cal.book(52, 58));
  });
}

}  // namespace
