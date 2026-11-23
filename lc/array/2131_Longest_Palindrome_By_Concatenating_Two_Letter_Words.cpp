// LeetCode 2131: Longest Palindrome By Concatenating Two Letter Words
// https://leetcode.com/problems/longest-palindrome-by-concatenating-two-letter-words/
//
// Problem:
//   Given an array `words` of 2-character lowercase English strings, select
//   any subset of elements (each used at most once) and concatenate them in
//   any order to form the longest possible palindrome. Return the length of
//   that longest palindrome, or 0 if no non-empty palindrome can be formed.
//   Constraints: 1 <= words.size() <= 10^5; words[i].size() == 2; lowercase
//   English letters `'a'`..`'z'`.
//
// Algorithm:
//   Greedy Pairing with a Fixed 26 x 26 Frequency Matrix:
//   Every two-letter word `(a, b)` can pair symmetrically on opposite ends of
//   the palindrome with its reverse `(b, a)`, adding 4 characters to the total
//   length. Notice this rule works uniformly for both asymmetric words (e.g.,
//   `"lc"` + `"cl"`) and symmetric words (e.g., two copies of `"gg"`):
//   1. Maintain a 26 x 26 table `counts[a][b]` of currently unpaired words.
//   2. For each word `(a, b)`:
//      - If `counts[b][a] > 0`, greedily match it with one pending `(b, a)`,
//        decrement `counts[b][a]`, and add 4 to `palindrome_len`.
//      - Otherwise, increment `counts[a][b]` to hold it for a future match.
//   3. After all words are processed, scan the diagonal `counts[i][i]` for
//      `0 <= i < 26`. At most one leftover symmetric word (e.g., `"gg"`) can
//      sit at the exact center of the palindrome, contributing +2 to the
//      length.
//   Time Complexity:  O(n + |Sigma|) where |Sigma| = 26
//   Space Complexity: O(|Sigma|^2) = O(1) fixed stack-allocated 26 x 26 table
//
// Data Structures:
//   - `std::array<std::array<int, 26>, 26>`: constant-size direct-addressed
//     frequency matrix mapping each two-letter word to O(1) lookup/update
//     without string hashing or heap allocations.
//
// Real-World Applications:
//   - Bioinformatics: pairing reverse-complement k-mers / dinucleotides when
//     detecting palindromic restriction sites and hairpin stems in DNA/RNA.
//   - Network & order-book matching engines pairing complementary bid/ask or
//     directed-edge tokens `(u, v)` with `(v, u)` in O(1) per event.
//   - Data compression and symmetry detection in fixed-width bigram streams.
//
// Related LeetCode Problems:
//   - #409 Longest Palindrome [done] (greedy character-pair counting plus at most one odd center)
//   - #336 Palindrome Pairs [done] (generalized palindrome concatenation over variable-length words)
//   - #125 Valid Palindrome [done] (fundamental palindrome verification)
//   - #1400 Construct K Palindrome Strings [new] (frequency parity analysis for palindromes)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <string>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the maximum length of a palindrome formed by concatenating
  // selected two-letter words.
  //
  // Invariant: After processing a prefix of `words`, `palindrome_len` equals
  // 4 times the maximum number of disjoint reverse pairs formed within that
  // prefix, and `counts[a][b]` holds the count of currently unmatched words
  // equal to `ab`.
  //
  // Time: O(n + 26), Space: O(26^2) = O(1).
  int longestPalindrome(const std::vector<std::string>& words) {
    static constexpr std::size_t kAlphabetSize = 26uz;
    std::array<std::array<int, kAlphabetSize>, kAlphabetSize> counts{};
    int palindrome_len = 0;

    // Phase 1: Greedily match every word with an unpaired reverse partner.
    for (const std::string& word : words) {
      const auto first = static_cast<std::size_t>(word[0] - 'a');
      const auto second = static_cast<std::size_t>(word[1] - 'a');

      if (counts[second][first] > 0) {
        // Pair `word` with a waiting reverse word on opposite ends (+4 chars).
        palindrome_len += 4;
        --counts[second][first];
      } else {
        // No reverse partner waiting yet; record `word` as unpaired.
        ++counts[first][second];
      }
    }

    // Phase 2: At most one leftover symmetric word (e.g., "gg") can occupy the
    // center of the palindrome (+2 chars).
    for (std::size_t i = 0uz; i < kAlphabetSize; ++i) {
      if (counts[i][i] > 0) {
        palindrome_len += 2;
        break;
      }
    }

    return palindrome_len;
  }
};

LC_PROBLEM(2131, "Longest Palindrome By Concatenating Two Letter Words",
           "array", ctx) {
  ctx.test("example_1_pair_and_center", [&] {
    Solution sol;
    ctx.expect_eq(sol.longestPalindrome({"lc", "cl", "gg"}), 6);
  });

  ctx.test("example_2_multiple_pairs_no_center", [&] {
    Solution sol;
    ctx.expect_eq(sol.longestPalindrome({"ab", "ty", "yt", "lc", "cl", "ab"}),
                  8);
  });

  ctx.test("example_3_only_distinct_symmetric_words", [&] {
    Solution sol;
    ctx.expect_eq(sol.longestPalindrome({"cc", "ll", "xx"}), 2);
  });

  ctx.test("odd_and_even_symmetric_multiplicities", [&] {
    Solution sol;
    // "aa" x3 -> one pair (4) + one leftover for center (2);
    // "bb" x2 -> one pair (4); total = 10.
    ctx.expect_eq(sol.longestPalindrome({"aa", "aa", "aa", "bb", "bb"}), 10);
  });

  ctx.test("no_palindrome_possible", [&] {
    Solution sol;
    ctx.expect_eq(sol.longestPalindrome({"ab", "cd", "ef"}), 0);
  });

  ctx.test("multiple_duplicate_reverse_pairs", [&] {
    Solution sol;
    // Two ("ab", "ba") pairs -> 8, plus one unused "ab".
    ctx.expect_eq(sol.longestPalindrome({"ab", "ab", "ab", "ba", "ba"}), 8);
  });
}

}  // namespace
