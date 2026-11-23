// LeetCode 10: Regular Expression Matching
// https://leetcode.com/problems/regular-expression-matching/
//
// Problem:
//   Given an input string `s` and a pattern `p`, implement regular expression
//   matching with full-string coverage supporting two metacharacters:
//     - '.' matches any single character.
//     - '*' matches zero or more occurrences of the immediately preceding
//       element.
//   Return true if `p` matches the entire string `s`, and false otherwise.
//   Constraints: 1 <= s.length <= 20, 1 <= p.length <= 20; `s` contains only
//   lowercase English letters, `p` contains lowercase English letters, '.',
//   and '*', and every '*' is guaranteed to have a valid preceding character.
//
// Examples:
//   Example 1:
//     Input:  s = "aab", p = "c*a*b"
//     Output: true
//     Explanation:
//       - Start `dfs(0, 0)` where `p[1] == '*'`; since `s[0] = 'a' != 'c'`,
//         `first_match` is `false`, so skip `"c*"` via `dfs(0, 2)`.
//       - At `dfs(0, 2)` (`p[2..] = "a*b"`), `p[3] == '*'` and `s[0] == 'a'` allow
//         taking the `first_match` branch `dfs(1, 2)` after `dfs(0, 4)` fails to
//         match `"aab"` against `"b"`.
//       - At `dfs(1, 2)`, `s[1] == 'a'` advances to `dfs(2, 2)`, where `s[2] = 'b'
//         != 'a'` forces skipping `"a*"` to `dfs(2, 4)`.
//       - At `dfs(2, 4)`, `s[2] == p[4] == 'b'` advances to `dfs(3, 5)`, where `j
//         == 5 == n` and `i == 3 == m` return `true`.
//
//   Example 2:
//     Input:  s = "mississippi", p = "mis*is*p*."
//     Output: false
//     Explanation:
//       - Match `"mi"` literally via `dfs(0, 0) -> dfs(1, 1) -> dfs(2, 2)`.
//       - Consume `"ss"` with the first `"s*"` (`dfs(2, 2) -> dfs(3, 2) -> dfs(4,
//         2)`), skip `"s*"` to `dfs(4, 4)`, and match `'i'` to reach `dfs(5, 5)`.
//       - Consume `"ss"` with the second `"s*"` (`dfs(5, 5) -> dfs(6, 5) -> dfs(7,
//         5)`), then skip `"s*"` to `dfs(7, 7)` where `s[7..] = "ippi"` and `p[7..]
//         = "p*."`.
//       - At `dfs(7, 7)`, `s[7] = 'i' != 'p'` skips `"p*"` to `dfs(7, 9)`, which
//         matches `'.'` against `'i'` to reach `dfs(8, 10)`; with `j == 10` (`n`)
//         and `i == 8 != 11` (`m`), memoized evaluation returns `false` across all
//         branches.
//
//   Example 3:
//     Input:  s = "", p = "a*b*.*"
//     Output: true
//     Explanation:
//       - In `isMatchDp`, set `dp[0][0] = 1` for empty string (`m = 0`) and pattern
//         length `n = 6`.
//       - Scan `j = 2..6` for `i = 0`: at `j = 2` (`p[1] = '*'`), assign `dp[0][2]
//         = dp[0][0] = 1`.
//       - At `j = 4` (`p[3] = '*'`), assign `dp[0][4] = dp[0][2] = 1`, and at `j =
//         6` (`p[5] = '*'`), assign `dp[0][6] = dp[0][4] = 1`.
//       - Return `dp[0][6] != 0`, which evaluates to `true`.
//
// Algorithm:
//   1. Top-Down Memoized Backtracking (`isMatch`):
//      Recursively decide whether `s[i..]` matches `p[j..]`.
//      - Base case: when `j == n`, the match succeeds iff `i == m`.
//      - Check whether the current characters match:
//        `first_match = (i < m) && (p[j] == '.' || s[i] == p[j])`.
//      - If `j + 1 < n && p[j + 1] == '*'`, branch into two choices:
//          (a) Consume zero occurrences of `p[j]*`: advance pattern to `j + 2`.
//          (b) If `first_match`, consume one character from `s` while keeping
//              pattern at `j` so `p[j]*` may match additional characters.
//      - Otherwise, advance both indices to `(i + 1, j + 1)` when `first_match`.
//      Memoizing `(i, j)` in a flat table eliminates exponential backtracking
//      on adversarial inputs such as `s = "aaa...ab"`, `p = "a*a*...a*"`.
//   2. Bottom-Up Dynamic Programming (`isMatchDp`):
//      `dp[i][j]` is true iff prefix `s[0..i)` matches prefix `p[0..j)`.
//      - `dp[0][0] = true`; for empty `s`, `dp[0][j] = dp[0][j - 2]` whenever
//        `p[j - 1] == '*'`.
//      - For each `(i, j)`, either match a single character (`dp[i - 1][j - 1]`)
//        or handle `p[j - 1] == '*'` via zero occurrences (`dp[i][j - 2]`) or
//        one-or-more occurrences (`prev_matches && dp[i - 1][j]`).
//   Time Complexity:  O(m * n) for both memoized backtracking and bottom-up DP.
//   Space Complexity: O(m * n) for the memoization / DP table (plus O(m + n)
//                     recursion stack for top-down backtracking).
//
// Data Structures:
//   - Flat 2D memoization table (`std::vector<std::int8_t>`) indexed by
//     `(i * (n + 1)) + j` storing tri-state values (`-1` unvisited, `0` false,
//     `1` true) for cache-friendly O(1) lookup.
//   - Flat 2D boolean-byte DP table (`std::vector<std::uint8_t>`) for the
//     iterative bottom-up formulation.
//
// Real-World Applications:
//   - Lexers, tokenizers, and syntax highlighters matching source-code tokens
//     against regular grammars.
//   - Web application firewalls, log analyzers, and URL routers performing
//     glob/regex filtering without catastrophic ReDoS backtracking.
//   - Bioinformatics motif matching and text search engines (`grep`, `ripgrep`,
//     RE2) using Thompson NFA / DP state tables for linear-to-quadratic worst-
//     case matching guarantees.
//
// Related LeetCode Problems:
//   - #44 Wildcard Matching [done] (similar pattern matching with '?' and standalone '*')
//   - #72 Edit Distance [done] (2D string-prefix DP over two indices)
//   - #97 Interleaving String [done] (memoized backtracking / 2D DP matching prefixes)
//   - #115 Distinct Subsequences [done] (2D DP counting string-to-pattern alignments)
//   - #139 Word Break [done] (memoized backtracking over string prefixes)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Top-down memoized backtracking matching entire string `s` against regex `p`.
  // Time: O(m * n), Space: O(m * n), where m = s.size() and n = p.size().
  bool isMatch(std::string_view s, std::string_view p) {
    const std::size_t m = s.size();
    const std::size_t n = p.size();
    const std::size_t stride = n + 1;

    // Tri-state memo table: -1 = uncomputed, 0 = false, 1 = true.
    std::vector<std::int8_t> memo((m + 1) * stride, -1);

    // Recursive helper returning whether `s[i..]` matches `p[j..]`.
    auto dfs = [&](this auto&& self, std::size_t i, std::size_t j) -> bool {
      const std::size_t cell = (i * stride) + j;
      if (memo[cell] != -1) {
        return memo[cell] == 1;
      }

      // Base case: pattern exhausted; match succeeds only if input is also done.
      if (j == n) {
        memo[cell] = (i == m) ? 1 : 0;
        return memo[cell] == 1;
      }

      // Does the current pattern atom match `s[i]`?
      const bool first_match = (i < m) && (p[j] == '.' || s[i] == p[j]);

      bool matched = false;
      if (j + 1 < n && p[j + 1] == '*') {
        // Branch 1: match zero occurrences of `p[j]*` (skip both chars in `p`).
        // Branch 2: match one occurrence (advance `s`) and keep `p[j]*` active.
        matched = self(i, j + 2) || (first_match && self(i + 1, j));
      } else {
        // Exact single-character match: advance both string and pattern by one.
        matched = first_match && self(i + 1, j + 1);
      }

      memo[cell] = matched ? 1 : 0;
      return matched;
    };

    return dfs(0uz, 0uz);
  }

  // Bottom-up dynamic programming approach matching `s` against regex `p`.
  // Invariant: `dp[(i * stride) + j]` is 1 iff `s[0..i)` matches `p[0..j)`.
  // Time: O(m * n), Space: O(m * n).
  bool isMatchDp(std::string_view s, std::string_view p) {
    const std::size_t m = s.size();
    const std::size_t n = p.size();
    const std::size_t stride = n + 1;

    std::vector<std::uint8_t> dp((m + 1) * stride, 0);
    // Empty string matches empty pattern.
    dp[0] = 1;

    // Empty string can only match non-empty patterns composed of `x*` pairs.
    for (std::size_t j = 2; j <= n; ++j) {
      if (p[j - 1] == '*') {
        dp[j] = dp[j - 2];
      }
    }

    for (std::size_t i = 1; i <= m; ++i) {
      for (std::size_t j = 1; j <= n; ++j) {
        const std::size_t cur = (i * stride) + j;
        if (p[j - 1] == '.' || p[j - 1] == s[i - 1]) {
          // Single-character match extends the match of both shorter prefixes.
          dp[cur] = dp[((i - 1) * stride) + (j - 1)];
        } else if (p[j - 1] == '*' && j >= 2) {
          // Zero occurrences of `p[j - 2]*`.
          const bool zero_match = dp[(i * stride) + (j - 2)] != 0;
          // One or more occurrences when `p[j - 2]` matches `s[i - 1]`.
          const bool prev_char_matches =
              (p[j - 2] == '.' || p[j - 2] == s[i - 1]);
          const bool more_match =
              prev_char_matches && (dp[((i - 1) * stride) + j] != 0);
          dp[cur] = (zero_match || more_match) ? 1 : 0;
        }
      }
    }

    return dp[(m * stride) + n] != 0;
  }
};

LC_PROBLEM(10, "Regular Expression Matching", "backtracking", ctx) {
  Solution sol;

  auto expect_both = [&](std::string_view s, std::string_view p,
                         bool expected) {
    ctx.expect_eq(sol.isMatch(s, p), expected);
    ctx.expect_eq(sol.isMatchDp(s, p), expected);
  };

  ctx.test("basic_examples", [&] {
    expect_both("aa", "a", false);
    expect_both("aa", "aa", true);
    expect_both("aaa", "aa", false);
    expect_both("aa", "a*", true);
    expect_both("aa", ".*", true);
    expect_both("ab", ".*", true);
    expect_both("aab", "c*a*b", true);
  });

  ctx.test("exponential_backtracking_trap", [&] {
    // Mirrors the Python `build()` stress test that blows up naive recursion.
    expect_both("aaaaaaaaaaaaaaaaaaab", "a*a*a*a*a*a*a*a*a*a*", false);
    expect_both("aaaaaaaaaaaaaaaaaaaa", "a*a*a*a*a*a*a*a*a*a*", true);
  });

  ctx.test("empty_and_zero_repetition", [&] {
    expect_both("", "", true);
    expect_both("", "a*", true);
    expect_both("", "a*b*.*", true);
    expect_both("", "a*b", false);
    expect_both("a", "", false);
  });

  ctx.test("complex_patterns", [&] {
    expect_both("mississippi", "mis*is*p*.", false);
    expect_both("mississippi", "mis*is*ip*.", true);
    expect_both("ab", ".*c", false);
    expect_both("aaa", "ab*a*c*a", true);
  });
}

}  // namespace
