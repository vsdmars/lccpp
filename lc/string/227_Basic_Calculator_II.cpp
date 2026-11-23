// LeetCode 227: Basic Calculator II
// https://leetcode.com/problems/basic-calculator-ii/
//
// Problem:
//   Given a string `s` representing a valid arithmetic expression containing
//   non-negative integers, binary operators (`+`, `-`, `*`, `/`), and spaces,
//   evaluate the expression and return its integer result. Integer division
//   must truncate toward zero, and built-in expression evaluators are
//   disallowed.
//
// Algorithm:
//   Operator precedence requires evaluating `*` and `/` before `+` and `-`.
//   We scan `s` left to right while accumulating the current multi-digit
//   operand and remembering the previous operator `pending_op` (initialized to
//   `'+'`). Whenever a new operator is encountered -- or at a virtual trailing
//   `'+'` sentinel after the last character -- we apply `pending_op` to the
//   completed operand:
//     - `'+'`: push `+current` onto the stack of additive terms.
//     - `'-'`: push `-current` onto the stack of additive terms.
//     - `'*'`: multiply the top term by `current` in place.
//     - `'/'`: divide the top term by `current` in place (C++ signed integer
//       division naturally truncates toward zero, fixing Python's floor `//`
//       pitfall on negative terms).
//   After the pass, summing all deferred additive terms yields the final value.
//   An O(1)-space variant (`calculateConstantSpace`) replaces the stack with
//   two running accumulators (`result` and `last_term`).
//   Time Complexity:  O(n)
//   Space Complexity: O(n) for the stack version; O(1) for the streaming variant
//
// Data Structures:
//   - `std::vector<std::int64_t>`: term stack holding signed additive terms
//     after high-precedence `*` and `/` contractions.
//   - `std::int64_t`: 64-bit accumulators to prevent intermediate 32-bit signed
//     overflow while parsing large operands.
//
// Real-World Applications:
//   - Expression evaluation in spreadsheet engines, query filters, and
//     configuration DSLs without constructing a full AST.
//   - Precedence climbing / operator-precedence parsing in compilers and
//     interpreters.
//   - Embedded calculators and financial formula evaluators requiring
//     deterministic truncation semantics.
//
// Related LeetCode Problems:
//   - #224 Basic Calculator [done] (adds parentheses and unary minus via an explicit sign stack)
//   - #772 Basic Calculator III [done] (combines `+`, `-`, `*`, `/` with nested parentheses)
//   - #150 Evaluate Reverse Polish Notation [new] (postfix expression evaluation with a value stack)
//   - #282 Expression Add Operators [done] (inserts `+`, `-`, `*` with deferred multiplication tracking)
//   - #394 Decode String [done] (stack-based parsing of nested string/number expressions)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Evaluates a basic arithmetic expression with `+`, `-`, `*`, `/` using a
  // stack of contracted additive terms (mirrors the Python "contract" design).
  // Time: O(n), Space: O(n).
  int calculate(std::string_view s) {
    std::vector<std::int64_t> terms;
    std::int64_t current = 0;
    char pending_op = '+';

    // Run one extra iteration (`i == s.size()`) with a virtual '+' sentinel so
    // the final parsed operand is committed via the exact same code path.
    for (std::size_t i = 0uz; i <= s.size(); ++i) {
      const char ch = (i < s.size()) ? s[i] : '+';
      if (ch == ' ') {
        continue;
      }
      if (ch >= '0' && ch <= '9') {
        current = (current * 10) + static_cast<std::int64_t>(ch - '0');
        continue;
      }

      // Contract high-precedence operators immediately; defer +/- onto `terms`.
      apply_op(terms, pending_op, current);
      pending_op = ch;
      current = 0;
    }

    std::int64_t result = 0;
    for (const std::int64_t term : terms) {
      result += term;
    }
    return static_cast<int>(result);
  }

  // Evaluates the same expression in O(1) auxiliary space by collapsing the
  // term stack into a running `total` plus the most recent active `last_term`.
  // Time: O(n), Space: O(1).
  int calculateConstantSpace(std::string_view s) {
    std::int64_t total = 0;
    std::int64_t last_term = 0;
    std::int64_t current = 0;
    char pending_op = '+';

    for (std::size_t i = 0uz; i <= s.size(); ++i) {
      const char ch = (i < s.size()) ? s[i] : '+';
      if (ch == ' ') {
        continue;
      }
      if (ch >= '0' && ch <= '9') {
        current = (current * 10) + static_cast<std::int64_t>(ch - '0');
        continue;
      }

      if (pending_op == '+' || pending_op == '-') {
        // Previous multiplicative chain is complete; fold it into `total`.
        total += last_term;
        last_term = (pending_op == '+') ? current : -current;
      } else if (pending_op == '*') {
        last_term *= current;
      } else if (pending_op == '/' && current != 0) {
        last_term /= current;
      }

      pending_op = ch;
      current = 0;
    }

    return static_cast<int>(total + last_term);
  }

 private:
  // Applies `op` with right-hand operand `value` to the term stack.
  static void apply_op(std::vector<std::int64_t>& terms, char op,
                       std::int64_t value) {
    switch (op) {
      case '+':
        terms.push_back(value);
        break;
      case '-':
        terms.push_back(-value);
        break;
      case '*':
        if (!terms.empty()) {
          terms.back() *= value;
        }
        break;
      case '/':
        if (!terms.empty() && value != 0) {
          terms.back() /= value;
        }
        break;
      default:
        break;
    }
  }
};

LC_PROBLEM(227, "Basic Calculator II", "string", ctx) {
  Solution sol;

  auto check_both = [&](std::string_view expr, int expected) {
    ctx.expect_eq(sol.calculate(expr), expected);
    ctx.expect_eq(sol.calculateConstantSpace(expr), expected);
  };

  ctx.test("example_1_precedence", [&] { check_both("3+2*2", 7); });

  ctx.test("example_2_spaces_and_division", [&] { check_both(" 3/2 ", 1); });

  ctx.test("example_3_mixed_spaces", [&] { check_both(" 3+5 / 2 ", 5); });

  ctx.test("negative_term_truncates_toward_zero", [&] {
    // 14 - (3 / 2) = 14 - 1 = 13 (NOT floor(-3 / 2) = -2).
    check_both("14-3/2", 13);
    check_both("0-5/2", -2);
  });

  ctx.test("chained_mul_and_div_left_associative", [&] {
    check_both("100/10/2*3", 15);
    check_both("2*3*4-10/3+1", 22);
  });

  ctx.test("single_and_large_numbers", [&] {
    check_both("42", 42);
    check_both("  0  ", 0);
    check_both("2147483647", 2147483647);
  });
}

}  // namespace
