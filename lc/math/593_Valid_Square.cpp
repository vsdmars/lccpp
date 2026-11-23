// LeetCode 593: Valid Square
// https://leetcode.com/problems/valid-square/
//
// Problem:
//   Given the 2D integer coordinates of four points `p1`, `p2`, `p3`, `p4` in
//   arbitrary order, return `true` if the four points construct a valid square
//   (four equal sides of strictly positive length and four 90-degree interior
//   angles), and `false` otherwise.
//   Constraints: Each point has 2 integer coordinates in [-10^4, 10^4].
//
// Algorithm:
//   Order-invariant pairwise squared-distance classification.
//   Compute all C(4, 2) = 6 pairwise squared Euclidean distances using exact
//   64-bit integer arithmetic (avoiding floating-point `sqrt` precision error).
//   Sort the 6 squared distances in ascending order: d[0] <= ... <= d[5].
//   Four points in 2D form a valid square if and only if:
//     1. d[0] > 0               (all four points are distinct; positive sides),
//     2. d[0] == d[3]           (the 4 smallest distances are all equal sides),
//     3. d[4] == d[5]           (the 2 largest distances are equal diagonals),
//     4. d[4] == 2 * d[0]       (Pythagorean relation confirms 90-degree angles).
//   Time Complexity:  O(1) — constant 6 distances and fixed-size sort.
//   Space Complexity: O(1) — fixed `std::array<std::int64_t, 6>`.
//
// Data Structures:
//   - `std::array<std::int64_t, 6>`: holds the 6 pairwise squared distances
//     for sorting and invariant verification without heap allocation.
//
// Real-World Applications:
//   - Computer vision and fiducial marker detection (e.g., QR code / AprilTag
//     corner validation from unordered detected keypoints).
//   - CAD / GIS geometry validation engines checking orthogonal quad cells and
//     axis-rotated square footprints.
//   - Computational geometry mesh quality checks verifying undistorted quad
//     elements in finite-element analysis.
//
// Related LeetCode Problems:
//   - #939 Minimum Area Rectangle [done] (identifying axis-aligned rectangles from 2D point sets)
//   - #963 Minimum Area Rectangle II [done] (identifying arbitrary rotated rectangles via diagonals)
//   - #2013 Detect Squares [new] (counting axis-aligned squares dynamically on a 2D plane)
//   - #149 Max Points on a Line [done] (exact integer 2D geometry avoiding floating-point drift)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Determines whether four unordered 2D points form a non-degenerate square.
  //
  // Invariant: A valid square has 4 equal positive side lengths s and 2 equal
  // diagonal lengths d satisfying d^2 == 2 * s^2.
  // Time: O(1), Space: O(1).
  bool validSquare(const std::vector<int>& p1, const std::vector<int>& p2,
                   const std::vector<int>& p3, const std::vector<int>& p4) {
    std::array<std::int64_t, 6> dists = {
        squared_dist(p1, p2), squared_dist(p1, p3), squared_dist(p1, p4),
        squared_dist(p2, p3), squared_dist(p2, p4), squared_dist(p3, p4),
    };

    std::ranges::sort(dists);

    // Since `dists` is sorted:
    //   - dists[0] > 0 rejects coinciding points (zero-length sides).
    //   - dists[0] == dists[3] implies dists[0] == dists[1] == dists[2] == dists[3]
    //     (all four sides are equal).
    //   - dists[4] == dists[5] verifies equal diagonals (rules out non-square rhombi).
    //   - dists[4] == 2 * dists[0] confirms right angles via s^2 + s^2 = d^2.
    return dists[0] > 0 && dists[0] == dists[3] && dists[4] == dists[5] &&
           dists[4] == (2 * dists[0]);
  }

 private:
  // Computes exact squared Euclidean distance between two 2D points in 64-bit
  // integer arithmetic to avoid floating-point error and signed overflow.
  static std::int64_t squared_dist(const std::vector<int>& a,
                                   const std::vector<int>& b) {
    const std::int64_t dx = static_cast<std::int64_t>(a[0]) - b[0];
    const std::int64_t dy = static_cast<std::int64_t>(a[1]) - b[1];
    return (dx * dx) + (dy * dy);
  }
};

LC_PROBLEM(593, "Valid Square", "math", ctx) {
  Solution sol;

  ctx.test("example_1_unit_square", [&] {
    ctx.expect_true(sol.validSquare({0, 0}, {1, 1}, {1, 0}, {0, 1}));
  });

  ctx.test("rotated_square", [&] {
    // 45-degree rotated square (diamond) with side length sqrt(2).
    ctx.expect_true(sol.validSquare({1, 0}, {-1, 0}, {0, 1}, {0, -1}));
  });

  ctx.test("arbitrary_tilted_square", [&] {
    // Side vector (2, 1) and (-1, 2) -> side^2 = 5, diag^2 = 10.
    ctx.expect_true(sol.validSquare({0, 0}, {2, 1}, {1, 3}, {-1, 2}));
  });

  ctx.test("non_square_rectangle", [&] {
    ctx.expect_false(sol.validSquare({0, 0}, {2, 0}, {2, 1}, {0, 1}));
  });

  ctx.test("all_points_coincide", [&] {
    ctx.expect_false(sol.validSquare({0, 0}, {0, 0}, {0, 0}, {0, 0}));
  });

  ctx.test("duplicate_pairs_degenerate", [&] {
    ctx.expect_false(sol.validSquare({0, 0}, {0, 0}, {1, 0}, {1, 0}));
  });

  ctx.test("slightly_skewed_quad", [&] {
    ctx.expect_false(sol.validSquare({0, 0}, {1, 1}, {1, 0}, {0, 12}));
  });
}

}  // namespace
