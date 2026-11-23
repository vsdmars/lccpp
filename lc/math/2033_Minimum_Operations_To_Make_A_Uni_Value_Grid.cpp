// LeetCode 2033: Minimum Operations To Make A Uni Value Grid
// https://leetcode.com/problems/minimum-operations-to-make-a-uni-value-grid/
//
// Problem:
//   Given an `m x n` integer grid and a positive step `x`, one operation adds
//   or subtracts `x` from any single cell. Return the minimum number of
//   operations needed to make all cells equal (a uni-value grid), or `-1` if
//   no sequence of operations can achieve this.
//   Constraints: 1 <= m, n <= 10^5, 1 <= m * n <= 10^5,
//                1 <= x, grid[i][j] <= 10^4.
//
// Examples:
//   Example 1:
//     Input:  grid = [[2, 4], [6, 8]], x = 2
//     Output: 4
//     Explanation:
//       - Compute base_remainder = grid[0][0] % 2 = 0 and flatten the grid into
//         vals = [2, 4, 6, 8], verifying every element has remainder 0 modulo 2.
//       - Compute mid = 4 / 2 = 2 and partition vals with std::ranges::nth_element
//         so vals[2] holds median = 6.
//       - Accumulate std::abs(val - 6) / 2 across [2, 4, 6, 8]: 4/2 + 2/2 + 0/2 +
//         2/2 = 2 + 1 + 0 + 1 = 4 operations.
//
//   Example 2:
//     Input:  grid = [[1, 2], [3, 4]], x = 2
//     Output: -1
//     Explanation:
//       - Compute base_remainder = grid[0][0] % 2 = 1 and append 1 to vals.
//       - Inspect grid[0][1] = 2 and find 2 % 2 = 0 != 1, returning -1 immediately
//         because cells cannot share a target value via +/- 2 steps.
//
//   Example 3:
//     Input:  grid = [[1, 4, 7]], x = 3
//     Output: 2
//     Explanation:
//       - Set base_remainder = 1 % 3 = 1 and flatten the row into vals = [1, 4, 7],
//         since 4 % 3 = 1 and 7 % 3 = 1.
//       - With mid = 3 / 2 = 1, std::ranges::nth_element places median = 4 at
//         vals[1].
//       - Sum std::abs(val - 4) / 3 over [1, 4, 7]: 3/3 + 0/3 + 3/3 = 1 + 0 + 1 = 2
//         operations.
//
// Algorithm:
//   1. Feasibility via modular congruence: adding or subtracting `x` never
//      changes `val % x`. Thus all grid values can converge to a common target
//      if and only if every cell shares the same remainder modulo `x`.
//   2. Optimal target via median selection: once feasible, the number of
//      operations to move every cell `v` to target `T` is
//      `sum(|v - T| / x) = (1 / x) * sum(|v - T|)`. The sum of L1 distances
//      is minimized when `T` is the median of all flattened grid values.
//   3. Instead of a full O(N log N) sort, we partition around the median index
//      using `std::ranges::nth_element` (Introselect / Quickselect) in average
//      O(N) time, then accumulate `|v - median| / x`.
//   Time Complexity:  O(m * n) average time via `std::ranges::nth_element`.
//   Space Complexity: O(m * n) for the flattened value buffer.
//
// Data Structures:
//   - `std::vector<int>`: 1D flattened copy of the `m x n` grid used for
//     in-place median selection.
//
// Real-World Applications:
//   - Clock/frequency synchronization and quantization where nodes adjust
//     state in fixed discrete quanta.
//   - Load balancing across servers with fixed-size task units to minimize
//     total migrations.
//   - L1-norm / median-based robust consensus in distributed sensor networks.
//
// Related LeetCode Problems:
//   - #462 Minimum Moves to Equal Array Elements II [new] (same L1 median problem with x = 1)
//   - #453 Minimum Moves to Equal Array Elements [done] (incrementing n - 1 elements reduces to subtracting 1)
//   - #1515 Best Position For A Service Centre [done] (2D Euclidean generalization of median minimization)
//   - #215 Kth Largest Element in an Array [done] (linear-time selection via nth_element / quickselect)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <iterator>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the minimum +/- x operations to equalize every cell in `grid`,
  // or -1 if cells fall into different residue classes modulo `x`.
  // Time: O(m * n) average, Space: O(m * n).
  int minOperations(const std::vector<std::vector<int>>& grid, int x) {
    if (grid.empty() || grid[0].empty()) {
      return 0;
    }

    const std::size_t rows = grid.size();
    const std::size_t cols = grid[0].size();
    const int base_remainder = grid[0][0] % x;

    // Flatten the grid while verifying that every value shares the same
    // remainder modulo `x` (necessary and sufficient for reachability).
    std::vector<int> vals;
    vals.reserve(rows * cols);
    for (const auto& row : grid) {
      for (const int val : row) {
        if ((val % x) != base_remainder) {
          return -1;
        }
        vals.push_back(val);
      }
    }

    // Partition in average linear time to locate the median, which minimizes
    // the total L1 distance sum(|v - target|).
    const std::size_t mid = vals.size() / 2uz;
    std::ranges::nth_element(
        vals, std::next(vals.begin(), static_cast<std::ptrdiff_t>(mid)));
    const int median = vals[mid];

    int operations = 0;
    for (const int val : vals) {
      operations += std::abs(val - median) / x;
    }

    return operations;
  }
};

LC_PROBLEM(2033, "Minimum Operations To Make A Uni Value Grid", "math", ctx) {
  ctx.test("example_1_even_step", [&] {
    Solution sol;
    ctx.expect_eq(sol.minOperations({{2, 4}, {6, 8}}, 2), 4);
  });

  ctx.test("example_2_build_unit_step", [&] {
    Solution sol;
    ctx.expect_eq(sol.minOperations({{1, 5}, {2, 3}}, 1), 5);
  });

  ctx.test("example_3_impossible_remainders", [&] {
    Solution sol;
    ctx.expect_eq(sol.minOperations({{1, 2}, {3, 4}}, 2), -1);
  });

  ctx.test("single_cell", [&] {
    Solution sol;
    ctx.expect_eq(sol.minOperations({{42}}, 7), 0);
  });

  ctx.test("already_uniform", [&] {
    Solution sol;
    ctx.expect_eq(sol.minOperations({{5, 5}, {5, 5}}, 3), 0);
  });

  ctx.test("odd_element_count", [&] {
    Solution sol;
    // Median is 4; moving 1 -> 4 and 7 -> 4 takes 1 + 1 = 2 steps of size 3.
    ctx.expect_eq(sol.minOperations({{1, 4, 7}}, 3), 2);
  });
}

}  // namespace
