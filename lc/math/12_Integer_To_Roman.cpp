// LeetCode 12: Integer To Roman
// https://leetcode.com/problems/integer-to-roman/
//
// Problem:
//   Given an integer `num` in the range [1, 3999], convert it to its canonical
//   Roman numeral string using the seven standard symbols (I=1, V=5, X=10,
//   L=50, C=100, D=500, M=1000) and the six subtractive forms (IV=4, IX=9,
//   XL=40, XC=90, CD=400, CM=900).
//
// Algorithm:
//   Greedy decomposition over a fixed descending table of all 13 canonical
//   Roman value-symbol pairs (the seven base symbols interleaved with the six
//   subtractive pairs). For each entry `(value, symbol)`, divide the remaining
//   `num` by `value` to obtain the exact repeat count `q = num / value`,
//   append `symbol` `q` times to the result buffer, and replace `num` with
//   `num % value`. Because the Roman numeral system is canonical under greedy
//   selection once the six subtractive pairs are included, this produces the
//   unique minimal representation in a single fixed-length pass.
//   Time Complexity: O(1) (at most 13 table entries and at most 15 output
//     characters for any input in [1, 3999]).
//   Space Complexity: O(1) auxiliary space (excluding the returned string).
//
// Data Structures:
//   - `std::array<RomanToken, 13>`: compile-time lookup table pairing each
//     integer threshold with its `std::string_view` Roman numeral token.
//   - `std::string`: output buffer pre-reserved for the final numeral.
//
// Real-World Applications:
//   - Localization, formatting, and typography engines (chapter/section
//     numbering, legal citations, clock faces, and publishing toolchains).
//   - Canonical greedy denomination decomposition (currency change making
//     for canonical coin systems, human-readable duration/unit formatting).
//
// Related LeetCode Problems:
//   - #13 Roman To Integer [done] (inverse parser converting Roman numerals back to integers)
//   - #273 Integer to English Words [done] (hierarchical base-1000 numeral formatting)
//   - #168 Excel Sheet Column Title [done] (bijective base-26 integer-to-string conversion)
//   - #322 Coin Change [done] (generalized denomination decomposition where greedy fails)

#include "framework/lc_framework.h"

#include <array>
#include <string>
#include <string_view>

namespace {

struct RomanToken {
  int value;
  std::string_view symbol;
};

// All 13 canonical Roman numeral tokens (base + subtractive) in descending
// order so greedy subtraction always yields the unique standard form.
constexpr std::array<RomanToken, 13> kRomanTable{{
    {.value = 1000, .symbol = "M"},
    {.value = 900, .symbol = "CM"},
    {.value = 500, .symbol = "D"},
    {.value = 400, .symbol = "CD"},
    {.value = 100, .symbol = "C"},
    {.value = 90, .symbol = "XC"},
    {.value = 50, .symbol = "L"},
    {.value = 40, .symbol = "XL"},
    {.value = 10, .symbol = "X"},
    {.value = 9, .symbol = "IX"},
    {.value = 5, .symbol = "V"},
    {.value = 4, .symbol = "IV"},
    {.value = 1, .symbol = "I"},
}};

class Solution {
 public:
  // Converts an integer in [1, 3999] into its canonical Roman numeral string
  // by greedily consuming the largest fitting token from `kRomanTable`.
  // Time: O(1), Space: O(1) auxiliary.
  std::string intToRoman(int num) {
    std::string result;
    // Longest valid Roman numeral in [1, 3999] is 3888 ("MMMDCCCLXXXVIII", 15).
    result.reserve(15);

    for (const auto& [value, symbol] : kRomanTable) {
      if (num == 0) {
        break;
      }
      if (num >= value) {
        const int repeat = num / value;
        num %= value;
        for (int i = 0; i < repeat; ++i) {
          result.append(symbol);
        }
      }
    }

    return result;
  }
};

LC_PROBLEM(12, "Integer To Roman", "math", ctx) {
  Solution sol;

  ctx.test("examples", [&] {
    ctx.expect_eq(sol.intToRoman(3), std::string("III"));
    ctx.expect_eq(sol.intToRoman(4), std::string("IV"));
    ctx.expect_eq(sol.intToRoman(9), std::string("IX"));
    ctx.expect_eq(sol.intToRoman(58), std::string("LVIII"));
    ctx.expect_eq(sol.intToRoman(1994), std::string("MCMXCIV"));
  });

  ctx.test("boundaries_and_subtractive_pairs", [&] {
    ctx.expect_eq(sol.intToRoman(1), std::string("I"));
    ctx.expect_eq(sol.intToRoman(40), std::string("XL"));
    ctx.expect_eq(sol.intToRoman(90), std::string("XC"));
    ctx.expect_eq(sol.intToRoman(400), std::string("CD"));
    ctx.expect_eq(sol.intToRoman(900), std::string("CM"));
    ctx.expect_eq(sol.intToRoman(3888), std::string("MMMDCCCLXXXVIII"));
    ctx.expect_eq(sol.intToRoman(3999), std::string("MMMCMXCIX"));
  });
}

}  // namespace
