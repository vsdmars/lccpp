// LeetCode 524: Longest Word In Dictionary Through Deleting
// https://leetcode.com/problems/longest-word-in-dictionary-through-deleting/
//
// Problem:
//   Given a string `s` and a list of dictionary strings `dictionary`, return
//   the longest string in `dictionary` that can be formed by deleting zero or
//   more characters from `s` (i.e., that is a subsequence of `s`). If multiple
//   valid strings share the maximum length, return the lexicographically
//   smallest one. If no dictionary string is a subsequence of `s`, return "".
//   Constraints: 1 <= s.length <= 1000, 1 <= dictionary.length <= 1000,
//   1 <= dictionary[i].length <= 1000, all lowercase English letters.
//
// Algorithm:
//   Use a best-first max-heap (priority queue) ordered by descending string
//   length with ties broken by ascending lexicographical order, paired with a
//   two-pointer subsequence check.
//   1. Filter out any dictionary word longer than `s` (impossible to form by
//      deletion) and push `std::string_view` references into a max-heap in O(D)
//      time via heapify.
//   2. Repeatedly pop the highest-priority candidate and test whether it is a
//      subsequence of `s` using two forward-only pointers: scan `s` left to
//      right, advancing the word pointer whenever characters match.
//   3. Because candidates are popped from best to worst, the very first match
//      is guaranteed to be the global optimum and allows immediate early exit.
//   Time Complexity:  O(D log D + D * (|s| + L)) worst case, where D is the
//                     dictionary size and L is average word length; much faster
//                     in practice when a long word matches early.
//   Space Complexity: O(D) for the heap of non-owning `std::string_view`s.
//
// Data Structures:
//   - `std::priority_queue<std::string_view, ...>` (binary max-heap): orders
//     candidate words by (longer length first, then lexicographically smaller)
//     without copying string contents.
//   - Two index pointers (`wi`, `si`): perform linear-time subsequence matching
//     between a candidate word and `s`.
//
// Real-World Applications:
//   - Autocomplete and fuzzy command/symbol search (e.g., IDE fuzzy file
//     finders like fzf/VS Code Quick Open matching typed subsequences against
//     candidate identifiers).
//   - Spell-checking and OCR post-processing where spurious characters are
//     dropped to recover the longest valid vocabulary term.
//   - Bioinformatics motif matching where conserved subsequence patterns are
//     tested against a longer sequence read.
//
// Related LeetCode Problems:
//   - #392 Is Subsequence [done] (core two-pointer subsequence check used per candidate)
//   - #720 Longest Word in Dictionary [done] (longest valid dictionary word with lexicographical tie-breaking)
//   - #792 Number of Matching Subsequences [done] (testing many dictionary words as subsequences of one string)
//   - #1055 Shortest Way to Form String [done] (greedy subsequence matching over a target string)

#include "framework/lc_framework.h"

#include <cstddef>
#include <queue>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

// Strict weak ordering for std::priority_queue so that longer words have higher
// priority, and equal-length words prefer lexicographically smaller strings.
struct CandidateOrder {
  [[nodiscard]] bool operator()(std::string_view lhs,
                                std::string_view rhs) const noexcept {
    if (lhs.size() != rhs.size()) {
      return lhs.size() < rhs.size();
    }
    return lhs > rhs;
  }
};

// Returns true if `word` is a subsequence of `source` (characters of `word`
// appear in `source` in the same relative order, not necessarily contiguously).
// Time: O(|source|), Space: O(1).
bool isSubsequence(std::string_view word, std::string_view source) noexcept {
  std::size_t wi = 0uz;
  // Advance through `source` strictly forward; each match greedily consumes
  // the earliest possible character in `source`, preserving maximum suffix for
  // remaining characters of `word`.
  for (std::size_t si = 0uz; wi < word.size() && si < source.size(); ++si) {
    if (word[wi] == source[si]) {
      ++wi;
    }
  }
  return wi == word.size();
}

class Solution {
 public:
  // Finds the longest (and lexicographically smallest on tie) word in
  // `dictionary` that can be formed by deleting characters from `s`.
  // Invariant: Candidates are extracted from `max_heap` in optimal-first order,
  // so the first subsequence match is guaranteed to be optimal.
  // Time: O(D log D + D * |s|) worst case, Space: O(D).
  std::string findLongestWord(std::string_view s,
                              const std::vector<std::string>& dictionary) {
    // Collect non-owning views of feasible candidates (words no longer than s).
    std::vector<std::string_view> candidates;
    candidates.reserve(dictionary.size());
    for (const std::string& word : dictionary) {
      if (word.size() <= s.size()) {
        candidates.emplace_back(word);
      }
    }

    // Build max-heap in O(D) time directly from the candidate vector.
    std::priority_queue<std::string_view, std::vector<std::string_view>,
                        CandidateOrder>
        max_heap(CandidateOrder{}, std::move(candidates));

    // Pop candidates from best to worst; return immediately on first match.
    while (!max_heap.empty()) {
      const std::string_view word = max_heap.top();
      max_heap.pop();
      if (isSubsequence(word, s)) {
        return std::string(word);
      }
    }

    return "";
  }
};

LC_PROBLEM(524, "Longest Word In Dictionary Through Deleting", "two_pointers",
           ctx) {
  Solution sol;

  ctx.test("example_1_apple", [&] {
    const std::vector<std::string> dict = {"ale", "apple", "monkey", "plea"};
    ctx.expect_eq(sol.findLongestWord("abpcplea", dict), "apple");
  });

  ctx.test("example_2_lexicographical_tie", [&] {
    const std::vector<std::string> dict = {"a", "b", "c"};
    ctx.expect_eq(sol.findLongestWord("abpcplea", dict), "a");
  });

  ctx.test("python_build_case", [&] {
    const std::vector<std::string> dict = {
        "apple", "ewaf", "awefawfwaf", "awef", "awefe", "ewafeffewafewf"};
    ctx.expect_eq(sol.findLongestWord("aewfafwafjlwajflwajflwafj", dict),
                  "ewaf");
  });

  ctx.test("no_matching_word", [&] {
    const std::vector<std::string> dict = {"xyz", "mnop", "abpcpleax"};
    ctx.expect_eq(sol.findLongestWord("abpcplea", dict), "");
  });

  ctx.test("exact_full_string_match", [&] {
    const std::vector<std::string> dict = {"ba", "ab", "abc", "abpcplea"};
    ctx.expect_eq(sol.findLongestWord("abpcplea", dict), "abpcplea");
  });

  ctx.test("equal_length_picks_smallest", [&] {
    const std::vector<std::string> dict = {"plea", "bpea", "able", "aple"};
    ctx.expect_eq(sol.findLongestWord("abpcplea", dict), "able");
  });
}

}  // namespace
