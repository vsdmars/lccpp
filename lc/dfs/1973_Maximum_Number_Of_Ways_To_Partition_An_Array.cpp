// LeetCode 1973: Count Nodes Equal to Sum of Descendants
// https://leetcode.com/problems/count-nodes-equal-to-sum-of-descendants/
// (Registered under the source file's title metadata:
//  "Maximum Number Of Ways To Partition An Array".)
//
// Problem:
//   Given the root of a binary tree with 1 <= n <= 10^5 nodes and values
//   0 <= Node.val <= 10^5, return the number of nodes whose value equals the
//   sum of the values of all of its descendants. A node with no descendants
//   (a leaf) has a descendant sum of 0.
//
// Algorithm:
//   Run a single post-order (bottom-up) DFS returning the total sum of values
//   in each subtree as a 64-bit integer (`std::int64_t`), preventing 32-bit
//   signed overflow when up to 10^5 nodes of value 10^5 sum to 10^10:
//     - Base case: `nullptr` contributes a subtree sum of 0.
//     - Recursively compute `left_sum` and `right_sum`.
//     - The sum of `node`'s strict descendants is `descendants_sum = left_sum
//       + right_sum`. If `node->val == descendants_sum`, increment `matches`.
//     - Return `descendants_sum + node->val` to `node`'s parent.
//
//   Time Complexity:  O(n), visiting every node once.
//   Space Complexity: O(h) recursion stack, where h is the tree height.
//
// Data Structures:
//   - Binary tree (`TreeNode` with integer value and left/right child links).
//   - Implicit DFS call stack propagating 64-bit subtree sums upward.
//
// Real-World Applications:
//   - Accounting and ledger rollup verification: validating that every parent
//     cost-center or summary account equals the exact sum of its sub-accounts.
//   - Storage and resource-quota auditing: checking directory or container
//     usage totals against the sum of nested child entries.
//   - Financial bill-of-materials (BOM) integrity checks in ERP systems.
//
// Related LeetCode Problems:
//   - #2265 Count Nodes Equal to Average of Subtree [new] (post-order subtree sum and count aggregation)
//   - #508 Most Frequent Subtree Sum [done] (computing all subtree sums via post-order DFS)
//   - #563 Binary Tree Tilt [new] (bottom-up subtree sum aggregation)
//   - #1123 Lowest Common Ancestor of Deepest Leaves [done] (post-order binary tree DFS)

#include "framework/lc_framework.h"

#include <cstdint>
#include <deque>

namespace {

// Binary tree node matching LeetCode's standard representation.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;
};

// RAII arena for allocating test TreeNodes without raw new/delete leaks.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    nodes_.push_back(TreeNode{.val = val, .left = left, .right = right});
    return &nodes_.back();
  }

 private:
  std::deque<TreeNode> nodes_;
};

class Solution {
 public:
  // Counts how many nodes in the binary tree have a value equal to the sum of
  // all of their descendants' values.
  //
  // Complexity: O(n) time, O(h) auxiliary stack space.
  int equalToDescendants(const TreeNode* root) {
    int matches = 0;

    // Post-order DFS returning the 64-bit sum of all node values in `node`'s
    // subtree (including `node->val`).
    auto dfs = [&](this auto&& self, const TreeNode* node) -> std::int64_t {
      if (node == nullptr) {
        return 0;
      }

      const std::int64_t left_sum = self(node->left);
      const std::int64_t right_sum = self(node->right);
      const std::int64_t descendants_sum = left_sum + right_sum;

      // Compare in 64-bit width so large subtree sums never wrap around.
      if (static_cast<std::int64_t>(node->val) == descendants_sum) {
        ++matches;
      }

      return descendants_sum + static_cast<std::int64_t>(node->val);
    };

    dfs(root);
    return matches;
  }
};

LC_PROBLEM(1973, "Maximum Number Of Ways To Partition An Array", "dfs", ctx) {
  Solution sol;

  ctx.test("example_1_two_matching_nodes", [&] {
    // Tree: [10,3,4,2,1] -> node 10 (3+4+2+1=10) and node 3 (2+1=3) match.
    TreeArena arena;
    TreeNode* n2 = arena.make(2);
    TreeNode* n1 = arena.make(1);
    TreeNode* n3 = arena.make(3, n2, n1);
    TreeNode* n4 = arena.make(4);
    TreeNode* root = arena.make(10, n3, n4);

    ctx.expect_eq(sol.equalToDescendants(root), 2);
  });

  ctx.test("example_2_no_matching_nodes", [&] {
    // Tree: [2,3,null,2,null] -> 0 matches.
    TreeArena arena;
    TreeNode* root = arena.make(2, arena.make(3, arena.make(2), nullptr));
    ctx.expect_eq(sol.equalToDescendants(root), 0);
  });

  ctx.test("example_3_single_zero_leaf", [&] {
    // Leaf with value 0 equals empty descendant sum 0.
    TreeArena arena;
    TreeNode* root = arena.make(0);
    ctx.expect_eq(sol.equalToDescendants(root), 1);
  });

  ctx.test("all_zeros_every_node_matches", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(0, arena.make(0), arena.make(0));
    ctx.expect_eq(sol.equalToDescendants(root), 3);
  });

  ctx.test("empty_tree_returns_zero", [&] {
    ctx.expect_eq(sol.equalToDescendants(nullptr), 0);
  });

  ctx.test("64bit_subtree_sum_avoids_int32_overflow", [&] {
    // Descendants sum to 3 * 10^9 (> INT32_MAX), which must not wrap to match
    // the root's 32-bit value.
    TreeArena arena;
    TreeNode* left =
        arena.make(1000000000, arena.make(500000000), arena.make(500000000));
    TreeNode* right = arena.make(1000000000);
    // Left node (10^9 == 5e8 + 5e8) matches; root (0 != 3e9) does not.
    TreeNode* root = arena.make(0, left, right);
    ctx.expect_eq(sol.equalToDescendants(root), 1);
  });
}

}  // namespace
