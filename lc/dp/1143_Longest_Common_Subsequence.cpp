// LeetCode 1143: Longest Common Subsequence
// https://leetcode.com/problems/longest-common-subsequence/
//
// Problem:
//   Given two lowercase English strings `text1` and `text2` (each of length
//   1..1000), return the length of their longest common subsequence (LCS).
//   A subsequence is formed by deleting zero or more characters without
//   reordering the remaining characters; return 0 if no common subsequence
//   exists.
//
// Algorithm:
//   Bottom-up 2D dynamic programming over prefixes of both strings. Let
//   `dp[i][j]` be the LCS length of `text1[0..i-1]` and `text2[0..j-1]`, with
//   base row `dp[0][*]` = 0 and base column `dp[*][0]` = 0 for empty prefixes.
//   For each pair `(i, j)`:
//     - If `text1[i] == text2[j]`, both characters extend the optimal LCS of
//       the preceding prefixes: `dp[i + 1][j + 1] = 1 + dp[i][j]`.
//     - Otherwise, at least one of the two trailing characters cannot appear in
//       an optimal common subsequence, so we take the better of skipping either
//       one: `dp[i + 1][j + 1] = max(dp[i][j + 1], dp[i + 1][j])`.
//   Time Complexity:  O(m * n), where `m = text1.size()` and `n = text2.size()`.
//   Space Complexity: O(m * n) for the full DP table.
//
// Data Structures:
//   - 2D DP table (`std::vector<std::vector<int>>` of size `(m + 1) x (n + 1)`)
//     storing optimal LCS lengths for all prefix pairs, with row 0 and column 0
//     acting as zero-initialized sentinel boundaries.
//
// Real-World Applications:
//   - Version-control diff utilities (`git diff`, Myers' diff) comparing lines
//     across file revisions to compute minimal edit scripts.
//   - Bioinformatics global sequence alignment (DNA, RNA, and protein homology).
//   - Plagiarism detection and document revision synchronization.
//
// Related LeetCode Problems:
//   - #72 Edit Distance [done] (2D prefix DP extending LCS with replace/insert/delete)
//   - #516 Longest Palindromic Subsequence [done] (LCS of a string with its reverse)
//   - #583 Delete Operation for Two Strings [done] (minimum deletions equals m + n - 2 * LCS)
//   - #1092 Shortest Common Supersequence [new] (reconstructs merged string from LCS table)
//   - #1216 Valid Palindrome III [done] (palindromic subsequence DP for k-deletion check)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the length of the longest common subsequence between `text1` and
  // `text2` using bottom-up 2D dynamic programming.
  // Time: O(m * n), Space: O(m * n).
  int longestCommonSubsequence(std::string_view text1, std::string_view text2) {
    const std::size_t m = text1.size();
    const std::size_t n = text2.size();

    // `dp[i][j]` holds the LCS length of prefixes `text1[0..i)` and
    // `text2[0..j)`. Row 0 and column 0 remain 0 because an empty prefix shares
    // no characters with any string.
    std::vector<std::vector<int>> dp(m + 1uz, std::vector<int>(n + 1uz, 0));

    for (std::size_t i = 0uz; i < m; ++i) {
      for (std::size_t j = 0uz; j < n; ++j) {
        if (text1[i] == text2[j]) {
          // Matching characters always contribute +1 to the best alignment of
          // strictly shorter prefixes.
          dp[i + 1uz][j + 1uz] = dp[i][j] + 1;
        } else {
          // Mismatched characters cannot both be matched at this position; drop
          // whichever trailing character yields the longer common subsequence.
          dp[i + 1uz][j + 1uz] = std::max(dp[i][j + 1uz], dp[i + 1uz][j]);
        }
      }
    }

    return dp[m][n];
  }
};

LC_PROBLEM(1143, "Longest Common Subsequence", "dp", ctx) {
  Solution sol;

  ctx.test("example_1_subsequence_match", [&] {
    ctx.expect_eq(sol.longestCommonSubsequence("abcde", "ace"), 3);
  });

  ctx.test("example_2_identical_strings", [&] {
    ctx.expect_eq(sol.longestCommonSubsequence("abc", "abc"), 3);
  });

  ctx.test("example_3_disjoint_strings", [&] {
    ctx.expect_eq(sol.longestCommonSubsequence("abc", "def"), 0);
  });

  ctx.test("python_build_case", [&] {
    ctx.expect_eq(sol.longestCommonSubsequence("vozsh", "psnw"), 1);
  });

  ctx.test("interleaved_subsequence_not_substring", [&] {
    ctx.expect_eq(sol.longestCommonSubsequence("abg", "adbeh"), 2);
  });

  ctx.test("edge_cases_single_and_repeated_chars", [&] {
    ctx.expect_eq(sol.longestCommonSubsequence("a", "a"), 1);
    ctx.expect_eq(sol.longestCommonSubsequence("a", "b"), 0);
    ctx.expect_eq(sol.longestCommonSubsequence("aaaa", "aa"), 2);
    ctx.expect_eq(sol.longestCommonSubsequence("abc", "cba"), 1);
    ctx.expect_eq(sol.longestCommonSubsequence("", "abc"), 0);
  });
}

}  // namespace
