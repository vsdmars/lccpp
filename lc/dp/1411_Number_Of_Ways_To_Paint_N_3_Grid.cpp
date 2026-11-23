// LeetCode 1411: Number Of Ways To Paint N 3 Grid
// https://leetcode.com/problems/number-of-ways-to-paint-n-3-grid/
//
// Problem:
//   Given an `n x 3` grid (`1 <= n <= 5000`), paint every cell one of three
//   colors (Red, Yellow, Green) such that no two horizontally or vertically
//   adjacent cells share the same color. Return the total number of valid
//   colorings modulo `10^9 + 7`.
//
// Examples:
//   Example 1:
//     Input:  n = 1
//     Output: 12
//     Explanation:
//       - Initialize row 1 counts to abc = 6 (three-color patterns) and aba = 6
//         (two-color patterns).
//       - The transition loop for row = 1 to n - 1 executes zero iterations because
//         n = 1.
//       - Return (abc + aba) % 1000000007 = (6 + 6) % 1000000007 = 12.
//
//   Example 2:
//     Input:  n = 2
//     Output: 54
//     Explanation:
//       - Initialize row 1 counts to abc = 6 and aba = 6.
//       - In the single iteration row = 1, compute next_abc = (2 * 6 + 2 * 6) %
//         1000000007 = 24 and next_aba = (2 * 6 + 3 * 6) % 1000000007 = 30.
//       - Update abc = 24 and aba = 30, then return (24 + 30) % 1000000007 = 54.
//
//   Example 3:
//     Input:  n = 4
//     Output: 1122
//     Explanation:
//       - Initialize row 1 counts to abc = 6 and aba = 6.
//       - At row = 1, transition to abc = (2 * 6 + 2 * 6) = 24 and aba = (2 * 6 + 3
//         * 6) = 30.
//       - At row = 2, transition to abc = (2 * 24 + 2 * 30) = 108 and aba = (2 * 24
//         + 3 * 30) = 138.
//       - At row = 3, transition to abc = (2 * 108 + 2 * 138) = 492 and aba = (2 *
//         108 + 3 * 138) = 630, and return (492 + 630) % 1000000007 = 1122.
//
// Algorithm:
//   State-equivalence linear dynamic programming across rows. Any valid single
//   row of 3 cells belongs to one of two symmetry classes:
//     - `abc` (3 distinct colors, e.g., R-Y-G): `3 * 2 * 1 = 6` base patterns.
//     - `aba` (2 colors with matching outer cells, e.g., R-Y-R): `3 * 2 = 6`
//       base patterns.
//   Inspecting all vertically compatible next rows yields a fixed 2x2 linear
//   recurrence:
//     - Below an `ABC` row: 2 `ABC` rows (`BCA, CAB`) and 2 `ABA` rows
//       (`BAB, BCB`) are valid.
//     - Below an `ABA` row: 2 `ABC` rows (`BAC, CAB`) and 3 `ABA` rows
//       (`BAB, BCB, CAC`) are valid.
//   Hence for each subsequent row:
//     `next_abc = (2 * abc + 2 * aba) % (10^9 + 7)`
//     `next_aba = (2 * abc + 3 * aba) % (10^9 + 7)`
//   After `n - 1` transitions, the answer is `(abc + aba) % (10^9 + 7)`.
//   Time Complexity:  O(n) (can also be accelerated to O(log n) via 2x2 matrix
//                     exponentiation).
//   Space Complexity: O(1).
//
// Data Structures:
//   - Two 64-bit scalar DP state variables (`abc` and `aba`) holding the count
//     of valid prefix colorings ending in each row symmetry class.
//
// Real-World Applications:
//   - Transfer-matrix counting in statistical physics (Potts / chromatic
//     partition functions on strip lattices).
//   - Channel/frequency assignment and register allocation on fixed-width
//     interference graphs (bounded-treewidth graph coloring).
//   - Combinatorial tiling and constraint-satisfaction counting in VLSI layout.
//
// Related LeetCode Problems:
//   - #1931 Painting a Grid With Three Different Colors [new] (generalized m x n strip coloring via bitmask transfer DP)
//   - #276 Paint Fence [new] (1D adjacent-color constraint counting DP)
//   - #256 Paint House [done] (minimum-cost 3-color adjacent row DP)
//   - #790 Domino and Tromino Tiling [new] (fixed-width strip state-transition DP)

#include "framework/lc_framework.h"

#include <cstdint>

namespace {

class Solution {
 public:
  // Counts valid 3-colorings of an `n x 3` grid with no adjacent matching
  // colors, modulo 10^9 + 7.
  // Time: O(n), Space: O(1).
  int numOfWays(int n) {
    static constexpr std::int64_t kMod = 1'000'000'007;

    // Row 1 base counts: 6 three-color patterns (ABC) and 6 two-color patterns
    // (ABA).
    std::int64_t abc = 6;
    std::int64_t aba = 6;

    for (int row = 1; row < n; ++row) {
      // Transition from row `i - 1` to row `i`:
      //   ABC <- 2 from ABC + 2 from ABA
      //   ABA <- 2 from ABC + 3 from ABA
      const std::int64_t next_abc = ((2 * abc) + (2 * aba)) % kMod;
      const std::int64_t next_aba = ((2 * abc) + (3 * aba)) % kMod;
      abc = next_abc;
      aba = next_aba;
    }

    return static_cast<int>((abc + aba) % kMod);
  }
};

LC_PROBLEM(1411, "Number Of Ways To Paint N 3 Grid", "dp", ctx) {
  Solution sol;

  ctx.test("example_1_single_row", [&] {
    ctx.expect_eq(sol.numOfWays(1), 12);
  });

  ctx.test("example_2_two_rows", [&] {
    ctx.expect_eq(sol.numOfWays(2), 54);
  });

  ctx.test("example_3_python_build", [&] {
    ctx.expect_eq(sol.numOfWays(3), 246);
  });

  ctx.test("example_4_seven_rows", [&] {
    ctx.expect_eq(sol.numOfWays(7), 106'494);
  });

  ctx.test("example_5_max_constraint_modulo", [&] {
    ctx.expect_eq(sol.numOfWays(5000), 30'228'214);
  });

  ctx.test("four_rows_check", [&] {
    ctx.expect_eq(sol.numOfWays(4), 1122);
  });
}

}  // namespace
