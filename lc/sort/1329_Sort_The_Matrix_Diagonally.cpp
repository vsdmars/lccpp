// LeetCode 1329: Sort The Matrix Diagonally
// https://leetcode.com/problems/sort-the-matrix-diagonally/
//
// Problem:
//   A matrix diagonal starts at any cell in the topmost row `(0, c)` or the
//   leftmost column `(r, 0)` and proceeds down-right `(r + 1, c + 1)` until it
//   exits the matrix. Given an `m x n` integer matrix `mat`, sort every matrix
//   diagonal independently in ascending order and return the resulting matrix.
//   Constraints: `1 <= m, n <= 100`, `1 <= mat[i][j] <= 100`.
//
// Examples:
//   Example 1:
//     Input:  mat = [[3, 3, 1, 1], [2, 2, 1, 2], [1, 1, 1, 2]]
//     Output: [[1, 1, 1, 1], [1, 2, 2, 2], [1, 2, 3, 3]]
//     Explanation:
//       - Phase 1 extracts and sorts diagonals starting on column 0: (0, 0), (1,
//         0), and (2, 0).
//       - The main diagonal from (0, 0) extracts [3, 2, 1] into buffer, sorts it to
//         [1, 2, 3], and writes it back to (0, 0), (1, 1), and (2, 2).
//       - Diagonal (1, 0) sorts [2, 1] to [1, 2] at (1, 0) and (2, 1), while (2, 0)
//         keeps its single element [1].
//       - Phase 2 sorts top-row diagonals from (0, 1), (0, 2), and (0, 3): [3, 1,
//         2] becomes [1, 2, 3], [1, 2] stays [1, 2], and [1] stays [1], yielding
//         [[1, 1, 1, 1], [1, 2, 2, 2], [1, 2, 3, 3]].
//
//   Example 2:
//     Input:  mat = [[9, 8, 7], [6, 5, 4], [3, 2, 1]]
//     Output: [[1, 4, 7], [2, 5, 8], [3, 6, 9]]
//     Explanation:
//       - Phase 1 processes left-column starts: [9, 5, 1] from (0, 0) sorts to [1,
//         5, 9], [6, 2] from (1, 0) sorts to [2, 6], and [3] at (2, 0) is
//         unchanged.
//       - Phase 2 processes remaining top-row starts: [8, 4] from (0, 1) sorts to
//         [4, 8], and [7] at (0, 2) is unchanged.
//       - Writing each sorted buffer back along its down-right path produces [[1,
//         4, 7], [2, 5, 8], [3, 6, 9]].
//
//   Example 3:
//     Input:  mat = [[5, 3, 1, 4, 2]]
//     Output: [[5, 3, 1, 4, 2]]
//     Explanation:
//       - Phase 1 runs only for r = 0 because rows = 1, extracting [5] from (0, 0)
//         before the diagonal exits the 1-row matrix.
//       - Phase 2 visits columns 1 through 4 on row 0, each extracting a 1-element
//         buffer ([3], [1], [4], [2]) that is already sorted.
//       - Writing each single-element buffer back leaves mat unchanged as [[5, 3,
//         1, 4, 2]].
//
// Algorithm:
//   Per-diagonal extract-sort-writeback:
//   Every main diagonal is uniquely identified by its starting cell on the top
//   row `(0, c)` (`1 <= c < n`) or left column `(r, 0)` (`0 <= r < m`). Notice
//   all cells on a given diagonal share the invariant `r - c = constant`.
//   For each diagonal start `(start_row, start_col)`:
//   1. Walk down-right, copying its elements into a reusable scratch buffer.
//   2. Sort the buffer in ascending order via `std::ranges::sort`.
//   3. Walk down-right again, writing the sorted values back into `mat`.
//   Time Complexity:  O(m * n * log(min(m, n))) — each of the `m + n - 1`
//                     diagonals has length at most `min(m, n)`.
//   Space Complexity: O(min(m, n)) auxiliary space for the single reused
//                     diagonal buffer (modifies `mat` in place).
//
// Data Structures:
//   - Reusable diagonal buffer (`std::vector<int>`): pre-reserved to
//     `min(m, n)` so all diagonals are sorted with a single heap allocation.
//
// Real-World Applications:
//   - Banded and Toeplitz/diagonal-storage linear algebra kernels (DIA sparse
//     matrix format) that normalize or compress entries along matrix diagonals.
//   - 2D image and signal processing filters operating independently along
//     slanted scanlines (e.g., shear-aligned median/order-statistic filters).
//   - Data-parallel sorting of independent diagonal stripes in dynamic
//     programming or finite-difference stencil grids.
//
// Related LeetCode Problems:
//   - #498 Diagonal Traverse [done] (visiting anti-diagonals grouped by `r + c`)
//   - #1424 Diagonal Traverse II [done] (anti-diagonal ordering on ragged matrices)
//   - #766 Toeplitz Matrix [done] (checking constant-value invariant along `r - c` diagonals)
//   - #2545 Sort the Students by Their Kth Score [new] (row-wise matrix sorting)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Sorts each top-left-to-bottom-right diagonal of `mat` in ascending order.
  //
  // Time: O(m * n * log(min(m, n))), Space: O(min(m, n)) auxiliary.
  std::vector<std::vector<int>> diagonalSort(
      std::vector<std::vector<int>> mat) {
    if (mat.empty() || mat[0].empty()) {
      return mat;
    }

    const std::size_t rows = mat.size();
    const std::size_t cols = mat[0].size();

    // Reuse a single buffer across all diagonals to avoid repeated allocations.
    std::vector<int> buffer;
    buffer.reserve(std::min(rows, cols));

    // Phase 1: Sort all diagonals starting on the leftmost column (including
    // the main diagonal starting at `(0, 0)`).
    for (std::size_t r = 0; r < rows; ++r) {
      sort_single_diagonal(mat, r, 0, buffer);
    }

    // Phase 2: Sort all remaining diagonals starting on the topmost row
    // (`c >= 1`, since `(0, 0)` was already sorted in Phase 1).
    for (std::size_t c = 1; c < cols; ++c) {
      sort_single_diagonal(mat, 0, c, buffer);
    }

    return mat;
  }

 private:
  // Extracts the diagonal beginning at `(start_row, start_col)`, sorts it in
  // ascending order, and writes the sorted values back in place.
  static void sort_single_diagonal(std::vector<std::vector<int>>& mat,
                                   std::size_t start_row, std::size_t start_col,
                                   std::vector<int>& buffer) {
    const std::size_t rows = mat.size();
    const std::size_t cols = mat[0].size();

    buffer.clear();
    for (std::size_t r = start_row, c = start_col; r < rows && c < cols;
         ++r, ++c) {
      buffer.push_back(mat[r][c]);
    }

    std::ranges::sort(buffer);

    std::size_t idx = 0;
    for (std::size_t r = start_row, c = start_col; r < rows && c < cols;
         ++r, ++c) {
      mat[r][c] = buffer[idx];
      ++idx;
    }
  }
};

LC_PROBLEM(1329, "Sort The Matrix Diagonally", "sort", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    const std::vector<std::vector<int>> input = {
        {3, 3, 1, 1}, {2, 2, 1, 2}, {1, 1, 1, 2}};
    const std::vector<std::vector<int>> expected = {
        {1, 1, 1, 1}, {1, 2, 2, 2}, {1, 2, 3, 3}};
    ctx.expect_eq(sol.diagonalSort(input), expected);
  });

  ctx.test("example_2_python_build", [&] {
    const std::vector<std::vector<int>> input = {
        {11, 25, 66, 1, 69, 7},
        {23, 55, 17, 45, 15, 52},
        {75, 31, 36, 44, 58, 8},
        {22, 27, 33, 25, 68, 4},
        {84, 28, 14, 11, 5, 50}};
    const std::vector<std::vector<int>> expected = {
        {5, 17, 4, 1, 52, 7},
        {11, 11, 25, 45, 8, 69},
        {14, 23, 25, 44, 58, 15},
        {22, 27, 31, 36, 50, 66},
        {84, 28, 75, 33, 55, 68}};
    ctx.expect_eq(sol.diagonalSort(input), expected);
  });

  ctx.test("square_descending", [&] {
    const std::vector<std::vector<int>> input = {
        {9, 8, 7}, {6, 5, 4}, {3, 2, 1}};
    const std::vector<std::vector<int>> expected = {
        {1, 4, 7}, {2, 5, 8}, {3, 6, 9}};
    ctx.expect_eq(sol.diagonalSort(input), expected);
  });

  ctx.test("single_row", [&] {
    const std::vector<std::vector<int>> input = {{5, 3, 1, 4, 2}};
    ctx.expect_eq(sol.diagonalSort(input), input);
  });

  ctx.test("single_column", [&] {
    const std::vector<std::vector<int>> input = {{5}, {3}, {1}};
    ctx.expect_eq(sol.diagonalSort(input), input);
  });

  ctx.test("single_element", [&] {
    const std::vector<std::vector<int>> input = {{42}};
    ctx.expect_eq(sol.diagonalSort(input), input);
  });
}

}  // namespace
