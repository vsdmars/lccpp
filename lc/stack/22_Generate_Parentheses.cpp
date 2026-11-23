// LeetCode 22: Generate Parentheses
// https://leetcode.com/problems/generate-parentheses/
//
// Problem:
//   Given an integer `n` representing `n` pairs of parentheses, generate all
//   combinations of well-formed (balanced) parentheses of length `2 * n`.
//
// Algorithm:
//   Use depth-first backtracking with an explicit character stack `current`
//   and two remaining-budget counters: `open_rem` (number of '(' left to
//   place) and `close_rem` (number of ')' left to place).
//   Key invariant: at every prefix, the number of ')' placed never exceeds
//   the number of '(' placed, which is equivalent to `0 <= open_rem <=
//   close_rem`.
//   - Whenever `open_rem > 0`, we may safely push '(', recurse with
//     `(open_rem - 1, close_rem)`, and pop on backtrack.
//   - Whenever `close_rem > open_rem`, at least one '(' is currently unclosed,
//     so we may safely push ')', recurse with `(open_rem, close_rem - 1)`, and
//     pop on backtrack.
//   - When both counters reach 0, `current` is a complete valid string and is
//     appended to `result`.
//   Because invalid prefixes are pruned immediately, every leaf corresponds to
//   a unique valid Catalan sequence.
//   Time complexity: O(n * C_n) = O(4^n / sqrt(n)), where C_n is the n-th
//   Catalan number (output-size bound).
//   Space complexity: O(n) for recursion depth and the mutable path buffer,
//   excluding the returned output vector.
//
// Data Structures:
//   - std::string acting as a mutable character stack (push_back / pop_back)
//     for the current prefix being explored.
//   - std::vector<std::string> collecting all valid well-formed combinations.
//
// Real-World Applications:
//   - Enumerating valid parenthesizations for matrix-chain multiplication and
//     expression-tree generation in compilers and query optimizers.
//   - Exhaustive fuzzing and property-based test-case generation for parsers
//     handling nested grammars (Lisp, SQL subqueries, JSON, ASTs).
//   - Enumerating rooted ordered trees and Dyck paths in combinatorics and
//     bioinformatics (RNA secondary-structure folding).
//
// Related LeetCode Problems:
//   - #20 Valid Parentheses [done] (validate a given bracket string)
//   - #17 Letter Combinations of a Phone Number [done] (backtracking generation)
//   - #32 Longest Valid Parentheses [done] (longest valid bracket substring)
//   - #301 Remove Invalid Parentheses [done] (minimal edits to reach validity)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <vector>

namespace {

class Solution {
 public:
  // Generates all well-formed strings consisting of `n` pairs of parentheses.
  // Time: O(n * C_n), Space: O(n) auxiliary stack space.
  std::vector<std::string> generateParenthesis(int n) {
    if (n <= 0) {
      return {};
    }

    std::vector<std::string> result;
    // Mutable character stack holding the current prefix along the DFS path.
    std::string current;
    current.reserve(static_cast<std::size_t>(n) * 2uz);

    // Recursive DFS maintaining invariant `0 <= open_rem <= close_rem`.
    auto dfs = [&](this auto&& self, int open_rem, int close_rem) -> void {
      // Base case: all n opening and n closing parentheses have been placed.
      if (open_rem == 0 && close_rem == 0) {
        result.push_back(current);
        return;
      }

      // Branch 1: place '(' whenever opening budget remains.
      if (open_rem > 0) {
        current.push_back('(');
        self(open_rem - 1, close_rem);
        current.pop_back();
      }

      // Branch 2: place ')' only when more '(' than ')' have been placed so
      // far (equivalently, strictly more ')' remain than '(').
      if (close_rem > open_rem) {
        current.push_back(')');
        self(open_rem, close_rem - 1);
        current.pop_back();
      }
    };

    dfs(n, n);
    return result;
  }
};

LC_PROBLEM(22, "Generate Parentheses", "stack", ctx) {
  Solution sol;

  ctx.test("n_equals_3_example", [&] {
    const std::vector<std::string> expected = {
        "((()))", "(()())", "(())()", "()(())", "()()()",
    };
    ctx.expect_eq(sol.generateParenthesis(3), expected);
  });

  ctx.test("n_equals_1_and_2", [&] {
    ctx.expect_eq(sol.generateParenthesis(1), std::vector<std::string>{"()"});
    ctx.expect_eq(sol.generateParenthesis(2),
                  std::vector<std::string>{"(())", "()()"});
  });

  ctx.test("catalan_counts_and_zero", [&] {
    ctx.expect_true(sol.generateParenthesis(0).empty());
    // C_4 = 14, C_5 = 42.
    ctx.expect_eq(sol.generateParenthesis(4).size(), 14uz);
    ctx.expect_eq(sol.generateParenthesis(5).size(), 42uz);
  });
}

}  // namespace
