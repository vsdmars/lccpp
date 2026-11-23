// LeetCode 1859: Sorting The Sentence
// https://leetcode.com/problems/sorting-the-sentence/
//
// Problem:
//   A sentence is a sequence of English words separated by single spaces with
//   no leading or trailing spaces. A shuffled sentence `s` is produced by
//   appending each word's 1-indexed original position (a digit '1'-'9') to the
//   end of the word and permuting the words. Given `s` (up to 9 words in the
//   standard constraints), reconstruct and return the original sentence with
//   the trailing position numbers removed.
//
// Examples:
//   Example 1:
//     Input:  s = "is2 sentence4 This1 a3"
//     Output: "This is a sentence"
//     Explanation:
//       - Phase 1 scans space-delimited tokens and calls parseToken on each,
//         populating words = [(2, "is"), (4, "sentence"), (1, "This"), (3, "a")]
//         with total_chars = 2 + 8 + 4 + 1 = 15.
//       - Phase 2 sorts words ascending by the first tuple element (position),
//         reordering it to [(1, "This"), (2, "is"), (3, "a"), (4, "sentence")].
//       - Phase 3 reserves 15 + (4 - 1) = 18 characters in result and appends the
//         sorted words separated by single spaces to return "This is a sentence".
//
//   Example 2:
//     Input:  s = "ten10 one1 two2 three3 four4 five5 six6 seven7 eight8 nine9"
//     Output: "one two three four five six seven eight nine ten"
//     Explanation:
//       - Phase 1 calls parseToken on each token; on "ten10", walking backward over
//         trailing digits sets split = 3, extracts word = "ten", and parses "10" in
//         base 10 as position = 10, while the other nine tokens parse to positions
//         1 through 9.
//       - Phase 2 sorts the 10-element words vector by numeric position so (1,
//         "one") through (9, "nine") precede (10, "ten").
//       - Phase 3 joins the ordered words with single spaces into a pre-reserved
//         buffer and returns "one two three four five six seven eight nine ten".
//
//   Example 3:
//     Input:  s = ""
//     Output: ""
//     Explanation:
//       - Phase 1 begins with i = 0 and n = s.size() = 0, so the tokenization loop
//         never runs and words stays empty.
//       - The words.empty() check triggers and immediately returns "".
//
// Algorithm:
//   Scan `s` linearly to extract each space-delimited token as a non-owning
//   `std::string_view`. For each token, walk backward over its trailing decimal
//   digits to separate the alphabetic word prefix from its 1-indexed target
//   position, storing `(position, word)` pairs and accumulating total word
//   length. Sort the pairs ascending by `position` using `std::ranges::sort`,
//   then join the ordered words with single spaces into a pre-reserved output
//   `std::string`. Supporting multi-digit suffixes naturally generalizes the
//   problem beyond 9 words while faithfully preserving the Python approach.
//   Time Complexity:  O(N + W log W), where N = s.size() and W is the number of
//                     words (effectively O(N) since W <= 9 under LeetCode
//                     constraints).
//   Space Complexity: O(N) for the reconstructed output string and O(W)
//                     auxiliary space for the `(position, string_view)` vector.
//
// Data Structures:
//   - std::string_view: zero-copy non-owning slices referencing each word inside
//     the input string without per-word heap allocations.
//   - std::vector<std::pair<std::size_t, std::string_view>>: stores each word
//     paired with its parsed 1-indexed target position for sorting.
//   - std::string: pre-reserved output buffer sized to the exact final length
//     to avoid intermediate reallocations while joining words.
//
// Real-World Applications:
//   - Reassembling out-of-order network packets, log fragments, or chunked
//     streaming payloads using embedded sequence numbers.
//   - Reconstructing ordered records or message parts from tagged key-value
//     tokens in ETL pipelines and wire-protocol parsers.
//
// Related LeetCode Problems:
//   - #151 Reverse Words in a String [done] (tokenizing and reordering space-separated words)
//   - #186 Reverse Words in a String II [done] (in-place word-level reordering of a sentence)
//   - #557 Reverse Words in a String III [done] (word-by-word transformation preserving word boundaries)
//   - #1528 Shuffle String [new] (reconstructing a string from explicit target indices)
//   - #1451 Rearrange Words in a Sentence [new] (sorting sentence words by a key and rejoining with spaces)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

constexpr std::size_t kDecimalBase = 10uz;

// Pairs a word's parsed 1-indexed target position with a non-owning view of
// its alphabetic text.
using IndexedWord = std::pair<std::size_t, std::string_view>;

// Returns true if `ch` is an ASCII decimal digit ('0'-'9').
constexpr bool isAsciiDigit(char ch) noexcept {
  return ch >= '0' && ch <= '9';
}

class Solution {
 public:
  // Reconstructs the original sentence from a space-separated shuffled string
  // where each word ends with its 1-indexed position.
  //
  // Invariant: Every token in `words` holds a valid non-owning slice of `s`
  // paired with its parsed numeric position, and `total_chars` tracks the exact
  // sum of word lengths (excluding spaces and digits).
  //
  // Time Complexity:  O(N + W log W) (O(N) for W <= 9)
  // Space Complexity: O(N) for output, O(W) auxiliary
  std::string sortSentence(std::string_view s) {
    std::vector<IndexedWord> words;
    std::size_t total_chars = 0uz;

    // Phase 1: Tokenize `s` on spaces and parse each `(position, word)` pair.
    std::size_t i = 0uz;
    const std::size_t n = s.size();
    while (i < n) {
      // Skip any separating spaces so extra whitespace is handled gracefully.
      while (i < n && s[i] == ' ') {
        ++i;
      }
      if (i == n) {
        break;
      }

      // Advance to the end of the current space-delimited token.
      const std::size_t token_start = i;
      while (i < n && s[i] != ' ') {
        ++i;
      }

      const IndexedWord parsed =
          parseToken(s.substr(token_start, i - token_start));
      total_chars += parsed.second.size();
      words.push_back(parsed);
    }

    // Edge case: empty or whitespace-only input produces an empty sentence.
    if (words.empty()) {
      return {};
    }

    // Phase 2: Order all words by their 1-indexed target position.
    std::ranges::sort(words, {}, &IndexedWord::first);

    // Phase 3: Join sorted words with single spaces into an exact-sized buffer.
    std::string result;
    result.reserve(total_chars + (words.size() - 1uz));
    for (const auto& entry : words) {
      if (!result.empty()) {
        result.push_back(' ');
      }
      result.append(entry.second);
    }

    return result;
  }

 private:
  // Splits a single token such as "sentence4" into `{4, "sentence"}` by
  // scanning backward over all trailing ASCII digits.
  //
  // Time Complexity:  O(L), where L = token.size()
  // Space Complexity: O(1)
  static IndexedWord parseToken(std::string_view token) {
    // Walk backward to locate the boundary between word letters and digits.
    std::size_t split = token.size();
    while (split > 0uz && isAsciiDigit(token[split - 1uz])) {
      --split;
    }

    const std::string_view word = token.substr(0uz, split);

    // Parse the trailing digits into a base-10 position index.
    std::size_t position = 0uz;
    for (const char ch : token.substr(split)) {
      position =
          (position * kDecimalBase) + static_cast<std::size_t>(ch - '0');
    }

    return {position, word};
  }
};

LC_PROBLEM(1859, "Sorting The Sentence", "string", ctx) {
  ctx.test("example_1_and_build_case", [&] {
    Solution sol;
    ctx.expect_eq(sol.sortSentence("is2 sentence4 This1 a3"),
                  std::string("This is a sentence"));
  });

  ctx.test("example_2", [&] {
    Solution sol;
    ctx.expect_eq(sol.sortSentence("Myself2 Me1 I4 and3"),
                  std::string("Me Myself and I"));
  });

  ctx.test("single_word", [&] {
    Solution sol;
    ctx.expect_eq(sol.sortSentence("Hello1"), std::string("Hello"));
    ctx.expect_eq(sol.sortSentence("a1"), std::string("a"));
  });

  ctx.test("already_sorted", [&] {
    Solution sol;
    ctx.expect_eq(sol.sortSentence("One1 Two2 Three3"),
                  std::string("One Two Three"));
  });

  ctx.test("max_nine_words_reversed", [&] {
    Solution sol;
    ctx.expect_eq(
        sol.sortSentence("nine9 eight8 seven7 six6 five5 four4 three3 two2 "
                         "one1"),
        std::string("one two three four five six seven eight nine"));
  });

  ctx.test("mixed_case_single_letters", [&] {
    Solution sol;
    ctx.expect_eq(sol.sortSentence("Z3 a1 B2"), std::string("a B Z"));
  });

  ctx.test("multi_digit_positions", [&] {
    Solution sol;
    ctx.expect_eq(
        sol.sortSentence("ten10 one1 two2 three3 four4 five5 six6 seven7 "
                         "eight8 nine9"),
        std::string("one two three four five six seven eight nine ten"));
  });

  ctx.test("empty_input", [&] {
    Solution sol;
    ctx.expect_eq(sol.sortSentence(""), std::string(""));
  });
}

}  // namespace
