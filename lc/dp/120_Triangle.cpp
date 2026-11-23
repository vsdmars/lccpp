// LeetCode 120: Triangle
// https://leetcode.com/problems/triangle/
//
// Problem:
//   Given a triangular array of integers where row i contains i + 1 elements,
//   return the minimum path sum from top (row 0) to bottom (row n - 1). At each
//   step, from index j on row i you may move to either index j or index j + 1
//   on row i + 1.
//   Constraints: 1 <= triangle.length <= 200; -10^4 <= triangle[i][j] <= 10^4.
//   Follow-up: solve using only O(n) extra space, where n is the number of rows.
//
// Algorithm:
//   Top-down row-by-row Dynamic Programming with two rolling 1D rows:
//   - Maintain `prev[idx]` = minimum path sum from the apex down to element
//     `idx` of the previous row.
//   - For each new row `r` (of length k = r.size()):
//       * Leftmost element (idx == 0) has only one valid parent: prev[0].
//       * Rightmost element (idx == k - 1) has only one valid parent:
//         prev[k - 2].
//       * Interior elements (0 < idx < k - 1) choose the cheaper of their two
//         parents: min(prev[idx - 1], prev[idx]) + r[idx].
//   - Move `curr` into `prev` and proceed to the next row.
//   - After processing the last row, the answer is the minimum value in `prev`.
//   - Handling boundary indices explicitly avoids sentinel infinity arithmetic
//     and guarantees zero integer overflow.
//   - Time Complexity: O(n^2), where n is the number of rows (linear in total
//     number of elements n*(n + 1)/2).
//   - Space Complexity: O(n) extra space for the two rolling row vectors.
//
// Data Structures:
//   - Two 1D rolling DP vectors (`prev` and `curr`, std::vector<int>) of size
//     at most n, representing adjacent rows of the triangle.
//
// Real-World Applications:
//   - Seam carving for content-aware image resizing (finding minimum-energy
//     vertical paths through a pixel-energy grid).
//   - Dynamic time warping and Viterbi decoding on layered DAGs / trellises.
//   - Binomial option pricing trees in quantitative finance (evaluating optimal
//     exercise paths across discrete time steps).
//
// Related LeetCode Problems:
//   - #64 Minimum Path Sum [done] (minimum-cost path across a rectangular grid)
//   - #931 Minimum Falling Path Sum [new] (row-by-row DP with 3 adjacent column transitions)
//   - #1289 Minimum Falling Path Sum II [new] (row-by-row DP choosing non-same-column parent)
//   - #118 Pascal's Triangle [done] (same triangular indexing geometry)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the minimum path sum from top to bottom of `triangle` using O(n)
  // auxiliary space via two rolling DP rows.
  //
  // Invariant: at the end of iteration r, `prev[j]` equals the minimum path sum
  // from triangle[0][0] to triangle[r][j].
  // Time: O(n^2), Space: O(n).
  int minimumTotal(const std::vector<std::vector<int>>& triangle) {
    if (triangle.empty()) {
      return 0;
    }

    // Start with the apex row as the initial minimum path sums.
    std::vector<int> prev = triangle[0];
    std::vector<int> curr;
    curr.reserve(triangle.size());

    for (std::size_t r = 1uz; r < triangle.size(); ++r) {
      const auto& row = triangle[r];
      const std::size_t width = row.size();
      curr.resize(width);

      // Left boundary: only reachable from prev[0].
      curr[0] = prev[0] + row[0];

      // Interior positions: reachable from either upper-left (idx - 1) or
      // upper-right (idx) in the previous row.
      for (std::size_t idx = 1uz; idx + 1 < width; ++idx) {
        curr[idx] = std::min(prev[idx - 1], prev[idx]) + row[idx];
      }

      // Right boundary: only reachable from prev[width - 2].
      curr[width - 1] = prev[width - 2] + row[width - 1];

      // Advance the rolling row without reallocating `prev`'s buffer on next
      // iteration.
      std::swap(prev, curr);
    }

    return std::ranges::min(prev);
  }
};

LC_PROBLEM(120, "Triangle", "dp", ctx) {
  Solution sol;

  ctx.test("example_1_standard_triangle", [&] {
    const std::vector<std::vector<int>> triangle = {
        {2},
        {3, 4},
        {6, 5, 7},
        {4, 1, 8, 3},
    };
    ctx.expect_eq(sol.minimumTotal(triangle), 11);
  });

  ctx.test("single_row", [&] {
    const std::vector<std::vector<int>> triangle = {{-10}};
    ctx.expect_eq(sol.minimumTotal(triangle), -10);
  });

  ctx.test("negative_values", [&] {
    // Optimal path: -1 -> 3 -> -3 = -1.
    const std::vector<std::vector<int>> triangle = {
        {-1},
        {2, 3},
        {1, -1, -3},
    };
    ctx.expect_eq(sol.minimumTotal(triangle), -1);
  });

  ctx.test("strictly_left_and_right_edges", [&] {
    const std::vector<std::vector<int>> left_optimal = {
        {1},
        {2, 100},
        {3, 100, 100},
    };
    ctx.expect_eq(sol.minimumTotal(left_optimal), 6);

    const std::vector<std::vector<int>> right_optimal = {
        {1},
        {100, 2},
        {100, 100, 3},
    };
    ctx.expect_eq(sol.minimumTotal(right_optimal), 6);
  });
}

}  // namespace
