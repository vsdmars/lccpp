// LeetCode 856: Score Of Parentheses
// https://leetcode.com/problems/score-of-parentheses/
//
// Problem:
//   Given a balanced parentheses string `s` consisting only of `(` and `)`,
//   compute its score according to three recursive rules:
//   - `"()"` has score 1.
//   - `AB` has score `score(A) + score(B)` for balanced substrings `A` and `B`.
//   - `(A)` has score `2 * score(A)` for a balanced substring `A`.
//
// Algorithm:
//   1) Depth-Frame Stack (`scoreOfParentheses`):
//      Maintain a stack `scores` where each entry holds the accumulated score
//      of the currently open nesting depth, initialized with `{0}` for the
//      top-level scope.
//      - On `(`, push `0` to begin accumulating a new nested scope.
//      - On `)`, pop the inner scope's accumulated value `inner`. If
//        `inner == 0`, the closing parenthesis completed an atomic `"()"`
//        contributing `1`; otherwise it wrapped a non-empty balanced expression
//        contributing `2 * inner`. Add `std::max(2 * inner, 1)` to the new
//        stack top (the enclosing scope).
//      At the end, `scores.front()` holds the total score.
//      Time Complexity: O(N), where N = s.size().
//      Space Complexity: O(N) worst case for deeply nested parentheses.
//   2) O(1)-Space Contribution Counting (`scoreOfParenthesesO1Space`):
//      Because multiplication distributes over addition (`2 * (A + B) =
//      2 * A + 2 * B`), every atomic `"()"` core nested at depth `d`
//      contributes `2^d` to the total score. Track nesting depth in one pass
//      and add `1 << depth` whenever `s[i - 1] == '('` and `s[i] == ')'`.
//      Time Complexity: O(N), Space Complexity: O(1).
//
// Data Structures:
//   - `std::vector<int> scores`: explicit call-frame stack holding the running
//     partial score at each active parenthesis nesting depth.
//
// Real-World Applications:
//   - Nested-scope cost and weight aggregation in AST / S-expression
//     interpreters (e.g., loop-nest execution-weight estimation in compilers).
//   - Hierarchical markup and XML/JSON tree folding without materializing the
//     full parse tree.
//   - Binary-tree depth weighting and Huffman-like path-length calculations.
//
// Related LeetCode Problems:
//   - #20 Valid Parentheses [done] (foundational bracket-matching stack)
//   - #32 Longest Valid Parentheses [done] (balanced span measurement)
//   - #921 Minimum Add To Make Parentheses Valid [done] (balance counting)
//   - #1021 Remove Outermost Parentheses [new] (depth-tracking decomposition)
//   - #1614 Maximum Nesting Depth of the Parentheses [done] (depth tracking)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the score of balanced parentheses string `s` using an explicit
  // stack of per-depth score accumulators.
  // Invariant: `scores.size() - 1` equals the current nesting depth, and
  // `scores[d]` holds the sum of completed sibling subexpressions at depth `d`.
  // Time: O(N), Space: O(N).
  int scoreOfParentheses(std::string_view s) {
    // Seed depth 0 with 0 to accumulate all top-level sibling groups.
    std::vector<int> scores{0};

    for (const char ch : s) {
      if (ch == '(') {
        // Enter a new nested frame with initial inner score 0.
        scores.push_back(0);
      } else {
        // Close the current frame: an empty inner frame "()" scores 1,
        // while a non-empty frame "(A)" doubles its inner score.
        const int inner = scores.back();
        scores.pop_back();
        scores.back() += std::max(2 * inner, 1);
      }
    }

    return scores.front();
  }

  // O(1)-space alternative using distributivity: each atomic "()" enclosed by
  // `depth` outer pairs contributes 2^depth to the final score.
  // Time: O(N), Space: O(1).
  int scoreOfParenthesesO1Space(std::string_view s) {
    static constexpr unsigned kMaxShift = 30U;

    int total = 0;
    unsigned depth = 0U;

    for (std::size_t i = 0uz; i < s.size(); ++i) {
      if (s[i] == '(') {
        ++depth;
      } else if (depth > 0U) {
        --depth;
        // Only innermost "()" cores generate base score 1, scaled by 2^depth.
        // Bounding `depth <= kMaxShift` guarantees the shift stays within
        // signed 32-bit range (valid LeetCode inputs have length <= 50).
        if (i > 0uz && s[i - 1uz] == '(' && depth <= kMaxShift) {
          total += static_cast<int>(1U << depth);
        }
      }
    }

    return total;
  }
};

LC_PROBLEM(856, "Score Of Parentheses", "stack", ctx) {
  Solution sol;

  auto check_both = [&](std::string_view s, int expected) {
    ctx.expect_eq(sol.scoreOfParentheses(s), expected);
    ctx.expect_eq(sol.scoreOfParenthesesO1Space(s), expected);
  };

  ctx.test("basic examples", [&] {
    check_both("()", 1);
    check_both("(())", 2);
    check_both("()()", 2);
  });

  ctx.test("nested and sibling combinations", [&] {
    check_both("(()(()))", 6);
    check_both("(())()(())", 5);
    check_both("((()()))", 8);
  });

  ctx.test("deep nesting", [&] {
    check_both("((((()))))", 16);
    check_both("()()()()()", 5);
  });
}

}  // namespace
