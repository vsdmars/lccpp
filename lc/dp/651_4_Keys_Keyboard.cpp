// LeetCode 651: 4 Keys Keyboard
// https://leetcode.com/problems/4-keys-keyboard/
//
// Problem:
//   Imagine a special keyboard with four keys:
//     - Key 1 (A): print one 'A' on the screen.
//     - Key 2 (Ctrl-A): select the entire screen.
//     - Key 3 (Ctrl-C): copy the selection to the clipboard buffer.
//     - Key 4 (Ctrl-V): append the clipboard buffer to the screen.
//   Given an integer n (1 <= n <= 50), return the maximum number of 'A's that
//   can appear on the screen after at most n key presses.
//
// Algorithm:
//   Bottom-up dynamic programming over the total number of key presses.
//   Let dp[k] be the maximum number of 'A's obtainable in exactly k presses.
//   Any optimal sequence ending at press k either:
//   1) Presses 'A' directly for all k steps (or appends one 'A' to dp[k - 1]):
//      baseline dp[k] = k.
//   2) Ends with a contiguous block of `Ctrl-A`, `Ctrl-C`, followed by one or
//      more `Ctrl-V` presses starting immediately after press `i` (where
//      1 <= i <= k - 3). Because `Ctrl-A` and `Ctrl-C` consume 2 presses, the
//      remaining `k - i - 2` presses are `Ctrl-V`, appending `k - i - 2`
//      copies of `dp[i]` to the existing `dp[i]` on screen for a total of:
//        dp[i] * (k - i - 1).
//   Taking the maximum over all valid breakpoints `i` in `1..k-3` computes
//   dp[k] exactly.
//   Time Complexity: O(n^2) (can be tightened to O(n) since optimal breakpoints
//   always satisfy k - 6 <= i <= k - 3).
//   Space Complexity: O(n) for the 1D DP table.
//
// Data Structures:
//   - 1D DP array (`std::vector<int>` of size n + 1): stores the optimal 'A'
//     count achievable after every press count 0..n.
//
// Real-World Applications:
//   - Optimal checkpointing / batching trade-offs where a fixed setup overhead
//     (Select + Copy) multiplies future throughput over repeated executions.
//   - Compounding resource-allocation planning with setup-cost vs.
//     exploitation-horizon decisions.
//
// Related LeetCode Problems:
//   - #650 2 Keys Keyboard [done] (dual minimization problem without Select-All)
//   - #343 Integer Break [new] (maximizing a product over additive budgets)
//   - #279 Perfect Squares [done] (1D optimal-breakpoint DP)
//   - #322 Coin Change [done] (unbounded bottom-up 1D DP over step budgets)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the maximum number of 'A's printable in `n` key presses using
  // bottom-up breakpoint DP.
  // Time: O(n^2), Space: O(n).
  int maxA(int n) {
    if (n <= 0) {
      return 0;
    }

    const auto total_presses = static_cast<std::size_t>(n);
    std::vector<int> dp(total_presses + 1uz, 0);

    for (std::size_t k = 1uz; k <= total_presses; ++k) {
      // Baseline: either press 'A' k times in a row, or append one 'A' to the
      // best (k - 1)-press screen.
      int best = std::max(static_cast<int>(k), dp[k - 1uz] + 1);

      // Try every breakpoint `i` where we have `dp[i]` A's on screen, then
      // press Ctrl-A (press i + 1), Ctrl-C (press i + 2), and Ctrl-V for the
      // remaining (k - i - 2) presses. That yields 1 + (k - i - 2) = k - i - 1
      // total copies of dp[i].
      if (k >= 4uz) {
        for (std::size_t i = 1uz; i <= (k - 3uz); ++i) {
          const int multiplier = static_cast<int>((k - i) - 1uz);
          best = std::max(best, dp[i] * multiplier);
        }
      }

      dp[k] = best;
    }

    return dp[total_presses];
  }
};

LC_PROBLEM(651, "4 Keys Keyboard", "dp", ctx) {
  ctx.test("examples_from_prompt", [&] {
    Solution sol;
    ctx.expect_eq(sol.maxA(3), 3);
    ctx.expect_eq(sol.maxA(7), 9);
  });

  ctx.test("small_press_counts", [&] {
    Solution sol;
    ctx.expect_eq(sol.maxA(0), 0);
    ctx.expect_eq(sol.maxA(1), 1);
    ctx.expect_eq(sol.maxA(2), 2);
    ctx.expect_eq(sol.maxA(4), 4);
    ctx.expect_eq(sol.maxA(5), 5);
    ctx.expect_eq(sol.maxA(6), 6);
  });

  ctx.test("medium_sequences_with_multiple_copy_cycles", [&] {
    Solution sol;
    ctx.expect_eq(sol.maxA(8), 12);
    ctx.expect_eq(sol.maxA(9), 16);
    ctx.expect_eq(sol.maxA(10), 20);
    ctx.expect_eq(sol.maxA(11), 27);
    ctx.expect_eq(sol.maxA(12), 36);
    ctx.expect_eq(sol.maxA(15), 81);
  });

  ctx.test("upper_constraint_n50", [&] {
    Solution sol;
    // Optimal schedule for n = 50 fits within 32-bit signed int: 1,327,104.
    ctx.expect_eq(sol.maxA(50), 1327104);
  });
}

}  // namespace
