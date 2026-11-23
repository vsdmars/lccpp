// LeetCode 282: Expression Add Operators
// https://leetcode.com/problems/expression-add-operators/
//
// Problem:
//   Given a string `num` containing only digits '0'-'9' (1 <= num.size() <= 10)
//   and a 32-bit signed integer `target`, return all possibilities of inserting
//   binary operators '+', '-', or '*' between the digits of `num` so that the
//   resulting arithmetic expression evaluates to `target`. Operands must not
//   contain leading zeros (e.g., "05" is invalid, while "0" is valid), and
//   standard operator precedence applies ('*' binds tighter than '+' and '-').
//
// Algorithm:
//   Depth-first backtracking that partitions `num` from left to right into
//   valid integer operands and branches on the three binary operators '+', '-',
//   and '*' before every operand after the first. To evaluate '*' in O(1) time
//   without a separate expression parser, the recursion tracks both the running
//   expression value `curr` and the signed value of the most recently added
//   multiplicative term `last_term`:
//   - Adding `val`:      curr' = curr + val,                 last_term' = +val
//   - Subtracting `val`: curr' = curr - val,                 last_term' = -val
//   - Multiplying `val`: curr' = (curr - last_term) + (last_term * val),
//                        last_term' = last_term * val
//   Using 64-bit signed integers (`std::int64_t`) prevents overflow on 10-digit
//   operands and products. Note: unlike the naive pruning heuristic in the
//   Python draft (which incorrectly pruned multiplying by 0, e.g., "10", 0 ->
//   "1*0"), we perform exact backtracking over all valid splits.
//   Time Complexity: O(n * 4^n) — each of the n - 1 gaps has up to 4 choices
//     (concatenate, '+', '-', '*'), and emitting a valid expression costs O(n).
//   Space Complexity: O(n) auxiliary space for the recursion stack and mutable
//     expression buffer (excluding output storage).
//
// Data Structures:
//   - std::string: mutable path buffer updated in-place and restored via
//     `resize()` during backtracking to avoid repeated string allocations.
//   - std::vector<std::string>: collects all valid target-matching expressions.
//
// Real-World Applications:
//   - Program synthesis and symbolic regression searching for arithmetic
//     formulas that match observed numerical constraints.
//   - Automated puzzle solvers (e.g., Countdown numbers game, 24-game solvers)
//     and constraint-satisfaction search engines.
//   - Compiler constant-folding verification and fuzz testing of arithmetic
//     expression evaluators.
//
// Related LeetCode Problems:
//   - #241 Different Ways to Add Parentheses [done] (enumerating expression tree groupings)
//   - #227 Basic Calculator II [done] (evaluating '+', '-', '*', '/' with precedence)
//   - #224 Basic Calculator [done] (evaluating expressions with parentheses)
//   - #494 Target Sum [done] (assigning '+' and '-' signs to reach a target value)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Returns all valid expressions formed by inserting '+', '-', or '*' between
  // digits of `num` that evaluate to `target`.
  //
  // Invariant: At depth `idx`, `path` holds a valid prefix expression covering
  // `num[0..idx-1]`, `curr` is its exact value under standard precedence, and
  // `last_term` is the signed multiplicative term that immediately precedes the
  // next operator so '*' can retroactively bind to it.
  //
  // Time: O(n * 4^n); Space: O(n) auxiliary stack/buffer space.
  std::vector<std::string> addOperators(std::string_view num, int target) {
    std::vector<std::string> results;
    if (num.empty()) {
      return results;
    }

    std::string path;
    path.reserve(num.size() * 2uz);
    const auto target64 = static_cast<std::int64_t>(target);

    backtrack(num, target64, 0uz, 0, 0, path, results);
    return results;
  }

 private:
  // Explores every valid next operand starting at `idx`, updating `path` in
  // place and restoring its length after each branch.
  static void backtrack(std::string_view num, std::int64_t target,
                        std::size_t idx, std::int64_t curr,
                        std::int64_t last_term, std::string& path,
                        std::vector<std::string>& results) {
    if (idx == num.size()) {
      if (curr == target) {
        results.push_back(path);
      }
      return;
    }

    const std::size_t saved_len = path.size();
    std::int64_t operand = 0;

    for (std::size_t end = idx; end < num.size(); ++end) {
      // Multi-digit operands cannot have a leading '0' (e.g., "05" is invalid).
      if (end > idx && num[idx] == '0') {
        break;
      }

      operand = (operand * 10) + static_cast<std::int64_t>(num[end] - '0');
      const std::string_view token = num.substr(idx, (end - idx) + 1uz);

      if (idx == 0uz) {
        // First operand has no preceding binary operator.
        path.append(token);
        backtrack(num, target, end + 1uz, operand, operand, path, results);
        path.resize(saved_len);
        continue;
      }

      // Branch 1: Addition ('+').
      path.push_back('+');
      path.append(token);
      backtrack(num, target, end + 1uz, curr + operand, operand, path, results);
      path.resize(saved_len);

      // Branch 2: Subtraction ('-').
      path.push_back('-');
      path.append(token);
      backtrack(num, target, end + 1uz, curr - operand, -operand, path,
                results);
      path.resize(saved_len);

      // Branch 3: Multiplication ('*'). Undo `last_term` from `curr` and re-add
      // the higher-precedence product `last_term * operand`.
      path.push_back('*');
      path.append(token);
      const std::int64_t merged_term = last_term * operand;
      backtrack(num, target, end + 1uz, (curr - last_term) + merged_term,
                merged_term, path, results);
      path.resize(saved_len);
    }
  }
};

// Sorts string results for deterministic comparison in unit tests.
std::vector<std::string> sortedCopy(std::vector<std::string> values) {
  std::ranges::sort(values);
  return values;
}

LC_PROBLEM(282, "Expression Add Operators", "dfs", ctx) {
  ctx.test("example1_123_target_6", [&] {
    Solution sol;
    ctx.expect_eq(sortedCopy(sol.addOperators("123", 6)),
                  std::vector<std::string>{"1*2*3", "1+2+3"});
  });

  ctx.test("example2_232_target_8", [&] {
    Solution sol;
    ctx.expect_eq(sortedCopy(sol.addOperators("232", 8)),
                  std::vector<std::string>{"2*3+2", "2+3*2"});
  });

  ctx.test("example3_leading_zero_exclusion_105_target_5", [&] {
    Solution sol;
    ctx.expect_eq(sortedCopy(sol.addOperators("105", 5)),
                  std::vector<std::string>{"1*0+5", "10-5"});
  });

  ctx.test("example4_all_zeros", [&] {
    Solution sol;
    ctx.expect_eq(sortedCopy(sol.addOperators("00", 0)),
                  std::vector<std::string>{"0*0", "0+0", "0-0"});
  });

  ctx.test("multiply_by_zero_regression", [&] {
    Solution sol;
    // Ensures multiplying by trailing zero is not incorrectly pruned.
    ctx.expect_eq(sol.addOperators("10", 0), std::vector<std::string>{"1*0"});
  });

  ctx.test("example5_no_solution", [&] {
    Solution sol;
    ctx.expect_eq(sol.addOperators("3456237490", 9191),
                  std::vector<std::string>{});
  });

  ctx.test("int32_overflow_boundary", [&] {
    Solution sol;
    ctx.expect_eq(
        sol.addOperators("2147483648", std::numeric_limits<int>::min()),
        std::vector<std::string>{});
  });

  ctx.test("single_digit", [&] {
    Solution sol;
    ctx.expect_eq(sol.addOperators("0", 0), std::vector<std::string>{"0"});
  });
}

}  // namespace
