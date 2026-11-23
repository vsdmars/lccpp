// LeetCode 386: Lexicographical Numbers
// https://leetcode.com/problems/lexicographical-numbers/
//
// Problem:
//   Given an integer `n` (`1 <= n <= 50000`), return all integers in the range
//   `[1, n]` sorted in lexicographical (dictionary string) order. The algorithm
//   should run in O(n) time and use O(1) extra space (excluding the output
//   vector).
//
// Examples:
//   Example 1:
//     Input:  n = 13
//     Output: [1, 10, 11, 12, 13, 2, 3, 4, 5, 6, 7, 8, 9]
//     Explanation:
//       - Start at current = 1 with descent threshold n / 10 = 1, emit 1, and
//         descend via current *= 10 to 10 since 1 <= 1.
//       - Emit 10, 11, 12, and 13 by incrementing ++current at each step because
//         current > 1, current % 10 != 9, and current + 1 <= 13 holds before 13.
//       - After emitting 13, ascend out of the exhausted subtree because 13 + 1 >
//         13 triggers current /= 10 to 1, then increment to sibling 2.
//       - Emit 2 through 9 via ++current across the remaining 8 iterations since
//         2..9 exceed n / 10 = 1 and finish all 13 values.
//
//   Example 2:
//     Input:  n = 2
//     Output: [1, 2]
//     Explanation:
//       - Set n / 10 = 0 and start at current = 1, emitting 1 on the first
//         iteration.
//       - Skip descending since 1 > 0, increment ++current to 2 because 1 % 10 != 9
//         and 1 + 1 <= 2, and emit 2 on the second iteration.
//
//   Example 3:
//     Input:  n = 10
//     Output: [1, 10, 2, 3, 4, 5, 6, 7, 8, 9]
//     Explanation:
//       - Start at current = 1 with n / 10 = 1, emit 1, and descend via current *=
//         10 to 10.
//       - Emit 10, then climb via current /= 10 back to 1 because 10 + 1 > 10, and
//         increment to sibling 2.
//       - Emit 2 through 9 consecutively via ++current since each satisfies current
//         > 1, completing all 10 iterations.
//
// Algorithm:
//   The integers `[1, n]` in lexicographical order correspond exactly to a
//   preorder traversal of a 10-ary (denary) prefix tree rooted at digits
//   `1..9`, where each node `x` has children `10*x + 0, ..., 10*x + 9` that do
//   not exceed `n`.
//
//   1. Iterative Preorder Walk (primary, O(1) extra space):
//      Start at `current = 1` and emit `n` numbers:
//        - If `current * 10 <= n` (checked overflow-safely as `current <= n / 10`),
//          descend to the leftmost child (`current *= 10`).
//        - Otherwise, while `current` is a rightmost child (`current % 10 == 9`)
//          or incrementing it would exceed `n` (`current + 1 > n`), climb up to
//          the parent (`current /= 10`), then step to the next sibling
//          (`++current`).
//      Every edge of the n-node denary tree is traversed at most twice.
//      Time Complexity:  O(n)
//      Space Complexity: O(1) auxiliary (O(n) output)
//
//   2. Recursive DFS (secondary, mirrors original Python):
//      Recursively visits `val` and then `val * 10 + digit` for `digit = 0..9`
//      while within `[1, n]`.
//      Time Complexity:  O(n)
//      Space Complexity: O(log_10 n) call stack
//
// Data Structures:
//   - Implicit 10-ary prefix tree (trie) navigated purely with integer
//     multiplication (`* 10`) and division (`/ 10`) -- no heap nodes allocated.
//   - `std::vector<int>`: pre-reserved output container of size `n`.
//
// Real-World Applications:
//   - Key-range pagination and prefix scans in LSM-tree / B-tree key-value
//     stores (RocksDB, LevelDB, Bigtable) where numeric keys are stored as
//     UTF-8/ASCII strings.
//   - Generating deterministic lexicographical test fixtures, hierarchical
//     outline numbering, and object-storage key-prefix sharding (S3/GCS).
//   - Counting and ranking k-th lexicographical elements without materializing
//     string representations.
//
// Related LeetCode Problems:
//   - #440 K-th Smallest in Lexicographical Order [new] (skips entire denary-trie subtrees in O((log n)^2) time)
//   - #208 Implement Trie [done] (Prefix Tree) (explicit prefix-tree traversal and lexicographical ordering)
//   - #179 Largest Number [done] (custom lexicographical ordering on stringified integers)
//   - #31 Next Permutation [done] (in-place O(1)-space lexicographical successor step)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

namespace {

class Solution {
 public:
  // Returns `[1, n]` in lexicographical order using an iterative preorder walk
  // on the implicit 10-ary trie.
  // Invariant: `current` is always the exact next integer in lexicographical
  // order within `[1, n]`.
  // Time: O(n), Space: O(1) auxiliary.
  std::vector<int> lexicalOrder(int n) {
    if (n <= 0) {
      return {};
    }

    std::vector<int> result;
    result.reserve(static_cast<std::size_t>(n));

    int current = 1;
    for (int count = 0; count < n; ++count) {
      result.push_back(current);

      if (current <= n / 10) {
        // Prefer descending to the leftmost child (`current * 10`).
        current *= 10;
      } else {
        // Otherwise ascend out of exhausted subtrees (trailing 9 or right bound
        // `n`) before advancing to the next sibling.
        while (current % 10 == 9 || current + 1 > n) {
          current /= 10;
        }
        ++current;
      }
    }

    return result;
  }

  // Recursive denary-trie DFS preserving the original Python solution.
  // Time: O(n), Space: O(log_10 n) call stack.
  std::vector<int> lexicalOrderDfs(int n) {
    if (n <= 0) {
      return {};
    }

    std::vector<int> result;
    result.reserve(static_cast<std::size_t>(n));

    // Preorder DFS using C++23 deducing `this` for zero-overhead recursion.
    auto dfs = [&](this auto&& self, int val) -> void {
      result.push_back(val);
      if (val > n / 10) {
        return;
      }
      const int base = val * 10;
      for (int digit = 0; digit < 10; ++digit) {
        const int next = base + digit;
        if (next > n) {
          break;
        }
        self(next);
      }
    };

    for (int root = 1; root <= 9 && root <= n; ++root) {
      dfs(root);
    }

    return result;
  }
};

LC_PROBLEM(386, "Lexicographical Numbers", "string", ctx) {
  Solution sol;

  ctx.test("example_1_n13", [&] {
    const std::vector<int> expected{1, 10, 11, 12, 13, 2, 3, 4, 5, 6, 7, 8, 9};
    ctx.expect_eq(sol.lexicalOrder(13), expected);
    ctx.expect_eq(sol.lexicalOrderDfs(13), expected);
  });

  ctx.test("example_2_n2", [&] {
    const std::vector<int> expected{1, 2};
    ctx.expect_eq(sol.lexicalOrder(2), expected);
    ctx.expect_eq(sol.lexicalOrderDfs(2), expected);
  });

  ctx.test("boundary_n1_and_n10", [&] {
    ctx.expect_eq(sol.lexicalOrder(1), std::vector<int>{1});
    ctx.expect_eq(sol.lexicalOrderDfs(1), std::vector<int>{1});

    const std::vector<int> expected_10{1, 10, 2, 3, 4, 5, 6, 7, 8, 9};
    ctx.expect_eq(sol.lexicalOrder(10), expected_10);
    ctx.expect_eq(sol.lexicalOrderDfs(10), expected_10);
  });

  ctx.test("build_case_n999_matches_string_sort", [&] {
    constexpr int kLimit = 999;
    std::vector<int> expected;
    expected.reserve(static_cast<std::size_t>(kLimit));
    for (int v = 1; v <= kLimit; ++v) {
      expected.push_back(v);
    }
    std::ranges::sort(expected, [](int lhs, int rhs) {
      return std::to_string(lhs) < std::to_string(rhs);
    });

    ctx.expect_eq(sol.lexicalOrder(kLimit), expected);
    ctx.expect_eq(sol.lexicalOrderDfs(kLimit), expected);
  });
}

}  // namespace
