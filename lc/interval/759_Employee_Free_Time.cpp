// LeetCode 759: Employee Free Time
// https://leetcode.com/problems/employee-free-time/
//
// Problem:
//   Given a list `schedule` where `schedule[e]` is a sorted list of
//   non-overlapping busy intervals `[start, end]` for employee `e`, return the
//   sorted list of finite, positive-length intervals representing time spans
//   when all employees are simultaneously free. Leading `(-inf, min_start)`
//   and trailing `(max_end, +inf)` intervals are excluded, as are zero-length
//   gaps between touching intervals.
//   Constraints: 1 <= schedule.size() <= 50, each employee has 1..50 intervals
//   (empty lists are handled gracefully), 0 <= start < end <= 10^8.
//
// Algorithm:
//   Two approaches are implemented:
//   1. `employeeFreeTime` (K-way min-heap merge): Push each non-empty
//      employee's first interval into a min-heap ordered by `start`. Repeatedly
//      pop the earliest interval, push that employee's next interval, and
//      track the rightmost merged busy end `current_end`. Whenever the popped
//      interval satisfies `iv.start > current_end`, record the common free gap
//      `[current_end, iv.start]`, then update `current_end = max(current_end,
//      iv.end)`.
//      Time: O(N log K), Space: O(K) auxiliary (excluding output), where N is
//      the total number of intervals and K is the number of employees.
//   2. `employeeFreeTimeSort` (flatten and sort): Flatten all intervals into a
//      single vector, sort by `start`, and run the same gap-detection scan.
//      Time: O(N log N), Space: O(N).
//   Key invariant: because intervals are visited in non-decreasing start order,
//   `current_end` equals the end of the merged busy union seen so far; a gap
//   exists if and only if the next interval starts strictly after `current_end`.
//
// Data Structures:
//   - `struct Interval`: `[start, end]` value type representing busy/free spans.
//   - `struct HeapEntry` & `HeapEntryGreater`: min-heap cursor storing
//     `(start, emp_idx, iv_idx)` to advance each employee's sorted list.
//   - `std::priority_queue<HeapEntry, ...> min_heap`: K-way merge frontier.
//   - `std::vector<Interval> all` (in `employeeFreeTimeSort`): flattened buffer
//     for global sorting.
//
// Real-World Applications:
//   - Multi-participant calendar "Find a Time" scheduling across teams to
//     surface mutually open meeting windows.
//   - Identifying global maintenance windows when all dependent services or
//     batch pipelines are simultaneously idle.
//   - Computing idle gaps across multi-machine or multi-core execution traces
//     in distributed systems profilers.
//
// Related LeetCode Problems:
//   - #56 Merge Intervals [done] (merging overlapping busy intervals into a union)
//   - #23 Merge k Sorted Lists [done] (same K-way min-heap merge over K sorted streams)
//   - #986 Interval List Intersections [done] (overlap between two sorted schedules)
//   - #1229 Meeting Scheduler [new] (earliest common free slot of a target duration)
//   - #57 Insert Interval [done] (maintaining a sorted disjoint interval set)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <ostream>
#include <queue>
#include <vector>

namespace {

struct Interval {
  int start = 0;
  int end = 0;

  bool operator==(const Interval&) const = default;
};

std::ostream& operator<<(std::ostream& os, const Interval& iv) {
  return os << '[' << iv.start << ", " << iv.end << ']';
}

struct HeapEntry {
  int start = 0;
  std::size_t emp_idx = 0uz;
  std::size_t iv_idx = 0uz;
};

struct HeapEntryGreater {
  bool operator()(const HeapEntry& a, const HeapEntry& b) const noexcept {
    return a.start > b.start;
  }
};

class Solution {
 public:
  // K-way merge across the K employees' already-sorted schedules using a
  // min-heap ordered by interval start. Tracks the rightmost merged busy end
  // `current_end`; any interval starting strictly after `current_end` exposes
  // a common free gap [current_end, interval.start].
  //
  // Time:  O(N log K), where N = total intervals and K = number of employees
  // Space: O(K) auxiliary (excluding output)
  std::vector<Interval> employeeFreeTime(
      const std::vector<std::vector<Interval>>& schedule) {
    std::priority_queue<HeapEntry, std::vector<HeapEntry>, HeapEntryGreater>
        min_heap;
    for (std::size_t e = 0uz; e < schedule.size(); ++e) {
      if (!schedule[e].empty()) {
        min_heap.push(HeapEntry{
            .start = schedule[e][0].start, .emp_idx = e, .iv_idx = 0uz});
      }
    }

    std::vector<Interval> result;
    bool has_prev = false;
    int current_end = 0;

    while (!min_heap.empty()) {
      const HeapEntry top = min_heap.top();
      min_heap.pop();

      const Interval& iv = schedule[top.emp_idx][top.iv_idx];
      if (has_prev) {
        if (iv.start > current_end) {
          result.push_back(Interval{.start = current_end, .end = iv.start});
        }
        current_end = std::max(current_end, iv.end);
      } else {
        current_end = iv.end;
        has_prev = true;
      }

      const std::size_t next_iv = top.iv_idx + 1uz;
      if (next_iv < schedule[top.emp_idx].size()) {
        min_heap.push(HeapEntry{.start = schedule[top.emp_idx][next_iv].start,
                                .emp_idx = top.emp_idx,
                                .iv_idx = next_iv});
      }
    }

    return result;
  }

  // Alternative flatten-and-sort approach (analogous to LC #56 Merge
  // Intervals).
  //
  // Time:  O(N log N)
  // Space: O(N)
  std::vector<Interval> employeeFreeTimeSort(
      const std::vector<std::vector<Interval>>& schedule) {
    std::vector<Interval> all;
    for (const auto& emp : schedule) {
      all.insert(all.end(), emp.begin(), emp.end());
    }
    if (all.empty()) {
      return {};
    }

    std::ranges::sort(all, {}, &Interval::start);

    std::vector<Interval> result;
    int current_end = all[0].end;
    for (std::size_t i = 1uz; i < all.size(); ++i) {
      if (all[i].start > current_end) {
        result.push_back(Interval{.start = current_end, .end = all[i].start});
      }
      current_end = std::max(current_end, all[i].end);
    }

    return result;
  }
};

LC_PROBLEM(759, "Employee Free Time", "interval", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    const std::vector<std::vector<Interval>> schedule = {
        {{.start = 1, .end = 2}, {.start = 5, .end = 6}},
        {{.start = 1, .end = 3}},
        {{.start = 4, .end = 10}},
    };
    const std::vector<Interval> expected = {{.start = 3, .end = 4}};
    ctx.expect_eq(sol.employeeFreeTime(schedule), expected);
    ctx.expect_eq(sol.employeeFreeTimeSort(schedule), expected);
  });

  ctx.test("example_2", [&] {
    const std::vector<std::vector<Interval>> schedule = {
        {{.start = 1, .end = 3}, {.start = 6, .end = 7}},
        {{.start = 2, .end = 4}},
        {{.start = 2, .end = 5}, {.start = 9, .end = 12}},
    };
    const std::vector<Interval> expected = {{.start = 5, .end = 6},
                                            {.start = 7, .end = 9}};
    ctx.expect_eq(sol.employeeFreeTime(schedule), expected);
    ctx.expect_eq(sol.employeeFreeTimeSort(schedule), expected);
  });

  ctx.test("no_common_free_time_and_touching_intervals", [&] {
    const std::vector<std::vector<Interval>> schedule = {
        {{.start = 1, .end = 2}, {.start = 2, .end = 5}},
        {{.start = 2, .end = 4}, {.start = 4, .end = 8}},
    };
    const std::vector<Interval> expected = {};
    ctx.expect_eq(sol.employeeFreeTime(schedule), expected);
    ctx.expect_eq(sol.employeeFreeTimeSort(schedule), expected);
  });

  ctx.test("single_employee_and_empty_inputs", [&] {
    const std::vector<std::vector<Interval>> single = {
        {{.start = 1, .end = 3},
         {.start = 5, .end = 7},
         {.start = 10, .end = 12}}};
    const std::vector<Interval> expected = {{.start = 3, .end = 5},
                                            {.start = 7, .end = 10}};
    ctx.expect_eq(sol.employeeFreeTime(single), expected);
    ctx.expect_eq(sol.employeeFreeTimeSort(single), expected);

    const std::vector<std::vector<Interval>> empty_schedule = {{}, {}};
    const std::vector<Interval> empty_expected = {};
    ctx.expect_eq(sol.employeeFreeTime(empty_schedule), empty_expected);
    ctx.expect_eq(sol.employeeFreeTimeSort(empty_schedule), empty_expected);
  });
}

}  // namespace
