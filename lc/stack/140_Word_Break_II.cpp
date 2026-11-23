// LeetCode 140: Word Break II
// https://leetcode.com/problems/word-break-ii/
//
// Problem:
//   Given a string `s` and a dictionary of distinct strings `wordDict`, insert
//   spaces into `s` to construct every possible sentence where each word is a
//   valid dictionary word. Dictionary words may be reused any number of times.
//   Return all such sentences in any order.
//
// Algorithm:
//   Primary (Top-Down DFS with Memoization):
//     Store `wordDict` in a hash set of `std::string_view` and record
//     `max_word_len` to bound prefix probes. Define `dfs(start)` as the list
//     of all valid sentences formed from `s[start..]`, memoized per index
//     `start` so overlapping suffix subproblems (and dead-end suffixes) are
//     evaluated at most once. For each valid dictionary prefix `s[start..end)`,
//     prepend the word to every sentence returned by `dfs(end)`.
//
//   Secondary (Pruned Stack Backtracking — matching the review approach):
//     First run an O(n * L) bottom-up DP pass (`can_break[i]`) to identify
//     which suffixes are segmentable, pruning dead-end branches in O(1). Then
//     perform DFS using an explicit stack of chosen `std::string_view` words,
//     pushing on match, recursing, joining on reaching `n`, and popping on
//     backtrack.
//
//   Time complexity: O(n * L + output_size), where n = s.size() and L is the
//   maximum dictionary word length (worst-case exponential O(n * 2^n) when
//   exponentially many valid sentences exist).
//   Space complexity: O(n * 2^n) worst case for memoized sentences (or O(n)
//   auxiliary stack space excluding output for the backtracking variant).
//
// Data Structures:
//   - std::unordered_set<std::string_view> for O(1) average dictionary lookup
//     with zero substring allocations.
//   - std::vector<std::vector<std::string>> + std::vector<bool> memo table for
//     suffix results in top-down DFS.
//   - std::vector<std::string_view> explicit word stack + std::vector<bool>
//     reachability table for the backtracking variant.
//
// Real-World Applications:
//   - Natural language processing word segmentation for spaceless scripts
//     (Chinese, Japanese, Thai) and hashtag/URL-slug splitting.
//   - Speech recognition and OCR lattice decoding to enumerate candidate
//     tokenizations for downstream language-model scoring.
//   - Bioinformatics motif decomposition and lexical ambiguity resolution.
//
// Related LeetCode Problems:
//   - #139 Word Break [done] (boolean reachability decision version)
//   - #472 Concatenated Words [new] (words formed by concatenating others)
//   - #131 Palindrome Partitioning [done] (enumerate all valid string splits)
//   - #2707 Extra Characters in a String [new] (DP dictionary segmentation)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <ranges>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Enumerates all valid dictionary segmentations of `s` via memoized top-down
  // DFS. Time: O(n * L + output_size), Space: O(output_size + n).
  std::vector<std::string> wordBreak(std::string_view s,
                                     const std::vector<std::string>& wordDict) {
    if (s.empty() || wordDict.empty()) {
      return {};
    }

    const auto [dict, max_word_len] = build_dict(wordDict);
    const std::size_t n = s.size();

    // `memo[i]` stores all valid sentences covering suffix `s[i..]`.
    std::vector<std::vector<std::string>> memo(n + 1uz);
    std::vector<bool> computed(n + 1uz, false);
    // Base case: empty suffix yields one empty sentinel sentence.
    memo[n] = {""};
    computed[n] = true;

    auto dfs = [&](this auto&& self,
                   std::size_t start) -> const std::vector<std::string>& {
      if (computed[start]) {
        return memo[start];
      }

      std::vector<std::string> sentences;
      const std::size_t max_end = std::min(n, start + max_word_len);

      for (std::size_t end = start + 1uz; end <= max_end; ++end) {
        const std::string_view word = s.substr(start, end - start);
        if (!dict.contains(word)) {
          continue;
        }

        // Combine `word` with every valid segmentation of the remaining suffix.
        for (const std::string& tail : self(end)) {
          if (tail.empty()) {
            sentences.emplace_back(word);
          } else {
            std::string sentence;
            sentence.reserve(word.size() + 1uz + tail.size());
            sentence.append(word);
            sentence.push_back(' ');
            sentence.append(tail);
            sentences.push_back(std::move(sentence));
          }
        }
      }

      computed[start] = true;
      memo[start] = std::move(sentences);
      return memo[start];
    };

    return dfs(0uz);
  }

  // Alternative stack-based backtracking approach (mirroring `review()`),
  // using a reachability DP table to prune unsegmentable suffixes in O(1).
  std::vector<std::string> wordBreakBacktrack(
      std::string_view s, const std::vector<std::string>& wordDict) {
    if (s.empty() || wordDict.empty()) {
      return {};
    }

    const auto [dict, max_word_len] = build_dict(wordDict);
    const std::size_t n = s.size();
    const std::vector<bool> can_break =
        compute_reachability(s, dict, max_word_len);

    if (!can_break[0uz]) {
      return {};
    }

    std::vector<std::string> results;
    // Explicit stack of words chosen along the current root-to-leaf DFS path.
    std::vector<std::string_view> path;

    auto dfs = [&](this auto&& self, std::size_t start) -> void {
      if (start == n) {
        results.push_back(join_words(path));
        return;
      }

      const std::size_t max_end = std::min(n, start + max_word_len);
      for (std::size_t end = start + 1uz; end <= max_end; ++end) {
        if (!can_break[end]) {
          continue;
        }
        const std::string_view word = s.substr(start, end - start);
        if (dict.contains(word)) {
          path.push_back(word);
          self(end);
          path.pop_back();
        }
      }
    };

    dfs(0uz);
    return results;
  }

 private:
  struct DictIndex {
    std::unordered_set<std::string_view> words;
    std::size_t max_len = 0uz;
  };

  // Builds a hash set of non-owning views over `wordDict` along with the
  // longest word length to bound prefix probes.
  static DictIndex build_dict(const std::vector<std::string>& wordDict) {
    DictIndex index;
    index.words.reserve(wordDict.size());
    for (const std::string& word : wordDict) {
      index.words.insert(word);
      index.max_len = std::max(index.max_len, word.size());
    }
    return index;
  }

  // Computes bottom-up reachability: `can_break[i]` is true iff suffix
  // `s[i..]` can be segmented into dictionary words.
  static std::vector<bool> compute_reachability(
      std::string_view s, const std::unordered_set<std::string_view>& dict,
      std::size_t max_word_len) {
    const std::size_t n = s.size();
    std::vector<bool> can_break(n + 1uz, false);
    can_break[n] = true;

    for (std::size_t i = n; i > 0uz; --i) {
      const std::size_t start = i - 1uz;
      const std::size_t max_end = std::min(n, start + max_word_len);
      for (std::size_t end = start + 1uz; end <= max_end; ++end) {
        if (can_break[end] && dict.contains(s.substr(start, end - start))) {
          can_break[start] = true;
          break;
        }
      }
    }
    return can_break;
  }

  // Joins a stack of word slices with single spaces.
  static std::string join_words(const std::vector<std::string_view>& words) {
    std::size_t total_len = words.empty() ? 0uz : (words.size() - 1uz);
    for (const std::string_view w : words) {
      total_len += w.size();
    }
    std::string out;
    out.reserve(total_len);
    for (std::size_t i = 0uz; i < words.size(); ++i) {
      if (i > 0uz) {
        out.push_back(' ');
      }
      out.append(words[i]);
    }
    return out;
  }
};

LC_PROBLEM(140, "Word Break II", "stack", ctx) {
  Solution sol;

  auto expect_both = [&](std::string_view s,
                         const std::vector<std::string>& dict,
                         std::vector<std::string> expected) {
    std::vector<std::string> actual_memo = sol.wordBreak(s, dict);
    std::vector<std::string> actual_bt = sol.wordBreakBacktrack(s, dict);
    std::ranges::sort(expected);
    std::ranges::sort(actual_memo);
    std::ranges::sort(actual_bt);
    ctx.expect_eq(actual_memo, expected);
    ctx.expect_eq(actual_bt, expected);
  };

  ctx.test("leetcode_examples", [&] {
    expect_both("catsanddog", {"cat", "cats", "and", "sand", "dog"},
                {"cats and dog", "cat sand dog"});
    expect_both("pineapplepenapple",
                {"apple", "pen", "applepen", "pine", "pineapple"},
                {"pine apple pen apple", "pineapple pen apple",
                 "pine applepen apple"});
    expect_both("catsandog", {"cats", "dog", "sand", "and", "cat"}, {});
  });

  ctx.test("source_build_case", [&] {
    expect_both("catsandog", {"cats", "og", "sand", "and", "cat"},
                {"cat sand og", "cats and og"});
  });

  ctx.test("edge_and_pruning_cases", [&] {
    expect_both("", {"a"}, {});
    expect_both("abc", {}, {});
    expect_both("a", {"a"}, {"a"});
    // Adversarial unsegmentable suffix that would blow up naive unpruned DFS.
    expect_both("aaaaaaaaaaaaaaaaaaaaab", {"a", "aa", "aaa", "aaaa", "aaaaa"},
                {});
  });
}

}  // namespace
