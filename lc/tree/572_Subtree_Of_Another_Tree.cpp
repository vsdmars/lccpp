// LeetCode 572: Subtree Of Another Tree
// https://leetcode.com/problems/subtree-of-another-tree/
//
// Problem:
//   Given the roots of two binary trees `root` and `subRoot`, return `true` if
//   there exists a subtree of `root` with the exact same structure and node
//   values as `subRoot`, and `false` otherwise. A subtree of `root` consists
//   of a node in `root` together with all of its descendants (so `root` itself
//   counts as a subtree, and partial truncations of a node's descendants do
//   not).
//
// Examples:
//   Example 1:
//     Input:  root = [3, 4, 5, 1, 2], subRoot = [4, 1, 2]
//     Output: true
//     Explanation:
//       - Initialize BFS frontier = [3] and dequeue 3; since 3 != sub_root->val
//         (4), skip isSameTree and enqueue children 4 and 5.
//       - Dequeue node 4, which matches sub_root->val (4), and call isSameTree(4,
//         4).
//       - isSameTree recursively verifies left child 1 == 1 and right child 2 == 2
//         with matching nullptr grandchildren, returning true.
//       - isSubtree immediately returns true without needing to visit node 5.
//
//   Example 2:
//     Input:  root = [3, 4, 5, 1, 2, null, null, null, null, 0], subRoot = [4, 1,
//             2]
//     Output: false
//     Explanation:
//       - Dequeue root 3 (3 != 4) and enqueue its children 4 and 5.
//       - Dequeue node 4 (4 == 4) and call isSameTree(4, 4): left child 1 matches,
//         but at right child 2, a->left is node 0 while b->left is nullptr, so
//         isSameTree returns false.
//       - Enqueue 4's children 1 and 2, then dequeue 5, 1, 2, and 0 in BFS order;
//         none has value 4.
//       - When frontier empties without a matching subtree, return false.
//
//   Example 3:
//     Input:  root = [1, 1], subRoot = [1]
//     Output: true
//     Explanation:
//       - Initialize frontier = [1 (root)] and dequeue root 1, whose value matches
//         sub_root->val (1).
//       - Call isSameTree(root, sub_root): root->left is non-null (node 1) while
//         sub_root->left is nullptr, so isSameTree returns false.
//       - Enqueue root->left (leaf 1), dequeue it, and call isSameTree(leaf,
//         sub_root) since 1 == 1.
//       - Both children of leaf 1 and sub_root are nullptr, so isSameTree returns
//         true and isSubtree returns true.
//
// Algorithm:
//   Traverse every candidate node in `root` via level-order Breadth-First
//   Search (BFS). Whenever a visited node's value matches `subRoot->val`,
//   invoke a recursive structural equality helper `isSameTree(node, subRoot)`
//   that verifies both trees have identical shape and node values all the way
//   down to null children. Return `true` immediately upon the first exact
//   match, or `false` if all nodes of `root` are exhausted without a match.
//   Time Complexity: O(m * n) in the worst case, where m = |root| and
//   n = |subRoot| (e.g., when many nodes share `subRoot->val`).
//   Space Complexity: O(m + h_n), for the BFS queue width (up to O(m)) plus
//   the `isSameTree` recursion stack (O(h_n), height of `subRoot`).
//
// Data Structures:
//   - Binary trees (`TreeNode`).
//   - FIFO queue (`std::queue<const TreeNode*>`) for BFS candidate traversal.
//
// Real-World Applications:
//   - AST pattern matching and code-clone detection in compilers, linters,
//     and static analysis tools.
//   - XML/HTML DOM fragment matching and query engines.
//   - Structural rule rewriting in symbolic algebra and database query
//     optimizers.
//
// Related LeetCode Problems:
//   - #100 Same Tree [done] (exact tree equality helper used at each candidate node)
//   - #101 Symmetric Tree [done] (paired recursive structural comparison)
//   - #652 Find Duplicate Subtrees [done] (identifying identical subtrees within one tree)
//   - #1367 Linked List in Binary Tree [new] (pattern matching a structure inside a binary tree)

#include "framework/lc_framework.h"

#include <deque>
#include <queue>

namespace {

// Standard binary tree node used by LeetCode tree problems.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;
};

class Solution {
 public:
  // Checks whether `sub_root` appears as an exact subtree anywhere in `root`.
  // Time: O(|root| * |sub_root|) worst case, Space: O(|root| + h_sub).
  bool isSubtree(const TreeNode* root, const TreeNode* sub_root) {
    // An empty tree is vacuously a subtree of any tree; a non-empty tree
    // cannot be a subtree of an empty tree.
    if (sub_root == nullptr) {
      return true;
    }
    if (root == nullptr) {
      return false;
    }

    // BFS over every node in `root`, triggering a full structural comparison
    // only when the candidate root's value matches `sub_root->val`.
    std::queue<const TreeNode*> frontier;
    frontier.push(root);

    while (!frontier.empty()) {
      const TreeNode* node = frontier.front();
      frontier.pop();

      if (node->val == sub_root->val && isSameTree(node, sub_root)) {
        return true;
      }

      if (node->left != nullptr) {
        frontier.push(node->left);
      }
      if (node->right != nullptr) {
        frontier.push(node->right);
      }
    }

    return false;
  }

 private:
  // Returns true iff trees `a` and `b` have identical structure and values.
  static bool isSameTree(const TreeNode* a, const TreeNode* b) {
    if (a == nullptr && b == nullptr) {
      return true;
    }
    if (a == nullptr || b == nullptr || a->val != b->val) {
      return false;
    }
    return isSameTree(a->left, b->left) && isSameTree(a->right, b->right);
  }
};

// RAII arena that owns test TreeNode instances without raw heap allocations.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    nodes_.push_back(TreeNode{.val = val, .left = left, .right = right});
    return &nodes_.back();
  }

 private:
  std::deque<TreeNode> nodes_;
};

LC_PROBLEM(572, "Subtree Of Another Tree", "tree", ctx) {
  Solution sol;

  ctx.test("example_1_exact_subtree_match", [&] {
    //     3              4
    //    / \            / \
    //   4   5          1   2
    //  / \
    // 1   2
    TreeArena arena;
    TreeNode* n4 = arena.make(4, arena.make(1), arena.make(2));
    TreeNode* root = arena.make(3, n4, arena.make(5));
    TreeNode* sub = arena.make(4, arena.make(1), arena.make(2));
    ctx.expect_true(sol.isSubtree(root, sub));
  });

  ctx.test("example_2_extra_descendant_breaks_match", [&] {
    //     3              4
    //    / \            / \
    //   4   5          1   2
    //  / \
    // 1   2
    //    /
    //   0
    TreeArena arena;
    TreeNode* n2 = arena.make(2, arena.make(0), nullptr);
    TreeNode* n4 = arena.make(4, arena.make(1), n2);
    TreeNode* root = arena.make(3, n4, arena.make(5));
    TreeNode* sub = arena.make(4, arena.make(1), arena.make(2));
    ctx.expect_false(sol.isSubtree(root, sub));
  });

  ctx.test("python_build_match_and_mismatch", [&] {
    //      3
    //     / \
    //    9  20
    //   /  /  \
    //  8  15   7
    TreeArena arena;
    TreeNode* n9 = arena.make(9, arena.make(8), nullptr);
    TreeNode* n20 = arena.make(20, arena.make(15), arena.make(7));
    TreeNode* root = arena.make(3, n9, n20);
    TreeNode* sub = arena.make(20, arena.make(15), arena.make(7));
    ctx.expect_true(sol.isSubtree(root, sub));

    // Adding child 11 under 7 makes `sub` no longer a complete subtree.
    n20->right->right = arena.make(11);
    ctx.expect_false(sol.isSubtree(root, sub));
  });

  ctx.test("duplicate_values_deeper_match", [&] {
    //   1
    //  /
    // 1   and subRoot = 1 -> matches the leaf 1.
    TreeArena arena;
    TreeNode* root = arena.make(1, arena.make(1), nullptr);
    TreeNode* sub = arena.make(1);
    ctx.expect_true(sol.isSubtree(root, sub));
  });

  ctx.test("tree_is_subtree_of_itself_and_null_cases", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(7, arena.make(3), arena.make(9));
    ctx.expect_true(sol.isSubtree(root, root));
    ctx.expect_true(sol.isSubtree(root, nullptr));
    ctx.expect_false(sol.isSubtree(nullptr, root));
  });
}

}  // namespace
