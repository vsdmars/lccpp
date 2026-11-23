// LeetCode 557: Reverse Words In A String III
// https://leetcode.com/problems/reverse-words-in-a-string-iii/
//
// Problem:
//   Given a string `s` representing a sentence where words are separated by
//   spaces, reverse the character order of each individual word while
//   preserving all whitespace positions and the original order of words.
//   Constraints: 1 <= s.size() <= 5 * 10^4, printable ASCII characters, words
//   separated by single spaces with no leading or trailing spaces.
//
// Algorithm:
//   Two-pointer in-place word scanning:
//   1. Maintain a scanning index `start` moving left-to-right across `s`.
//   2. Skip any space characters to locate the start of the next word.
//   3. Advance `end` until the next space or the end of `s`, delimiting the
//      half-open word range `[start, end)`.
//   4. Reverse the characters within `[start, end)` in-place via
//      `std::ranges::reverse`, then set `start = end`.
//   Because every character is visited a constant number of times and mutated
//   in-place inside the returned string buffer, no intermediate word array is
//   needed (unlike splitting and rejoining).
//   Time Complexity: O(N), where N = s.size().
//   Space Complexity: O(1) auxiliary space (excluding the returned string).
//
// Data Structures:
//   - `std::string`: mutable character buffer operated on directly in-place
//     using two index pointers (`start`, `end`).
//
// Real-World Applications:
//   - In-place text/token transformations in streaming editors and lexical
//     normalizers where minimizing heap allocations matters.
//   - Bidirectional text (BiDi) layout passes that reverse individual runs
//     within a line while keeping overall segment ordering intact.
//   - Low-memory embedded or systems string utilities performing segmented
//     byte/character reversals.
//
// Related LeetCode Problems:
//   - #151 Reverse Words in a String [done] (reversing word order while trimming extra spaces)
//   - #186 Reverse Words in a String II [done] (in-place double-reversal of full string then each word)
//   - #344 Reverse String [done] (in-place two-pointer reversal of a character array)
//   - #541 Reverse String II [new] (reversing fixed-stride k-character chunks in-place)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string>

namespace {

class Solution {
 public:
  // Reverses the characters of each whitespace-delimited word in `s` while
  // preserving word order and whitespace positions.
  // Time: O(N), Space: O(1) auxiliary space.
  std::string reverseWords(std::string s) {
    const std::size_t n = s.size();
    std::size_t start = 0uz;

    while (start < n) {
      // Advance past any separating spaces to locate the start of a word.
      while (start < n && s[start] == ' ') {
        ++start;
      }

      // Scan forward to find the half-open end of the current word.
      std::size_t end = start;
      while (end < n && s[end] != ' ') {
        ++end;
      }

      // Reverse the word slice [start, end) directly in-place.
      std::ranges::reverse(s.begin() + static_cast<std::ptrdiff_t>(start),
                           s.begin() + static_cast<std::ptrdiff_t>(end));

      // Continue scanning immediately after the reversed word.
      start = end;
    }

    return s;
  }
};

LC_PROBLEM(557, "Reverse Words In A String III", "string", ctx) {
  ctx.test("example_1_sentence", [&] {
    Solution sol;
    ctx.expect_eq(sol.reverseWords("Let's take LeetCode contest"),
                  "s'teL ekat edoCteeL tsetnoc");
  });

  ctx.test("example_2_two_words", [&] {
    Solution sol;
    ctx.expect_eq(sol.reverseWords("Mr Ding"), "rM gniD");
  });

  ctx.test("single_word_and_single_char", [&] {
    Solution sol;
    ctx.expect_eq(sol.reverseWords("a"), "a");
    ctx.expect_eq(sol.reverseWords("abcdef"), "fedcba");
  });

  ctx.test("empty_and_multiple_spaces", [&] {
    Solution sol;
    ctx.expect_eq(sol.reverseWords(""), "");
    ctx.expect_eq(sol.reverseWords("  ab   cd "), "  ba   dc ");
  });
}

}  // namespace
