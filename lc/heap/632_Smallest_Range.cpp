// LeetCode 632: Smallest Range
// https://leetcode.com/problems/smallest-range-covering-elements-from-k-lists/
//
// Problem:
//   You are given k lists of sorted integers in non-decreasing order. Find the
//   smallest closed interval [a, b] that contains at least one number from
//   each of the k lists. An interval [a, b] is smaller than [c, d] if
//   (b - a) < (d - c), or if (b - a) == (d - c) and a < c.
//
// Examples:
//   Example 1:
//     Input:  nums = [[4, 10, 15, 24, 26], [0, 9, 12, 20], [5, 18, 22, 30]]
//     Output: [20, 24]
//     Explanation:
//       - Seed min_heap with (0, list 1, idx 0), (4, list 0, idx 0), and (5, list
//         2, idx 0), setting current_max = 5 and best range [0, 5] (span 5).
//       - Repeatedly pop the minimum entry and advance its list, scanning through
//         windows [4, 9], [5, 10], [9, 18], [10, 18], [12, 18], [15, 20], and [18,
//         24] without beating span 5.
//       - After advancing list 0 to 24 and list 2 to 22, popping (20, list 1, idx
//         3) sees current_max = 24, updating the best range to [20, 24] (span 4 <
//         5).
//       - List 1 has no element after 20 (next_idx == 4), so the loop terminates
//         and returns [20, 24].
//
//   Example 2:
//     Input:  nums = [[-10, -5, 0], [-8, -2, 4], [-6, 1, 5]]
//     Output: [-8, -5]
//     Explanation:
//       - Initialize min_heap with -10, -8, and -6, setting current_max = -6 and
//         initial best range [-10, -6] (span 4).
//       - Pop -10 from list 0 and push -5 (current_max = -5), then pop -8 from list
//         1 to record [-8, -5] with span 3 < 4.
//       - Continue advancing through [-6, -2] (span 4), [-5, 1] (span 6), and [-2,
//         1] (span 3, loses tie-break because -2 > -8) until list 0 finishes at 0,
//         returning [-8, -5].
//
//   Example 3:
//     Input:  nums = [[10, 20, 30]]
//     Output: [10, 10]
//     Explanation:
//       - Seed min_heap with (10, list 0, idx 0) and current_max = 10, initializing
//         best range to [10, 10] (span 0).
//       - Advance through (20, list 0, idx 1) and (30, list 0, idx 2), each giving
//         span 0 with larger left endpoints until list 0 ends, returning [10, 10].
//
// Algorithm:
//   K-Way Min-Heap Window Tracking:
//   1. Maintain a window containing exactly one active element from each of
//      the k sorted lists, initialized with index 0 of every list.
//   2. Store all k active elements in a min-heap so the current window minimum
//      `left` is always at the top, and track the current window maximum
//      `current_max` in a scalar variable.
//   3. At each step, `[left, current_max]` is a valid covering range. Update
//      the best range if its width is strictly smaller (or equal with smaller
//      left endpoint).
//   4. To shrink the window, we must increase its minimum: pop the minimum
//      entry `(left, list_idx, elem_idx)` and advance that list to
//      `elem_idx + 1`, updating `current_max`. As soon as any list is
//      exhausted, no further range can cover all k lists, so terminate.
//   Time:  O(N * log(k)), where N is the total number of elements across all k
//          lists.
//   Space: O(k) for the min-heap.
//
// Data Structures:
//   - Min-heap (`std::priority_queue` with `std::greater<>`) holding one
//     `(value, list_idx, elem_idx)` entry per list to retrieve the window
//     minimum in O(log k).
//   - Running scalar `current_max` tracking the window maximum in O(1).
//
// Real-World Applications:
//   - Full-text search engines generating minimal proximity snippets that
//     contain all query keywords across sorted posting lists.
//   - Multi-sensor time-series synchronization finding the tightest time
//     window containing a reading from every sensor stream.
//   - Event correlation and log aggregation across distributed services.
//
// Related LeetCode Problems:
//   - #23 Merge k Sorted Lists [done] (k-way min-heap merge foundation)
//   - #76 Minimum Window Substring [done] (shortest covering window over a single sequence)
//   - #378 Kth Smallest Element in a Sorted Matrix [done] (k-way heap traversal)
//   - #239 Sliding Window Maximum [done] (window extreme tracking)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the shortest interval [a, b] containing at least one element from
  // each sorted list in `nums`.
  // Time: O(N * log(k)), Space: O(k).
  [[nodiscard]] std::vector<int> smallestRange(
      const std::vector<std::vector<int>>& nums) const {
    if (nums.empty()) {
      return {};
    }

    struct Entry {
      int value = 0;
      std::size_t list_idx = 0;
      std::size_t elem_idx = 0;

      [[nodiscard]] bool operator>(const Entry& other) const noexcept {
        return value > other.value;
      }
    };

    std::vector<Entry> initial;
    initial.reserve(nums.size());
    int current_max = std::numeric_limits<int>::min();

    // Seed the window with the first element of each list.
    for (std::size_t i = 0uz; i < nums.size(); ++i) {
      if (nums[i].empty()) {
        return {};
      }
      initial.push_back(
          Entry{.value = nums[i][0], .list_idx = i, .elem_idx = 0});
      current_max = std::max(current_max, nums[i][0]);
    }

    // Heapify in O(k) time so min_heap.top() gives the initial window minimum.
    std::priority_queue<Entry, std::vector<Entry>, std::greater<>> min_heap(
        std::greater<>{}, std::move(initial));

    int best_left = min_heap.top().value;
    int best_right = current_max;

    while (!min_heap.empty()) {
      const Entry smallest = min_heap.top();
      min_heap.pop();

      // Compare range spans in 64-bit arithmetic to avoid overflow when
      // endpoints span negative and positive extremes.
      const std::int64_t cur_span =
          static_cast<std::int64_t>(current_max) - smallest.value;
      const std::int64_t best_span =
          static_cast<std::int64_t>(best_right) - best_left;
      if (cur_span < best_span ||
          (cur_span == best_span && smallest.value < best_left)) {
        best_left = smallest.value;
        best_right = current_max;
      }

      // Advance the list that owned the minimum element; if it reaches the end,
      // no subsequent window can cover all k lists.
      const std::size_t next_idx = smallest.elem_idx + 1;
      if (next_idx == nums[smallest.list_idx].size()) {
        break;
      }

      const int next_val = nums[smallest.list_idx][next_idx];
      current_max = std::max(current_max, next_val);
      min_heap.push(Entry{
          .value = next_val,
          .list_idx = smallest.list_idx,
          .elem_idx = next_idx,
      });
    }

    return {best_left, best_right};
  }
};

LC_PROBLEM(632, "Smallest Range", "heap", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    const std::vector<std::vector<int>> nums = {
        {4, 10, 15, 24, 26},
        {0, 9, 12, 20},
        {5, 18, 22, 30},
    };
    ctx.expect_eq(sol.smallestRange(nums), std::vector<int>{20, 24});
  });

  ctx.test("identical_lists_zero_span", [&] {
    Solution sol;
    const std::vector<std::vector<int>> nums = {
        {1, 2, 3},
        {1, 2, 3},
        {1, 2, 3},
    };
    ctx.expect_eq(sol.smallestRange(nums), std::vector<int>{1, 1});
  });

  ctx.test("negative_values_and_tie_break", [&] {
    Solution sol;
    const std::vector<std::vector<int>> nums = {
        {-10, -5, 0},
        {-8, -2, 4},
        {-6, 1, 5},
    };
    // Both [-8, -5] and [-2, 1] have span 3; [-8, -5] wins by smaller left.
    ctx.expect_eq(sol.smallestRange(nums), std::vector<int>{-8, -5});
  });

  ctx.test("single_list", [&] {
    Solution sol;
    const std::vector<std::vector<int>> nums = {{10, 20, 30}};
    ctx.expect_eq(sol.smallestRange(nums), std::vector<int>{10, 10});
  });
}

}  // namespace
