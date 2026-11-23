// LeetCode 285: Inorder Successor In BST
// https://leetcode.com/problems/inorder-successor-in-bst/
//
// Problem:
//   Given the root of a binary search tree (BST) with unique node values and a
//   node `p` in the tree, return the in-order successor of `p` in the BST
//   (the node with the smallest key strictly greater than `p->val`). If `p`
//   has no in-order successor (i.e., `p` holds the maximum value), return
//   `nullptr`.
//
// Examples:
//   Example 1:
//     Input:  root = [2, 1, 3], p = 1
//     Output: 2
//     Explanation:
//       - Start at root 2; since p->val (1) < 2, mark node 2 as a candidate
//         successor and recurse into node->left (node 1).
//       - At node 1, p->val (1) < 1 is false, so recurse into node->right
//         (nullptr), which returns nullptr.
//       - Back at node 2, left_candidate is nullptr, so return node 2 as the
//         inorder successor.
//
//   Example 2:
//     Input:  root = [10, 5, 20, null, null, 15, 25, 12], p = 10
//     Output: 12
//     Explanation:
//       - Start at root 10; since p->val (10) < 10 is false, discard 10 and its
//         left subtree and move right to node 20.
//       - At node 20, 10 < 20 holds, so record 20 as a candidate successor and move
//         left to node 15.
//       - At node 15, 10 < 15 holds, so update the candidate successor to 15 and
//         move left to node 12.
//       - At node 12, 10 < 12 holds, so update the candidate to 12 and move left to
//         nullptr, returning node 12.
//
//   Example 3:
//     Input:  root = [5, 3, 7, 2, 4], p = 7
//     Output: null
//     Explanation:
//       - Start at root 5; since p->val (7) < 5 is false, descend right to node 7.
//       - At node 7, 7 < 7 is false, so descend into node->right (nullptr).
//       - Reaching nullptr indicates no key is strictly greater than 7, so return
//         nullptr.
//
// Algorithm:
//   Exploit the BST ordering property to locate the smallest key strictly
//   greater than `p->val` without performing a full inorder traversal:
//   1. `inorderSuccessor` (recursive BST descent, matching the Python source):
//      - If `p->val < node->val`, `node` is a valid candidate successor. Recurse
//        left to see if a tighter (smaller) candidate exists; if the left call
//        returns `nullptr`, `node` itself is the answer.
//      - Otherwise (`p->val >= node->val`), `node` and its entire left subtree
//        cannot be the successor, so recurse into `node->right`.
//   2. `inorderSuccessorIterative` (iterative O(1)-space descent):
//      Walk down from `root`, updating `successor = cur` whenever moving left
//      (`p->val < cur->val`), and moving right otherwise.
//   - Time Complexity:  O(h), where h is the height of the BST (O(log n) for
//     balanced trees, O(n) worst case for skewed trees).
//   - Space Complexity: O(h) for recursive `inorderSuccessor`, O(1) for
//     iterative `inorderSuccessorIterative`.
//
// Data Structures:
//   - Binary tree nodes (`TreeNode`).
//   - Single pointer tracking the tightest ancestor candidate strictly greater
//     than `p->val`.
//
// Real-World Applications:
//   - Ordered container `std::map::upper_bound` / iterator increment logic in
//     search trees without parent pointers.
//   - Event-driven simulation schedulers querying the next scheduled event
//     strictly after a given timestamp.
//   - Database B-tree range-scan continuation (resuming a paginated query from
//     the last returned key).
//
// Related LeetCode Problems:
//   - #510 Inorder Successor in BST II [done] (finding successor using parent pointers without root)
//   - #173 Binary Search Tree Iterator [done] (stepping through every inorder successor sequentially)
//   - #270 Closest Binary Search Tree Value [done] (same O(h) candidate-tracking BST descent)
//   - #450 Delete Node in a BST [done] (uses the inorder successor to replace a deleted two-child node)
//   - #98 Validate Binary Search Tree [done] (BST ordering invariant)

#include "framework/lc_framework.h"

#include <deque>

namespace {

// Binary tree node definition matching LeetCode's standard representation.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int v) : val(v) {}
  TreeNode(int v, TreeNode* l, TreeNode* r) : val(v), left(l), right(r) {}
};

// RAII arena for test trees; std::deque guarantees pointer stability on growth.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    nodes_.emplace_back(val, left, right);
    return &nodes_.back();
  }

 private:
  std::deque<TreeNode> nodes_;
};

class Solution {
 public:
  // Finds the in-order successor of `p` in the BST rooted at `root` using
  // recursive BST descent. Returns `nullptr` if no successor exists.
  // Time: O(h), Space: O(h).
  TreeNode* inorderSuccessor(TreeNode* root, const TreeNode* p) {
    if (root == nullptr || p == nullptr) {
      return nullptr;
    }

    auto dfs = [&](this auto&& self, TreeNode* node) -> TreeNode* {
      if (node == nullptr) {
        return nullptr;
      }

      if (p->val < node->val) {
        // `node` is strictly greater than `p`, so it is a candidate successor.
        // Prefer any closer successor found in the left subtree; if none
        // exists, `node` itself is the tightest upper bound.
        TreeNode* left_candidate = self(node->left);
        return (left_candidate != nullptr) ? left_candidate : node;
      }

      // `node->val <= p->val`: neither `node` nor anything in its left subtree
      // can exceed `p->val`, so the successor must lie in the right subtree.
      return self(node->right);
    };

    return dfs(root);
  }

  // Iterative O(1)-space BST descent finding the same in-order successor.
  // Time: O(h), Space: O(1).
  TreeNode* inorderSuccessorIterative(TreeNode* root, const TreeNode* p) {
    if (root == nullptr || p == nullptr) {
      return nullptr;
    }

    TreeNode* successor = nullptr;
    TreeNode* cur = root;

    while (cur != nullptr) {
      if (p->val < cur->val) {
        // Record `cur` as the closest known upper bound and search left for a
        // strictly smaller value that still exceeds `p->val`.
        successor = cur;
        cur = cur->left;
      } else {
        // Discard `cur` and its left subtree; move right toward larger keys.
        cur = cur->right;
      }
    }

    return successor;
  }
};

LC_PROBLEM(285, "Inorder Successor In BST", "binary_search_tree", ctx) {
  Solution sol;

  ctx.test("python_build_all_nodes", [&] {
    TreeArena arena;
    //       5
    //      / \
    //     3   7
    //    / \
    //   2   4
    TreeNode* n2 = arena.make(2);
    TreeNode* n4 = arena.make(4);
    TreeNode* n3 = arena.make(3, n2, n4);
    TreeNode* n7 = arena.make(7);
    TreeNode* root = arena.make(5, n3, n7);

    // Verify full inorder successor chain: 2 -> 3 -> 4 -> 5 -> 7 -> nullptr.
    ctx.expect_eq(sol.inorderSuccessor(root, n2), n3);
    ctx.expect_eq(sol.inorderSuccessorIterative(root, n2), n3);

    ctx.expect_eq(sol.inorderSuccessor(root, n3), n4);
    ctx.expect_eq(sol.inorderSuccessorIterative(root, n3), n4);

    ctx.expect_eq(sol.inorderSuccessor(root, n4), root);
    ctx.expect_eq(sol.inorderSuccessorIterative(root, n4), root);

    ctx.expect_eq(sol.inorderSuccessor(root, root), n7);
    ctx.expect_eq(sol.inorderSuccessorIterative(root, root), n7);

    ctx.expect_eq(sol.inorderSuccessor(root, n7), nullptr);
    ctx.expect_eq(sol.inorderSuccessorIterative(root, n7), nullptr);
  });

  ctx.test("leetcode_example_1_and_right_only_tree", [&] {
    TreeArena arena;
    //   2
    //  / \
    // 1   3
    TreeNode* n1 = arena.make(1);
    TreeNode* n3 = arena.make(3);
    TreeNode* root = arena.make(2, n1, n3);

    ctx.expect_eq(sol.inorderSuccessor(root, n1), root);
    ctx.expect_eq(sol.inorderSuccessorIterative(root, n1), root);
    ctx.expect_eq(sol.inorderSuccessor(root, root), n3);
    ctx.expect_eq(sol.inorderSuccessorIterative(root, root), n3);
    ctx.expect_eq(sol.inorderSuccessor(root, n3), nullptr);
    ctx.expect_eq(sol.inorderSuccessorIterative(root, n3), nullptr);
  });

  ctx.test("deep_leftmost_in_right_subtree", [&] {
    TreeArena arena;
    //       10
    //      /  \
    //     5    20
    //         /  \
    //       15    25
    //       /
    //     12
    TreeNode* n12 = arena.make(12);
    TreeNode* n15 = arena.make(15, n12, nullptr);
    TreeNode* n20 = arena.make(20, n15, arena.make(25));
    TreeNode* root = arena.make(10, arena.make(5), n20);

    // Successor of 10 is the leftmost descendant of its right subtree (12).
    ctx.expect_eq(sol.inorderSuccessor(root, root), n12);
    ctx.expect_eq(sol.inorderSuccessorIterative(root, root), n12);
  });

  ctx.test("null_inputs_safe", [&] {
    TreeArena arena;
    TreeNode* single = arena.make(1);
    ctx.expect_eq(sol.inorderSuccessor(nullptr, single), nullptr);
    ctx.expect_eq(sol.inorderSuccessorIterative(nullptr, single), nullptr);
    ctx.expect_eq(sol.inorderSuccessor(single, nullptr), nullptr);
    ctx.expect_eq(sol.inorderSuccessorIterative(single, nullptr), nullptr);
  });
}

}  // namespace
