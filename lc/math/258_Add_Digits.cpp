// LeetCode 258: Add Digits
// https://leetcode.com/problems/add-digits/
//
// Problem:
//   Given a non-negative integer `num` (0 <= num <= 2^31 - 1), repeatedly add
//   all of its decimal digits until the result has only a single digit, and
//   return that digit. The follow-up asks for an O(1) time solution without
//   any loop or recursion.
//
// Algorithm:
//   Use the congruence property of the base-10 digital root. Because
//   10^k === 1 (mod 9) for every k >= 0, a number's decimal representation
//   sum(d_k * 10^k) is congruent to the sum of its digits sum(d_k) modulo 9.
//   Repeating digit summation preserves the remainder modulo 9 until a single
//   digit remains:
//     - If num == 0, the digital root is 0.
//     - If num > 0 and num % 9 == 0, the single non-zero digit congruent to 0
//       (mod 9) is 9.
//     - Otherwise, the digital root is num % 9.
//   Time Complexity: O(1).
//   Space Complexity: O(1).
//
// Data Structures:
//   None; operates purely on scalar integers in constant space.
//
// Real-World Applications:
//   Casting out nines for lightweight arithmetic error checking, checksum digit
//   design (such as ISBN/IBAN/Verhoeff-style modular checks), and hash-bucket
//   folding in constrained embedded hardware.
//
// Related LeetCode Problems:
//   - #202 Happy Number [done] (repeated digit-based transformation of an integer)
//   - #1085 Sum of Digits in the Minimum Number [new] (digit-sum parity check)
//   - #1945 Sum of Digits of String After Convert [new] (bounded k-round digit summation)
//   - #2160 Minimum Sum of Four Digit Number After Splitting Digits [new] (decimal digit manipulation)

#include "framework/lc_framework.h"

#include <limits>

namespace {

class Solution {
 public:
  // Computes the base-10 digital root of `num` in O(1) time and O(1) space
  // using modular arithmetic modulo 9.
  int addDigits(int num) {
    // Zero is the unique non-negative integer whose digital root is 0.
    if (num == 0) {
      return 0;
    }

    // Because 10^k === 1 (mod 9), repeated digit summation is invariant mod 9.
    // Non-zero multiples of 9 converge to 9 rather than 0.
    const int remainder = num % 9;
    return remainder == 0 ? 9 : remainder;
  }
};

LC_PROBLEM(258, "Add Digits", "math", ctx) {
  ctx.test("example_38", [&] {
    Solution sol;
    ctx.expect_eq(sol.addDigits(38), 2);
  });

  ctx.test("build_multiple_of_9", [&] {
    Solution sol;
    ctx.expect_eq(sol.addDigits(18), 9);
  });

  ctx.test("zero", [&] {
    Solution sol;
    ctx.expect_eq(sol.addDigits(0), 0);
  });

  ctx.test("single_digits", [&] {
    Solution sol;
    for (int d = 1; d <= 9; ++d) {
      ctx.expect_eq(sol.addDigits(d), d);
    }
  });

  ctx.test("large_values", [&] {
    Solution sol;
    ctx.expect_eq(sol.addDigits(999999), 9);
    // 2147483647 -> digit sum 46 -> 10 -> 1.
    ctx.expect_eq(sol.addDigits(std::numeric_limits<int>::max()), 1);
  });
}

}  // namespace
