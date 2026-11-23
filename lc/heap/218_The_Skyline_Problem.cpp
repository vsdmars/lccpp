// LeetCode 218: The Skyline Problem
// https://leetcode.com/problems/the-skyline-problem/
//
// Problem:
//   Given a list of axis-aligned rectangular buildings where each building is
//   represented as `[left_i, right_i, height_i]` (sorted in non-decreasing
//   order of `left_i`, with `0 <= left_i < right_i <= INT_MAX` and
//   `0 < height_i <= INT_MAX`), compute the outer contour ("skyline") formed
//   by all buildings collectively. Return the skyline as a list of key points
//   `[x, y]` sorted by `x`, where each key point is the left endpoint of a
//   horizontal segment, with no two consecutive key points sharing the same
//   height, ending with height 0 after the rightmost building.
//
// Algorithm:
//   Sweep-line with a lazy-deletion max-heap of active buildings:
//   Maintain a max-heap `live` of `(height, right)` pairs representing
//   buildings whose left edge has been swept past. At each step, the next
//   critical x-coordinate is either the next building's left edge or the right
//   edge of the currently tallest active building (`live.top().right`),
//   whichever comes first:
//   - If `live` is empty or the next building starts at or before
//     `live.top().right`, advance `x` to that left edge and push every building
//     starting at `x` into `live`.
//   - Otherwise, advance `x` to `live.top().right` and pop all active
//     buildings whose right edge is <= `x` (lazy expiration of buried shorter
//     buildings).
//   After updating `live`, the current skyline height is `live.top().height`
//   (or 0 if `live` is empty). Emit `[x, cur_height]` only when `cur_height`
//   differs from the previously recorded skyline height.
//   Time Complexity: O(N log N) — each building is pushed onto and popped from
//   the max-heap at most once.
//   Space Complexity: O(N) — for the active-building heap and output skyline.
//
// Data Structures:
//   - std::priority_queue<ActiveBuilding> (max-heap ordered by height descending,
//     breaking ties by right edge descending): identifies the tallest currently
//     active building in O(1) and supports lazy eviction in amortized O(log N).
//   - std::vector<std::vector<int>>: accumulates deduplicated skyline key points.
//
// Real-World Applications:
//   - Computational geometry and 2D CAD/GIS union-of-rectangles contour
//     extraction and visibility / shadow-horizon rendering in graphics engines.
//   - Cloud resource utilization and capacity-planning dashboards that compute
//     peak concurrent demand envelopes across overlapping job intervals.
//   - Network bandwidth / SLA envelope calculation over scheduled reservations.
//
// Related LeetCode Problems:
//   - #56 Merge Intervals [done] (1D interval union without height dimension)
//   - #253 Meeting Rooms II [done] (sweep-line / min-heap tracking concurrent active intervals)
//   - #699 Falling Squares [new] (dynamic 1D skyline height queries over sequential drops)
//   - #732 My Calendar III [new] (sweep-line / boundary counting over overlapping intervals)
//   - #850 Rectangle Area II [new] (2D sweep-line computing total union area of rectangles)

#include "framework/lc_framework.h"

#include <compare>
#include <cstddef>
#include <limits>
#include <queue>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the outer skyline contour key points from buildings sorted by
  // left x-coordinate using a sweep-line and lazy-deletion max-heap.
  // Time: O(N log N), Space: O(N).
  std::vector<std::vector<int>> getSkyline(
      const std::vector<std::vector<int>>& buildings) {
    // Ordered lexicographically by (height, right) in a max-heap so the
    // tallest building (and among ties, the one extending furthest right) sits
    // at the top. Storing positive values avoids negation overflow on INT_MAX.
    struct ActiveBuilding {
      int height = 0;
      int right = 0;

      auto operator<=>(const ActiveBuilding&) const = default;
    };

    std::vector<std::vector<int>> skyline;
    std::priority_queue<ActiveBuilding> live;

    const std::size_t n = buildings.size();
    std::size_t idx = 0uz;

    while (idx < n || !live.empty()) {
      int x = 0;

      // Process starting events first when a new building begins at or before
      // the tallest active building ends. If the new building is taller, it
      // immediately raises the skyline at `x`; if shorter and starting exactly
      // at `live.top().right`, the height stays unchanged on this step and the
      // next iteration expires the finished taller building at the same `x`.
      if (live.empty() || (idx < n && buildings[idx][0] <= live.top().right)) {
        x = buildings[idx][0];
        while (idx < n && buildings[idx][0] == x) {
          live.push(ActiveBuilding{
              .height = buildings[idx][2],
              .right = buildings[idx][1],
          });
          ++idx;
        }
      } else {
        // The tallest active building ends before the next building begins;
        // advance to its right edge and lazily purge all expired buildings.
        x = live.top().right;
        while (!live.empty() && live.top().right <= x) {
          live.pop();
        }
      }

      const int cur_height = live.empty() ? 0 : live.top().height;

      // Suppress consecutive horizontal segments of equal height.
      if (skyline.empty() || skyline.back()[1] != cur_height) {
        skyline.push_back({x, cur_height});
      }
    }

    return skyline;
  }
};

LC_PROBLEM(218, "The Skyline Problem", "heap", ctx) {
  ctx.test("example_1_cityscape", [&] {
    Solution sol;
    const std::vector<std::vector<int>> buildings = {
        {2, 9, 10}, {3, 7, 15}, {5, 12, 12}, {15, 20, 10}, {19, 24, 8},
    };
    const std::vector<std::vector<int>> expected = {
        {2, 10}, {3, 15}, {7, 12}, {12, 0}, {15, 10}, {20, 8}, {24, 0},
    };
    ctx.expect_eq(sol.getSkyline(buildings), expected);
  });

  ctx.test("example_2_adjacent_same_height_merged", [&] {
    Solution sol;
    const std::vector<std::vector<int>> buildings = {{0, 2, 3}, {2, 5, 3}};
    const std::vector<std::vector<int>> expected = {{0, 3}, {5, 0}};
    ctx.expect_eq(sol.getSkyline(buildings), expected);
  });

  ctx.test("touching_step_down_and_step_up", [&] {
    Solution sol;
    const std::vector<std::vector<int>> step_down = {{0, 2, 10}, {2, 5, 5}};
    ctx.expect_eq(sol.getSkyline(step_down),
                  std::vector<std::vector<int>>({{0, 10}, {2, 5}, {5, 0}}));

    const std::vector<std::vector<int>> step_up = {{0, 2, 5}, {2, 5, 10}};
    ctx.expect_eq(sol.getSkyline(step_up),
                  std::vector<std::vector<int>>({{0, 5}, {2, 10}, {5, 0}}));
  });

  ctx.test("same_start_and_contained_buildings", [&] {
    Solution sol;
    const std::vector<std::vector<int>> buildings = {
        {1, 5, 6}, {1, 5, 10}, {2, 4, 8},
    };
    const std::vector<std::vector<int>> expected = {{1, 10}, {5, 0}};
    ctx.expect_eq(sol.getSkyline(buildings), expected);
  });

  ctx.test("empty_and_int_max_right_edge", [&] {
    Solution sol;
    ctx.expect_eq(sol.getSkyline({}), std::vector<std::vector<int>>{});

    const int max_x = std::numeric_limits<int>::max();
    const std::vector<std::vector<int>> buildings = {{0, max_x, 5}};
    const std::vector<std::vector<int>> expected = {{0, 5}, {max_x, 0}};
    ctx.expect_eq(sol.getSkyline(buildings), expected);
  });
}

}  // namespace
