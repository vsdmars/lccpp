// LeetCode 235: Lowest Common Ancestor Of A Binary Search Tree
// https://leetcode.com/problems/lowest-common-ancestor-of-a-binary-search-tree/
//
// Problem:
//   Given a binary search tree (BST) with unique node values and two nodes `p`
//   and `q` present in the BST, find their lowest common ancestor (LCA). The
//   LCA of `p` and `q` is the lowest (deepest) node in the tree that has both
//   `p` and `q` as descendants, where a node is allowed to be a descendant of
//   itself.
//
// Algorithm:
//   Leverage the BST ordering invariant (every value in `node->left` is
//   strictly less than `node->val`, and every value in `node->right` is
//   strictly greater than `node->val`):
//     - Normalize `low` and `high` so `low->val <= high->val`.
//     - Starting from `root`:
//         * If `node->val > high->val`, both targets lie strictly in the left
//           subtree, so descend to `node->left`.
//         * If `node->val < low->val`, both targets lie strictly in the right
//           subtree, so descend to `node->right`.
//         * Otherwise `low->val <= node->val <= high->val`: `node` is the
//           unique split point (or equals one of the targets), which is the
//           LCA.
//   1. Recursive DFS (`lowestCommonAncestor`):
//      Time Complexity:  O(h) where h is tree height (O(log n) balanced, O(n)
//                        skewed).
//      Space Complexity: O(h) recursion stack.
//   2. Iterative walk (`lowestCommonAncestorIterative`):
//      Follows the exact same root-to-split-point path in a `while` loop.
//      Time Complexity:  O(h).
//      Space Complexity: O(1).
//
// Data Structures:
//   - Binary Search Tree (`TreeNode` pointers) traversed along a single
//     root-to-LCA search path.
//
// Real-World Applications:
//   - Range-query subtree localization in B-trees and ordered database indexes
//     (finding the minimal covering subtree for a key interval `[low, high]`).
//   - Routing and prefix-tree / IP range aggregation where keys are totally
//     ordered.
//   - Finding the narrowest enclosing scope or memory segment in sorted
//     interval trees.
//
// Related LeetCode Problems:
//   - #236 Lowest Common Ancestor Of A Binary Tree [done] (general binary tree without BST ordering)
//   - #1650 Lowest Common Ancestor of a Binary Tree III [done] (LCA with parent pointers)
//   - #1676 Lowest Common Ancestor of a Binary Tree IV [new] (LCA of multiple target nodes)
//   - #272 Closest Binary Search Tree Value II [done] (BST ordering search around a target value)

#include "framework/lc_framework.h"

#include <deque>
#include <utility>

namespace {

struct TreeNode {
  int val;
  TreeNode* left;
  TreeNode* right;

  explicit TreeNode(int v, TreeNode* l = nullptr, TreeNode* r = nullptr)
      : val(v), left(l), right(r) {}
};

class Solution {
 public:
  // Finds the LCA of `p` and `q` in a BST using recursive BST partitioning.
  // Time: O(h), Space: O(h) recursion stack.
  TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
    if (root == nullptr || p == nullptr || q == nullptr) {
      return nullptr;
    }

    // Order targets so `low->val <= high->val` for clean interval comparisons.
    TreeNode* low = p;
    TreeNode* high = q;
    if (low->val > high->val) {
      std::swap(low, high);
    }

    auto dfs = [&](this auto&& self, TreeNode* node) -> TreeNode* {
      if (node == nullptr || node == low || node == high) {
        return node;
      }
      // Both targets are strictly smaller: LCA must reside in the left subtree.
      if (node->val > high->val) {
        return self(node->left);
      }
      // Both targets are strictly larger: LCA must reside in the right subtree.
      if (node->val < low->val) {
        return self(node->right);
      }
      // Split point: `low->val < node->val < high->val`, so `node` is the LCA.
      return node;
    };

    return dfs(root);
  }

  // Finds the LCA of `p` and `q` in a BST iteratively in O(1) extra space.
  // Time: O(h), Space: O(1).
  TreeNode* lowestCommonAncestorIterative(TreeNode* root, TreeNode* p,
                                          TreeNode* q) {
    if (root == nullptr || p == nullptr || q == nullptr) {
      return nullptr;
    }

    TreeNode* curr = root;
    while (curr != nullptr) {
      if (p->val > curr->val && q->val > curr->val) {
        // Both targets lie in the right subtree.
        curr = curr->right;
      } else if (p->val < curr->val && q->val < curr->val) {
        // Both targets lie in the left subtree.
        curr = curr->left;
      } else {
        // `curr` separates `p` and `q` (or matches one of them).
        return curr;
      }
    }
    return nullptr;
  }
};

LC_PROBLEM(235, "Lowest Common Ancestor Of A Binary Search Tree", "tree", ctx) {
  Solution sol;

  // Shared sample BST:
  //         6
  //       /   \
  //      2     8
  //     / \   / \
  //    0   4 7   9
  //       / \
  //      3   5
  auto build_sample = [](std::deque<TreeNode>& arena) {
    auto make = [&](int v) -> TreeNode* { return &arena.emplace_back(v); };
    TreeNode* root = make(6);
    root->left = make(2);
    root->right = make(8);
    root->left->left = make(0);
    root->left->right = make(4);
    root->left->right->left = make(3);
    root->left->right->right = make(5);
    root->right->left = make(7);
    root->right->right = make(9);
    return root;
  };

  ctx.test("split_at_root", [&] {
    std::deque<TreeNode> arena;
    TreeNode* root = build_sample(arena);
    TreeNode* p = root->left;   // 2
    TreeNode* q = root->right;  // 8

    ctx.expect_eq(sol.lowestCommonAncestor(root, p, q), root);
    ctx.expect_eq(sol.lowestCommonAncestorIterative(root, p, q), root);
  });

  ctx.test("ancestor_is_self_and_swapped_args", [&] {
    std::deque<TreeNode> arena;
    TreeNode* root = build_sample(arena);
    TreeNode* n2 = root->left;         // 2
    TreeNode* n4 = root->left->right;  // 4

    ctx.expect_eq(sol.lowestCommonAncestor(root, n2, n4), n2);
    ctx.expect_eq(sol.lowestCommonAncestorIterative(root, n2, n4), n2);
    ctx.expect_eq(sol.lowestCommonAncestor(root, n4, n2), n2);
    ctx.expect_eq(sol.lowestCommonAncestorIterative(root, n4, n2), n2);
  });

  ctx.test("deep_subtrees", [&] {
    std::deque<TreeNode> arena;
    TreeNode* root = build_sample(arena);
    TreeNode* n3 = root->left->right->left;
    TreeNode* n5 = root->left->right->right;
    TreeNode* n4 = root->left->right;
    TreeNode* n0 = root->left->left;
    TreeNode* n7 = root->right->left;
    TreeNode* n9 = root->right->right;

    ctx.expect_eq(sol.lowestCommonAncestor(root, n3, n5), n4);
    ctx.expect_eq(sol.lowestCommonAncestorIterative(root, n3, n5), n4);

    ctx.expect_eq(sol.lowestCommonAncestor(root, n0, n5), root->left);
    ctx.expect_eq(sol.lowestCommonAncestorIterative(root, n0, n5), root->left);

    ctx.expect_eq(sol.lowestCommonAncestor(root, n7, n9), root->right);
    ctx.expect_eq(sol.lowestCommonAncestorIterative(root, n7, n9), root->right);
  });

  ctx.test("edge_cases_null_and_single_node", [&] {
    TreeNode single(10);
    ctx.expect_eq(sol.lowestCommonAncestor(&single, &single, &single), &single);
    ctx.expect_eq(sol.lowestCommonAncestorIterative(&single, &single, &single),
                  &single);
    ctx.expect_eq(sol.lowestCommonAncestor(nullptr, &single, &single), nullptr);
    ctx.expect_eq(sol.lowestCommonAncestorIterative(nullptr, &single, &single),
                  nullptr);
  });
}

}  // namespace
