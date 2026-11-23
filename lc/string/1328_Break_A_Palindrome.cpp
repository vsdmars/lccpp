// LeetCode 1328: Break A Palindrome
// https://leetcode.com/problems/break-a-palindrome/
//
// Problem:
//   Given a palindromic string `palindrome` of lowercase English letters
//   (1 <= palindrome.length <= 1000), replace exactly one character with any
//   lowercase English letter so that the resulting string is NOT a palindrome
//   and is lexicographically smallest among all valid results. Return "" if
//   no single-character replacement can break the palindrome.
//
// Algorithm:
//   1. If `palindrome.size() <= 1`, any single-character string remains a
//      palindrome after replacement, so return "".
//   2. Greedy pass over the first half `[0, n / 2)`:
//      - Changing the exact middle character of an odd-length palindrome never
//        breaks symmetry, so indices `>= n / 2` are intentionally excluded
//        from this pass.
//      - At the earliest index `i < n / 2` with `palindrome[i] != 'a'`,
//        replacing `palindrome[i]` with `'a'` strictly decreases the string at
//        the leftmost possible position to the smallest possible character,
//        while its mirror `palindrome[n - 1 - i]` remains non-'a', breaking
//        the palindrome optimally.
//   3. Fallback when every character in the first half (and by symmetry, the
//      second half) is `'a'` (e.g., "aaaa" or "aabaa"): no character can be
//      decreased to break symmetry, so we must increase some character. To
//      minimize the lexicographical increase, change the rightmost character
//      `result.back()` to `'b'`.
//   Time Complexity:  O(N), where N = palindrome.size().
//   Space Complexity: O(N) for the returned string; O(1) auxiliary space.
//
// Data Structures:
//   - std::string: mutable copy of the input modified in place at a single
//     index.
//
// Real-World Applications:
//   - Lexicographically minimal perturbation and symmetry-breaking heuristics
//     in constraint solvers and fuzz-test input mutators.
//   - Generating minimal counterexamples for string-property verification.
//
// Related LeetCode Problems:
//   - #125 Valid Palindrome [done] (two-pointer palindrome check)
//   - #680 Valid Palindrome II [done] (palindrome check with at most one deletion)
//   - #2697 Lexicographically Smallest Palindrome [new] (greedy palindrome construction)
//   - #564 Find the Closest Palindrome [done] (minimal numeric palindrome perturbation)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>

namespace {

class Solution {
 public:
  // Replaces exactly one character in `palindrome` to produce the
  // lexicographically smallest non-palindromic string, or "" if impossible.
  //
  // Time Complexity:  O(N)
  // Space Complexity: O(N) for output, O(1) auxiliary
  std::string breakPalindrome(std::string_view palindrome) {
    const std::size_t n = palindrome.size();
    // Every 0- or 1-character string is trivially a palindrome regardless of
    // which letter is substituted.
    if (n <= 1uz) {
      return {};
    }

    std::string result(palindrome);
    const std::size_t half = n / 2uz;

    // Scan strictly the first half (skipping the center index on odd lengths,
    // whose modification preserves symmetry) for the first non-'a' character.
    for (std::size_t i = 0uz; i < half; ++i) {
      if (result[i] != 'a') {
        result[i] = 'a';
        return result;
      }
    }

    // All non-center characters are 'a'; bump the last character to 'b' for
    // the smallest possible lexicographical increase.
    result.back() = 'b';
    return result;
  }
};

LC_PROBLEM(1328, "Break A Palindrome", "string", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    ctx.expect_eq(sol.breakPalindrome("abccba"), std::string("aaccba"));
  });

  ctx.test("example_2_single_char", [&] {
    Solution sol;
    ctx.expect_eq(sol.breakPalindrome("a"), std::string{});
  });

  ctx.test("build_case_all_a", [&] {
    Solution sol;
    ctx.expect_eq(sol.breakPalindrome("aaaa"), std::string("aaab"));
  });

  ctx.test("odd_length_center_non_a", [&] {
    Solution sol;
    // Center 'b' must be skipped; only replacing the trailing 'a' breaks it.
    ctx.expect_eq(sol.breakPalindrome("aabaa"), std::string("aabab"));
  });

  ctx.test("odd_length_first_half_non_a", [&] {
    Solution sol;
    ctx.expect_eq(sol.breakPalindrome("abcba"), std::string("aacba"));
  });

  ctx.test("length_two", [&] {
    Solution sol;
    ctx.expect_eq(sol.breakPalindrome("aa"), std::string("ab"));
    ctx.expect_eq(sol.breakPalindrome("bb"), std::string("ab"));
  });
}

}  // namespace
