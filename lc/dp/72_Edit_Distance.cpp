// LeetCode 72: Edit Distance
// https://leetcode.com/problems/edit-distance/
//
// Problem:
//   Given two strings word1 and word2 (lengths up to 500), return the minimum
//   number of single-character edits — insert, delete, or replace — required to
//   transform word1 into word2 (Levenshtein distance).
//
// Algorithm:
//   Bottom-up dynamic programming (Wagner-Fischer algorithm) with 1D rolling
//   row space optimization. Let dp[i][j] denote the edit distance between
//   word1[0..i) and word2[0..j).
//   - Base cases: dp[0][j] = j (j insertions) and dp[i][0] = i (i deletions).
//   - Transition for i >= 1, j >= 1:
//       * If word1[i - 1] == word2[j - 1]: dp[i][j] = dp[i - 1][j - 1].
//       * Otherwise: dp[i][j] = 1 + min(dp[i - 1][j - 1] [replace],
//                                       dp[i - 1][j]     [delete],
//                                       dp[i][j - 1]     [insert]).
//   Because edit distance is symmetric and row i only reads row i - 1 and the
//   immediately preceding column in row i, we swap strings if needed so n <= m
//   and keep a single array of size n + 1 plus one scalar `prev_diag` tracking
//   dp[i - 1][j - 1].
//   Time Complexity:  O(m * n).
//   Space Complexity: O(min(m, n)).
//
// Data Structures:
//   - std::vector<int> dp of length min(m, n) + 1: rolling DP row holding
//     prefix edit distances, paired with scalar `prev_diag` for the diagonal
//     predecessor dp[i - 1][j - 1].
//
// Real-World Applications:
//   - Spell checkers, search-query autocorrection, and fuzzy autocomplete.
//   - DNA/protein sequence alignment and mutation-distance estimation in
//     bioinformatics.
//   - Diff utilities (`git diff`), plagiarism detection, and OCR post-processing.
//
// Related LeetCode Problems:
//   - #161 One Edit Distance [done] (O(n) check for edit distance == 1)
//   - #583 Delete Operation for Two Strings [done] (delete-only edit distance)
//   - #712 Minimum ASCII Delete Sum for Two Strings [new] (weighted deletions)
//   - #1143 Longest Common Subsequence [done] (dual two-string prefix DP)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string_view>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the Levenshtein edit distance between `word1` and `word2`.
  // Time: O(m * n), Space: O(min(m, n)).
  int minDistance(std::string_view word1, std::string_view word2) {
    // Edit distance is symmetric; make `word2` the shorter string so the
    // rolling DP row uses O(min(m, n)) memory.
    if (word1.size() < word2.size()) {
      std::swap(word1, word2);
    }

    const std::size_t m = word1.size();
    const std::size_t n = word2.size();

    // Base case row i = 0: converting "" to word2[0..j) requires j insertions.
    std::vector<int> dp(n + 1uz, 0);
    for (std::size_t j = 0uz; j <= n; ++j) {
      dp[j] = static_cast<int>(j);
    }

    for (std::size_t i = 1uz; i <= m; ++i) {
      // Save dp[i - 1][0] before overwriting dp[0] with the base case for row i
      // (converting word1[0..i) to "" requires i deletions).
      int prev_diag = dp[0];
      dp[0] = static_cast<int>(i);

      for (std::size_t j = 1uz; j <= n; ++j) {
        // Capture dp[i - 1][j] (delete cost) before updating dp[j] in place;
        // it becomes dp[i - 1][(j + 1) - 1] on the next column iteration.
        const int prev_up = dp[j];

        if (word1[i - 1uz] == word2[j - 1uz]) {
          // Matching characters incur no additional edit cost.
          dp[j] = prev_diag;
        } else {
          const int replace_cost = prev_diag;
          const int delete_cost = prev_up;
          const int insert_cost = dp[j - 1uz];
          dp[j] = 1 + std::min({replace_cost, delete_cost, insert_cost});
        }

        prev_diag = prev_up;
      }
    }

    return dp[n];
  }
};

LC_PROBLEM(72, "Edit Distance", "dp", ctx) {
  Solution sol;

  ctx.test("example_1_horse_to_ros", [&] {
    // Matches the Python build() test case.
    ctx.expect_eq(sol.minDistance("horse", "ros"), 3);
    ctx.expect_eq(sol.minDistance("ros", "horse"), 3);
  });

  ctx.test("example_2_intention_to_execution", [&] {
    ctx.expect_eq(sol.minDistance("intention", "execution"), 5);
  });

  ctx.test("empty_strings", [&] {
    ctx.expect_eq(sol.minDistance("", ""), 0);
    ctx.expect_eq(sol.minDistance("", "abc"), 3);
    ctx.expect_eq(sol.minDistance("abcd", ""), 4);
  });

  ctx.test("identical_and_single_char", [&] {
    ctx.expect_eq(sol.minDistance("algorithm", "algorithm"), 0);
    ctx.expect_eq(sol.minDistance("a", "b"), 1);
    ctx.expect_eq(sol.minDistance("a", "a"), 0);
  });
}

}  // namespace
