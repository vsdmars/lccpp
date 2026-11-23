// LeetCode 772: Basic Calculator III
// https://leetcode.com/problems/basic-calculator-iii/
//
// Problem:
//   Evaluate a valid mathematical expression string `s` containing non-negative
//   integers, the binary operators `+`, `-`, `*`, `/`, nested parentheses `(`
//   and `)`, and optional whitespace. Standard operator precedence applies
//   (`*` and `/` before `+` and `-`), integer division truncates toward zero,
//   and all intermediate results fit within 32-bit signed integers.
//
// Examples:
//   Example 1:
//     Input:  s = "2*(5+5*2)/3+(6/2+8)"
//     Output: 21
//     Explanation:
//       - Read 2 then * and (: push 2 onto terms = [2], save frame {op = '*', base
//         = 1}, and reset prev_op = '+' and num = 0.
//       - Inside 5+5*2): apply_op builds terms = [2, 5, 10], and on )
//         collapse_frame(terms, 1) sums 5 + 10 = 15 into num and restores prev_op =
//         '*' with terms = [2].
//       - At / then 3 then +: prev_op = '*' multiplies terms.back() to 2 * 15 = 30,
//         then prev_op = '/' divides 30 / 3 = 10, leaving terms = [10] and prev_op
//         = '+'.
//       - At the second (: push frame {op = '+', base = 1}, evaluate 6/2+8 into
//         terms = [10, 3, 8], and collapse on ) to num = 11 with prev_op = '+' and
//         terms = [10].
//       - The virtual trailing '+' sentinel pushes +11 onto terms = [10, 11], and
//         collapse_frame(terms, 0) returns 21.
//
//   Example 2:
//     Input:  s = " 6 - 4 / 24 + 10"
//     Output: 16
//     Explanation:
//       - Skip the leading space, read 6, and on - apply prev_op = '+' to push 6
//         onto terms = [6] while setting prev_op = '-'.
//       - Read 4 and on / apply prev_op = '-' to push -4 onto terms = [6, -4] while
//         setting prev_op = '/'.
//       - Read 24 and on + apply prev_op = '/' so integer division truncates
//         terms.back() to -4 / 24 = 0, leaving terms = [6, 0] and prev_op = '+'.
//       - Read 10 and on the trailing '+' sentinel push +10 onto terms = [6, 0,
//         10], summing via collapse_frame(terms, 0) to 16.
//
//   Example 3:
//     Input:  s = "-(2 + 3) * 4"
//     Output: -20
//     Explanation:
//       - At -, apply initial prev_op = '+' to push num = 0 onto terms = [0] and
//         set prev_op = '-'.
//       - At (, push frame {op = '-', base = 1} and evaluate 2 + 3 inside to
//         produce terms = [0, 2, 3].
//       - At ), collapse_frame(terms, 1) sums 2 + 3 into num = 5 and restores
//         prev_op = '-', leaving terms = [0].
//       - At *, apply prev_op = '-' to push -5 onto terms = [0, -5]; the trailing
//         '+' sentinel multiplies terms.back() by 4 to yield terms = [0, -20],
//         which sums to -20.
//
// Algorithm:
//   Single-pass deferred-operator stack evaluation with explicit parenthesis
//   frames. Maintain a running operand `num`, the pending operator `prev_op`
//   immediately preceding `num` (initialized to `+`), a flat `terms` stack of
//   signed additive terms, and a `frames` stack recording `{saved_op, base}`
//   whenever `(` opens a subexpression.
//   - Digits accumulate into `num`.
//   - On `(`, push `{prev_op, terms.size()}` onto `frames`, then reset
//     `prev_op = '+'` and `num = 0` for the inner subexpression.
//   - On any operator (`+`, `-`, `*`, `/`), `)`, or a virtual trailing `+`
//     sentinel at end-of-input, apply `prev_op` to `num` against `terms`:
//     `+` pushes `+num`, `-` pushes `-num`, `*` multiplies `terms.back()`,
//     and `/` divides `terms.back()` toward zero.
//   - If the trigger was `)`, pop the matching frame `{saved_op, base}`, sum
//     and pop all terms from `base` upward into `num`, and restore
//     `prev_op = saved_op` so the subsequent operator (or outer `)`/sentinel)
//     combines the parenthesized value with the enclosing expression.
//   - Otherwise record the new `prev_op` and reset `num = 0`.
//   Finally, sum all remaining values in `terms`.
//   Time Complexity: O(N), where N = s.size(); every character is scanned once
//   and each term is pushed and popped at most once.
//   Space Complexity: O(N) in the worst case for the term and frame stacks.
//
// Data Structures:
//   - `std::vector<std::int64_t> terms`: flat value stack storing signed
//     additive terms (64-bit to prevent signed overflow on intermediate values
//     such as -2^31).
//   - `std::vector<Frame> frames`: parallel frame stack storing each open
//     parenthesis's saved preceding operator and starting index in `terms`.
//
// Real-World Applications:
//   - Spreadsheet formula engines (Excel, Google Sheets) evaluating nested
//     arithmetic expressions with operator precedence.
//   - Compiler front-ends and scripting interpreters implementing operator-
//     precedence parsing and constant folding.
//   - Financial calculators, query-engine filter evaluators, and rule engines.
//
// Related LeetCode Problems:
//   - #224 Basic Calculator [done] (parentheses with + and - only)
//   - #227 Basic Calculator II [done] (+, -, *, / precedence without parentheses)
//   - #150 Evaluate Reverse Polish Notation [new] (postfix expression evaluation)
//   - #770 Basic Calculator IV [new] (symbolic polynomial expression evaluator)
//   - #1597 Build Binary Expression Tree From Infix Expression [new] (AST version)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Evaluates the infix arithmetic expression `s` supporting +, -, *, /,
  // arbitrary parenthesis nesting, and spaces.
  // Invariant: after each operator or closing parenthesis, every completed
  // multiplicative term inside the current parenthesis frame has been folded
  // into `terms`, so summing `terms[frame.base..]` yields the frame's value.
  // Time: O(N), Space: O(N).
  int calculate(std::string_view s) {
    std::vector<std::int64_t> terms;
    std::vector<Frame> frames;

    std::int64_t num = 0;
    char prev_op = '+';

    // Iterate one step past the end with a virtual '+' sentinel so the final
    // operand (or closing parenthesis value) is always flushed into `terms`.
    for (std::size_t i = 0uz; i <= s.size(); ++i) {
      const char ch = (i < s.size()) ? s[i] : '+';

      if (ch == ' ') {
        continue;
      }

      if (is_digit(ch)) {
        num = (num * 10) + static_cast<std::int64_t>(ch - '0');
        continue;
      }

      if (ch == '(') {
        // Save the operator waiting outside '(' and mark where this frame's
        // additive terms begin, then start a clean inner subexpression.
        frames.emplace_back(prev_op, terms.size());
        prev_op = '+';
        num = 0;
        continue;
      }

      if (is_operator_or_close(ch)) {
        // Flush the completed operand using the operator immediately before it.
        apply_op(terms, prev_op, num);

        if (ch == ')') {
          // Collapse all additive terms belonging to the closing parenthesis
          // into a single operand `num` and restore the outer operator so the
          // next token combines `num` into the enclosing frame.
          const Frame frame = frames.back();
          frames.pop_back();
          num = collapse_frame(terms, frame.base);
          prev_op = frame.op;
        } else {
          prev_op = ch;
          num = 0;
        }
      }
    }

    return static_cast<int>(collapse_frame(terms, 0uz));
  }

 private:
  // Captures the state of an enclosing expression when entering '('.
  struct Frame {
    char op = '+';
    std::size_t base = 0uz;
  };

  static constexpr bool is_digit(char ch) noexcept {
    return ch >= '0' && ch <= '9';
  }

  static constexpr bool is_operator_or_close(char ch) noexcept {
    return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == ')';
  }

  // Applies `op` to `val` against the top of `terms`.
  // '+' and '-' push signed additive terms; '*' and '/' bind immediately to
  // the preceding term to honor higher multiplicative precedence.
  static void apply_op(std::vector<std::int64_t>& terms, char op,
                       std::int64_t val) {
    if (op == '+') {
      terms.push_back(val);
    } else if (op == '-') {
      terms.push_back(-val);
    } else if (op == '*') {
      terms.back() *= val;
    } else if (op == '/') {
      // C++ signed integer division truncates toward zero as required.
      terms.back() /= val;
    }
  }

  // Sums and removes all terms from index `base` to the end of `terms`.
  static std::int64_t collapse_frame(std::vector<std::int64_t>& terms,
                                     std::size_t base) {
    std::int64_t sum = 0;
    while (terms.size() > base) {
      sum += terms.back();
      terms.pop_back();
    }
    return sum;
  }
};

LC_PROBLEM(772, "Basic Calculator III", "stack", ctx) {
  Solution sol;

  ctx.test("simple addition", [&] { ctx.expect_eq(sol.calculate("1 + 1"), 2); });

  ctx.test("division truncating to zero",
           [&] { ctx.expect_eq(sol.calculate(" 6 - 4 / 24 + 10"), 16); });

  ctx.test("nested parentheses and precedence", [&] {
    ctx.expect_eq(sol.calculate("2*(5+5*2)/3+(6/2+8)"), 21);
    ctx.expect_eq(sol.calculate("(2+6* 3+5- (3*14/7+2)*5)+3"), -12);
  });

  ctx.test("flat precedence without parentheses",
           [&] { ctx.expect_eq(sol.calculate("30 + 50/2 + 15"), 70); });

  ctx.test("negative division truncation and unary minus", [&] {
    ctx.expect_eq(sol.calculate("0 - 7 / 2"), -3);
    ctx.expect_eq(sol.calculate("-(2 + 3) * 4"), -20);
    ctx.expect_eq(sol.calculate("2 * (-3 + 5)"), 4);
    ctx.expect_eq(sol.calculate("1 - (-2)"), 3);
  });

  ctx.test("deeply nested and single number", [&] {
    ctx.expect_eq(sol.calculate("((1 + 2))"), 3);
    ctx.expect_eq(sol.calculate("42"), 42);
    ctx.expect_eq(sol.calculate("0"), 0);
  });
}

}  // namespace
