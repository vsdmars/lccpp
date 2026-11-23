// LeetCode 115: Distinct Subsequences
// https://leetcode.com/problems/distinct-subsequences/
//
// Problem:
//   Given two strings s and t consisting of English letters, return the number
//   of distinct subsequences of s that equal t. A subsequence is formed by
//   deleting zero or more characters from s without disturbing the relative
//   order of the remaining characters. The final answer is guaranteed to fit in
//   a 32-bit signed integer.
//   Constraints: 1 <= s.length, t.length <= 1000.
//
// Algorithm:
//   Bottom-up 2D Dynamic Programming over string prefixes:
//   - Define dp[y][x] = number of distinct subsequences of s[0..x-1] that equal
//     t[0..y-1].
//   - Base cases:
//       * dp[0][x] = 1 for all 0 <= x <= |s| (the empty string t="" appears as
//         a subsequence of any prefix of s in exactly one way: delete all).
//       * dp[y][0] = 0 for all 1 <= y <= |t| (a non-empty target cannot be
//         formed from an empty source).
//   - Recurrence for 1 <= y <= |t| and 1 <= x <= |s|:
//       * Always able to skip s[x - 1]: contributes dp[y][x - 1].
//       * If s[x - 1] == t[y - 1], we may also match s[x - 1] with t[y - 1]:
//         contributes dp[y - 1][x - 1].
//       * Thus dp[y][x] = dp[y][x - 1] + (s[x - 1] == t[y - 1] ? dp[y - 1][x - 1] : 0).
//   - Intermediate DP values are stored in std::uint64_t to prevent signed
//     32-bit integer overflow on unreachable/unused subproblems before casting
//     the guaranteed-in-range final answer dp[|t|][|s|] to int.
//   - Time Complexity: O(|s| * |t|).
//   - Space Complexity: O(|s| * |t|) for the 2D table (reducible to O(|t|) by
//     iterating y backwards over a single 1D array).
//
// Data Structures:
//   - 2D DP table (std::vector<std::vector<std::uint64_t>>) of size
//     (|t| + 1) x (|s| + 1) storing subsequence counts per prefix pair.
//   - std::string_view for zero-copy read-only string access.
//
// Real-World Applications:
//   - Counting pattern occurrences with gaps in biological sequence analysis
//     (motif matching in DNA/protein sequences).
//   - Fuzzy log and event-stream querying (counting distinct ways a target
//     workflow signature occurred within an interleaved audit log).
//   - Natural language processing alignment models and edit-transcript counting.
//
// Related LeetCode Problems:
//   - #97 Interleaving String [done] (2D DP matching characters across two strings)
//   - #72 Edit Distance [done] (2D prefix DP transforming one string into another)
//   - #392 Is Subsequence [done] (existence version checking whether count > 0)
//   - #940 Distinct Subsequences II [new] (counting all distinct non-empty subsequences of one string)
//   - #1143 Longest Common Subsequence [done] (2D prefix DP comparing two strings)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Counts the number of distinct subsequences of `s` equal to `t` via
  // bottom-up 2D DP.
  //
  // Invariant: dp[y][x] holds the number of ways to form prefix t[0..y-1] as a
  // subsequence of prefix s[0..x-1].
  // Time: O(|s| * |t|), Space: O(|s| * |t|).
  int numDistinct(std::string_view s, std::string_view t) {
    const std::size_t s_len = s.size();
    const std::size_t t_len = t.size();

    // A subsequence cannot be longer than the source string.
    if (s_len < t_len) {
      return 0;
    }

    // Use uint64_t internally because intermediate prefix counts can exceed
    // INT_MAX even when the final answer fits in a 32-bit signed int.
    std::vector<std::vector<std::uint64_t>> dp(
        t_len + 1, std::vector<std::uint64_t>(s_len + 1, 0U));

    // Base case: the empty target t[0..0) is formed in exactly 1 way from any
    // prefix of s (by selecting no characters).
    for (std::size_t x = 0uz; x <= s_len; ++x) {
      dp[0][x] = 1U;
    }

    // Fill row by row (cache-friendly row-major order over x).
    for (std::size_t y = 1uz; y <= t_len; ++y) {
      for (std::size_t x = 1uz; x <= s_len; ++x) {
        // Case 1: skip s[x - 1] and reuse all matches found in s[0..x-2].
        dp[y][x] = dp[y][x - 1];

        // Case 2: if the current characters match, add all ways to form
        // t[0..y-2] from s[0..x-2] and append s[x - 1].
        if (s[x - 1] == t[y - 1]) {
          dp[y][x] += dp[y - 1][x - 1];
        }
      }
    }

    return static_cast<int>(dp[t_len][s_len]);
  }
};

LC_PROBLEM(115, "Distinct Subsequences", "dp", ctx) {
  Solution sol;

  ctx.test("example_1_rabbbit", [&] {
    ctx.expect_eq(sol.numDistinct("rabbbit", "rabbit"), 3);
  });

  ctx.test("example_2_babgbag", [&] {
    ctx.expect_eq(sol.numDistinct("babgbag", "bag"), 5);
  });

  ctx.test("identical_strings", [&] {
    ctx.expect_eq(sol.numDistinct("leetcode", "leetcode"), 1);
  });

  ctx.test("source_shorter_than_target", [&] {
    ctx.expect_eq(sol.numDistinct("ab", "abc"), 0);
  });

  ctx.test("no_matching_subsequence", [&] {
    ctx.expect_eq(sol.numDistinct("abcdef", "xyz"), 0);
  });

  ctx.test("repeated_characters_binomial", [&] {
    // Choosing 2 positions out of 5 identical 'a' characters: C(5, 2) = 10.
    ctx.expect_eq(sol.numDistinct("aaaaa", "aa"), 10);
  });

  ctx.test("empty_target", [&] {
    ctx.expect_eq(sol.numDistinct("abc", ""), 1);
    ctx.expect_eq(sol.numDistinct("", ""), 1);
  });
}

}  // namespace
