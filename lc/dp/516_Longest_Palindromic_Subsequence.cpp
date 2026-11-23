// LeetCode 516: Longest Palindromic Subsequence
// https://leetcode.com/problems/longest-palindromic-subsequence/
//
// Problem:
//   Given a string `s`, return the length of its longest palindromic
//   subsequence. A subsequence is formed by deleting zero or more characters
//   from `s` without changing the relative order of the remaining characters.
//   Constraints: 1 <= s.size() <= 1000, `s` consists of lowercase English
//   letters.
//
// Examples:
//   Example 1:
//     Input:  s = "bbbab"
//     Output: 4
//     Explanation:
//       - Initialize dp = [1, 1, 1, 1, 1] since every single character is a
//         length-1 palindrome.
//       - At j = 1 ('b'), s[0] == s[1] with prev_diag = 0 updates dp[0] = 2 + 0 =
//         2, giving dp = [2, 1, 1, 1, 1].
//       - At j = 2 ('b'), sweeping i = 1 down to 0 matches 'b' at both indices and
//         updates dp to [3, 2, 1, 1, 1].
//       - At j = 3 ('a'), no left character matches 'a', so max(dp[i], dp[i + 1])
//         leaves dp = [3, 2, 1, 1, 1].
//       - At j = 4 ('b'), i = 3 gives dp[3] = 1 (setting prev_diag = 1), and then i
//         = 2, 1, 0 match 'b' with prev_diag values 1, 1, 2 to yield dp = [4, 3, 3,
//         1, 1]; return dp[0] = 4.
//
//   Example 2:
//     Input:  s = "cbbd"
//     Output: 2
//     Explanation:
//       - Initialize dp = [1, 1, 1, 1] for length n = 4.
//       - At j = 1 ('b'), s[0] = 'c' != 'b' keeps dp[0] = max(1, 1) = 1.
//       - At j = 2 ('b'), i = 1 matches 'b' to set dp[1] = 2 + 0 = 2 (prev_diag
//         becomes 1), and i = 0 mismatches so dp[0] = max(1, 2) = 2.
//       - At j = 3 ('d'), all i = 2, 1, 0 mismatch 'd', preserving dp = [2, 2, 1,
//         1], and return dp[0] = 2.
//
//   Example 3:
//     Input:  s = "abcdef"
//     Output: 1
//     Explanation:
//       - Initialize dp = [1, 1, 1, 1, 1, 1] for the 6-character string.
//       - Across every j from 1 to 5 and i from j - 1 down to 0, s[i] != s[j] holds
//         because all characters are distinct.
//       - Every update takes max(dp[i], dp[i + 1]) = 1, leaving dp[0] = 1 as the
//         final answer.
//
// Algorithm:
//   Use space-optimized bottom-up interval dynamic programming over expanding
//   right endpoints `j` from `1` to `n - 1`.
//   In the full 2D recurrence, `dp[i][j]` is the LPS length in `s[i..j]`:
//     - `dp[i][i] = 1`
//     - If `s[i] == s[j]`: `dp[i][j] = 2 + dp[i + 1][j - 1]`
//       (where empty interval `i + 1 > j - 1` contributes 0)
//     - Otherwise: `dp[i][j] = max(dp[i + 1][j], dp[i][j - 1])`
//   Compressing along `j` into a single 1D array `dp[0..n-1]`:
//     - Before processing `j`, `dp[i]` holds `dp[i][j - 1]`.
//     - Sweep `i` downwards from `j - 1` to `0` while carrying `prev_diag`
//       (initialized to `0`), which holds the pre-update value of `dp[i + 1]`
//       (i.e. `dp[i + 1][j - 1]`).
//     - Because `i` moves right-to-left, `dp[i + 1]` already contains the
//       updated `dp[i + 1][j]`, and `dp[i]` still contains `dp[i][j - 1]`.
//   After all `j`, `dp[0]` equals the LPS length of the entire string `s`.
//   Time Complexity:  O(n^2).
//   Space Complexity: O(n) for the compressed 1D DP array.
//
// Data Structures:
//   - 1D compressed interval DP vector (`std::vector<int>` of size `n`) plus a
//     single scalar `prev_diag` holding the overwritten diagonal entry
//     `dp[i + 1][j - 1]`.
//
// Real-World Applications:
//   - Bioinformatics RNA secondary-structure prediction (identifying maximal
//     complementary stem-loop base pairings, analogous to palindromic matches).
//   - Genomic sequence symmetry detection and structural motif discovery.
//   - Minimum-deletion palindrome repair in text normalization and diffing
//     (`n - LPS(s)` equals the minimum deletions/insertions to make `s` a
//     palindrome).
//
// Related LeetCode Problems:
//   - #5 Longest Palindromic Substring [done] (contiguous palindrome variant)
//   - #1143 Longest Common Subsequence [done] (LPS(s) == LCS(s, reverse(s)))
//   - #1312 Minimum Insertion Steps to Make a String Palindrome [new] (directly equals n - LPS(s))
//   - #647 Palindromic Substrings [done] (counting palindromic intervals)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the length of the longest palindromic subsequence of `s` using
  // 1D space-optimized bottom-up interval DP.
  // Time: O(n^2), Space: O(n).
  int longestPalindromeSubseq(std::string_view s) {
    const std::size_t n = s.size();
    if (n == 0uz) {
      return 0;
    }

    // Every single character forms a palindrome of length 1.
    // After finishing outer iteration `j`, `dp[i]` stores LPS(s[i..j]) for all
    // `0 <= i <= j`.
    std::vector<int> dp(n, 1);

    for (std::size_t j = 1uz; j < n; ++j) {
      // `prev_diag` tracks the pre-update `dp[i + 1]`, which corresponds to
      // `dp_2d[i + 1][j - 1]`. At `i = j - 1`, the inner substring is empty,
      // so `prev_diag` starts at 0.
      int prev_diag = 0;

      for (std::size_t k = j; k > 0uz; --k) {
        const std::size_t i = k - 1uz;
        // Save `dp[i][j - 1]` before overwriting so the next iteration (`i - 1`)
        // receives it as its diagonal `dp[i][j - 1]`.
        const int saved = dp[i];

        if (s[i] == s[j]) {
          dp[i] = 2 + prev_diag;
        } else {
          dp[i] = std::max(dp[i], dp[i + 1uz]);
        }

        prev_diag = saved;
      }
    }

    return dp[0uz];
  }
};

LC_PROBLEM(516, "Longest Palindromic Subsequence", "dp", ctx) {
  ctx.test("example_1_bbbab", [&] {
    Solution sol;
    ctx.expect_eq(sol.longestPalindromeSubseq("bbbab"), 4);
  });

  ctx.test("example_2_cbbd", [&] {
    Solution sol;
    ctx.expect_eq(sol.longestPalindromeSubseq("cbbd"), 2);
  });

  ctx.test("all_identical_characters", [&] {
    Solution sol;
    ctx.expect_eq(sol.longestPalindromeSubseq("bbbbb"), 5);
  });

  ctx.test("single_and_empty_string", [&] {
    Solution sol;
    ctx.expect_eq(sol.longestPalindromeSubseq("a"), 1);
    ctx.expect_eq(sol.longestPalindromeSubseq(""), 0);
  });

  ctx.test("all_distinct_characters", [&] {
    Solution sol;
    ctx.expect_eq(sol.longestPalindromeSubseq("abcdef"), 1);
  });

  ctx.test("full_palindrome", [&] {
    Solution sol;
    ctx.expect_eq(sol.longestPalindromeSubseq("racecar"), 7);
  });
}

}  // namespace
