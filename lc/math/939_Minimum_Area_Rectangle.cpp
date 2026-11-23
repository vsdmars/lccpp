// LeetCode 939: Minimum Area Rectangle
// https://leetcode.com/problems/minimum-area-rectangle/
//
// Problem:
//   Given an array `points` of distinct 2D integer points (1 <= points.size()
//   <= 500, 0 <= x, y <= 40000), return the minimum area of a rectangle formed
//   from any four of these points with sides parallel to the x- and y-axes.
//   If no such rectangle exists, return 0.
//
// Examples:
//   Example 1:
//     Input:  points = [[1, 1], [1, 3], [3, 1], [3, 3], [2, 2]]
//     Output: 4
//     Explanation:
//       - Process indices 0..2: (1, 1), (1, 3), and (3, 1) share coordinates or
//         lack both off-diagonal corners in visited, so visited holds those three
//         points and min_area stays at max.
//       - At i = 3, pair points[3] = (3, 3) with j = 0, points[0] = (1, 1) as an
//         opposite diagonal since 3 != 1 in both x and y.
//       - Confirm both off-diagonal corners (3, 1) and (1, 3) exist in visited,
//         updating min_area to |3 - 1| * |3 - 1| = 4.
//       - At i = 4, (2, 2) forms diagonal pairs with earlier points, but none have
//         both matching off-diagonal corners in visited, so return 4.
//
//   Example 2:
//     Input:  points = [[1, 1], [1, 3], [3, 1], [3, 3], [4, 1], [4, 3]]
//     Output: 2
//     Explanation:
//       - Through i = 3, (1, 1) and (3, 3) establish min_area = 4 using corners (1,
//         3) and (3, 1) in visited.
//       - At i = 4, (4, 1) is added to visited without completing any new
//         axis-aligned rectangle.
//       - At i = 5, pair points[5] = (4, 3) with j = 2, points[2] = (3, 1), and
//         verify that both (4, 1) and (3, 3) are present in visited.
//       - Compute area |4 - 3| * |3 - 1| = 1 * 2 = 2, tightening min_area from 4 to
//         2 (smaller than the 3 * 2 = 6 rectangle with (1, 1)).
//
//   Example 3:
//     Input:  points = [[0, 1], [1, 0], [1, 2], [2, 1]]
//     Output: 0
//     Explanation:
//       - Iterate over all pairs (points[i], points[j]) with differing x and y
//         coordinates, such as (1, 0) with (0, 1) or (2, 1) with (1, 2).
//       - For every candidate diagonal, at least one required axis-aligned corner
//         (e.g., (0, 0), (1, 1), or (2, 2)) is missing from visited.
//       - Finish all pairs with min_area unchanged at INT64_MAX, so return 0
//         because only a rotated diamond is present.
//
// Algorithm:
//   1. Diagonal-Pair Hash Lookup (`minAreaRect`):
//      Iterate through `points[i]` while maintaining a hash set `visited` of
//      all previously processed points `points[0..i-1]`. For every earlier
//      point `points[j]` not sharing an x- or y-coordinate with `points[i]`,
//      treat `(points[i], points[j])` as a candidate diagonal and check whether
//      both off-diagonal corners `(x1, y2)` and `(x2, y1)` are already in
//      `visited`. Every valid rectangle is discovered exactly once when its
//      highest-index vertex is processed.
//      Time: O(N^2) average, Space: O(N).
//   2. Sorted Axis Sweep (`minAreaRectSweep`):
//      Group points by whichever axis has more distinct coordinates (shrinking
//      per-bucket pair counts) into a sorted `std::map`. Sweep columns in
//      increasing primary-coordinate order, sort each column's secondary
//      coordinates, and record the most recent primary coordinate seen for each
//      secondary pair `(y1, y2)` in a hash map. Because columns are visited in
//      sorted order, only the immediately previous column containing `(y1, y2)`
//      can produce the minimal width for that vertical edge.
//      Time: O(N^2) worst-case (O(N^1.5) when coordinates are balanced),
//      Space: O(N + P) where P is the number of distinct vertical segments.
//
// Data Structures:
//   - `std::unordered_set<std::uint64_t>` with 64-bit bit-packed `(x, y)` keys
//     for O(1) expected corner lookup.
//   - `std::map<int, std::vector<int>>` and `std::unordered_map<std::uint64_t, int>`
//     for the sorted column-sweep approach.
//
// Real-World Applications:
//   - Detecting axis-aligned bounding regions and minimal enclosing tiles in
//     VLSI / PCB layout verification and CAD geometry engines.
//   - Grid-aligned feature extraction and corner-pair matching in computer
//     vision and optical document layout analysis.
//
// Related LeetCode Problems:
//   - #963 Minimum Area Rectangle II [done] (generalization allowing rotated rectangles)
//   - #836 Rectangle Overlap [done] (axis-aligned rectangle intersection test)
//   - #223 Rectangle Area [done] (axis-aligned rectangle union area computation)
//   - #2013 Detect Squares [new] (streaming axis-aligned square counting via coordinate maps)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the minimum area of an axis-aligned rectangle formed by 4 points
  // using diagonal-pair enumeration with an O(1)-lookup hash set.
  //
  // Invariant: When processing `points[i]`, `visited` contains exactly
  // `points[0..i-1]`, so any rectangle whose largest-index vertex is `i` has
  // its opposite corner and both adjacent corners already present in `visited`.
  //
  // Time: O(N^2) average, Space: O(N).
  int minAreaRect(const std::vector<std::vector<int>>& points) {
    const std::size_t n = points.size();
    if (n < 4uz) {
      return 0;
    }

    std::unordered_set<std::uint64_t> visited;
    visited.reserve(n);

    std::int64_t min_area = std::numeric_limits<std::int64_t>::max();

    for (std::size_t i = 0uz; i < n; ++i) {
      const int x1 = points[i][0];
      const int y1 = points[i][1];

      // Pair points[i] with each prior point points[j] as an opposite diagonal.
      for (std::size_t j = 0uz; j < i; ++j) {
        const int x2 = points[j][0];
        const int y2 = points[j][1];

        // Opposite corners of a non-degenerate axis-aligned rectangle must
        // differ in both x and y.
        if (x1 == x2 || y1 == y2) {
          continue;
        }

        // Check whether the other two corners completing the rectangle exist.
        if (visited.contains(packPair(x1, y2)) &&
            visited.contains(packPair(x2, y1))) {
          const auto width = static_cast<std::int64_t>(std::abs(x1 - x2));
          const auto height = static_cast<std::int64_t>(std::abs(y1 - y2));
          min_area = std::min(min_area, width * height);
        }
      }

      visited.insert(packPair(x1, y1));
    }

    return min_area == std::numeric_limits<std::int64_t>::max()
               ? 0
               : static_cast<int>(min_area);
  }

  // Secondary sweep-line solution matching the Python `faster()` approach:
  // groups points along the axis with more distinct values and sweeps sorted
  // columns, tracking the most recent primary coordinate for each pair of
  // secondary coordinates.
  //
  // Time: O(N^2) worst-case (faster in practice / O(N^1.5) when balanced),
  // Space: O(N + distinct vertical segments).
  int minAreaRectSweep(const std::vector<std::vector<int>>& points) {
    const std::size_t n = points.size();
    if (n < 4uz) {
      return 0;
    }

    // Count distinct x- and y-coordinates to detect impossible inputs early
    // and choose the finer partition axis.
    std::unordered_set<int> xs;
    std::unordered_set<int> ys;
    xs.reserve(n);
    ys.reserve(n);
    for (const auto& pt : points) {
      xs.insert(pt[0]);
      ys.insert(pt[1]);
    }
    if (xs.size() == n || ys.size() == n) {
      return 0;
    }

    const bool group_by_x = xs.size() > ys.size();
    std::map<int, std::vector<int>> columns;
    for (const auto& pt : points) {
      if (group_by_x) {
        columns[pt[0]].push_back(pt[1]);
      } else {
        columns[pt[1]].push_back(pt[0]);
      }
    }

    // Map each sorted secondary pair (s1, s2) to the latest primary coordinate.
    std::unordered_map<std::uint64_t, int> last_primary;
    std::int64_t min_area = std::numeric_limits<std::int64_t>::max();

    for (auto& [primary, secondaries] : columns) {
      if (secondaries.size() < 2uz) {
        continue;
      }
      std::ranges::sort(secondaries);
      min_area = std::min(
          min_area, scanColumnPairs(primary, secondaries, last_primary));
    }

    return min_area == std::numeric_limits<std::int64_t>::max()
               ? 0
               : static_cast<int>(min_area);
  }

 private:
  // Bit-packs two signed 32-bit integers into a collision-free 64-bit key.
  static std::uint64_t packPair(int a, int b) {
    const auto hi = static_cast<std::uint64_t>(static_cast<std::uint32_t>(a));
    const auto lo = static_cast<std::uint64_t>(static_cast<std::uint32_t>(b));
    return (hi << 32U) | lo;
  }

  // Evaluates all pairs (s1 < s2) within one sorted column, updating
  // `last_primary` and returning the smallest rectangle area closed at this
  // column (or `max()` if none).
  static std::int64_t scanColumnPairs(
      int primary, const std::vector<int>& secondaries,
      std::unordered_map<std::uint64_t, int>& last_primary) {
    std::int64_t best = std::numeric_limits<std::int64_t>::max();
    const std::size_t m = secondaries.size();
    for (std::size_t i = 0uz; i < m; ++i) {
      for (std::size_t j = 0uz; j < i; ++j) {
        const int s1 = secondaries[j];
        const int s2 = secondaries[i];
        if (s1 == s2) {
          continue;
        }
        const std::uint64_t key = packPair(s1, s2);
        if (const auto it = last_primary.find(key); it != last_primary.end()) {
          const auto width = static_cast<std::int64_t>(primary - it->second);
          const auto height = static_cast<std::int64_t>(s2 - s1);
          best = std::min(best, width * height);
        }
        last_primary[key] = primary;
      }
    }
    return best;
  }
};

LC_PROBLEM(939, "Minimum Area Rectangle", "math", ctx) {
  Solution sol;

  ctx.test("example_1_single_rectangle_with_center_point", [&] {
    const std::vector<std::vector<int>> pts = {
        {1, 1}, {1, 3}, {3, 1}, {3, 3}, {2, 2}};
    ctx.expect_eq(sol.minAreaRect(pts), 4);
    ctx.expect_eq(sol.minAreaRectSweep(pts), 4);
  });

  ctx.test("example_2_multiple_rectangles", [&] {
    const std::vector<std::vector<int>> pts = {
        {1, 1}, {1, 3}, {3, 1}, {3, 3}, {4, 1}, {4, 3}};
    ctx.expect_eq(sol.minAreaRect(pts), 2);
    ctx.expect_eq(sol.minAreaRectSweep(pts), 2);
  });

  ctx.test("no_rectangle_possible", [&] {
    const std::vector<std::vector<int>> too_few = {{1, 1}, {1, 3}};
    ctx.expect_eq(sol.minAreaRect(too_few), 0);
    ctx.expect_eq(sol.minAreaRectSweep(too_few), 0);

    // Diamond / rotated square is not axis-aligned.
    const std::vector<std::vector<int>> rotated = {
        {0, 1}, {1, 0}, {1, 2}, {2, 1}};
    ctx.expect_eq(sol.minAreaRect(rotated), 0);
    ctx.expect_eq(sol.minAreaRectSweep(rotated), 0);
  });

  ctx.test("tall_vs_wide_axis_grouping", [&] {
    // More distinct y-values than x-values exercises the group_by_y branch.
    const std::vector<std::vector<int>> tall = {
        {0, 0}, {0, 2}, {0, 5}, {0, 6}, {3, 0}, {3, 5}, {3, 6}};
    ctx.expect_eq(sol.minAreaRect(tall), 3);
    ctx.expect_eq(sol.minAreaRectSweep(tall), 3);

    // More distinct x-values than y-values exercises the group_by_x branch.
    const std::vector<std::vector<int>> wide = {
        {0, 0}, {2, 0}, {5, 0}, {6, 0}, {0, 3}, {5, 3}, {6, 3}};
    ctx.expect_eq(sol.minAreaRect(wide), 3);
    ctx.expect_eq(sol.minAreaRectSweep(wide), 3);
  });

  ctx.test("max_coordinate_area", [&] {
    const std::vector<std::vector<int>> pts = {
        {0, 0}, {0, 40000}, {40000, 0}, {40000, 40000}};
    ctx.expect_eq(sol.minAreaRect(pts), 1'600'000'000);
    ctx.expect_eq(sol.minAreaRectSweep(pts), 1'600'000'000);
  });
}

}  // namespace
