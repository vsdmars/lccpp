// LeetCode 921: Minimum Add To Make Parentheses Valid
// https://leetcode.com/problems/minimum-add-to-make-parentheses-valid/
//
// Problem:
//   Given a string `s` consisting solely of `(` and `)`, return the minimum
//   number of parentheses (`(` or `)`) that must be inserted at any positions
//   to make `s` a valid balanced parentheses string.
//
// Examples:
//   Example 1:
//     Input:  s = "())"
//     Output: 1
//     Explanation:
//       - Initialize need_left = 0 and need_right = 0.
//       - Index 0 ('(') increments need_right to 1, and index 1 (')') pairs with it
//         to decrement need_right back to 0.
//       - Index 2 (')') sees need_right == 0 and increments need_left to 1,
//         returning need_left + need_right = 1.
//
//   Example 2:
//     Input:  s = "()))(("
//     Output: 4
//     Explanation:
//       - Index 0 ('(') sets need_right = 1, and index 1 (')') matches it to reset
//         need_right = 0.
//       - Indices 2 and 3 (')') both encounter need_right == 0, incrementing
//         need_left twice to 2.
//       - Indices 4 and 5 ('(') increment need_right twice to 2, returning
//         need_left + need_right = 2 + 2 = 4.
//
//   Example 3:
//     Input:  s = ")("
//     Output: 2
//     Explanation:
//       - Initialize need_left = 0 and need_right = 0.
//       - Index 0 (')') finds need_right == 0 so no opening '(' is available to
//         pair with it, incrementing need_left to 1.
//       - Index 1 ('(') increments need_right to 1, returning need_left +
//         need_right = 1 + 1 = 2.
//
// Algorithm:
//   1) Greedy Balance Counters (`minAddToMakeValid`):
//      Scan `s` left-to-right tracking two counts:
//      - `need_right`: number of unmatched `(` seen so far that still require
//        a matching `)` to their right.
//      - `need_left`: number of unmatched `)` encountered when `need_right`
//        was already zero, each requiring a `(` inserted to its left.
//      On `(`, increment `need_right`. On `)`, if `need_right > 0`, pair it
//      with an earlier open parenthesis by decrementing `need_right`;
//      otherwise increment `need_left`. The answer is `need_left + need_right`.
//      Time Complexity: O(N), where N = s.size().
//      Space Complexity: O(1).
//   2) Explicit Stack (`minAddToMakeValidStack`):
//      Push `(` onto a character stack; on `)`, cancel it against a top `(` if
//      present, otherwise push `)`. Every character left on the stack after
//      the scan is unmatched and needs one complementary insertion.
//      Time Complexity: O(N), Space Complexity: O(N).
//
// Data Structures:
//   - Two integer balance counters (`need_left`, `need_right`) for the O(1)
//     optimal solution.
//   - `std::vector<char>` stack for the alternative explicit-stack method.
//
// Real-World Applications:
//   - IDE syntax-error recovery and auto-closing bracket insertion for
//     incomplete code snippets.
//   - Compiler and linter diagnostics computing minimal edit repairs for
//     unbalanced delimiters or query expressions.
//   - Streaming XML/JSON/formula validators detecting missing opening or
//     closing tokens in a single pass.
//
// Related LeetCode Problems:
//   - #20 Valid Parentheses [done] (checking balanced brackets)
//   - #1249 Minimum Remove to Make Valid Parentheses [done] (delete instead of add)
//   - #1541 Minimum Insertions to Balance a Parentheses String [done] (1 '(' matches 2 ')')
//   - #1963 Minimum Number of Swaps to Make the String Balanced [new] (swaps vs inserts)
//   - #301 Remove Invalid Parentheses [done] (enumerate all minimal valid removals)

#include "framework/lc_framework.h"

#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the minimum insertions required to balance `s` in one pass with
  // O(1) auxiliary space.
  // Invariant: after scanning any prefix, `need_left` equals the number of
  // unmatched ')' in the prefix and `need_right` equals the number of
  // unmatched '(' still open.
  // Time: O(N), Space: O(1).
  int minAddToMakeValid(std::string_view s) {
    int need_left = 0;
    int need_right = 0;

    for (const char ch : s) {
      if (ch == '(') {
        ++need_right;
      } else if (need_right > 0) {
        // Match this ')' against the most recent unmatched '('.
        --need_right;
      } else {
        // No open '(' available to pair with this ')', so insert a '(' before it.
        ++need_left;
      }
    }

    return need_left + need_right;
  }

  // Alternative explicit-stack solution that cancels valid "()" pairs and
  // counts all remaining unmatched characters.
  // Time: O(N), Space: O(N).
  int minAddToMakeValidStack(std::string_view s) {
    std::vector<char> stack;
    stack.reserve(s.size());

    for (const char ch : s) {
      if (ch == ')' && !stack.empty() && stack.back() == '(') {
        stack.pop_back();
      } else {
        stack.push_back(ch);
      }
    }

    return static_cast<int>(stack.size());
  }
};

LC_PROBLEM(921, "Minimum Add To Make Parentheses Valid", "stack", ctx) {
  Solution sol;

  auto check_both = [&](std::string_view s, int expected) {
    ctx.expect_eq(sol.minAddToMakeValid(s), expected);
    ctx.expect_eq(sol.minAddToMakeValidStack(s), expected);
  };

  ctx.test("examples from prompt", [&] {
    check_both("())", 1);
    check_both("(((", 3);
    check_both("()", 0);
    check_both("()))((", 4);
  });

  ctx.test("edge cases", [&] {
    check_both("", 0);
    check_both(")(", 2);
    check_both("((()", 2);
    check_both("))))", 4);
    check_both("(())()", 0);
  });
}

}  // namespace
