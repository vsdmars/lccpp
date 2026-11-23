// LeetCode 54: Spiral Matrix
// https://leetcode.com/problems/spiral-matrix/
//
// Problem:
//   Given an `m x n` matrix of integers, return all `m * n` elements of the
//   matrix visited in clockwise spiral order starting from the top-left corner
//   `(0, 0)` and moving right, down, left, and up while shrinking inward.
//
// Algorithm:
//   Maintain four half-open layer boundaries `[top, bottom)` for rows and
//   `[left, right)` for columns. In each iteration, peel off the outer
//   perimeter in four sweeps:
//     1. Top row from `left` to `right - 1`, then increment `top`.
//     2. Right column from `top` to `bottom - 1`, then decrement `right`.
//     3. Bottom row from `right - 1` down to `left` (only if `top < bottom`),
//        then decrement `bottom`.
//     4. Left column from `bottom - 1` down to `top` (only if `left < right`),
//        then increment `left`.
//   Using half-open `std::size_t` intervals avoids unsigned underflow, never
//   mutates the input matrix, and requires no auxiliary visited grid.
//   Time Complexity: O(m * n), visiting every cell exactly once.
//   Space Complexity: O(1) auxiliary space (excluding the output vector).
//
// Data Structures:
//   - Four `std::size_t` boundary cursors (`top`, `bottom`, `left`, `right`).
//   - Pre-reserved `std::vector<int>` storing the spiral traversal order.
//
// Real-World Applications:
//   - Image and video scan-order transforms (e.g., spiral/zigzag traversal for
//     block codecs, radial search in motion estimation, and pixel-patch sampling).
//   - Robotics coverage path planning and CNC spiral milling toolpaths.
//   - Printing, radar/sensor rasterization, and tile-based map loading from
//     the viewport outward or inward.
//
// Related LeetCode Problems:
//   - #59 Spiral Matrix II [done] (constructs an n x n matrix in clockwise spiral order)
//   - #885 Spiral Matrix III [done] (unbounded outward spiral walk on a grid)
//   - #2326 Spiral Matrix IV [new] (fills a rectangular matrix in spiral order from a linked list)
//   - #48 Rotate Image [done] (layer-by-layer concentric matrix manipulation)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Returns all elements of `matrix` in clockwise spiral order without
  // mutating the input matrix or allocating a visited grid.
  // Time: O(m * n), Space: O(1) auxiliary.
  std::vector<int> spiralOrder(const std::vector<std::vector<int>>& matrix) {
    if (matrix.empty() || matrix[0].empty()) {
      return {};
    }

    // Half-open active submatrix bounds: rows in [top, bottom), cols in [left, right).
    std::size_t top = 0uz;
    std::size_t bottom = matrix.size();
    std::size_t left = 0uz;
    std::size_t right = matrix[0].size();

    std::vector<int> result;
    result.reserve(bottom * right);

    while (top < bottom && left < right) {
      // 1. Traverse left-to-right across the current top row.
      for (std::size_t c = left; c < right; ++c) {
        result.push_back(matrix[top][c]);
      }
      ++top;

      // 2. Traverse top-to-bottom down the current rightmost column.
      for (std::size_t r = top; r < bottom; ++r) {
        result.push_back(matrix[r][right - 1uz]);
      }
      --right;

      // 3. Traverse right-to-left across the bottom row if an unvisited row remains.
      if (top < bottom) {
        for (std::size_t c = right; c > left; --c) {
          result.push_back(matrix[bottom - 1uz][c - 1uz]);
        }
        --bottom;
      }

      // 4. Traverse bottom-to-top up the leftmost column if an unvisited column remains.
      if (left < right) {
        for (std::size_t r = bottom; r > top; --r) {
          result.push_back(matrix[r - 1uz][left]);
        }
        ++left;
      }
    }

    return result;
  }
};

LC_PROBLEM(54, "Spiral Matrix", "math", ctx) {
  Solution sol;

  ctx.test("example_3x3", [&] {
    const std::vector<std::vector<int>> matrix = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9},
    };
    const std::vector<int> expected = {1, 2, 3, 6, 9, 8, 7, 4, 5};
    ctx.expect_eq(sol.spiralOrder(matrix), expected);
  });

  ctx.test("example_3x4", [&] {
    const std::vector<std::vector<int>> matrix = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12},
    };
    const std::vector<int> expected = {1, 2, 3, 4, 8, 12, 11, 10, 9, 5, 6, 7};
    ctx.expect_eq(sol.spiralOrder(matrix), expected);
  });

  ctx.test("degenerate_shapes", [&] {
    ctx.expect_eq(sol.spiralOrder({}), std::vector<int>{});
    ctx.expect_eq(sol.spiralOrder({{42}}), std::vector<int>{42});
    ctx.expect_eq(sol.spiralOrder({{1, 2, 3, 4}}),
                  std::vector<int>{1, 2, 3, 4});
    ctx.expect_eq(sol.spiralOrder({{1}, {2}, {3}, {4}}),
                  std::vector<int>{1, 2, 3, 4});
  });

  ctx.test("tall_rectangle_4x2", [&] {
    const std::vector<std::vector<int>> matrix = {
        {1, 2},
        {3, 4},
        {5, 6},
        {7, 8},
    };
    const std::vector<int> expected = {1, 2, 4, 6, 8, 7, 5, 3};
    ctx.expect_eq(sol.spiralOrder(matrix), expected);
  });
}

}  // namespace
