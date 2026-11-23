// LeetCode 1123: Lowest Common Ancestor Of Deepest Leaves
// https://leetcode.com/problems/lowest-common-ancestor-of-deepest-leaves/
//
// Problem:
//   Given the root of a binary tree (1 <= n <= 1000, unique node values),
//   return the lowest common ancestor (LCA) of all of its deepest leaves.
//   The depth of the root is 0, each child has depth `parent_depth + 1`, and
//   the LCA of a set S of nodes is the deepest node whose subtree contains
//   every node in S.
//
// Examples:
//   Example 1:
//     Input:  root = [3, 5, 1, 6, 2, 0, 8, null, null, 7, 4]
//     Output: node 2 (subtree [2, 7, 4])
//     Explanation:
//       - Bottom-up dfs returns {n7, 1} and {n4, 1} for leaves 7 and 4 because both
//         null children have height 0.
//       - At node 2, left_height == right_height == 1, so dfs returns {n2, 2} as
//         the LCA of deepest leaves 7 and 4.
//       - At node 5, left returns {n6, 1} and right returns {n2, 2}; since
//         right_height > left_height, node 5 propagates {n2, 3}.
//       - In the right subtree of 3, leaves 0 and 8 each return height 1 so node 1
//         returns {n1, 2}.
//       - At root 3, left_height (3) > right_height (2), so dfs returns {n2, 4} and
//         lcaDeepestLeaves returns node 2.
//
//   Example 2:
//     Input:  root = [0, 1, 3, null, 2]
//     Output: node 2
//     Explanation:
//       - Leaf 2 sees two null children of height 0 and returns {n2, 1}.
//       - At node 1, left is {nullptr, 0} and right is {n2, 1}; because
//         right_height > left_height, node 1 returns {n2, 2}.
//       - Leaf 3 returns {n3, 1}, so at root 0 left_height (2) > right_height (1)
//         yields {n2, 3} and returns node 2.
//
//   Example 3:
//     Input:  root = [1]
//     Output: node 1
//     Explanation:
//       - dfs recurses on both null children of node 1, each returning {nullptr,
//         0}.
//       - Because left_height == right_height == 0, dfs returns {n1, 1} and
//         lcaDeepestLeaves returns node 1.
//
// Algorithm:
//   Run a single bottom-up (post-order) DFS returning `{lca, height}` for each
//   subtree, where `height` is the maximum number of nodes along any downward
//   path in that subtree and `lca` is the LCA of all deepest leaves within it:
//     - Base case: `nullptr` returns `{nullptr, 0}`.
//     - Recurse on `node->left` and `node->right`.
//     - If `left.height > right.height`, all deepest leaves of this subtree
//       live exclusively in the left subtree, so return `{left.lca, left.height + 1}`.
//     - If `right.height > left.height`, similarly return `{right.lca, right.height + 1}`.
//     - If `left.height == right.height`, deepest leaves exist in both
//       subtrees (or `node` itself is a leaf when both heights are 0), making
//       `node` their LCA: return `{node, left.height + 1}`.
//
//   Time Complexity:  O(n), visiting every node once.
//   Space Complexity: O(h) recursion stack, where h is the tree height.
//
// Data Structures:
//   - Binary tree (`TreeNode`).
//   - Lightweight `SubtreeResult` value struct (`lca` pointer + subtree
//     `height`) returned up the DFS call stack.
//
// Real-World Applications:
//   - Version-control / dependency-tree analysis: finding the narrowest common
//     ancestor package or commit that governs the deepest frontier of a build.
//   - Compiler dominator-tree queries: locating the immediate common scope
//     enclosing the deepest nested basic blocks or expressions.
//   - Phylogenetic tree analysis: identifying the most recent common ancestor
//     of the most-diverged leaf taxa in an evolutionary tree.
//
// Related LeetCode Problems:
//   - #865 Smallest Subtree with all the Deepest Nodes [done] (identical problem statement)
//   - #236 Lowest Common Ancestor of a Binary Tree [done] (LCA of two specified nodes via post-order DFS)
//   - #104 Maximum Depth of Binary Tree [done] (computing subtree heights bottom-up)
//   - #1644 Lowest Common Ancestor of a Binary Tree II [done] (post-order LCA with existence tracking)

#include "framework/lc_framework.h"

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
  // Returns a pointer to the lowest common ancestor of the deepest leaves in
  // the binary tree rooted at `root`.
  //
  // Complexity: O(n) time, O(h) auxiliary stack space.
  TreeNode* lcaDeepestLeaves(TreeNode* root) {
    struct SubtreeResult {
      TreeNode* lca = nullptr;
      int height = 0;
    };

    // Post-order DFS returning the LCA of the deepest leaves in `node`'s
    // subtree together with that subtree's height.
    auto dfs = [](this auto&& self, TreeNode* node) -> SubtreeResult {
      if (node == nullptr) {
        return {.lca = nullptr, .height = 0};
      }

      const auto [left_lca, left_height] = self(node->left);
      const auto [right_lca, right_height] = self(node->right);

      // Strictly deeper side owns all deepest leaves in this subtree.
      if (left_height > right_height) {
        return {.lca = left_lca, .height = left_height + 1};
      }
      if (right_height > left_height) {
        return {.lca = right_lca, .height = right_height + 1};
      }
      // Equal heights (including 0 == 0 at a leaf) mean `node` is the lowest
      // node whose subtree covers every deepest leaf below it.
      return {.lca = node, .height = left_height + 1};
    };

    return dfs(root).lca;
  }
};

LC_PROBLEM(1123, "Lowest Common Ancestor Of Deepest Leaves", "dfs", ctx) {
  Solution sol;

  ctx.test("example_1_two_deepest_leaves", [&] {
    // Tree: [3,5,1,6,2,0,8,null,null,7,4] -> deepest leaves 7 and 4 -> LCA 2.
    TreeArena arena;
    TreeNode* n7 = arena.make(7);
    TreeNode* n4 = arena.make(4);
    TreeNode* n2 = arena.make(2, n7, n4);
    TreeNode* n6 = arena.make(6);
    TreeNode* n5 = arena.make(5, n6, n2);
    TreeNode* n0 = arena.make(0);
    TreeNode* n8 = arena.make(8);
    TreeNode* n1 = arena.make(1, n0, n8);
    TreeNode* root = arena.make(3, n5, n1);

    TreeNode* lca = sol.lcaDeepestLeaves(root);
    ctx.expect_eq(lca, n2);
    if (lca != nullptr) {
      ctx.expect_eq(lca->val, 2);
    }
  });

  ctx.test("example_2_single_node", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(1);
    ctx.expect_eq(sol.lcaDeepestLeaves(root), root);
  });

  ctx.test("example_3_single_deepest_leaf", [&] {
    // Tree: [0,1,3,null,2] -> deepest leaf is 2 -> LCA is 2 itself.
    TreeArena arena;
    TreeNode* n2 = arena.make(2);
    TreeNode* n1 = arena.make(1, nullptr, n2);
    TreeNode* n3 = arena.make(3);
    TreeNode* root = arena.make(0, n1, n3);

    ctx.expect_eq(sol.lcaDeepestLeaves(root), n2);
  });

  ctx.test("asymmetric_left_heavy_tree", [&] {
    // Source build() tree: 1 -> left 2 -> (left 3, right 4 -> left 5).
    TreeArena arena;
    TreeNode* n5 = arena.make(5);
    TreeNode* n4 = arena.make(4, n5, nullptr);
    TreeNode* n3 = arena.make(3);
    TreeNode* n2 = arena.make(2, n3, n4);
    TreeNode* root = arena.make(1, n2, nullptr);

    ctx.expect_eq(sol.lcaDeepestLeaves(root), n5);
  });

  ctx.test("full_symmetric_tree_returns_root", [&] {
    TreeArena arena;
    TreeNode* root =
        arena.make(1, arena.make(2, arena.make(4), arena.make(5)),
                   arena.make(3, arena.make(6), arena.make(7)));
    ctx.expect_eq(sol.lcaDeepestLeaves(root), root);
  });

  ctx.test("empty_tree_returns_null", [&] {
    ctx.expect_eq(sol.lcaDeepestLeaves(nullptr), nullptr);
  });
}

}  // namespace
