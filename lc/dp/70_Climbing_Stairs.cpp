// LeetCode 70: Climbing Stairs
// https://leetcode.com/problems/climbing-stairs/
//
// Problem:
//   You are climbing a staircase with n steps (1 <= n <= 45). Each move lets
//   you advance by either 1 step or 2 steps. Return the total number of
//   distinct step sequences that reach step n.
//
// Algorithm:
//   Bottom-up dynamic programming with O(1) space (rolling Fibonacci recurrence).
//   Let ways(i) be the number of distinct sequences to reach step i. Every path
//   to step i must arrive either via a 1-step hop from i - 1 or a 2-step hop
//   from i - 2, yielding the recurrence ways(i) = ways(i - 1) + ways(i - 2)
//   with base cases ways(0) = 1 and ways(1) = 1. Because each state only reads
//   the two preceding states, two rolling integers suffice in place of a full
//   DP array or dictionary.
//   Time Complexity:  O(n).
//   Space Complexity: O(1).
//
// Data Structures:
//   - Two scalar rolling variables (`prev2` for ways(i - 2) and `prev1` for
//     ways(i - 1)) replacing an explicit DP table.
//
// Real-World Applications:
//   - Combinatorial tiling counts (1x2 domino tilings of a 2xn strip) used in
//     statistical mechanics and layout enumeration.
//   - Run-length / variable-width packet framing counts in communication theory.
//   - Baseline linear-recurrence modeling in dynamic programming and automated
//     workflow step enumeration.
//
// Related LeetCode Problems:
//   - #509 Fibonacci Number [done] (identical linear recurrence with shifted base)
//   - #746 Min Cost Climbing Stairs [done] (weighted min-cost variant)
//   - #1137 N-th Tribonacci Number [new] (1, 2, or 3 step generalization)
//   - #91 Decode Ways [done] (1-char / 2-char conditional Fibonacci DP)

#include "framework/lc_framework.h"

namespace {

class Solution {
 public:
  // Computes the number of distinct ways to climb `n` stairs taking 1 or 2
  // steps at a time.
  // Time: O(n), Space: O(1).
  int climbStairs(int n) {
    if (n <= 1) {
      return 1;
    }

    // Invariant at start of iteration i:
    //   prev2 == ways(i - 2), prev1 == ways(i - 1).
    int prev2 = 1;  // ways(0): 1 empty sequence
    int prev1 = 1;  // ways(1): [1]

    for (int i = 2; i <= n; ++i) {
      const int cur = prev1 + prev2;
      prev2 = prev1;
      prev1 = cur;
    }

    return prev1;
  }
};

LC_PROBLEM(70, "Climbing Stairs", "dp", ctx) {
  Solution sol;

  ctx.test("base_case_1", [&] {
    ctx.expect_eq(sol.climbStairs(1), 1);
  });

  ctx.test("example_1_two_steps", [&] {
    ctx.expect_eq(sol.climbStairs(2), 2);
  });

  ctx.test("three_and_five_steps", [&] {
    ctx.expect_eq(sol.climbStairs(3), 3);
    // Matches the Python build() test case.
    ctx.expect_eq(sol.climbStairs(5), 8);
  });

  ctx.test("medium_and_max_constraint", [&] {
    ctx.expect_eq(sol.climbStairs(10), 89);
    // Max LeetCode constraint (n = 45) still fits in 32-bit signed int.
    ctx.expect_eq(sol.climbStairs(45), 1836311903);
  });
}

}  // namespace
