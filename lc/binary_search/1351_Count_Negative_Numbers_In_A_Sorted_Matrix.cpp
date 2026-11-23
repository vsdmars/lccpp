// LeetCode 1351: Count Negative Numbers In A Sorted Matrix
// https://leetcode.com/problems/count-negative-numbers-in-a-sorted-matrix/
//
// Problem:
//   Given an `m x n` integer matrix `grid` sorted in non-increasing order both
//   row-wise (left to right) and column-wise (top to bottom), return the total
//   count of strictly negative numbers (`< 0`) in `grid`.
//   Constraints: 1 <= m, n <= 100, -100 <= grid[i][j] <= 100.
//
// Examples:
//   Example 1:
//     Input:  grid = [[4, 3, 2, -1], [3, 2, 1, -1], [1, 1, -1, -2], [-1, -1, -2,
//             -3]]
//     Output: 8
//     Explanation:
//       - Initialize cols = 4, first_neg_col = 4, and count = 0 for the staircase
//         walk.
//       - Row 0 [4, 3, 2, -1] decrements first_neg_col to 3 (row[2] = 2 >= 0),
//         adding 4 - 3 = 1 (count = 1); row 1 [3, 2, 1, -1] keeps first_neg_col = 3
//         and adds 1 (count = 2).
//       - Row 2 [1, 1, -1, -2] decrements first_neg_col across index 2 (-1 < 0) to
//         2 (row[1] = 1 >= 0), adding 4 - 2 = 2 (count = 4).
//       - Row 3 [-1, -1, -2, -3] decrements first_neg_col from 2 to 0, adding 4 - 0
//         = 4 and returning 8.
//
//   Example 2:
//     Input:  grid = [[3, 2], [1, 0]]
//     Output: 0
//     Explanation:
//       - Initialize cols = 2, first_neg_col = 2, and count = 0.
//       - In row 0 [3, 2], row[1] = 2 >= 0 leaves first_neg_col = 2 and adds 2 - 2
//         = 0 to count.
//       - In row 1 [1, 0], row[1] = 0 >= 0 is not strictly negative, so
//         first_neg_col remains 2 and the total count is 0.
//
//   Example 3:
//     Input:  grid = [[5, 1, 0], [-5, -5, -5]]
//     Output: 3
//     Explanation:
//       - Initialize cols = 3, first_neg_col = 3, and count = 0.
//       - In row 0 [5, 1, 0], row[2] = 0 >= 0 keeps first_neg_col = 3, contributing
//         3 - 3 = 0 negatives.
//       - In row 1 [-5, -5, -5], first_neg_col moves left across indices 2, 1, and
//         0 (all -5 < 0) to 0, adding 3 - 0 = 3 and returning 3.
//
// Algorithm:
//   Two clean approaches exploiting the matrix monotonicity:
//   1. Per-Row Binary Search (`countNegatives`):
//      Each row is partitioned into a prefix of non-negative values (`>= 0`)
//      followed by a suffix of negative values (`< 0`). Use
//      `std::ranges::partition_point` on each row to find the first negative
//      element in O(log n) without copying or reversing the row, then add the
//      suffix length to the total.
//      Time Complexity:  O(m log n).
//      Space Complexity: O(1).
//   2. Staircase Boundary Traversal (`countNegativesStaircase`, optimal):
//      Because columns are also non-increasing, the index of the first negative
//      element can only move left (or stay put) as we advance downward from row
//      `0` to row `m - 1`. Track `first_neg_col` starting at `n`; in each row,
//      decrement `first_neg_col` while `row[first_neg_col - 1] < 0`, then add
//      `n - first_neg_col` to the answer.
//      Time Complexity:  O(m + n).
//      Space Complexity: O(1).
//
// Data Structures:
//   - `std::vector<std::vector<int>>` (2D row-major matrix input).
//
// Real-World Applications:
//   - Counting threshold violations (e.g., sub-zero temperatures, negative
//     margins, or SLA breaches) across sorted 2D pivot tables or heatmaps.
//   - Range counting over monotonically ordered 2D cost/elevation grids in
//     GIS, scientific computing, and database query engines.
//
// Related LeetCode Problems:
//   - #74 Search a 2D Matrix [done] (binary search over globally sorted 2D grid)
//   - #240 Search a 2D Matrix II [done] (same O(m + n) staircase elimination pattern)
//   - #378 Kth Smallest Element in a Sorted Matrix [done] (value BS + staircase count)
//   - #1060 Missing Element In Sorted Array [done] (monotonic predicate binary search)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <vector>

namespace {

class Solution {
 public:
  // Counts negative entries by binary-searching the non-negative/negative
  // partition boundary in each non-increasing row.
  // Time: O(m log n), Space: O(1).
  int countNegatives(const std::vector<std::vector<int>>& grid) {
    int count = 0;
    for (const auto& row : grid) {
      // Every non-increasing row has all `>= 0` elements before all `< 0`.
      const auto first_neg =
          std::ranges::partition_point(row, [](int val) { return val >= 0; });
      count += static_cast<int>(std::distance(first_neg, row.end()));
    }
    return count;
  }

  // Optimal staircase walk exploiting both row-wise and column-wise
  // non-increasing order.
  // Time: O(m + n), Space: O(1).
  int countNegativesStaircase(const std::vector<std::vector<int>>& grid) {
    if (grid.empty() || grid.front().empty()) {
      return 0;
    }

    const std::size_t cols = grid.front().size();
    std::size_t first_neg_col = cols;
    int count = 0;

    // As we move down rows, column values only decrease, so `first_neg_col`
    // moves monotonically left from `cols` toward `0`.
    for (const auto& row : grid) {
      while (first_neg_col > 0uz && row[first_neg_col - 1uz] < 0) {
        --first_neg_col;
      }
      count += static_cast<int>(cols - first_neg_col);
    }

    return count;
  }
};

LC_PROBLEM(1351, "Count Negative Numbers In A Sorted Matrix", "binary_search",
           ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    const std::vector<std::vector<int>> grid = {
        {4, 3, 2, -1},
        {3, 2, 1, -1},
        {1, 1, -1, -2},
        {-1, -1, -2, -3},
    };
    ctx.expect_eq(sol.countNegatives(grid), 8);
    ctx.expect_eq(sol.countNegativesStaircase(grid), 8);
  });

  ctx.test("example_2_no_negatives", [&] {
    const std::vector<std::vector<int>> grid = {{3, 2}, {1, 0}};
    ctx.expect_eq(sol.countNegatives(grid), 0);
    ctx.expect_eq(sol.countNegativesStaircase(grid), 0);
  });

  ctx.test("all_negatives", [&] {
    const std::vector<std::vector<int>> grid = {{-1, -2}, {-3, -4}};
    ctx.expect_eq(sol.countNegatives(grid), 4);
    ctx.expect_eq(sol.countNegativesStaircase(grid), 4);
  });

  ctx.test("single_cell_boundary_cases", [&] {
    ctx.expect_eq(sol.countNegatives({{0}}), 0);
    ctx.expect_eq(sol.countNegativesStaircase({{0}}), 0);
    ctx.expect_eq(sol.countNegatives({{-1}}), 1);
    ctx.expect_eq(sol.countNegativesStaircase({{-1}}), 1);
  });

  ctx.test("rectangular_matrix_with_zeros", [&] {
    const std::vector<std::vector<int>> grid = {
        {5, 1, 0},
        {-5, -5, -5},
    };
    ctx.expect_eq(sol.countNegatives(grid), 3);
    ctx.expect_eq(sol.countNegativesStaircase(grid), 3);
  });
}

}  // namespace
