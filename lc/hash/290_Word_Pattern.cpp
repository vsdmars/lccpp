// LeetCode 290: Word Pattern
// https://leetcode.com/problems/word-pattern/
//
// Problem:
//   Given a pattern string `pattern` of lowercase letters and a space-separated
//   string `s` of lowercase words, determine whether `s` follows the exact same
//   pattern. Following the pattern means there exists a bijection (one-to-one
//   and onto mapping) between each distinct character in `pattern` and each
//   distinct non-empty word in `s`.
//
// Algorithm:
//   Split `s` into non-empty `std::string_view` word tokens without copying
//   character data. First verify that the number of tokens matches
//   `pattern.size()`. Then scan `(pattern[i], words[i])` pairs together while
//   maintaining two complementary hash maps (`char -> word` and `word -> char`).
//   For each pair, insert into both maps via `try_emplace`; if either key
//   already exists with a different partner, the bijection is violated and we
//   immediately return `false`.
//   Time Complexity: O(N + M), where N = pattern.size() and M = s.size().
//   Space Complexity: O(N + U), where U is the number of distinct words/chars
//                     stored (zero string allocations thanks to string_view).
//
// Data Structures:
//   - `std::vector<std::string_view>`: zero-copy token list sliced from `s`.
//   - `std::unordered_map<char, std::string_view>`: enforces single-valued
//     forward mapping from pattern character to word.
//   - `std::unordered_map<std::string_view, char>`: enforces injectivity
//     (no two distinct pattern characters map to the same word).
//
// Real-World Applications:
//   - Structural log-template matching and anomaly detection (verifying that a
//     tokenized log line conforms to a known symbolic schema).
//   - Compiler AST / bytecode isomorphism checks and macro-pattern verification.
//   - Natural language template validation and regex backreference matching.
//
// Related LeetCode Problems:
//   - #205 Isomorphic Strings [done] (character-to-character bijection between two strings)
//   - #291 Word Pattern II [new] (backtracking bijection without space delimiters)
//   - #890 Find and Replace Pattern [new] (filtering words that match a character bijection)
//   - #49 Group Anagrams [done] (canonical signature hashing over strings)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Checks whether `s` follows `pattern` under a strict bijection between
  // characters of `pattern` and space-separated words of `s`.
  // Time: O(|pattern| + |s|), Space: O(|pattern|).
  bool wordPattern(std::string_view pattern, std::string_view s) {
    const std::vector<std::string_view> words = splitWords(s);
    if (pattern.size() != words.size()) {
      return false;
    }

    // Two maps enforce both directions of the bijection:
    //   char_to_word: each letter maps to at most one word
    //   word_to_char: each word maps to at most one letter
    std::unordered_map<char, std::string_view> char_to_word;
    std::unordered_map<std::string_view, char> word_to_char;

    for (std::size_t i = 0uz; i < pattern.size(); ++i) {
      const char ch = pattern[i];
      const std::string_view word = words[i];

      const auto [c_it, c_inserted] = char_to_word.try_emplace(ch, word);
      if (!c_inserted && c_it->second != word) {
        return false;
      }

      const auto [w_it, w_inserted] = word_to_char.try_emplace(word, ch);
      if (!w_inserted && w_it->second != ch) {
        return false;
      }
    }
    return true;
  }

 private:
  // Splits `text` on ASCII spaces into non-empty zero-copy string_view slices.
  [[nodiscard]] static std::vector<std::string_view> splitWords(
      std::string_view text) {
    std::vector<std::string_view> words;
    std::size_t pos = 0uz;
    while (pos < text.size()) {
      while (pos < text.size() && text[pos] == ' ') {
        ++pos;
      }
      const std::size_t start = pos;
      while (pos < text.size() && text[pos] != ' ') {
        ++pos;
      }
      if (pos > start) {
        words.push_back(text.substr(start, pos - start));
      }
    }
    return words;
  }
};

LC_PROBLEM(290, "Word Pattern", "hash", ctx) {
  ctx.test("example_1_valid_bijection", [&] {
    Solution sol;
    ctx.expect_true(sol.wordPattern("abba", "dog cat cat dog"));
  });

  ctx.test("example_2_forward_mismatch", [&] {
    Solution sol;
    ctx.expect_false(sol.wordPattern("abba", "dog cat cat fish"));
  });

  ctx.test("example_3_constant_pattern_mismatch", [&] {
    Solution sol;
    ctx.expect_false(sol.wordPattern("aaaa", "dog cat cat dog"));
  });

  ctx.test("example_4_non_injective_words", [&] {
    Solution sol;
    ctx.expect_false(sol.wordPattern("abba", "dog dog dog dog"));
  });

  ctx.test("length_mismatch_and_single_token", [&] {
    Solution sol;
    ctx.expect_false(sol.wordPattern("aba", "dog cat cat"));
    ctx.expect_false(sol.wordPattern("abc", "dog cat"));
    ctx.expect_false(sol.wordPattern("ab", "dog cat bird"));
    ctx.expect_true(sol.wordPattern("a", "dog"));
  });
}

}  // namespace
