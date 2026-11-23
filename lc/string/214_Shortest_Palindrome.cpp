// LeetCode 214: Shortest Palindrome
// https://leetcode.com/problems/shortest-palindrome/
//
// Problem:
//   Given a string `s` consisting of lowercase English letters, find and return
//   the shortest palindrome that can be formed by prepending zero or more
//   characters in front of `s`.
//
// Examples:
//   Example 1:
//     Input:  s = "aacecaaa"
//     Output: "aaacecaaa"
//     Explanation:
//       - Build rev = "aaacecaa" and concatenate s + '#' + rev to form combined =
//         "aacecaaa#aaacecaa" of length 17.
//       - Compute the KMP prefix function lps across combined, ending at
//         pal_prefix_len = lps.back() = 7 because the prefix "aacecaa" matches the
//         last 7 characters of rev.
//       - Compute prepend_len = s.size() - pal_prefix_len = 8 - 7 = 1 and prepend
//         rev[0..1) = "a" to s, returning "aaacecaaa".
//
//   Example 2:
//     Input:  s = "abcd"
//     Output: "dcbabcd"
//     Explanation:
//       - Build rev = "dcba" and form combined = "abcd#dcba" of length 9.
//       - Compute the KMP table lps = [0, 0, 0, 0, 0, 0, 0, 0, 1], giving
//         pal_prefix_len = lps.back() = 1 (only the single-character prefix "a"
//         matches).
//       - Compute prepend_len = 4 - 1 = 3 and prepend rev[0..3) = "dcb" to "abcd",
//         returning "dcbabcd".
//
//   Example 3:
//     Input:  s = "abbacd"
//     Output: "dcabbacd"
//     Explanation:
//       - Build rev = "dcabba" and form combined = "abbacd#dcabba" of length 13.
//       - Computing lps tracks matches at the trailing suffix "abba" (indices 9
//         through 12 incrementing matched from 1 to 4), yielding pal_prefix_len =
//         lps.back() = 4.
//       - Compute prepend_len = 6 - 4 = 2 and prepend rev[0..2) = "dc" to "abbacd",
//         returning "dcabbacd".
//
// Algorithm:
//   Finding the shortest string to prepend to `s` is equivalent to finding the
//   longest palindromic prefix of `s`: once the longest prefix `s[0..L-1]` that
//   is already a palindrome is known, only the remaining suffix `s[L..n-1]`
//   needs to be reversed and prepended in front of `s`.
//
//   1. KMP Prefix Function (primary, optimal):
//      Form `combined = s + '#' + reverse(s)`, where `'#'` is a sentinel
//      character guaranteed not to occur in `s`. Compute the Knuth-Morris-Pratt
//      longest proper prefix-suffix (LPS) table on `combined`. The last entry
//      `lps.back()` gives the longest prefix of `s` that matches a suffix of
//      `reverse(s)` -- which is identically the longest palindromic prefix of
//      `s`. Prepending the unmatched prefix of `reverse(s)` yields the shortest
//      palindrome.
//      Time Complexity:  O(n)
//      Space Complexity: O(n)
//
//   2. Reverse-Prefix Scan (secondary, direct from original Python):
//      Reverse `s` into `rev` and find the smallest offset `idx` such that `s`
//      starts with `rev[idx..]`. Then `rev[0..idx-1] + s` is the shortest
//      palindrome.
//      Time Complexity:  O(n^2) worst case
//      Space Complexity: O(n)
//
// Data Structures:
//   - `std::string`: stores the reversed copy and sentinel-joined KMP pattern.
//   - `std::vector<std::size_t>`: KMP prefix-function (LPS) failure table.
//   - `std::string_view`: zero-copy prefix/suffix slicing for the scan variant.
//
// Real-World Applications:
//   - DNA/RNA sequence analysis for detecting palindromic restriction sites and
//     minimal flank completions in bioinformatics.
//   - Pattern matching and streaming synchronization (KMP failure automata in
//     intrusion-detection engines, log scanners, and text editors).
//   - Error-resilient frame alignment where minimal header padding restores
//     symmetric checksum or framing invariants.
//
// Related LeetCode Problems:
//   - #5 Longest Palindromic Substring [done] (palindromic structure via expansion or Manacher's algorithm)
//   - #28 Find the Index of the First Occurrence in a String [done] (core KMP prefix-function string matching)
//   - #125 Valid Palindrome [done] (fundamental palindrome verification with two pointers)
//   - #336 Palindrome Pairs [done] (matching reversed prefixes/suffixes across multiple words)
//   - #459 Repeated Substring Pattern [new] (another classic KMP LPS table property)
//   - #1392 Longest Happy Prefix [new] (directly returns the final KMP LPS table value)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the shortest palindrome formed by prepending characters to `s`
  // using the KMP failure function on `s + '#' + reverse(s)`.
  // Invariant: after computing `lps`, `lps[i]` is the length of the longest
  // proper prefix of `combined[0..i]` that is also a suffix of `combined[0..i]`.
  // Time: O(n), Space: O(n).
  std::string shortestPalindrome(std::string_view s) {
    if (s.size() < 2uz) {
      return std::string(s);
    }

    const std::string rev(s.rbegin(), s.rend());

    // Build `s + '#' + rev`. The sentinel '#' prevents any prefix-suffix match
    // from spanning beyond the length of `s` (valid because `s` only contains
    // lowercase English letters).
    std::string combined;
    combined.reserve((s.size() * 2uz) + 1uz);
    combined.append(s);
    combined.push_back('#');
    combined.append(rev);

    // Compute the KMP prefix function (LPS array) over `combined`.
    std::vector<std::size_t> lps(combined.size(), 0uz);
    for (std::size_t i = 1uz; i < combined.size(); ++i) {
      std::size_t matched = lps[i - 1uz];
      // Fall back along shorter border lengths until characters match or we
      // reach length 0.
      while (matched > 0uz && combined[i] != combined[matched]) {
        matched = lps[matched - 1uz];
      }
      if (combined[i] == combined[matched]) {
        ++matched;
      }
      lps[i] = matched;
    }

    // `pal_prefix_len` is the longest palindromic prefix of `s`; prepend the
    // reversed unmatched suffix `rev[0 .. s.size() - pal_prefix_len)`.
    const std::size_t pal_prefix_len = lps.back();
    const std::size_t prepend_len = s.size() - pal_prefix_len;

    std::string result;
    result.reserve(prepend_len + s.size());
    result.append(rev, 0uz, prepend_len);
    result.append(s);
    return result;
  }

  // Direct reverse-prefix scan matching the original Python solution: finds the
  // smallest `idx` such that `s` starts with `reverse(s)[idx..]`.
  // Time: O(n^2) worst case, Space: O(n).
  std::string shortestPalindromeReverseScan(std::string_view s) {
    if (s.size() < 2uz) {
      return std::string(s);
    }

    const std::string rev(s.rbegin(), s.rend());
    const std::string_view rev_view(rev);

    // The first matching `idx` minimizes the number of prepended characters.
    for (std::size_t idx = 0uz; idx < rev_view.size(); ++idx) {
      if (s.starts_with(rev_view.substr(idx))) {
        std::string result;
        result.reserve(idx + s.size());
        result.append(rev_view.substr(0uz, idx));
        result.append(s);
        return result;
      }
    }
    return std::string(s);
  }
};

LC_PROBLEM(214, "Shortest Palindrome", "string", ctx) {
  Solution sol;

  auto check_both = [&](std::string_view input, std::string_view expected) {
    ctx.expect_eq(sol.shortestPalindrome(input), expected);
    ctx.expect_eq(sol.shortestPalindromeReverseScan(input), expected);
  };

  ctx.test("example_1_aacecaaa", [&] { check_both("aacecaaa", "aaacecaaa"); });

  ctx.test("example_2_abcd", [&] { check_both("abcd", "dcbabcd"); });

  ctx.test("docstring_acbba", [&] { check_both("acbba", "abbcacbba"); });

  ctx.test("empty_and_single_char", [&] {
    check_both("", "");
    check_both("a", "a");
  });

  ctx.test("already_palindrome", [&] {
    check_both("aba", "aba");
    check_both("abba", "abba");
    check_both("aaaaa", "aaaaa");
  });

  ctx.test("partial_palindromic_prefix", [&] {
    check_both("abbacd", "dcabbacd");
    check_both("abac", "cabac");
  });
}

}  // namespace
