// LeetCode 447: Number Of Boomerangs
// https://leetcode.com/problems/number-of-boomerangs/
//
// Problem:
//   Given n pairwise-distinct points on a 2D plane, a "boomerang" is an
//   ordered tuple of three points (i, j, k) such that the Euclidean distance
//   between i and j equals the Euclidean distance between i and k. Return the
//   total number of boomerangs.
//
// Examples:
//   Example 1:
//     Input:  points = [[0, 0], [1, 0], [2, 0]]
//     Output: 2
//     Explanation:
//       - Initialize unordered_pairs = 0 and iterate each point i as the pivot
//         using 64-bit squared Euclidean distance dist_sq = dx*dx + dy*dy.
//       - For pivot i = 0 at (0, 0), neighbors (1, 0) and (2, 0) give dist_sq
//         values 1 and 4, which are distinct so dist_counts[1] = 1, dist_counts[4]
//         = 1, and unordered_pairs stays 0.
//       - For pivot i = 1 at (1, 0), neighbor (0, 0) sets dist_counts[1] = 1
//         (adding 0), and neighbor (2, 0) also has dist_sq = 1 so it adds
//         dist_counts[1] = 1 to unordered_pairs and increments dist_counts[1] to 2.
//       - For pivot i = 2 at (2, 0), neighbors (0, 0) and (1, 0) give dist_sq
//         values 4 and 1, contributing 0 more pairs.
//       - Multiply unordered_pairs * 2 = 1 * 2 = 2 to count both ordered boomerangs
//         ((1,0),(0,0),(2,0)) and ((1,0),(2,0),(0,0)).
//
//   Example 2:
//     Input:  points = [[0, 0], [1, 0], [2, 0], [1, 1]]
//     Output: 8
//     Explanation:
//       - For pivot i = 0 at (0, 0), squared distances to (1, 0), (2, 0), and (1,
//         1) are 1, 4, and 2, contributing 0 pairs.
//       - For pivot i = 1 at (1, 0), all three other points lie at dist_sq = 1, so
//         dist_counts[1] progresses 0 -> 1 -> 2 -> 3 and adds 0 + 1 + 2 = 3 to
//         unordered_pairs.
//       - For pivot i = 2 at (2, 0), squared distances to (0, 0), (1, 0), and (1,
//         1) are 4, 1, and 2, contributing 0 pairs.
//       - For pivot i = 3 at (1, 1), squared distances to (0, 0), (1, 0), and (2,
//         0) are 2, 1, and 2, so the two points at dist_sq = 2 add 1 to
//         unordered_pairs (total 4).
//       - Return unordered_pairs * 2 = 4 * 2 = 8 ordered boomerangs.
//
//   Example 3:
//     Input:  points = [[0, 0], [1, 0]]
//     Output: 0
//     Explanation:
//       - Check n = points.size(), which equals 2.
//       - Since n < 3, fewer than three distinct points are available to form a
//         tuple (i, j, k), so the early return immediately returns 0.
//
// Algorithm:
//   Fix each point `i` as the boomerang's vertex (pivot) and group every other
//   point `j != i` by its squared Euclidean distance from `i`:
//     dist_sq = (x_i - x_j)^2 + (y_i - y_j)^2
//   Using squared integer distance in 64-bit arithmetic avoids both `std::sqrt`
//   cost and floating-point precision errors. If `m` distinct points lie at the
//   same distance `d` from pivot `i`, any ordered pair `(j, k)` among them
//   forms a boomerang, giving `P(m, 2) = m * (m - 1)` boomerangs. We compute
//   this incrementally in a single pass per pivot: when encountering the
//   `(c + 1)`-th point at distance `d` (where `c` were already seen), it forms
//   `c` new unordered pairs with the previous `c` points; summing `c` yields
//   `m * (m - 1) / 2`, and multiplying the total by 2 gives the exact number
//   of ordered boomerangs.
//   Time Complexity: O(n^2) average.
//   Space Complexity: O(n) for the per-pivot distance frequency hash map.
//
// Data Structures:
//   - std::unordered_map<std::int64_t, int>: maps each squared distance from
//     the current pivot point to the count of neighbor points at that distance;
//     reused and cleared across pivots to avoid repeated heap allocations.
//
// Real-World Applications:
//   - Computational geometry and 2D/3D point-cloud pattern matching (detecting
//     isosceles triangles and equidistant constellations in LIDAR or astronomy).
//   - Wireless sensor network localization and triangulation checking
//     equidistant anchor pairs from a reference node.
//   - Molecular structure analysis counting symmetric bond-length pairs
//     around a central atom.
//
// Related LeetCode Problems:
//   - #356 Line Reflection [done] (hash-based geometric symmetry on 2D integer points)
//   - #149 Max Points on a Line [done] (per-pivot geometric hashing by slope)
//   - #593 Valid Square [done] (exact squared Euclidean distance comparisons on 2D points)
//   - #1512 Number of Good Pairs [done] (incremental pair counting via frequency hash map)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Counts all ordered tuples (i, j, k) of distinct points where
  // dist(i, j) == dist(i, k).
  //
  // Invariant: For a fixed pivot `i`, when `dist_counts[d]` holds `c` prior
  // points at squared distance `d`, the newly seen point forms `c` unordered
  // equidistant pairs with them (`2 * c` ordered boomerangs).
  //
  // Time Complexity: O(n^2) average.
  // Space Complexity: O(n).
  int numberOfBoomerangs(const std::vector<std::vector<int>>& points) {
    const std::size_t n = points.size();
    if (n < 3) {
      return 0;
    }

    // Reuse a single hash map across pivots to minimize heap allocations.
    std::unordered_map<std::int64_t, int> dist_counts;
    dist_counts.reserve(n);

    int unordered_pairs = 0;

    for (std::size_t i = 0uz; i < n; ++i) {
      dist_counts.clear();
      const auto xi = static_cast<std::int64_t>(points[i][0]);
      const auto yi = static_cast<std::int64_t>(points[i][1]);

      for (std::size_t j = 0uz; j < n; ++j) {
        if (i == j) {
          continue;
        }
        const std::int64_t dx = xi - static_cast<std::int64_t>(points[j][0]);
        const std::int64_t dy = yi - static_cast<std::int64_t>(points[j][1]);
        const std::int64_t dist_sq = (dx * dx) + (dy * dy);

        // Each previously seen point at this distance pairs with point j.
        unordered_pairs += dist_counts[dist_sq]++;
      }
    }

    // Multiply by 2 because (i, j, k) and (i, k, j) are distinct boomerangs.
    return unordered_pairs * 2;
  }
};

LC_PROBLEM(447, "Number Of Boomerangs", "hash", ctx) {
  Solution sol;

  ctx.test("example_1_collinear_three", [&] {
    ctx.expect_eq(sol.numberOfBoomerangs({{0, 0}, {1, 0}, {2, 0}}), 2);
  });

  ctx.test("example_2_diagonal_three", [&] {
    ctx.expect_eq(sol.numberOfBoomerangs({{1, 1}, {2, 2}, {3, 3}}), 2);
  });

  ctx.test("example_3_single_and_two_points", [&] {
    ctx.expect_eq(sol.numberOfBoomerangs({{1, 1}}), 0);
    ctx.expect_eq(sol.numberOfBoomerangs({{0, 0}, {1, 0}}), 0);
  });

  ctx.test("t_shape_four_points_from_build_input", [&] {
    // Pivot (1,0) has 3 points at dist^2 = 1 -> 3*2 = 6;
    // Pivot (1,1) has 2 points at dist^2 = 2 -> 2*1 = 2; total = 8.
    ctx.expect_eq(sol.numberOfBoomerangs({{0, 0}, {1, 0}, {2, 0}, {1, 1}}), 8);
  });

  ctx.test("unit_square_four_corners", [&] {
    // Each of the 4 corners has 2 adjacent corners at dist^2 = 1 -> 4 * 2 = 8.
    ctx.expect_eq(sol.numberOfBoomerangs({{0, 0}, {0, 1}, {1, 0}, {1, 1}}), 8);
  });

  ctx.test("negative_coordinates_large_span", [&] {
    ctx.expect_eq(
        sol.numberOfBoomerangs({{-10000, -10000}, {0, 0}, {10000, 10000}}), 2);
  });
}

}  // namespace
