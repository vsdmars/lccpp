// LeetCode 1048: Longest String Chain
// https://leetcode.com/problems/longest-string-chain/
//
// Problem:
//   Given an array `words` of lowercase English strings, `wordA` is a
//   predecessor of `wordB` iff inserting exactly one letter anywhere into
//   `wordA` (preserving the order of existing letters) yields `wordB` —
//   equivalently, deleting one letter from `wordB` yields `wordA`.
//   A word chain is a sequence `[word_1, word_2, ..., word_k]` (`k >= 1`) where
//   each word is a predecessor of the next. Return the maximum possible chain
//   length formed using words from `words`.
//   Constraints: `1 <= words.length <= 1000`, `1 <= words[i].length <= 16`.
//
// Algorithm:
//   Sort-by-length + Hash-Map Dynamic Programming (Longest Path in a DAG):
//   1. Sort `words` in ascending order of length so every possible predecessor
//      of `word` is processed before `word` itself.
//   2. Let `dp[w]` be the length of the longest valid chain ending at word `w`.
//   3. For each word `w`, try deleting each index `i in [0, |w| - 1]` to form
//      candidate predecessor `pred = w[0..i-1] + w[i+1..]`. If `pred` exists in
//      `dp`, transition `dp[w] = max(dp[w], dp[pred] + 1)`.
//   4. Track and return the global maximum `dp[w]` across all words.
//   Time Complexity:  O(N log N + N * L^2), where N = |words| and L <= 16 is
//                     the maximum word length (each word generates L candidates
//                     of length L - 1, each hashed in O(L)).
//   Space Complexity: O(N * L) for the DP hash map storing all distinct words.
//
// Data Structures:
//   - View array (`std::vector<std::string_view>`): non-owning views sorted by
//     length without copying or moving underlying `std::string` allocations.
//   - Hash map (`std::unordered_map<std::string_view, int>`): maps each word
//     view to the longest chain length ending at that word in O(L) average time.
//   - Reusable string buffer (`std::string`): avoids repeated heap allocations
//     when generating single-character-deletion predecessors.
//
// Real-World Applications:
//   - Spell-checker and fuzzy-dictionary edit-graph traversal (finding maximal
//     single-insertion morphological derivation chains).
//   - Schema/API version evolution analysis where each version adds a single
//     backwards-compatible field and longest linear upgrade paths are tracked.
//   - Bioinformatic k-mer / insertion-chain assembly and longest-path analysis
//     in layered directed acyclic graphs (DAGs).
//
// Related LeetCode Problems:
//   - #300 Longest Increasing Subsequence [done] (canonical DP for longest valid chain)
//   - #646 Maximum Length of Pair Chain [new] (sorting + longest chain DP/greedy)
//   - #368 Largest Divisible Subset [done] (sort + predecessor DP on divisibility DAG)
//   - #127 Word Ladder [done] (single-character edit graph shortest path)
//   - #720 Longest Word in Dictionary [done] (building words one character at a time)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the length of the longest word chain where each consecutive word
  // adds exactly one character to its predecessor.
  //
  // Time: O(N log N + N * L^2), Space: O(N * L).
  int longestStrChain(const std::vector<std::string>& words) {
    if (words.empty()) {
      return 0;
    }

    // Phase 1: Sort non-owning string views by length so shorter words are
    // processed before longer words (topological order of the predecessor DAG)
    // without moving or copying `std::string` buffers.
    std::vector<std::string_view> sorted_words;
    sorted_words.reserve(words.size());
    for (const auto& word : words) {
      sorted_words.emplace_back(word);
    }
    std::ranges::sort(sorted_words,
                      [](std::string_view lhs, std::string_view rhs) {
                        return lhs.size() < rhs.size();
                      });

    // Phase 2: Bottom-up DP over the predecessor DAG.
    // `dp[word]` stores the longest valid chain ending with `word`.
    std::unordered_map<std::string_view, int> dp;
    dp.reserve(sorted_words.size());

    int longest = 1;
    std::string pred;
    for (const std::string_view word : sorted_words) {
      int best = 1;
      pred.reserve(word.size());

      // Generate every 1-character deletion of `word` and check if it appeared
      // among shorter words in the input.
      for (std::size_t idx = 0uz; idx < word.size(); ++idx) {
        pred.clear();
        pred.append(word.substr(0uz, idx));
        pred.append(word.substr(idx + 1uz));

        if (const auto it = dp.find(pred); it != dp.end()) {
          best = std::max(best, it->second + 1);
        }
      }

      dp[word] = best;
      longest = std::max(longest, best);
    }

    return longest;
  }
};

LC_PROBLEM(1048, "Longest String Chain", "sort", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    ctx.expect_eq(
        sol.longestStrChain({"a", "b", "ba", "bca", "bda", "bdca"}), 4);
  });

  ctx.test("example_2", [&] {
    ctx.expect_eq(
        sol.longestStrChain({"xbc", "pcxbcf", "xb", "cxbc", "pcxbc"}), 5);
  });

  ctx.test("example_3_no_chain", [&] {
    ctx.expect_eq(sol.longestStrChain({"abcd", "dbqca"}), 1);
  });

  ctx.test("single_word", [&] {
    ctx.expect_eq(sol.longestStrChain({"solo"}), 1);
  });

  ctx.test("multiple_disconnected_chains", [&] {
    ctx.expect_eq(
        sol.longestStrChain({"a", "ab", "abc", "x", "xy", "xyz", "xyzw"}), 4);
  });

  ctx.test("duplicate_words", [&] {
    ctx.expect_eq(sol.longestStrChain({"a", "a", "ab", "ab", "abc"}), 3);
  });
}

}  // namespace
