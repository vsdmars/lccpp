// LeetCode 205: Isomorphic Strings
// https://leetcode.com/problems/isomorphic-strings/
//
// Problem:
//   Given two strings `s` and `t`, determine whether they are isomorphic. Two
//   strings are isomorphic if the characters in `s` can be replaced to obtain
//   `t` such that every occurrence of a character maps consistently to the
//   same character, order is preserved, and no two distinct characters in `s`
//   map to the same character in `t` (a character may map to itself).
//   Constraints: 1 <= s.length <= 5 * 10^4, t.length == s.length, valid ASCII.
//
// Examples:
//   Example 1:
//     Input:  s = "egg", t = "add"
//     Output: true
//     Explanation:
//       - Initialize 256-entry tables s_to_t and t_to_s to -1.
//       - At i = 0, 'e' and 'a' are unmapped, so s_to_t['e'] = 'a' and t_to_s['a']
//         = 'e'; at i = 1, s_to_t['g'] = 'd' and t_to_s['d'] = 'g'.
//       - At i = 2, s_to_t['g'] already equals 'd' and t_to_s['d'] already equals
//         'g', so the loop finishes and returns true.
//
//   Example 2:
//     Input:  s = "badc", t = "baba"
//     Output: false
//     Explanation:
//       - At i = 0 and i = 1, the loop records s_to_t['b'] = 'b', t_to_s['b'] =
//         'b', s_to_t['a'] = 'a', and t_to_s['a'] = 'a'.
//       - At i = 2 (s[2] = 'd', t[2] = 'b'), s_to_t['d'] is -1 but t_to_s['b'] is
//         already 'b' != 'd', violating injectivity.
//       - The reverse-mapping conflict check triggers immediately and returns
//         false.
//
//   Example 3:
//     Input:  s = "ab", t = "a"
//     Output: false
//     Explanation:
//       - The entry guard compares s.size() (2) with t.size() (1).
//       - Because the lengths are unequal, isIsomorphic returns false immediately
//         without scanning characters.
//
// Algorithm:
//   Isomorphism requires a bijection (one-to-one and onto correspondence)
//   between the characters occurring in `s` and those occurring in `t`.
//   Maintain two fixed-size direct-mapped lookup tables `s_to_t` and `t_to_s`
//   initialized to -1 (unmapped). For each index `i`:
//   - If `s[i]` was already mapped to a different character than `t[i]`, the
//     forward mapping is inconsistent -> return false.
//   - If `t[i]` was already mapped from a different character than `s[i]`,
//     two distinct characters in `s` collide on `t[i]` -> return false.
//   - Otherwise record both directions `s_to_t[s[i]] = t[i]` and
//     `t_to_s[t[i]] = s[i]`.
//   Note: checking only one direction (or `|zip(s,t)| == |set(t)|` without
//   `|set(s)|`) fails on cases like `s = "egg", t = "adc"`.
//   Time:  O(n)
//   Space: O(1) (two 256-entry ASCII tables)
//
// Data Structures:
//   - std::array<int, 256> s_to_t: Forward character substitution table.
//   - std::array<int, 256> t_to_s: Reverse character substitution table
//     enforcing injectivity (no two source chars map to the same target char).
//
// Real-World Applications:
//   - Alpha-equivalence checking in compilers and theorem provers (verifying
//     two expressions differ only by consistent variable renaming).
//   - Monoalphabetic substitution cipher matching and pattern normalization
//     in cryptanalysis and log-template clustering.
//   - Schema/identifier canonicalization when comparing anonymized traces.
//
// Related LeetCode Problems:
//   - #290 Word Pattern [done] (same bijection check between chars and words)
//   - #890 Find and Replace Pattern [new] (filtering words isomorphic to a pattern)
//   - #49 Group Anagrams [done] (grouping strings by structural equivalence)
//   - #249 Group Shifted Strings [done] (equivalence classes under cyclic shifts)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <string_view>
#include <utility>

namespace {

constexpr std::size_t kAsciiSize = 256uz;

class Solution {
 public:
  // Returns true iff there exists a character-level bijection mapping `s` to
  // `t` while preserving character positions.
  // Time: O(n), Space: O(1).
  bool isIsomorphic(std::string_view s, std::string_view t) {
    if (s.size() != t.size()) {
      return false;
    }

    // Direct-mapped tables for both directions of the bijection (-1 = unseen).
    std::array<int, kAsciiSize> s_to_t{};
    std::array<int, kAsciiSize> t_to_s{};
    s_to_t.fill(-1);
    t_to_s.fill(-1);

    for (std::size_t i = 0uz; i < s.size(); ++i) {
      // Cast via unsigned char to avoid negative indices for extended ASCII.
      const auto sc = static_cast<unsigned char>(s[i]);
      const auto tc = static_cast<unsigned char>(t[i]);

      // Reject if either forward or reverse mapping conflicts with prior pairs.
      if (s_to_t[sc] != -1 && std::cmp_not_equal(s_to_t[sc], tc)) {
        return false;
      }
      if (t_to_s[tc] != -1 && std::cmp_not_equal(t_to_s[tc], sc)) {
        return false;
      }

      s_to_t[sc] = static_cast<int>(tc);
      t_to_s[tc] = static_cast<int>(sc);
    }

    return true;
  }
};

LC_PROBLEM(205, "Isomorphic Strings", "hash", ctx) {
  Solution sol;

  ctx.test("leetcode_examples", [&] {
    ctx.expect_true(sol.isIsomorphic("egg", "add"));
    ctx.expect_false(sol.isIsomorphic("foo", "bar"));
    ctx.expect_true(sol.isIsomorphic("paper", "title"));
  });

  ctx.test("both_directions_required", [&] {
    // 'g' cannot map to both 'd' and 'c' (fixes the one-sided check bug).
    ctx.expect_false(sol.isIsomorphic("egg", "adc"));
    // 'b' and 'd' cannot both map to 'b'.
    ctx.expect_false(sol.isIsomorphic("badc", "baba"));
    // 'a' and 'b' cannot both map to 'a'.
    ctx.expect_false(sol.isIsomorphic("ab", "aa"));
  });

  ctx.test("edge_cases", [&] {
    ctx.expect_true(sol.isIsomorphic("", ""));
    ctx.expect_true(sol.isIsomorphic("a", "b"));
    ctx.expect_true(sol.isIsomorphic("ab", "ab"));
    ctx.expect_false(sol.isIsomorphic("ab", "a"));
  });
}

}  // namespace
