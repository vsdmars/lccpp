// LeetCode 836: Rectangle Overlap
// https://leetcode.com/problems/rectangle-overlap/
//
// Problem:
//   An axis-aligned rectangle is represented as a 4-element list
//   [x1, y1, x2, y2], where (x1, y1) is the bottom-left corner and (x2, y2) is
//   the top-right corner, with coordinates in [-10^9, 10^9]. Two rectangles
//   overlap if and only if the area of their intersection is strictly positive
//   (rectangles that merely touch along an edge or at a corner do not overlap).
//   Given two rectangles `rec1` and `rec2`, return whether they overlap.
//
// Algorithm:
//   Separating Axis Theorem (1D interval projection) on the x- and y-axes:
//   - The intersection of two axis-aligned rectangles is itself an axis-aligned
//     rectangle whose x-projection is [max(x1, x1'), min(x2, x2')] and whose
//     y-projection is [max(y1, y1'), min(y2, y2')].
//   - The intersection has strictly positive area if and only if both projected
//     intervals have strictly positive length:
//       max(rec1[0], rec2[0]) < min(rec1[2], rec2[2]) &&
//       max(rec1[1], rec2[1]) < min(rec1[3], rec2[3])
//   - This formulation naturally rejects both separated/touching rectangles and
//     degenerate zero-area (line or point) rectangles.
//   Time: O(1), Space: O(1).
//
// Data Structures:
//   None (constant-time comparisons on two 4-element integer arrays).
//
// Real-World Applications:
//   - Broad-phase AABB (Axis-Aligned Bounding Box) collision detection in 2D
//     physics engines and games.
//   - Spatial indexing (R-trees, quadtrees) and viewport/window dirty-region
//     intersection in GUI compositors and GIS map engines.
//
// Related LeetCode Problems:
//   - #223 Rectangle Area [done] (computes union area of two rectangles using their overlap)
//   - #391 Perfect Rectangle [new] (checks exact non-overlapping cover of a bounding box)
//   - #850 Rectangle Area II [new] (union area of multiple overlapping rectangles via sweep line)
//   - #56 Merge Intervals [done] (1D interval overlap detection and merging)

#include "framework/lc_framework.h"

#include <algorithm>
#include <vector>

namespace {

class Solution {
 public:
  // Returns true iff the axis-aligned rectangles `rec1` and `rec2` share a
  // strictly positive intersection area.
  //
  // Invariant: Positive 2D overlap holds iff both the x-interval intersection
  // and y-interval intersection have strictly positive width/height.
  //
  // Time: O(1), Space: O(1).
  bool isRectangleOverlap(const std::vector<int>& rec1,
                          const std::vector<int>& rec2) {
    if (rec1.size() < 4uz || rec2.size() < 4uz) {
      return false;
    }

    // Project onto the x-axis and y-axis: strictly positive intersection length
    // on both axes guarantees positive overlap area (and rejects touching edges
    // as well as degenerate zero-width/zero-height rectangles).
    const bool x_overlap = std::max(rec1[0], rec2[0]) < std::min(rec1[2], rec2[2]);
    const bool y_overlap = std::max(rec1[1], rec2[1]) < std::min(rec1[3], rec2[3]);
    return x_overlap && y_overlap;
  }
};

LC_PROBLEM(836, "Rectangle Overlap", "math", ctx) {
  Solution sol;

  ctx.test("example_1_partial_overlap", [&] {
    ctx.expect_true(sol.isRectangleOverlap({0, 0, 2, 2}, {1, 1, 3, 3}));
  });

  ctx.test("example_2_touching_edge_not_overlap", [&] {
    ctx.expect_false(sol.isRectangleOverlap({0, 0, 1, 1}, {1, 0, 2, 1}));
  });

  ctx.test("touching_corner_not_overlap", [&] {
    ctx.expect_false(sol.isRectangleOverlap({0, 0, 1, 1}, {1, 1, 2, 2}));
  });

  ctx.test("python_build_case_tall_inside_wide", [&] {
    ctx.expect_true(sol.isRectangleOverlap({7, 8, 13, 15}, {10, 8, 12, 20}));
  });

  ctx.test("one_contains_another", [&] {
    ctx.expect_true(sol.isRectangleOverlap({-10, -10, 10, 10}, {-2, -3, 4, 5}));
  });

  ctx.test("completely_disjoint", [&] {
    ctx.expect_false(sol.isRectangleOverlap({0, 0, 2, 2}, {5, 5, 7, 7}));
    ctx.expect_false(sol.isRectangleOverlap({5, 5, 7, 7}, {0, 0, 2, 2}));
  });

  ctx.test("degenerate_zero_area_rectangle", [&] {
    ctx.expect_false(sol.isRectangleOverlap({0, 0, 0, 2}, {-1, -1, 1, 1}));
    ctx.expect_false(sol.isRectangleOverlap({-1, -1, 1, 1}, {0, 0, 2, 0}));
  });

  ctx.test("extreme_coordinate_bounds", [&] {
    ctx.expect_true(sol.isRectangleOverlap({-1'000'000'000, -1'000'000'000, 1, 1},
                                           {0, 0, 1'000'000'000, 1'000'000'000}));
  });
}

}  // namespace
