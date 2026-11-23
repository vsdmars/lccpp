// LeetCode 865: Smallest Subtree With All The Deepest Nodes
// https://leetcode.com/problems/smallest-subtree-with-all-the-deepest-nodes/
//
// Problem:
//   Given the root of a binary tree, the depth of each node is its shortest
//   distance to the root, and a node is deepest if it achieves the maximum
//   depth in the entire tree. Return the root of the smallest (i.e., deepest)
//   subtree that contains all of the deepest nodes in the tree.
//
// Algorithm:
//   Perform a single bottom-up post-order DFS returning a `(height, lca)` pair
//   for each subtree:
//     - Base case: `nullptr` has height `0` and `lca = nullptr`.
//     - Recurse on `node->left` and `node->right` to get `(ld, lnode)` and
//       `(rd, rnode)`.
//     - If `ld == rd`, both sides reach the exact same maximum depth (including
//       the leaf case `0 == 0`), so `node` itself is the lowest common ancestor
//       covering all deepest nodes in this subtree: return `{ld + 1, node}`.
//     - If `ld > rd`, all deepest nodes in `node`'s subtree lie strictly inside
//       the left subtree: propagate `{ld + 1, lnode}`.
//     - Otherwise (`rd > ld`), propagate `{rd + 1, rnode}`.
//   Time Complexity:  O(n), visiting every node once.
//   Space Complexity: O(h) recursion stack where h is the tree height.
//
// Data Structures:
//   - Binary tree nodes (`TreeNode`).
//   - Lightweight `SubtreeResult` struct `{int depth, TreeNode* lca}` returned
//     up the DFS call stack.
//
// Real-World Applications:
//   - Version-control / DAG ancestry: finding the most recent common ancestor
//     (merge-base) spanning the deepest active development branches.
//   - DOM / UI layout invalidation: locating the smallest enclosing container
//     subtree that covers all deepest dirty elements.
//   - Distributed tracing: identifying the narrowest parent span encompassing
//     all maximum-depth leaf spans in a trace tree.
//
// Related LeetCode Problems:
//   - #1123 Lowest Common Ancestor of Deepest Leaves [done] (identical problem under a different title)
//   - #236 Lowest Common Ancestor of a Binary Tree [done] (LCA of two specific nodes via post-order DFS)
//   - #104 Maximum Depth of Binary Tree [done] (computing subtree height via post-order DFS)
//   - #1644 Lowest Common Ancestor of a Binary Tree II [done] (LCA variant handling possibly absent nodes)

#include "framework/lc_framework.h"

#include <deque>
#include <queue>
#include <vector>

namespace {

// Standard binary tree node.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int v, TreeNode* l = nullptr, TreeNode* r = nullptr)
      : val(v), left(l), right(r) {}
};

// RAII arena owning test TreeNode allocations.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    return &nodes_.emplace_back(val, left, right);
  }

 private:
  std::deque<TreeNode> nodes_;
};

// Collects node values in level-order for concise subtree assertions in tests.
std::vector<int> level_order_values(const TreeNode* root) {
  std::vector<int> values;
  if (root == nullptr) {
    return values;
  }
  std::queue<const TreeNode*> q;
  q.push(root);
  while (!q.empty()) {
    const TreeNode* curr = q.front();
    q.pop();
    values.push_back(curr->val);
    if (curr->left != nullptr) {
      q.push(curr->left);
    }
    if (curr->right != nullptr) {
      q.push(curr->right);
    }
  }
  return values;
}

class Solution {
 public:
  // Returns the root of the smallest subtree containing all deepest nodes.
  // Time: O(n), Space: O(h).
  TreeNode* subtreeWithAllDeepest(TreeNode* root) {
    struct SubtreeResult {
      int depth = 0;
      TreeNode* lca = nullptr;
    };

    auto dfs = [](this auto&& self, TreeNode* node) -> SubtreeResult {
      if (node == nullptr) {
        return {.depth = 0, .lca = nullptr};
      }

      const SubtreeResult left = self(node->left);
      const SubtreeResult right = self(node->right);

      // Equal depths mean deepest nodes exist on both sides (or `node` is a
      // leaf), making `node` the lowest common ancestor for this subtree.
      if (left.depth == right.depth) {
        return {.depth = left.depth + 1, .lca = node};
      }

      // Otherwise, only the deeper side contains the subtree's deepest nodes.
      if (left.depth > right.depth) {
        return {.depth = left.depth + 1, .lca = left.lca};
      }
      return {.depth = right.depth + 1, .lca = right.lca};
    };

    return dfs(root).lca;
  }
};

LC_PROBLEM(865, "Smallest Subtree With All The Deepest Nodes", "tree", ctx) {
  ctx.test("example1_two_deepest_siblings", [&] {
    //         3
    //       /   \
    //      5     1
    //     / \   / \
    //    6   2 0   8
    //       / \
    //      7   4
    // Deepest nodes are 7 and 4 -> smallest enclosing subtree is [2, 7, 4].
    TreeArena arena;
    TreeNode* n2 = arena.make(2, arena.make(7), arena.make(4));
    TreeNode* n5 = arena.make(5, arena.make(6), n2);
    TreeNode* n1 = arena.make(1, arena.make(0), arena.make(8));
    TreeNode* root = arena.make(3, n5, n1);

    Solution sol;
    TreeNode* ans = sol.subtreeWithAllDeepest(root);
    ctx.expect_eq(ans, n2);
    ctx.expect_eq(level_order_values(ans), std::vector<int>({2, 7, 4}));
  });

  ctx.test("single_node_returns_root", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(1);
    Solution sol;
    ctx.expect_eq(sol.subtreeWithAllDeepest(root), root);
  });

  ctx.test("single_deepest_leaf_returns_itself", [&] {
    //      0
    //     / \
    //    1   3
    //     \
    //      2
    TreeArena arena;
    TreeNode* n2 = arena.make(2);
    TreeNode* n1 = arena.make(1, nullptr, n2);
    TreeNode* root = arena.make(0, n1, arena.make(3));
    Solution sol;
    ctx.expect_eq(sol.subtreeWithAllDeepest(root), n2);
  });

  ctx.test("perfect_binary_tree_returns_root", [&] {
    //       1
    //      / \
    //     2   3
    //    / \ / \
    //   4  5 6  7
    TreeArena arena;
    TreeNode* left = arena.make(2, arena.make(4), arena.make(5));
    TreeNode* right = arena.make(3, arena.make(6), arena.make(7));
    TreeNode* root = arena.make(1, left, right);
    Solution sol;
    ctx.expect_eq(sol.subtreeWithAllDeepest(root), root);
  });

  ctx.test("skewed_chain_returns_deepest_leaf", [&] {
    TreeArena arena;
    TreeNode* n4 = arena.make(4);
    TreeNode* n3 = arena.make(3, n4, nullptr);
    TreeNode* n2 = arena.make(2, n3, nullptr);
    TreeNode* root = arena.make(1, n2, nullptr);
    Solution sol;
    ctx.expect_eq(sol.subtreeWithAllDeepest(root), n4);
  });

  ctx.test("empty_tree_returns_nullptr", [&] {
    Solution sol;
    ctx.expect_eq(sol.subtreeWithAllDeepest(nullptr), nullptr);
  });
}

}  // namespace
