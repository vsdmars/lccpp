// LeetCode 62: Unique Paths
// https://leetcode.com/problems/unique-paths/
//
// Problem:
//   A robot starts at the top-left corner (0, 0) of an m x n grid (1 <= m, n
//   <= 100) and can only move either down or right at each step. Return the
//   total number of distinct paths to reach the bottom-right corner
//   (m - 1, n - 1). The answer is guaranteed to fit within a 32-bit signed int
//   (<= 2 * 10^9).
//
// Algorithm:
//   Bottom-up dynamic programming with 1D rolling-row space optimization.
//   In the full 2D formulation, dp[r][c] = dp[r - 1][c] + dp[r][c - 1] with
//   the first row and first column all equal to 1 (only one straight path
//   exists along each border). Because each row depends solely on the previous
//   row and the cell immediately to its left, we collapse the 2D table into a
//   single vector of length min(m, n) initialized to 1s: updating
//   dp[c] += dp[c - 1] in-place left-to-right adds the top neighbor (old dp[c])
//   and the left neighbor (new dp[c - 1]).
//   Time Complexity:  O(m * n).
//   Space Complexity: O(min(m, n)).
//
// Data Structures:
//   - std::vector<int> dp: 1D rolling DP array of size min(m, n) storing path
//     counts for the current grid row.
//
// Real-World Applications:
//   - Lattice-path combinatorics and binomial-coefficient computation in
//     probability, queuing theory, and statistical testing.
//   - Manhattan-grid route enumeration in VLSI wire-routing and warehouse
//     robotics path planning.
//   - Sequence-alignment DAG path counting in bioinformatics.
//
// Related LeetCode Problems:
//   - #63 Unique Paths II [done] (adds blocked obstacle cells to the grid)
//   - #64 Minimum Path Sum [done] (replaces path counting with min weighted sum)
//   - #70 Climbing Stairs [done] (1D additive DP recurrence)
//   - #980 Unique Paths III [new] (Hamiltonian path visiting every empty cell)

#include "framework/lc_framework.h"

#include <cstddef>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Counts unique right/down paths across an `m x n` grid using a 1D rolling
  // bottom-up DP table.
  // Time: O(m * n), Space: O(min(m, n)).
  int uniquePaths(int m, int n) {
    if (m <= 0 || n <= 0) {
      return 0;
    }

    // By grid symmetry, swapping m and n keeps the path count unchanged while
    // minimizing the rolling-row allocation to O(min(m, n)).
    if (m < n) {
      std::swap(m, n);
    }

    const auto rows = static_cast<std::size_t>(m);
    const auto cols = static_cast<std::size_t>(n);

    // Row 0 has exactly 1 path to every column (all Right moves).
    std::vector<int> dp(cols, 1);

    for (std::size_t r = 1uz; r < rows; ++r) {
      // Column 0 always stays 1 (all Down moves).
      for (std::size_t c = 1uz; c < cols; ++c) {
        // Before update: dp[c] holds paths from above (r - 1, c);
        // dp[c - 1] already holds paths from the left (r, c - 1).
        dp[c] += dp[c - 1];
      }
    }

    return dp[cols - 1];
  }
};

LC_PROBLEM(62, "Unique Paths", "dp", ctx) {
  Solution sol;

  ctx.test("example_1_and_3_symmetry", [&] {
    ctx.expect_eq(sol.uniquePaths(3, 7), 28);
    ctx.expect_eq(sol.uniquePaths(7, 3), 28);
  });

  ctx.test("example_2", [&] {
    ctx.expect_eq(sol.uniquePaths(3, 2), 3);
  });

  ctx.test("example_4_square_grid", [&] {
    // Matches the Python build() test case.
    ctx.expect_eq(sol.uniquePaths(3, 3), 6);
  });

  ctx.test("degenerate_single_row_or_col", [&] {
    ctx.expect_eq(sol.uniquePaths(1, 1), 1);
    ctx.expect_eq(sol.uniquePaths(1, 10), 1);
    ctx.expect_eq(sol.uniquePaths(10, 1), 1);
  });

  ctx.test("larger_grid", [&] {
    // C(18, 9) = 48620.
    ctx.expect_eq(sol.uniquePaths(10, 10), 48620);
  });
}

}  // namespace
