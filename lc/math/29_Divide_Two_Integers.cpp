// LeetCode 29: Divide Two Integers
// https://leetcode.com/problems/divide-two-integers/
//
// Problem:
//   Given two 32-bit signed integers `dividend` and `divisor` (with
//   `divisor != 0`), compute their integer quotient truncated toward zero
//   without using multiplication, division, or modulo operators. Assume the
//   environment stores only 32-bit signed integers in [-2^31, 2^31 - 1]; if
//   the exact quotient overflows (specifically `INT_MIN / -1`), return
//   `INT_MAX` (2^31 - 1).
//
// Algorithm:
//   Exponential galloping (binary long division) performed entirely in the
//   negative 32-bit domain. Because the negative half `[-2^31, 0]` has one
//   extra representable value over `[0, 2^31 - 1]`, mapping both `dividend`
//   and `divisor` to non-positive numbers prevents signed overflow on
//   `INT_MIN` without needing 64-bit integers. After handling the single
//   overflow case (`INT_MIN / -1`), repeatedly find the largest power-of-two
//   multiple `value = neg_divisor * 2^k` that still fits inside
//   `neg_dividend` (guarded by `value >= kHalfIntMin` so `value + value`
//   never underflows `INT_MIN`), subtract `value` from `neg_dividend`, and
//   accumulate the negative quotient `power_of_two = -2^k`. Finally, restore
//   the sign based on whether the original operands differed in sign.
//   Time Complexity: O(log^2 |dividend|) (at most 31 outer steps, each doing
//     at most 31 doublings; strictly O(1) bounded for 32-bit integers).
//   Space Complexity: O(1).
//
// Data Structures:
//   Scalar 32-bit signed integers (`int`) tracking the remaining negative
//   dividend, doubled divisor multiple, corresponding power-of-two weight,
//   and accumulated negative quotient.
//
// Real-World Applications:
//   - Software division routines (`__divsi3`) in compiler runtime libraries
//     for microcontrollers and RISC cores lacking a hardware divider.
//   - Arbitrary-precision (BigInt) binary long-division primitives and
//     cryptographic modular reduction routines.
//
// Related LeetCode Problems:
//   - #50 Powx N [done] (binary exponentiation using repeated doubling/squaring)
//   - #69 Sqrtx [done] (integer root computation with 32-bit boundary care)
//   - #7 Reverse Integer [done] (strict 32-bit signed range and overflow guards)
//   - #43 Multiply Strings [done] (arithmetic without built-in multiplication)

#include "framework/lc_framework.h"

#include <limits>

namespace {

class Solution {
 public:
  // Computes `dividend / divisor` truncated toward zero using only addition,
  // subtraction, and comparisons in strictly 32-bit signed arithmetic.
  // Returns `INT_MAX` if the result exceeds `INT_MAX` (`INT_MIN / -1`).
  // Time: O(log^2 |dividend|), Space: O(1).
  int divide(int dividend, int divisor) {
    constexpr int kIntMin = std::numeric_limits<int>::min();
    constexpr int kIntMax = std::numeric_limits<int>::max();
    // Exact half of INT_MIN (-2^30): any `value >= kHalfIntMin` can be doubled
    // via `value + value` without signed underflow.
    constexpr int kHalfIntMin = -1073741824;

    // The only 32-bit division whose quotient exceeds INT_MAX is INT_MIN / -1.
    if (dividend == kIntMin && divisor == -1) {
      return kIntMax;
    }

    const bool negative = (dividend < 0) != (divisor < 0);

    // Convert both operands to non-positive values so INT_MIN never needs to
    // be negated (which would overflow 32-bit signed int).
    int neg_dividend = (dividend > 0) ? -dividend : dividend;
    const int neg_divisor = (divisor > 0) ? -divisor : divisor;

    // Accumulate the quotient as a negative integer as well so that
    // `INT_MIN / 1 == INT_MIN` remains representable without overflow.
    int neg_quotient = 0;

    while (neg_dividend <= neg_divisor) {
      int value = neg_divisor;
      int power_of_two = -1;

      // Double `value` (moving more negative) while it stays >= kHalfIntMin
      // and `value + value` still fits inside `neg_dividend`.
      while (value >= kHalfIntMin && neg_dividend <= (value + value)) {
        value += value;
        power_of_two += power_of_two;
      }

      neg_dividend -= value;
      neg_quotient += power_of_two;
    }

    return negative ? neg_quotient : -neg_quotient;
  }
};

LC_PROBLEM(29, "Divide Two Integers", "math", ctx) {
  Solution sol;

  ctx.test("examples", [&] {
    ctx.expect_eq(sol.divide(10, 3), 3);
    ctx.expect_eq(sol.divide(7, -3), -2);
    ctx.expect_eq(sol.divide(-7, -3), 2);
    ctx.expect_eq(sol.divide(-7, 3), -2);
  });

  ctx.test("zero_and_identity", [&] {
    ctx.expect_eq(sol.divide(0, 1), 0);
    ctx.expect_eq(sol.divide(0, -5), 0);
    ctx.expect_eq(sol.divide(42, 42), 1);
    ctx.expect_eq(sol.divide(42, -42), -1);
    ctx.expect_eq(sol.divide(3, 10), 0);
  });

  ctx.test("int32_boundaries", [&] {
    constexpr int kIntMin = std::numeric_limits<int>::min();
    constexpr int kIntMax = std::numeric_limits<int>::max();

    ctx.expect_eq(sol.divide(kIntMin, -1), kIntMax);
    ctx.expect_eq(sol.divide(kIntMin, 1), kIntMin);
    ctx.expect_eq(sol.divide(kIntMin, 2), -1073741824);
    ctx.expect_eq(sol.divide(kIntMin, -2), 1073741824);
    ctx.expect_eq(sol.divide(kIntMin, kIntMin), 1);
    ctx.expect_eq(sol.divide(kIntMax, 1), kIntMax);
    ctx.expect_eq(sol.divide(kIntMax, -1), -kIntMax);
    ctx.expect_eq(sol.divide(kIntMax, kIntMin), 0);
  });
}

}  // namespace
