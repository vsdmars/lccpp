// LeetCode 819: Most Common Word
// https://leetcode.com/problems/most-common-word/
//
// Problem:
//   Given a string `paragraph` and a list of lowercase `banned` words, return
//   the most frequent word in `paragraph` that is not in `banned`. Words are
//   case-insensitive, punctuation separates words (even without spaces), and
//   the returned word must be lowercase. It is guaranteed that at least one
//   non-banned word exists and the answer is unique.
//
// Algorithm:
//   Normalize every banned word to lowercase and store them in an unordered
//   set for O(1) expected membership checks. Then perform a single linear scan
//   over `paragraph`: accumulate consecutive ASCII letters in lowercase into a
//   reusable token buffer, and whenever a non-letter delimiter (or end of
//   string) is reached, flush the completed token. If the token is non-empty
//   and not banned, increment its frequency in a hash map and update the
//   running maximum on the fly.
//   Time Complexity:  O(N + B), where N = paragraph.size() and B is the total
//                     number of characters across all banned words.
//   Space Complexity: O(N + B) for the banned-word set and frequency table.
//
// Data Structures:
//   - std::unordered_set<std::string>: O(1) average-case lookup of normalized
//     banned words.
//   - std::unordered_map<std::string, int>: counts occurrences of each
//     non-banned lowercase word encountered in the paragraph.
//   - std::string token buffer: reused across words to avoid repeated heap
//     allocations while tokenizing.
//
// Real-World Applications:
//   - Search-engine inverted indexing and stop-word filtering during document
//     ingestion.
//   - Log analytics and alerting pipelines identifying top error tokens while
//     excluding noisy boilerplate terms.
//   - Content moderation and NLP preprocessing (term-frequency extraction).
//
// Related LeetCode Problems:
//   - #347 Top K Frequent Elements [done] (frequency counting with top-k selection)
//   - #692 Top K Frequent Words [done] (word frequency ranking with tie-breaking)
//   - #884 Uncommon Words from Two Sentences [new] (sentence tokenization and counting)
//   - #1160 Find Words That Can Be Formed by Characters [new] (character frequency filtering)

#include "framework/lc_framework.h"

#include <cctype>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the lowercase word with the highest frequency in `paragraph` that
  // does not appear in `banned`.
  //
  // Time Complexity:  O(N + B)
  // Space Complexity: O(N + B)
  std::string mostCommonWord(std::string_view paragraph,
                             const std::vector<std::string>& banned) {
    // Normalize banned words defensively in case callers supply mixed case.
    std::unordered_set<std::string> banned_set;
    banned_set.reserve(banned.size());
    for (const std::string& word : banned) {
      banned_set.insert(to_lower_copy(word));
    }

    std::unordered_map<std::string, int> counts;
    std::string token;
    std::string best_word;
    int best_count = 0;

    // Single pass: build alphabetic tokens in lowercase and flush at every
    // delimiter so adjacent punctuation like "ball," splits cleanly.
    for (const char ch : paragraph) {
      const auto uch = static_cast<unsigned char>(ch);
      if (std::isalpha(uch) != 0) {
        token.push_back(static_cast<char>(std::tolower(uch)));
      } else {
        record_token(token, banned_set, counts, best_word, best_count);
      }
    }

    // Flush the final token when the paragraph ends with a letter rather than
    // punctuation or whitespace.
    record_token(token, banned_set, counts, best_word, best_count);
    return best_word;
  }

 private:
  // Converts an ASCII string view to a lowercase std::string copy.
  static std::string to_lower_copy(std::string_view text) {
    std::string lower;
    lower.reserve(text.size());
    for (const char ch : text) {
      const auto uch = static_cast<unsigned char>(ch);
      lower.push_back(static_cast<char>(std::tolower(uch)));
    }
    return lower;
  }

  // Flushes `token` into `counts` if non-empty and not banned, updating the
  // running winner (`best_word`, `best_count`), then clears `token` for reuse.
  static void record_token(std::string& token,
                           const std::unordered_set<std::string>& banned_set,
                           std::unordered_map<std::string, int>& counts,
                           std::string& best_word, int& best_count) {
    if (token.empty()) {
      return;
    }
    if (!banned_set.contains(token)) {
      const int freq = ++counts[token];
      if (freq > best_count) {
        best_count = freq;
        best_word = token;
      }
    }
    token.clear();
  }
};

LC_PROBLEM(819, "Most Common Word", "string", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    ctx.expect_eq(
        sol.mostCommonWord(
            "Bob hit a ball, the hit BALL flew far after it was hit.", {"hit"}),
        std::string("ball"));
  });

  ctx.test("punctuation_without_spaces", [&] {
    Solution sol;
    ctx.expect_eq(sol.mostCommonWord("a, a, a, a, b,b,b,c, c", {"a"}),
                  std::string("b"));
  });

  ctx.test("single_word_no_banned", [&] {
    Solution sol;
    ctx.expect_eq(sol.mostCommonWord("a.", {}), std::string("a"));
  });

  ctx.test("trailing_word_without_punctuation", [&] {
    Solution sol;
    ctx.expect_eq(sol.mostCommonWord("Bob. hIt, baLl", {"bob", "hit"}),
                  std::string("ball"));
  });

  ctx.test("case_insensitive_banned_list", [&] {
    Solution sol;
    ctx.expect_eq(sol.mostCommonWord("Hello, hello! World.", {"HELLO"}),
                  std::string("world"));
  });
}

}  // namespace
