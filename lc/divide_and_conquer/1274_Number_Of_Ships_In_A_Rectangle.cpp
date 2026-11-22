// https://leetcode.com/problems/number-of-ships-in-a-rectangle/description/
//
// Each ship is located at an integer point on a 2D Cartesian plane, with at
// most one ship per integer point and at most 10 ships in the queried
// rectangle. Given an interactive `Sea::hasShips(topRight, bottomLeft)` API
// that returns true iff at least one ship lies inside or on the boundary of
// the rectangle, return the exact number of ships inside the rectangle.

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
