// LeetCode 256: Paint House
// https://leetcode.com/problems/paint-house/
//
// Problem:
//   There is a row of n houses, each of which can be painted one of 3 colors:
//   red (0), blue (1), or green (2). The cost of painting house i with color c
//   is given by `costs[i][c]`. No two adjacent houses may share the same color.
//   Return the minimum total cost required to paint all houses.
//
// Examples:
//   Example 1:
//     Input:  costs = [[17, 2, 17], [16, 16, 5], [14, 3, 19]]
//     Output: 10
//     Explanation:
//       - Initialize dp = [0, 0, 0] for ending colors red (0), blue (1), and green
//         (2).
//       - After house 0 ([17, 2, 17]), dp becomes [17, 2, 17].
//       - After house 1 ([16, 16, 5]), next = [16 + min(2, 17), 16 + min(17, 17), 5
//         + min(17, 2)] updates dp to [18, 33, 7].
//       - After house 2 ([14, 3, 19]), next = [14 + min(33, 7), 3 + min(18, 7), 19
//         + min(18, 33)] gives dp = [21, 10, 37], and std::ranges::min(dp) returns
//         10.
//
//   Example 2:
//     Input:  costs = [[1, 2, 100], [1, 100, 100]]
//     Output: 3
//     Explanation:
//       - Initialize dp = [0, 0, 0] and process house 0 ([1, 2, 100]) to get dp =
//         [1, 2, 100].
//       - Process house 1 ([1, 100, 100]): next[0] = 1 + min(2, 100) = 3, next[1] =
//         100 + min(1, 100) = 101, and next[2] = 100 + min(1, 2) = 101.
//       - Return min(3, 101, 101) = 3, which corresponds to painting house 0 color
//         1 (cost 2) and house 1 color 0 (cost 1).
//
//   Example 3:
//     Input:  costs = []
//     Output: 0
//     Explanation:
//       - Initialize dp = [0, 0, 0].
//       - The loop over costs executes zero iterations because there are no houses,
//         and std::ranges::min(dp) returns 0.
//
// Algorithm:
//   Bottom-up state-machine dynamic programming with O(1) space.
//   Maintain a 3-element array `dp` where `dp[c]` is the minimum total cost to
//   paint all houses processed so far such that the last painted house has
//   color `c`. When advancing to the next house with color costs `cost`:
//     next[0] = cost[0] + min(dp[1], dp[2])
//     next[1] = cost[1] + min(dp[0], dp[2])
//     next[2] = cost[2] + min(dp[0], dp[1])
//   After processing all n houses, the answer is min(dp[0], dp[1], dp[2]).
//   Time Complexity:  O(n), where n = costs.size().
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   Fixed-size `std::array<int, 3>` rolling DP state vector holding the minimum
//   cumulative cost ending in each of the three colors.
//
// Real-World Applications:
//   - Viterbi / minimum-cost state-sequence decoding in hidden Markov models
//     with forbidden self-transitions.
//   - Manufacturing line changeover optimization where consecutive batches must
//     alternate configurations or materials at different per-step costs.
//   - Compiler register-class or execution-port assignment along a basic block
//     with back-to-back resource conflict constraints.
//
// Related LeetCode Problems:
//   - #265 Paint House II [done] (generalizes 3 colors to k colors in O(nk) time)
//   - #1473 Paint House III [new] (adds target neighborhood count and pre-painted houses)
//   - #276 Paint Fence [new] (counting colorings with at most two consecutive same colors)
//   - #198 House Robber [done] (simpler 2-state adjacent-constraint linear DP)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the minimum total cost to paint all houses such that no two
  // adjacent houses have the same color.
  //
  // Invariant: After processing house i, `dp[c]` equals the minimum cost to
  // paint houses 0..i with house i painted color c (for c in {0, 1, 2}).
  //
  // Time: O(n), Space: O(1).
  int minCost(const std::vector<std::vector<int>>& costs) {
    // Before painting any house, the cost for each ending color is 0.
    std::array<int, 3> dp = {0, 0, 0};

    for (const auto& cost : costs) {
      // Each color for the current house can only follow one of the other two
      // colors on the previous house.
      const std::array<int, 3> next = {
          cost[0] + std::min(dp[1], dp[2]),
          cost[1] + std::min(dp[0], dp[2]),
          cost[2] + std::min(dp[0], dp[1]),
      };
      dp = next;
    }

    return std::ranges::min(dp);
  }
};

LC_PROBLEM(256, "Paint House", "dp", ctx) {
  Solution sol;

  ctx.test("source_build_case", [&] {
    // House 0 -> green (2), House 1 -> red (3) => total 5.
    ctx.expect_eq(sol.minCost({{7, 6, 2}, {3, 9, 3}}), 5);
  });

  ctx.test("leetcode_examples", [&] {
    ctx.expect_eq(sol.minCost({{17, 2, 17}, {16, 16, 5}, {14, 3, 19}}), 10);
    ctx.expect_eq(sol.minCost({{7, 6, 2}}), 2);
  });

  ctx.test("edge_cases", [&] {
    ctx.expect_eq(sol.minCost({}), 0);
    ctx.expect_eq(sol.minCost({{5, 5, 5}, {5, 5, 5}, {5, 5, 5}}), 15);
  });

  ctx.test("forces_non_greedy_choice", [&] {
    // Picking cheapest color 0 (1) on house 0 forces a costly house 1 (100);
    // picking color 1 (2) on house 0 then color 0 (1) on house 1 yields 3.
    ctx.expect_eq(sol.minCost({{1, 2, 100}, {1, 100, 100}}), 3);
  });
}

}  // namespace
