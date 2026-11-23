// LeetCode 32: Longest Valid Parentheses
// https://leetcode.com/problems/longest-valid-parentheses/
//
// Problem:
//   Given a string s consisting solely of '(' and ')' characters (length up to
//   3 * 10^4), return the length of the longest contiguous substring that forms
//   a well-formed (balanced) parentheses expression.
//
// Examples:
//   Example 1:
//     Input:  s = ")()())"
//     Output: 4
//     Explanation:
//       - Initialize dp = [0, 0, 0, 0, 0, 0] and skip i = 1 since s[1] == '('.
//       - At i = 2 (s[2] == ')', s[1] == '('), Case 1 sets dp[2] = dp[0] + 2 = 2.
//       - Skip i = 3 ('('), then at i = 4 (s[4] == ')', s[3] == '('), Case 1
//         extends dp[2] to give dp[4] = dp[2] + 2 = 4.
//       - At i = 5 (s[5] == ')', s[4] == ')'), Case 2 checks open_idx = 5 - dp[4] -
//         1 = 0 where s[0] == ')' != '(', leaving dp[5] = 0 and max_len = 4.
//
//   Example 2:
//     Input:  s = "()(()())"
//     Output: 8
//     Explanation:
//       - At i = 1 (s[1] == ')', s[0] == '('), Case 1 sets dp[1] = 2; opening
//         parens at indices 2 and 3 keep dp[2] = dp[3] = 0.
//       - At i = 4 (s[4] == ')', s[3] == '('), Case 1 sets dp[4] = dp[2] + 2 = 2,
//         and index 5 ('(') stays dp[5] = 0.
//       - At i = 6 (s[6] == ')', s[5] == '('), Case 1 merges with dp[4] to compute
//         dp[6] = dp[4] + 2 = 4.
//       - At i = 7 (s[7] == ')', s[6] == ')'), Case 2 uses inner_len = dp[6] = 4 to
//         match s[open_idx] = s[2] == '(' and merges dp[1] = 2, yielding dp[7] = 4
//         + 2 + 2 = 8.
//
//   Example 3:
//     Input:  s = "(()(((()"
//     Output: 2
//     Explanation:
//       - Initialize dp of length 8 to all zeros; i = 1 ('(') stays 0, and i = 2
//         (')' matching '(' at 1) sets dp[2] = dp[0] + 2 = 2.
//       - Indices 3 through 6 are all '(' characters, so dp[3] = dp[4] = dp[5] =
//         dp[6] = 0.
//       - At i = 7 (s[7] == ')', s[6] == '('), Case 1 sets dp[7] = dp[5] + 2 = 2
//         without reaching dp[2], returning max_len = 2.
//
// Algorithm:
//   Bottom-up 1D dynamic programming. Let dp[i] be the length of the longest
//   valid parentheses substring that ends exactly at index i. Any index with
//   s[i] == '(' has dp[i] = 0 because a valid substring cannot end with '('.
//   When s[i] == ')':
//     1. Immediate pair ("...()"): if s[i - 1] == '(', then dp[i] = 2 +
//        (i >= 2 ? dp[i - 2] : 0), extending any valid substring immediately
//        before the pair.
//     2. Nested pair ("...((...))"): if s[i - 1] == ')', the valid block ending
//        at i - 1 has length prev = dp[i - 1]. The candidate matching '(' sits
//        at open_idx = i - prev - 1. If open_idx is in bounds and
//        s[open_idx] == '(', then dp[i] = prev + 2 plus any contiguous valid
//        substring ending immediately before open_idx (dp[open_idx - 1]).
//   The answer is max(dp[i]) over all indices.
//   Time Complexity:  O(n), single left-to-right pass over s.
//   Space Complexity: O(n) for the dp table.
//
// Data Structures:
//   - std::vector<std::size_t> dp: stores the longest valid substring length
//     ending at each character index so adjacent valid blocks can be merged in
//     O(1) time.
//
// Real-World Applications:
//   - Compiler and IDE syntax-error recovery: isolating the longest well-formed
//     expression or balanced delimiter span in malformed source code.
//   - Log and query-language parsers validating nested grouping expressions.
//   - RNA secondary-structure analysis identifying longest balanced base-pair
//     motifs.
//
// Related LeetCode Problems:
//   - #20 Valid Parentheses [done] (stack-based full-string validity check)
//   - #22 Generate Parentheses [done] (enumerating all valid balanced strings)
//   - #301 Remove Invalid Parentheses [done] (minimum removals to balance)
//   - #856 Score of Parentheses [done] (recursive/stack scoring of balanced spans)
//   - #1249 Minimum Remove to Make Valid Parentheses [done] (single valid string)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the length of the longest contiguous valid parentheses substring
  // in `s` via bottom-up DP.
  // Time: O(n), Space: O(n).
  int longestValidParentheses(std::string_view s) {
    const std::size_t n = s.size();
    if (n < 2) {
      return 0;
    }

    // dp[i] = length of the longest valid parentheses substring ending at i.
    // Entries for '(' remain 0 because no valid substring can end with '('.
    std::vector<std::size_t> dp(n, 0uz);
    std::size_t max_len = 0uz;

    for (std::size_t i = 1uz; i < n; ++i) {
      if (s[i] != ')') {
        continue;
      }

      if (s[i - 1] == '(') {
        // Case 1: "...()" — extend any valid substring ending at i - 2.
        const std::size_t before_pair = (i >= 2uz) ? dp[i - 2] : 0uz;
        dp[i] = before_pair + 2uz;
      } else {
        // Case 2: "...))" — skip over the valid block ending at i - 1 to check
        // whether the character immediately before it is a matching '('.
        const std::size_t inner_len = dp[i - 1];
        if (i > inner_len && s[i - inner_len - 1] == '(') {
          const std::size_t open_idx = i - inner_len - 1;
          const std::size_t before_open = (open_idx > 0uz) ? dp[open_idx - 1] : 0uz;
          dp[i] = inner_len + 2uz + before_open;
        }
      }

      max_len = std::max(max_len, dp[i]);
    }

    return static_cast<int>(max_len);
  }
};

LC_PROBLEM(32, "Longest Valid Parentheses", "dp", ctx) {
  Solution sol;

  ctx.test("example_1_leading_open", [&] {
    ctx.expect_eq(sol.longestValidParentheses("(()"), 2);
  });

  ctx.test("example_2_consecutive_pairs", [&] {
    ctx.expect_eq(sol.longestValidParentheses(")()())"), 4);
  });

  ctx.test("nested_and_concatenated", [&] {
    // Matches the Python build() test case.
    ctx.expect_eq(sol.longestValidParentheses("((()))()"), 8);
    ctx.expect_eq(sol.longestValidParentheses("()(())"), 6);
  });

  ctx.test("empty_and_single_char", [&] {
    ctx.expect_eq(sol.longestValidParentheses(""), 0);
    ctx.expect_eq(sol.longestValidParentheses("("), 0);
    ctx.expect_eq(sol.longestValidParentheses(")"), 0);
  });

  ctx.test("broken_by_unmatched_open", [&] {
    ctx.expect_eq(sol.longestValidParentheses("(()(((()"), 2);
    ctx.expect_eq(sol.longestValidParentheses(")))((("), 0);
  });

  ctx.test("deeply_nested_with_prefix_merge", [&] {
    ctx.expect_eq(sol.longestValidParentheses("()(()())"), 8);
    ctx.expect_eq(sol.longestValidParentheses("(()())"), 6);
  });
}

}  // namespace
