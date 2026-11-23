// LeetCode 236: Lowest Common Ancestor Of A Binary Tree
// https://leetcode.com/problems/lowest-common-ancestor-of-a-binary-tree/
//
// Problem:
//   Given a general binary tree (not necessarily a BST) and two nodes `p` and
//   `q` guaranteed to exist in the tree, find their lowest common ancestor
//   (LCA) — the deepest node in the tree that has both `p` and `q` as
//   descendants (allowing a node to be a descendant of itself).
//
// Examples:
//   Example 1:
//     Input:  root = [3, 5, 1, 6, 2, 0, 8, null, null, 7, 4], p = 5, q = 1
//     Output: 3
//     Explanation:
//       - Start post-order DFS at root = 3 and recurse into left = 5 and right = 1.
//       - At node 5, the base case node == p matches and returns lhs = 5 without
//         searching below.
//       - At node 1, the base case node == q matches and returns rhs = 1.
//       - Back at node 3, both lhs (5) and rhs (1) are non-null, so node 3 is
//         returned as the LCA.
//
//   Example 2:
//     Input:  root = [3, 5, 1, 6, 2, 0, 8, null, null, 7, 4], p = 5, q = 4
//     Output: 5
//     Explanation:
//       - From root = 3, recurse into left = 5 where node == p triggers the base
//         case and returns lhs = 5 immediately.
//       - In the right subtree rooted at 1 (with children 0 and 8), neither 5 nor 4
//         appears, so rhs returns nullptr.
//       - At node 3, lhs != nullptr ? lhs : rhs propagates node 5 upward as the
//         LCA.
//
//   Example 3:
//     Input:  root = [3, 5, 1, 6, 2, 0, 8, null, null, 7, 4], p = 7, q = 4
//     Output: 2
//     Explanation:
//       - Traverse from 3 into 5, where left child 6 returns nullptr, and descend
//         into node 2.
//       - At node 2, left child 7 matches p and returns lhs = 7, while right child
//         4 matches q and returns rhs = 4.
//       - Because both lhs and rhs are non-null at node 2, node 2 is returned up
//         through node 5 and node 3 (whose right subtree returns nullptr).
//
// Algorithm:
//   Post-order divide-and-conquer DFS:
//     - Base case: if `node == nullptr`, `node == p`, or `node == q`, return
//       `node` immediately. Note that when `node == p` and `q` lies inside `p`'s
//       subtree, returning `p` directly without searching below is still
//       correct by problem contract because `p` itself is the LCA.
//     - Recursively query `lhs = dfs(node->left)` and `rhs = dfs(node->right)`.
//     - Combine results bottom-up:
//         * If both `lhs != nullptr` and `rhs != nullptr`, `p` and `q` were
//           found in opposite subtrees, so `node` is their unique LCA.
//         * Otherwise return whichever of `lhs` or `rhs` is non-null (or
//           `nullptr` if neither target is in this subtree), bubbling up either
//           a found target or the already-identified LCA.
//   Time Complexity:  O(n) — visits each node at most once.
//   Space Complexity: O(h) recursion stack where h is tree height (O(n) worst
//                     case for a skewed tree).
//
// Data Structures:
//   - Binary tree (`TreeNode` pointers).
//   - Implicit DFS call stack.
//
// Real-World Applications:
//   - Git merge-base calculation (finding the lowest common ancestor commit in
//     a revision tree/DAG to perform a three-way merge).
//   - DOM event bubbling and shared-container resolution in web browsers
//     (finding the common ancestor element of two UI nodes).
//   - Compiler dominator-tree analysis and class-hierarchy type unification
//     (finding the least upper bound / most specific common superclass).
//
// Related LeetCode Problems:
//   - #235 Lowest Common Ancestor Of A Binary Search Tree [done] (O(h) BST-specialized version)
//   - #1644 Lowest Common Ancestor of a Binary Tree II [done] (targets may be absent from the tree)
//   - #1650 Lowest Common Ancestor of a Binary Tree III [done] (nodes have parent pointers, no root)
//   - #1123 Lowest Common Ancestor of Deepest Leaves [done] (LCA of all deepest nodes)

#include "framework/lc_framework.h"

#include <deque>

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
  // Finds the lowest common ancestor of `p` and `q` in a general binary tree
  // assuming both `p` and `q` exist in the tree.
  // Time: O(n), Space: O(h) recursion stack.
  TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
    // Post-order DFS returning:
    //   - `nullptr` if neither `p` nor `q` exists in `node`'s subtree,
    //   - `p` or `q` if exactly one of them is found (or one is an ancestor of
    //     the other),
    //   - the LCA node once both targets have been discovered in distinct
    //     subtrees.
    auto dfs = [&](this auto&& self, TreeNode* node) -> TreeNode* {
      if (node == nullptr || node == p || node == q) {
        return node;
      }

      TreeNode* lhs = self(node->left);
      TreeNode* rhs = self(node->right);

      // Both subtrees reported a hit: `node` is the lowest meeting point.
      if (lhs != nullptr && rhs != nullptr) {
        return node;
      }
      // Propagate whichever subtree found a target (or already found the LCA).
      return lhs != nullptr ? lhs : rhs;
    };

    return dfs(root);
  }
};

LC_PROBLEM(236, "Lowest Common Ancestor Of A Binary Tree", "tree", ctx) {
  Solution sol;

  // Shared sample binary tree:
  //         3
  //       /   \
  //      5     1
  //     / \   / \
  //    6   2 0   8
  //       / \
  //      7   4
  auto build_sample = [](std::deque<TreeNode>& arena) {
    auto make = [&](int v) -> TreeNode* { return &arena.emplace_back(v); };
    TreeNode* root = make(3);
    root->left = make(5);
    root->right = make(1);
    root->left->left = make(6);
    root->left->right = make(2);
    root->left->right->left = make(7);
    root->left->right->right = make(4);
    root->right->left = make(0);
    root->right->right = make(8);
    return root;
  };

  ctx.test("split_at_root_examples", [&] {
    std::deque<TreeNode> arena;
    TreeNode* root = build_sample(arena);
    TreeNode* n5 = root->left;
    TreeNode* n1 = root->right;
    TreeNode* n6 = root->left->left;
    TreeNode* n8 = root->right->right;

    // LeetCode example 1: LCA(5, 1) == 3.
    ctx.expect_eq(sol.lowestCommonAncestor(root, n5, n1), root);
    // Source build() example: LCA(6, 8) == 3.
    ctx.expect_eq(sol.lowestCommonAncestor(root, n6, n8), root);
  });

  ctx.test("one_node_is_ancestor_of_other", [&] {
    std::deque<TreeNode> arena;
    TreeNode* root = build_sample(arena);
    TreeNode* n5 = root->left;
    TreeNode* n4 = root->left->right->right;

    // LeetCode example 2: LCA(5, 4) == 5.
    ctx.expect_eq(sol.lowestCommonAncestor(root, n5, n4), n5);
    ctx.expect_eq(sol.lowestCommonAncestor(root, n4, n5), n5);
  });

  ctx.test("internal_subtrees", [&] {
    std::deque<TreeNode> arena;
    TreeNode* root = build_sample(arena);
    TreeNode* n6 = root->left->left;
    TreeNode* n2 = root->left->right;
    TreeNode* n7 = n2->left;
    TreeNode* n4 = n2->right;
    TreeNode* n0 = root->right->left;
    TreeNode* n8 = root->right->right;

    ctx.expect_eq(sol.lowestCommonAncestor(root, n7, n4), n2);
    ctx.expect_eq(sol.lowestCommonAncestor(root, n6, n4), root->left);
    ctx.expect_eq(sol.lowestCommonAncestor(root, n0, n8), root->right);
  });

  ctx.test("edge_cases_two_nodes_same_node_null", [&] {
    std::deque<TreeNode> arena;
    TreeNode* root = &arena.emplace_back(1);
    TreeNode* child = &arena.emplace_back(2);
    root->left = child;

    ctx.expect_eq(sol.lowestCommonAncestor(root, root, child), root);
    ctx.expect_eq(sol.lowestCommonAncestor(root, child, child), child);
    ctx.expect_eq(sol.lowestCommonAncestor(nullptr, root, child), nullptr);
  });
}

}  // namespace
