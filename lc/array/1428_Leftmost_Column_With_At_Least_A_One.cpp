// LeetCode 1428: Leftmost Column With At Least A One
// https://leetcode.com/problems/leftmost-column-with-at-least-a-one/
//
// Problem:
//   (Interactive problem.) A row-sorted binary matrix contains only 0s and 1s,
//   and each individual row is sorted in non-decreasing order (all 0s precede
//   all 1s). Direct access to the matrix is prohibited; elements may only be
//   queried via a BinaryMatrix interface exposing `get(row, col)` and
//   `dimensions() -> [rows, cols]`. Return the 0-indexed leftmost column that
//   contains at least one 1, or -1 if the entire matrix contains only 0s.
//   Constraints: 1 <= rows, cols <= 100, and at most 1,000 calls to `get()`
//   are permitted.
//
// Examples:
//   Example 1:
//     Input:  mat = [[0, 0], [1, 1]]
//     Output: 0
//     Explanation:
//       - Retrieve dimensions {2, 2} and start the staircase pointer at top-right
//         corner (row = 0, col = 1).
//       - Query get(0, 1) == 0, so row 0 has no 1s at or left of column 1 and the
//         scan steps down to (1, 1).
//       - Query get(1, 1) == 1, so column 1 is a valid candidate and the scan steps
//         left to (1, 0).
//       - Query get(1, 0) == 1, stepping left to col = -1 to exit the loop and
//         return col + 1 = 0.
//
//   Example 2:
//     Input:  mat = [[0, 0, 0, 1], [0, 0, 1, 1], [0, 1, 1, 1]]
//     Output: 1
//     Explanation:
//       - Retrieve dimensions {3, 4} and begin at (row = 0, col = 3).
//       - Query get(0, 3) == 1 to step left to (0, 2), then query get(0, 2) == 0 to
//         step down to (1, 2).
//       - Query get(1, 2) == 1 to step left to (1, 1), then query get(1, 1) == 0 to
//         step down to (2, 1).
//       - Query get(2, 1) == 1 to step left to (2, 0), then query get(2, 0) == 0 to
//         step down to row = 3 and exit.
//       - Since col = 0 differs from cols - 1 = 3, return col + 1 = 1.
//
//   Example 3:
//     Input:  mat = [[0, 0], [0, 0]]
//     Output: -1
//     Explanation:
//       - Retrieve dimensions {2, 2} and initialize the pointer at (row = 0, col =
//         1).
//       - Query get(0, 1) == 0 to step down to (1, 1), then query get(1, 1) == 0 to
//         step down to row = 2 and end the loop.
//       - Because col stayed at cols - 1 = 1 without ever moving left, return -1.
//
// Algorithm:
//   Top-Right Staircase Scan (monotonic two-pointer walk):
//   Start at the top-right corner `(row = 0, col = cols - 1)`. Because every
//   row is sorted in non-decreasing order:
//   - If `get(row, col) == 1`, column `col` is a valid candidate and we can
//     safely advance left (`--col`) to search for an even smaller column index
//     without needing to inspect column `col` in any lower row.
//   - If `get(row, col) == 0`, no 1 can exist at or to the left of `col` in
//     this row, so we move down (`++row`) to the next row.
//   Stop as soon as `row == rows` (all rows exhausted) or `col < 0` (column 0
//   already contains a 1, which is globally optimal).
//   Every step either increments `row` or decrements `col`, so at most
//   `rows + cols` queries are made (<= 200 for 100 x 100, well within 1,000).
//   Time Complexity:  O(rows + cols)
//   Space Complexity: O(1)
//
// Data Structures:
//   - Two scalar integer pointers (`row`, `col`) tracking the frontier of the
//     staircase walk across the hidden matrix.
//   - Mock `BinaryMatrix` wrapper holding the backing 2D grid and a query
//     counter to verify compliance with the 1,000-call interactive limit.
//
// Real-World Applications:
//   - Earliest-activation detection across sorted time-series event matrices
//     or monotonic boolean feature flags stored behind rate-limited RPC APIs.
//   - Bisecting regression matrices (commits x test suites) where failures
//     persist monotonically after the first breaking commit per suite.
//   - Saddleback search in sorted 2D database index partitions and GPU
//     occupancy tables where direct random access is costly.
//
// Related LeetCode Problems:
//   - #240 Search a 2D Matrix II [done] (classic top-right staircase / saddleback search)
//   - #74 Search a 2D Matrix [done] (binary search on fully sorted 2D matrix)
//   - #1351 Count Negative Numbers in a Sorted Matrix [done] (same staircase elimination in O(m + n))
//   - #278 First Bad Version [done] (interactive query-budgeted monotonic boundary search)
//   - #1095 Find in Mountain Array [new] (interactive array API with strict call-count limit)

#include "framework/lc_framework.h"

#include <cstddef>
#include <utility>
#include <vector>

namespace {

// Mock implementation of the interactive LeetCode BinaryMatrix judge interface.
// Tracks the total number of `get()` calls so unit tests can verify adherence
// to the 1,000-call budget.
class BinaryMatrix {
 public:
  explicit BinaryMatrix(std::vector<std::vector<int>> mat)
      : mat_(std::move(mat)) {}

  // Returns the binary value at (row, col) and increments the query counter.
  [[nodiscard]] int get(int row, int col) {
    ++call_count_;
    return mat_[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)];
  }

  // Returns {rows, cols} describing the dimensions of the hidden matrix.
  [[nodiscard]] std::vector<int> dimensions() const {
    const int rows = static_cast<int>(mat_.size());
    const int cols = mat_.empty() ? 0 : static_cast<int>(mat_[0].size());
    return {rows, cols};
  }

  // Returns the number of `get()` invocations made so far.
  [[nodiscard]] int call_count() const noexcept { return call_count_; }

 private:
  std::vector<std::vector<int>> mat_;
  int call_count_ = 0;
};

class Solution {
 public:
  // Finds the leftmost column index containing at least one 1 using a
  // top-right staircase walk.
  //
  // Invariant: Before each check at (row, col), every column strictly greater
  // than `col` is known to contain at least one 1 (in some row <= `row`), and
  // no row strictly above `row` contains a 1 at or to the left of `col`.
  //
  // Time: O(rows + cols), Space: O(1).
  int leftMostColumnWithOne(BinaryMatrix& binary_matrix) {
    const std::vector<int> dims = binary_matrix.dimensions();
    const int rows = dims[0];
    const int cols = dims[1];

    // Guard against degenerate empty matrices.
    if (rows == 0 || cols == 0) {
      return -1;
    }

    // Begin at the top-right corner; `col` always points one position left of
    // the best (leftmost) column known to contain a 1 so far.
    int row = 0;
    int col = cols - 1;

    while (row < rows && col >= 0) {
      if (binary_matrix.get(row, col) == 1) {
        // Column `col` has a 1; try to improve the answer by stepping left.
        --col;
      } else {
        // Because the row is sorted non-decreasing, all entries to the left in
        // this row are also 0; discard this row and move down.
        ++row;
      }
    }

    // If `col` never moved left from `cols - 1`, no 1 existed in the matrix.
    return col == cols - 1 ? -1 : col + 1;
  }
};

LC_PROBLEM(1428, "Leftmost Column With At Least A One", "array", ctx) {
  ctx.test("example_1_bottom_row_all_ones", [&] {
    BinaryMatrix mat({{0, 0}, {1, 1}});
    Solution sol;
    ctx.expect_eq(sol.leftMostColumnWithOne(mat), 0);
    ctx.expect_true(mat.call_count() <= 4);
  });

  ctx.test("example_2_single_one_bottom_right", [&] {
    BinaryMatrix mat({{0, 0}, {0, 1}});
    Solution sol;
    ctx.expect_eq(sol.leftMostColumnWithOne(mat), 1);
  });

  ctx.test("example_3_all_zeros", [&] {
    BinaryMatrix mat({{0, 0}, {0, 0}});
    Solution sol;
    ctx.expect_eq(sol.leftMostColumnWithOne(mat), -1);
  });

  ctx.test("example_4_staircase_ones", [&] {
    BinaryMatrix mat({{0, 0, 0, 1}, {0, 0, 1, 1}, {0, 1, 1, 1}});
    Solution sol;
    ctx.expect_eq(sol.leftMostColumnWithOne(mat), 1);
  });

  ctx.test("single_cell_matrices", [&] {
    BinaryMatrix one({{1}});
    BinaryMatrix zero({{0}});
    Solution sol;
    ctx.expect_eq(sol.leftMostColumnWithOne(one), 0);
    ctx.expect_eq(sol.leftMostColumnWithOne(zero), -1);
  });

  ctx.test("max_size_100x100_within_query_budget", [&] {
    static constexpr std::size_t kDim = 100uz;
    static constexpr int kMaxAllowedCalls = 1000;
    std::vector<std::vector<int>> grid(kDim, std::vector<int>(kDim, 0));
    // Place the earliest 1 in the last row at column 37.
    for (std::size_t c = 37uz; c < kDim; ++c) {
      grid[kDim - 1uz][c] = 1;
    }
    BinaryMatrix mat(std::move(grid));
    Solution sol;
    ctx.expect_eq(sol.leftMostColumnWithOne(mat), 37);
    ctx.expect_true(mat.call_count() <= kMaxAllowedCalls);
  });
}

}  // namespace
