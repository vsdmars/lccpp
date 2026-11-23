// LeetCode 20: Valid Parentheses
// https://leetcode.com/problems/valid-parentheses/
//
// Problem:
//   Given a string `s` containing only the bracket characters '(', ')', '{',
//   '}', '[', and ']', determine whether the input string is valid. A string
//   is valid if every open bracket is closed by the same type of bracket,
//   open brackets are closed in the exact reverse (LIFO) order they were
//   opened, and every closing bracket has a corresponding preceding open
//   bracket.
//
// Examples:
//   Example 1:
//     Input:  s = "()[]{}"
//     Output: true
//     Explanation:
//       - s.size() is 6 (even), so the odd-length early-exit check passes and
//         open_brackets starts empty.
//       - Each opening bracket '(', '[', '{' is pushed onto open_brackets and
//         immediately matched and popped by its adjacent closing bracket ')', ']',
//         '}'.
//       - After scanning all 6 characters, open_brackets is empty, so isValid
//         returns true.
//
//   Example 2:
//     Input:  s = "([)]"
//     Output: false
//     Explanation:
//       - s.size() is 4 (even); scanning '(' and '[' pushes both onto
//         open_brackets, leaving open_brackets = ['(', '['].
//       - At ')', matching_open(')') is '(', which does not match
//         open_brackets.back() == '['.
//       - isValid immediately returns false without scanning the remaining
//         character.
//
//   Example 3:
//     Input:  s = "((({{{[[[]]]}}}))"
//     Output: false
//     Explanation:
//       - s.size() is 17, which fails the parity check (s.size() % 2uz) != 0uz.
//       - Because balanced pairs always have even total length, isValid returns
//         false in O(1) before inspecting any character.
//
// Algorithm:
//   Scan the string left to right while maintaining a stack of unmatched open
//   brackets. Whenever an open bracket ('(', '[', '{') is encountered, push it
//   onto the stack. Whenever a closing bracket (')', ']', '}') is encountered,
//   verify that the stack is non-empty and its top element is the matching
//   open bracket; if not, immediately return false. Otherwise pop the matched
//   open bracket. An odd-length string can never be balanced, so we reject it
//   in O(1) up front. After scanning all characters, the string is valid if
//   and only if the stack is empty (no unclosed openers remain).
//   Time complexity: O(n), where n = s.size().
//   Space complexity: O(n) in the worst case (all opening brackets).
//
// Data Structures:
//   - std::vector<char> used as an explicit LIFO stack storing pending open
//     brackets waiting to be matched.
//
// Real-World Applications:
//   - Compiler front-ends and lexical/syntax parsers validating balanced
//     delimiters in source code, JSON, XML, and S-expressions.
//   - Code editors and IDEs performing real-time bracket matching, syntax
//     highlighting, and structural code folding.
//   - Mathematical expression validators and template-engine tag verifiers.
//
// Related LeetCode Problems:
//   - #22 Generate Parentheses [done] (generate all valid bracket strings)
//   - #32 Longest Valid Parentheses [done] (longest valid contiguous substring)
//   - #301 Remove Invalid Parentheses [done] (minimum removals to make valid)
//   - #921 Minimum Add to Make Parentheses Valid [done] (min insertions needed)
//   - #1249 Minimum Remove to Make Valid Parentheses [done] (stack cleanup)

#include "framework/lc_framework.h"

#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Returns true if `s` consists of properly nested and matched brackets.
  // Time: O(n), Space: O(n).
  bool isValid(std::string_view s) {
    // Every valid pair consumes two characters, so odd-length strings cannot
    // be balanced.
    if ((s.size() % 2uz) != 0uz) {
      return false;
    }

    // Stack invariant: `open_brackets` holds all unmatched opening brackets
    // seen so far in left-to-right order, with the most recent opener on top.
    std::vector<char> open_brackets;
    open_brackets.reserve(s.size() / 2uz);

    for (const char ch : s) {
      if (ch == '(' || ch == '[' || ch == '{') {
        open_brackets.push_back(ch);
        continue;
      }

      // A closing bracket must match the most recently opened bracket.
      const char expected = matching_open(ch);
      if (expected == '\0' || open_brackets.empty() ||
          open_brackets.back() != expected) {
        return false;
      }
      open_brackets.pop_back();
    }

    // All opened brackets must have been closed.
    return open_brackets.empty();
  }

 private:
  // Maps a closing bracket to its corresponding opening bracket, or '\0' if
  // `close` is not a recognized closing bracket.
  static constexpr char matching_open(char close) noexcept {
    switch (close) {
      case ')':
        return '(';
      case ']':
        return '[';
      case '}':
        return '{';
      default:
        return '\0';
    }
  }
};

LC_PROBLEM(20, "Valid Parentheses", "stack", ctx) {
  Solution sol;

  ctx.test("leetcode_examples", [&] {
    ctx.expect_true(sol.isValid("()"));
    ctx.expect_true(sol.isValid("()[]{}"));
    ctx.expect_false(sol.isValid("(]"));
    ctx.expect_false(sol.isValid("([)]"));
  });

  ctx.test("source_build_cases", [&] {
    ctx.expect_false(sol.isValid("["));
    ctx.expect_true(sol.isValid("{}({[]})"));
    ctx.expect_false(sol.isValid("]"));
    ctx.expect_false(sol.isValid("{[}]"));
    ctx.expect_true(sol.isValid(""));
    ctx.expect_true(sol.isValid("[()]"));
  });

  ctx.test("deep_nesting_and_edge_cases", [&] {
    ctx.expect_true(sol.isValid("((({{{[[[]]]}}})))"));
    ctx.expect_false(sol.isValid("((({{{[[[]]]}}}))"));
    ctx.expect_false(sol.isValid(")(()"));
    ctx.expect_false(sol.isValid("{{{"));
    ctx.expect_false(sol.isValid("}}}"));
  });
}

}  // namespace
