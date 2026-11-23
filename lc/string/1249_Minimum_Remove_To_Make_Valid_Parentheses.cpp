// LeetCode 1249: Minimum Remove To Make Valid Parentheses
// https://leetcode.com/problems/minimum-remove-to-make-valid-parentheses/
//
// Problem:
//   Given a string `s` containing '(', ')', and lowercase English letters
//   (1 <= s.length <= 10^5), remove the minimum number of parentheses at any
//   positions so that the resulting string has properly balanced parentheses,
//   and return any such valid string.
//
// Examples:
//   Example 1:
//     Input:  s = "lee(t(c)o)de)"
//     Output: "lee(t(c)o)de"
//     Explanation:
//       - In Pass 1, push '(' indices 3 and 5 onto open_indices = [3, 5] while
//         skipping letters.
//       - Pop 5 when ')' is seen at index 7, and pop 3 when ')' is seen at index 9,
//         leaving open_indices empty.
//       - At index 12 (')'), open_indices is empty, so set remove_mask[12] = 1 to
//         mark that unmatched closing parenthesis.
//       - In Pass 2, omit index 12 and append all remaining characters to return
//         "lee(t(c)o)de".
//
//   Example 2:
//     Input:  s = "(a(b(c)d)"
//     Output: "a(b(c)d)"
//     Explanation:
//       - In Pass 1, push '(' at indices 0, 2, and 4 onto open_indices = [0, 2, 4].
//       - Pop 4 at index 6 (')') and pop 2 at index 8 (')'), leaving unmatched open
//         index 0 on open_indices.
//       - Drain open_indices after the scan and set remove_mask[0] = 1.
//       - In Pass 2, skip index 0 and copy indices 1..8 to produce "a(b(c)d)".
//
//   Example 3:
//     Input:  s = "))(("
//     Output: ""
//     Explanation:
//       - At indices 0 and 1 (')'), open_indices is empty, so set remove_mask[0] =
//         1 and remove_mask[1] = 1.
//       - At indices 2 and 3 ('('), push both positions onto open_indices = [2, 3].
//       - After Pass 1, mark remaining open_indices entries 2 and 3 in remove_mask
//         as 1, so Pass 2 skips every index and returns "".
//
// Algorithm:
//   Scan `s` from left to right while maintaining a stack of indices of
//   currently unmatched '(' characters and a boolean removal mask initialized
//   to false:
//     - On '(', push its index onto `open_indices`.
//     - On ')', if `open_indices` is non-empty, pop the top '(' to form a
//       valid matched pair; otherwise this ')' cannot match any earlier '(',
//       so mark its index in `remove_mask`.
//   After the scan, every index still remaining on `open_indices` represents
//   an unmatched '(' and is marked in `remove_mask`. Finally, build the output
//   string by copying all indices not flagged in `remove_mask`. Because only
//   strictly unmatched parentheses are removed, the removal count is provably
//   minimal.
//   Time Complexity:  O(N), where N = s.size().
//   Space Complexity: O(N) for the index stack and removal mask.
//
// Data Structures:
//   - std::vector<std::size_t> open_indices: LIFO stack tracking positions of
//     open '(' waiting for a matching ')'.
//   - std::vector<std::uint8_t> remove_mask: byte-addressable mask flagging
//     exact indices to omit during final string assembly.
//
// Real-World Applications:
//   - Syntax error recovery in compilers, IDEs, and SQL/expression parsers
//     suggesting minimal bracket deletions to repair malformed user input.
//   - Sanitizing unbalanced markup, math expressions, or nested query filters.
//
// Related LeetCode Problems:
//   - #20 Valid Parentheses [done] (basic stack-based bracket validation)
//   - #301 Remove Invalid Parentheses [done] (enumerate all distinct minimum-removal results)
//   - #921 Minimum Add to Make Parentheses Valid [done] (minimum insertions to balance)
//   - #32 Longest Valid Parentheses [done] (longest contiguous balanced substring)
//   - #1963 Minimum Number of Swaps to Make the String Balanced [new] (minimum swaps for balance)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Removes the minimum number of invalid parentheses from `s` so the returned
  // string is balanced.
  //
  // Time Complexity:  O(N)
  // Space Complexity: O(N)
  std::string minRemoveToMakeValid(std::string_view s) {
    const std::size_t n = s.size();
    std::vector<std::size_t> open_indices;
    std::vector<std::uint8_t> remove_mask(n, 0);

    // Pass 1: pair every ')' with the nearest unmatched '(' to its left,
    // immediately flagging any ')' that has no available partner.
    for (std::size_t i = 0uz; i < n; ++i) {
      if (s[i] == '(') {
        open_indices.push_back(i);
      } else if (s[i] == ')') {
        if (!open_indices.empty()) {
          open_indices.pop_back();
        } else {
          remove_mask[i] = 1;
        }
      }
    }

    // Any '(' still on the stack has no matching ')' to its right.
    for (const std::size_t idx : open_indices) {
      remove_mask[idx] = 1;
    }

    // Pass 2: assemble the valid string by omitting all flagged positions.
    std::string result;
    result.reserve(n);
    for (std::size_t i = 0uz; i < n; ++i) {
      if (remove_mask[i] == 0) {
        result.push_back(s[i]);
      }
    }
    return result;
  }
};

// Test helper verifying that a string's parentheses are balanced.
bool is_balanced(std::string_view text) {
  int balance = 0;
  for (const char ch : text) {
    if (ch == '(') {
      ++balance;
    } else if (ch == ')') {
      --balance;
      if (balance < 0) {
        return false;
      }
    }
  }
  return balance == 0;
}

LC_PROBLEM(1249, "Minimum Remove To Make Valid Parentheses", "string", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    const std::string out = sol.minRemoveToMakeValid("lee(t(c)o)de)");
    ctx.expect_eq(out, std::string("lee(t(c)o)de"));
    ctx.expect_true(is_balanced(out));
  });

  ctx.test("example_2", [&] {
    Solution sol;
    ctx.expect_eq(sol.minRemoveToMakeValid("a)b(c)d"), std::string("ab(c)d"));
  });

  ctx.test("example_3_all_invalid", [&] {
    Solution sol;
    ctx.expect_eq(sol.minRemoveToMakeValid("))(("), std::string{});
  });

  ctx.test("example_4_unmatched_open", [&] {
    Solution sol;
    ctx.expect_eq(sol.minRemoveToMakeValid("(a(b(c)d)"),
                  std::string("a(b(c)d)"));
  });

  ctx.test("build_case", [&] {
    Solution sol;
    ctx.expect_eq(sol.minRemoveToMakeValid("a))b(c)d"), std::string("ab(c)d"));
  });

  ctx.test("already_valid_and_empty", [&] {
    Solution sol;
    ctx.expect_eq(sol.minRemoveToMakeValid("((a)(b))"),
                  std::string("((a)(b))"));
    ctx.expect_eq(sol.minRemoveToMakeValid("leetcode"),
                  std::string("leetcode"));
    ctx.expect_eq(sol.minRemoveToMakeValid(""), std::string{});
  });
}

}  // namespace
