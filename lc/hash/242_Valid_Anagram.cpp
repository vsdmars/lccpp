// LeetCode 242: Valid Anagram
// https://leetcode.com/problems/valid-anagram/
//
// Problem:
//   Given two strings `s` and `t`, return true if `t` is an anagram of `s`
//   (contains the exact same characters with the exact same frequencies), and
//   false otherwise.
//   Constraints: 1 <= s.length, t.length <= 5 * 10^4; lowercase English letters.
//   Follow-up: How to adapt if inputs contain arbitrary Unicode code points?
//
// Examples:
//   Example 1:
//     Input:  s = "anagram", t = "nagaram"
//     Output: true
//     Explanation:
//       - Check `s.size() == t.size()` (both equal `7`) and zero-initialize the
//         26-element `counts` array.
//       - In one loop over `i = 0..6`, increment `counts[s[i] - 'a']` and decrement
//         `counts[t[i] - 'a']`, yielding net balances `'a': 3 - 3 = 0`, `'n': 1 - 1
//         = 0`, `'g': 1 - 1 = 0`, `'r': 1 - 1 = 0`, and `'m': 1 - 1 = 0`.
//       - Confirm via `std::ranges::all_of` that all 26 entries in `counts` equal
//         `0`, returning `true`.
//
//   Example 2:
//     Input:  s = "rat", t = "car"
//     Output: false
//     Explanation:
//       - Verify equal length `3` and allocate the zero-initialized 26-element
//         `counts` array.
//       - Scan `i = 0..2` updating `(r, c)`, `(a, a)`, and `(t, r)`, which leaves
//         `counts['c' - 'a'] = -1` and `counts['t' - 'a'] = +1` while `'r'` and
//         `'a'` net to `0`.
//       - Detect nonzero entries in `counts` via `std::ranges::all_of` and return
//         `false`.
//
//   Example 3:
//     Input:  s = "aacc", t = "ccac"
//     Output: false
//     Explanation:
//       - Verify equal length `4` and scan `i = 0..3` incrementing counts for `s`
//         and decrementing for `t`.
//       - After all four positions, `counts['a' - 'a']` is `2 - 1 = +1` and
//         `counts['c' - 'a']` is `2 - 3 = -1`.
//       - Since `counts` contains nonzero entries, `std::ranges::all_of` evaluates
//         to `false`.
//
// Algorithm:
//   1. Lowercase ASCII (`isAnagram`):
//      First check `s.size() == t.size()`; different lengths cannot be anagrams.
//      Use a single fixed-size 26-element balance array `counts`. In one pass
//      over index `i`, increment `counts[s[i] - 'a']` and decrement
//      `counts[t[i] - 'a']`. The strings are anagrams iff every entry is 0.
//      Time:  O(n)
//      Space: O(1) (fixed 26-int array)
//   2. Unicode Follow-up (`isAnagramUnicode`):
//      Replace the 26-entry array with `std::unordered_map<char32_t, int>` over
//      UTF-32 code points so any Unicode alphabet is supported.
//      Time:  O(n) average
//      Space: O(k), where k is the number of distinct code points in `s`.
//
// Data Structures:
//   - std::array<int, 26>: Direct-addressed frequency balance table for lowercase
//     English letters.
//   - std::unordered_map<char32_t, int>: Sparse frequency map for the Unicode
//     follow-up.
//
// Real-World Applications:
//   - Comparing unordered multisets (e.g., verifying inventory reconciliation or
//     bag-of-words equivalence in O(n) without sorting).
//   - Word-puzzle validation, linguistic analysis, and spell-checker candidate
//     filtering.
//   - Checking equivalence of unordered argument/tag collections in compilers
//     and query planners.
//
// Related LeetCode Problems:
//   - #49 Group Anagrams [done] (partitioning many strings into anagram classes)
//   - #266 Palindrome Permutation [done] (character-frequency parity check)
//   - #438 Find All Anagrams in a String [done] (sliding-window frequency matching)
//   - #383 Ransom Note [done] (subset character-frequency check)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <string_view>
#include <unordered_map>

namespace {

constexpr std::size_t kAlphabetSize = 26uz;

class Solution {
 public:
  // Checks whether `t` is an anagram of `s` for lowercase English strings.
  // Invariant: after scanning prefix [0..i], counts[c] == freq(s, c) - freq(t, c).
  // Time: O(n), Space: O(1).
  bool isAnagram(std::string_view s, std::string_view t) {
    if (s.size() != t.size()) {
      return false;
    }

    std::array<int, kAlphabetSize> counts{};
    for (std::size_t i = 0uz; i < s.size(); ++i) {
      ++counts[static_cast<std::size_t>(s[i] - 'a')];
      --counts[static_cast<std::size_t>(t[i] - 'a')];
    }

    return std::ranges::all_of(counts, [](int delta) { return delta == 0; });
  }

  // Follow-up: checks whether two UTF-32 code-point sequences are anagrams
  // using a hash map to support arbitrary Unicode characters.
  // Time: O(n) average, Space: O(k) distinct code points.
  bool isAnagramUnicode(std::u32string_view s, std::u32string_view t) {
    if (s.size() != t.size()) {
      return false;
    }

    std::unordered_map<char32_t, int> counts;
    counts.reserve(s.size());
    for (char32_t cp : s) {
      ++counts[cp];
    }
    for (char32_t cp : t) {
      const auto it = counts.find(cp);
      if (it == counts.end() || it->second == 0) {
        return false;
      }
      --it->second;
    }
    return true;
  }
};

LC_PROBLEM(242, "Valid Anagram", "hash", ctx) {
  Solution sol;

  ctx.test("leetcode_examples", [&] {
    ctx.expect_true(sol.isAnagram("anagram", "nagaram"));
    ctx.expect_false(sol.isAnagram("rat", "car"));
  });

  ctx.test("edge_cases", [&] {
    ctx.expect_true(sol.isAnagram("", ""));
    ctx.expect_true(sol.isAnagram("a", "a"));
    ctx.expect_false(sol.isAnagram("a", "ab"));
    ctx.expect_false(sol.isAnagram("aacc", "ccac"));
  });

  ctx.test("unicode_followup", [&] {
    ctx.expect_true(sol.isAnagramUnicode(U"café", U"éfac"));
    ctx.expect_false(sol.isAnagramUnicode(U"café", U"cafe"));
    ctx.expect_true(sol.isAnagramUnicode(U"你好世界", U"界世好你"));
  });
}

}  // namespace
