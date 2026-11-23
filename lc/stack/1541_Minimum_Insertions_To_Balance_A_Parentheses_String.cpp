// LeetCode 1541: Minimum Insertions To Balance A Parentheses String
// https://leetcode.com/problems/minimum-insertions-to-balance-a-parentheses-string/
//
// Problem:
//   Given a string `s` containing only `'('` and `')'`, a string is balanced if
//   every `'('` is matched by two *consecutive* `')'` characters appearing
//   after it (i.e., `'('` opens and `"))"` closes). You may insert `'('` or
//   `')'` at any position. Return the minimum number of insertions needed to
//   balance `s`.
//
// Algorithm:
//   Single-Pass Greedy Balance Tracking (Implicit Stack):
//     Maintain two integer counters while scanning `s` from left to right:
//       - `insertions`: committed insertions already forced by past characters.
//       - `needed_right`: number of `')'` characters still required to close
//         currently open `'('` characters.
//     Key invariant: every open `'('` needs an *even* pair of consecutive `')'`.
//       1. On `'('`:
//            If `needed_right` is odd, a previous `'('` had seen only a single
//            `')'` before this new `'('` interrupted it. Because closing `"))"`
//            must be consecutive, we must insert one `')'` right before this
//            `'('` (`++insertions; --needed_right;`). Then add 2 to
//            `needed_right` for the newly opened `'('`.
//       2. On `')'`:
//            Decrement `needed_right`. If it becomes `-1`, there was no open
//            `'('` to absorb this `')'`, so we must insert a `'('` before it
//            (`++insertions`), which contributes 2 required `')'` — one is
//            consumed by the current `')'`, leaving `needed_right = 1`
//            (`needed_right += 2`).
//     At the end of `s`, any remaining `needed_right` must be inserted as `')'`,
//     so the total minimum insertions is `insertions + needed_right`.
//   Time Complexity:  O(n), where n = s.size().
//   Space Complexity: O(1).
//
// Data Structures:
//   - Two scalar counters (`insertions`, `needed_right`) acting as an O(1)-space
//     compression of an explicit parenthesis stack with parity tracking.
//
// Real-World Applications:
//   - Syntax error recovery in compilers, linters, and IDE auto-closers for
//     asymmetric multi-token delimiters (e.g., `if ... fi`, template brackets,
//     or markup tags requiring paired closing tokens).
//   - Protocol frame validation and minimal repair in streaming parsers.
//
// Related LeetCode Problems:
//   - #921 Minimum Add to Make Parentheses Valid [done] (1-to-1 '(' and ')' balancing)
//   - #20 Valid Parentheses [done] (validation of nested delimiters)
//   - #1249 Minimum Remove to Make Valid Parentheses [done] (greedy deletion balancing)
//   - #301 Remove Invalid Parentheses [done] (enumerating minimal balanced repairs)
//   - #1963 Minimum Number of Swaps to Make the String Balanced [new] (balancing via swaps)

#include "framework/lc_framework.h"

#include <string_view>

namespace {

class Solution {
 public:
  // Computes the minimum insertions of '(' or ')' so every '(' pairs with two
  // consecutive '))'.
  // Time: O(n), Space: O(1).
  int minInsertions(std::string_view s) {
    int insertions = 0;    // Already committed insertions that cannot change.
    int needed_right = 0;  // Pending ')' characters required by open '('s.

    for (char c : s) {
      if (c == '(') {
        // Because each '(' requires two *consecutive* ')', an odd pending
        // count means an earlier '(' received only one ')' before being
        // interrupted by this '('. Close that lone ')' immediately.
        if ((needed_right % 2) != 0) {
          ++insertions;
          --needed_right;
        }
        needed_right += 2;
      } else {
        // Consume one needed ')' for the current closing parenthesis.
        --needed_right;
        if (needed_right < 0) {
          // Unmatched ')': insert a preceding '(', which demands two ')'.
          // One is satisfied right now, leaving one ')' still needed.
          ++insertions;
          needed_right += 2;
        }
      }
    }

    // Any still-unfulfilled ')' demands must be appended at the end.
    return insertions + needed_right;
  }
};

LC_PROBLEM(1541, "Minimum Insertions To Balance A Parentheses String", "stack",
           ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    ctx.expect_eq(sol.minInsertions("(()))"), 1);
  });

  ctx.test("example_2_already_balanced", [&] {
    Solution sol;
    ctx.expect_eq(sol.minInsertions("())"), 0);
  });

  ctx.test("example_3_mixed", [&] {
    Solution sol;
    ctx.expect_eq(sol.minInsertions("))())("), 3);
  });

  ctx.test("example_4_all_open", [&] {
    Solution sol;
    ctx.expect_eq(sol.minInsertions("(((((("), 12);
  });

  ctx.test("example_5_all_close", [&] {
    Solution sol;
    ctx.expect_eq(sol.minInsertions(")))))))"), 5);
  });

  ctx.test("interrupted_closing_pairs", [&] {
    Solution sol;
    // ")(" needs '(' + ')' around first ')' and "))" after '(' -> 4 total.
    ctx.expect_eq(sol.minInsertions(")("), 4);
    // "()()" has two separate single ')' each interrupted/incomplete -> 2.
    ctx.expect_eq(sol.minInsertions("()()"), 2);
    // Fully balanced nested & concatenated expression.
    ctx.expect_eq(sol.minInsertions("(())))())"), 0);
  });
}

}  // namespace
