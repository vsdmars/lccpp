// LeetCode 1274: Number of Ships in a Rectangle
// https://leetcode.com/problems/number-of-ships-in-a-rectangle/
//
// Problem:
//   Each ship sits at a distinct integer coordinate on a 2D Cartesian grid
//   (up to 1000 x 1000), with at most one ship per point and at most 10 ships
//   inside the queried rectangle `[bottomLeft, topRight]`. Given only an
//   interactive oracle `Sea::hasShips(topRight, bottomLeft)` that returns true
//   iff at least one ship lies inside or on the boundary of that rectangle,
//   return the exact number of ships while making at most 400 oracle calls.
//
// Examples:
//   Example 1:
//     Input:  ships = [[1, 1], [2, 2], [3, 3], [5, 5]], topRight = [4, 4],
//             bottomLeft = [0, 0]
//     Output: 3
//     Explanation:
//       - countShips calls sea.hasShips([4, 4], [0, 0]), which returns true while
//         ignoring [5, 5] outside the queried bounds.
//       - The rectangle splits at mid_x = 2, mid_y = 2 into four disjoint
//         quadrants: [0..2, 0..2], [3..4, 3..4], [0..2, 3..4], and [3..4, 0..2].
//       - Quadrants [0..2, 3..4] and [3..4, 0..2] each return 0 in a single
//         hasShips call because they contain no ships.
//       - Quadrant [0..2, 0..2] recurses down to single points [1, 1] and [2, 2] to
//         return 2, and [3..4, 3..4] narrows down to [3, 3] to return 1, summing to
//         3.
//
//   Example 2:
//     Input:  ships = [], topRight = [1000, 1000], bottomLeft = [0, 0]
//     Output: 0
//     Explanation:
//       - Bounds check passes for [0, 0] and [1000, 1000], so countShips invokes
//         sea.hasShips([1000, 1000], [0, 0]).
//       - The oracle finds no ships inside the rectangle and returns false after 1
//         query.
//       - countShips immediately returns 0 without subdividing into quadrants.
//
//   Example 3:
//     Input:  ships = [[7, 9]], topRight = [7, 9], bottomLeft = [7, 9]
//     Output: 1
//     Explanation:
//       - Bounds check passes since 7 <= 7 and 9 <= 9, and sea.hasShips([7, 9], [7,
//         9]) returns true.
//       - The base-case check bottom_left == top_right evaluates to true at [7, 9].
//       - countShips returns 1 directly without computing midpoints or making
//         recursive calls.
//
// Algorithm:
//   2D divide-and-conquer (implicit quadtree search). For a rectangle
//   `[bottom_left, top_right]`, first discard inverted bounds and immediately
//   prune empty regions when `sea.hasShips` returns false. If the rectangle
//   collapses to a single point (`bottom_left == top_right`), it must contain
//   exactly 1 ship. Otherwise, split at `(mid_x, mid_y)` into four pairwise
//   disjoint subrectangles so no boundary point is double-counted, and sum the
//   recursive counts. Because at most `S <= 10` ships exist, at most `S`
//   branches stay active at each level of the quadtree, keeping total queries
//   far below the 400-call budget.
//   Time: O(S * log(max(W, H))) oracle calls and work, where W and H are the
//   rectangle dimensions and S <= 10. Space: O(log(max(W, H))) recursion stack.
//
// Data Structures:
//   - `Point`: lightweight 2D integer coordinate struct (`x`, `y`) with
//     defaulted equality comparison.
//   - `Sea`: mock judge oracle holding `std::vector<Point> ships_` and a
//     mutable `query_count_` counter for test verification.
//   - Call stack: implicitly represents the active root-to-leaf path in the
//     quadtree without allocating explicit tree nodes.
//
// Real-World Applications:
//   - Spatial indexing and quadtree range search in GIS, radar/lidar target
//     localization, and game-engine broad-phase collision detection.
//   - Sparse 2D defect detection and group testing, where area probes quickly
//     eliminate large empty regions before zooming into active hotspots.
//   - Adaptive image compression and hierarchical spatial partitioning.
//
// Related LeetCode Problems:
//   - #427 Construct Quad Tree [done] (explicitly builds a 2D quadtree by recursively
//     subdividing a grid into four quadrants)
//   - #558 Logical OR of Two Binary Grids Represented as Quad-Trees [new] (operates
//     recursively over quadtree partitions)
//   - #240 Search a 2D Matrix II [done] (2D divide-and-conquer / quadrant pruning)
//   - #302 Smallest Rectangle Enclosing Black Pixels [new] (binary-search probing
//     over a 2D grid to locate sparse occupied coordinates)

#include "framework/lc_framework.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace {

struct Point {
  int x = 0;
  int y = 0;

  bool operator==(const Point&) const = default;
};

// Mock judge API backing `hasShips` and tracking the number of oracle queries.
class Sea {
 public:
  explicit Sea(std::vector<Point> ships) : ships_(std::move(ships)) {}

  [[nodiscard]] bool hasShips(Point top_right, Point bottom_left) const {
    ++query_count_;
    return std::ranges::any_of(
        ships_, [top_right, bottom_left](const Point& ship) {
          return ship.x >= bottom_left.x && ship.x <= top_right.x &&
                 ship.y >= bottom_left.y && ship.y <= top_right.y;
        });
  }

  [[nodiscard]] int query_count() const noexcept { return query_count_; }

 private:
  std::vector<Point> ships_;
  mutable int query_count_ = 0;
};

class Solution {
 public:
  // 2D divide and conquer (quadtree search): prune empty subrectangles
  // immediately via `hasShips`, and split non-empty rectangles into four
  // disjoint quadrants until reaching single points.
  // Time: O(S * log(max(W, H))) where S <= 10 is the number of ships.
  // Space: O(log(max(W, H))) recursion depth.
  int countShips(const Sea& sea, Point top_right, Point bottom_left) {
    if (bottom_left.x > top_right.x || bottom_left.y > top_right.y) {
      return 0;
    }
    if (!sea.hasShips(top_right, bottom_left)) {
      return 0;
    }
    if (bottom_left == top_right) {
      return 1;
    }

    const int mid_x = bottom_left.x + ((top_right.x - bottom_left.x) / 2);
    const int mid_y = bottom_left.y + ((top_right.y - bottom_left.y) / 2);

    // Partition into four non-overlapping quadrants to avoid double-counting.
    return countShips(sea, {.x = mid_x, .y = mid_y}, bottom_left) +
           countShips(sea, top_right, {.x = mid_x + 1, .y = mid_y + 1}) +
           countShips(sea, {.x = mid_x, .y = top_right.y},
                      {.x = bottom_left.x, .y = mid_y + 1}) +
           countShips(sea, {.x = top_right.x, .y = mid_y},
                      {.x = mid_x + 1, .y = bottom_left.y});
  }
};

LC_PROBLEM(1274, "Number Of Ships In A Rectangle", "divide_and_conquer", ctx) {
  ctx.test("leetcode_example_1", [&] {
    const Sea sea({{.x = 1, .y = 1},
                   {.x = 2, .y = 2},
                   {.x = 3, .y = 3},
                   {.x = 5, .y = 5}});
    Solution sol;
    ctx.expect_eq(sol.countShips(sea, {.x = 4, .y = 4}, {.x = 0, .y = 0}), 3);
    ctx.expect_true(sea.query_count() <= 400);
  });

  ctx.test("leetcode_example_2_large_grid", [&] {
    const Sea sea({{.x = 1, .y = 1}, {.x = 2, .y = 2}, {.x = 3, .y = 3}});
    Solution sol;
    ctx.expect_eq(
        sol.countShips(sea, {.x = 1000, .y = 1000}, {.x = 0, .y = 0}), 3);
    ctx.expect_true(sea.query_count() <= 400);
  });

  ctx.test("no_ships_prunes_in_one_query", [&] {
    const Sea sea({});
    Solution sol;
    ctx.expect_eq(
        sol.countShips(sea, {.x = 1000, .y = 1000}, {.x = 0, .y = 0}), 0);
    ctx.expect_eq(sea.query_count(), 1);
  });

  ctx.test("single_point_rectangle", [&] {
    const Sea sea({{.x = 7, .y = 9}});
    Solution sol;
    ctx.expect_eq(sol.countShips(sea, {.x = 7, .y = 9}, {.x = 7, .y = 9}), 1);
    ctx.expect_eq(sol.countShips(sea, {.x = 7, .y = 8}, {.x = 7, .y = 8}), 0);
  });

  ctx.test("boundary_and_ten_ships_within_query_limit", [&] {
    const Sea sea({{.x = 0, .y = 0},
                   {.x = 1000, .y = 1000},
                   {.x = 0, .y = 1000},
                   {.x = 1000, .y = 0},
                   {.x = 500, .y = 500},
                   {.x = 123, .y = 456},
                   {.x = 789, .y = 321},
                   {.x = 42, .y = 999},
                   {.x = 999, .y = 42},
                   {.x = 250, .y = 750}});
    Solution sol;
    ctx.expect_eq(
        sol.countShips(sea, {.x = 1000, .y = 1000}, {.x = 0, .y = 0}), 10);
    ctx.expect_true(sea.query_count() <= 400);
  });
}

}  // namespace
