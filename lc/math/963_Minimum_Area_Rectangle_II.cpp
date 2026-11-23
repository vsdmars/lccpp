// LeetCode 963: Minimum Area Rectangle II
// https://leetcode.com/problems/minimum-area-rectangle-ii/
//
// Problem:
//   Given an array of unique 2D integer points `points` where
//   `points[i] = [x_i, y_i]`, find the minimum area of any rectangle formed by
//   choosing four of these points as vertices. Unlike axis-aligned rectangle
//   problems, the sides of the rectangle may be rotated at any angle relative
//   to the X and Y axes. Return 0.0 if no four points form a rectangle.
//   Answers within 1e-5 of the exact value are accepted.
//   Constraints: 1 <= points.length <= 50, 0 <= x_i, y_i <= 40000, all points
//   are distinct.
//
// Algorithm:
//   Store every input point in a hash set for O(1) average-time membership
//   queries. Enumerate all unordered triples of indices (i, j, k) with
//   i < j < k and treat P_i as the candidate right-angle corner with adjacent
//   edges v1 = P_j - P_i and v2 = P_k - P_i.
//   Key invariant: every valid rectangle has a unique vertex with the smallest
//   index among its four corners; its two adjacent corners necessarily have
//   strictly larger indices, so enumerating only i < j < k visits every
//   rectangle at least once (in fact, exactly once via its minimum-index
//   corner) without missing any orientation.
//   Two edge vectors v1 and v2 meet at a 90-degree angle iff their dot product
//   (dx1 * dx2) + (dy1 * dy2) == 0. When orthogonal, the fourth corner that
//   completes the parallelogram (and hence rectangle) is uniquely determined as
//   P_4 = P_i + v1 + v2 = (x_j + x_k - x_i, y_j + y_k - y_i). If P_4 exists in
//   the point set, compute the area |v1| * |v2| in double precision and update
//   the running minimum. All intermediate differences and dot products use
//   64-bit integers to avoid 32-bit signed overflow.
//   Time Complexity:  O(N^3) average over N points (N <= 50 => at most ~20k
//                     triples).
//   Space Complexity: O(N) for the point hash set.
//
// Data Structures:
//   - std::unordered_set<Point, PointHash>: stores all input 2D integer points
//     to verify in O(1) expected time whether the implied fourth corner exists.
//   - Point / PointHash: lightweight 64-bit coordinate struct with custom hash
//     combining x and y so negative or out-of-range inferred fourth corners are
//     handled safely without coordinate-packing collisions.
//
// Real-World Applications:
//   - Computer vision & OCR: detecting oriented bounding boxes or fiducial
//     markers (e.g., calibration checkerboards, QR corners) from detected
//     2D keypoints under arbitrary camera rotation.
//   - Computational geometry & CAD: identifying rectangular primitives and
//     orthogonal pad footprints in PCB layout / LiDAR point-cloud extraction.
//   - Geospatial analysis: finding minimum-footprint rectangular building plots
//     from surveyed boundary vertices.
//
// Related LeetCode Problems:
//   - #939 Minimum Area Rectangle [done] (axis-aligned version solvable in O(N^2) via diagonal enumeration)
//   - #593 Valid Square [done] (verifying four 2D points form a square via pairwise squared distances)
//   - #149 Max Points on a Line [done] (collinearity and vector-slope hashing over 2D integer points)
//   - #223 Rectangle Area [done] (2D rectangle geometry and area computation)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <unordered_set>
#include <vector>

namespace {

// 2D lattice point stored in 64-bit coordinates so inferred fourth corners
// (which may fall outside the input bounds or become negative) never overflow.
struct Point {
  std::int64_t x = 0;
  std::int64_t y = 0;

  bool operator==(const Point&) const = default;
};

// Fast hash functor for Point combining both 64-bit coordinates.
struct PointHash {
  [[nodiscard]] std::size_t operator()(const Point& p) const noexcept {
    // Mix x and y with golden-ratio constant to avoid clustering on grid lines.
    const auto hx = static_cast<std::uint64_t>(p.x);
    const auto hy = static_cast<std::uint64_t>(p.y);
    constexpr std::uint64_t kMix = 0x9e3779b97f4a7c15ULL;
    const std::uint64_t combined = hx ^ ((hy + kMix) + (hx << 6U) + (hx >> 2U));
    return static_cast<std::size_t>(combined);
  }
};

class Solution {
 public:
  // Finds the minimum area of any (possibly rotated) rectangle whose four
  // corners belong to `points`, or 0.0 if no such rectangle exists.
  // Time: O(N^3) average, Space: O(N).
  [[nodiscard]] double minAreaFreeRect(
      const std::vector<std::vector<int>>& points) const {
    const std::size_t n = points.size();
    // Fewer than four points cannot form a rectangle.
    if (n < 4uz) {
      return 0.0;
    }

    // Index all input points for O(1) expected lookup of the fourth vertex.
    std::unordered_set<Point, PointHash> point_set;
    point_set.reserve(n);
    for (const auto& pt : points) {
      point_set.insert(Point{.x = static_cast<std::int64_t>(pt[0]),
                             .y = static_cast<std::int64_t>(pt[1])});
    }

    double min_area = std::numeric_limits<double>::infinity();

    // Enumerate all triples (i < j < k) with points[i] as the right-angle
    // corner. Every rectangle is discovered via its minimum-index vertex.
    for (std::size_t i = 0uz; i < n; ++i) {
      const auto x1 = static_cast<std::int64_t>(points[i][0]);
      const auto y1 = static_cast<std::int64_t>(points[i][1]);

      for (std::size_t j = i + 1uz; j < n; ++j) {
        const std::int64_t dx1 = static_cast<std::int64_t>(points[j][0]) - x1;
        const std::int64_t dy1 = static_cast<std::int64_t>(points[j][1]) - y1;
        const std::int64_t len1_sq = (dx1 * dx1) + (dy1 * dy1);

        for (std::size_t k = j + 1uz; k < n; ++k) {
          const std::int64_t dx2 = static_cast<std::int64_t>(points[k][0]) - x1;
          const std::int64_t dy2 = static_cast<std::int64_t>(points[k][1]) - y1;

          // Edge vectors v1 and v2 are perpendicular iff their dot product is 0.
          if (((dx1 * dx2) + (dy1 * dy2)) != 0) {
            continue;
          }

          // The fourth corner completing the rectangle is P_i + v1 + v2.
          const Point fourth{.x = (x1 + dx1) + dx2, .y = (y1 + dy1) + dy2};
          if (!point_set.contains(fourth)) {
            continue;
          }

          const std::int64_t len2_sq = (dx2 * dx2) + (dy2 * dy2);
          const double area = std::sqrt(static_cast<double>(len1_sq)) *
                              std::sqrt(static_cast<double>(len2_sq));
          min_area = std::min(min_area, area);
        }
      }
    }

    return std::isfinite(min_area) ? min_area : 0.0;
  }
};

[[nodiscard]] bool approx_equal(double actual, double expected) noexcept {
  constexpr double kEps = 1e-5;
  return std::fabs(actual - expected) <= kEps;
}

LC_PROBLEM(963, "Minimum Area Rectangle II", "math", ctx) {
  const Solution sol;

  ctx.test("example_1_rotated_square", [&] {
    const std::vector<std::vector<int>> pts = {{1, 2}, {2, 1}, {1, 0}, {0, 1}};
    ctx.expect_true(approx_equal(sol.minAreaFreeRect(pts), 2.0));
  });

  ctx.test("example_2_unit_square", [&] {
    const std::vector<std::vector<int>> pts = {
        {0, 1}, {2, 1}, {1, 1}, {1, 0}, {2, 0}};
    ctx.expect_true(approx_equal(sol.minAreaFreeRect(pts), 1.0));
  });

  ctx.test("example_3_no_rectangle", [&] {
    const std::vector<std::vector<int>> pts = {
        {0, 3}, {1, 2}, {3, 1}, {1, 3}, {2, 1}};
    ctx.expect_true(approx_equal(sol.minAreaFreeRect(pts), 0.0));
  });

  ctx.test("example_4_from_python_build", [&] {
    const std::vector<std::vector<int>> pts = {{3, 1}, {1, 1}, {0, 1}, {2, 1},
                                               {3, 3}, {3, 2}, {0, 2}, {2, 3}};
    ctx.expect_true(approx_equal(sol.minAreaFreeRect(pts), 2.0));
  });

  ctx.test("tilted_non_square_rectangle", [&] {
    // Sides (2, 1) of length sqrt(5) and (-2, 4) of length 2*sqrt(5) => area 10.
    const std::vector<std::vector<int>> pts = {
        {0, 0}, {2, 1}, {0, 5}, {-2, 4}};
    ctx.expect_true(approx_equal(sol.minAreaFreeRect(pts), 10.0));
  });

  ctx.test("fewer_than_four_points", [&] {
    const std::vector<std::vector<int>> pts = {{0, 0}, {1, 0}, {0, 1}};
    ctx.expect_true(approx_equal(sol.minAreaFreeRect(pts), 0.0));
  });
}

}  // namespace
