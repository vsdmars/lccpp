// LeetCode 1554: Strings Differ By One Character
// https://leetcode.com/problems/strings-differ-by-one-character/
//
// Problem:
//   Given a list `dict` of `n` unique strings of identical length `m`,
//   consisting only of lowercase English letters, return `true` if there exist
//   two strings in `dict` that differ in exactly one character at the same
//   index, and `false` otherwise.
//   Constraints: total number of characters n * m <= 10^5; all strings have
//   equal length m; strings are unique and lowercase English letters.
//   Follow-up: Achieve O(n * m) time complexity.
//
// Examples:
//   Example 1:
//     Input:  dict = ["abcd", "acbd", "aacd"]
//     Output: true
//     Explanation:
//       - compute_hashes encodes each 4-letter word with base 131 mod 1000000007
//         (char_weight(c) = c - 'a' + 1), producing hashes [2282810, 2299840,
//         2265649].
//       - At col = 3 (power = 1) and col = 2 (power = 131), subtracting
//         char_weight(dict[i][col]) * power yields distinct masked hashes across
//         all three strings.
//       - At col = 1 (power = 17161), removing 'b' (2 * 17161 = 34322) from "abcd"
//         gives masked hash 2248488 and removing 'a' (1 * 17161 = 17161) from
//         "aacd" also gives 2248488.
//       - differs_only_at("abcd", "aacd", 1) confirms 'b' != 'a' with matching
//         prefix "a" and suffix "cd", returning true.
//
//   Example 2:
//     Input:  dict = ["ab", "cd", "yz"]
//     Output: false
//     Explanation:
//       - compute_hashes computes the full polynomial hashes 133 for "ab", 397 for
//         "cd", and 3301 for "yz".
//       - At col = 1 (power = 1), masking out the last character produces distinct
//         hashes 131, 393, and 3275 in seen.
//       - At col = 0 (power = 131), masking out the first character produces
//         distinct hashes 2, 4, and 26, so no column finds a match and differByOne
//         returns false.
//
//   Example 3:
//     Input:  dict = ["abcd", "abcd"]
//     Output: false
//     Explanation:
//       - Both strings produce the identical full hash 2282810 in hashes[0] and
//         hashes[1].
//       - At every column col from 3 down to 0, masking out dict[i][col] produces
//         equal masked hashes for both indices and enters the bucket check.
//       - differs_only_at("abcd", "abcd", col) sees a[col] == b[col] at every
//         column and rejects the pair, returning false.
//
// Algorithm:
//   Polynomial rolling hash (Rabin-Karp) with column-wise wildcard masking and
//   exact collision verification:
//   1. Precompute the full polynomial hash H(s) = sum_{j=0}^{m-1} val(s[j]) *
//      B^(m-1-j) mod M for every string in O(n * m) time, where val(c) =
//      (c - 'a') + 1 so 'a' contributes a non-zero weight.
//   2. Sweep columns from right to left (col = m - 1 down to 0), maintaining
//      `power = B^(m - 1 - col) mod M`. Subtracting `val(s[col]) * power mod M`
//      from H(s) masks out position `col` in O(1) time without copying strings.
//   3. Group string indices by their masked hash inside a hash map for the
//      current column. Whenever two strings share a masked hash, explicitly
//      verify that their prefixes [0..col) and suffixes (col..m) match and
//      their characters at `col` differ. Exact verification guarantees zero
//      false positives even under adversarial hash collisions while keeping
//      expected work O(n * m).
//   Time Complexity: O(n * m) expected.
//   Space Complexity: O(n) auxiliary space for hashes and the per-column map.
//
// Data Structures:
//   - std::vector<std::uint64_t>: Stores each string's full polynomial hash.
//   - std::unordered_map<std::uint64_t, std::vector<std::size_t>>: Maps a
//     column-masked hash to the indices of strings producing that hash so
//     candidates can be verified in O(1) average bucket size.
//
// Real-World Applications:
//   - Spell checkers and autocomplete engines detecting single-substitution
//     typos (Hamming distance 1) across large dictionaries.
//   - Bioinformatics DNA/RNA k-mer clustering and SNP (single-nucleotide
//     polymorphism) detection across fixed-length sequence reads.
//   - Coding theory and error-detection verification checking minimum Hamming
//     distance between fixed-width codewords.
//
// Related LeetCode Problems:
//   - #127 Word Ladder [done] (Hamming-distance-1 adjacency via wildcard signatures)
//   - #676 Implement Magic Dictionary [done] (querying strings differing by one character)
//   - #1044 Longest Duplicate Substring [done] (Rabin-Karp polynomial rolling hash)
//   - #1638 Count Substrings That Differ by One Character [new] (counting Hamming-distance-1 pairs)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Returns true if any two strings in `dict` differ at exactly one index.
  // Time: O(n * m) expected, Space: O(n).
  bool differByOne(const std::vector<std::string>& dict) {
    if (dict.size() < 2uz || dict[0].empty()) {
      return false;
    }

    const std::size_t m = dict[0].size();
    const std::vector<std::uint64_t> hashes = compute_hashes(dict);

    // Sweep columns from right to left so `power` starts at B^0 = 1 and
    // advances by a single modular multiplication per column.
    std::uint64_t power = 1ULL;
    for (std::size_t step = 0uz; step < m; ++step) {
      const std::size_t col = m - 1uz - step;
      if (has_match_at_column(dict, hashes, col, power)) {
        return true;
      }
      power = (power * kBase) % kMod;
    }

    return false;
  }

 private:
  static constexpr std::uint64_t kBase = 131ULL;
  static constexpr std::uint64_t kMod = 1'000'000'007ULL;

  // Maps a lowercase character to a non-zero 1-based weight so leading 'a's
  // affect the polynomial hash.
  static constexpr std::uint64_t char_weight(char c) {
    return static_cast<std::uint64_t>(static_cast<unsigned char>(c) -
                                      static_cast<unsigned char>('a')) +
           1ULL;
  }

  // Computes the full polynomial rolling hash for every string in `dict`.
  static std::vector<std::uint64_t> compute_hashes(
      const std::vector<std::string>& dict) {
    std::vector<std::uint64_t> hashes;
    hashes.reserve(dict.size());
    for (const auto& word : dict) {
      std::uint64_t h = 0ULL;
      for (const char c : word) {
        h = ((h * kBase) + char_weight(c)) % kMod;
      }
      hashes.push_back(h);
    }
    return hashes;
  }

  // Explicitly verifies that `a` and `b` match everywhere except index `col`,
  // eliminating any possibility of false positives from hash collisions.
  static bool differs_only_at(std::string_view a, std::string_view b,
                              std::size_t col) {
    return a[col] != b[col] && a.substr(0uz, col) == b.substr(0uz, col) &&
           a.substr(col + 1uz) == b.substr(col + 1uz);
  }

  // Masks out column `col` in O(1) per string using `power = B^(m-1-col) % M`
  // and checks whether any two strings collide and genuinely differ only there.
  static bool has_match_at_column(const std::vector<std::string>& dict,
                                  const std::vector<std::uint64_t>& hashes,
                                  std::size_t col, std::uint64_t power) {
    std::unordered_map<std::uint64_t, std::vector<std::size_t>> seen;
    seen.reserve(dict.size());

    for (std::size_t i = 0uz; i < dict.size(); ++i) {
      const std::uint64_t removed = (char_weight(dict[i][col]) * power) % kMod;
      const std::uint64_t masked = (hashes[i] + kMod - removed) % kMod;

      auto& bucket = seen[masked];
      for (const std::size_t prev : bucket) {
        if (differs_only_at(dict[prev], dict[i], col)) {
          return true;
        }
      }
      bucket.push_back(i);
    }
    return false;
  }
};

LC_PROBLEM(1554, "Strings Differ By One Character", "hash", ctx) {
  Solution sol;

  ctx.test("example_1_true", [&] {
    const std::vector<std::string> dict = {"abcd", "acbd", "aacd"};
    ctx.expect_true(sol.differByOne(dict));
  });

  ctx.test("example_2_false", [&] {
    const std::vector<std::string> dict = {"ab", "cd", "yz"};
    ctx.expect_false(sol.differByOne(dict));
  });

  ctx.test("example_3_true", [&] {
    const std::vector<std::string> dict = {"abcd", "cccc", "abyd", "abab"};
    ctx.expect_true(sol.differByOne(dict));
  });

  ctx.test("single_character_strings", [&] {
    ctx.expect_true(sol.differByOne({"a", "b"}));
    ctx.expect_false(sol.differByOne({"a"}));
  });

  ctx.test("differ_at_first_or_last_index", [&] {
    ctx.expect_true(sol.differByOne({"zbcde", "abcde"}));
    ctx.expect_true(sol.differByOne({"abcde", "abcdf"}));
    ctx.expect_false(sol.differByOne({"abcde", "abcfg"}));
  });

  ctx.test("identical_strings_do_not_differ_by_one", [&] {
    ctx.expect_false(sol.differByOne({"abcd", "abcd"}));
  });
}

}  // namespace
