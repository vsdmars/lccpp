// LeetCode 296: Best Meeting Point
// https://leetcode.com/problems/best-meeting-point/
//
// Problem:
//   Given an m x n binary grid (1 <= m, n <= 200) where each cell equal to 1
//   marks the home of a friend (at least two friends exist), find a meeting
//   point in the grid that minimizes the total Manhattan travel distance
//   sum(|x_i - x| + |y_i - y|) across all friends, and return that minimal
//   total distance.
//
// Algorithm:
//   Manhattan distance separates cleanly into two independent 1D problems:
//     min_{x, y} sum(|x_i - x| + |y_i - y|)
//       = min_x sum(|x_i - x|) + min_y sum(|y_i - y|).
//   In 1D, the sum of absolute deviations (L1 norm) is minimized at the median
//   of the coordinates: shifting a point away from the median moves farther
//   from at least as many friends as it moves closer to.
//   To avoid an O(k log k) sort, we collect row indices in row-major order and
//   column indices in column-major order so both coordinate arrays are already
//   sorted in non-decreasing order. We then pick each array's median and sum
//   its absolute deviations.
//   Time Complexity: O(m * n) to scan the grid twice and sum distances.
//   Space Complexity: O(k) <= O(m * n), where k is the number of friends.
//
// Data Structures:
//   Two `std::vector<int>` arrays holding the already-sorted row coordinates
//   and column coordinates of all friend locations.
//
// Real-World Applications:
//   Facility and warehouse placement on rectilinear street grids, CDN edge PoP
//   selection under additive routing hops, and robust L1 point estimation
//   (geometric median along orthogonal axes) in spatial analytics.
//
// Related LeetCode Problems:
//   - #317 Shortest Distance from All Buildings [done] (2D grid meeting point with obstacles requiring BFS)
//   - #462 Minimum Moves to Equal Array Elements II [new] (exact 1D median L1 minimization)
//   - #1515 Best Position for a Service Centre [done] (2D Euclidean geometric median minimization)
//   - #2448 Minimum Cost to Make Array Equal [new] (weighted 1D median minimization)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdlib>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the minimal total Manhattan distance from all marked friends to an
  // optimal meeting point.
  // Time: O(m * n), Space: O(k) where k is the number of friends.
  int minTotalDistance(const std::vector<std::vector<int>>& grid) {
    if (grid.empty() || grid[0].empty()) {
      return 0;
    }

    const std::size_t num_rows = grid.size();
    const std::size_t num_cols = grid[0].size();

    // Collect row coordinates in row-major order so `rows` is naturally sorted.
    std::vector<int> rows;
    for (std::size_t r = 0uz; r < num_rows; ++r) {
      for (std::size_t c = 0uz; c < num_cols; ++c) {
        if (grid[r][c] != 0) {
          rows.push_back(static_cast<int>(r));
        }
      }
    }

    // Collect column coordinates in column-major order so `cols` is naturally
    // sorted without needing an O(k log k) sort.
    std::vector<int> cols;
    cols.reserve(rows.size());
    for (std::size_t c = 0uz; c < num_cols; ++c) {
      for (std::size_t r = 0uz; r < num_rows; ++r) {
        if (grid[r][c] != 0) {
          cols.push_back(static_cast<int>(c));
        }
      }
    }

    return min_distance_1d(rows) + min_distance_1d(cols);
  }

 private:
  // Computes the minimum total 1D L1 distance to any target point for a sorted
  // list of coordinates by choosing the median element.
  static int min_distance_1d(const std::vector<int>& sorted_coords) {
    if (sorted_coords.empty()) {
      return 0;
    }

    // For both odd and even counts, `sorted_coords[n / 2]` lies in the optimal
    // median interval minimizing sum(|coord - target|).
    const int median = sorted_coords[sorted_coords.size() / 2uz];
    int total = 0;
    for (const int coord : sorted_coords) {
      total += std::abs(coord - median);
    }
    return total;
  }
};

LC_PROBLEM(296, "Best Meeting Point", "math", ctx) {
  ctx.test("example_1_three_friends", [&] {
    Solution sol;
    const std::vector<std::vector<int>> grid = {
        {1, 0, 0, 0, 1},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
    };
    ctx.expect_eq(sol.minTotalDistance(grid), 6);
  });

  ctx.test("example_2_adjacent_pair", [&] {
    Solution sol;
    const std::vector<std::vector<int>> grid = {{1, 1}};
    ctx.expect_eq(sol.minTotalDistance(grid), 1);
  });

  ctx.test("single_column", [&] {
    Solution sol;
    const std::vector<std::vector<int>> grid = {{1}, {0}, {1}};
    ctx.expect_eq(sol.minTotalDistance(grid), 2);
  });

  ctx.test("four_corners_even_count", [&] {
    Solution sol;
    const std::vector<std::vector<int>> grid = {
        {1, 0, 1},
        {0, 0, 0},
        {1, 0, 1},
    };
    // Meeting at (1, 1) or any interior point yields distance 2 * 4 = 8.
    ctx.expect_eq(sol.minTotalDistance(grid), 8);
  });

  ctx.test("dense_2x2_grid", [&] {
    Solution sol;
    const std::vector<std::vector<int>> grid = {
        {1, 1},
        {1, 1},
    };
    ctx.expect_eq(sol.minTotalDistance(grid), 4);
  });
}

}  // namespace
