// LeetCode 554: Brick Wall
// https://leetcode.com/problems/brick-wall/
//
// Problem:
//   A rectangular brick wall consists of several rows of bricks having equal
//   height and positive integer widths, where every row sums to the same total
//   width. Draw a vertical line from the top of the wall to the bottom
//   (excluding the two outer vertical boundaries of the wall) that crosses the
//   minimum number of bricks. Passing exactly through a brick seam/edge does
//   not count as crossing a brick. Return the minimum number of crossed bricks.
//
// Algorithm:
//   Minimizing the number of crossed bricks is equivalent to maximizing the
//   number of interior brick seams aligned at the same horizontal coordinate:
//     crossed_bricks = total_rows - max_aligned_edges.
//   For every row, compute the prefix sum of brick widths for all bricks
//   EXCEPT the last brick (since the final prefix sum equals the right wall
//   edge, which is disallowed). Tally each interior seam x-coordinate in a
//   hash map and track the highest frequency `max_aligned_edges`.
//   Time Complexity:  O(B), where B is the total number of bricks in the wall.
//   Space Complexity: O(min(B, W)), where W is the total wall width, for
//                     storing distinct interior edge positions.
//
// Data Structures:
//   - std::unordered_map<std::int64_t, int> edge_counts: maps each interior
//     horizontal seam coordinate (stored as 64-bit integer to avoid overflow)
//     to the number of rows sharing a brick edge at that x-coordinate.
//
// Real-World Applications:
//   - Video/audio stream editing and timeline splicing (finding keyframe or
//     chunk boundaries that align across the most parallel media tracks).
//   - Structural engineering and PCB layer routing (finding vertical cut lines
//     or drill positions minimizing component intersections).
//   - Job scheduling and interval partitioning (identifying timestamps with
//     minimum active tasks to schedule maintenance windows).
//
// Related LeetCode Problems:
//   - #560 Subarray Sum Equals K [done] (prefix sums tallied in a hash map)
//   - #253 Meeting Rooms II [done] (counting aligned interval boundaries)
//   - #1094 Car Pooling [done] (sweep-line boundary accumulation)
//   - #2013 Detect Squares [new] (coordinate frequency counting via hash map)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the minimum number of bricks crossed by a vertical line passing
  // strictly between the left and right edges of `wall`.
  // Time: O(total bricks), Space: O(distinct interior seam coordinates).
  int leastBricks(const std::vector<std::vector<int>>& wall) {
    // Maps an interior horizontal seam position to the number of rows that
    // have a brick boundary at that exact position.
    std::unordered_map<std::int64_t, int> edge_counts;
    int max_aligned_edges = 0;

    for (const auto& row : wall) {
      std::int64_t prefix_width = 0;
      // Stop before the last brick: the right boundary of the wall is excluded.
      for (std::size_t i = 0uz; i + 1uz < row.size(); ++i) {
        prefix_width += row[i];
        const int count = ++edge_counts[prefix_width];
        max_aligned_edges = std::max(max_aligned_edges, count);
      }
    }

    // Any row without an edge at the optimal coordinate must be crossed.
    return static_cast<int>(wall.size()) - max_aligned_edges;
  }
};

LC_PROBLEM(554, "Brick Wall", "hash", ctx) {
  ctx.test("example_standard_wall", [&] {
    Solution sol;
    const std::vector<std::vector<int>> wall = {
        {1, 2, 2, 1},
        {3, 1, 2},
        {1, 3, 2},
        {2, 4},
        {3, 1, 2},
        {1, 3, 1, 1},
    };
    ctx.expect_eq(sol.leastBricks(wall), 2);
  });

  ctx.test("single_brick_per_row_must_cross_all", [&] {
    Solution sol;
    const std::vector<std::vector<int>> wall = {{1}, {1}, {1}};
    ctx.expect_eq(sol.leastBricks(wall), 3);
  });

  ctx.test("perfectly_aligned_seam_crosses_zero", [&] {
    Solution sol;
    const std::vector<std::vector<int>> wall = {
        {2, 3},
        {1, 1, 3},
        {2, 1, 2},
    };
    ctx.expect_eq(sol.leastBricks(wall), 0);
  });

  ctx.test("large_brick_widths_no_overflow", [&] {
    Solution sol;
    const std::vector<std::vector<int>> wall = {
        {1000000000, 1000000000},
        {1000000000, 1000000000},
    };
    ctx.expect_eq(sol.leastBricks(wall), 0);
  });

  ctx.test("empty_wall", [&] {
    Solution sol;
    const std::vector<std::vector<int>> wall;
    ctx.expect_eq(sol.leastBricks(wall), 0);
  });
}

}  // namespace
