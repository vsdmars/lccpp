// LeetCode 241: Different Ways To Add Parentheses
// https://leetcode.com/problems/different-ways-to-add-parentheses/
//
// Problem:
//   Given a string `expression` consisting of non-negative integers and the
//   binary operators '+', '-', and '*', return all possible results from
//   evaluating every valid parenthesization of the expression. Results may be
//   returned in any order, and duplicate values produced by distinct
//   parenthesizations must be preserved. Inputs have length at most 20 and all
//   output values fit in a signed 32-bit integer.
//
// Algorithm:
//   Divide-and-conquer DFS with memoization over sub-expression slices. Every
//   operator index `i` can serve as the root (last-evaluated operator) of an
//   expression tree, splitting `expression` into left and right sub-expressions
//   `expression[0..i-1]` and `expression[i+1..n-1]`. Recursively compute all
//   possible results for both halves and take their Cartesian product under
//   `expression[i]`. When a slice contains no operators, it is a base-case
//   integer literal. Memoizing by `std::string_view` avoids re-solving
//   identical sub-expressions and avoids substring allocations.
//   Time Complexity: O(n * C_m), where m is the number of operators and C_m is
//     the m-th Catalan number bounding the number of distinct parenthesizations.
//   Space Complexity: O(n^2 * C_m) in the worst case for the memoization table
//     across O(n^2) slices, plus O(n) recursion stack depth.
//
// Data Structures:
//   - std::unordered_map<std::string_view, std::vector<int>>: memoizes the
//     evaluated results for each visited sub-expression slice without copying
//     substrings.
//   - std::vector<int>: stores the multiset of evaluation outcomes at each
//     divide-and-conquer level.
//
// Real-World Applications:
//   - Compiler parser generators and ambiguity analysis when enumerating all
//     parse trees of an ambiguous context-free grammar.
//   - Database query optimizers exploring alternative join association trees
//     (bushy join plans) to evaluate cost variations.
//   - Computer algebra systems and symbolic differentiation tools analyzing
//     non-associative operator groupings (e.g., floating-point rounding bounds).
//
// Related LeetCode Problems:
//   - #95 Unique Binary Search Trees II [done] (Catalan-structured recursive tree enumeration)
//   - #282 Expression Add Operators [done] (backtracking over expression splits and operators)
//   - #224 Basic Calculator [done] (evaluating expressions with explicit parentheses)
//   - #312 Burst Balloons [done] (interval DP choosing the last split point in a range)
//   - #1130 Minimum Cost Tree From Leaf Values [new] (optimizing over all binary parenthesizations)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Computes all results obtainable by parenthesizing `expression` in every
  // valid way.
  //
  // Invariant: Each recursive call evaluates a contiguous non-empty
  // sub-expression `expr` and caches its full list of outcomes (including
  // duplicates from distinct parse trees) in `memo`.
  //
  // Time: O(n * C_m) where m is the operator count and C_m is the m-th Catalan
  // number; Space: O(n^2 * C_m) for memoized sub-expression results.
  std::vector<int> diffWaysToCompute(std::string_view expression) {
    std::unordered_map<std::string_view, std::vector<int>> memo;

    auto dfs = [&memo](this auto&& self,
                       std::string_view expr) -> std::vector<int> {
      // Return cached results if this exact sub-expression was already solved.
      if (const auto it = memo.find(expr); it != memo.end()) {
        return it->second;
      }

      std::vector<int> results;

      // Try each operator as the top-level (last-evaluated) split point.
      for (std::size_t i = 0uz; i < expr.size(); ++i) {
        const char op = expr[i];
        if (!isOperator(op)) {
          continue;
        }

        const std::vector<int> left_vals = self(expr.substr(0uz, i));
        const std::vector<int> right_vals = self(expr.substr(i + 1uz));

        // Combine every left sub-expression outcome with every right outcome.
        for (const int lhs : left_vals) {
          for (const int rhs : right_vals) {
            results.push_back(applyOp(lhs, rhs, op));
          }
        }
      }

      // Base case: no operator was found, so `expr` is a single integer token.
      if (results.empty()) {
        results.push_back(parseNonNegativeInt(expr));
      }

      memo.emplace(expr, results);
      return results;
    };

    return dfs(expression);
  }

 private:
  // Returns true if `ch` is one of the supported binary arithmetic operators.
  static constexpr bool isOperator(char ch) noexcept {
    return ch == '+' || ch == '-' || ch == '*';
  }

  // Evaluates a single binary arithmetic operation.
  static constexpr int applyOp(int lhs, int rhs, char op) noexcept {
    if (op == '+') {
      return lhs + rhs;
    }
    if (op == '-') {
      return lhs - rhs;
    }
    return lhs * rhs;
  }

  // Parses a non-empty ASCII digit slice into a non-negative 32-bit integer.
  static constexpr int parseNonNegativeInt(std::string_view digits) noexcept {
    int value = 0;
    for (const char ch : digits) {
      value = (value * 10) + (ch - '0');
    }
    return value;
  }
};

// Sorts a result vector for order-independent comparison in unit tests.
std::vector<int> sortedCopy(std::vector<int> values) {
  std::ranges::sort(values);
  return values;
}

LC_PROBLEM(241, "Different Ways To Add Parentheses", "dfs", ctx) {
  ctx.test("example1_subtraction", [&] {
    Solution sol;
    ctx.expect_eq(sortedCopy(sol.diffWaysToCompute("2-1-1")),
                  std::vector<int>{0, 2});
  });

  ctx.test("example2_mixed_operators", [&] {
    Solution sol;
    ctx.expect_eq(sortedCopy(sol.diffWaysToCompute("2*3-4*5")),
                  std::vector<int>{-34, -14, -10, -10, 10});
  });

  ctx.test("single_zero", [&] {
    Solution sol;
    ctx.expect_eq(sol.diffWaysToCompute("0"), std::vector<int>{0});
  });

  ctx.test("multi_digit_literal", [&] {
    Solution sol;
    ctx.expect_eq(sol.diffWaysToCompute("42"), std::vector<int>{42});
  });

  ctx.test("multi_digit_with_operators", [&] {
    Solution sol;
    // ((11 - 2) - 3) = 6, (11 - (2 - 3)) = 12
    ctx.expect_eq(sortedCopy(sol.diffWaysToCompute("11-2-3")),
                  std::vector<int>{6, 12});
  });

  ctx.test("preserves_duplicate_outcomes", [&] {
    Solution sol;
    // Both ((2 + 3) + 4) and (2 + (3 + 4)) yield 9.
    ctx.expect_eq(sol.diffWaysToCompute("2+3+4"), std::vector<int>{9, 9});
  });
}

}  // namespace
