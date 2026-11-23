// LeetCode 17: Letter Combinations Of A Phone Number
// https://leetcode.com/problems/letter-combinations-of-a-phone-number/
//
// Problem:
//   Given a string `digits` containing digits from `'2'` to `'9'` inclusive,
//   return all possible letter combinations that the number could represent on
//   a classic telephone keypad (`2 -> "abc"`, ..., `7 -> "pqrs"`, `8 -> "tuv"`,
//   `9 -> "wxyz"`). Return an empty list if `digits` is empty.
//
//   Constraints:
//   - 0 <= digits.length <= 4
//   - digits[i] is a digit in the range `['2', '9']`
//
// Algorithm:
//   Iterative level-by-level breadth-first (Cartesian product) expansion:
//   1. If `digits` is empty, return `{}` immediately.
//   2. Initialize the frontier `combinations = {""}`.
//   3. For each digit in `digits`, look up its mapped letter string in a
//      compile-time `std::array` table. Allocate `next` with exact reserved
//      capacity `combinations.size() * letters.size()`, append every mapped
//      letter to every prefix in the current frontier, and move `next` into
//      `combinations`.
//   4. After processing all `n` digits, `combinations` holds every length-`n`
//      combination in lexicographical order.
//
//   Time Complexity:  O(n * 4^n), where n = digits.size() (up to 4 letters per
//                     digit and O(n) to construct each output string).
//   Space Complexity: O(n * 4^n) to hold the frontier and final output list.
//
// Data Structures:
//   - `std::array<std::string_view, 8>`: zero-overhead compile-time keypad
//     lookup table indexed by `digit - '2'`.
//   - Double-buffered `std::vector<std::string>` (`combinations` and `next`)
//     for breadth-first layer expansion.
//
// Real-World Applications:
//   - T9 predictive text and phone-keypad vanity-number dictionaries.
//   - Combinatorial test-case generation (Cartesian product of parameter
//     domains) in fuzzers and property-based testing frameworks.
//   - Query expansion and spelling-variant enumeration in search engines.
//
// Related LeetCode Problems:
//   - #22 Generate Parentheses [done] (combinatorial string generation)
//   - #39 Combination Sum [done] (backtracking over choice branches)
//   - #46 Permutations [done] (exhaustive sequence enumeration)
//   - #78 Subsets [done] (iterative/recursive power-set expansion)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Generates all telephone-keypad letter combinations for `digits` using
  // iterative breadth-first expansion.
  // Invariant: after step `k`, `combinations` contains every valid length-`k`
  // prefix corresponding to `digits[0..k-1]`.
  // Time: O(n * 4^n), Space: O(n * 4^n).
  std::vector<std::string> letterCombinations(std::string_view digits) {
    // Compile-time telephone keypad mapping for digits '2' through '9'.
    static constexpr std::array<std::string_view, 8> kDigitToLetters = {
        "abc",   // '2'
        "def",   // '3'
        "ghi",   // '4'
        "jkl",   // '5'
        "mno",   // '6'
        "pqrs",  // '7'
        "tuv",   // '8'
        "wxyz",  // '9'
    };

    if (digits.empty()) {
      return {};
    }

    std::vector<std::string> combinations = {""};
    for (const char digit : digits) {
      // Defensive guard against invalid digits outside ['2', '9'].
      if (digit < '2' || digit > '9') {
        return {};
      }

      const auto key =
          static_cast<std::size_t>(static_cast<unsigned char>(digit) -
                                   static_cast<unsigned char>('2'));
      const std::string_view letters = kDigitToLetters[key];

      // Expand every partial prefix in the current layer by each candidate
      // letter for the current digit.
      std::vector<std::string> next;
      next.reserve(combinations.size() * letters.size());
      for (const std::string& prefix : combinations) {
        for (const char letter : letters) {
          std::string candidate;
          candidate.reserve(prefix.size() + 1uz);
          candidate.append(prefix);
          candidate.push_back(letter);
          next.push_back(std::move(candidate));
        }
      }
      combinations = std::move(next);
    }

    return combinations;
  }
};

LC_PROBLEM(17, "Letter Combinations Of A Phone Number", "string", ctx) {
  Solution sol;

  ctx.test("example_two_digits", [&] {
    const std::vector<std::string> expected = {
        "ad", "ae", "af", "bd", "be", "bf", "cd", "ce", "cf",
    };
    ctx.expect_eq(sol.letterCombinations("23"), expected);
  });

  ctx.test("empty_and_single_digit", [&] {
    ctx.expect_eq(sol.letterCombinations(""), std::vector<std::string>{});
    ctx.expect_eq(sol.letterCombinations("2"),
                  std::vector<std::string>{"a", "b", "c"});
    ctx.expect_eq(sol.letterCombinations("7"),
                  std::vector<std::string>{"p", "q", "r", "s"});
    ctx.expect_eq(sol.letterCombinations("9"),
                  std::vector<std::string>{"w", "x", "y", "z"});
  });

  ctx.test("three_digits_from_build", [&] {
    const std::vector<std::string> actual = sol.letterCombinations("345");
    ctx.expect_eq(actual.size(), 27uz);
    ctx.expect_eq(actual.front(), "dgj");
    ctx.expect_eq(actual.back(), "fil");
  });

  ctx.test("four_letter_keypad_combination_counts", [&] {
    ctx.expect_eq(sol.letterCombinations("79").size(), 16uz);
    ctx.expect_eq(sol.letterCombinations("2345").size(), 81uz);
  });
}

}  // namespace
