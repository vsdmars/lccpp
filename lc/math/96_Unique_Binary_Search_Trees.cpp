// LeetCode 96: Unique Binary Search Trees
// https://leetcode.com/problems/unique-binary-search-trees/
//
// Problem:
//   Given an integer n (1 <= n <= 19), return the number of structurally unique
//   binary search trees (BSTs) that store exactly n nodes with distinct values
//   from 1 to n.
//
// Examples:
//   Example 1:
//     Input:  n = 3
//     Output: 5
//     Explanation:
//       - Allocate dp = [1, 0, 0, 0] so dp[0] = 1 represents the single empty BST.
//       - For m = 1, root i = 1 gives dp[1] = dp[0] * dp[0] = 1.
//       - For m = 2, roots i = 1, 2 give dp[2] = dp[0] * dp[1] + dp[1] * dp[0] =
//         1*1 + 1*1 = 2.
//       - For m = 3, summing dp[i - 1] * dp[3 - i] over i = 1, 2, 3 yields 1*2 +
//         1*1 + 2*1 = 5, returning dp[3] = 5.
//
//   Example 2:
//     Input:  n = 4
//     Output: 14
//     Explanation:
//       - Initialize dp[0] = 1 and populate dp[1] = 1, dp[2] = 2, and dp[3] = 5 in
//         bottom-up order.
//       - For m = 4, iterate root i from 1 to 4 and accumulate dp[0]*dp[3] +
//         dp[1]*dp[2] + dp[2]*dp[1] + dp[3]*dp[0].
//       - Evaluate 1*5 + 1*2 + 2*1 + 5*1 = 14 into dp[4] and return 14.
//
//   Example 3:
//     Input:  n = 1
//     Output: 1
//     Explanation:
//       - Allocate dp of size 2 initialized to [1, 0].
//       - Run the outer loop for m = 1 and inner loop for i = 1, computing dp[1] =
//         dp[0] * dp[0] = 1.
//       - Return dp[1] = 1.
//
// Algorithm:
//   Dynamic Programming (Catalan number recurrence).
//   Choosing any value i in [1, m] as the root of an m-node BST partitions the
//   remaining values into i - 1 smaller keys (left subtree) and m - i larger
//   keys (right subtree). Because the exact key values do not affect the count
//   of shapes—only the relative ordering matters—each subtree count depends
//   solely on its size. By the rule of product, rooting at i yields
//   dp[i - 1] * dp[m - i] distinct trees, so:
//       dp[m] = sum_{i=1}^{m} dp[i - 1] * dp[m - i],  with dp[0] = 1.
//   This is the n-th Catalan number C_n. Both bottom-up tabulation and top-down
//   memoized DFS are implemented for clarity.
//   Time Complexity:  O(n^2)
//   Space Complexity: O(n)
//
// Data Structures:
//   - std::vector<int>: 1D DP table storing the count of unique BSTs for every
//     subtree size from 0 up to n (and memo table for the top-down variant).
//
// Real-World Applications:
//   - Database query optimizers estimating the size of the join-tree search
//     space for n relations (Catalan growth explains why dynamic programming or
//     heuristics are required over exhaustive enumeration).
//   - Compiler parser design and combinatorics of parenthesizing associative
//     expressions or counting distinct abstract syntax tree shapes.
//   - RNA secondary structure counting and non-crossing partition enumeration
//     in computational biology.
//
// Related LeetCode Problems:
//   - #95 Unique Binary Search Trees II [done] (constructs all unique BSTs rather than counting them)
//   - #98 Validate Binary Search Tree [done] (verifies BST ordering invariants)
//   - #22 Generate Parentheses [done] (another classic Catalan-number enumeration problem)
//   - #241 Different Ways to Add Parentheses [done] (Catalan-structured divide-and-conquer partitioning)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the number of structurally unique BSTs with `n` nodes via
  // bottom-up dynamic programming over the Catalan recurrence.
  //
  // Invariant: after outer iteration `m`, `dp[k]` equals the exact number of
  // unique BSTs of size `k` for every `0 <= k <= m`.
  //
  // Time: O(n^2), Space: O(n).
  int numTrees(int n) {
    if (n <= 0) {
      return 1;
    }

    const auto target = static_cast<std::size_t>(n);
    // dp[m] stores the number of structurally unique BSTs containing `m` nodes.
    // An empty tree (m == 0) has exactly 1 structural form (the null tree).
    std::vector<int> dp(target + 1uz, 0);
    dp[0] = 1;

    // Build counts bottom-up for every subtree size m from 1 to n.
    for (std::size_t m = 1uz; m <= target; ++m) {
      int total = 0;
      // Treat each key i in [1, m] as the root:
      //   left subtree has (i - 1) nodes, right subtree has (m - i) nodes.
      // Because left and right choices are independent, multiply their counts.
      for (std::size_t i = 1uz; i <= m; ++i) {
        total += (dp[i - 1uz] * dp[m - i]);
      }
      dp[m] = total;
    }

    return dp[target];
  }

  // Top-down memoized DFS variant (mirrors the recursive Python solution).
  //
  // Time: O(n^2), Space: O(n) for the memo table and recursion stack.
  int numTreesMemo(int n) {
    if (n <= 0) {
      return 1;
    }

    const auto target = static_cast<std::size_t>(n);
    // -1 marks uncomputed subtree sizes; base cases 0 and 1 have 1 unique BST.
    std::vector<int> memo(target + 1uz, -1);
    memo[0] = 1;
    memo[1] = 1;

    auto dfs = [&](this auto&& self, std::size_t nodes) -> int {
      if (memo[nodes] != -1) {
        return memo[nodes];
      }

      int count = 0;
      // Sum Cartesian products of left and right subtree counts over all roots.
      for (std::size_t root = 1uz; root <= nodes; ++root) {
        count += (self(root - 1uz) * self(nodes - root));
      }
      memo[nodes] = count;
      return count;
    };

    return dfs(target);
  }
};

LC_PROBLEM(96, "Unique Binary Search Trees", "math", ctx) {
  Solution sol;

  ctx.test("example_n_3", [&] {
    ctx.expect_eq(sol.numTrees(3), 5);
    ctx.expect_eq(sol.numTreesMemo(3), 5);
  });

  ctx.test("example_n_1", [&] {
    ctx.expect_eq(sol.numTrees(1), 1);
    ctx.expect_eq(sol.numTreesMemo(1), 1);
  });

  ctx.test("small_catalan_sequence", [&] {
    ctx.expect_eq(sol.numTrees(2), 2);
    ctx.expect_eq(sol.numTrees(4), 14);
    ctx.expect_eq(sol.numTrees(5), 42);
    ctx.expect_eq(sol.numTreesMemo(4), 14);
    ctx.expect_eq(sol.numTreesMemo(5), 42);
  });

  ctx.test("max_constraint_n_19", [&] {
    // 19th Catalan number fits in signed 32-bit int.
    ctx.expect_eq(sol.numTrees(19), 1767263190);
    ctx.expect_eq(sol.numTreesMemo(19), 1767263190);
  });
}

}  // namespace
