// LeetCode 792: Number Of Matching Subsequences
// https://leetcode.com/problems/number-of-matching-subsequences/
//
// Problem:
//   Given a lowercase English string `s` and an array of lowercase strings
//   `words`, return the number of elements `words[i]` that are subsequences of
//   `s` (i.e., can be obtained from `s` by deleting zero or more characters
//   without reordering the remaining characters). Duplicate words in `words`
//   are counted once per occurrence.
//
// Algorithm:
//   Instead of scanning `s` from scratch for each query word (which costs
//   O(|words| * |s|)), process all words simultaneously in a single pass over
//   `s` using 26 next-character waiting buckets:
//     1. Represent each word's remaining unmatched suffix as a zero-copy
//        `std::string_view` and place it into the bucket corresponding to its
//        first needed character `rem[0] - 'a'`.
//     2. Scan each character `ch` of `s`. Swap out `buckets[ch - 'a']` into a
//        temporary vector (leaving the bucket empty before re-insertion so a
//        word needing consecutive copies of `ch` advances at most once per
//        character of `s`).
//     3. For each waiting suffix, call `rem.remove_prefix(1)`. If `rem` is now
//        empty, the entire word has been matched; otherwise move `rem` into
//        the bucket for its new leading character.
//   Time Complexity:  O(|s| + sum(|w| for w in words)), visiting each character
//                     of `s` and each character of every word at most once.
//   Space Complexity: O(|words|) for storing one lightweight string_view per
//                     active word across the 26 buckets.
//
// Data Structures:
//   - std::array<std::vector<std::string_view>, 26>: next-letter waiting queues
//     indexed by the next required character ('a'-'z'), storing non-owning
//     views of each word's unmatched suffix.
//
// Real-World Applications:
//   - Multi-pattern fuzzy/subsequence filtering in IDE command palettes and
//     file finders matching a single typed stream against thousands of symbols.
//   - Event-stream complex event processing (CEP) tracking many concurrent
//     ordered state patterns over a single log or telemetry stream.
//   - Bioinformatics motif-screening pipelines checking batches of short
//     subsequence probes against a long reference sequence in one pass.
//
// Related LeetCode Problems:
//   - #392 Is Subsequence [done] (single-word subsequence check)
//   - #524 Longest Word in Dictionary through Deleting [done] (longest matching subsequence)
//   - #1055 Shortest Way to Form String [done] (greedy subsequence partitioning)
//   - #1143 Longest Common Subsequence [done] (DP for longest shared subsequence)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the number of strings in `words` that are subsequences of `s`.
  // Time: O(|s| + sum(|w|)), Space: O(|words|).
  int numMatchingSubseq(std::string_view s,
                        const std::vector<std::string>& words) {
    // buckets[c] stores the remaining unmatched suffix of every word currently
    // waiting for character ('a' + c).
    std::array<std::vector<std::string_view>, kAlphabetSize> buckets{};
    int matched = 0;

    for (const std::string& word : words) {
      if (word.empty()) {
        // The empty string is trivially a subsequence of any string.
        ++matched;
        continue;
      }
      const auto bucket_idx = static_cast<std::size_t>(word[0] - 'a');
      buckets[bucket_idx].emplace_back(word);
    }

    // Advance every word waiting on the current character `ch` in one pass.
    std::vector<std::string_view> waiting;
    for (const char ch : s) {
      const auto bucket_idx = static_cast<std::size_t>(ch - 'a');
      if (buckets[bucket_idx].empty()) {
        continue;
      }

      // Swap out the bucket before pushing advanced suffixes back so a word
      // like "aa" cannot consume the same character of `s` twice.
      waiting.clear();
      waiting.swap(buckets[bucket_idx]);

      for (std::string_view suffix : waiting) {
        suffix.remove_prefix(1uz);
        if (suffix.empty()) {
          ++matched;
        } else {
          const auto next_idx = static_cast<std::size_t>(suffix[0] - 'a');
          buckets[next_idx].push_back(suffix);
        }
      }
    }

    return matched;
  }

 private:
  static constexpr std::size_t kAlphabetSize = 26;
};

LC_PROBLEM(792, "Number Of Matching Subsequences", "string", ctx) {
  Solution sol;

  ctx.test("example_1_basic", [&] {
    const std::vector<std::string> words = {"a", "bb", "acd", "ace"};
    ctx.expect_eq(sol.numMatchingSubseq("abcde", words), 3);
  });

  ctx.test("example_2_longer_haystack", [&] {
    const std::vector<std::string> words = {"ahjpjau", "ja", "ahbwzgqnuk",
                                            "tnmlanowax"};
    ctx.expect_eq(sol.numMatchingSubseq("dsahjpjauf", words), 2);
  });

  ctx.test("repeated_characters_no_double_consume", [&] {
    const std::vector<std::string> words = {"a", "aa", "aaa", "aaaa"};
    ctx.expect_eq(sol.numMatchingSubseq("aaa", words), 3);
  });

  ctx.test("duplicate_words_counted_separately", [&] {
    const std::vector<std::string> words = {"ace", "ace", "bd", "bd", "zz"};
    ctx.expect_eq(sol.numMatchingSubseq("abcde", words), 4);
  });

  ctx.test("no_matches", [&] {
    const std::vector<std::string> words = {"xyz", "cba", "abcdef"};
    ctx.expect_eq(sol.numMatchingSubseq("abcde", words), 0);
  });

  ctx.test("empty_words_list", [&] {
    const std::vector<std::string> words;
    ctx.expect_eq(sol.numMatchingSubseq("abcde", words), 0);
  });
}

}  // namespace
