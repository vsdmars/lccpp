// LeetCode 186: Reverse Words In A String II
// https://leetcode.com/problems/reverse-words-in-a-string-ii/
//
// Problem:
//   Given a character array `s` representing space-separated words with no
//   leading or trailing spaces and exactly one space between adjacent words,
//   reverse the order of the words in-place using O(1) extra space.
//
// Examples:
//   Example 1:
//     Input:  s = ['t', 'h', 'e', ' ', 's', 'k', 'y', ' ', 'i', 's', ' ', 'b', 'l',
//             'u', 'e']
//     Output: ['b', 'l', 'u', 'e', ' ', 'i', 's', ' ', 's', 'k', 'y', ' ', 't',
//             'h', 'e']
//     Explanation:
//       - Stage 1 calls std::ranges::reverse(s) to reverse all 15 characters
//         in-place into "eulb si yks eht".
//       - Stage 2 finds the space at index 4, reverses [0, 4) in-place from "eulb"
//         to "blue", and advances word_start to index 5.
//       - Subsequent iterations locate the remaining space delimiters and reverse
//         "si" to "is", "yks" to "sky", and "eht" to "the" in-place.
//       - When word_end hits s.end() after "the", the loop breaks with s holding
//         "blue is sky the".
//
//   Example 2:
//     Input:  s = ['h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd']
//     Output: ['w', 'o', 'r', 'l', 'd', ' ', 'h', 'e', 'l', 'l', 'o']
//     Explanation:
//       - Stage 1 reverses the entire 11-character buffer in-place to "dlrow
//         olleh".
//       - Stage 2 finds the space at index 5, reverses [0, 5) to restore "world",
//         and steps word_start to index 6.
//       - Searching from index 6 finds word_end == s.end(), reverses [6, 11) from
//         "olleh" to "hello", and exits the loop.
//
//   Example 3:
//     Input:  s = ['a', 'n', 't', 'i', 'g', 'r', 'a', 'v', 'i', 't', 'y']
//     Output: ['a', 'n', 't', 'i', 'g', 'r', 'a', 'v', 'i', 't', 'y']
//     Explanation:
//       - Stage 1 reverses all 11 characters in-place into "ytivargitna".
//       - Stage 2 searches from s.begin() for ' ', reaches word_end == s.end(), and
//         reverses the entire range [0, 11) back to "antigravity".
//       - Because word_end == s.end(), the loop terminates with the single word
//         unchanged.
//
// Algorithm:
//   Classic two-stage in-place reversal (also known as the reversal algorithm
//   for block/array rotation):
//   1. Reverse the entire character array `s`. Now the words appear in the
//      desired final order, but each individual word's characters are reversed.
//   2. Scan left-to-right to locate each maximal non-space word subrange
//      `[word_start, word_end)` and reverse that subrange in-place to restore
//      each word's original character order.
//   Time Complexity:  O(N), where N = `s.size()` (each character is swapped at
//                     most twice).
//   Space Complexity: O(1) auxiliary space (strictly in-place).
//
// Data Structures:
//   - In-place `std::vector<char>` buffer mutated via bidirectional iterators.
//
// Real-World Applications:
//   - Zero-allocation in-place buffer manipulation in embedded systems, kernel
//     networking stacks, and real-time audio/video pipelines.
//   - Reversing multi-segment records or hierarchical names in fixed memory
//     without heap churn.
//   - Building block for in-place cyclic array rotation (Bentley's three-
//     reversal algorithm).
//
// Related LeetCode Problems:
//   - #151 Reverse Words In A String [done] (handles arbitrary extra whitespace)
//   - #189 Rotate Array [done] (same reversal trick applied to array rotation)
//   - #344 Reverse String [done] (single-range in-place reversal primitive)
//   - #557 Reverse Words in a String III [done] (reverse each word without reversing order)

#include "framework/lc_framework.h"

#include <algorithm>
#include <iterator>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Reverses the order of words in `s` strictly in-place.
  // Time: O(N), Space: O(1).
  void reverseWords(std::vector<char>& s) {
    // Stage 1: reverse the entire buffer so words move to their final order,
    // albeit with each word spelled backwards.
    std::ranges::reverse(s);

    // Stage 2: reverse each individual word back to its forward spelling.
    auto word_start = s.begin();
    while (word_start != s.end()) {
      auto word_end = std::ranges::find(word_start, s.end(), ' ');
      std::ranges::reverse(word_start, word_end);

      if (word_end == s.end()) {
        break;
      }
      // Step past the single space delimiter to the start of the next word.
      word_start = std::next(word_end);
    }
  }
};

// Convenience test helper converting a string literal into a `std::vector<char>`.
std::vector<char> toChars(std::string_view sv) {
  return {sv.begin(), sv.end()};
}

LC_PROBLEM(186, "Reverse Words In A String II", "string", ctx) {
  Solution sol;

  ctx.test("source build example", [&] {
    std::vector<char> s = toChars("the sky is blue");
    sol.reverseWords(s);
    ctx.expect_eq(s, toChars("blue is sky the"));
  });

  ctx.test("two words and varying lengths", [&] {
    std::vector<char> s = toChars("hello world");
    sol.reverseWords(s);
    ctx.expect_eq(s, toChars("world hello"));

    std::vector<char> uneven = toChars("a bb ccc dddd");
    sol.reverseWords(uneven);
    ctx.expect_eq(uneven, toChars("dddd ccc bb a"));
  });

  ctx.test("single word single char and empty", [&] {
    std::vector<char> one_char = toChars("a");
    sol.reverseWords(one_char);
    ctx.expect_eq(one_char, toChars("a"));

    std::vector<char> one_word = toChars("antigravity");
    sol.reverseWords(one_word);
    ctx.expect_eq(one_word, toChars("antigravity"));

    std::vector<char> empty;
    sol.reverseWords(empty);
    ctx.expect_eq(empty, toChars(""));
  });
}

}  // namespace
