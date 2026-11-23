// LeetCode 729: My Calendar I
// https://leetcode.com/problems/my-calendar-i/
//
// Problem:
//   Design a calendar data structure `MyCalendar` that supports booking events
//   represented as half-open integer intervals `[start, end)` (`start <= x < end`).
//   Calling `book(start, end)` adds the event and returns `true` if it does not
//   overlap with any previously booked event; otherwise it leaves the calendar
//   unchanged and returns `false`.
//   Constraints: `0 <= start < end <= 10^9`, at most `1000` calls to `book`.
//
// Algorithm:
//   Ordered interval map with binary search (`lower_bound`):
//   Store all accepted, mutually disjoint intervals in a balanced binary search
//   tree (`std::map<int, int>`) mapping `start -> end`. Because accepted
//   intervals never overlap, sorting by `start` also sorts them strictly by
//   `end`. To test a candidate `[start, end)`:
//   1. Find `next = bookings_.lower_bound(start)`, the first booked interval
//      with `next->first >= start`. If `next` exists and `next->first < end`,
//      then `[start, end)` overlaps `next` -> reject.
//   2. If `next != bookings_.begin()`, inspect the immediate predecessor
//      `prev = std::prev(next)` (which has `prev->first < start`). If
//      `prev->second > start`, then `[start, end)` overlaps `prev` -> reject.
//   3. Otherwise, insert `(start, end)` using `next` as an iterator hint and
//      return `true`.
//   Time Complexity:  O(log n) per `book` call; O(n log n) for `n` calls.
//   Space Complexity: O(n) for storing up to `n` booked intervals.
//
// Data Structures:
//   - Balanced BST (`std::map<int, int>`): keeps non-overlapping intervals
//     sorted by start time and provides O(log n) neighbor lookup and hinted
//     insertion.
//
// Real-World Applications:
//   - Meeting room, hotel, and resource reservation engines enforcing
//     non-overlapping time-slot locks.
//   - Operating system virtual memory area (VMA) managers (`mmap`) checking
//     whether a requested address range `[addr, addr + len)` collides with
//     existing mappings.
//   - Network bandwidth / spectrum slot allocation and cloud maintenance-window
//     conflict detection.
//
// Related LeetCode Problems:
//   - #731 My Calendar II [done] (allow up to double bookings, forbid triple bookings)
//   - #732 My Calendar III [new] (sweep-line maximum concurrent bookings)
//   - #56 Merge Intervals [done] (offline interval sorting and merging)
//   - #252 Meeting Rooms [done] (offline check for any overlapping interval pair)
//   - #57 Insert Interval [done] (inserting a new interval into a sorted disjoint set)

#include "framework/lc_framework.h"

#include <iterator>
#include <map>

namespace {

// Calendar supporting O(log n) non-overlapping half-open interval bookings.
class MyCalendar {
 public:
  // Attempts to book the half-open interval `[start, end)`.
  // Returns `true` and records the interval if it does not intersect any
  // existing booking; otherwise returns `false`.
  //
  // Time: O(log n), Space: O(1) per call (O(n) total).
  bool book(int start, int end) {
    // Find the earliest interval whose start time is >= `start`.
    const auto next = bookings_.lower_bound(start);

    // Overlap with successor occurs iff `next.start < end` (since `next.start >= start`).
    // Adjacent half-open intervals where `next.start == end` do not conflict.
    if (next != bookings_.end() && next->first < end) {
      return false;
    }

    // Overlap with predecessor occurs iff `prev.end > start` (since `prev.start < start`).
    // Adjacent half-open intervals where `prev.end == start` do not conflict.
    if (next != bookings_.begin()) {
      const auto prev = std::prev(next);
      if (prev->second > start) {
        return false;
      }
    }

    // No conflict with either immediate neighbor guarantees disjointness from
    // all intervals in the sorted map; insert with `next` as placement hint.
    bookings_.emplace_hint(next, start, end);
    return true;
  }

 private:
  // Disjoint booked intervals stored as `start -> end`, ordered by `start`.
  std::map<int, int> bookings_;
};

LC_PROBLEM(729, "My Calendar I", "sort", ctx) {
  ctx.test("example_1", [&] {
    MyCalendar cal;
    ctx.expect_true(cal.book(10, 20));
    ctx.expect_false(cal.book(15, 25));
    ctx.expect_true(cal.book(20, 30));
  });

  ctx.test("exact_duplicate_and_containment", [&] {
    MyCalendar cal;
    ctx.expect_true(cal.book(10, 30));
    ctx.expect_false(cal.book(10, 30));  // Exact duplicate.
    ctx.expect_false(cal.book(15, 20));  // Strictly inside existing.
    ctx.expect_false(cal.book(5, 35));   // Strictly encloses existing.
    ctx.expect_false(cal.book(5, 11));   // Overlaps left edge.
    ctx.expect_false(cal.book(29, 35));  // Overlaps right edge.
  });

  ctx.test("adjacent_half_open_boundaries", [&] {
    MyCalendar cal;
    ctx.expect_true(cal.book(20, 30));
    ctx.expect_true(cal.book(10, 20));  // Touches left boundary.
    ctx.expect_true(cal.book(30, 40));  // Touches right boundary.
    ctx.expect_true(cal.book(0, 10));   // Starts at 0.
    ctx.expect_false(cal.book(19, 21));
  });

  ctx.test("fit_between_existing_intervals", [&] {
    MyCalendar cal;
    ctx.expect_true(cal.book(10, 20));
    ctx.expect_true(cal.book(30, 40));
    ctx.expect_true(cal.book(20, 30));   // Exact fit between neighbors.
    ctx.expect_false(cal.book(25, 26));  // Now occupied.
  });

  ctx.test("large_coordinates", [&] {
    MyCalendar cal;
    ctx.expect_true(cal.book(0, 1'000'000'000));
    ctx.expect_false(cal.book(500'000'000, 500'000'001));
  });
}

}  // namespace
