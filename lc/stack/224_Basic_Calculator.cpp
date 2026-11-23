// LeetCode 224: Basic Calculator
// https://leetcode.com/problems/basic-calculator/
//
// Problem:
//   Given a valid mathematical expression string `s` containing non-negative
//   integers, '+' and '-' operators (including unary '-'), parentheses '('
//   and ')', and spaces, evaluate the expression and return its integer value
//   without calling a built-in expression evaluator.
//
// Examples:
//   Example 1:
//     Input:  s = "(1+(4+5+2)-3)+(6+8)"
//     Output: 23
//     Explanation:
//       - The outer '(' pushes {prev_result: 0, prev_sign: 1} onto frames; parsing
//         "1+" updates result = 1 and sign = 1 before the inner '(' pushes
//         {prev_result: 1, prev_sign: 1}.
//       - Inside "(4+5+2)", result accumulates 4 + 5 = 9 with cur = 2, and its ')'
//         folds 1 + 1 * (9 + 2) = 12 back into the first parenthesized scope.
//       - Parsing "-3)" applies -3 to get result = 12 - 3 = 9, then pops {0, 1} to
//         fold result = 0 + 1 * 9 = 9 at the top level.
//       - "+(6+8)" pushes {prev_result: 9, prev_sign: 1}, evaluates the inner scope
//         to 6 + 8 = 14, and folds 9 + 1 * 14 = 23 on the final ')'.
//
//   Example 2:
//     Input:  s = "- (3 + (4 + 5))"
//     Output: -12
//     Explanation:
//       - Leading '-' commits 0 * 1 = 0 and sets sign = -1, then the outer '('
//         saves {prev_result: 0, prev_sign: -1} on frames and resets result = 0,
//         sign = 1.
//       - "3 +" updates result = 3, and the nested '(' pushes {prev_result: 3,
//         prev_sign: 1} before resetting the inner scope.
//       - "4 + 5)" completes the inner sum 9 and pops {3, 1} to compute result = 3
//         + 1 * 9 = 12.
//       - The outer ')' pops {0, -1} and folds 0 + (-1) * 12 = -12, which is
//         returned directly.
//
//   Example 3:
//     Input:  s = "-2147483648"
//     Output: -2147483648
//     Explanation:
//       - Leading '-' commits 0 * 1 = 0 and sets sign = -1 with empty frames.
//       - Digits '2' through '8' accumulate cur = 2147483648 in 64-bit std::int64_t
//         arithmetic, avoiding signed 32-bit overflow.
//       - After the loop, result + (cur * sign) evaluates to 0 + 2147483648 * (-1)
//         = -2147483648, which converts cleanly to int (-2147483648).
//
// Algorithm:
//   Evaluate the expression in a single left-to-right pass using an explicit
//   scope stack for parenthesized subexpressions. Maintain three scalars for
//   the active parenthesis scope:
//   - `result`: running sum of completed terms in the current scope.
//   - `cur`: multi-digit integer currently being parsed.
//   - `sign`: +1 or -1 sign applying to `cur` (or to the next '(' group).
//   Use 64-bit signed arithmetic internally so boundary literals such as
//   `-2147483648` never cause 32-bit signed overflow during digit parsing.
//   Transitions per character:
//   - Digit: accumulate `cur = cur * 10 + (ch - '0')`.
//   - '+' or '-': commit `result += cur * sign`, reset `cur = 0`, and set
//     `sign` to +1 or -1.
//   - '(': push `{result, sign}` onto `frames` to save the enclosing scope,
//     then reset `result = 0` and `sign = 1` for the inner scope.
//   - ')': finish the inner scope (`result += cur * sign`, `cur = 0`), pop the
//     enclosing `{prev_result, prev_sign}`, and fold:
//     `result = prev_result + prev_sign * result`.
//   - Space: ignore.
//   After the loop, commit any trailing number: `result + cur * sign`.
//   Time complexity: O(n), where n = s.size().
//   Space complexity: O(n) in the worst case for deeply nested parentheses.
//
// Data Structures:
//   - std::vector<ScopeFrame> acting as a LIFO call/scope stack saving each
//     enclosing expression's accumulated sum and leading sign before '('.
//
// Real-World Applications:
//   - Spreadsheet formula engines (Excel, Google Sheets) and configuration
//     expression evaluators.
//   - Compiler constant-folding passes and recursive-descent / shift-reduce
//     arithmetic parsers.
//   - Financial calculators, query-filter evaluators, and DSL interpreters.
//
// Related LeetCode Problems:
//   - #20 Valid Parentheses [done] (nested parenthesis tracking via stack)
//   - #150 Evaluate Reverse Polish Notation [new] (postfix stack evaluation)
//   - #227 Basic Calculator II [done] (operator precedence with '*' and '/')
//   - #772 Basic Calculator III [done] (all four operators plus parentheses)

#include "framework/lc_framework.h"

#include <cstdint>
#include <limits>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Evaluates a valid arithmetic expression containing '+', '-', '(', ')',
  // spaces, and integers. Time: O(n), Space: O(n).
  int calculate(std::string_view s) {
    // Stores the outer scope's partial result and sign right before '('.
    struct ScopeFrame {
      std::int64_t prev_result = 0;
      std::int64_t prev_sign = 1;
    };

    std::vector<ScopeFrame> frames;
    std::int64_t result = 0;
    std::int64_t cur = 0;
    std::int64_t sign = 1;

    for (const char ch : s) {
      if (ch >= '0' && ch <= '9') {
        // Accumulate multi-digit integer in 64-bit to avoid UB on INT_MIN.
        cur = (cur * 10) + static_cast<std::int64_t>(ch - '0');
      } else if (ch == '+' || ch == '-') {
        // Flush the preceding operand with its sign, then record the new sign.
        result += cur * sign;
        cur = 0;
        sign = (ch == '+') ? 1 : -1;
      } else if (ch == '(') {
        // Suspend current scope on the stack and start a fresh inner scope.
        frames.push_back(ScopeFrame{.prev_result = result, .prev_sign = sign});
        result = 0;
        sign = 1;
      } else if (ch == ')') {
        // Complete the inner scope and fold it back into the enclosing scope.
        result += cur * sign;
        cur = 0;
        const ScopeFrame outer = frames.back();
        frames.pop_back();
        result = outer.prev_result + (outer.prev_sign * result);
        sign = 1;
      }
      // Spaces are ignored.
    }

    // Flush any final operand at the end of the expression.
    return static_cast<int>(result + (cur * sign));
  }
};

LC_PROBLEM(224, "Basic Calculator", "stack", ctx) {
  Solution sol;

  ctx.test("leetcode_examples", [&] {
    ctx.expect_eq(sol.calculate("1 + 1"), 2);
    ctx.expect_eq(sol.calculate(" 2-1 + 2 "), 3);
    ctx.expect_eq(sol.calculate("(1+(4+5+2)-3)+(6+8)"), 23);
  });

  ctx.test("source_build_and_unary_minus", [&] {
    ctx.expect_eq(sol.calculate("- (3 + (4 + 5))"), -12);
    ctx.expect_eq(sol.calculate("1-(2+3)"), -4);
    ctx.expect_eq(sol.calculate("1-(     -2)"), 3);
    ctx.expect_eq(sol.calculate("-2+ 1"), -1);
  });

  ctx.test("large_numbers_and_deep_nesting", [&] {
    ctx.expect_eq(sol.calculate("2147483647"), std::numeric_limits<int>::max());
    ctx.expect_eq(sol.calculate("-2147483648"),
                  std::numeric_limits<int>::min());
    ctx.expect_eq(sol.calculate("(((10 - (3 - 5)) - 2))"), 10);
    ctx.expect_eq(sol.calculate("0"), 0);
  });
}

}  // namespace
