// LeetCode 916: Word Subsets
// https://leetcode.com/problems/word-subsets/
//
// Problem:
//   You are given two string arrays `words1` and `words2`, each containing
//   lowercase English words. A string `b` is a subset of string `a` if every
//   letter in `b` occurs in `a` with at least the same multiplicity (e.g.,
//   `"wrr"` is a subset of `"warrior"`, but not `"world"`). A string `a` in
//   `words1` is *universal* if every string `b` in `words2` is a subset of `a`.
//   Return an array of all universal strings in `words1` (in any order).
//   Constraints: 1 <= words1.length, words2.length <= 10^4,
//   1 <= words1[i].length, words2[i].length <= 10.
//
// Algorithm:
//   Constraint Aggregation (Multiset Union) over Fixed Alphabet:
//   Checking every pair `(a, b)` directly takes O(|words1| * |words2|) time.
//   Instead, observe that `a` satisfies all `b` in `words2` simultaneously if
//   and only if, for each of the 26 letters `c`, `count(a, c)` is at least
//   `max_{b in words2} count(b, c)`.
//   1. Compute a single 26-element `required` frequency envelope by taking the
//      element-wise maximum of letter frequencies across all words in `words2`.
//   2. For each word in `words1`, compute its 26-element letter frequency and
//      include the word in the result iff every letter count is `>= required`.
//   Time Complexity: O(L1 + L2 + 26 * (|words1| + |words2|)) = O(L1 + L2),
//   where L1 and L2 are the total character counts of `words1` and `words2`.
//   Space Complexity: O(1) auxiliary space (fixed 26-int frequency arrays),
//   excluding the returned output list.
//
// Data Structures:
//   - Fixed-size frequency table (`std::array<int, 26>`): acts as an O(1),
//     cache-friendly hash table for lowercase ASCII letters `'a'..'z'`,
//     representing both per-word multisets and the merged requirement envelope.
//
// Real-World Applications:
//   - Query predicate merging in search engines and database optimizers:
//     folding many conjunctive lower-bound constraints into a single tight
//     filter before scanning candidate records.
//   - Role-based access control (RBAC) & capability matching: merging multiple
//     required permission sets into one minimal required capability vector.
//   - Supply-chain bill-of-materials checks: collapsing overlapping part
//     requirements across sub-orders into peak component demand.
//
// Related LeetCode Problems:
//   - #242 Valid Anagram [done] (exact 26-letter frequency equality check)
//   - #383 Ransom Note [done] (single-target character multiset containment)
//   - #438 Find All Anagrams in a String [done] (sliding-window 26-letter frequency matching)
//   - #1160 Find Words That Can Be Formed by Characters [new] (reverse direction: words covered by one character pool)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Returns all strings in `words1` that contain every string in `words2` as a
  // letter multiset subset.
  // Time: O(L1 + L2), Space: O(1) auxiliary.
  std::vector<std::string> wordSubsets(const std::vector<std::string>& words1,
                                       const std::vector<std::string>& words2) {
    // Collapse all words in `words2` into one tightest per-letter lower bound.
    FreqTable required{};
    for (const std::string& pattern : words2) {
      const FreqTable freq = count_letters(pattern);
      for (std::size_t i = 0uz; i < kAlphabetSize; ++i) {
        required[i] = std::max(required[i], freq[i]);
      }
    }

    // Filter `words1` against the single merged requirement vector.
    std::vector<std::string> result;
    result.reserve(words1.size());
    for (const std::string& candidate : words1) {
      if (satisfies(count_letters(candidate), required)) {
        result.push_back(candidate);
      }
    }
    return result;
  }

 private:
  static constexpr std::size_t kAlphabetSize = 26uz;
  using FreqTable = std::array<int, kAlphabetSize>;

  // Counts occurrences of each lowercase letter `'a'..'z'` in `word`.
  static FreqTable count_letters(std::string_view word) {
    FreqTable freq{};
    for (const char ch : word) {
      const auto idx = static_cast<std::size_t>(
          static_cast<unsigned char>(ch) - static_cast<unsigned char>('a'));
      ++freq[idx];
    }
    return freq;
  }

  // Returns true iff `freq` meets or exceeds `required` for every letter.
  static bool satisfies(const FreqTable& freq, const FreqTable& required) {
    for (std::size_t i = 0uz; i < kAlphabetSize; ++i) {
      if (freq[i] < required[i]) {
        return false;
      }
    }
    return true;
  }
};

LC_PROBLEM(916, "Word Subsets", "hash", ctx) {
  Solution sol;

  ctx.test("example_1_single_letters", [&] {
    const std::vector<std::string> words1 = {"amazon", "apple", "facebook",
                                             "google", "leetcode"};
    const std::vector<std::string> words2 = {"e", "o"};
    const std::vector<std::string> expected = {"facebook", "google",
                                               "leetcode"};
    ctx.expect_eq(sol.wordSubsets(words1, words2), expected);
  });

  ctx.test("multiplicity_constraint", [&] {
    // Requires at least one 'e' and two 'o's.
    const std::vector<std::string> words1 = {"amazon", "apple", "facebook",
                                             "google", "leetcode"};
    const std::vector<std::string> words2 = {"e", "oo"};
    const std::vector<std::string> expected = {"facebook", "google"};
    ctx.expect_eq(sol.wordSubsets(words1, words2), expected);
  });

  ctx.test("overlapping_multi_letter_patterns", [&] {
    // "lo" and "eo" together require 1 'l', 1 'e', and 1 'o' (not 2 'o's).
    const std::vector<std::string> words1 = {"amazon", "apple", "facebook",
                                             "google", "leetcode"};
    const std::vector<std::string> words2 = {"lo", "eo"};
    const std::vector<std::string> expected = {"google", "leetcode"};
    ctx.expect_eq(sol.wordSubsets(words1, words2), expected);
  });

  ctx.test("docstring_warrior_vs_world", [&] {
    const std::vector<std::string> words1 = {"warrior", "world"};
    const std::vector<std::string> words2 = {"wrr", "r"};
    const std::vector<std::string> expected = {"warrior"};
    ctx.expect_eq(sol.wordSubsets(words1, words2), expected);
  });

  ctx.test("no_universal_matches", [&] {
    const std::vector<std::string> words1 = {"abc", "def"};
    const std::vector<std::string> words2 = {"z"};
    const std::vector<std::string> expected = {};
    ctx.expect_eq(sol.wordSubsets(words1, words2), expected);
  });
}

}  // namespace
