// LeetCode 356: Line Reflection
// https://leetcode.com/problems/line-reflection/
//
// Problem:
//   Given n points on a 2D plane represented as integer coordinate pairs
//   [x, y], determine whether there exists a vertical line x = c (parallel to
//   the y-axis) such that reflecting the set of unique points across that line
//   maps the set onto itself. Note that duplicate points may appear in the
//   input and the reflection axis c may be a half-integer (e.g., 0.5).
//
// Algorithm:
//   If a vertical reflection line x = c exists, the leftmost point (min_x) and
//   rightmost point (max_x) must reflect into each other, which uniquely fixes
//   2c = min_x + max_x. In a first pass over all points, we track min_x and
//   max_x while inserting each point into a hash set packed into a lossless
//   64-bit key (upper 32 bits = x, lower 32 bits = y). In a second pass, for
//   every point (x, y) we verify that its reflected partner
//   (min_x + max_x - x, y) exists in the hash set. Using 2c = min_x + max_x
//   in 64-bit integer arithmetic completely avoids floating-point division and
//   signed 32-bit overflow.
//   Time Complexity: O(n) average.
//   Space Complexity: O(n) for the hash set of packed coordinates.
//
// Data Structures:
//   - std::unordered_set<std::uint64_t>: stores 64-bit bit-packed (x, y)
//     coordinates for O(1) average membership queries.
//
// Real-World Applications:
//   - Computer vision and CAD symmetry detection (finding bilateral axes of
//     reflection in 2D point clouds, meshes, and floor plans).
//   - Printed circuit board (PCB) and VLSI layout verification for symmetric
//     differential-pair routing.
//   - Molecular and crystallographic point-group symmetry checks.
//
// Related LeetCode Problems:
//   - #149 Max Points on a Line [done] (2D geometric hashing over integer coordinates)
//   - #447 Number of Boomerangs [done] (hash-based geometric pair counting on 2D points)
//   - #939 Minimum Area Rectangle [done] (packing 2D integer points into a hash set for O(1) corner lookups)
//   - #2013 Detect Squares [new] (hash-indexed 2D point geometry queries)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <unordered_set>
#include <vector>

namespace {

class Solution {
 public:
  // Returns true if there exists a vertical line x = c across which the set of
  // input points is symmetric.
  //
  // Invariant: Any valid reflection axis must satisfy 2c = min_x + max_x,
  // because reflection reverses x-order and therefore maps min_x <-> max_x.
  //
  // Time Complexity: O(n) average.
  // Space Complexity: O(n).
  bool isReflected(const std::vector<std::vector<int>>& points) {
    if (points.empty()) {
      return true;
    }

    std::int64_t min_x = std::numeric_limits<std::int64_t>::max();
    std::int64_t max_x = std::numeric_limits<std::int64_t>::min();

    // Store unique points as bit-packed 64-bit keys for O(1) average lookup.
    std::unordered_set<std::uint64_t> point_set;
    point_set.reserve(points.size());

    for (const auto& point : points) {
      const auto x = static_cast<std::int64_t>(point[0]);
      min_x = std::min(min_x, x);
      max_x = std::max(max_x, x);
      point_set.insert(packPoint(point[0], point[1]));
    }

    // 2 * axis_x in 64-bit arithmetic avoids both floating-point rounding and
    // 32-bit signed addition overflow.
    const std::int64_t twice_axis = min_x + max_x;

    // Verify that every point's mirror across x = twice_axis / 2 is present.
    // Because min_x <= x <= max_x, twice_axis - x is guaranteed to lie within
    // [min_x, max_x] and therefore fits in a 32-bit signed int.
    return std::ranges::all_of(
        points, [twice_axis, &point_set](const std::vector<int>& point) {
          const std::int64_t mirror_x =
              twice_axis - static_cast<std::int64_t>(point[0]);
          const std::uint64_t mirror_key =
              packPoint(static_cast<std::int32_t>(mirror_x), point[1]);
          return point_set.contains(mirror_key);
        });
  }

 private:
  // Bijectively packs a 2D 32-bit integer coordinate pair (x, y) into an
  // unsigned 64-bit key without signed-shift UB or sign-extension collisions.
  static constexpr std::uint64_t packPoint(std::int32_t x, std::int32_t y) {
    const auto ux = static_cast<std::uint32_t>(x);
    const auto uy = static_cast<std::uint32_t>(y);
    return (static_cast<std::uint64_t>(ux) << 32U) |
           static_cast<std::uint64_t>(uy);
  }
};

LC_PROBLEM(356, "Line Reflection", "hash", ctx) {
  Solution sol;

  ctx.test("example_1_symmetric_about_y_axis", [&] {
    ctx.expect_true(sol.isReflected({{1, 1}, {-1, 1}}));
  });

  ctx.test("example_2_different_y_coordinates", [&] {
    ctx.expect_false(sol.isReflected({{1, 1}, {-1, -1}}));
  });

  ctx.test("duplicate_points_same_location", [&] {
    ctx.expect_true(sol.isReflected({{1, 1}, {1, 1}, {1, 1}}));
  });

  ctx.test("duplicate_points_on_one_side_only", [&] {
    ctx.expect_true(sol.isReflected({{-16, 1}, {16, 1}, {16, 1}}));
  });

  ctx.test("point_lying_on_axis", [&] {
    ctx.expect_true(sol.isReflected({{1, 1}, {0, 0}, {-1, 1}}));
  });

  ctx.test("half_integer_reflection_axis", [&] {
    ctx.expect_true(sol.isReflected({{0, 0}, {1, 0}}));
  });

  ctx.test("asymmetric_collinear_points", [&] {
    ctx.expect_false(sol.isReflected({{0, 0}, {1, 0}, {3, 0}}));
  });

  ctx.test("empty_and_single_point", [&] {
    ctx.expect_true(sol.isReflected({}));
    ctx.expect_true(sol.isReflected({{-5, -10}}));
  });

  ctx.test("negative_coordinates_and_large_values", [&] {
    static constexpr int kLarge = 1'000'000'000;
    ctx.expect_true(sol.isReflected(
        {{kLarge, -kLarge}, {kLarge - 2, -kLarge}, {kLarge - 1, 42}}));
    ctx.expect_false(sol.isReflected({{-kLarge, -1}, {kLarge, -2}}));
  });
}

}  // namespace
