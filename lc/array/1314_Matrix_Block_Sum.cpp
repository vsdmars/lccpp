// LeetCode 1314: Matrix Block Sum
// https://leetcode.com/problems/matrix-block-sum/
//
// Problem:
//   Given an m x n integer matrix `mat` and an integer `k`, return an m x n
//   matrix `answer` where each `answer[i][j]` is the sum of all valid matrix
//   elements `mat[r][c]` satisfying:
//     i - k <= r <= i + k  and  j - k <= c <= j + k.
//   Constraints: 1 <= m, n, k <= 100, 1 <= mat[i][j] <= 100.
//
// Examples:
//   Example 1:
//     Input:  mat = [[1, 2, 3], [4, 5, 6], [7, 8, 9]], k = 1
//     Output: [[12, 21, 16], [27, 45, 33], [24, 39, 28]]
//     Explanation:
//       - Build the 4x4 zero-padded prefix table where rows 1..3 hold [0, 1, 3, 6],
//         [0, 5, 12, 21], and [0, 12, 27, 45].
//       - For (0, 0), clamp to [0, 2) x [0, 2) and evaluate prefix[2][2] -
//         prefix[0][2] - prefix[2][0] + prefix[0][0] = 12.
//       - For (1, 1), clamp to [0, 3) x [0, 3) to obtain prefix[3][3] = 45; for (2,
//         2), clamp to [1, 3) x [1, 3) to obtain 45 - 6 - 12 + 1 = 28.
//       - Applying the same O(1) inclusion-exclusion query to all 9 cells yields
//         [[12, 21, 16], [27, 45, 33], [24, 39, 28]].
//
//   Example 2:
//     Input:  mat = [[1, 2, 3], [4, 5, 6], [7, 8, 9]], k = 2
//     Output: [[45, 45, 45], [45, 45, 45], [45, 45, 45]]
//     Explanation:
//       - Build the 4x4 prefix table whose bottom-right entry prefix[3][3] equals
//         45 (the sum of the entire 3x3 matrix).
//       - With radius k = 2, every cell (r, c) clamps to r1 = 0, r2 = 3, c1 = 0, c2
//         = 3.
//       - Every cell evaluates prefix[3][3] - prefix[0][3] - prefix[3][0] +
//         prefix[0][0] = 45, filling the 3x3 output with 45.
//
//   Example 3:
//     Input:  mat = [[1, 2], [3, 4]], k = 0
//     Output: [[1, 2], [3, 4]]
//     Explanation:
//       - Build the 3x3 prefix table [[0, 0, 0], [0, 1, 3], [0, 4, 10]].
//       - With radius k = 0, each cell (r, c) clamps to the 1x1 half-open window
//         [r, r + 1) x [c, c + 1).
//       - Inclusion-exclusion recovers each single element (e.g., 10 - 3 - 4 + 1 =
//         4 at (1, 1)), returning [[1, 2], [3, 4]].
//
// Algorithm:
//   1. 2D Prefix Sum / Summed-Area Table (`matrixBlockSum`, optimal):
//      Build a 1-indexed `(m + 1) x (n + 1)` integral image `prefix` where
//      `prefix[r + 1][c + 1]` equals the sum of the submatrix `[0..r] x [0..c]`
//      via inclusion-exclusion:
//        prefix[r + 1][c + 1] = mat[r][c] + prefix[r][c + 1]
//                               + prefix[r + 1][c] - prefix[r][c].
//      Then for every cell `(r, c)`, clamp its window to valid half-open
//      bounds `[r1, r2) x [c1, c2)` and evaluate the rectangle sum in O(1):
//        prefix[r2][c2] - prefix[r1][c2] - prefix[r2][c1] + prefix[r1][c1].
//      Time Complexity:  O(m * n).
//      Space Complexity: O(m * n) for the prefix-sum table.
//
//   2. Direct Window Summation (`matrixBlockSumDirect`):
//      Mirrors the reference Python implementation by clamping `[r - k, r + k]`
//      and `[c - k, c + k]` and summing each window directly.
//      Time Complexity:  O(m * n * min(m, k) * min(n, k)).
//      Space Complexity: O(1) auxiliary space beyond the output matrix.
//
// Data Structures:
//   - 2D `std::vector<std::vector<int>>` integral image (`prefix`) with a
//     zero-padded top row and left column to eliminate boundary conditionals
//     during inclusion-exclusion queries.
//
// Real-World Applications:
//   - Computer vision box filters, mean blurs, and Haar-like feature
//     extraction (Viola-Jones integral images compute arbitrary rectangular
//     pixel sums in O(1) time regardless of kernel radius `k`).
//   - Geospatial and heatmap aggregation (computing population or event totals
//     within a Manhattan/Chebyshev neighborhood around every grid cell).
//   - OLAP range-sum cubes and scientific 2D stencil / rolling-window
//     computations.
//
// Related LeetCode Problems:
//   - #304 Range Sum Query 2D [done] - Immutable (2D prefix sum query abstraction)
//   - #1292 Maximum Side Length of a Square with Sum Less than or Equal to Threshold [new] (2D prefix sums + binary search)
//   - #1074 Number of Submatrices That Sum to Target [done] (2D prefix sums + hash map)
//   - #661 Image Smoother [new] (fixed k = 1 neighborhood average over a matrix)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the block sum around every cell within Chebyshev distance `k`
  // using a 2D summed-area table (integral image).
  // Time: O(m * n), Space: O(m * n).
  std::vector<std::vector<int>> matrixBlockSum(
      const std::vector<std::vector<int>>& mat, int k) {
    if (mat.empty() || mat[0].empty()) {
      return {};
    }

    const std::size_t rows = mat.size();
    const std::size_t cols = mat[0].size();
    const std::size_t radius = k > 0 ? static_cast<std::size_t>(k) : 0uz;

    // Phase 1: build 1-indexed 2D prefix-sum table with zero-padded border.
    std::vector<std::vector<int>> prefix(rows + 1uz,
                                         std::vector<int>(cols + 1uz, 0));
    for (std::size_t r = 0uz; r < rows; ++r) {
      for (std::size_t c = 0uz; c < cols; ++c) {
        prefix[r + 1uz][c + 1uz] = mat[r][c] + prefix[r][c + 1uz] +
                                   prefix[r + 1uz][c] - prefix[r][c];
      }
    }

    // Phase 2: answer each clamped window [r1, r2) x [c1, c2) in O(1).
    std::vector<std::vector<int>> answer(rows, std::vector<int>(cols, 0));
    for (std::size_t r = 0uz; r < rows; ++r) {
      const std::size_t r1 = r > radius ? (r - radius) : 0uz;
      const std::size_t r2 = std::min(rows, r + radius + 1uz);
      for (std::size_t c = 0uz; c < cols; ++c) {
        const std::size_t c1 = c > radius ? (c - radius) : 0uz;
        const std::size_t c2 = std::min(cols, c + radius + 1uz);
        answer[r][c] = prefix[r2][c2] - prefix[r1][c2] - prefix[r2][c1] +
                       prefix[r1][c1];
      }
    }

    return answer;
  }

  // Reference direct-summation approach matching the original Python solution.
  // Time: O(m * n * k^2), Space: O(1) auxiliary.
  std::vector<std::vector<int>> matrixBlockSumDirect(
      const std::vector<std::vector<int>>& mat, int k) {
    if (mat.empty() || mat[0].empty()) {
      return {};
    }

    const std::size_t rows = mat.size();
    const std::size_t cols = mat[0].size();
    const std::size_t radius = k > 0 ? static_cast<std::size_t>(k) : 0uz;
    std::vector<std::vector<int>> answer(rows, std::vector<int>(cols, 0));

    for (std::size_t r = 0uz; r < rows; ++r) {
      const std::size_t r1 = r > radius ? (r - radius) : 0uz;
      const std::size_t r2 = std::min(rows, r + radius + 1uz);
      for (std::size_t c = 0uz; c < cols; ++c) {
        const std::size_t c1 = c > radius ? (c - radius) : 0uz;
        const std::size_t c2 = std::min(cols, c + radius + 1uz);
        answer[r][c] = sumWindow(mat, r1, c1, r2, c2);
      }
    }

    return answer;
  }

 private:
  // Sums all elements in the half-open submatrix `[r1, r2) x [c1, c2)`.
  static int sumWindow(const std::vector<std::vector<int>>& mat,
                       std::size_t r1, std::size_t c1, std::size_t r2,
                       std::size_t c2) {
    int total = 0;
    for (std::size_t r = r1; r < r2; ++r) {
      for (std::size_t c = c1; c < c2; ++c) {
        total += mat[r][c];
      }
    }
    return total;
  }
};

LC_PROBLEM(1314, "Matrix Block Sum", "array", ctx) {
  Solution sol;

  ctx.test("example_1_k_1", [&] {
    const std::vector<std::vector<int>> mat = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9},
    };
    const std::vector<std::vector<int>> expected = {
        {12, 21, 16},
        {27, 45, 33},
        {24, 39, 28},
    };
    ctx.expect_eq(sol.matrixBlockSum(mat, 1), expected);
    ctx.expect_eq(sol.matrixBlockSumDirect(mat, 1), expected);
  });

  ctx.test("example_2_build_k_2", [&] {
    const std::vector<std::vector<int>> mat = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9},
    };
    const std::vector<std::vector<int>> expected = {
        {45, 45, 45},
        {45, 45, 45},
        {45, 45, 45},
    };
    ctx.expect_eq(sol.matrixBlockSum(mat, 2), expected);
    ctx.expect_eq(sol.matrixBlockSumDirect(mat, 2), expected);
  });

  ctx.test("rectangular_matrix", [&] {
    const std::vector<std::vector<int>> mat = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
    };
    const std::vector<std::vector<int>> expected = {
        {14, 24, 30, 22},
        {14, 24, 30, 22},
    };
    ctx.expect_eq(sol.matrixBlockSum(mat, 1), expected);
    ctx.expect_eq(sol.matrixBlockSumDirect(mat, 1), expected);
  });

  ctx.test("edge_cases_single_cell_and_zero_radius", [&] {
    const std::vector<std::vector<int>> single = {{7}};
    ctx.expect_eq(sol.matrixBlockSum(single, 5), single);
    ctx.expect_eq(sol.matrixBlockSumDirect(single, 5), single);

    const std::vector<std::vector<int>> mat = {{1, 2}, {3, 4}};
    ctx.expect_eq(sol.matrixBlockSum(mat, 0), mat);
    ctx.expect_eq(sol.matrixBlockSumDirect(mat, 0), mat);
  });
}

}  // namespace
