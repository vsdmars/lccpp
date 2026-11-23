// LeetCode 1515: Best Position For A Service Centre
// https://leetcode.com/problems/best-position-for-a-service-centre/
//
// Problem:
//   Given an array `positions` where `positions[i] = [xi, yi]` gives the 2D
//   coordinates of the i-th customer, choose a service center location
//   `(xc, yc)` in R^2 that minimizes the sum of Euclidean distances to all
//   customers:
//     f(x, y) = sum_i sqrt((x - xi)^2 + (y - yi)^2).
//   Return that minimum total distance (answers within 1e-5 are accepted).
//   Constraints: 1 <= positions.length <= 50, 0 <= xi, yi <= 100.
//
// Algorithm:
//   Finding the geometric median (Fermat-Weber point) minimizes a convex
//   objective function `f(x, y)` (note: the arithmetic mean minimizes squared
//   Euclidean distance, not raw Euclidean distance, so it serves only as a
//   strong initial guess). Starting from the centroid of all points with an
//   initial step size `step = 100.0`, we perform compass/pattern search along
//   the four cardinal directions. Whenever a neighbor achieves a strictly
//   smaller total distance, we move there and keep the current step size; once
//   no cardinal neighbor improves `f`, we halve `step` ("zoom in"). Because
//   `f` is convex, halving until `step <= 1e-7` converges tightly to the
//   global minimum well within the 1e-5 tolerance.
//   Time Complexity:  O(n * log(R / eps)), where n = positions.size(),
//                     R = 100 is the coordinate span, and eps = 1e-7.
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - Fixed-size `std::array<std::pair<double, double>, 4>`: stores the four
//     unit cardinal search directions.
//   - Scalar floating-point state (`x`, `y`, `best_dist`, `step`) tracking the
//     current candidate center and search radius.
//
// Real-World Applications:
//   - Facility location and warehouse / distribution hub placement to minimize
//     total delivery travel distance.
//   - Placing wireless access points, cellular base stations, or edge servers
//     to minimize aggregate latency/distance.
//   - Robust 2D spatial statistics (the geometric median is far less sensitive
//     to spatial outliers than the arithmetic centroid).
//
// Related LeetCode Problems:
//   - #296 Best Meeting Point [done] (1D/Manhattan-distance version solved exactly by medians)
//   - #462 Minimum Moves to Equal Array Elements II [new] (1D L1 distance minimization via median)
//   - #2033 Minimum Operations to Make a Uni Value Grid [done] (1D median minimization with step constraint)
//   - #973 K Closest Points to Origin [done] (Euclidean distance computation over 2D coordinates)

#include "framework/lc_framework.h"

#include <array>
#include <cmath>
#include <numbers>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the minimum possible sum of Euclidean distances from a chosen 2D
  // service center to all customer coordinates using shrinking-step compass
  // search on the convex Fermat-Weber objective.
  // Time: O(n * log(R / eps)), Space: O(1).
  double getMinDistSum(const std::vector<std::vector<int>>& positions) {
    if (positions.size() <= 1uz) {
      return 0.0;
    }

    // Initialize at the arithmetic centroid, which lies inside the convex hull
    // of the customer positions and gives a good starting point.
    double x = 0.0;
    double y = 0.0;
    for (const auto& pt : positions) {
      x += static_cast<double>(pt[0]);
      y += static_cast<double>(pt[1]);
    }
    const auto count = static_cast<double>(positions.size());
    x /= count;
    y /= count;

    double best_dist = total_distance(positions, x, y);
    double step = kInitialStep;

    // Shrink the search radius whenever no cardinal neighbor improves the
    // objective; stop once step <= kPrecision so total objective error is
    // well below the 1e-5 tolerance even with 50 points.
    while (step > kPrecision) {
      bool improved = false;

      for (const auto& [dx, dy] : kDirs) {
        const double nx = x + (dx * step);
        const double ny = y + (dy * step);
        const double dist = total_distance(positions, nx, ny);
        if (dist < best_dist) {
          best_dist = dist;
          x = nx;
          y = ny;
          improved = true;
        }
      }

      if (!improved) {
        step /= 2.0;
      }
    }

    return best_dist;
  }

 private:
  static constexpr double kInitialStep = 100.0;
  static constexpr double kPrecision = 1e-7;
  static constexpr std::array<std::pair<double, double>, 4> kDirs = {{
      {0.0, 1.0},
      {0.0, -1.0},
      {1.0, 0.0},
      {-1.0, 0.0},
  }};

  // Evaluates the sum of Euclidean distances from candidate center (x, y) to
  // every customer point.
  static double total_distance(const std::vector<std::vector<int>>& positions,
                               double x, double y) {
    double sum = 0.0;
    for (const auto& pt : positions) {
      const double dx = x - static_cast<double>(pt[0]);
      const double dy = y - static_cast<double>(pt[1]);
      sum += std::sqrt((dx * dx) + (dy * dy));
    }
    return sum;
  }
};

bool approx_eq(double actual, double expected, double eps = 1e-5) {
  return std::fabs(actual - expected) <= eps;
}

LC_PROBLEM(1515, "Best Position For A Service Centre", "math", ctx) {
  ctx.test("example_1_diamond", [&] {
    Solution sol;
    const double ans = sol.getMinDistSum({{0, 1}, {1, 0}, {1, 2}, {2, 1}});
    ctx.expect_true(approx_eq(ans, 4.0));
  });

  ctx.test("example_2_build_two_points", [&] {
    Solution sol;
    const double ans = sol.getMinDistSum({{1, 1}, {3, 3}});
    ctx.expect_true(approx_eq(ans, 2.0 * std::numbers::sqrt2));
  });

  ctx.test("single_customer", [&] {
    Solution sol;
    const double ans = sol.getMinDistSum({{5, 7}});
    ctx.expect_true(approx_eq(ans, 0.0));
  });

  ctx.test("collinear_asymmetric", [&] {
    Solution sol;
    // For collinear points, the geometric median is the 1D median (1, 0),
    // giving total distance 1 + 0 + 9 = 10.
    const double ans = sol.getMinDistSum({{0, 0}, {1, 0}, {10, 0}});
    ctx.expect_true(approx_eq(ans, 10.0));
  });

  ctx.test("square_corners", [&] {
    Solution sol;
    // Optimal center is (1, 1), distance sqrt(2) to each of the 4 corners.
    const double ans = sol.getMinDistSum({{0, 0}, {0, 2}, {2, 0}, {2, 2}});
    ctx.expect_true(approx_eq(ans, 4.0 * std::numbers::sqrt2));
  });

  ctx.test("obtuse_triangle_torricelli_vertex", [&] {
    Solution sol;
    // Angle at (2, 1) exceeds 120 degrees, so by Torricelli's theorem the
    // geometric median is the vertex (2, 1) itself, giving sum = 2 * sqrt(5).
    const double ans = sol.getMinDistSum({{0, 0}, {2, 1}, {4, 0}});
    ctx.expect_true(approx_eq(ans, 2.0 * std::sqrt(5.0)));
  });
}

}  // namespace
