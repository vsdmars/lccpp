// LeetCode 1614: Maximum Nesting Depth Of The Parentheses
// https://leetcode.com/problems/maximum-nesting-depth-of-the-parentheses/
//
// Problem:
//   Given a valid parentheses string (VPS) `s` consisting of digits, arithmetic
//   operators (`+`, `-`, `*`, `/`), and balanced parentheses `(` and `)`, return
//   the maximum nesting depth of `s` (the maximum number of simultaneously open
//   parentheses at any point in the string).
//
// Examples:
//   Example 1:
//     Input:  s = "(1+(2*3)+((8)/4))+1"
//     Output: 3
//     Explanation:
//       - Scanning '(' increments current_depth to 1 (max_depth = 1), ignoring '1+'
//         and then scanning '(' raises current_depth to 2 (max_depth = 2) before
//         ')' drops it back to 1.
//       - After ignoring '+', the two consecutive '(' before '8' raise
//         current_depth from 1 to 3 and update max_depth to 3.
//       - The remaining ')', '/4', '))', and '+1' decrement current_depth back down
//         to 0, leaving max_depth = 3.
//
//   Example 2:
//     Input:  s = "(1)+((2))+(((3)))"
//     Output: 3
//     Explanation:
//       - The first group "(1)" sets current_depth = 1, updates max_depth = 1, and
//         returns current_depth to 0.
//       - The second group "((2))" raises current_depth to 2, updates max_depth =
//         2, and returns current_depth to 0.
//       - The third group "(((3)))" raises current_depth to 3, sets max_depth = 3,
//         and closes back to 0, returning 3.
//
//   Example 3:
//     Input:  s = "1"
//     Output: 0
//     Explanation:
//       - Initialize current_depth = 0 and max_depth = 0.
//       - Scan the single character '1', which is neither '(' nor ')', so both
//         counters remain 0 and 0 is returned.
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
