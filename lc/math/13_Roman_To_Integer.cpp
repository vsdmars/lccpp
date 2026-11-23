// LeetCode 13: Roman To Integer
// https://leetcode.com/problems/roman-to-integer/
//
// Problem:
//   Given a valid Roman numeral string `s` representing an integer in the
//   range [1, 3999], convert it into its corresponding integer value. Roman
//   numerals normally place larger symbols before smaller ones, except in the
//   six subtractive pairs (IV, IX, XL, XC, CD, CM) where a smaller symbol
//   precedes a larger one.
//
// Examples:
//   Example 1:
//     Input:  s = "LVIII"
//     Output: 58
//     Explanation:
//       - At i = 0 ('L'=50, 'V'=5), curr >= next adds 50 to total (total = 50).
//       - At i = 1 ('V'=5, 'I'=1), curr >= next adds 5 to total (total = 55).
//       - At i = 2 and i = 3 ('I'=1, 'I'=1), curr >= next adds 1 twice (total =
//         57).
//       - Add the trailing symbol s.back() = 'I' (1) after the loop to return 58.
//
//   Example 2:
//     Input:  s = "MCMXCIV"
//     Output: 1994
//     Explanation:
//       - At i = 0 ('M'=1000, 'C'=100), add 1000 (total = 1000); at i = 1 ('C'=100,
//         'M'=1000), 100 < 1000 subtracts 100 (total = 900).
//       - At i = 2 ('M'=1000, 'X'=10), add 1000 (total = 1900); at i = 3 ('X'=10,
//         'C'=100), 10 < 100 subtracts 10 (total = 1890).
//       - At i = 4 ('C'=100, 'I'=1), add 100 (total = 1990); at i = 5 ('I'=1,
//         'V'=5), 1 < 5 subtracts 1 (total = 1989).
//       - Add the final character 'V' (5) outside the loop to get 1989 + 5 = 1994.
//
//   Example 3:
//     Input:  s = "I"
//     Output: 1
//     Explanation:
//       - Since s.size() == 1, the lookahead loop condition i + 1 < s.size() is
//         false immediately and total remains 0.
//       - Add symbolValue(s.back()) = symbolValue('I') = 1 and return 1.
//
// Algorithm:
//   Scan `s` from left to right comparing each symbol's numeric value `curr`
//   with the next symbol's value `next`. Whenever `curr < next`, the current
//   symbol participates in a subtractive pair and contributes `-curr` to the
//   running total; otherwise it contributes `+curr`. The final character has
//   no successor and is always added positively.
//   Time Complexity: O(n), where n = s.size() (bounded by 15 for valid inputs).
//   Space Complexity: O(1).
//
// Data Structures:
//   - Stateless `constexpr` character-to-value switch lookup (avoids heap
//     allocation and hash-table overhead).
//   - Scalar integer accumulator `total`.
//
// Real-World Applications:
//   - Lexical scanners and parsers with 1-token lookahead (LL(1) parsing).
//   - Document ingestion pipelines normalizing outlines, historical dates,
//     legal statutes, and bibliographic numbering into sortable integers.
//
// Related LeetCode Problems:
//   - #12 Integer To Roman [done] (inverse encoder from integer to Roman numeral)
//   - #171 Excel Sheet Column Number [done] (string-to-integer positional numeral decoding)
//   - #8 String to Integer [done] (atoi) (linear-scan numeral string parsing)
//   - #224 Basic Calculator [done] (sign-aware expression parsing with lookahead)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string_view>

namespace {

class Solution {
 public:
  // Converts a valid Roman numeral string `s` to its integer value using
  // one-character lookahead to distinguish additive vs. subtractive symbols.
  // Time: O(n), Space: O(1).
  int romanToInt(std::string_view s) {
    if (s.empty()) {
      return 0;
    }

    int total = 0;

    // Compare each symbol against its immediate right neighbor: if smaller,
    // subtract it; otherwise add it.
    for (std::size_t i = 0uz; i + 1uz < s.size(); ++i) {
      const int curr = symbolValue(s[i]);
      const int next = symbolValue(s[i + 1uz]);
      if (curr < next) {
        total -= curr;
      } else {
        total += curr;
      }
    }

    // The trailing symbol has no successor and is always additive.
    total += symbolValue(s.back());
    return total;
  }

 private:
  // Maps a single Roman numeral character to its integer value.
  static constexpr int symbolValue(char symbol) {
    switch (symbol) {
      case 'I':
        return 1;
      case 'V':
        return 5;
      case 'X':
        return 10;
      case 'L':
        return 50;
      case 'C':
        return 100;
      case 'D':
        return 500;
      case 'M':
        return 1000;
      default:
        return 0;
    }
  }
};

LC_PROBLEM(13, "Roman To Integer", "math", ctx) {
  Solution sol;

  ctx.test("standard_examples", [&] {
    ctx.expect_eq(sol.romanToInt("III"), 3);
    ctx.expect_eq(sol.romanToInt("IV"), 4);
    ctx.expect_eq(sol.romanToInt("IX"), 9);
    ctx.expect_eq(sol.romanToInt("LVIII"), 58);
    ctx.expect_eq(sol.romanToInt("MCMXCIV"), 1994);
  });

  ctx.test("source_cases", [&] {
    ctx.expect_eq(sol.romanToInt("DCXXIV"), 624);
    ctx.expect_eq(sol.romanToInt("DCXXI"), 621);
    ctx.expect_eq(sol.romanToInt("MCMXCVI"), 1996);
  });

  ctx.test("boundaries", [&] {
    ctx.expect_eq(sol.romanToInt("I"), 1);
    ctx.expect_eq(sol.romanToInt("MMMDCCCLXXXVIII"), 3888);
    ctx.expect_eq(sol.romanToInt("MMMCMXCIX"), 3999);
  });
}

}  // namespace
