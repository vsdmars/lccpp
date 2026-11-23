// LeetCode 474: Ones And Zeroes
// https://leetcode.com/problems/ones-and-zeroes/
//
// Problem:
//   Given an array `strs` of binary strings (containing only `'0'` and `'1'`)
//   and two non-negative integers `m` and `n`, return the size of the largest
//   subset of `strs` that contains at most `m` `'0'`s and `n` `'1'`s in total.
//   Each string may be chosen at most once.
//   Constraints: 1 <= strs.size() <= 600, 1 <= strs[i].size() <= 100,
//   1 <= m, n <= 100.
//
// Examples:
//   Example 1:
//     Input:  strs = ["10", "0001", "111001", "1", "0"], m = 5, n = 3
//     Output: 4
//     Explanation:
//       - Allocate dp[6][4] initialized to 0, where dp[i][j] tracks the largest
//         subset using at most i zeroes and j ones.
//       - Process "10" (cost 1,1) and "0001" (cost 3,1) in reverse capacity order,
//         raising dp[4][2]..dp[5][3] to 2, then skip "111001" (cost 2,4) because
//         ones = 4 exceeds n = 3.
//       - Process "1" (cost 0,1) to reach dp[5][3] = 3, then process "0" (cost 1,0)
//         so dp[5][3] = max(dp[5][3], dp[4][3] + 1) = 4 (subset {"10", "0001", "1",
//         "0"}).
//       - Return dp[5][3] = 4.
//
//   Example 2:
//     Input:  strs = ["10", "0", "1"], m = 1, n = 1
//     Output: 2
//     Explanation:
//       - Allocate dp[2][2] initialized to 0 for budget m = 1, n = 1.
//       - After processing "10" (cost 1,1), dp[1][1] becomes 1; after "0" (cost
//         1,0), dp[1][0] becomes 1 while dp[1][1] stays 1.
//       - After processing "1" (cost 0,1), dp[0][1] becomes 1 and dp[1][1] updates
//         to max(1, dp[1][0] + 1) = 2 by combining "0" and "1".
//       - Return dp[1][1] = 2.
//
//   Example 3:
//     Input:  strs = ["0", "00", "1", "01"], m = 3, n = 0
//     Output: 2
//     Explanation:
//       - Allocate dp[4][1] initialized to 0 for zero budget m = 3 and ones budget
//         n = 0.
//       - Process "0" (cost 1,0) to set dp[1][0] = dp[2][0] = dp[3][0] = 1, then
//         process "00" (cost 2,0) in reverse to update dp[3][0] = dp[1][0] + 1 = 2.
//       - Skip both "1" (cost 0,1) and "01" (cost 1,1) because their ones count 1
//         exceeds max_ones = 0.
//       - Return dp[3][0] = 2.
//
// Algorithm:
//   This is a two-dimensional 0/1 Knapsack problem where each string is an
//   item with weight vector `(zeros, ones)` and value `1`, and the knapsack
//   capacities are `(m, n)`.
//   Let `dp[i][j]` be the maximum number of strings formable using at most `i`
//   zeroes and `j` ones. For each string with cost `(z, o)`, iterate `i` from
//   `m` down to `z` and `j` from `n` down to `o`, updating:
//     dp[i][j] = max(dp[i][j], dp[i - z][j - o] + 1)
//   Traversing both capacity dimensions in descending order guarantees that
//   `dp[i - z][j - o]` still reflects the previous item layer, enforcing the
//   0/1 (at-most-once) selection invariant without allocating a 3D table.
//   Time Complexity:  O(K * (L_avg + m * n)), where K = strs.size() and L_avg
//                     is the average string length.
//   Space Complexity: O(m * n) for the 2D DP table.
//
// Data Structures:
//   - 2D DP table (`std::vector<std::vector<int>>` of dimensions `(m+1) x (n+1)`):
//     stores optimal subset cardinalities for every `(zeros_budget, ones_budget)`
//     pair.
//
// Real-World Applications:
//   - Multi-resource cloud job scheduling (bin packing tasks constrained by
//     both CPU cores and RAM capacity to maximize throughput).
//   - Embedded firmware feature selection under simultaneous Flash (ROM) and
//     SRAM memory budgets.
//   - Ad-slot / portfolio optimization choosing discrete campaigns bounded by
//     multiple orthogonal resource caps (e.g., budget and impression quotas).
//
// Related LeetCode Problems:
//   - #416 Partition Equal Subset Sum [done] (classic 1D 0/1 knapsack feasibility)
//   - #494 Target Sum [done] (0/1 knapsack counting ways)
//   - #322 Coin Change [done] (unbounded knapsack minimizing item count)
//   - #879 Profitable Schemes [new] (2D 0/1 knapsack counting subsets with two constraints)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the maximum subset size of `strs` using at most `m` zeroes and `n`
  // ones via space-optimized 2D 0/1 knapsack DP.
  // Time: O(K * (L_avg + m * n)), Space: O(m * n).
  int findMaxForm(const std::vector<std::string>& strs, int m, int n) {
    if (m < 0 || n < 0) {
      return 0;
    }

    const auto max_zeros = static_cast<std::size_t>(m);
    const auto max_ones = static_cast<std::size_t>(n);

    // `dp[i][j]` holds the best subset size achievable with at most `i` zeroes
    // and `j` ones after considering a prefix of `strs`.
    std::vector<std::vector<int>> dp(max_zeros + 1uz,
                                     std::vector<int>(max_ones + 1uz, 0));

    for (const std::string& str : strs) {
      const auto [zeros, ones] = count_bits(str);
      if (zeros > max_zeros || ones > max_ones) {
        continue;
      }

      // Iterate capacities backwards so each string is used at most once
      // (reading strictly from the prior item's DP state).
      for (std::size_t zi = max_zeros + 1uz; zi > zeros; --zi) {
        const std::size_t i = zi - 1uz;
        for (std::size_t oi = max_ones + 1uz; oi > ones; --oi) {
          const std::size_t j = oi - 1uz;
          dp[i][j] = std::max(dp[i][j], dp[i - zeros][j - ones] + 1);
        }
      }
    }

    return dp[max_zeros][max_ones];
  }

 private:
  // Counts the number of `'0'` and `'1'` characters in a binary string.
  [[nodiscard]] static std::pair<std::size_t, std::size_t> count_bits(
      std::string_view s) noexcept {
    std::size_t zeros = 0uz;
    std::size_t ones = 0uz;
    for (const char ch : s) {
      if (ch == '0') {
        ++zeros;
      } else if (ch == '1') {
        ++ones;
      }
    }
    return {zeros, ones};
  }
};

LC_PROBLEM(474, "Ones And Zeroes", "dp", ctx) {
  ctx.test("example_1_five_strings", [&] {
    Solution sol;
    const std::vector<std::string> strs = {"10", "0001", "111001", "1", "0"};
    ctx.expect_eq(sol.findMaxForm(strs, 5, 3), 4);
  });

  ctx.test("example_2_prefer_two_smaller", [&] {
    Solution sol;
    const std::vector<std::string> strs = {"10", "0", "1"};
    ctx.expect_eq(sol.findMaxForm(strs, 1, 1), 2);
  });

  ctx.test("insufficient_budget", [&] {
    Solution sol;
    const std::vector<std::string> strs = {"111", "000"};
    ctx.expect_eq(sol.findMaxForm(strs, 2, 2), 0);
  });

  ctx.test("zero_ones_budget", [&] {
    Solution sol;
    const std::vector<std::string> strs = {"0", "00", "1", "01"};
    ctx.expect_eq(sol.findMaxForm(strs, 3, 0), 2);
  });

  ctx.test("empty_input", [&] {
    Solution sol;
    const std::vector<std::string> strs = {};
    ctx.expect_eq(sol.findMaxForm(strs, 5, 5), 0);
  });
}

}  // namespace
