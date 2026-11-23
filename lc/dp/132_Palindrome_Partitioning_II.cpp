// LeetCode 132: Palindrome Partitioning II
// https://leetcode.com/problems/palindrome-partitioning-ii/
//
// Problem:
//   Given a string s of lowercase English letters, partition s so that every
//   substring in the partition is a palindrome. Return the minimum number of
//   cuts required to achieve such a palindrome partitioning.
//   Constraints: 1 <= s.length <= 2000.
//
// Examples:
//   Example 1:
//     Input:  s = "aab"
//     Output: 1
//     Explanation:
//       - Initialize dp = [-1, 0, 1, 2] where dp[i] = i - 1 is the worst-case cut
//         count for prefix s[0..i-1].
//       - At center = 0, odd expansion (0, 0) sets dp[1] = 0 and even expansion (0,
//         1) matches "aa" to relax dp[2] = min(1, dp[0] + 1) = 0.
//       - At center = 1, odd expansion (1, 1) keeps dp[2] = 0 and even expansion
//         (1, 2) stops immediately because 'a' != 'b'.
//       - At center = 2, odd expansion (2, 2) relaxes dp[3] = min(2, dp[2] + 1) =
//         1, returning dp[3] = 1.
//
//   Example 2:
//     Input:  s = "abacdc"
//     Output: 1
//     Explanation:
//       - Initialize dp = [-1, 0, 1, 2, 3, 4, 5] for n = 6.
//       - Expanding odd center = 1 ('b') matches [0, 2] ("aba") and relaxes dp[3] =
//         min(2, dp[0] + 1) = 0.
//       - Expanding odd center = 4 ('d') matches [3, 5] ("cdc") and relaxes dp[6] =
//         min(5, dp[3] + 1) = 1.
//       - Remaining center = 5 leaves dp[6] = 1 unchanged, returning 1 cut ("aba" |
//         "cdc").
//
//   Example 3:
//     Input:  s = "a"
//     Output: 0
//     Explanation:
//       - Check the string length n = s.size() = 1.
//       - Because n <= 1, minCut returns 0 immediately without allocating the DP
//         array.
//
// Algorithm:
//   Bottom-up 1D Dynamic Programming combined with expand-around-center:
//   - Define dp[i] = minimum cuts needed to partition the prefix s[0..i-1].
//   - Base case: dp[0] = -1, so that if an entire prefix s[0..r] is a single
//     palindrome, dp[0] + 1 = 0 cuts are needed (matching cut-count semantics
//     rather than partition-count semantics).
//   - Initialize dp[i] = i - 1 (worst case: cut between every adjacent pair of
//     characters).
//   - For each center index c from 0 to n - 1, expand both odd-length
//     palindromes (c, c) and even-length palindromes (c, c + 1) outward while
//     s[left] == s[right]. Whenever s[left..right] is a palindrome, relax:
//       dp[right + 1] = min(dp[right + 1], dp[left] + 1).
//   - Correctness invariant: because every palindrome ending before index `c`
//     has its center strictly less than `c`, dp[0..c] are already finalized
//     before center `c` reads dp[left] (since left <= c).
//   - This improves the naive O(n^3) substring-reversal check to optimal O(n^2)
//     time and O(n) space without needing a 2D palindrome table.
//   - Time Complexity: O(n^2).
//   - Space Complexity: O(n).
//
// Data Structures:
//   - 1D DP array (std::vector<int>) of size n + 1 storing minimum cuts for
//     every prefix length.
//   - std::string_view for zero-copy character indexing.
//
// Real-World Applications:
//   - Minimum-segment text/DNA decomposition into symmetric or inverted-repeat
//     structural motifs (e.g., RNA hairpin and restriction-site analysis).
//   - Optimal sequence segmentation in compilers and data compression where
//     valid chunks satisfy a structural predicate.
//   - Shortest-path reduction on implicit interval DAGs (each valid palindrome
//     is a unit-cost directed edge from `left` to `right + 1`).
//
// Related LeetCode Problems:
//   - #139 Word Break [done] (same 1D prefix-partition DP pattern over valid substrings)
//   - #131 Palindrome Partitioning [done] (enumerates all palindrome partitions via backtracking)
//   - #5 Longest Palindromic Substring [done] (uses the same expand-around-center technique)
//   - #647 Palindromic Substrings [done] (counts all palindromic substrings via center expansion)
//   - #1278 Palindrome Partitioning III [new] (partitions into exactly k substrings minimizing edits)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the minimum number of cuts to partition `s` into palindromes.
  //
  // Invariant: once all centers < c have been expanded, dp[0..c] contain the
  // globally optimal minimum cut counts for prefixes of lengths 0..c.
  // Time: O(n^2), Space: O(n).
  int minCut(std::string_view s) {
    const std::size_t n = s.size();
    if (n <= 1) {
      return 0;
    }

    // dp[i] = min cuts for prefix s[0..i-1].
    // dp[0] = -1 so a palindrome starting at index 0 produces dp[0] + 1 = 0
    // cuts; worst-case dp[i] = i - 1 cuts (single-character palindromes).
    std::vector<int> dp(n + 1, 0);
    for (std::size_t i = 0uz; i <= n; ++i) {
      dp[i] = static_cast<int>(i) - 1;
    }

    for (std::size_t center = 0uz; center < n; ++center) {
      // Odd-length palindromes centered at `center`.
      expandAndRelax(s, center, center, dp);
      // Even-length palindromes centered between `center` and `center + 1`.
      expandAndRelax(s, center, center + 1, dp);
    }

    return dp[n];
  }

 private:
  // Expands outward from [left, right] while characters match, relaxing
  // dp[right + 1] using the already-finalized prefix optimum dp[left].
  static void expandAndRelax(std::string_view s, std::size_t left,
                             std::size_t right, std::vector<int>& dp) {
    const std::size_t n = s.size();
    while (right < n && s[left] == s[right]) {
      dp[right + 1] = std::min(dp[right + 1], dp[left] + 1);
      if (left == 0) {
        break;
      }
      --left;
      ++right;
    }
  }
};

LC_PROBLEM(132, "Palindrome Partitioning II", "dp", ctx) {
  Solution sol;

  ctx.test("example_1_aab", [&] {
    ctx.expect_eq(sol.minCut("aab"), 1);
  });

  ctx.test("example_2_single_char", [&] {
    ctx.expect_eq(sol.minCut("a"), 0);
  });

  ctx.test("example_3_ab", [&] {
    ctx.expect_eq(sol.minCut("ab"), 1);
  });

  ctx.test("whole_string_palindromes", [&] {
    ctx.expect_eq(sol.minCut("racecar"), 0);
    ctx.expect_eq(sol.minCut("abba"), 0);
    ctx.expect_eq(sol.minCut("aabaa"), 0);
  });

  ctx.test("all_distinct_characters", [&] {
    ctx.expect_eq(sol.minCut("abcdef"), 5);
  });

  ctx.test("multi_cut_cases", [&] {
    // "aba" | "cdc" -> 1 cut.
    ctx.expect_eq(sol.minCut("abacdc"), 1);
    // "c" | "dd" -> 1 cut.
    ctx.expect_eq(sol.minCut("cdd"), 1);
    // "c" | "ababa" | "b" | "cbc" -> 3 cuts.
    ctx.expect_eq(sol.minCut("cabababcbc"), 3);
  });
}

}  // namespace
