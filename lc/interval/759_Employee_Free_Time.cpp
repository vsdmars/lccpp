// https://leetcode.com/problems/employee-free-time/
//
// Given a list `schedule` of employees where each employee has a sorted list of
// non-overlapping working intervals, return the sorted list of finite,
// positive-length intervals representing common free time for all employees.

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
