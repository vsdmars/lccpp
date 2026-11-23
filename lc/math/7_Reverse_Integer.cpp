// LeetCode 7: Reverse Integer
// https://leetcode.com/problems/reverse-integer/
//
// Problem:
//   Given a signed 32-bit integer `x`, return `x` with its base-10 digits
//   reversed while preserving its sign. If reversing the digits causes the
//   result to fall outside the signed 32-bit integer range
//   [-2^31, 2^31 - 1], return 0 instead. The environment is assumed not to
//   support 64-bit integers.
//
// Examples:
//   Example 1:
//     Input:  x = 123
//     Output: 321
//     Explanation:
//       - Initialize `result = 0` alongside overflow bounds `kMaxDiv10 = 214748364`
//         (`kMaxMod10 = 7`) and `kMinDiv10 = -214748364` (`kMinMod10 = -8`).
//       - Iteration 1 extracts `digit = 123 % 10 = 3`, updates `x = 12`, passes
//         overflow checks, and sets `result = 0 * 10 + 3 = 3`.
//       - Iteration 2 extracts `digit = 12 % 10 = 2`, updates `x = 1`, passes
//         overflow checks, and sets `result = 3 * 10 + 2 = 32`.
//       - Iteration 3 extracts `digit = 1 % 10 = 1`, updates `x = 0`, passes
//         overflow checks, and sets `result = 32 * 10 + 1 = 321`.
//       - Exit the loop when `x == 0` and return `321`.
//
//   Example 2:
//     Input:  x = -123
//     Output: -321
//     Explanation:
//       - Initialize `result = 0`; C++ truncating division toward zero produces
//         negative digits directly without negating `x`.
//       - Iteration 1 extracts `digit = -123 % 10 = -3`, sets `x = -12`, passes
//         overflow checks, and updates `result = -3`.
//       - Iteration 2 extracts `digit = -12 % 10 = -2`, sets `x = -1`, passes
//         overflow checks, and updates `result = -3 * 10 + (-2) = -32`.
//       - Iteration 3 extracts `digit = -1 % 10 = -1`, sets `x = 0`, passes
//         overflow checks, and updates `result = -32 * 10 + (-1) = -321`.
//       - Exit the loop when `x == 0` and return `-321`.
//
//   Example 3:
//     Input:  x = 1534236469
//     Output: 0
//     Explanation:
//       - Over the first 9 loop iterations, pop digits `9, 6, 4, 6, 3, 2, 4, 3, 5`
//         from `1534236469`, leaving `x = 1` and accumulating `result = 964632435`.
//       - On iteration 10, pop `digit = 1 % 10 = 1` and update `x = 0`.
//       - Check `result > kMaxDiv10`: `964632435 > 214748364` is true, so return
//         `0` immediately before overflow occurs.
//
// Algorithm:
//   Pop the least-significant base-10 digit of `x` repeatedly via modulo 10
//   and truncating division by 10, pushing each digit onto the right end of
//   an accumulator `result = (result * 10) + digit`. Because C++ signed
//   division and remainder truncate toward zero, negative inputs yield
//   negative digits directly and never require negating `INT_MIN` (which
//   would trigger signed overflow). Before every multiply-and-add step, check
//   whether `result` would exceed `INT_MAX / 10` or drop below `INT_MIN / 10`
//   (including the boundary last-digit checks `+7` and `-8`), returning 0
//   immediately on impending overflow.
//   Time Complexity: O(log10 |x|) (at most 10 iterations for 32-bit integers).
//   Space Complexity: O(1).
//
// Data Structures:
//   Scalar 32-bit signed integers (`int`) for the remaining input `x`,
//   extracted `digit`, and reversed accumulator `result`.
//
// Real-World Applications:
//   - Overflow-safe decimal parsing and serialization in embedded systems
//     and financial ledgers restricted to fixed-width arithmetic.
//   - Checksum/barcode digit manipulation and hardware-register BCD decoding.
//   - Compiler constant-folding and static-analysis overflow guards.
//
// Related LeetCode Problems:
//   - #8 String to Integer [done] (atoi) (32-bit overflow checking during base-10 accumulation)
//   - #9 Palindrome Number [done] (reversing half of a base-10 integer without overflow)
//   - #190 Reverse Bits [done] (bit-level analogue in base 2)
//   - #29 Divide Two Integers [done] (32-bit signed range and INT_MIN boundary handling)

#include "framework/lc_framework.h"

#include <limits>

namespace {

class Solution {
 public:
  // Reverses the base-10 digits of a signed 32-bit integer `x` using strictly
  // 32-bit arithmetic. Returns 0 if the reversed value would overflow the
  // signed 32-bit range [INT_MIN, INT_MAX].
  // Time: O(log10 |x|), Space: O(1).
  int reverse(int x) {
    constexpr int kMaxDiv10 = std::numeric_limits<int>::max() / 10;
    constexpr int kMaxMod10 = std::numeric_limits<int>::max() % 10;  // 7
    constexpr int kMinDiv10 = std::numeric_limits<int>::min() / 10;
    constexpr int kMinMod10 = std::numeric_limits<int>::min() % 10;  // -8

    int result = 0;

    // Extract digits from least-significant to most-significant. For negative
    // `x`, C++ guarantees `x % 10 <= 0`, so we accumulate directly toward
    // negative values without ever calling `abs(INT_MIN)`.
    while (x != 0) {
      const int digit = x % 10;
      x /= 10;

      // Guard against positive overflow before computing (result * 10) + digit.
      if (result > kMaxDiv10 || (result == kMaxDiv10 && digit > kMaxMod10)) {
        return 0;
      }

      // Guard against negative overflow before computing (result * 10) + digit.
      if (result < kMinDiv10 || (result == kMinDiv10 && digit < kMinMod10)) {
        return 0;
      }

      result = (result * 10) + digit;
    }

    return result;
  }
};

LC_PROBLEM(7, "Reverse Integer", "math", ctx) {
  Solution sol;

  ctx.test("examples", [&] {
    ctx.expect_eq(sol.reverse(123), 321);
    ctx.expect_eq(sol.reverse(-123), -321);
    ctx.expect_eq(sol.reverse(120), 21);
    ctx.expect_eq(sol.reverse(0), 0);
  });

  ctx.test("source_cases", [&] {
    ctx.expect_eq(sol.reverse(1534236469), 0);
    ctx.expect_eq(sol.reverse(-1234), -4321);
  });

  ctx.test("single_digits_and_trailing_zeros", [&] {
    ctx.expect_eq(sol.reverse(7), 7);
    ctx.expect_eq(sol.reverse(-8), -8);
    ctx.expect_eq(sol.reverse(10000), 1);
    ctx.expect_eq(sol.reverse(-10000), -1);
  });

  ctx.test("int32_boundaries", [&] {
    // Largest and smallest reversible 10-digit values within 32-bit bounds.
    ctx.expect_eq(sol.reverse(1463847412), 2147483641);
    ctx.expect_eq(sol.reverse(-1463847412), -2147483641);
    // Exact boundary values whose reversals overflow 32-bit signed int.
    ctx.expect_eq(sol.reverse(std::numeric_limits<int>::max()), 0);
    ctx.expect_eq(sol.reverse(std::numeric_limits<int>::min()), 0);
    // Boundary last-digit overflow cases (8 > 7 and -9 < -8).
    ctx.expect_eq(sol.reverse(846384741), 147483648);
    ctx.expect_eq(sol.reverse(1563847412), 0);
    ctx.expect_eq(sol.reverse(-1563847412), 0);
  });
}

}  // namespace
