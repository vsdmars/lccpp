// LeetCode 438: Find All Anagrams In A String
// https://leetcode.com/problems/find-all-anagrams-in-a-string/
//
// Problem:
//   Given two strings s and p consisting of lowercase English letters, return
//   all 0-based start indices of p's anagrams in s. An anagram is a substring
//   of length p.size() containing the exact same character multiplicities as p.
//
// Algorithm:
//   Fixed-size sliding window with 26-letter frequency signatures:
//   1. If p is empty or longer than s, no anagram can exist; return empty.
//   2. Compute the target frequency signature `p_freq` for all letters in p.
//   3. Pre-populate `window_freq` with the first `p.size() - 1` characters of s
//      so that each iteration of the main loop completes a window of exact
//      length `p.size()`.
//   4. For each right endpoint `right` from `p.size() - 1` to `s.size() - 1`:
//      - Include `s[right]` into `window_freq`.
//      - Compare `window_freq == p_freq` in O(26) = O(1); if equal, record
//        `left = (right + 1) - p.size()`.
//      - Remove `s[left]` from `window_freq` to prepare for the next slide.
//   Time Complexity: O(26 * n) = O(n), where n = s.size().
//   Space Complexity: O(1) auxiliary (two fixed 26-element arrays, excluding
//   the output vector).
//
// Data Structures:
//   - std::array<int, 26> (two instances): fixed-alphabet frequency vectors
//     representing the pattern signature and the active sliding window.
//
// Real-World Applications:
//   - Bioinformatics k-mer / motif composition scanning across genomic
//     sequences where base order within a short window may vary.
//   - Intrusion detection and log-stream pattern matching for fixed-window
//     event multisets (detecting anomalous bursts regardless of arrival order).
//   - Plagiarism and fuzzy text indexing using bag-of-characters fingerprints.
//
// Related LeetCode Problems:
//   - #567 Permutation in String [done] (boolean existence version of #438 using the same sliding window)
//   - #76 Minimum Window Substring [done] (variable-length sliding window over character frequencies)
//   - #242 Valid Anagram [done] (single static frequency comparison without sliding)
//   - #49 Group Anagrams [done] (grouping strings by their 26-letter frequency signature)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <string_view>
#include <vector>

namespace {

constexpr std::size_t kAlphabetSize = 26;

class Solution {
 public:
  // Returns all 0-based start indices in `s` where a substring of length
  // `p.size()` is an anagram of `p`.
  //
  // Invariant: Immediately before each iteration's check, `window_freq` holds
  // the exact letter counts of `s[left .. right]` of length `p.size()`.
  //
  // Time Complexity: O(n), where n = s.size().
  // Space Complexity: O(1) auxiliary space (excluding output).
  std::vector<int> findAnagrams(std::string_view s, std::string_view p) {
    if (p.empty() || s.size() < p.size()) {
      return {};
    }

    const std::size_t window_len = p.size();
    std::array<int, kAlphabetSize> p_freq{};
    std::array<int, kAlphabetSize> window_freq{};

    // Build target signature for p and seed the window with the first
    // (window_len - 1) characters of s.
    for (const char ch : p) {
      ++p_freq[static_cast<std::size_t>(ch - 'a')];
    }
    for (std::size_t i = 0uz; i < (window_len - 1); ++i) {
      ++window_freq[static_cast<std::size_t>(s[i] - 'a')];
    }

    std::vector<int> result;

    // Slide the fixed-width window across s one character at a time.
    for (std::size_t right = window_len - 1; right < s.size(); ++right) {
      ++window_freq[static_cast<std::size_t>(s[right] - 'a')];

      const std::size_t left = (right + 1) - window_len;
      if (window_freq == p_freq) {
        result.push_back(static_cast<int>(left));
      }

      // Evict the outgoing leftmost character before advancing `right`.
      --window_freq[static_cast<std::size_t>(s[left] - 'a')];
    }

    return result;
  }
};

LC_PROBLEM(438, "Find All Anagrams In A String", "hash", ctx) {
  Solution sol;

  ctx.test("example_1_cbaebabacd", [&] {
    ctx.expect_eq(sol.findAnagrams("cbaebabacd", "abc"), std::vector<int>{0, 6});
  });

  ctx.test("example_2_overlapping_abab", [&] {
    ctx.expect_eq(sol.findAnagrams("abab", "ab"), std::vector<int>{0, 1, 2});
  });

  ctx.test("duplicate_letters_in_pattern", [&] {
    ctx.expect_eq(sol.findAnagrams("ababb", "abb"), std::vector<int>{1, 2});
    ctx.expect_eq(sol.findAnagrams("aaaa", "aa"), std::vector<int>{0, 1, 2});
  });

  ctx.test("no_matches_and_short_source", [&] {
    ctx.expect_eq(sol.findAnagrams("abcdef", "xyz"), std::vector<int>{});
    ctx.expect_eq(sol.findAnagrams("ab", "abc"), std::vector<int>{});
  });

  ctx.test("exact_full_string_match", [&] {
    ctx.expect_eq(sol.findAnagrams("listen", "silent"), std::vector<int>{0});
  });
}

}  // namespace
