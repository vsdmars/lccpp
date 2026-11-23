// LeetCode 32: Longest Valid Parentheses
// https://leetcode.com/problems/longest-valid-parentheses/
//
// Problem:
//   Given a string s consisting solely of '(' and ')' characters (length up to
//   3 * 10^4), return the length of the longest contiguous substring that forms
//   a well-formed (balanced) parentheses expression.
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
