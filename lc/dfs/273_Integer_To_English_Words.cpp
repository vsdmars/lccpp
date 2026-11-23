// LeetCode 273: Integer To English Words
// https://leetcode.com/problems/integer-to-english-words/
//
// Problem:
//   Convert a non-negative 32-bit integer `num` (0 <= num <= 2^31 - 1) to its
//   English words representation, with words separated by single spaces and
//   Title Case capitalization (e.g., 12345 -> "Twelve Thousand Three Hundred
//   Forty Five", 0 -> "Zero").
//
// Examples:
//   Example 1:
//     Input:  num = 12345
//     Output: "Twelve Thousand Three Hundred Forty Five"
//     Explanation:
//       - appendWords(12345, tokens) scans kScales and matches divisor 1000
//         ("Thousand").
//       - It recurses on 12345 / 1000 = 12 (< 20) to push kBelowTwenty[12] =
//         "Twelve", then pushes "Thousand".
//       - Recursing on 12345 % 1000 = 345 matches divisor 100 ("Hundred"), pushes
//         kBelowTwenty[3] = "Three" from 345 / 100 = 3, and pushes "Hundred".
//       - Recursing on 345 % 100 = 45 (< 100) pushes kTens[4] = "Forty", then
//         recurses on 45 % 10 = 5 to push kBelowTwenty[5] = "Five".
//       - joinWithSpaces joins ["Twelve", "Thousand", "Three", "Hundred", "Forty",
//         "Five"] with single spaces.
//
//   Example 2:
//     Input:  num = 1000010
//     Output: "One Million Ten"
//     Explanation:
//       - appendWords(1000010, tokens) matches the 1000000 ("Million") scale in
//         kScales.
//       - Recursing on 1000010 / 1000000 = 1 pushes kBelowTwenty[1] = "One",
//         followed by the scale label "Million".
//       - Recursing on 1000010 % 1000000 = 10 hits n < 20 and pushes
//         kBelowTwenty[10] = "Ten" without emitting "Thousand" or "Hundred".
//       - joinWithSpaces combines ["One", "Million", "Ten"] into "One Million Ten".
//
//   Example 3:
//     Input:  num = 0
//     Output: "Zero"
//     Explanation:
//       - numberToWords(0) hits the early guard if (num == 0) before calling
//         appendWords.
//       - It returns "Zero" directly so appendWords's n == 0 no-op only suppresses
//         trailing zero remainders in positive numbers.
//
// Algorithm:
//   Top-down recursive decomposition by English place-value scales:
//   - Special case: num == 0 immediately returns "Zero".
//   - For positive integers, recursively emit word tokens into a buffer:
//     * n == 0: base no-op (prevents emitting spurious "Zero" for exact
//       multiples like 20, 100, or 1,000,000).
//     * n < 20: emit directly from the 1..19 lookup table.
//     * n < 100: emit the tens word (20..90) and recurse on `n % 10`.
//     * n >= 100: find the largest scale divisor in {10^9 ("Billion"),
//       10^6 ("Million"), 10^3 ("Thousand"), 10^2 ("Hundred")}, recurse on
//       `n / unit`, emit the scale label, and recurse on `n % unit`.
//   Finally, join the collected tokens with single spaces.
//   Time Complexity: O(log_10(num)), which is O(1) for 32-bit signed integers
//     (at most 10 digits and bounded word output).
//   Space Complexity: O(log_10(num)) for recursion depth (<= 4) and token list.
//
// Data Structures:
//   - std::array<std::string_view, ...>: compile-time tables for 1..19, tens
//     multiples (20..90), and scale (divisor, label) pairs.
//   - std::vector<std::string_view>: accumulates emitted words without
//     intermediate string concatenations before joining once at the end.
//
// Real-World Applications:
//   - Banking and financial systems printing legal written check amounts and
//     invoices where numeric values must be spelled out unambiguously.
//   - Text-to-speech (TTS) front-end normalization converting raw numeric
//     tokens in documents into spoken-language words.
//   - Accessibility screen readers and automated voice-response (IVR) systems
//     reading account balances, quantities, and verification numbers aloud.
//
// Related LeetCode Problems:
//   - #12 Integer to Roman [done] (scale-table decomposition from largest unit down)
//   - #13 Roman to Integer [done] (reverse numeral-to-integer parsing)
//   - #168 Excel Sheet Column Title [done] (positional base-26 string conversion)
//   - #405 Convert a Number to Hexadecimal [done] (positional radix formatting)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <limits>
#include <string>
#include <string_view>
#include <vector>

namespace {

// Direct English names for integers 0..19 (index 0 unused for positive values).
constexpr std::array<std::string_view, 20> kBelowTwenty = {
    "",        "One",     "Two",       "Three",    "Four",
    "Five",    "Six",     "Seven",     "Eight",    "Nine",
    "Ten",     "Eleven",  "Twelve",    "Thirteen", "Fourteen",
    "Fifteen", "Sixteen", "Seventeen", "Eighteen", "Nineteen"};

// Direct English names for tens multiples (index = value / 10).
constexpr std::array<std::string_view, 10> kTens = {
    "",      "",      "Twenty",  "Thirty", "Forty",
    "Fifty", "Sixty", "Seventy", "Eighty", "Ninety"};

struct Scale {
  int divisor = 1;
  std::string_view label;
};

// Place-value scales ordered strictly descending for greedy recursive split.
constexpr std::array<Scale, 4> kScales = {{
    {.divisor = 1'000'000'000, .label = "Billion"},
    {.divisor = 1'000'000, .label = "Million"},
    {.divisor = 1'000, .label = "Thousand"},
    {.divisor = 100, .label = "Hundred"},
}};

class Solution {
 public:
  // Converts a non-negative 32-bit integer `num` into English words.
  //
  // Time: O(log_10(num)) (O(1) for 32-bit ints); Space: O(log_10(num)).
  std::string numberToWords(int num) {
    if (num == 0) {
      return "Zero";
    }

    std::vector<std::string_view> tokens;
    appendWords(num, tokens);
    return joinWithSpaces(tokens);
  }

 private:
  // Recursively decomposes `n` into place-value components and appends each
  // spoken word token to `tokens`. Does nothing when `n == 0` so trailing
  // remainders like `20 % 10 == 0` emit no stray tokens.
  static void appendWords(int n, std::vector<std::string_view>& tokens) {
    if (n == 0) {
      return;
    }

    if (n < 20) {
      tokens.push_back(kBelowTwenty[static_cast<std::size_t>(n)]);
      return;
    }

    if (n < 100) {
      tokens.push_back(kTens[static_cast<std::size_t>(n / 10)]);
      appendWords(n % 10, tokens);
      return;
    }

    for (const Scale& scale : kScales) {
      if (n >= scale.divisor) {
        appendWords(n / scale.divisor, tokens);
        tokens.push_back(scale.label);
        appendWords(n % scale.divisor, tokens);
        return;
      }
    }
  }

  // Joins non-empty word tokens with a single space separator, reserving the
  // exact output length up front.
  static std::string joinWithSpaces(
      const std::vector<std::string_view>& tokens) {
    if (tokens.empty()) {
      return {};
    }

    std::size_t total_len = tokens.size() - 1uz;
    for (const std::string_view token : tokens) {
      total_len += token.size();
    }

    std::string output;
    output.reserve(total_len);
    for (std::size_t i = 0uz; i < tokens.size(); ++i) {
      if (i > 0uz) {
        output.push_back(' ');
      }
      output.append(tokens[i]);
    }
    return output;
  }
};

LC_PROBLEM(273, "Integer To English Words", "dfs", ctx) {
  ctx.test("zero", [&] {
    Solution sol;
    ctx.expect_eq(sol.numberToWords(0), "Zero");
  });

  ctx.test("teens_and_exact_tens", [&] {
    Solution sol;
    ctx.expect_eq(sol.numberToWords(14), "Fourteen");
    ctx.expect_eq(sol.numberToWords(20), "Twenty");
    ctx.expect_eq(sol.numberToWords(100), "One Hundred");
  });

  ctx.test("examples_from_problem", [&] {
    Solution sol;
    ctx.expect_eq(sol.numberToWords(123), "One Hundred Twenty Three");
    ctx.expect_eq(sol.numberToWords(12'345),
                  "Twelve Thousand Three Hundred Forty Five");
    ctx.expect_eq(
        sol.numberToWords(1'234'567),
        "One Million Two Hundred Thirty Four Thousand Five Hundred Sixty "
        "Seven");
  });

  ctx.test("internal_zeros", [&] {
    Solution sol;
    ctx.expect_eq(sol.numberToWords(12'302),
                  "Twelve Thousand Three Hundred Two");
    ctx.expect_eq(sol.numberToWords(1'000'010), "One Million Ten");
    ctx.expect_eq(sol.numberToWords(1'000'000), "One Million");
  });

  ctx.test("int32_max", [&] {
    Solution sol;
    ctx.expect_eq(
        sol.numberToWords(std::numeric_limits<int>::max()),
        "Two Billion One Hundred Forty Seven Million Four Hundred Eighty Three "
        "Thousand Six Hundred Forty Seven");
  });
}

}  // namespace
