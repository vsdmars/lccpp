// LeetCode 824: Goat Latin
// https://leetcode.com/problems/goat-latin/
//
// Problem:
//   Given a sentence `S` consisting of words separated by single spaces (with
//   uppercase and lowercase ASCII letters only), convert `S` to "Goat Latin":
//     1. If a word begins with a vowel ('a', 'e', 'i', 'o', 'u', case-
//        insensitive), append "ma" to the end of the word.
//     2. If a word begins with a consonant, move its first character to the
//        end of the word and then append "ma".
//     3. Append `i` copies of the letter 'a' to the i-th word (1-indexed).
//   Return the resulting space-separated sentence.
//
// Algorithm:
//   Scan `sentence` with two indices to extract each word as a zero-copy
//   `std::string_view`, maintaining a 1-based `word_index`. For each word,
//   append a separating space (after the first word), transform the word into
//   the output buffer according to the vowel/consonant rule, append "ma", and
//   append `word_index` copies of `'a'`.
//   Time Complexity:  O(N + W^2), where N = sentence.size() and W is the
//                     number of words (output length itself grows by
//                     1 + 2 + ... + W = O(W^2) trailing 'a' characters).
//   Space Complexity: O(N + W^2) for the output string; O(1) auxiliary space.
//
// Data Structures:
//   - std::string_view: non-owning slices for zero-allocation word scanning.
//   - std::string: output buffer built directly in one pass without
//     intermediate token vectors.
//
// Real-World Applications:
//   - Deterministic string tokenization and streaming rewrite passes in text
//     preprocessors, template engines, and protocol formatters.
//   - Phonetic and rule-based morphological text transformations in NLP
//     pipelines.
//
// Related LeetCode Problems:
//   - #151 Reverse Words in a String [done] (in-place/streaming word tokenization)
//   - #557 Reverse Words in a String III [done] (per-word string transformation)
//   - #1816 Truncate Sentence [new] (space-delimited word scanning)
//   - #2129 Capitalize the Title [new] (conditional per-word formatting rules)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>

namespace {

constexpr std::string_view kVowels = "aeiouAEIOU";

class Solution {
 public:
  // Converts a space-separated English sentence into Goat Latin.
  //
  // Time Complexity:  O(N + W^2)
  // Space Complexity: O(N + W^2) for the returned string, O(1) auxiliary
  std::string toGoatLatin(std::string_view sentence) {
    std::string result;
    // Every word gains at least "maa", so reserving twice the input size avoids
    // early reallocations on short sentences.
    result.reserve(sentence.size() * 2uz);

    std::size_t pos = 0uz;
    std::size_t word_index = 0uz;
    const std::size_t n = sentence.size();

    while (pos < n) {
      // Skip any separating spaces to locate the start of the next word.
      while (pos < n && sentence[pos] == ' ') {
        ++pos;
      }
      if (pos >= n) {
        break;
      }

      std::size_t end = pos;
      while (end < n && sentence[end] != ' ') {
        ++end;
      }

      const std::string_view word = sentence.substr(pos, end - pos);
      ++word_index;

      if (!result.empty()) {
        result.push_back(' ');
      }
      append_goat_word(word, word_index, result);

      pos = end;
    }

    return result;
  }

 private:
  // Returns true if `ch` is an uppercase or lowercase English vowel.
  static bool is_vowel(char ch) { return kVowels.contains(ch); }

  // Appends the Goat Latin transformation of `word` (at 1-based `word_index`)
  // directly onto `out`.
  static void append_goat_word(std::string_view word, std::size_t word_index,
                               std::string& out) {
    if (is_vowel(word.front())) {
      // Rule 1: vowel-initial words stay in place before appending "ma".
      out.append(word);
    } else {
      // Rule 2: consonant-initial words rotate the first character to the end.
      out.append(word.substr(1uz));
      out.push_back(word.front());
    }
    out.append("ma");
    // Rule 3: append `word_index` trailing 'a' characters.
    out.append(word_index, 'a');
  }
};

LC_PROBLEM(824, "Goat Latin", "string", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    ctx.expect_eq(sol.toGoatLatin("I speak Goat Latin"),
                  std::string("Imaa peaksmaaa oatGmaaaa atinLmaaaaa"));
  });

  ctx.test("example_2", [&] {
    Solution sol;
    ctx.expect_eq(
        sol.toGoatLatin("The quick brown fox jumped over the lazy dog"),
        std::string(
            "heTmaa uickqmaaa rownbmaaaa oxfmaaaaa umpedjmaaaaaa "
            "overmaaaaaaa hetmaaaaaaaa azylmaaaaaaaaa ogdmaaaaaaaaaa"));
  });

  ctx.test("build_case_uppercase_vowel", [&] {
    Solution sol;
    ctx.expect_eq(sol.toGoatLatin("A speak Goat Latin"),
                  std::string("Amaa peaksmaaa oatGmaaaa atinLmaaaaa"));
  });

  ctx.test("single_character_words", [&] {
    Solution sol;
    ctx.expect_eq(sol.toGoatLatin("a"), std::string("amaa"));
    ctx.expect_eq(sol.toGoatLatin("b"), std::string("bmaa"));
  });

  ctx.test("all_uppercase_vowels", [&] {
    Solution sol;
    ctx.expect_eq(
        sol.toGoatLatin("Each Apple In Over Under"),
        std::string("Eachmaa Applemaaa Inmaaaa Overmaaaaa Undermaaaaaa"));
  });
}

}  // namespace
