// LeetCode 171: Excel Sheet Column Number
// https://leetcode.com/problems/excel-sheet-column-number/
//
// Problem:
//   Given a string `columnTitle` representing a column title as it appears in
//   an Excel sheet (1 <= columnTitle.length <= 7, uppercase letters 'A'-'Z',
//   in the range ["A", "FXSHRXW"]), return its corresponding 1-based column
//   number ("A" -> 1, "Z" -> 26, "AA" -> 27, "ZY" -> 701).
//
// Examples:
//   Example 1:
//     Input:  columnTitle = "AB"
//     Output: 28
//     Explanation:
//       - Initialize total = 0.
//       - Process 'A' with digit = ('A' - 'A') + 1 = 1: total = 0 * 26 + 1 = 1.
//       - Process 'B' with digit = ('B' - 'A') + 1 = 2: total = 1 * 26 + 2 = 28,
//         and return 28.
//
//   Example 2:
//     Input:  columnTitle = "ZY"
//     Output: 701
//     Explanation:
//       - Initialize total = 0.
//       - Process 'Z' with digit = ('Z' - 'A') + 1 = 26: total = 0 * 26 + 26 = 26.
//       - Process 'Y' with digit = ('Y' - 'A') + 1 = 25: total = 26 * 26 + 25 =
//         701, and return 701.
//
//   Example 3:
//     Input:  columnTitle = "ABV"
//     Output: 750
//     Explanation:
//       - Initialize total = 0 and process 'A' (digit = 1) to set total = 0 * 26 +
//         1 = 1.
//       - Process 'B' (digit = 2) to update total = 1 * 26 + 2 = 28.
//       - Process 'V' (digit = 22) to update total = 28 * 26 + 22 = 750, and return
//         750.
//
// Algorithm:
//   Bijective base-26 positional decoding via Horner's method.
//   Scan `columnTitle` from most-significant character to least-significant
//   character. Each letter `ch` maps directly to its digit value
//   `(ch - 'A') + 1` in [1, 26]. On each character, shift the running total
//   left by one base-26 place and add the current digit:
//       total = total * 26 + ((ch - 'A') + 1).
//   Using Horner's rule avoids exponentiation and auxiliary lookup tables.
//   Time Complexity:  O(L), where L = columnTitle.size().
//   Space Complexity: O(1).
//
// Data Structures:
//   - std::string_view: zero-copy read-only view over the input column title.
//   - Scalar integer accumulator (`total`): holds the decoded base-10 value.
//
// Real-World Applications:
//   - Spreadsheet formula parsers translating A1-style cell references (e.g.
//     "AB12:ZY99") into integer matrix column offsets.
//   - Parsing bijective base-26 or base-62 identifiers in short-link resolvers
//     and distributed ID decoders.
//   - Compiler lexers evaluating radix-encoded numeric literals with Horner's
//     scheme in constant space.
//
// Related LeetCode Problems:
//   - #168 Excel Sheet Column Title [done] (exact inverse: integer to column title)
//   - #13 Roman to Integer [done] (left-to-right symbolic numeral decoding)
//   - #8 String to Integer [done] (atoi) (base-10 Horner accumulation from strings)
//   - #2194 Cells in a Range on an Excel Sheet [new] (manipulating Excel column ranges)

#include "framework/lc_framework.h"

#include <limits>
#include <string_view>

namespace {

constexpr int kAlphabetSize = 26;

class Solution {
 public:
  // Decodes an Excel column title into its 1-based positive integer index.
  //
  // Invariant: after processing prefix `column_title[0..i]`, `total` equals
  // the exact bijective base-26 integer value of that prefix.
  //
  // Time: O(L), Space: O(1).
  int titleToNumber(std::string_view column_title) {
    int total = 0;

    // Horner's method: each new character shifts all prior digits by a factor
    // of 26 and adds the 1-based digit value ('A' -> 1, ..., 'Z' -> 26).
    for (const char ch : column_title) {
      const int digit = (ch - 'A') + 1;
      total = (total * kAlphabetSize) + digit;
    }

    return total;
  }
};

LC_PROBLEM(171, "Excel Sheet Column Number", "math", ctx) {
  Solution sol;

  ctx.test("examples", [&] {
    ctx.expect_eq(sol.titleToNumber("A"), 1);
    ctx.expect_eq(sol.titleToNumber("AB"), 28);
    ctx.expect_eq(sol.titleToNumber("ZY"), 701);
  });

  ctx.test("edge_and_carry_cases", [&] {
    ctx.expect_eq(sol.titleToNumber(""), 0);
    ctx.expect_eq(sol.titleToNumber("Z"), 26);
    ctx.expect_eq(sol.titleToNumber("AA"), 27);
    ctx.expect_eq(sol.titleToNumber("AZ"), 52);
    ctx.expect_eq(sol.titleToNumber("ZZ"), 702);
    ctx.expect_eq(sol.titleToNumber("AAA"), 703);
    ctx.expect_eq(sol.titleToNumber("ABV"), 750);
  });

  ctx.test("max_32bit_title", [&] {
    ctx.expect_eq(sol.titleToNumber("FXSHRXW"),
                  std::numeric_limits<int>::max());
  });
}

}  // namespace
