// LeetCode 97: Interleaving String
// https://leetcode.com/problems/interleaving-string/
//
// Problem:
//   Given three strings s1, s2, and s3, determine whether s3 can be formed by
//   interleaving s1 and s2. An interleaving merges s1 and s2 while preserving
//   the relative order of characters within each string (alternating non-empty
//   substrings from s1 and s2 whose counts differ by at most 1).
//   Constraints: 0 <= s1.length, s2.length <= 100; 0 <= s3.length <= 200;
//   lowercase English letters only.
//
// Algorithm:
//   Bottom-up 2D Dynamic Programming over suffix indices (i, j):
//   - First guard: if s1.size() + s2.size() != s3.size(), return false.
//   - Define dp[i][j] = true iff s3[i + j..] can be formed by interleaving
//     s1[i..] and s2[j..].
//   - Base case: dp[m][n] = true (empty suffixes interleave to empty suffix).
//   - Fill the table backwards from (m, n) down to (0, 0). At state (i, j),
//     the next character to match in s3 is s3[i + j]:
//       * If i < m and s1[i] == s3[i + j], we can consume s1[i] and transition
//         to dp[i + 1][j].
//       * If j < n and s2[j] == s3[i + j], we can consume s2[j] and transition
//         to dp[i][j + 1].
//   - Answer is dp[0][0].
//   - Time Complexity: O(m * n), where m = s1.size() and n = s2.size().
//   - Space Complexity: O(m * n) for the 2D boolean table (can be reduced to
//     O(min(m, n)) with a single rolling row).
//
// Data Structures:
//   - 2D boolean DP table (std::vector<std::vector<bool>>) of dimensions
//     (m + 1) x (n + 1) storing reachability of suffix states.
//   - std::string_view parameters for zero-copy read-only string indexing.
//
// Real-World Applications:
//   - Verifying valid concurrent trace interleavings in multithreaded execution
//     checkers and distributed event-log verification.
//   - Stream merge validation in network packet reassembly and telemetry
//     pipelines where two ordered channels are multiplexed onto one stream.
//   - Bioinformatic sequence alignment and composite DNA/RNA sequence matching.
//
// Related LeetCode Problems:
//   - #115 Distinct Subsequences [done] (2D string DP over two string indices)
//   - #72 Edit Distance [done] (classic 2D DP on prefixes/suffixes of two strings)
//   - #1143 Longest Common Subsequence [done] (2D grid DP matching characters across two strings)
//   - #10 Regular Expression Matching [done] (2D suffix/prefix DP with branching transitions)
//   - #44 Wildcard Matching [done] (boolean 2D string-matching DP)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Determines whether `s3` is an order-preserving interleaving of `s1` and
  // `s2` using bottom-up suffix DP.
  //
  // Invariant: after processing cell (i, j), dp[i][j] is true iff s3[i + j..]
  // is a valid interleaving of s1[i..] and s2[j..].
  // Time: O(m * n), Space: O(m * n).
  bool isInterleave(std::string_view s1, std::string_view s2,
                    std::string_view s3) {
    const std::size_t m = s1.size();
    const std::size_t n = s2.size();

    // Quick length check: every character of s1 and s2 must be used exactly
    // once to build s3.
    if (m + n != s3.size()) {
      return false;
    }

    // dp[i][j] indicates whether s1[i..] and s2[j..] interleave to s3[i + j..].
    std::vector<std::vector<bool>> dp(m + 1, std::vector<bool>(n + 1, false));

    // Base case: two empty suffixes trivially interleave to the empty suffix.
    dp[m][n] = true;

    // Traverse backwards so that dp[i + 1][j] and dp[i][j + 1] are always
    // computed before dp[i][j]. Using `idx-- > 0` avoids unsigned underflow.
    for (std::size_t i = m + 1; i-- > 0;) {
      for (std::size_t j = n + 1; j-- > 0;) {
        // Option 1: match s3[i + j] with the next character of s1.
        if (i < m && s1[i] == s3[i + j] && dp[i + 1][j]) {
          dp[i][j] = true;
        }
        // Option 2: match s3[i + j] with the next character of s2.
        if (j < n && s2[j] == s3[i + j] && dp[i][j + 1]) {
          dp[i][j] = true;
        }
      }
    }

    return dp[0][0];
  }
};

LC_PROBLEM(97, "Interleaving String", "dp", ctx) {
  Solution sol;

  ctx.test("example_1_valid_interleaving", [&] {
    ctx.expect_true(sol.isInterleave("aabcc", "dbbca", "aadbbcbcac"));
  });

  ctx.test("example_2_invalid_interleaving", [&] {
    ctx.expect_false(sol.isInterleave("aabcc", "dbbca", "aadbbbaccc"));
  });

  ctx.test("python_build_case", [&] {
    ctx.expect_true(sol.isInterleave("abc", "def", "adefbc"));
  });

  ctx.test("all_empty_strings", [&] {
    ctx.expect_true(sol.isInterleave("", "", ""));
  });

  ctx.test("one_empty_string", [&] {
    ctx.expect_true(sol.isInterleave("", "abc", "abc"));
    ctx.expect_true(sol.isInterleave("abc", "", "abc"));
    ctx.expect_false(sol.isInterleave("abc", "", "abd"));
  });

  ctx.test("length_mismatch", [&] {
    ctx.expect_false(sol.isInterleave("a", "b", "abc"));
    ctx.expect_false(sol.isInterleave("ab", "cd", "abc"));
  });

  ctx.test("repeated_characters_ambiguous_choices", [&] {
    ctx.expect_true(sol.isInterleave("aa", "ab", "aaba"));
    ctx.expect_true(sol.isInterleave("aa", "ab", "abaa"));
    ctx.expect_false(sol.isInterleave("aa", "ab", "baaa"));
    ctx.expect_true(sol.isInterleave("aab", "aac", "aaacab"));
    ctx.expect_false(sol.isInterleave("aab", "aac", "aaacba"));
  });
}

}  // namespace
