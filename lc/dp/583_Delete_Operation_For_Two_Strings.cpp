// LeetCode 583: Delete Operation For Two Strings
// https://leetcode.com/problems/delete-operation-for-two-strings/
//
// Problem:
//   Given two strings `word1` and `word2`, return the minimum number of
//   single-character deletion steps required to make `word1` and `word2`
//   identical. In each step, you may delete one character from either string.
//
// Examples:
//   Example 1:
//     Input:  word1 = "sea", word2 = "eat"
//     Output: 2
//     Explanation:
//       - Initialize the rolling row dp = [0, 1, 2, 3] for matching "" against
//         prefixes of "eat".
//       - Row i = 1 ('s') matches no character in "eat", updating dp to [1, 2, 3,
//         4].
//       - Row i = 2 ('e') matches word2[0] = 'e' via prev_diag = 1, yielding dp =
//         [2, 1, 2, 3].
//       - Row i = 3 ('a') matches word2[1] = 'a' via prev_diag = 1, yielding dp =
//         [3, 2, 1, 2] and returning dp[3] = 2.
//
//   Example 2:
//     Input:  word1 = "leetcode", word2 = "etco"
//     Output: 4
//     Explanation:
//       - Initialize the rolling row dp = [0, 1, 2, 3, 4] for word2 = "etco" (n =
//         4).
//       - Across rows i = 1..6 ("leetco"), matching 'e', 't', 'c', 'o' in order
//         reduces dp[4] down to 2 (zero deletions needed beyond the two extra
//         prefix characters 'l' and 'e').
//       - Rows i = 7 ('d') and i = 8 ('e') increment the minimum deletion count
//         against "etco" from 2 to 4, ending at dp = [8, 7, 6, 5, 4] and returning
//         dp[4] = 4.
//
//   Example 3:
//     Input:  word1 = "abc", word2 = ""
//     Output: 3
//     Explanation:
//       - With n = 0, initialize dp = [0] of length 1.
//       - For each i from 1 to 3, the inner loop j = 1..0 does not run and dp[0] is
//         set to i, ending at dp[0] = 3.
//
// Algorithm:
//   Two equivalent bottom-up dynamic programming formulations:
//   1) Direct Deletion-Distance DP (primary, space-optimized to 1D):
//      Let dp[i][j] be the minimum deletions to equalize `word1[0..i)` and
//      `word2[0..j)`.
//        - Base cases: dp[i][0] = i, dp[0][j] = j (delete all characters of
//          the non-empty prefix).
//        - If word1[i - 1] == word2[j - 1], no deletion is needed for this
//          pair: dp[i][j] = dp[i - 1][j - 1].
//        - Otherwise, delete either word1[i - 1] or word2[j - 1]:
//          dp[i][j] = 1 + min(dp[i - 1][j], dp[i][j - 1]).
//      Because row `i` depends only on row `i - 1` and the left neighbor in
//      row `i`, we compress the table to a single row of size `word2.size() + 1`
//      while tracking `prev_diag` (= dp[i - 1][j - 1]).
//   2) Longest Common Subsequence (LCS) Reduction (secondary helper):
//      Keep the longest common subsequence of length L and delete every other
//      character: answer = word1.size() + word2.size() - 2 * L.
//   Time Complexity: O(m * n), where m = word1.size(), n = word2.size().
//   Space Complexity: O(n) for the rolling 1D DP row.
//
// Data Structures:
//   - 1D rolling DP vector (`std::vector<int>` of size n + 1): holds the
//     minimum deletion counts for the current prefix of `word1` against every
//     prefix of `word2`.
//
// Real-World Applications:
//   - Computing minimal diff scripts (`git diff`, Myers/LCS patch generators)
//     where edits are expressed as deletions and insertions.
//   - DNA/RNA sequence alignment and homology distance scoring in
//     bioinformatics.
//   - Plagiarism detection, fuzzy deduplication, and record-linkage similarity
//     metrics.
//
// Related LeetCode Problems:
//   - #72 Edit Distance [done] (generalizes deletions to also allow insertions and replacements)
//   - #1143 Longest Common Subsequence [done] (dual maximization formulation: m + n - 2 * LCS)
//   - #712 Minimum ASCII Delete Sum for Two Strings [new] (weighted character-deletion variant)
//   - #115 Distinct Subsequences [done] (counting subsequences via 2D prefix DP)
//   - #97 Interleaving String [done] (two-string prefix matching DP)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the minimum deletions to make `word1` and `word2` equal using
  // direct deletion-distance DP compressed to O(n) space.
  // Time: O(m * n), Space: O(n).
  int minDistance(std::string_view word1, std::string_view word2) {
    const std::size_t m = word1.size();
    const std::size_t n = word2.size();

    // Base row (i = 0): making "" equal to word2[0..j) requires j deletions.
    std::vector<int> dp(n + 1uz, 0);
    for (std::size_t j = 0uz; j <= n; ++j) {
      dp[j] = static_cast<int>(j);
    }

    for (std::size_t i = 1uz; i <= m; ++i) {
      // Save dp[i - 1][0] before overwriting dp[0] with the base value for
      // row `i` (making word1[0..i) equal to "" requires i deletions).
      int prev_diag = dp[0];
      dp[0] = static_cast<int>(i);

      for (std::size_t j = 1uz; j <= n; ++j) {
        // Save dp[i - 1][j] before updating dp[j] so it becomes `prev_diag`
        // for column `j + 1`.
        const int prev_up = dp[j];
        if (word1[i - 1uz] == word2[j - 1uz]) {
          // Matching characters require no deletion; carry over diagonal.
          dp[j] = prev_diag;
        } else {
          // Delete either word1[i - 1] (from prev_up) or word2[j - 1]
          // (from dp[j - 1]).
          dp[j] = 1 + std::min(prev_up, dp[j - 1uz]);
        }
        prev_diag = prev_up;
      }
    }

    return dp[n];
  }

  // Alternative LCS-based solution: delete every character outside a longest
  // common subsequence, yielding m + n - 2 * LCS(word1, word2).
  // Time: O(m * n), Space: O(n).
  int minDistanceViaLcs(std::string_view word1, std::string_view word2) {
    const std::size_t m = word1.size();
    const std::size_t n = word2.size();
    std::vector<int> lcs(n + 1uz, 0);

    for (std::size_t i = 1uz; i <= m; ++i) {
      int prev_diag = 0;
      for (std::size_t j = 1uz; j <= n; ++j) {
        const int prev_up = lcs[j];
        if (word1[i - 1uz] == word2[j - 1uz]) {
          lcs[j] = prev_diag + 1;
        } else {
          lcs[j] = std::max(prev_up, lcs[j - 1uz]);
        }
        prev_diag = prev_up;
      }
    }

    return static_cast<int>(m + n) - (2 * lcs[n]);
  }
};

LC_PROBLEM(583, "Delete Operation For Two Strings", "dp", ctx) {
  ctx.test("example_1_sea_eat", [&] {
    Solution sol;
    ctx.expect_eq(sol.minDistance("sea", "eat"), 2);
    ctx.expect_eq(sol.minDistanceViaLcs("sea", "eat"), 2);
  });

  ctx.test("example_2_leetcode_etco", [&] {
    Solution sol;
    ctx.expect_eq(sol.minDistance("leetcode", "etco"), 4);
    ctx.expect_eq(sol.minDistanceViaLcs("leetcode", "etco"), 4);
  });

  ctx.test("identical_and_empty_strings", [&] {
    Solution sol;
    ctx.expect_eq(sol.minDistance("algorithm", "algorithm"), 0);
    ctx.expect_eq(sol.minDistance("", ""), 0);
    ctx.expect_eq(sol.minDistance("abc", ""), 3);
    ctx.expect_eq(sol.minDistance("", "xy"), 2);
    ctx.expect_eq(sol.minDistanceViaLcs("abc", ""), 3);
  });

  ctx.test("disjoint_strings", [&] {
    Solution sol;
    ctx.expect_eq(sol.minDistance("abc", "defg"), 7);
    ctx.expect_eq(sol.minDistanceViaLcs("abc", "defg"), 7);
  });
}

}  // namespace
