// LeetCode 223: Rectangle Area
// https://leetcode.com/problems/rectangle-area/
//
// Problem:
//   Given the coordinates of two rectilinear (axis-aligned) rectangles in a 2D
//   plane—the first defined by bottom-left (ax1, ay1) and top-right (ax2, ay2),
//   and the second by bottom-left (bx1, by1) and top-right (bx2, by2)—return
//   the total area covered by the union of the two rectangles.
//   Constraints: -10^4 <= ax1 <= ax2 <= 10^4, -10^4 <= ay1 <= ay2 <= 10^4,
//   and identically for b.
//
// Examples:
//   Example 1:
//     Input:  ax1 = -3, ay1 = 0, ax2 = 3, ay2 = 4, bx1 = 0, by1 = -1, bx2 = 9, by2
//             = 2
//     Output: 45
//     Explanation:
//       - Compute 64-bit areas of both rectangles: area_a = (3 - (-3)) * (4 - 0) =
//         24 and area_b = (9 - 0) * (2 - (-1)) = 27.
//       - Compute x-axis overlap width: overlap_w = max(0, min(3, 9) - max(-3, 0))
//         = max(0, 3 - 0) = 3.
//       - Compute y-axis overlap height: overlap_h = max(0, min(4, 2) - max(0, -1))
//         = max(0, 2 - 0) = 2.
//       - Multiply overlap_w * overlap_h = 6 and apply inclusion-exclusion: (24 +
//         27) - 6 = 45.
//
//   Example 2:
//     Input:  ax1 = -2, ay1 = -2, ax2 = 2, ay2 = 2, bx1 = -2, by1 = -2, bx2 = 2,
//             by2 = 2
//     Output: 16
//     Explanation:
//       - Compute both rectangle areas: area_a = (2 - (-2)) * (2 - (-2)) = 16 and
//         area_b = 16.
//       - Compute axis overlaps: overlap_w = max(0, 2 - (-2)) = 4 and overlap_h =
//         max(0, 2 - (-2)) = 4.
//       - Subtract overlap_area = 4 * 4 = 16 from the sum: (16 + 16) - 16 = 16.
//
//   Example 3:
//     Input:  ax1 = 0, ay1 = 0, ax2 = 2, ay2 = 2, bx1 = 3, by1 = 3, bx2 = 5, by2 =
//             5
//     Output: 8
//     Explanation:
//       - Compute both rectangle areas: area_a = 2 * 2 = 4 and area_b = 2 * 2 = 4.
//       - Project onto both axes: overlap_w = max(0, min(2, 5) - max(0, 3)) =
//         max(0, -1) = 0 and overlap_h = max(0, -1) = 0.
//       - Compute overlap_area = 0 * 0 = 0 and return (4 + 4) - 0 = 8.
//
// Algorithm:
//   Inclusion-Exclusion Principle on axis-aligned bounding boxes (AABBs):
//       Area(A U B) = Area(A) + Area(B) - Area(A n B).
//   Two axis-aligned rectangles overlap if and only if their projections onto
//   both the x-axis and y-axis have positive overlap length:
//       overlap_w = max(0, min(ax2, bx2) - max(ax1, bx1))
//       overlap_h = max(0, min(ay2, by2) - max(ay1, by1))
//   Multiplying `overlap_w * overlap_h` automatically yields 0 for disjoint or
//   merely edge-touching rectangles. All intermediate arithmetic uses
//   `std::int64_t` to guarantee overflow safety even beyond problem bounds.
//   Time Complexity:  O(1)
//   Space Complexity: O(1)
//
// Data Structures:
//   - Scalar 64-bit integer geometry variables (no heap allocation required).
//
// Real-World Applications:
//   - Computer vision object-detection metrics: computing Intersection over
//     Union (IoU = Area(A n B) / Area(A U B)) for bounding-box evaluation and
//     Non-Maximum Suppression (NMS).
//   - UI compositors and window managers calculating dirty-region union areas
//     for minimal screen repaints.
//   - 2D game physics engines, GIS spatial indices (R-trees), and VLSI layout
//     verification computing bounding-box overlap and footprint coverage.
//
// Related LeetCode Problems:
//   - #836 Rectangle Overlap [done] (decides whether two AABBs have positive intersection area)
//   - #850 Rectangle Area II [new] (generalizes union area to N rectangles via sweep line)
//   - #391 Perfect Rectangle [new] (exact tiling check using area sum and corner parity)
//   - #598 Range Addition II [new] (repeated 2D prefix-rectangle intersection)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstdint>

namespace {

class Solution {
 public:
  // Returns the total area covered by rectangles A and B using inclusion-
  // exclusion: Area(A) + Area(B) - Area(A intersection B).
  //
  // Time: O(1), Space: O(1).
  int computeArea(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2,
                  int by2) {
    const std::int64_t area_a = rect_area(ax1, ay1, ax2, ay2);
    const std::int64_t area_b = rect_area(bx1, by1, bx2, by2);

    // Project both rectangles onto the x- and y-axes; clamping at 0 handles
    // disjoint and edge-touching rectangles without extra branches.
    const std::int64_t overlap_w =
        std::max<std::int64_t>(0, static_cast<std::int64_t>(std::min(ax2, bx2)) -
                                      static_cast<std::int64_t>(std::max(ax1, bx1)));
    const std::int64_t overlap_h =
        std::max<std::int64_t>(0, static_cast<std::int64_t>(std::min(ay2, by2)) -
                                      static_cast<std::int64_t>(std::max(ay1, by1)));
    const std::int64_t overlap_area = overlap_w * overlap_h;

    return static_cast<int>((area_a + area_b) - overlap_area);
  }

 private:
  // Computes the area of an axis-aligned rectangle in 64-bit arithmetic to
  // avoid signed 32-bit intermediate overflow.
  static std::int64_t rect_area(int x1, int y1, int x2, int y2) {
    const std::int64_t width =
        static_cast<std::int64_t>(x2) - static_cast<std::int64_t>(x1);
    const std::int64_t height =
        static_cast<std::int64_t>(y2) - static_cast<std::int64_t>(y1);
    return width * height;
  }
};

LC_PROBLEM(223, "Rectangle Area", "math", ctx) {
  Solution sol;

  ctx.test("example_1_partial_overlap", [&] {
    ctx.expect_eq(sol.computeArea(-3, 0, 3, 4, 0, -1, 9, 2), 45);
  });

  ctx.test("example_2_identical_rectangles", [&] {
    ctx.expect_eq(sol.computeArea(-2, -2, 2, 2, -2, -2, 2, 2), 16);
  });

  ctx.test("disjoint_and_touching", [&] {
    // Strictly disjoint.
    ctx.expect_eq(sol.computeArea(0, 0, 2, 2, 3, 3, 5, 5), 8);
    // Sharing an edge (zero-area overlap).
    ctx.expect_eq(sol.computeArea(0, 0, 2, 2, 2, 0, 4, 2), 8);
  });

  ctx.test("contained_and_degenerate", [&] {
    // Second rectangle strictly inside the first.
    ctx.expect_eq(sol.computeArea(0, 0, 10, 10, 2, 2, 5, 5), 100);
    // First rectangle degenerates to a vertical segment (zero area).
    ctx.expect_eq(sol.computeArea(0, 0, 0, 5, 1, 1, 3, 3), 4);
  });
}

}  // namespace
