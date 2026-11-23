// LeetCode 118: Pascals Triangle
// https://leetcode.com/problems/pascals-triangle/
//
// Problem:
//   Given a non-negative integer `numRows`, generate and return the first
//   `numRows` rows of Pascal's triangle, where row `r` (0-indexed) has length
//   `r + 1`, begins and ends with 1, and every interior entry is the sum of
//   the two entries directly above it in row `r - 1`.
//
// Algorithm:
//   Bottom-up dynamic programming using Pascal's identity:
//     C(r, 0) = C(r, r) = 1
//     C(r, c) = C(r - 1, c - 1) + C(r - 1, c)   for 0 < c < r.
//   For each row index `r` from `0` to `numRows - 1`, allocate a row of size
//   `r + 1` initialized to `1`, then compute each interior element directly
//   from the previous row `triangle[r - 1]`. This avoids the temporary
//   zero-padded copies and recursion overhead of the Python version while
//   preserving the exact row-by-row recurrence.
//   Time complexity: O(numRows^2) — total number of entries is
//     numRows * (numRows + 1) / 2.
//   Space complexity: O(numRows^2) for the output triangle (O(1) auxiliary
//     space beyond the returned structure).
//
// Data Structures:
//   - 2D jagged vector (`std::vector<std::vector<int>>`): stores rows 0 through
//     `numRows - 1`, with row `r` serving as the DP state table for row `r + 1`.
//
// Real-World Applications:
//   - Combinatorics and probability engines: precomputing binomial
//     coefficients C(n, k) without factorial overflow for binomial
//     distributions and A/B test p-value calculations.
//   - Computer graphics: evaluating Bernstein polynomials for Bezier curves
//     and spline surfaces.
//   - Digital signal processing: constructing binomial filter kernels (discrete
//     approximations to Gaussian blur kernels in image processing).
//
// Related LeetCode Problems:
//   - #119 Pascal's Triangle II [new] (returns only the k-th row using O(k) space)
//   - #120 Triangle [done] (minimum path sum over a triangular 2D array)
//   - #62 Unique Paths [done] (grid path counts are shifted Pascal's triangle entries)
//   - #2221 Find Triangular Sum of an Array [new] (reverse Pascal reduction mod 10)

#include "framework/lc_framework.h"

#include <cstddef>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Generates the first `num_rows` rows of Pascal's triangle iteratively.
  //
  // Invariant: row `r` contains the binomial coefficients C(r, 0..r).
  //
  // Time: O(num_rows^2), Space: O(num_rows^2) for the output.
  std::vector<std::vector<int>> generate(int num_rows) {
    if (num_rows <= 0) {
      return {};
    }

    const auto rows = static_cast<std::size_t>(num_rows);
    std::vector<std::vector<int>> triangle;
    triangle.reserve(rows);

    for (std::size_t r = 0uz; r < rows; ++r) {
      // Every row starts and ends with 1; initializing all entries to 1
      // handles both boundary elements automatically.
      std::vector<int> row(r + 1uz, 1);

      // Fill interior entries using Pascal's recurrence from the previous row.
      for (std::size_t c = 1uz; c < r; ++c) {
        row[c] = triangle[r - 1uz][c - 1uz] + triangle[r - 1uz][c];
      }

      triangle.push_back(std::move(row));
    }

    return triangle;
  }
};

LC_PROBLEM(118, "Pascals Triangle", "array", ctx) {
  Solution sol;

  ctx.test("example_5_rows", [&] {
    const std::vector<std::vector<int>> expected{
        {1},
        {1, 1},
        {1, 2, 1},
        {1, 3, 3, 1},
        {1, 4, 6, 4, 1},
    };
    ctx.expect_eq(sol.generate(5), expected);
  });

  ctx.test("zero_and_one_row", [&] {
    ctx.expect_true(sol.generate(0).empty());
    ctx.expect_eq(sol.generate(1), std::vector<std::vector<int>>{{1}});
  });

  ctx.test("two_and_six_rows", [&] {
    ctx.expect_eq(sol.generate(2), std::vector<std::vector<int>>{{1}, {1, 1}});

    const auto six = sol.generate(6);
    ctx.expect_eq(six.size(), 6uz);
    ctx.expect_eq(six.back(), std::vector<int>{1, 5, 10, 10, 5, 1});
  });
}

}  // namespace
