// LeetCode 404: Sum Of Left Leaves
// https://leetcode.com/problems/sum-of-left-leaves/
//
// Problem:
//   Given the root of a binary tree, return the sum of all left leaves. A leaf
//   is a node with no children, and a left leaf is a leaf that is the left
//   child of its parent. A single-node tree has no left leaves (sum is 0).
//
// Algorithm:
//   Perform a Depth-First Search carrying a boolean tag `is_left` indicating
//   whether the current node was reached via its parent's `left` pointer:
//     - The root is invoked with `is_left = false`.
//     - If `node == nullptr`, contribute 0.
//     - If `node` is a leaf (`left == nullptr && right == nullptr`), return
//       `node->val` when `is_left` is true, or 0 otherwise.
//     - Otherwise, recursively sum `dfs(node->left, true) +
//       dfs(node->right, false)`.
//   Time Complexity: O(N), visiting every node once.
//   Space Complexity: O(H), where H is the height of the tree for the
//   recursion call stack (O(N) worst case for a skewed tree).
//
// Data Structures:
//   - Binary tree (`TreeNode` with value and left/right child pointers).
//   - Implicit recursion stack tracking `(const TreeNode*, bool)` context.
//
// Real-World Applications:
//   - Compiler AST and expression-tree passes that aggregate values only on
//     specific syntactic positions (e.g., left-hand operands that are terminal
//     literals).
//   - Decision-tree and rule-engine analyzers summing weights or costs of
//     terminal "true/left" outcome branches.
//   - Binary space-partitioning / Merkle tree audits summing targeted leaf
//     subsets based on path parity.
//
// Related LeetCode Problems:
//   - #112 Path Sum [done] (root-to-leaf sum checks via DFS)
//   - #129 Sum Root to Leaf Numbers [done] (aggregating values across all tree leaves)
//   - #872 Leaf-Similar Trees [new] (collecting leaf sequences via DFS)
//   - #1302 Deepest Leaves Sum [new] (summing a filtered subset of leaves)

#include "framework/lc_framework.h"

#include <deque>

namespace {

// Standard binary tree node.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;
};

class Solution {
 public:
  // Computes the sum of all leaf nodes that are left children of their parent.
  // Time: O(N), Space: O(H).
  int sumOfLeftLeaves(const TreeNode* root) {
    auto dfs = [](this auto&& self, const TreeNode* node,
                  bool is_left) -> int {
      if (node == nullptr) {
        return 0;
      }

      // A leaf contributes its value if and only if it is a left child.
      if (node->left == nullptr && node->right == nullptr) {
        return is_left ? node->val : 0;
      }

      return self(node->left, true) + self(node->right, false);
    };

    return dfs(root, false);
  }
};

// RAII arena for allocating test TreeNodes without raw new/delete.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    return &nodes_.emplace_back(TreeNode{.val = val, .left = left, .right = right});
  }

 private:
  std::deque<TreeNode> nodes_;
};

LC_PROBLEM(404, "Sum Of Left Leaves", "tree", ctx) {
  ctx.test("example_1_standard_tree", [&] {
    // 3 -> (9, 20 -> (15, 7)): left leaves are 9 and 15 => 24
    TreeArena arena;
    TreeNode* n9 = arena.make(9);
    TreeNode* n15 = arena.make(15);
    TreeNode* n7 = arena.make(7);
    TreeNode* n20 = arena.make(20, n15, n7);
    TreeNode* root = arena.make(3, n9, n20);

    Solution sol;
    ctx.expect_eq(sol.sumOfLeftLeaves(root), 24);
  });

  ctx.test("nested_left_leaf_from_python_build", [&] {
    // 3 -> (9 -> left: 8, 20 -> (15, 7)): 9 is NOT a leaf; left leaves are 8
    // and 15 => 23.
    TreeArena arena;
    TreeNode* n8 = arena.make(8);
    TreeNode* n9 = arena.make(9, n8, nullptr);
    TreeNode* n15 = arena.make(15);
    TreeNode* n7 = arena.make(7);
    TreeNode* n20 = arena.make(20, n15, n7);
    TreeNode* root = arena.make(3, n9, n20);

    Solution sol;
    ctx.expect_eq(sol.sumOfLeftLeaves(root), 23);
  });

  ctx.test("empty_and_single_node", [&] {
    TreeArena arena;
    TreeNode* single = arena.make(1);

    Solution sol;
    ctx.expect_eq(sol.sumOfLeftLeaves(nullptr), 0);
    ctx.expect_eq(sol.sumOfLeftLeaves(single), 0);
  });

  ctx.test("right_skewed_and_negative_left_leaves", [&] {
    TreeArena arena;
    // Right-only chain has no left leaves.
    TreeNode* r3 = arena.make(3);
    TreeNode* r2 = arena.make(2, nullptr, r3);
    TreeNode* r1 = arena.make(1, nullptr, r2);

    // Negative left leaves: -4 and -6 => -10.
    TreeNode* nm4 = arena.make(-4);
    TreeNode* nm6 = arena.make(-6);
    TreeNode* right = arena.make(5, nm6, nullptr);
    TreeNode* neg_root = arena.make(10, nm4, right);

    Solution sol;
    ctx.expect_eq(sol.sumOfLeftLeaves(r1), 0);
    ctx.expect_eq(sol.sumOfLeftLeaves(neg_root), -10);
  });
}

}  // namespace
