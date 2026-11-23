// LeetCode 265: Paint House II
// https://leetcode.com/problems/paint-house-ii/
//
// Problem:
//   There is a row of n houses, each of which can be painted with one of k
//   colors. The cost of painting house i with color j is `costs[i][j]`. Paint
//   all n houses such that no two adjacent houses share the same color, and
//   return the minimum total cost. Follow-up: solve in O(n * k) time.
//
// Examples:
//   Example 1:
//     Input:  costs = [[1, 5, 3], [2, 9, 4]]
//     Output: 5
//     Explanation:
//       - Initialize prev_min1 = 0, prev_min2 = 0, and prev_min1_color = kNoColor.
//       - On house 0 ([1, 5, 3]), all colors add prev_best = 0 to produce totals
//         [1, 5, 3], updating prev_min1 = 1, prev_min2 = 3, and prev_min1_color =
//         0.
//       - On house 1 ([2, 9, 4]), color 0 matches prev_min1_color = 0 and uses
//         prev_min2 = 3 (total 2 + 3 = 5), while colors 1 and 2 use prev_min1 = 1
//         (totals 10 and 5).
//       - The row summary updates prev_min1 = 5 and prev_min2 = 5, and minCostII
//         returns 5.
//
//   Example 2:
//     Input:  costs = [[2, 2, 9], [1, 8, 9]]
//     Output: 3
//     Explanation:
//       - On house 0 ([2, 2, 9]), color 0 sets curr_min1 = 2 (curr_min1_color = 0),
//         tied color 1 sets curr_min2 = 2, and color 2 gives 9, leaving prev_min1 =
//         2, prev_min2 = 2, and prev_min1_color = 0.
//       - On house 1 ([1, 8, 9]), color 0 matches prev_min1_color = 0 and falls
//         back to prev_min2 = 2 for total 1 + 2 = 3, while colors 1 and 2 yield 10
//         and 11.
//       - Updating prev_min1 to 3 completes the loop and returns 3.
//
//   Example 3:
//     Input:  costs = [[8]]
//     Output: 8
//     Explanation:
//       - With num_colors = 1, start with prev_min1 = 0, prev_min2 = 0, and
//         prev_min1_color = kNoColor.
//       - On house 0 ([8]), color 0 differs from kNoColor and uses prev_best =
//         prev_min1 = 0, giving total = 8 + 0 = 8 and setting prev_min1 = 8 before
//         returning 8.
//
// Algorithm:
//   Bottom-up dynamic programming optimized to O(n * k) time and O(1) space.
//   Naively computing dp[i][j] = costs[i][j] + min_{c != j} dp[i - 1][c] takes
//   O(n * k^2) time. However, for any color j on row i, the minimum over all
//   c != j in row i - 1 is almost always the global row minimum `prev_min1`,
//   EXCEPT when j equals the unique color index `prev_min1_color` that achieved
//   `prev_min1`—in which case it must use the second smallest value `prev_min2`
//   (which naturally handles duplicate minima when two colors tie for min1).
//   Tracking only `(prev_min1, prev_min2, prev_min1_color)` across rows computes
//   every transition in O(1) without mutating the input or allocating a DP row.
//   Time Complexity:  O(n * k).
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   Three rolling scalar summary variables (`prev_min1`, `prev_min2`, and
//   `prev_min1_color`) compressing an entire O(k) DP row into O(1) state.
//
// Real-World Applications:
//   - Large-state-space Viterbi decoding where all non-self transitions share
//     uniform penalty (reducing O(T * S^2) DP to O(T * S)).
//   - Cloud server / microservice failover assignment across k availability
//     zones so consecutive pipeline stages never co-locate on the same zone.
//   - Multi-material scheduling in industrial batch manufacturing with
//     no-consecutive-repeat constraints.
//
// Related LeetCode Problems:
//   - #256 Paint House [done] (fixed k = 3 special case)
//   - #1289 Minimum Falling Path Sum II [new] (identical O(n * k) min1/min2 DP on a grid)
//   - #1473 Paint House III [new] (adds neighborhood target constraint to house painting)
//   - #198 House Robber [done] (foundational adjacent-constraint DP)

#include "framework/lc_framework.h"

#include <cstddef>
#include <limits>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the minimum total cost to paint n houses with k colors so that no
  // two adjacent houses share a color.
  //
  // Invariant: Before processing house i, `prev_min1` and `prev_min2` are the
  // smallest and second-smallest cumulative costs achievable for houses
  // 0..(i - 1), and `prev_min1_color` is the ending color achieving `prev_min1`
  // (or `kNoColor` before house 0).
  //
  // Time: O(n * k), Space: O(1).
  int minCostII(const std::vector<std::vector<int>>& costs) {
    if (costs.empty() || costs[0].empty()) {
      return 0;
    }

    static constexpr std::size_t kNoColor =
        std::numeric_limits<std::size_t>::max();
    static constexpr int kInf = std::numeric_limits<int>::max();

    const std::size_t num_colors = costs[0].size();
    int prev_min1 = 0;
    int prev_min2 = 0;
    std::size_t prev_min1_color = kNoColor;

    for (const auto& house_costs : costs) {
      int curr_min1 = kInf;
      int curr_min2 = kInf;
      std::size_t curr_min1_color = kNoColor;

      for (std::size_t color = 0uz; color < num_colors; ++color) {
        // Avoid repeating the previous house's best color by falling back to
        // its second-best cumulative cost.
        const int prev_best =
            (color == prev_min1_color) ? prev_min2 : prev_min1;
        const int total = house_costs[color] + prev_best;

        // Maintain the smallest and second-smallest totals for the current row.
        if (total < curr_min1) {
          curr_min2 = curr_min1;
          curr_min1 = total;
          curr_min1_color = color;
        } else if (total < curr_min2) {
          curr_min2 = total;
        }
      }

      prev_min1 = curr_min1;
      prev_min2 = curr_min2;
      prev_min1_color = curr_min1_color;
    }

    return prev_min1;
  }
};

LC_PROBLEM(265, "Paint House II", "dp", ctx) {
  Solution sol;

  ctx.test("leetcode_examples", [&] {
    ctx.expect_eq(sol.minCostII({{1, 5, 3}, {2, 9, 4}}), 5);
    ctx.expect_eq(sol.minCostII({{1, 3}, {2, 4}}), 5);
  });

  ctx.test("source_build_case", [&] {
    ctx.expect_eq(sol.minCostII({{7, 6, 2}, {3, 9, 3}}), 5);
  });

  ctx.test("edge_cases", [&] {
    ctx.expect_eq(sol.minCostII({}), 0);
    ctx.expect_eq(sol.minCostII({{8}}), 8);
    ctx.expect_eq(sol.minCostII({{4, 2, 9, 1}}), 1);
  });

  ctx.test("tied_minimum_costs_in_same_row", [&] {
    // Row 0 has two colors tied at cost 2, so row 1 can also pick color 0 (1)
    // paired with row 0 color 1 (2) => 3.
    ctx.expect_eq(sol.minCostII({{2, 2, 9}, {1, 8, 9}}), 3);
  });

  ctx.test("many_colors_and_houses", [&] {
    const std::vector<std::vector<int>> costs = {
        {10, 2, 15, 4, 8},
        {3, 1, 20, 6, 7},
        {9, 8, 2, 1, 5},
        {4, 12, 3, 2, 11},
    };
    // Optimal: row0 c1(2) -> row1 c0(3) -> row2 c3(1) -> row3 c2(3) = 9.
    ctx.expect_eq(sol.minCostII(costs), 9);
  });
}

}  // namespace
