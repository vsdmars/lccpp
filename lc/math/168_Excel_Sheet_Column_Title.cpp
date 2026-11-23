// LeetCode 168: Excel Sheet Column Title
// https://leetcode.com/problems/excel-sheet-column-title/
//
// Problem:
//   Given a positive integer `columnNumber` (1 <= columnNumber <= 2^31 - 1),
//   return its corresponding spreadsheet column title where 1 -> "A", ...,
//   26 -> "Z", 27 -> "AA", 28 -> "AB", etc.
//
// Examples:
//   Example 1:
//     Input:  columnNumber = 28
//     Output: "AB"
//     Explanation:
//       - Iteration 1 decrements column_number from 28 to 27, computes offset = 27
//         % 26 = 1 to push 'B', and sets column_number = 27 / 26 = 1.
//       - Iteration 2 decrements column_number from 1 to 0, computes offset = 0 %
//         26 = 0 to push 'A', and sets column_number = 0 / 26 = 0.
//       - Reversing the collected string "BA" produces "AB".
//
//   Example 2:
//     Input:  columnNumber = 702
//     Output: "ZZ"
//     Explanation:
//       - Iteration 1 decrements 702 to 701, computes offset = 701 % 26 = 25 to
//         push 'Z' ('A' + 25), and updates column_number = 701 / 26 = 26.
//       - Iteration 2 decrements 26 to 25, computes offset = 25 % 26 = 25 to push
//         'Z', and updates column_number = 25 / 26 = 0.
//       - Reversing "ZZ" returns "ZZ".
//
//   Example 3:
//     Input:  columnNumber = 750
//     Output: "ABV"
//     Explanation:
//       - Iteration 1 decrements 750 to 749, computes 749 % 26 = 21 to push 'V'
//         ('A' + 21), and sets column_number = 749 / 26 = 28.
//       - Iteration 2 decrements 28 to 27, computes 27 % 26 = 1 to push 'B', and
//         sets column_number = 27 / 26 = 1.
//       - Iteration 3 decrements 1 to 0, computes 0 % 26 = 0 to push 'A', and sets
//         column_number = 0 / 26 = 0.
//       - Reversing the collected string "VBA" returns "ABV".
//
// Algorithm:
//   Bijective base-26 conversion.
//   Unlike standard base-26 (which uses digits 0..25), Excel column titles form
//   a bijective numeration system using digits 1..26 ('A'..'Z') with no zero
//   symbol. Subtracting 1 before taking `% 26` and `/ 26` shifts the current
//   least-significant digit from [1, 26] into [0, 25], ensuring multiples of 26
//   (such as 26 -> "Z" and 52 -> "AZ") produce 'Z' and decrement the carry
//   correctly instead of emitting a nonexistent zero digit. Digits are
//   collected least-significant first and reversed at the end.
//   Time Complexity:  O(log_26(n))
//   Space Complexity: O(log_26(n)) for the output string (O(1) auxiliary).
//
// Data Structures:
//   - std::string: buffer collecting base-26 characters in reverse order before
//     an in-place `std::ranges::reverse`.
//
// Real-World Applications:
//   - Spreadsheet engines (Excel, Google Sheets, LibreOffice Calc) mapping
//     internal 1-based column indices to human-readable cell addresses.
//   - URL shorteners, compact alphanumeric ID generators, and inventory /
//     warehouse aisle labeling systems using bijective alphabetic codes.
//   - Partition and shard naming schemes where compact, zero-free lexicographic
//     identifiers are desired.
//
// Related LeetCode Problems:
//   - #171 Excel Sheet Column Number [done] (exact inverse: column title to integer)
//   - #2194 Cells in a Range on an Excel Sheet [new] (enumerating spreadsheet coordinates)
//   - #504 Base 7 [new] (standard positional base conversion)
//   - #12 Integer to Roman [done] (converting positive integers to symbolic numerals)

#include "framework/lc_framework.h"

#include <algorithm>
#include <limits>
#include <string>

namespace {

constexpr int kAlphabetSize = 26;

class Solution {
 public:
  // Converts a 1-based positive column number into its Excel column title.
  //
  // Invariant: at the start of each loop iteration, `column_number` is the
  // remaining positive bijective base-26 prefix still to be encoded, and
  // `result` holds the already-decoded suffix characters in reverse order.
  //
  // Time: O(log_26(column_number)), Space: O(1) extra beyond the output string.
  std::string convertToTitle(int column_number) {
    std::string result;

    while (column_number > 0) {
      // Shift from 1-based [1..26] to 0-based [0..25] before modulo/division
      // so multiples of 26 map cleanly to 'Z' (offset 25) with proper carry.
      --column_number;
      const int offset = column_number % kAlphabetSize;
      result.push_back(static_cast<char>('A' + offset));
      column_number /= kAlphabetSize;
    }

    // Characters were extracted least-significant first; reverse to restore
    // left-to-right reading order.
    std::ranges::reverse(result);
    return result;
  }
};

LC_PROBLEM(168, "Excel Sheet Column Title", "math", ctx) {
  Solution sol;

  ctx.test("single_letter_boundaries", [&] {
    ctx.expect_eq(sol.convertToTitle(1), "A");
    ctx.expect_eq(sol.convertToTitle(23), "W");
    ctx.expect_eq(sol.convertToTitle(26), "Z");
  });

  ctx.test("two_letter_and_carry_corners", [&] {
    ctx.expect_eq(sol.convertToTitle(27), "AA");
    ctx.expect_eq(sol.convertToTitle(28), "AB");
    ctx.expect_eq(sol.convertToTitle(52), "AZ");
    ctx.expect_eq(sol.convertToTitle(701), "ZY");
    ctx.expect_eq(sol.convertToTitle(702), "ZZ");
  });

  ctx.test("three_letter_and_int_max", [&] {
    ctx.expect_eq(sol.convertToTitle(703), "AAA");
    ctx.expect_eq(sol.convertToTitle(750), "ABV");
    ctx.expect_eq(sol.convertToTitle(std::numeric_limits<int>::max()),
                  "FXSHRXW");
  });
}

}  // namespace
