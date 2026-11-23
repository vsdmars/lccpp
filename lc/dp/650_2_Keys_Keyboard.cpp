// LeetCode 650: 2 Keys Keyboard
// https://leetcode.com/problems/2-keys-keyboard/
//
// Problem:
//   A notepad starts with a single character 'A'. Each step allows either:
//     - Copy All: copy all characters currently on the notepad to the
//       clipboard.
//     - Paste: append the clipboard's contents to the notepad.
//   Given an integer n (1 <= n <= 1000), return the minimum number of steps
//   required to have exactly n 'A's on the notepad.
//
// Examples:
//   Example 1:
//     Input:  n = 3
//     Output: 3
//     Explanation:
//       - Initialize dp[1] = 0 because the notepad starts with one 'A'.
//       - For cur = 2, best starts at 2 and the divisor loop (d * d <= 2) does not
//         run, giving dp[2] = 2.
//       - For cur = 3, best starts at 3 and the divisor loop (d * d <= 3) does not
//         run, setting dp[3] = 3 (Copy All once, Paste twice).
//
//   Example 2:
//     Input:  n = 6
//     Output: 5
//     Explanation:
//       - Compute earlier DP values dp[1] = 0, dp[2] = 2, dp[3] = 3, dp[4] = 4, and
//         dp[5] = 5.
//       - At cur = 6, initialize best = 6 and test divisor d = 2 with complementary
//         quotient q = 3.
//       - Evaluate both transitions dp[2] + 3 = 5 and dp[3] + 2 = 5, updating dp[6]
//         to 5.
//
//   Example 3:
//     Input:  n = 1
//     Output: 0
//     Explanation:
//       - Check the base condition n <= 1 at the start of minSteps(1).
//       - Return 0 immediately because one 'A' is already on the notepad without
//         any Copy All or Paste steps.
//
// Algorithm:
//   Bottom-up dynamic programming over divisor transitions (plus an equivalent
//   O(sqrt(n)) prime-factorization verification method).
//   Let dp[k] be the minimum steps to produce exactly k 'A's.
//   - Base case: dp[1] = 0 (we already begin with one 'A').
//   - For each target k in 2..n:
//     Initialize dp[k] = k (Copy once at length 1, then Paste k - 1 times).
//     Whenever `d` divides `k` (1 < d < k), reaching `d` in dp[d] steps,
//     performing 1 Copy All, and pasting `(k / d) - 1` times produces `k` in
//     `dp[d] + (k / d)` steps. Iterating `d` from 2 up to sqrt(k) lets us test
//     both complementary divisors `d` and `k / d` in O(sqrt(k)) time per k.
//   Mathematically, any optimal sequence partitions into Copy-Paste^p blocks
//   that multiply the screen count by prime factors, so dp[n] also equals the
//   sum of all prime factors of n (counted with multiplicity).
//   Time Complexity: O(n * sqrt(n)) for the DP; O(sqrt(n)) for prime factoring.
//   Space Complexity: O(n) for the DP table; O(1) for prime factoring.
//
// Data Structures:
//   - 1D DP array (`std::vector<int>` of size n + 1): memoizes the minimum
//     operation count for every intermediate character count 1..n.
//
// Real-World Applications:
//   - Addition-chain and multiplication-chain optimization in compilers and
//     cryptographic exponentiation (minimizing instructions to build constants).
//   - Optimal batching / replication scheduling when bulk-copying state across
//     distributed nodes.
//
// Related LeetCode Problems:
//   - #651 4 Keys Keyboard [done] (adds Ctrl-A/Select and maximizes 'A's for fixed presses)
//   - #279 Perfect Squares [done] (1D minimum-steps DP over numeric transitions)
//   - #343 Integer Break [new] (multiplicative decomposition DP / number theory)
//   - #991 Broken Calculator [new] (minimum operations to reach a target integer)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the minimum Copy-All / Paste steps to reach `n` 'A's via
  // bottom-up divisor DP.
  // Time: O(n * sqrt(n)), Space: O(n).
  int minSteps(int n) {
    if (n <= 1) {
      return 0;
    }

    const auto target = static_cast<std::size_t>(n);
    std::vector<int> dp(target + 1uz, 0);

    for (std::size_t cur = 2uz; cur <= target; ++cur) {
      // Worst-case bound: copy a single 'A' (1 step) and paste (cur - 1)
      // times, for `cur` total steps. This is optimal whenever `cur` is prime.
      int best = static_cast<int>(cur);

      // Check every factor pair (d, q) with d * q == cur.
      // Reaching `d` then doing 1 Copy + (q - 1) Pastes takes dp[d] + q steps;
      // symmetrically, reaching `q` then multiplying by `d` takes dp[q] + d.
      for (std::size_t d = 2uz; (d * d) <= cur; ++d) {
        if ((cur % d) != 0uz) {
          continue;
        }
        const std::size_t q = cur / d;
        best = std::min(best, dp[d] + static_cast<int>(q));
        best = std::min(best, dp[q] + static_cast<int>(d));
      }

      dp[cur] = best;
    }

    return dp[target];
  }

  // Optimal number-theoretic method: the minimum steps equal the sum of prime
  // factors of `n` (with multiplicity).
  // Time: O(sqrt(n)), Space: O(1).
  int minStepsPrimeFactor(int n) {
    if (n <= 1) {
      return 0;
    }

    int steps = 0;
    int remaining = n;
    for (int factor = 2; (factor * factor) <= remaining; ++factor) {
      while ((remaining % factor) == 0) {
        steps += factor;
        remaining /= factor;
      }
    }
    if (remaining > 1) {
      steps += remaining;
    }
    return steps;
  }
};

LC_PROBLEM(650, "2 Keys Keyboard", "dp", ctx) {
  ctx.test("examples_and_python_build_cases", [&] {
    Solution sol;
    ctx.expect_eq(sol.minSteps(1), 0);
    ctx.expect_eq(sol.minSteps(2), 2);
    ctx.expect_eq(sol.minSteps(3), 3);
    ctx.expect_eq(sol.minSteps(4), 4);
    ctx.expect_eq(sol.minSteps(5), 5);
  });

  ctx.test("composite_and_prime_numbers", [&] {
    Solution sol;
    ctx.expect_eq(sol.minSteps(6), 5);     // 2 + 3
    ctx.expect_eq(sol.minSteps(9), 6);     // 3 + 3
    ctx.expect_eq(sol.minSteps(12), 7);    // 2 + 2 + 3
    ctx.expect_eq(sol.minSteps(18), 8);    // 2 + 3 + 3
    ctx.expect_eq(sol.minSteps(997), 997); // prime
    ctx.expect_eq(sol.minSteps(1000), 21); // 2^3 * 5^3 -> 6 + 15
  });

  ctx.test("dp_matches_prime_factorization", [&] {
    Solution sol;
    for (int n = 1; n <= 120; ++n) {
      ctx.expect_eq(sol.minSteps(n), sol.minStepsPrimeFactor(n));
    }
  });
}

}  // namespace
