// LeetCode 110: Balanced Binary Tree
// https://leetcode.com/problems/balanced-binary-tree/
//
// Problem:
//   Given a binary tree, determine whether it is height-balanced. A binary
//   tree is height-balanced if, for every node in the tree, the heights of its
//   left and right subtrees differ by at most 1.
//
// Algorithm:
//   Single-pass bottom-up post-order Depth-First Search (DFS).
//   A naive top-down check recomputes subtree heights at every node in O(N^2)
//   worst-case time. Instead, our recursive helper returns a compact
//   `SubtreeInfo{balanced, height}` in one pass:
//     - Null node -> {true, 0}.
//     - Evaluate left subtree; if already unbalanced, short-circuit immediately
//       without traversing the right subtree.
//     - Evaluate right subtree; if unbalanced or |left.height - right.height|
//       > 1, propagate {false, 0}.
//     - Otherwise return {true, 1 + max(left.height, right.height)}.
//   Time Complexity:  O(N), visiting each node at most once (and often fewer
//                     thanks to early short-circuiting on imbalance).
//   Space Complexity: O(H) recursion stack depth (O(log N) balanced, O(N)
//                     worst case for a skewed tree).
//
// Data Structures:
//   - Binary tree nodes (`TreeNode` with value, left, and right pointers).
//   - Value return struct (`SubtreeInfo`) carrying balance flag and height.
//   - RAII `TreeArena` (`std::deque<TreeNode>`) in tests for leak-free node
//     allocation.
//
// Real-World Applications:
//   - Verifying AVL tree invariants in unit tests, fuzzers, and database
//     storage-engine consistency checkers.
//   - Detecting tree skew in self-balancing index structures and bounding
//     volume hierarchies (BVHs) to trigger rebalancing or rebuilds.
//   - Monitoring partition-tree health in distributed range-sharded stores.
//
// Related LeetCode Problems:
//   - #104 Maximum Depth Of Binary Tree [done] (computes subtree height via DFS)
//   - #543 Diameter of Binary Tree [done] (single-pass bottom-up height DFS)
//   - #108 Convert Sorted Array to Binary Search Tree [done] (builds balanced BST)
//   - #1382 Balance a Binary Search Tree [done] (rebalances an arbitrary BST)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <deque>
#include <optional>
#include <queue>
#include <vector>

namespace {

struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;
};

// Result returned by bottom-up DFS for a subtree: whether it satisfies the
// AVL height-balance invariant and, if so, its height in nodes.
struct SubtreeInfo {
  bool balanced = true;
  int height = 0;
};

// RAII arena that owns TreeNode instances in a std::deque so raw pointers
// stay valid as new nodes are appended and are automatically freed on exit.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    TreeNode& node = nodes_.emplace_back();
    node.val = val;
    node.left = left;
    node.right = right;
    return &node;
  }

  // Constructs a binary tree from LeetCode's level-order array representation.
  TreeNode* fromLevelOrder(const std::vector<std::optional<int>>& vals) {
    if (vals.empty() || !vals[0].has_value()) {
      return nullptr;
    }
    TreeNode* root = make(*vals[0]);
    std::queue<TreeNode*> q;
    q.push(root);
    std::size_t idx = 1uz;
    while (!q.empty() && idx < vals.size()) {
      TreeNode* cur = q.front();
      q.pop();
      if (vals[idx].has_value()) {
        cur->left = make(*vals[idx]);
        q.push(cur->left);
      }
      ++idx;
      if (idx < vals.size() && vals[idx].has_value()) {
        cur->right = make(*vals[idx]);
        q.push(cur->right);
      }
      ++idx;
    }
    return root;
  }

 private:
  std::deque<TreeNode> nodes_;
};

class Solution {
 public:
  // Returns true iff every node's left and right subtree heights differ by <= 1.
  // Time: O(N), Space: O(H) recursion stack.
  bool isBalanced(const TreeNode* root) { return checkSubtree(root).balanced; }

 private:
  // Post-order DFS returning balance status and height in a single pass.
  static SubtreeInfo checkSubtree(const TreeNode* node) {
    if (node == nullptr) {
      return {.balanced = true, .height = 0};
    }

    // Short-circuit immediately if the left subtree already violates balance.
    const SubtreeInfo left = checkSubtree(node->left);
    if (!left.balanced) {
      return {.balanced = false, .height = 0};
    }

    const SubtreeInfo right = checkSubtree(node->right);
    if (!right.balanced || std::abs(left.height - right.height) > 1) {
      return {.balanced = false, .height = 0};
    }

    return {
        .balanced = true,
        .height = 1 + std::max(left.height, right.height),
    };
  }
};

LC_PROBLEM(110, "Balanced Binary Tree", "dfs", ctx) {
  Solution sol;

  ctx.test("example_1_balanced", [&] {
    TreeArena arena;
    const TreeNode* root =
        arena.fromLevelOrder({3, 9, 20, std::nullopt, std::nullopt, 15, 7});
    ctx.expect_true(sol.isBalanced(root));
  });

  ctx.test("example_2_unbalanced_left", [&] {
    TreeArena arena;
    const TreeNode* root = arena.fromLevelOrder(
        {1, 2, 2, 3, 3, std::nullopt, std::nullopt, 4, 4});
    ctx.expect_false(sol.isBalanced(root));
  });

  ctx.test("python_build_unbalanced_right", [&] {
    TreeArena arena;
    TreeNode* n8 = arena.make(8);
    TreeNode* n7 = arena.make(7, nullptr, n8);
    TreeNode* n15 = arena.make(15);
    TreeNode* n20 = arena.make(20, n15, n7);
    TreeNode* n9 = arena.make(9);
    const TreeNode* root = arena.make(3, n9, n20);
    ctx.expect_false(sol.isBalanced(root));
  });

  ctx.test("empty_and_single_node", [&] {
    TreeArena arena;
    ctx.expect_true(sol.isBalanced(nullptr));
    ctx.expect_true(sol.isBalanced(arena.make(10)));
  });

  ctx.test("root_balanced_but_inner_subtree_unbalanced", [&] {
    TreeArena arena;
    // Left height = 3, right height = 3 at root, yet each side is skewed.
    TreeNode* ll = arena.make(4);
    TreeNode* l = arena.make(2, ll, nullptr);
    TreeNode* rr = arena.make(5);
    TreeNode* r = arena.make(3, nullptr, rr);
    const TreeNode* root = arena.make(1, l, r);
    // Note: l has heights (1, 0), r has heights (0, 1) -> actually balanced.
    ctx.expect_true(sol.isBalanced(root));

    // Add one more child under ll so node 2 has heights (2, 0) -> unbalanced.
    ll->left = arena.make(6);
    rr->right = arena.make(7);
    ctx.expect_false(sol.isBalanced(root));
  });
}

}  // namespace
