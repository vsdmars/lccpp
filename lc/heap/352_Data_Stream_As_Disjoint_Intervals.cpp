// LeetCode 352: Data Stream As Disjoint Intervals
// https://leetcode.com/problems/data-stream-as-disjoint-intervals/
//
// Problem:
//   Given a data stream of non-negative integers `a1, a2, ..., an, ...`,
//   design a `SummaryRanges` data structure supporting:
//   - `addNum(int value)`: adds `value` to the stream.
//   - `getIntervals()`: returns a summary of all integers seen so far as a
//     sorted list of disjoint closed intervals `[start_i, end_i]` such that no
//     two intervals overlap or are adjacent (`end_i + 1 < start_{i+1}`).
//
// Algorithm:
//   Primary (`SummaryRanges`, Lazy Min-Heap Compaction):
//     Matches the primary Python solution:
//     - On `addNum(value)`: deduplicate with an unordered set; if unseen, push
//       the singleton interval `[value, value]` into a min-heap ordered by
//       `(start, end)`.
//     - On `getIntervals()`: pop all intervals from the min-heap in ascending
//       order, coalescing any interval whose `start <= prev.end + 1` into the
//       previous interval, and re-seed the min-heap with only the compacted
//       intervals so subsequent queries benefit from prior merges.
//     Time Complexity: `addNum` is O(log N) amortized; `getIntervals` is
//     O(M log M) where M is the number of uncompacted intervals since the last
//     call (and O(I log I) on repeated calls with I disjoint intervals).
//     Space Complexity: O(N) for the deduplication set plus O(I) for intervals.
//
//   Follow-up (`SummaryRangesOrderedMap`, Eager Balanced-BST Merge):
//     Answers the follow-up ("many merges, few disjoint intervals"):
//     Store disjoint intervals in a `std::map<int, int>` (`start -> end`). On
//     `addNum(value)`, locate the first interval starting after `value` via
//     `upper_bound(value)` and check its immediate predecessor and successor to
//     ignore duplicates, extend left/right, or bridge both into one interval.
//     Time Complexity: `addNum` is O(log I); `getIntervals` is O(I).
//     Space Complexity: O(I), where I is the number of disjoint intervals.
//
// Data Structures:
//   - std::priority_queue (min-heap of intervals) + std::unordered_set<int>
//     for lazy-compaction streaming range tracking.
//   - std::map<int, int> (red-black tree) for O(log I) eager interval merging.
//
// Real-World Applications:
//   - TCP selective acknowledgment (SACK) blocks tracking contiguous received
//     sequence-number ranges out of an out-of-order packet stream.
//   - Memory allocator / filesystem free-extent coalescing (merging adjacent
//     freed block ranges into maximal contiguous extents).
//   - Video streaming / distributed download chunk-progress trackers.
//
// Related LeetCode Problems:
//   - #56 Merge Intervals [done] (offline batch merging of overlapping intervals)
//   - #57 Insert Interval [done] (inserting a single interval into a sorted disjoint list)
//   - #228 Summary Ranges [done] (offline summary of a sorted array into contiguous ranges)
//   - #715 Range Module [done] (supports both adding and removing arbitrary intervals with an ordered map)
//   - #2276 Count Integers in Intervals [new] (maintains disjoint intervals plus live total cardinality)

#include "framework/lc_framework.h"

#include <algorithm>
#include <compare>
#include <functional>
#include <iterator>
#include <map>
#include <queue>
#include <unordered_set>
#include <vector>

namespace {

// Primary implementation using a min-heap + visited set with lazy compaction
// on `getIntervals()`, matching the Python `SummaryRanges` solution.
class SummaryRanges {
 public:
  // Records `value` as a singleton interval if it has not been seen before.
  // Time: O(log N) average.
  void addNum(int value) {
    if (visited_.insert(value).second) {
      min_heap_.push(Interval{.start = value, .end = value});
    }
  }

  // Drains and coalesces all pending intervals in sorted order, then stores
  // the compacted intervals back into the heap for future calls.
  // Time: O(M log M) where M is the current heap size.
  std::vector<std::vector<int>> getIntervals() {
    std::vector<Interval> merged;

    while (!min_heap_.empty()) {
      const Interval head = min_heap_.top();
      min_heap_.pop();

      // Merge if `head` overlaps or is immediately adjacent to `merged.back()`.
      // Check `head.start - 1 == merged.back().end` to avoid `end + 1` overflow.
      if (!merged.empty() && (head.start <= merged.back().end ||
                              head.start - 1 == merged.back().end)) {
        merged.back().end = std::max(merged.back().end, head.end);
      } else {
        merged.push_back(head);
      }
    }

    std::vector<std::vector<int>> result;
    result.reserve(merged.size());
    for (const Interval& iv : merged) {
      min_heap_.push(iv);
      result.push_back({iv.start, iv.end});
    }
    return result;
  }

 private:
  struct Interval {
    int start = 0;
    int end = 0;

    auto operator<=>(const Interval&) const = default;
  };

  std::priority_queue<Interval, std::vector<Interval>, std::greater<>>
      min_heap_;
  std::unordered_set<int> visited_;
};

// Follow-up implementation maintaining disjoint intervals eagerly in a
// balanced BST (`std::map<start, end>`) so space is O(I) and `addNum` is
// O(log I) even when the stream has many duplicates or heavy merging.
class SummaryRangesOrderedMap {
 public:
  // Inserts `value` and immediately merges with adjacent left/right intervals.
  // Time: O(log I), Space: O(I).
  void addNum(int value) {
    auto next = intervals_.upper_bound(value);
    const bool has_prev = (next != intervals_.begin());
    auto prev = has_prev ? std::prev(next) : intervals_.end();

    // Already contained in the preceding interval `[prev->first, prev->second]`.
    if (has_prev && prev->second >= value) {
      return;
    }

    // Because `prev->second < value` and `next->first > value`, neither
    // `prev->second + 1` nor `next->first - 1` can overflow.
    const bool merge_left = has_prev && (prev->second + 1 == value);
    const bool merge_right =
        (next != intervals_.end()) && (next->first - 1 == value);

    if (merge_left && merge_right) {
      prev->second = next->second;
      intervals_.erase(next);
    } else if (merge_left) {
      prev->second = value;
    } else if (merge_right) {
      const int new_end = next->second;
      intervals_.erase(next);
      intervals_.emplace(value, new_end);
    } else {
      intervals_.emplace(value, value);
    }
  }

  // Returns all disjoint intervals in sorted order.
  // Time: O(I).
  [[nodiscard]] std::vector<std::vector<int>> getIntervals() const {
    std::vector<std::vector<int>> result;
    result.reserve(intervals_.size());
    for (const auto& [start, end] : intervals_) {
      result.push_back({start, end});
    }
    return result;
  }

 private:
  // Maps each disjoint interval's `start` to its inclusive `end`.
  std::map<int, int> intervals_;
};

LC_PROBLEM(352, "Data Stream As Disjoint Intervals", "heap", ctx) {
  ctx.test("example_1_step_by_step", [&] {
    SummaryRanges heap_sr;
    SummaryRangesOrderedMap map_sr;

    const auto check = [&](int val,
                           const std::vector<std::vector<int>>& expected) {
      heap_sr.addNum(val);
      map_sr.addNum(val);
      ctx.expect_eq(heap_sr.getIntervals(), expected);
      ctx.expect_eq(map_sr.getIntervals(), expected);
    };

    check(1, {{1, 1}});
    check(3, {{1, 1}, {3, 3}});
    check(7, {{1, 1}, {3, 3}, {7, 7}});
    check(2, {{1, 3}, {7, 7}});
    check(6, {{1, 3}, {6, 7}});
  });

  ctx.test("python_main_sequence", [&] {
    SummaryRanges heap_sr;
    SummaryRangesOrderedMap map_sr;
    for (const int v : {1, 5, 2, 4}) {
      heap_sr.addNum(v);
      map_sr.addNum(v);
    }
    const std::vector<std::vector<int>> expected = {{1, 2}, {4, 5}};
    ctx.expect_eq(heap_sr.getIntervals(), expected);
    ctx.expect_eq(map_sr.getIntervals(), expected);

    // Adding 3 bridges both intervals into [1, 5].
    heap_sr.addNum(3);
    map_sr.addNum(3);
    ctx.expect_eq(heap_sr.getIntervals(),
                  std::vector<std::vector<int>>({{1, 5}}));
    ctx.expect_eq(map_sr.getIntervals(),
                  std::vector<std::vector<int>>({{1, 5}}));
  });

  ctx.test("duplicates_zero_and_reverse_order", [&] {
    SummaryRanges heap_sr;
    SummaryRangesOrderedMap map_sr;

    ctx.expect_eq(heap_sr.getIntervals(), std::vector<std::vector<int>>{});
    ctx.expect_eq(map_sr.getIntervals(), std::vector<std::vector<int>>{});

    for (const int v : {5, 4, 4, 3, 0, 2, 1, 0, 5}) {
      heap_sr.addNum(v);
      map_sr.addNum(v);
    }
    const std::vector<std::vector<int>> expected = {{0, 5}};
    ctx.expect_eq(heap_sr.getIntervals(), expected);
    ctx.expect_eq(map_sr.getIntervals(), expected);
  });
}

}  // namespace
