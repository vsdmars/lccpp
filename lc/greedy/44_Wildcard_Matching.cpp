// LeetCode 44: Wildcard Matching
// https://leetcode.com/problems/wildcard-matching/
//
// Problem:
//   Given an input string `s` and a pattern `p`, implement full-string wildcard
//   pattern matching where:
//     - '?' matches any single character.
//     - '*' matches any sequence of characters (including the empty sequence).
//   The match must cover the entire input string `s` (not a partial substring).
//
// Algorithm:
//   Greedy two-pointer scan with single-checkpoint backtracking (the classic
//   O(1)-space algorithm).
//   - Maintain indices `si` into `s` and `pi` into `p`, plus a single
//     backtracking checkpoint `(star_pi, star_match_si)` recording the most
//     recent `*` seen in `p` and the position in `s` up to which that `*` has
//     currently expanded.
//   - While `si < s.size()`:
//       1. If `pi < p.size()` and (`p[pi] == '?' || p[pi] == s[si]`), advance
//          both `si` and `pi`.
//       2. Else if `pi < p.size()` and `p[pi] == '*'`, greedily match 0
//          characters first by saving `star_pi = pi`, `star_match_si = si`,
//          and advancing `pi`.
//       3. Else if a prior `*` exists (`star_pi != npos`), backtrack `pi` to
//          `star_pi + 1` and let that `*` absorb one more character from `s`
//          (`si = ++star_match_si`).
//       4. Otherwise, no match is possible -> return `false`.
//   - Key greedy invariant: we only ever need to backtrack to the MOST RECENT
//     `*`. Once a later `*` is reached, the literal segment between the earlier
//     `*` and the later `*` has already matched as far left as possible; any
//     extra characters that could be absorbed by expanding the earlier `*` can
//     just as well be absorbed by the later `*`.
//   - After `s` is exhausted, skip any remaining `*` characters in `p`; the
//     match succeeds iff `pi == p.size()`.
//
//   Time Complexity:  O(m + n) on typical inputs, O(m * n) worst case (where
//                     `m = s.size()`, `n = p.size()`), and O(min(m, n)) best.
//   Space Complexity: O(1) — only four scalar index variables, no DP table or
//                     recursion stack.
//
// Data Structures:
//   - std::string_view (`s`, `p`): Zero-copy views of input and pattern.
//   - Four `std::size_t` indices (`si`, `pi`, `star_pi`, `star_match_si`)
//     encoding the entire matching state.
//
// Real-World Applications:
//   - Shell filename globbing (`*.cpp`, `build/?_test.*`) and `.gitignore` /
//     build-system file matching.
//   - SQL `LIKE` pattern evaluation (`%` / `_` wildcards) inside database query
//     executors.
//   - Route/URL access-control lists, firewall rules, and log-filtering engines
//     requiring linear/bounded-memory wildcard matching without ReDoS risk.
//
// Related LeetCode Problems:
//   - #10 Regular Expression Matching [done] ('.' and preceding-char '*' requiring DP)
//   - #72 Edit Distance [done] (two-string sequence alignment via DP)
//   - #1143 Longest Common Subsequence [done] (classic two-string DP)
//   - #28 Find the Index of the First Occurrence in a String [done] (exact substring matching)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>

namespace {

class Solution {
 public:
  // Returns true iff wildcard pattern `p` matches the entire string `s`.
  //
  // Invariant: Whenever `star_pi != npos`, `p[0..star_pi]` matches
  // `s[0..star_match_si - 1]` (or empty when `star_match_si == 0`), and we are
  // attempting to match `p[star_pi + 1..]` against `s[star_match_si..]`.
  // Time: O(m * n) worst-case (O(m + n) average), Space: O(1).
  bool isMatch(std::string_view s, std::string_view p) {
    std::size_t si = 0;
    std::size_t pi = 0;
    std::size_t star_pi = std::string_view::npos;
    std::size_t star_match_si = 0;

    while (si < s.size()) {
      if (pi < p.size() && (p[pi] == '?' || p[pi] == s[si])) {
        // Exact character match or single-character '?' wildcard.
        ++si;
        ++pi;
      } else if (pi < p.size() && p[pi] == '*') {
        // Checkpoint this '*' and initially match zero characters of `s`.
        // Earlier '*' checkpoints are superseded and safely discarded.
        star_pi = pi;
        star_match_si = si;
        ++pi;
      } else if (star_pi != std::string_view::npos) {
        // Mismatch: backtrack to right after the latest '*' and expand its
        // match by one additional character of `s`.
        ++star_match_si;
        si = star_match_si;
        pi = star_pi + 1;
      } else {
        // Mismatch with no active '*' to absorb characters.
        return false;
      }
    }

    // Any trailing '*' wildcards in `p` can match the empty suffix.
    while (pi < p.size() && p[pi] == '*') {
      ++pi;
    }

    return pi == p.size();
  }
};

LC_PROBLEM(44, "Wildcard Matching", "greedy", ctx) {
  ctx.test("docstring_examples", [&] {
    Solution sol;
    ctx.expect_false(sol.isMatch("aa", "a"));
    ctx.expect_true(sol.isMatch("aa", "aa"));
    ctx.expect_false(sol.isMatch("aaa", "aa"));
    ctx.expect_true(sol.isMatch("aa", "*"));
    ctx.expect_true(sol.isMatch("aa", "a*"));
    ctx.expect_true(sol.isMatch("ab", "?*"));
    ctx.expect_false(sol.isMatch("aab", "c*a*b"));
  });

  ctx.test("python_build_and_multi_star_backtracking", [&] {
    Solution sol;
    ctx.expect_false(sol.isMatch("acdcb", "a*c?b"));
    ctx.expect_true(sol.isMatch("abefcdgiescdfimde", "ab*cd?i*de"));
    ctx.expect_true(sol.isMatch("adceb", "*a*b"));
  });

  ctx.test("empty_string_and_pattern_edge_cases", [&] {
    Solution sol;
    ctx.expect_true(sol.isMatch("", ""));
    ctx.expect_true(sol.isMatch("", "*"));
    ctx.expect_true(sol.isMatch("", "***"));
    ctx.expect_false(sol.isMatch("", "?"));
    ctx.expect_false(sol.isMatch("a", ""));
  });

  ctx.test("adversarial_backtracking_no_exponential_blowup", [&] {
    Solution sol;
    // Naive unmemoized recursion hangs on repeated '*' segments ending in a
    // mismatch; the greedy two-pointer algorithm finishes in O(m * n).
    const std::string s(100uz, 'a');
    ctx.expect_false(sol.isMatch(s + "b", "*a*a*a*a*a*a*a*a*c"));
    ctx.expect_true(sol.isMatch(s + "b", "*a*a*a*a*a*a*a*a*b"));
  });
}

}  // namespace
