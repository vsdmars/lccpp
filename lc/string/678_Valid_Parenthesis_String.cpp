// LeetCode 678: Valid Parenthesis String
// https://leetcode.com/problems/valid-parenthesis-string/
//
// Problem:
//   Given a string `s` containing only `'('`, `')'`, and `'*'`, determine
//   whether `s` can represent a valid balanced parenthesis string. Each `'*'`
//   wildcard may independently act as `'('`, `')'`, or the empty string `""`.
//   An empty string is valid.
//   Constraints: 1 <= s.size() <= 100.
//
// Algorithm:
//   Greedy feasible-balance interval `[min_open, max_open]`:
//   At every prefix of `s`, the set of achievable unmatched `'('` counts
//   across all valid wildcard assignments forms a contiguous integer interval
//   `[min_open, max_open]`:
//   - Seeing `'('`: every assignment gains one open paren ->
//     `++min_open`, `++max_open`.
//   - Seeing `')'`: every assignment closes one open paren ->
//     `--min_open`, `--max_open`.
//   - Seeing `'*'`: it can close one (`--min_open`), act as empty, or open one
//     (`++max_open`).
//   Invariants maintained at each step:
//   1. If `max_open < 0`, even turning every prior `'*'` into `'('` cannot
//      match the excess `')'` at this prefix -> immediately return `false`.
//   2. Clamp `min_open = std::max(0, min_open)`: any branch that would have
//      dipped to `-1` is discarded (meaning a prior `'*'` is treated as empty
//      or `'('` rather than `')'`); since `max_open >= 0`, `0` remains inside
//      the feasible interval.
//   At the end of `s`, `0` lies inside `[min_open, max_open]` iff
//   `min_open == 0`.
//   Time Complexity: O(N), where N = s.size().
//   Space Complexity: O(1).
//
// Data Structures:
//   - Two scalar integer bounds (`min_open`, `max_open`) representing the
//     minimum and maximum feasible count of unmatched `'('` so far.
//
// Real-World Applications:
//   - Fuzzy/error-tolerant parsers and IDE syntax checkers validating
//     partially typed or placeholder-containing bracketed expressions.
//   - Static analysis of template/macro expansions where optional tokens may
//     introduce or omit grouping delimiters.
//   - Protocol and grammar validators checking balanced framing under wildcard
//     or masked-byte uncertainty.
//
// Related LeetCode Problems:
//   - #20 Valid Parentheses [done] (exact bracket matching with a stack, no wildcards)
//   - #32 Longest Valid Parentheses [done] (finding the longest well-formed parenthesis substring)
//   - #301 Remove Invalid Parentheses [done] (enumerating minimal deletions for balanced parentheses)
//   - #1249 Minimum Remove to Make Valid Parentheses [done] (single-pass repair of unmatched parentheses)
//   - #2116 Check if a Parentheses String Can Be Valid [new] (same [min_open, max_open] invariant with locked mask)

#include "framework/lc_framework.h"

#include <algorithm>
#include <string_view>

namespace {

class Solution {
 public:
  // Returns true iff `s` can be balanced by interpreting each '*' as '(', ')',
  // or empty string.
  // Time: O(N), Space: O(1).
  bool checkValidString(std::string_view s) {
    // [min_open, max_open] tracks the smallest and largest valid count of
    // currently unmatched '(' across all wildcard choices so far.
    int min_open = 0;
    int max_open = 0;

    for (const char c : s) {
      if (c == '(') {
        ++min_open;
        ++max_open;
      } else if (c == ')') {
        --min_open;
        --max_open;
      } else if (c == '*') {
        // '*' can act as ')' (decreasing lower bound) or '(' (raising upper).
        --min_open;
        ++max_open;
      }

      // Even treating every previous '*' as '(' leaves an unmatched ')'.
      if (max_open < 0) {
        return false;
      }

      // Negative open counts are invalid prefixes; clamp the lower bound to 0
      // (equivalent to treating a prior '*' as "" instead of ')').
      min_open = std::max(0, min_open);
    }

    // Valid iff zero unmatched '(' is reachable at the end of the string.
    return min_open == 0;
  }
};

LC_PROBLEM(678, "Valid Parenthesis String", "string", ctx) {
  ctx.test("example_1_simple_pair", [&] {
    Solution sol;
    ctx.expect_true(sol.checkValidString("()"));
  });

  ctx.test("example_2_star_as_empty", [&] {
    Solution sol;
    ctx.expect_true(sol.checkValidString("(*)"));
  });

  ctx.test("example_3_star_as_open", [&] {
    Solution sol;
    ctx.expect_true(sol.checkValidString("(*))"));
  });

  ctx.test("python_build_trailing_open", [&] {
    Solution sol;
    ctx.expect_false(sol.checkValidString("**("));
  });

  ctx.test("edge_cases_order_and_wildcards", [&] {
    Solution sol;
    ctx.expect_true(sol.checkValidString(""));
    ctx.expect_true(sol.checkValidString("*"));
    ctx.expect_true(sol.checkValidString("(((******))"));
    ctx.expect_false(sol.checkValidString(")("));
    ctx.expect_false(sol.checkValidString("(()"));
    ctx.expect_false(sol.checkValidString(")(*"));
  });
}

}  // namespace
