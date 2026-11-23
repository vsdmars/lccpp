// LeetCode 311: Sparse Matrix Multiplication
// https://leetcode.com/problems/sparse-matrix-multiplication/
//
// Problem:
//   Given two sparse 2D integer matrices `mat1` of dimensions `m x k` and
//   `mat2` of dimensions `k x n`, return their matrix product `mat1 * mat2` of
//   dimensions `m x n`. Most entries in both input matrices are zero.
//
// Examples:
//   Example 1:
//     Input:  mat1 = [[1, 0, 0], [-1, 0, 3]], mat2 = [[7, 0, 0], [0, 0, 0], [0, 0,
//             1]]
//     Output: [[7, 0, 0], [-7, 0, 3]]
//     Explanation:
//       - compressRows builds sparse_a = [[{col: 0, val: 1}], [{col: 0, val: -1},
//         {col: 2, val: 3}]] and sparse_b = [[{col: 0, val: 7}], [], [{col: 2, val:
//         1}]].
//       - For row i = 0, {0, 1} pairs with sparse_b[0] ({0, 7}), setting
//         result[0][0] = 1 * 7 = 7.
//       - For row i = 1, {0, -1} pairs with sparse_b[0] ({0, 7}) to add -7 to
//         result[1][0], and {2, 3} pairs with sparse_b[2] ({2, 1}) to add 3 to
//         result[1][2], skipping empty sparse_b[1] and returning [[7, 0, 0], [-7,
//         0, 3]].
//
//   Example 2:
//     Input:  mat1 = [[1, -5], [0, 2], [4, 0]], mat2 = [[12, 0, -1], [-1, 3, 0]]
//     Output: [[17, -15, -1], [-2, 6, 0], [48, 0, -4]]
//     Explanation:
//       - compressRows extracts sparse_a = [[{0, 1}, {1, -5}], [{1, 2}], [{0, 4}]]
//         and sparse_b = [[{0, 12}, {2, -1}], [{0, -1}, {1, 3}]] into a 3x3
//         zero-initialized result.
//       - For i = 0, {0, 1} adds [12, 0, -1] and {1, -5} adds [5, -15, 0] into
//         result[0], accumulating to [17, -15, -1].
//       - For i = 1, {1, 2} scales sparse_b[1] to produce [-2, 6, 0], and for i =
//         2, {0, 4} scales sparse_b[0] to produce [48, 0, -4].
//
//   Example 3:
//     Input:  mat1 = [[0]], mat2 = [[0]]
//     Output: [[0]]
//     Explanation:
//       - compressRows produces empty entry lists [[]] for both mat1 and mat2.
//       - With sparse_a[0] empty, no multiplications occur and the zero-initialized
//         1x1 matrix [[0]] is returned.
//
// Algorithm:
//   Compress both matrices into row-wise lists of non-zero `(column, value)`
//   entries (Compressed Sparse Row / list-of-lists format). Then compute the
//   product using row-by-row accumulation:
//     For each row `i` of `mat1`:
//       For each non-zero `(k, a_val)` in row `i` of `mat1`:
//         For each non-zero `(j, b_val)` in row `k` of `mat2`:
//           `result[i][j] += a_val * b_val`
//   This completely skips every all-zero row of `mat1`, every zero entry in
//   `mat1`, and every zero entry in `mat2`, performing multiplications solely
//   when both factors are non-zero.
//   Time Complexity: O(m*k + k*n + sum_k nnz(mat1[:, k]) * nnz(mat2[k, :])),
//                    which is vastly faster than O(m*k*n) on sparse inputs.
//   Space Complexity: O(nnz(mat1) + nnz(mat2)) auxiliary space (plus O(m*n)
//                     for the dense output matrix).
//
// Data Structures:
//   - `std::vector<std::vector<SparseEntry>>`: row-indexed sparse
//     representation storing only non-zero `(col, val)` pairs for each matrix.
//   - `std::vector<std::vector<int>>`: zero-initialized dense output matrix.
//
// Real-World Applications:
//   - Graph analytics and PageRank / GNN message passing via sparse adjacency
//     matrix multiplication (SpGEMM).
//   - Finite-element PDE solvers and circuit simulators (SPICE) operating on
//     large sparse stiffness/conductance matrices.
//   - Recommendation systems and NLP TF-IDF cosine-similarity pipelines where
//     user-item or document-term matrices are >99% zeros.
//
// Related LeetCode Problems:
//   - #1570 Dot Product of Two Sparse Vectors [done] (1D sparse inner product via compressed pairs)
//   - #48 Rotate Image [done] (in-place 2D matrix coordinate transformation)
//   - #54 Spiral Matrix [done] (structured 2D matrix traversal)
//   - #73 Set Matrix Zeroes [done] (exploiting sparsity/zero markers in matrices)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

// Represents a single non-zero element inside a compressed matrix row.
struct SparseEntry {
  std::size_t col = 0uz;
  int val = 0;
};

class Solution {
 public:
  // Multiplies sparse `m x k` matrix `mat1` by sparse `k x n` matrix `mat2`.
  // Time: O(m*k + k*n + non-zero matched products),
  // Space: O(nnz(mat1) + nnz(mat2)) auxiliary.
  std::vector<std::vector<int>> multiply(
      const std::vector<std::vector<int>>& mat1,
      const std::vector<std::vector<int>>& mat2) {
    if (mat1.empty() || mat2.empty() || mat1[0].empty() || mat2[0].empty()) {
      return {};
    }

    const std::size_t rows_a = mat1.size();
    const std::size_t cols_b = mat2[0].size();

    // Compress both matrices so zero entries are never visited during
    // multiplication.
    const std::vector<std::vector<SparseEntry>> sparse_a = compressRows(mat1);
    const std::vector<std::vector<SparseEntry>> sparse_b = compressRows(mat2);

    std::vector<std::vector<int>> result(rows_a, std::vector<int>(cols_b, 0));

    // Accumulate outer contributions: each non-zero A[i][k] scales the sparse
    // row B[k][*] directly into result[i][*].
    for (std::size_t i = 0uz; i < rows_a; ++i) {
      for (const SparseEntry& entry_a : sparse_a[i]) {
        for (const SparseEntry& entry_b : sparse_b[entry_a.col]) {
          result[i][entry_b.col] += entry_a.val * entry_b.val;
        }
      }
    }

    return result;
  }

 private:
  // Extracts non-zero `(col, val)` entries for every row of `mat`.
  [[nodiscard]] static std::vector<std::vector<SparseEntry>> compressRows(
      const std::vector<std::vector<int>>& mat) {
    std::vector<std::vector<SparseEntry>> compressed(mat.size());
    for (std::size_t r = 0uz; r < mat.size(); ++r) {
      for (std::size_t c = 0uz; c < mat[r].size(); ++c) {
        if (mat[r][c] != 0) {
          compressed[r].push_back(SparseEntry{.col = c, .val = mat[r][c]});
        }
      }
    }
    return compressed;
  }
};

LC_PROBLEM(311, "Sparse Matrix Multiplication", "hash", ctx) {
  ctx.test("leetcode_example", [&] {
    Solution sol;
    const std::vector<std::vector<int>> mat1 = {{1, 0, 0}, {-1, 0, 3}};
    const std::vector<std::vector<int>> mat2 = {
        {7, 0, 0}, {0, 0, 0}, {0, 0, 1}};
    const std::vector<std::vector<int>> expected = {{7, 0, 0}, {-7, 0, 3}};
    ctx.expect_eq(sol.multiply(mat1, mat2), expected);
  });

  ctx.test("with_all_zero_row", [&] {
    Solution sol;
    const std::vector<std::vector<int>> mat1 = {
        {1, 0, 0}, {0, 0, 0}, {-1, 0, 3}};
    const std::vector<std::vector<int>> mat2 = {
        {7, 0, 0}, {0, 0, 0}, {0, 0, 1}};
    const std::vector<std::vector<int>> expected = {
        {7, 0, 0}, {0, 0, 0}, {-7, 0, 3}};
    ctx.expect_eq(sol.multiply(mat1, mat2), expected);
  });

  ctx.test("single_element_zero_and_nonzero", [&] {
    Solution sol;
    ctx.expect_eq(sol.multiply({{0}}, {{0}}),
                  std::vector<std::vector<int>>{{0}});
    ctx.expect_eq(sol.multiply({{3}}, {{-4}}),
                  std::vector<std::vector<int>>{{-12}});
  });

  ctx.test("rectangular_dense_overlap", [&] {
    Solution sol;
    const std::vector<std::vector<int>> mat1 = {{1, -5}, {0, 2}, {4, 0}};
    const std::vector<std::vector<int>> mat2 = {{12, 0, -1}, {-1, 3, 0}};
    const std::vector<std::vector<int>> expected = {
        {17, -15, -1}, {-2, 6, 0}, {48, 0, -4}};
    ctx.expect_eq(sol.multiply(mat1, mat2), expected);
  });
}

}  // namespace
