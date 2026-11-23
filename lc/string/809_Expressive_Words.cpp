// LeetCode 809: Expressive Words
// https://leetcode.com/problems/expressive-words/
//
// Problem:
//   A query word `w` is called "stretchy" with respect to a target string `s`
//   if `w` can be transformed into `s` by applying zero or more extension
//   operations. In one extension operation, choose any maximal group of
//   identical consecutive characters in `w` and insert additional copies of
//   that same character so that the resulting group has length 3 or more.
//   Given `s` and a list of query strings `words`, return how many words in
//   `words` are stretchy.
//
// Examples:
//   Example 1:
//     Input:  s = "heeellooo", words = ["hello", "hi", "helo"]
//     Output: 1
//     Explanation:
//       - Run-length encode s into target_runs = [('h', 1), ('e', 3), ('l', 2),
//         ('o', 3)].
//       - Check "hello" with runs [('h', 1), ('e', 1), ('l', 2), ('o', 1)]: 'h' and
//         'l' match exactly, while 'e' and 'o' can stretch to target count 3 >= 3,
//         incrementing stretchy_count to 1.
//       - Reject "hi" because it has 2 runs instead of 4.
//       - Reject "helo" on the 'l' run because query count 1 != target count 2
//         while target count 2 < 3.
//
//   Example 2:
//     Input:  s = "eeoooe", words = ["eeo", "eeoe", "eoe", "eeooooe", "eoooe",
//             "eeooe", "eeooee"]
//     Output: 2
//     Explanation:
//       - Encode s into target_runs = [('e', 2), ('o', 3), ('e', 1)].
//       - Accept "eeoe" ([('e', 2), ('o', 1), ('e', 1)]) and "eeooe" ([('e', 2),
//         ('o', 2), ('e', 1)]) because both 'e' runs match exactly and the 'o' run
//         stretches to 3 >= 3.
//       - Reject "eeo" (only 2 runs), "eoe" and "eoooe" (first 'e' has count 1 != 2
//         with target < 3), "eeooooe" ('o' count 4 > 3), and "eeooee" (last 'e'
//         count 2 > 1), returning 2.
//
//   Example 3:
//     Input:  s = "", words = ["", "a"]
//     Output: 1
//     Explanation:
//       - Encode s = "" into an empty target_runs vector of length 0.
//       - Match word "" whose encode_runs("") also has length 0, incrementing
//         stretchy_count to 1.
//       - Reject "a" because its 1 run does not match target_runs size 0, returning
//         1.
//
// Algorithm:
//   Compress `s` once into its run-length encoding (RLE) signature: a sequence
//   of `(character, count)` pairs representing each maximal contiguous group of
//   identical letters.
//   For each query word `w`, compute its RLE signature and verify whether it
//   can stretch to `s`'s signature:
//     1. Both signatures must have the exact same number of runs.
//     2. For every run index i:
//          - The run characters must match (`s_run.ch == w_run.ch`).
//          - The query run cannot be longer than the target run
//            (`w_run.count <= s_run.count`).
//          - If the runs differ in length (`w_run.count < s_run.count`), the
//            target run must have length at least 3 (`s_run.count >= 3`), since
//            an extension is only allowed to produce groups of size >= 3.
//   Time Complexity:  O(|s| + sum(|w| for w in words)) — each string is
//                     run-length encoded and compared in a single linear pass.
//   Space Complexity: O(|s| + max(|w|)) for the RLE vectors.
//
// Data Structures:
//   - std::vector<Run>: compact run-length encoding of `(char ch, int count)`
//     pairs capturing the exact group structure of `s` and each candidate word.
//
// Real-World Applications:
//   - Social-media text normalization and sentiment analysis pipelines mapping
//     elongated slang ("sooooo", "heeellooo") back to canonical dictionary words.
//   - Speech-to-text post-processing and phoneme duration alignment where
//     stretched syllables match canonical lexicon entries.
//   - Fuzzy query matching and spam/profanity filters resilient to intentional
//     character repetition.
//
// Related LeetCode Problems:
//   - #925 Long Pressed Name [new] (simpler run-length matching where any >= expansion is allowed)
//   - #443 String Compression [done] (in-place run-length encoding)
//   - #830 Positions of Large Groups [new] (identifying consecutive groups of size >= 3)
//   - #696 Count Binary Substrings [done] (analyzing consecutive character run lengths)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the number of strings in `words` that can be stretched into `s`.
  // Time: O(|s| + sum(|w|)), Space: O(|s| + max(|w|)).
  int expressiveWords(std::string_view s,
                      const std::vector<std::string>& words) {
    const std::vector<Run> target_runs = encode_runs(s);

    int stretchy_count = 0;
    for (const std::string& word : words) {
      if (is_stretchy(target_runs, encode_runs(word))) {
        ++stretchy_count;
      }
    }
    return stretchy_count;
  }

 private:
  static constexpr int kMinStretchLen = 3;

  // One maximal consecutive run of identical characters.
  struct Run {
    char ch = '\0';
    int count = 0;
  };

  // Run-length encodes `text` into consecutive (character, count) groups.
  static std::vector<Run> encode_runs(std::string_view text) {
    std::vector<Run> runs;
    if (text.empty()) {
      return runs;
    }

    char current_ch = text[0];
    int current_cnt = 1;
    for (std::size_t i = 1uz; i < text.size(); ++i) {
      if (text[i] == current_ch) {
        ++current_cnt;
      } else {
        runs.push_back({.ch = current_ch, .count = current_cnt});
        current_ch = text[i];
        current_cnt = 1;
      }
    }
    runs.push_back({.ch = current_ch, .count = current_cnt});
    return runs;
  }

  // Checks whether `word_runs` can be stretched into `target_runs` under the
  // rule that any extended group in the target must have length >= 3.
  static bool is_stretchy(const std::vector<Run>& target_runs,
                          const std::vector<Run>& word_runs) {
    if (target_runs.size() != word_runs.size()) {
      return false;
    }

    for (std::size_t i = 0uz; i < target_runs.size(); ++i) {
      const Run& target = target_runs[i];
      const Run& query = word_runs[i];

      if (target.ch != query.ch) {
        return false;
      }
      // Stretching can only add characters, never shrink a group.
      if (query.count > target.count) {
        return false;
      }
      // If target group has size < 3, no extension could have created it, so
      // the query group must already match its exact length.
      if (target.count < kMinStretchLen && query.count != target.count) {
        return false;
      }
    }

    return true;
  }
};

LC_PROBLEM(809, "Expressive Words", "string", ctx) {
  Solution sol;

  ctx.test("example_1_heeellooo", [&] {
    const std::vector<std::string> words = {"hello", "hi", "helo"};
    ctx.expect_eq(sol.expressiveWords("heeellooo", words), 1);
  });

  ctx.test("example_2_all_stretchy", [&] {
    const std::vector<std::string> words = {"zzyy", "zy", "zyy"};
    ctx.expect_eq(sol.expressiveWords("zzzzzyyyyy", words), 3);
  });

  ctx.test("source_build_case_eeoooe", [&] {
    const std::vector<std::string> words = {"eeo",     "eeoe",  "eoe",
                                            "eeooooe", "eoooe", "eeooe",
                                            "eeooee"};
    // Only "eeoe" and "eeooe" match ("ee" stays 2, "o"/"oo" stretch to "ooo").
    ctx.expect_eq(sol.expressiveWords("eeoooe", words), 2);
  });

  ctx.test("exact_match_without_extension", [&] {
    const std::vector<std::string> words = {"aabb", "ab", "aab"};
    ctx.expect_eq(sol.expressiveWords("aabb", words), 1);
  });

  ctx.test("empty_strings", [&] {
    const std::vector<std::string> words = {"", "a"};
    ctx.expect_eq(sol.expressiveWords("", words), 1);
  });

  ctx.test("reordered_characters_rejected", [&] {
    const std::vector<std::string> words = {"olleh", "hello"};
    ctx.expect_eq(sol.expressiveWords("heeellooo", words), 1);
  });
}

}  // namespace
