// LeetCode 2301: Match Substring After Replacement
// https://leetcode.com/problems/match-substring-after-replacement/
//
// Problem:
//   Given two strings `s` and `sub`, and a list of character replacement rules
//   `mappings` where `mappings[i] = [old_i, new_i]` permits replacing `old_i`
//   in `sub` with `new_i`, determine whether `sub` can be transformed into a
//   contiguous substring of `s`. Each character position in `sub` may be
//   replaced at most once (so mappings do not chain transitively).
//   Constraints: 1 <= sub.length <= s.length <= 5000; 0 <= mappings.length <=
//   1000; characters are uppercase/lowercase English letters and digits.
//
// Algorithm:
//   Direct-address character lookup table with sliding-window matching:
//   1. Build a 256 x 256 bitset table `can_replace` representing a collision-
//      free O(1) hash table over all byte values. Set the identity diagonal
//      `can_replace[c][c] = true` (zero replacements needed when characters
//      already match) and mark `can_replace[old_c][new_c] = true` for every
//      rule in `mappings`.
//   2. Slide a window of length `m = sub.size()` over every valid starting
//      offset `start` in `[0, n - m]` of `s`. At each offset, verify whether
//      every position `j` in `[0, m)` satisfies
//      `can_replace[sub[j]][s[start + j]]`, breaking early on the first
//      mismatch.
//   Time Complexity: O(|mappings| + (n - m + 1) * m) worst-case, where
//   n = s.size() and m = sub.size(); in practice mismatches fail after O(1)
//   comparisons per offset.
//   Space Complexity: O(1) (fixed 8 KB bitset table for all 256 byte values).
//
// Data Structures:
//   - std::array<std::bitset<256>, 256>: Fixed-size direct-address lookup
//     table mapping `(sub_char, target_char)` pairs to reachability in O(1)
//     time with zero dynamic allocations or hash collisions.
//
// Real-World Applications:
//   - Content moderation and anti-spam filters detecting "leetspeak" or
//     homoglyph obfuscations (e.g., matching "l33t" against "leet").
//   - OCR post-processing and barcode/VIN matching where specific character
//     confusions (e.g., '0'/'O', '1'/'l') are permitted unilaterally.
//   - Bioinformatics IUPAC degenerate motif matching where pattern symbols
//     match defined subsets of concrete nucleotide/amino-acid characters.
//
// Related LeetCode Problems:
//   - #28 Find the Index of the First Occurrence in a String [done] (exact substring matching)
//   - #205 Isomorphic Strings [done] (character-to-character mapping constraints)
//   - #796 Rotate String [new] (substring search over fixed-length windows)
//   - #2300 Successful Pairs of Spells and Potions [new] (companion problem from the same contest)

#include "framework/lc_framework.h"

#include <array>
#include <bitset>
#include <cstddef>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Returns true if `sub` can match some contiguous substring of `s` after
  // applying at most one allowed replacement per character position in `sub`.
  // Time: O(|mappings| + (n - m + 1) * m), Space: O(1).
  bool matchReplacement(std::string_view s, std::string_view sub,
                        const std::vector<std::vector<char>>& mappings) {
    if (sub.empty()) {
      return true;
    }
    if (sub.size() > s.size()) {
      return false;
    }

    // Build direct-address reachability table: every character always matches
    // itself, plus any explicitly permitted one-step replacement.
    LookupTable can_replace{};
    for (std::size_t c = 0uz; c < kCharSpace; ++c) {
      can_replace[c].set(c);
    }
    for (const auto& rule : mappings) {
      if (rule.size() >= 2uz) {
        can_replace[to_index(rule[0])].set(to_index(rule[1]));
      }
    }

    // Try every starting position in `s` where `sub` fits completely.
    const std::size_t max_start = s.size() - sub.size();
    for (std::size_t start = 0uz; start <= max_start; ++start) {
      if (matches_at(s, sub, start, can_replace)) {
        return true;
      }
    }
    return false;
  }

 private:
  static constexpr std::size_t kCharSpace = 256uz;
  using LookupTable = std::array<std::bitset<kCharSpace>, kCharSpace>;

  // Safely converts a possibly-signed `char` into an unsigned `[0, 255]` index.
  static constexpr std::size_t to_index(char c) {
    return static_cast<std::size_t>(static_cast<unsigned char>(c));
  }

  // Checks whether `sub` matches `s[start .. start + sub.size())` under the
  // allowed one-step character replacements in `can_replace`.
  static bool matches_at(std::string_view s, std::string_view sub,
                         std::size_t start, const LookupTable& can_replace) {
    for (std::size_t j = 0uz; j < sub.size(); ++j) {
      if (!can_replace[to_index(sub[j])].test(to_index(s[start + j]))) {
        return false;
      }
    }
    return true;
  }
};

LC_PROBLEM(2301, "Match Substring After Replacement", "hash", ctx) {
  Solution sol;

  ctx.test("example_1_leetspeak_match", [&] {
    const std::vector<std::vector<char>> mappings = {
        {'e', '3'}, {'t', '7'}, {'t', '8'}};
    ctx.expect_true(sol.matchReplacement("fool3e7bar", "leet", mappings));
  });

  ctx.test("example_2_reverse_mapping_disallowed", [&] {
    const std::vector<std::vector<char>> mappings = {{'o', '0'}};
    ctx.expect_false(sol.matchReplacement("fooleetbar", "f00l", mappings));
  });

  ctx.test("example_3_multiple_replacements", [&] {
    const std::vector<std::vector<char>> mappings = {
        {'e', '3'}, {'t', '7'}, {'t', '8'}, {'d', 'b'}, {'p', 'b'}};
    ctx.expect_true(sol.matchReplacement("Fool33tbaR", "leetd", mappings));
  });

  ctx.test("no_chaining_allowed", [&] {
    // 'a' -> 'b' and 'b' -> 'c' do not permit 'a' -> 'c' in a single step.
    const std::vector<std::vector<char>> mappings = {{'a', 'b'}, {'b', 'c'}};
    ctx.expect_false(sol.matchReplacement("c", "a", mappings));
    ctx.expect_true(sol.matchReplacement("b", "a", mappings));
  });

  ctx.test("exact_match_with_no_mappings", [&] {
    const std::vector<std::vector<char>> mappings = {};
    ctx.expect_true(sol.matchReplacement("abcdef", "cde", mappings));
    ctx.expect_false(sol.matchReplacement("abc", "abcd", mappings));
  });
}

}  // namespace
