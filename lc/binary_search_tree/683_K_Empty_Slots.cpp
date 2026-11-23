// LeetCode 683: K Empty Slots
// https://leetcode.com/problems/k-empty-slots/
//
// Problem:
//   You have N bulbs (or flower slots) numbered 1 to N arranged in a row.
//   Initially all bulbs are turned off. Each day (1-indexed), exactly one bulb
//   is turned on according to the permutation `bulbs`, where `bulbs[d]` is the
//   1-based position turned on during day `d + 1`.
//   Given a non-negative integer k, return the earliest day on which there
//   exist two turned-on bulbs with strictly k turned-off bulbs between them
//   (and no turned-on bulbs in between). Return -1 if no such day exists.
//
// Algorithm:
//   1. Balanced BST (std::set<int>, primary approach matching sorted insertion):
//      Maintain the sorted set of currently blooming positions day by day.
//      When inserting today's position `pos`, the only blooming flowers that
//      could form a gap of k unbloomed slots with `pos` are its immediate
//      predecessor and immediate successor in sorted order (any other blooming
//      flower has at least one blooming flower between it and `pos`).
//      Check if `(pos - prev) - 1 == k` or `(next - pos) - 1 == k`; if so,
//      return the current 1-based day immediately.
//      Time Complexity: O(N log N). Space Complexity: O(N).
//   2. Sliding Window on Bloom-Day Array (optimal O(N) alternative):
//      Invert `bulbs` into `days[slot] = day_it_blooms`. A window [left, right]
//      of width k + 1 is valid iff every interior slot blooms strictly later
//      than both endpoints, yielding candidate day max(days[left], days[right]).
//      Whenever an interior slot `idx` blooms earlier than either endpoint, no
//      valid window can start in (left, idx), so jump `left = idx`.
//      Time Complexity: O(N). Space Complexity: O(N).
//
// Data Structures:
//   - Ordered Set / Red-Black Tree (std::set<int>): supports O(log N) insertion
//     and O(1) amortized predecessor/successor lookup to detect exact gaps.
//   - Inverse Day-Lookup Array (std::vector<int>): maps each slot to its bloom
//     day for the O(N) sliding-window scan.
//
// Real-World Applications:
//   - Memory allocator free-block coalescing and exact-fit gap detection as
//     blocks are allocated over time along a linear address space.
//   - Dynamic spectrum / radio-channel allocation checking whether a newly
//     assigned frequency leaves an exact guard band of k unused channels.
//   - Distributed seating, parking, or rack-slot placement enforcing minimum
//     or exact separation buffers between active units.
//
// Related LeetCode Problems:
//   - #1562 Find Latest Group of Size M [new] (tracks contiguous active segments as bits flip one by one)
//   - #855 Exam Room [done] (maintains occupied seat positions in an ordered set to maximize gaps)
//   - #220 Contains Duplicate III [done] (uses an ordered BST to query nearest neighbors in O(log N))
//   - #715 Range Module [done] (maintains disjoint active intervals in a balanced BST)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <limits>
#include <set>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the earliest 1-based day on which two active bulbs have exactly k
  // inactive bulbs strictly between them, using an ordered BST (std::set).
  // Time: O(N log N), Space: O(N).
  int kEmptySlots(const std::vector<int>& bulbs, int k) {
    if (k < 0 || bulbs.size() < 2uz) {
      return -1;
    }

    // Stores positions of all bulbs currently turned on in ascending order.
    std::set<int> blooming;

    for (std::size_t day_idx = 0uz; day_idx < bulbs.size(); ++day_idx) {
      const int pos = bulbs[day_idx];
      const int day = static_cast<int>(day_idx + 1uz);

      // Insert today's bulb; its iterator gives O(1) access to its immediate
      // left and right neighbors in sorted position order.
      const auto [it, inserted] = blooming.insert(pos);
      if (!inserted) {
        continue;
      }

      // Check immediate predecessor: number of slots strictly between prev and
      // pos is `(pos - prev) - 1`.
      if (it != blooming.begin()) {
        const auto prev_it = std::prev(it);
        if ((pos - *prev_it) - 1 == k) {
          return day;
        }
      }

      // Check immediate successor: number of slots strictly between pos and
      // next is `(next - pos) - 1`.
      const auto next_it = std::next(it);
      if (next_it != blooming.end() && ((*next_it - pos) - 1 == k)) {
        return day;
      }
    }

    return -1;
  }

  // Optimal O(N) sliding-window alternative over the inverse bloom-day array.
  // Time: O(N), Space: O(N).
  int kEmptySlotsSlidingWindow(const std::vector<int>& bulbs, int k) {
    const std::size_t n = bulbs.size();
    if (k < 0) {
      return -1;
    }
    const auto gap = static_cast<std::size_t>(k);
    if ((gap + 2uz) > n) {
      return -1;
    }

    // Invert mapping: days[slot] holds the 1-based day when 0-based `slot`
    // turns on.
    std::vector<int> days(n, 0);
    for (std::size_t day_idx = 0uz; day_idx < n; ++day_idx) {
      const auto slot = static_cast<std::size_t>(bulbs[day_idx] - 1);
      days[slot] = static_cast<int>(day_idx + 1uz);
    }

    // Maintain candidate window [left, right] where right = left + k + 1.
    // Invariant: for every slot j in (left, idx), days[j] > max(days[left],
    // days[right]), meaning j is still off when both endpoints are on.
    std::size_t left = 0uz;
    std::size_t right = (left + gap) + 1uz;
    int earliest_day = std::numeric_limits<int>::max();

    for (std::size_t idx = 1uz; right < n; ++idx) {
      // Interior slot blooms strictly after both endpoints; window still holds.
      if (days[idx] > days[left] && days[idx] > days[right]) {
        continue;
      }

      // Reached the right endpoint without any interior violation: record the
      // day when the second of the two endpoints turns on.
      if (idx == right) {
        earliest_day =
            std::min(earliest_day, std::max(days[left], days[right]));
      }

      // Either the window succeeded or `idx` bloomed earlier than an endpoint;
      // in both cases no valid window can start strictly between left and idx,
      // so advance left directly to idx.
      left = idx;
      right = (left + gap) + 1uz;
    }

    return earliest_day == std::numeric_limits<int>::max() ? -1 : earliest_day;
  }
};

LC_PROBLEM(683, "K Empty Slots", "binary_search_tree", ctx) {
  Solution sol;

  auto expect_both = [&](const std::vector<int>& bulbs, int k, int expected) {
    ctx.expect_eq(sol.kEmptySlots(bulbs, k), expected);
    ctx.expect_eq(sol.kEmptySlotsSlidingWindow(bulbs, k), expected);
  };

  ctx.test("example_1_gap_one_found_day_two", [&] {
    expect_both({1, 3, 2}, 1, 2);
  });

  ctx.test("example_2_no_valid_gap", [&] {
    expect_both({1, 2, 3}, 1, -1);
  });

  ctx.test("python_build_cases", [&] {
    expect_both({1, 5, 2, 4, 3}, 2, 3);
    expect_both({1, 3, 2}, 2, -1);
  });

  ctx.test("zero_gap_adjacent_bulbs", [&] {
    expect_both({1, 3, 2}, 0, 3);
    expect_both({2, 1}, 0, 2);
  });

  ctx.test("single_bulb_and_oversized_k", [&] {
    expect_both({1}, 0, -1);
    expect_both({1, 4, 2, 3}, 3, -1);
    expect_both({1, 5, 3, 2, 4}, 3, 2);
  });

  ctx.test("multiple_valid_days_returns_earliest", [&] {
    // Day 1: {6}, Day 2: {1, 6} (gap 4), Day 3: {1, 4, 6} (gap 2 at [1,4] and
    // gap 1 at [4,6]), Day 4: {1, 3, 4, 6} (another gap 1 at [1,3]).
    // For k = 1, [1,3] yields day 4 while [4,6] yields day 3; earliest is 3.
    expect_both({6, 1, 4, 3, 2, 5}, 1, 3);
    expect_both({6, 1, 4, 3, 2, 5}, 2, 3);
  });
}

}  // namespace
