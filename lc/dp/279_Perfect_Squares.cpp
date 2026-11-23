// LeetCode 279: Perfect Squares
// https://leetcode.com/problems/perfect-squares/
//
// Problem:
//   Given a positive integer `n`, return the least number of perfect square
//   numbers (1, 4, 9, 16, ...) whose sum equals `n`.
//
// Algorithm:
//   Bottom-up unbounded-knapsack dynamic programming (primary `numSquares`),
//   plus a top-down memoized DFS variant (`numSquaresMemo`) mirroring the
//   secondary approach from the Python source.
//   Let dp[x] be the minimum number of perfect squares summing to x, with
//   dp[0] = 0. For each integer x from 1 to n, try every perfect square s^2 <= x
//   as the last summand:
//     dp[x] = min_{1 <= s, s*s <= x} (dp[x - s*s] + 1).
//   Every positive integer is guaranteed to be expressible as at most 4 squares
//   (Lagrange's four-square theorem), so all states are well-defined and finite.
//   Time Complexity:  O(n * sqrt(n)).
//   Space Complexity: O(n).
//
// Data Structures:
//   - 1D DP table (`std::vector<int>`) of size n + 1 where `dp[x]` stores the
//     minimum square count for target sum x.
//   - 1D memoization table (`std::vector<int>`) in the top-down DFS method.
//
// Real-World Applications:
//   - Unbounded coin-change / resource-denomination optimization (selecting the
//     fewest standard-sized chunks, tiles, or memory blocks to cover a target).
//   - Signal quantization and sparse basis decomposition over quadratic
//     step sizes.
//   - Shortest-path modeling on implicit integer-state transition graphs.
//
// Related LeetCode Problems:
//   - #322 Coin Change [done] (unbounded minimum-count knapsack with arbitrary denominations)
//   - #518 Coin Change II [done] (counts distinct combinations instead of minimizing count)
//   - #264 Ugly Number II [new] (bottom-up number-theoretic DP)
//   - #494 Target Sum [done] (subset-sum DP/DFS referenced in the source notes)
//   - #377 Combination Sum IV [done] (unbounded target-sum DP)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <vector>

namespace {

class Solution {
 public:
  // Bottom-up DP: returns the minimum number of perfect squares summing to `n`.
  //
  // Invariant: After iteration `x`, `dp[0..x]` holds the exact minimum square
  // count for every integer in [0, x].
  //
  // Time: O(n * sqrt(n)), Space: O(n).
  int numSquares(int n) {
    if (n <= 0) {
      return 0;
    }

    const auto target = static_cast<std::size_t>(n);
    std::vector<int> dp(target + 1uz, std::numeric_limits<int>::max());
    dp[0] = 0;

    for (int x = 1; x <= n; ++x) {
      int best = std::numeric_limits<int>::max();
      // Try every perfect square s*s <= x as the final summand.
      for (int s = 1; (s * s) <= x; ++s) {
        const auto prev_idx = static_cast<std::size_t>(x - (s * s));
        best = std::min(best, dp[prev_idx] + 1);
      }
      dp[static_cast<std::size_t>(x)] = best;
    }

    return dp[target];
  }

  // Top-down memoized DFS variant (mirrors Python `rewrite`, simplified to a
  // 1D memo table because the largest usable square depends solely on `rem`).
  //
  // Time: O(n * sqrt(n)), Space: O(n).
  int numSquaresMemo(int n) {
    if (n <= 0) {
      return 0;
    }

    std::vector<int> memo(static_cast<std::size_t>(n) + 1uz, -1);
    memo[0] = 0;

    auto dfs = [&memo](this auto&& self, int rem) -> int {
      const auto idx = static_cast<std::size_t>(rem);
      if (memo[idx] != -1) {
        return memo[idx];
      }

      // Compute floor(sqrt(rem)) via integer arithmetic to avoid floating-point
      // narrowing conversions, then try larger squares first to shrink `rem`
      // rapidly and keep recursion depth shallow.
      int root = 1;
      while (((root + 1) * (root + 1)) <= rem) {
        ++root;
      }

      int best = std::numeric_limits<int>::max();
      for (int s = root; s >= 1; --s) {
        best = std::min(best, self(rem - (s * s)) + 1);
      }

      memo[idx] = best;
      return best;
    };

    return dfs(n);
  }
};

LC_PROBLEM(279, "Perfect Squares", "dp", ctx) {
  Solution sol;

  ctx.test("leetcode_examples", [&] {
    // 12 = 4 + 4 + 4 (3 squares), 13 = 4 + 9 (2 squares).
    ctx.expect_eq(sol.numSquares(12), 3);
    ctx.expect_eq(sol.numSquares(13), 2);
    ctx.expect_eq(sol.numSquaresMemo(12), 3);
    ctx.expect_eq(sol.numSquaresMemo(13), 2);
  });

  ctx.test("small_and_exact_squares", [&] {
    ctx.expect_eq(sol.numSquares(1), 1);
    ctx.expect_eq(sol.numSquares(4), 1);
    ctx.expect_eq(sol.numSquares(9), 1);
    // 7 = 4 + 1 + 1 + 1 (Legendre 4^0 * (8*0 + 7) requires 4 squares).
    ctx.expect_eq(sol.numSquares(7), 4);
    ctx.expect_eq(sol.numSquaresMemo(7), 4);
  });

  ctx.test("source_build_larger_inputs", [&] {
    // 1001 = 31^2 + 6^2 + 2^2 = 961 + 36 + 4 (3 squares).
    ctx.expect_eq(sol.numSquares(1001), 3);
    ctx.expect_eq(sol.numSquaresMemo(1001), 3);

    // 8935 = 8 * 1116 + 7 => requires 4 squares by Legendre's theorem.
    ctx.expect_eq(sol.numSquares(8935), 4);
  });
}

}  // namespace
