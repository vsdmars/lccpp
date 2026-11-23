// LeetCode 1614: Maximum Nesting Depth Of The Parentheses
// https://leetcode.com/problems/maximum-nesting-depth-of-the-parentheses/
//
// Problem:
//   Given a valid parentheses string (VPS) `s` consisting of digits, arithmetic
//   operators (`+`, `-`, `*`, `/`), and balanced parentheses `(` and `)`, return
//   the maximum nesting depth of `s` (the maximum number of simultaneously open
//   parentheses at any point in the string).
//
// Algorithm:
//   Single-Pass Depth Counter (Implicit Stack):
//     Because `s` is guaranteed to be a valid parentheses string, the nesting
//     depth at any character equals the number of currently unmatched `'('`
//     characters — i.e., the height of a matching stack. Rather than allocating
//     a stack, maintain an integer `current_depth` and `max_depth`:
//       - Increment `current_depth` on `'('` and update `max_depth`.
//       - Decrement `current_depth` on `')'`.
//       - Ignore all non-parenthesis characters.
//   Time Complexity:  O(n), where n = s.size().
//   Space Complexity: O(1).
//
// Data Structures:
//   - Scalar depth counters (`current_depth`, `max_depth`) replacing an
//     explicit stack since only stack height is needed.
//
// Real-World Applications:
//   - Enforcing parser recursion / AST nesting depth limits in JSON, XML, SQL,
//     and programming-language parsers to prevent stack-overflow DoS attacks.
//   - Computing expression complexity metrics in static analysis and linters.
//
// Related LeetCode Problems:
//   - #20 Valid Parentheses [done] (checking whether parentheses are balanced)
//   - #1021 Remove Outermost Parentheses [new] (filtering primitives at depth 0)
//   - #1111 Maximum Nesting Depth of Two Valid Parentheses Strings [new] (partitioning depth across two groups)
//   - #856 Score of Parentheses [done] (evaluating weighted nesting depth)

#include "framework/lc_framework.h"

#include <algorithm>
#include <string_view>

namespace {

class Solution {
 public:
  // Computes the peak number of simultaneously open parentheses in a VPS.
  // Time: O(n), Space: O(1).
  int maxDepth(std::string_view s) {
    int current_depth = 0;
    int max_depth = 0;

    for (char c : s) {
      if (c == '(') {
        // Entering one deeper parenthesis level; update peak immediately.
        ++current_depth;
        max_depth = std::max(max_depth, current_depth);
      } else if (c == ')') {
        // Leaving the innermost open parenthesis level.
        --current_depth;
      }
    }

    return max_depth;
  }
};

LC_PROBLEM(1614, "Maximum Nesting Depth Of The Parentheses", "stack", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    ctx.expect_eq(sol.maxDepth("(1+(2*3)+((8)/4))+1"), 3);
  });

  ctx.test("example_2", [&] {
    Solution sol;
    ctx.expect_eq(sol.maxDepth("(1)+((2))+(((3)))"), 3);
  });

  ctx.test("example_3", [&] {
    Solution sol;
    ctx.expect_eq(sol.maxDepth("1+(2*3)/(2-1)"), 1);
  });

  ctx.test("example_4_no_parentheses", [&] {
    Solution sol;
    ctx.expect_eq(sol.maxDepth("1"), 0);
  });

  ctx.test("edge_cases", [&] {
    Solution sol;
    ctx.expect_eq(sol.maxDepth(""), 0);
    ctx.expect_eq(sol.maxDepth("()"), 1);
    ctx.expect_eq(sol.maxDepth("()()()"), 1);
    ctx.expect_eq(sol.maxDepth("((((()))))"), 5);
  });
}

}  // namespace
