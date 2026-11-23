// LeetCode 8: String To Integer Atoi
// https://leetcode.com/problems/string-to-integer-atoi/
//
// Problem:
//   Implement `myAtoi(string s)`, which parses a 32-bit signed integer from a
//   string according to the following rules:
//   1. Discard leading space characters (`' '` only).
//   2. Read an optional `'+'` or `'-'` sign to determine polarity.
//   3. Consume consecutive decimal digits (`'0'`-`'9'`), ignoring any trailing
//      non-digit characters. If no digits are found, return 0.
//   4. Clamp the resulting integer into the signed 32-bit range
//      `[-2^31, 2^31 - 1]` (`[-2147483648, 2147483647]`).
//
// Algorithm:
//   Single-pass deterministic linear scan with pre-multiplication overflow
//   detection:
//   1. Advance index `i` past leading `' '` characters.
//   2. Check `s[i]` for an optional `'+'` or `'-'` sign and record `negative`.
//   3. While `s[i]` is an ASCII digit, check whether `(result * 10) + digit`
//      would exceed `INT_MAX` (`2147483647`) BEFORE multiplying:
//        `result > INT_MAX / 10 || (result == INT_MAX / 10 && digit > 7)`.
//      If so, immediately return `negative ? INT_MIN : INT_MAX`. Note that when
//      `negative` is true and the exact value is `-2147483648` (`digit == 8`),
//      this guard triggers and returns `INT_MIN` directly without ever invoking
//      signed integer overflow (undefined behavior in C++).
//   4. Return `negative ? -result : result`.
//
//   Time Complexity:  O(n), where n = s.size(), visiting each character at
//                     most once.
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - Scalar cursor index (`std::size_t`) and 32-bit accumulator (`int`) over a
//     non-owning `std::string_view`.
//
// Real-World Applications:
//   - Lexers and tokenizers for compilers, JSON/YAML/TOML parsers, and SQL
//     engines.
//   - CLI argument and environment variable parsing with saturating bounds.
//   - Network protocol header parsing (e.g., HTTP `Content-Length`, SIP/SMTP
//     status codes) where overflow safety is security-critical.
//
// Related LeetCode Problems:
//   - #7 Reverse Integer [done] (32-bit signed pre-multiplication overflow guard)
//   - #65 Valid Number [done] (full DFA parsing for numeric literals)
//   - #273 Integer to English Words [done] (inverse integer-to-text formatting)
//   - #415 Add Strings [done] (ASCII digit-by-digit decimal arithmetic)

#include "framework/lc_framework.h"

#include <cstddef>
#include <limits>
#include <string_view>

namespace {

class Solution {
 public:
  // Parses a leading signed 32-bit integer from `s`, clamping out-of-range
  // values to `[INT_MIN, INT_MAX]` without triggering signed overflow.
  // Time: O(n), Space: O(1).
  int myAtoi(std::string_view s) {
    constexpr int kMaxInt = std::numeric_limits<int>::max();  //  2147483647
    constexpr int kMinInt = std::numeric_limits<int>::min();  // -2147483648
    constexpr int kOverflowCutoff = kMaxInt / 10;             //  214748364
    constexpr int kMaxLastDigit = kMaxInt % 10;               //  7

    const std::size_t n = s.size();
    std::size_t i = 0uz;

    // Phase 1: Skip leading ASCII spaces only (per LeetCode specification).
    while (i < n && s[i] == ' ') {
      ++i;
    }

    if (i == n) {
      return 0;
    }

    // Phase 2: Consume an optional leading sign character.
    bool negative = false;
    if (s[i] == '+' || s[i] == '-') {
      negative = (s[i] == '-');
      ++i;
    }

    // Phase 3: Accumulate digits with pre-overflow clamping.
    // Invariant: `result` stays within `[0, kMaxInt]`, so `-result` is always
    // representable without overflow when returning at the end.
    int result = 0;
    while (i < n && s[i] >= '0' && s[i] <= '9') {
      const int digit = static_cast<unsigned char>(s[i]) -
                        static_cast<unsigned char>('0');

      // Check overflow before computing `(result * 10) + digit`. For negative
      // inputs reaching magnitude 2147483648 (`digit == 8`), clamping returns
      // `kMinInt`, which is also the exact value.
      if (result > kOverflowCutoff ||
          (result == kOverflowCutoff && digit > kMaxLastDigit)) {
        return negative ? kMinInt : kMaxInt;
      }

      result = (result * 10) + digit;
      ++i;
    }

    return negative ? -result : result;
  }
};

LC_PROBLEM(8, "String To Integer Atoi", "string", ctx) {
  Solution sol;

  ctx.test("examples_from_statement", [&] {
    ctx.expect_eq(sol.myAtoi("42"), 42);
    ctx.expect_eq(sol.myAtoi("   -42"), -42);
    ctx.expect_eq(sol.myAtoi("4193 with words"), 4193);
    ctx.expect_eq(sol.myAtoi("words and 987"), 0);
    ctx.expect_eq(sol.myAtoi("-91283472332"), std::numeric_limits<int>::min());
  });

  ctx.test("signs_and_leading_zeros", [&] {
    ctx.expect_eq(sol.myAtoi("+123"), 123);
    ctx.expect_eq(sol.myAtoi("+words and 987"), 0);
    ctx.expect_eq(sol.myAtoi("   +000042"), 42);
    ctx.expect_eq(sol.myAtoi("-000"), 0);
    ctx.expect_eq(sol.myAtoi("+-12"), 0);
  });

  ctx.test("int32_boundaries_and_clamping", [&] {
    constexpr int kMax = std::numeric_limits<int>::max();
    constexpr int kMin = std::numeric_limits<int>::min();
    ctx.expect_eq(sol.myAtoi("2147483647"), kMax);
    ctx.expect_eq(sol.myAtoi("2147483648"), kMax);
    ctx.expect_eq(sol.myAtoi("-2147483647"), -2147483647);
    ctx.expect_eq(sol.myAtoi("-2147483648"), kMin);
    ctx.expect_eq(sol.myAtoi("-2147483649"), kMin);
    ctx.expect_eq(sol.myAtoi("999999999999999999999999"), kMax);
  });

  ctx.test("empty_whitespace_and_non_space_prefix", [&] {
    ctx.expect_eq(sol.myAtoi(""), 0);
    ctx.expect_eq(sol.myAtoi("    "), 0);
    ctx.expect_eq(sol.myAtoi("+"), 0);
    ctx.expect_eq(sol.myAtoi("-"), 0);
    ctx.expect_eq(sol.myAtoi("\t42"), 0);
  });
}

}  // namespace
