// LeetCode 2235: Add Two Integers
// https://leetcode.com/problems/add-two-integers/
//
// Problem:
//   Given two integers `num1` and `num2`, return their arithmetic sum
//   `num1 + num2`.
//   Constraints: -100 <= num1, num2 <= 100.
//
// Algorithm:
//   Direct constant-time signed integer addition. Given the bounded input
//   range `[-100, 100]`, the result lies within `[-200, 200]`, so signed
//   32-bit `int` addition is exact and cannot overflow.
//   Time Complexity:  O(1).
//   Space Complexity: O(1).
//
// Data Structures:
//   - None (primitive `int` operands only).
//
// Real-World Applications:
//   - Fundamental ALU arithmetic operation underlying all numerical computing,
//     address offset calculation, and accumulator/counter updates.
//   - Smoke-test / pipeline-verification workload for validating compiler,
//     runtime, and judging infrastructure.
//
// Related LeetCode Problems:
//   - #371 Sum of Two Integers [done] (computing a + b using bitwise operations without +)
//   - #2 Add Two Numbers [done] (digit-by-digit addition of linked-list big integers)
//   - #415 Add Strings [done] (arbitrary-precision decimal addition on strings)
//   - #67 Add Binary [done] (binary string addition with carry propagation)

#include "framework/lc_framework.h"

namespace {

class Solution {
 public:
  // Returns the arithmetic sum of `num1` and `num2`.
  // Within the problem constraints [-100, 100], signed overflow cannot occur.
  // Time: O(1), Space: O(1).
  int sum(int num1, int num2) {
    return num1 + num2;
  }
};

LC_PROBLEM(2235, "Add Two Integers", "math", ctx) {
  ctx.test("example_1_build_positive", [&] {
    Solution sol;
    ctx.expect_eq(sol.sum(12, 5), 17);
  });

  ctx.test("example_2_mixed_sign", [&] {
    Solution sol;
    ctx.expect_eq(sol.sum(-10, 4), -6);
  });

  ctx.test("zeros_and_cancellation", [&] {
    Solution sol;
    ctx.expect_eq(sol.sum(0, 0), 0);
    ctx.expect_eq(sol.sum(-42, 42), 0);
  });

  ctx.test("constraint_boundaries", [&] {
    Solution sol;
    ctx.expect_eq(sol.sum(-100, -100), -200);
    ctx.expect_eq(sol.sum(100, 100), 200);
  });
}

}  // namespace
