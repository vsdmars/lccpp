// LeetCode 166: Fraction To Recurring Decimal
// https://leetcode.com/problems/fraction-to-recurring-decimal/
//
// Problem:
//   Given two integers `numerator` and `denominator` representing a rational
//   fraction (denominator != 0, both within signed 32-bit int range), return
//   the decimal representation as a string. If the fractional part repeats
//   infinitely, enclose the repeating cycle in parentheses (e.g. 2/3 -> "0.(6)",
//   4/333 -> "0.(012)").
//
// Algorithm:
//   Long division with remainder-position cycle detection.
//   1. Handle zero numerator immediately to avoid producing "-0".
//   2. Determine sign via `(numerator < 0) != (denominator < 0)` and widen both
//      operands to `std::int64_t` before taking `std::abs` so `INT_MIN` does
//      not overflow.
//   3. Emit the integer quotient `num / den`. If `rem = num % den` is zero,
//      the fraction terminates cleanly with no decimal point.
//   4. Otherwise append `'.'` and simulate base-10 long division. A recurring
//      cycle begins the exact moment a non-zero remainder `rem` repeats (not
//      when a quotient digit repeats!). Record each remainder's starting index
//      in the output string inside a hash map; upon revisiting a remainder,
//      insert `'('` at its recorded index, append `')'`, and stop.
//   Time Complexity:  O(d) worst-case, where d = |denominator| (at most d - 1
//                     distinct non-zero remainders exist modulo d).
//   Space Complexity: O(d) for the remainder-to-index map and result string.
//
// Data Structures:
//   - std::unordered_map<std::int64_t, std::size_t>: maps each seen non-zero
//     remainder to the exact character index in the result string where the
//     digit produced from that remainder was placed.
//   - std::string: accumulates the sign, integer part, fractional digits, and
//     cycle parentheses.
//
// Real-World Applications:
//   - Computer algebra systems (Mathematica, SymPy) and exact-arithmetic
//     calculators formatting rational numbers without floating-point truncation.
//   - Financial and accounting software detecting exact vs. repeating unit
//     prices and exchange-rate ratios before rounding rules are applied.
//   - Number theory and cryptography (computing multiplicative orders modulo d
//     via decimal period lengths of 1/d).
//
// Related LeetCode Problems:
//   - #29 Divide Two Integers [done] (integer division with 32-bit INT_MIN overflow handling)
//   - #592 Fraction Addition and Subtraction [new] (exact rational arithmetic and GCD reduction)
//   - #972 Equal Rational Numbers [new] (inverse problem: parsing repeating decimal strings)
//   - #43 Multiply Strings [done] (arbitrary-precision decimal simulation)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <string>
#include <unordered_map>

namespace {

constexpr std::int64_t kDecimalBase = 10;

class Solution {
 public:
  // Converts `numerator / denominator` into its exact decimal string, wrapping
  // any repeating fractional cycle in parentheses.
  //
  // Invariant: before computing the next fractional digit from remainder `rem`,
  // `rem_to_index` contains every previously encountered remainder mapped to
  // the index in `result` where that remainder's quotient digit begins.
  //
  // Time: O(|denominator|), Space: O(|denominator|).
  std::string fractionToDecimal(int numerator, int denominator) {
    // Guard against emitting "-0" when numerator is 0 and denominator < 0.
    if (numerator == 0) {
      return "0";
    }

    std::string result;
    if ((numerator < 0) != (denominator < 0)) {
      result.push_back('-');
    }

    // Widen to 64-bit before taking abs so INT_MIN (-2^31) never overflows.
    const std::int64_t num = std::abs(static_cast<std::int64_t>(numerator));
    const std::int64_t den = std::abs(static_cast<std::int64_t>(denominator));

    // Append integer part and stop early if the division is exact.
    result += std::to_string(num / den);
    std::int64_t rem = num % den;
    if (rem == 0) {
      return result;
    }

    result.push_back('.');
    std::unordered_map<std::int64_t, std::size_t> rem_to_index;

    // Simulate long division one decimal digit at a time until remainder hits 0
    // (terminating decimal) or repeats (periodic cycle).
    while (rem != 0) {
      if (const auto it = rem_to_index.find(rem); it != rem_to_index.end()) {
        // Every digit from `it->second` onward will repeat indefinitely.
        result.insert(it->second, 1uz, '(');
        result.push_back(')');
        break;
      }

      rem_to_index[rem] = result.size();
      rem *= kDecimalBase;
      result.push_back(static_cast<char>('0' + (rem / den)));
      rem %= den;
    }

    return result;
  }
};

LC_PROBLEM(166, "Fraction To Recurring Decimal", "math", ctx) {
  Solution sol;

  ctx.test("terminating_and_integer_examples", [&] {
    ctx.expect_eq(sol.fractionToDecimal(1, 2), "0.5");
    ctx.expect_eq(sol.fractionToDecimal(2, 1), "2");
    ctx.expect_eq(sol.fractionToDecimal(0, -5), "0");
  });

  ctx.test("repeating_decimals", [&] {
    ctx.expect_eq(sol.fractionToDecimal(2, 3), "0.(6)");
    ctx.expect_eq(sol.fractionToDecimal(4, 333), "0.(012)");
    ctx.expect_eq(sol.fractionToDecimal(1, 6), "0.1(6)");
    ctx.expect_eq(sol.fractionToDecimal(1, 99), "0.(01)");
  });

  ctx.test("negative_fractions", [&] {
    ctx.expect_eq(sol.fractionToDecimal(-2, 3), "-0.(6)");
    ctx.expect_eq(sol.fractionToDecimal(-50, 8), "-6.25");
    ctx.expect_eq(sol.fractionToDecimal(7, -12), "-0.58(3)");
    ctx.expect_eq(sol.fractionToDecimal(-1, -2), "0.5");
  });

  ctx.test("int_min_overflow_safety", [&] {
    const int kMinInt = std::numeric_limits<int>::min();
    ctx.expect_eq(sol.fractionToDecimal(kMinInt, -1), "2147483648");
    ctx.expect_eq(sol.fractionToDecimal(-1, kMinInt),
                  "0.0000000004656612873077392578125");
  });
}

}  // namespace
