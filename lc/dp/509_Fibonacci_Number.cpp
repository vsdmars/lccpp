// LeetCode 509: Fibonacci Number
// https://leetcode.com/problems/fibonacci-number/
//
// Problem:
//   The Fibonacci sequence is defined by F(0) = 0, F(1) = 1, and
//   F(n) = F(n - 1) + F(n - 2) for n > 1. Given an integer `n`, return F(n).
//   Constraints: 0 <= n <= 30.
//
// Examples:
//   Example 1:
//     Input:  n = 4
//     Output: 3
//     Explanation:
//       - Initialize rolling state variables prev2 = 0 (F(0)) and prev1 = 1 (F(1)).
//       - At i = 2, compute curr = 0 + 1 = 1 and advance to prev2 = 1, prev1 = 1.
//       - At i = 3, compute curr = 1 + 1 = 2 and advance to prev2 = 1, prev1 = 2.
//       - At i = 4, compute curr = 1 + 2 = 3, update prev2 = 2 and prev1 = 3, and
//         return prev1 = 3.
//
//   Example 2:
//     Input:  n = 10
//     Output: 55
//     Explanation:
//       - Start with prev2 = 0 and prev1 = 1 before iterating i from 2 to 10.
//       - Advance (prev2, prev1) across i = 2..6 through (1, 1), (1, 2), (2, 3),
//         (3, 5), and (5, 8).
//       - Advance (prev2, prev1) across i = 7..10 through (8, 13), (13, 21), (21,
//         34), and (34, 55).
//       - Exit the loop after i = 10 and return prev1 = 55.
//
//   Example 3:
//     Input:  n = 0
//     Output: 0
//     Explanation:
//       - Evaluate the base guard `if (n <= 0)` at the start of fib(0).
//       - Bypass the rolling-variable loop and immediately return 0.
//
// Algorithm:
//   1. Bottom-Up Iterative DP (`fib`):
//      Since F(i) depends only on the two immediately preceding terms, keep
//      two rolling state variables `prev2 = F(i - 2)` and `prev1 = F(i - 1)`
//      and advance from `i = 2` up to `n`.
//      Time Complexity:  O(n).
//      Space Complexity: O(1).
//   2. Direct Recursion (`fibRecursive`):
//      Directly mirrors the mathematical recurrence `F(n - 1) + F(n - 2)` for
//      comparison on small `n`.
//      Time Complexity:  O(phi^n) (exponential).
//      Space Complexity: O(n) call stack depth.
//
// Data Structures:
//   - Two scalar rolling registers (`prev2`, `prev1`) replacing a full O(n) DP
//     table.
//
// Real-World Applications:
//   - Retry backoff scheduling (Fibonacci backoff spreads client retries more
//     gently than pure exponential doubling under transient outages).
//   - Fibonacci search for unimodal / sorted-array lookup on storage media
//     with non-uniform seek costs (uses addition/subtraction instead of division).
//   - Amortized heap analysis and structural growth models (Fibonacci heaps,
//     AVL tree minimum node bounds).
//
// Related LeetCode Problems:
//   - #70 Climbing Stairs [done] (shifted Fibonacci recurrence with F(1)=1, F(2)=2)
//   - #1137 N-th Tribonacci Number [new] (3-term rolling linear recurrence)
//   - #746 Min Cost Climbing Stairs [done] (weighted 2-step linear DP with O(1) space)
//   - #198 House Robber [done] (2-state rolling DP over a linear array)

#include "framework/lc_framework.h"

#include <array>
#include <utility>

namespace {

class Solution {
 public:
  // Computes F(n) using bottom-up rolling-variable DP.
  // Time: O(n), Space: O(1).
  int fib(int n) {
    if (n <= 0) {
      return 0;
    }
    if (n == 1) {
      return 1;
    }

    // Invariant at start of iteration `i`: `prev2 == F(i - 2)`, `prev1 == F(i - 1)`.
    int prev2 = 0;
    int prev1 = 1;
    for (int i = 2; i <= n; ++i) {
      const int curr = prev2 + prev1;
      prev2 = prev1;
      prev1 = curr;
    }

    return prev1;
  }

  // Direct recursive formulation matching the Python reference's `naive()`
  // method (suitable for small `n`).
  // Time: O(phi^n), Space: O(n).
  int fibRecursive(int n) {
    auto compute = [](this auto&& self, int k) -> int {
      if (k <= 0) {
        return 0;
      }
      if (k == 1) {
        return 1;
      }
      return self(k - 1) + self(k - 2);
    };
    return compute(n);
  }
};

LC_PROBLEM(509, "Fibonacci Number", "dp", ctx) {
  ctx.test("examples_and_base_cases", [&] {
    Solution sol;
    const std::array<std::pair<int, int>, 6> cases = {{
        {0, 0},
        {1, 1},
        {2, 1},
        {3, 2},
        {4, 3},
        {10, 55},
    }};
    for (const auto& [n, expected] : cases) {
      ctx.expect_eq(sol.fib(n), expected);
      ctx.expect_eq(sol.fibRecursive(n), expected);
    }
  });

  ctx.test("max_constraint_n_30", [&] {
    Solution sol;
    ctx.expect_eq(sol.fib(30), 832040);
  });
}

}  // namespace
