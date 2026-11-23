// LeetCode 5: Longest Palindromic Substring
// https://leetcode.com/problems/longest-palindromic-substring/
//
// Problem:
//   Given a string `s`, return the longest contiguous substring of `s` that
//   reads the same forward and backward. If multiple palindromic substrings
//   share the maximum length, any valid one may be returned.
//
// Examples:
//   Example 1:
//     Input:  s = "babad"
//     Output: "bab"
//     Explanation:
//       - Initialize best_start = 0 and best_len = 1, then iterate center from 0 to
//         4 checking odd (center, center) and even (center, center + 1) expansions.
//       - At center = 0, odd expansion gives "b" (length 1) and even (0, 1) stops
//         immediately because 'b' != 'a'.
//       - At center = 1, odd expansion matches s[0] == s[2] == 'b' to form "bab"
//         (start = 0, length = 3), updating best_start = 0 and best_len = 3.
//       - At center = 2, odd expansion finds "aba" (length 3, not strictly greater
//         than best_len = 3), and centers 3 and 4 yield length 1, so s.substr(0, 3)
//         returns "bab".
//
//   Example 2:
//     Input:  s = "cbbd"
//     Output: "bb"
//     Explanation:
//       - Initialize best_start = 0 and best_len = 1 for s.size() = 4.
//       - At center = 0 ('c'), odd expansion gives length 1 and even (0, 1) returns
//         length 0 since 'c' != 'b'.
//       - At center = 1 ('b'), even expansion (1, 2) sees s[1] == s[2] == 'b',
//         stops because s[0] ('c') != s[3] ('d'), and updates best_start = 1 and
//         best_len = 2.
//       - Centers 2 and 3 produce lengths at most 1, so s.substr(1, 2) returns
//         "bb".
//
//   Example 3:
//     Input:  s = "aaaa"
//     Output: "aaaa"
//     Explanation:
//       - At center = 0, odd (0, 0) gives length 1 and even (0, 1) expands to [0,
//         1] ("aa"), updating best_start = 0 and best_len = 2.
//       - At center = 1, odd (1, 1) expands to [0, 2] ("aaa", best_len = 3), then
//         even (1, 2) expands to [0, 3] ("aaaa"), updating best_start = 0 and
//         best_len = 4.
//       - Centers 2 and 3 yield lengths at most 3, so s.substr(0, 4) returns
//         "aaaa".
//
// Algorithm:
//   Expand around center. Every palindrome is symmetric around either a
//   single character (odd length, center `(i, i)`) or the gap between two
//   adjacent equal characters (even length, center `(i, i + 1)`). Iterate
//   through each index `i` in `s`, expand outward with two pointers `left`
//   and `right` while `s[left] == s[right]` within bounds, and track only
//   the starting index and length of the longest palindrome found. Construct
//   the resulting `std::string` once at the end to avoid repeated substring
//   allocations inside the inner loop.
//   Time Complexity:  O(n^2) in the worst case (e.g., all identical chars),
//                     where n = s.size().
//   Space Complexity: O(1) auxiliary space (excluding the returned string).
//
// Data Structures:
//   - Two `std::size_t` pointers (`left`, `right`): expand outward from each
//     candidate center without heap allocations.
//   - Best-span pair (`best_start`, `best_len`): records the winning slice in
//     O(1) space until final string materialization.
//
// Real-World Applications:
//   - Bioinformatics (detecting palindromic restriction enzyme recognition
//     sites and hairpin stem-loop structures in DNA/RNA sequences).
//   - String processing and text compression (identifying symmetric patterns
//     and self-reversing motifs in large logs or corpora).
//   - Signal and sequence symmetry analysis in anomaly detection.
//
// Related LeetCode Problems:
//   - #647 Palindromic Substrings [done] (counts all palindromic substrings via the same center expansion)
//   - #516 Longest Palindromic Subsequence [done] (non-contiguous DP variant)
//   - #214 Shortest Palindrome [done] (longest palindromic prefix using KMP)
//   - #125 Valid Palindrome [done] (inward two-pointer palindrome verification)
//   - #680 Valid Palindrome II [done] (two-pointer palindrome check with at most one deletion)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <utility>

namespace {

class Solution {
 public:
  // Finds and returns a longest palindromic substring of `s` by expanding
  // around all `2n - 1` possible odd and even centers.
  //
  // Time: O(n^2), Space: O(1) auxiliary.
  std::string longestPalindrome(std::string_view s) {
    if (s.size() < 2uz) {
      return std::string(s);
    }

    std::size_t best_start = 0uz;
    std::size_t best_len = 1uz;

    for (std::size_t center = 0uz; center < s.size(); ++center) {
      // Odd-length palindrome centered at `center`.
      const auto [odd_start, odd_len] = expandAroundCenter(s, center, center);
      if (odd_len > best_len) {
        best_start = odd_start;
        best_len = odd_len;
      }

      // Even-length palindrome centered between `center` and `center + 1`.
      const auto [even_start, even_len] =
          expandAroundCenter(s, center, center + 1uz);
      if (even_len > best_len) {
        best_start = even_start;
        best_len = even_len;
      }
    }

    return std::string(s.substr(best_start, best_len));
  }

 private:
  // Expands symmetrically from initial indices `[left, right]` as long as the
  // boundary characters match. Returns `{start_index, length}` of the maximal
  // palindrome around this center, or `{left, 0}` if the seed itself is invalid.
  static std::pair<std::size_t, std::size_t> expandAroundCenter(
      std::string_view s, std::size_t left, std::size_t right) {
    if (right >= s.size() || s[left] != s[right]) {
      return {left, 0uz};
    }

    // Check bounds before decrementing `left` to prevent unsigned underflow.
    while (left > 0uz && right + 1uz < s.size() &&
           s[left - 1uz] == s[right + 1uz]) {
      --left;
      ++right;
    }

    return {left, right - left + 1uz};
  }
};

LC_PROBLEM(5, "Longest Palindromic Substring", "two_pointers", ctx) {
  Solution sol;

  ctx.test("example_babad", [&] {
    const std::string res = sol.longestPalindrome("babad");
    ctx.expect_true(res == "bab" || res == "aba",
                    "expected 'bab' or 'aba' for 'babad'");
  });

  ctx.test("example_cbbd", [&] {
    ctx.expect_eq(sol.longestPalindrome("cbbd"), std::string("bb"));
  });

  ctx.test("single_and_empty", [&] {
    ctx.expect_eq(sol.longestPalindrome("a"), std::string("a"));
    ctx.expect_eq(sol.longestPalindrome(""), std::string(""));
  });

  ctx.test("all_same_chars", [&] {
    ctx.expect_eq(sol.longestPalindrome("aaaa"), std::string("aaaa"));
  });

  ctx.test("prefix_and_full_palindrome", [&] {
    ctx.expect_eq(sol.longestPalindrome("bbbad"), std::string("bbb"));
    ctx.expect_eq(sol.longestPalindrome("racecar"), std::string("racecar"));
  });

  ctx.test("no_multi_char_palindrome", [&] {
    const std::string res = sol.longestPalindrome("ac");
    ctx.expect_eq(res.size(), 1uz);
    ctx.expect_true(res == "a" || res == "c", "expected 'a' or 'c'");
  });
}

}  // namespace
