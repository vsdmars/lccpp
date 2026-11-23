// LeetCode 151: Reverse Words In A String
// https://leetcode.com/problems/reverse-words-in-a-string/
//
// Problem:
//   Given an input string `s`, reverse the order of the words. A word is
//   defined as a maximal sequence of non-space characters. The input may have
//   leading spaces, trailing spaces, or multiple spaces between words. Return a
//   string of the words in reverse order concatenated by a single space, with
//   no leading or trailing spaces.
//
// Algorithm:
//   Scan `s` from right to left using a single cursor index `i`:
//   1. Skip any contiguous space characters before `i`.
//   2. Mark `end = i` as the exclusive right boundary of the next word, then
//      walk left until hitting a space or the start of `s` to find `start = i`.
//   3. Append a single space delimiter if `result` is non-empty, followed by
//      the zero-copy slice `s.substr(start, end - start)`.
//   Scanning in reverse avoids allocating an intermediate vector of word tokens
//   while collapsing all redundant whitespace in one linear pass.
//   Time Complexity:  O(N), where N is the length of `s`.
//   Space Complexity: O(N) for the returned string (O(1) auxiliary workspace).
//
// Data Structures:
//   - `std::string_view` over the input for zero-copy word slicing.
//   - `std::string` output buffer pre-reserved to `s.size()` bytes.
//
// Real-World Applications:
//   - Whitespace normalization and token reordering in search query
//     canonicalization and NLP preprocessing pipelines.
//   - Hierarchical identifier reversal (e.g., reverse-DNS notation like
//     `com.example.api` or path-segment reordering in routing tables).
//   - Log formatting and CLI text-processing filters.
//
// Related LeetCode Problems:
//   - #186 Reverse Words In A String II [done] (in-place word reversal without extra spaces)
//   - #557 Reverse Words in a String III [done] (reverse characters within each word)
//   - #344 Reverse String [done] (in-place character reversal primitive)
//   - #68 Text Justification [done] (word tokenization and space formatting)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>

namespace {

class Solution {
 public:
  // Reverses the order of whitespace-delimited words in `s` and normalizes
  // separators to a single space.
  // Time: O(N), Space: O(N) for output / O(1) auxiliary.
  std::string reverseWords(std::string_view s) {
    std::string result;
    result.reserve(s.size());

    std::size_t i = s.size();
    // Loop invariant: all words strictly to the right of index `i` have been
    // appended to `result` in reverse order, separated by single spaces.
    while (i > 0uz) {
      // Skip trailing or inter-word spaces.
      while (i > 0uz && s[i - 1uz] == ' ') {
        --i;
      }
      if (i == 0uz) {
        break;
      }

      // Locate the start of the current word ending at `end`.
      const std::size_t end = i;
      while (i > 0uz && s[i - 1uz] != ' ') {
        --i;
      }

      if (!result.empty()) {
        result.push_back(' ');
      }
      result.append(s.substr(i, end - i));
    }

    return result;
  }
};

LC_PROBLEM(151, "Reverse Words In A String", "string", ctx) {
  Solution sol;

  ctx.test("basic example", [&] {
    ctx.expect_eq(sol.reverseWords("the sky is blue"), "blue is sky the");
  });

  ctx.test("leading trailing and multiple spaces", [&] {
    ctx.expect_eq(sol.reverseWords("   a   b "), "b a");
    ctx.expect_eq(sol.reverseWords("the  sky is blue "), "blue is sky the");
    ctx.expect_eq(sol.reverseWords(" the sky"), "sky the");
    ctx.expect_eq(sol.reverseWords("  hello world  "), "world hello");
  });

  ctx.test("empty single word and spaces only", [&] {
    ctx.expect_eq(sol.reverseWords(""), "");
    ctx.expect_eq(sol.reverseWords(" "), "");
    ctx.expect_eq(sol.reverseWords("     "), "");
    ctx.expect_eq(sol.reverseWords("word"), "word");
  });
}

}  // namespace
