// LeetCode 50: Powx N
// https://leetcode.com/problems/powx-n/
//
// Problem:
//   Implement `myPow(x, n)`, which calculates `x` raised to the integer power
//   `n` (`x^n`), where `x` is a double-precision floating-point number in
//   (-100.0, 100.0) and `n` is a 32-bit signed integer in [-2^31, 2^31 - 1].
//
// Examples:
//   Example 1:
//     Input:  x = 2.0, n = 10
//     Output: 1024.0
//     Explanation:
//       - Because exp = 10 is non-negative, base stays 2.0 and result starts at
//         1.0.
//       - Iteration 1 (exp = 10, even): (exp & 1) == 0 leaves result = 1.0, exp
//         shifts right to 5, and base squares to 4.0.
//       - Iteration 2 (exp = 5, odd): result becomes 1.0 * 4.0 = 4.0, exp shifts
//         right to 2, and base squares to 16.0.
//       - Iteration 3 (exp = 2, even): result stays 4.0, exp shifts right to 1, and
//         base squares to 256.0.
//       - Iteration 4 (exp = 1, odd): result becomes 4.0 * 256.0 = 1024.0, exp
//         shifts right to 0, and the loop terminates returning 1024.0.
//
//   Example 2:
//     Input:  x = 2.0, n = -2
//     Output: 0.25
//     Explanation:
//       - Because exp = -2 < 0, the base is inverted to base = 1.0 / 2.0 = 0.5 and
//         exp is negated to 2, with result = 1.0.
//       - Iteration 1 (exp = 2, even): result remains 1.0, exp shifts right to 1,
//         and base squares to 0.5 * 0.5 = 0.25.
//       - Iteration 2 (exp = 1, odd): result multiplies by 0.25 to become 0.25, exp
//         shifts right to 0, and the loop ends returning 0.25.
//
//   Example 3:
//     Input:  x = -1.0, n = -2147483648
//     Output: 1.0
//     Explanation:
//       - Widening n to 64-bit exp = -2147483648 prevents signed overflow when exp
//         < 0 inverts base to 1.0 / -1.0 = -1.0 and negates exp to 2147483648
//         (2^31).
//       - During the 31 iterations where exp decreases from 2^31 down to 2, (exp &
//         1) == 0 keeps result = 1.0 while base squares from -1.0 to 1.0 on the
//         first step and remains 1.0 thereafter.
//       - On the final iteration (exp = 1), (exp & 1) != 0 multiplies result = 1.0
//         by base = 1.0, then exp becomes 0 and the function returns 1.0.
//
// Algorithm:
//   Iterative binary exponentiation (exponentiation by squaring). First widen
//   `n` to `std::int64_t` so negating `INT_MIN` (`-2^31`) does not overflow.
//   If the exponent is negative, invert the base (`base = 1.0 / base`) and
//   negate the exponent. Then decompose the non-negative exponent in binary:
//   whenever the least-significant bit of `exp` is 1, multiply `result` by the
//   current power `base`; shift `exp` right by 1 and square `base` whenever
//   remaining bits exist. This computes `x^n` in O(log |n|) multiplications
//   instead of O(|n|).
//   Time Complexity: O(log |n|).
//   Space Complexity: O(1).
//
// Data Structures:
//   Scalar floating-point values (`base`, `result`) and a 64-bit signed
//   integer (`exp`) for safe exponent negation and bit shifting.
//
// Real-World Applications:
//   - Modular exponentiation in public-key cryptography (RSA, Diffie-Hellman,
//     elliptic-curve scalar multiplication via double-and-add).
//   - Fast matrix exponentiation for linear recurrences, Markov-chain steady
//     states, and graph path counting in O(k^3 log n).
//   - Compound interest, geometric decay, and numerical math libraries.
//
// Related LeetCode Problems:
//   - #69 Sqrtx [done] (fast logarithmic-time numerical math routine)
//   - #29 Divide Two Integers [done] (binary doubling with INT_MIN handling)
//   - #372 Super Pow [new] (modular exponentiation with huge array-encoded exponents)
//   - #1922 Count Good Numbers [new] (modular binary exponentiation over combinatorics)

#include "framework/lc_framework.h"

#include <cmath>
#include <cstdint>
#include <limits>

namespace {

bool approxEqual(double a, double b, double eps = 1e-5) {
  return std::fabs(a - b) <= eps;
}

class Solution {
 public:
  // Computes `x` raised to integer power `n` via iterative binary
  // exponentiation (exponentiation by squaring).
  // Handles `n == 0` (including `0^0 == 1.0`) and `n == INT_MIN` safely.
  // Time: O(log |n|), Space: O(1).
  double myPow(double x, int n) {
    // Widen to 64-bit before negating so `n = INT_MIN` (-2^31) cannot overflow.
    std::int64_t exp = n;
    double base = x;

    if (exp < 0) {
      base = 1.0 / base;
      exp = -exp;
    }

    double result = 1.0;

    // Loop invariant: the desired answer equals `result * (base ^ exp)`.
    while (exp > 0) {
      if ((exp & 1) != 0) {
        result *= base;
      }
      exp >>= 1;
      if (exp > 0) {
        base *= base;
      }
    }

    return result;
  }
};

LC_PROBLEM(50, "Powx N", "math", ctx) {
  Solution sol;

  ctx.test("standard_examples", [&] {
    ctx.expect_true(approxEqual(sol.myPow(2.0, 10), 1024.0));
    ctx.expect_true(approxEqual(sol.myPow(2.1, 3), 9.261));
    ctx.expect_true(approxEqual(sol.myPow(2.0, -2), 0.25));
  });

  ctx.test("zero_and_unit_bases", [&] {
    ctx.expect_true(approxEqual(sol.myPow(0.0, 0), 1.0));
    ctx.expect_true(approxEqual(sol.myPow(0.0, 5), 0.0));
    ctx.expect_true(approxEqual(sol.myPow(5.0, 0), 1.0));
    ctx.expect_true(approxEqual(sol.myPow(-2.0, 3), -8.0));
    ctx.expect_true(approxEqual(sol.myPow(-2.0, 4), 16.0));
  });

  ctx.test("extreme_exponents", [&] {
    constexpr int kIntMax = std::numeric_limits<int>::max();
    constexpr int kIntMin = std::numeric_limits<int>::min();

    // Case from original Python script.
    ctx.expect_true(approxEqual(sol.myPow(0.00001, kIntMax), 0.0));
    ctx.expect_true(approxEqual(sol.myPow(1.0, kIntMin), 1.0));
    ctx.expect_true(approxEqual(sol.myPow(-1.0, kIntMin), 1.0));
    ctx.expect_true(approxEqual(sol.myPow(-1.0, kIntMax), -1.0));
    ctx.expect_true(approxEqual(sol.myPow(2.0, kIntMin), 0.0));
  });
}

}  // namespace
