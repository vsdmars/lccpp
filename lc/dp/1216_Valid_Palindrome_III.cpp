// LeetCode 1216: Valid Palindrome III
// https://leetcode.com/problems/valid-palindrome-iii/
//
// Problem:
//   Given a lowercase English string `s` (length 1..1000) and an integer `k`
//   (`1 <= k <= s.length`), determine whether `s` is a K-Palindrome — i.e.,
//   whether `s` can be turned into a palindrome by deleting at most `k`
//   characters.
//
// Algorithm:
//   A string of length `n` can become a palindrome with at most `k` deletions
//   if and only if its Longest Palindromic Subsequence (LPS) has length at
//   least `n - k` (equivalently, `n - LPS(s) <= k`).
//   We compute `LPS(s)` with space-optimized bottom-up interval DP over
//   substrings `s[l..r]`, iterating `l` backward from `n - 1` down to `0` and
//   `r` forward from `l + 1` to `n - 1`:
//     - Base case: every single character is a palindrome of length 1
//       (`cur[l] = 1`).
//     - If `s[l] == s[r]`: both endpoints wrap the inner optimal palindrome, so
//       `cur[r] = 2 + prev[r - 1]` (where `prev` stores row `l + 1`; when
//       `r == l + 1`, `prev[l]` is 0 for the empty inner range).
//     - Otherwise: `cur[r] = max(cur[r - 1], prev[r])`, dropping either the
//       right or left endpoint.
//   After finishing all `r`, `prev` and `cur` are swapped so `prev` becomes the
//   completed row `l`.
//   Time Complexity:  O(n^2), where `n = s.size()`.
//   Space Complexity: O(n) using two rolling 1D rows.
//
// Data Structures:
//   - Two 1D DP vectors (`prev_dp` and `cur_dp` of size `n`) representing
//     consecutive rows `l + 1` and `l` of the interval DP table.
//
// Real-World Applications:
//   - Bioinformatics detection of imperfect inverted repeats and RNA secondary
//     structure hairpins allowing up to `k` bulges/mismatches.
//   - Fuzzy string matching and error-tolerant symmetry verification in coding
//     theory and data validation.
//
// Related LeetCode Problems:
//   - #125 Valid Palindrome [done] (two-pointer exact palindrome verification)
//   - #680 Valid Palindrome II [done] (special case k = 1 solved greedily in O(n))
//   - #516 Longest Palindromic Subsequence [done] (exact underlying LPS DP)
//   - #1312 Minimum Insertion Steps to Make a String Palindrome [new] (identical n - LPS metric)
//   - #1143 Longest Common Subsequence [done] (general 2D sequence alignment DP)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string_view>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Checks whether `s` can be transformed into a palindrome by deleting at most
  // `k` characters via rolling-row Longest Palindromic Subsequence DP.
  // Time: O(n^2), Space: O(n).
  bool isValidPalindrome(std::string_view s, int k) {
    const std::size_t n = s.size();
    if (n == 0uz) {
      return true;
    }

    // `prev_dp[r]` holds LPS length for `s[l + 1 .. r]`; `cur_dp[r]` computes
    // row `l`. Indices `< l` remain 0 so `prev_dp[l]` cleanly represents the
    // empty substring `s[l + 1 .. l]` when `r == l + 1`.
    std::vector<int> prev_dp(n, 0);
    std::vector<int> cur_dp(n, 0);

    for (std::size_t left = n; left > 0uz; --left) {
      const std::size_t l = left - 1uz;
      cur_dp[l] = 1;

      for (std::size_t r = l + 1uz; r < n; ++r) {
        if (s[l] == s[r]) {
          // Endpoints match: extend the best palindrome strictly inside
          // `s[l + 1 .. r - 1]`.
          cur_dp[r] = prev_dp[r - 1uz] + 2;
        } else {
          // Endpoints differ: exclude either `s[r]` or `s[l]`.
          cur_dp[r] = std::max(cur_dp[r - 1uz], prev_dp[r]);
        }
      }

      std::swap(prev_dp, cur_dp);
    }

    const int min_deletions = static_cast<int>(n) - prev_dp[n - 1uz];
    return min_deletions <= k;
  }
};

LC_PROBLEM(1216, "Valid Palindrome III", "dp", ctx) {
  Solution sol;

  ctx.test("example_1_valid_with_two_deletions", [&] {
    ctx.expect_true(sol.isValidPalindrome("abcdeca", 2));
    ctx.expect_false(sol.isValidPalindrome("abcdeca", 1));
  });

  ctx.test("example_2_valid_with_one_deletion", [&] {
    ctx.expect_true(sol.isValidPalindrome("abbababa", 1));
  });

  ctx.test("python_build_case", [&] {
    // LPS of "bacabaaa" has length 5 ("bacab" or "aabaa"), so 3 deletions are
    // required (8 - 5 = 3).
    ctx.expect_false(sol.isValidPalindrome("bacabaaa", 2));
    ctx.expect_true(sol.isValidPalindrome("bacabaaa", 3));
  });

  ctx.test("edge_cases_palindromes_and_all_distinct", [&] {
    ctx.expect_true(sol.isValidPalindrome("a", 0));
    ctx.expect_true(sol.isValidPalindrome("racecar", 0));
    ctx.expect_true(sol.isValidPalindrome("aa", 0));
    ctx.expect_false(sol.isValidPalindrome("abcd", 2));
    ctx.expect_true(sol.isValidPalindrome("abcd", 3));
  });
}

}  // namespace
