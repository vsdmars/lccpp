// LeetCode 301: Remove Invalid Parentheses
// https://leetcode.com/problems/remove-invalid-parentheses/
//
// Problem:
//   Given a string `s` containing parentheses `'('`, `')'` and lowercase
//   English letters, remove the minimum number of invalid parentheses to make
//   the input string balanced. Return all unique valid results in any order.
//   Input length is at most 25 with at most 20 parentheses.
//
// Algorithm:
//   We provide two complementary approaches from the Python source:
//   1. Level-order BFS (`removeInvalidParentheses`): Treat each single
//      parenthesis deletion as an unweighted edge in an implicit state graph.
//      Level `k` holds all unique strings formed by deleting exactly `k`
//      parentheses from `s`. Scan the current level: every balanced string is
//      collected into `results`. As soon as at least one valid string is found
//      at depth `k`, stop generating depth `k + 1` and return `results` —
//      guaranteeing minimum removals. Because every string at depth `k` has
//      length `|s| - k`, using an `unordered_set` for `next_frontier` suffices
//      to eliminate all duplicates without cross-level collisions.
//   2. Two-pass DFS (`removeInvalidParenthesesDfs`): Scan left-to-right; the
//      first prefix with negative balance (`')'` count exceeds `'('` count)
//      must delete one `')'` in `[last_j, i]`. To avoid duplicate outputs,
//      only delete the first `')'` in any contiguous run (`j == last_j ||
//      expr[j - 1] != close`) and pass `(i, j)` to keep deletions monotonic.
//      Once no excess `')'` remains, reverse the string and run the same pass
//      with swapped roles `{')', '('}` to prune excess `'('`, then reverse
//      back.
//   Time Complexity:  O(N * 2^N) worst case for BFS (pruned heavily in
//                     practice by early level stop and deduplication);
//                     O(N * C(N, k)) for two-pass DFS where k is the minimum
//                     number of removals.
//   Space Complexity: O(N * C(N, k)) for the BFS frontier set; O(N) recursion
//                     depth plus output storage for two-pass DFS.
//
// Data Structures:
//   - `std::unordered_set<std::string>`: Deduplicated BFS frontier for each
//     removal depth.
//   - `std::array<char, 2>`: Parameterizes open/close parenthesis roles across
//     forward and reversed passes in two-pass DFS.
//
// Real-World Applications:
//   - Syntax-error recovery in compilers, IDEs, and SQL/formula parsers that
//     suggest minimal edits to repair unbalanced expressions.
//   - Spreadsheet and calculator expression sanitizers that preserve user
//     identifiers while stripping minimal malformed grouping tokens.
//   - Automated grammar repair and query-rewriting pipelines.
//
// Related LeetCode Problems:
//   - #20 Valid Parentheses [done] (core balance validation predicate)
//   - #22 Generate Parentheses [done] (backtracking over balanced parentheses)
//   - #1249 Minimum Remove to Make Valid Parentheses [done] (single valid fix)
//   - #921 Minimum Add to Make Parentheses Valid [done] (minimum edit count)
//   - #32 Longest Valid Parentheses [done] (balanced substring analysis)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Returns all unique valid strings formed by removing the minimum number of
  // invalid parentheses from `s` using level-order BFS.
  // Invariant: Level `k` contains only strings formed by deleting exactly `k`
  // parentheses; stopping at the first level containing a valid string ensures
  // minimum removals.
  // Time: O(N * 2^N) worst case, Space: O(N * C(N, k)).
  std::vector<std::string> removeInvalidParentheses(std::string_view s) {
    std::unordered_set<std::string> frontier = {std::string(s)};
    std::vector<std::string> results;

    while (!frontier.empty()) {
      std::unordered_set<std::string> next_frontier;

      for (const std::string& expr : frontier) {
        if (is_valid(expr)) {
          results.push_back(expr);
          continue;
        }
        // Once any valid string exists at this depth, deeper levels can never
        // be minimal, so skip generating children.
        if (!results.empty()) {
          continue;
        }
        generate_deletions(expr, next_frontier);
      }

      if (!results.empty()) {
        return results;
      }
      frontier = std::move(next_frontier);
    }

    return results;
  }

  // Two-pass DFS alternative (matching Python's `fast()`): removes excess ')'
  // left-to-right, reverses the string, and removes excess '(' with the same
  // routine.
  // Time: O(N * C(N, k)), Space: O(N) stack + output.
  std::vector<std::string> removeInvalidParenthesesDfs(std::string_view s) {
    std::vector<std::string> results;
    dfs_prune(s, 0uz, 0uz, {'(', ')'}, results);
    return results;
  }

 private:
  // Returns true iff all '(' and ')' in `expr` are properly balanced.
  static bool is_valid(std::string_view expr) {
    int open_count = 0;
    for (const char ch : expr) {
      if (ch == '(') {
        ++open_count;
      } else if (ch == ')') {
        if (open_count == 0) {
          return false;
        }
        --open_count;
      }
    }
    return open_count == 0;
  }

  // Inserts all distinct strings obtained by removing one parenthesis from
  // `expr` into `next_frontier`.
  static void generate_deletions(
      std::string_view expr, std::unordered_set<std::string>& next_frontier) {
    for (std::size_t i = 0uz; i < expr.size(); ++i) {
      if (expr[i] != '(' && expr[i] != ')') {
        continue;
      }
      // Removing any character inside a contiguous run of identical
      // parentheses yields the exact same string; skip redundant positions.
      if (i > 0uz && expr[i] == expr[i - 1uz]) {
        continue;
      }
      std::string candidate;
      candidate.reserve(expr.size() - 1uz);
      candidate.append(expr.substr(0uz, i));
      candidate.append(expr.substr(i + 1uz));
      next_frontier.insert(std::move(candidate));
    }
  }

  // Prunes excess `parens[1]` characters from `expr` starting scan at `last_i`
  // and allowing deletions only from `last_j` onward to guarantee uniqueness.
  static void dfs_prune(std::string_view expr, std::size_t last_i,
                        std::size_t last_j, std::array<char, 2> parens,
                        std::vector<std::string>& results) {
    int balance = 0;
    for (std::size_t i = last_i; i < expr.size(); ++i) {
      if (expr[i] == parens[0]) {
        ++balance;
      } else if (expr[i] == parens[1]) {
        --balance;
      }
      if (balance >= 0) {
        continue;
      }

      // First prefix with negative balance: try deleting each distinct
      // `parens[1]` in `[last_j, i]` (first in any consecutive run).
      for (std::size_t j = last_j; j <= i; ++j) {
        if (expr[j] == parens[1] &&
            (j == last_j || expr[j - 1uz] != parens[1])) {
          std::string next;
          next.reserve(expr.size() - 1uz);
          next.append(expr.substr(0uz, j));
          next.append(expr.substr(j + 1uz));
          dfs_prune(next, i, j, parens, results);
        }
      }
      return;
    }

    // No unmatched `parens[1]` remains. Reverse and run a second pass for
    // excess `parens[0]`, or emit the restored string after the second pass.
    std::string reversed(expr.rbegin(), expr.rend());
    if (parens[0] == '(') {
      dfs_prune(reversed, 0uz, 0uz, {')', '('}, results);
      return;
    }
    results.push_back(std::move(reversed));
  }
};

LC_PROBLEM(301, "Remove Invalid Parentheses", "bfs", ctx) {
  Solution sol;

  auto expect_same_results = [&](std::string_view input,
                                 std::vector<std::string> expected) {
    std::ranges::sort(expected);

    auto bfs_res = sol.removeInvalidParentheses(input);
    std::ranges::sort(bfs_res);
    ctx.expect_eq(bfs_res, expected);

    auto dfs_res = sol.removeInvalidParenthesesDfs(input);
    std::ranges::sort(dfs_res);
    ctx.expect_eq(dfs_res, expected);
  };

  ctx.test("example_1_parentheses_only", [&] {
    expect_same_results("()())()", {"(())()", "()()()"});
  });

  ctx.test("example_2_with_letters", [&] {
    expect_same_results("(a)())()", {"(a())()", "(a)()()"});
  });

  ctx.test("example_3_all_invalid", [&] { expect_same_results(")(", {""}); });

  ctx.test("already_valid_and_empty", [&] {
    expect_same_results("((a))", {"((a))"});
    expect_same_results("", {""});
    expect_same_results("abc", {"abc"});
  });

  ctx.test("multiple_excess_open_and_close", [&] {
    expect_same_results("))((", {""});
    expect_same_results("(()((", {"()"});
  });
}

}  // namespace
