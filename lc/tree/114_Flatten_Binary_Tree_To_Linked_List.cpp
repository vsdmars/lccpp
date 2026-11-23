// LeetCode 114: Flatten Binary Tree To Linked List
// https://leetcode.com/problems/flatten-binary-tree-to-linked-list/
//
// Problem:
//   Given the root of a binary tree, flatten the tree into a "linked list"
//   in-place using the same TreeNode instances:
//     - The right child pointer points to the next node in the list.
//     - The left child pointer is always nullptr.
//     - The list order must match the binary tree's preorder traversal
//       (root, left subtree, right subtree).
//
// Algorithm:
//   Two clean in-place approaches (matching both methods in the Python source):
//     1. Iterative Preorder Stack (`flatten`):
//        Maintain an explicit LIFO stack seeded with `root`. In each iteration,
//        pop `curr`, push its right child (if any) followed by its left child
//        (if any) so the left subtree is visited first. Then detach
//        `curr->left = nullptr` and wire `curr->right` to the top of the stack
//        (or `nullptr` if the stack is now empty). Because `stack.back()` is
//        always the next node in preorder, the linked list is stitched in one
//        forward pass.
//     2. Recursive Tail-Splicing (`flattenRecursive`):
//        Recursively flatten the left subtree (returning its tail `left_tail`)
//        and the right subtree (returning its tail). Splice the flattened left
//        list between `node` and its original right subtree by setting
//        `node->right = node->left`, `node->left = nullptr`, and
//        `left_tail->right = right_sub`. Return the rightmost tail node of the
//        combined subtree so the parent can splice in O(1).
//   (Note: A Morris-style predecessor relinking variant can reduce auxiliary
//    space to O(1) while preserving O(N) time.)
//   Time Complexity:  O(N) for both approaches, where N is the number of nodes.
//   Space Complexity: O(H) for both approaches (explicit or implicit stack),
//                     where H is the tree height (O(N) worst case).
//
// Data Structures:
//   - Binary tree (`TreeNode`) rewired in-place into a singly linked list via
//     `right` pointers.
//   - `std::vector<TreeNode*>` used as a LIFO stack in the iterative solution.
//
// Real-World Applications:
//   - Linearizing hierarchical scene graphs, DOM subtrees, or ASTs into flat
//     iteration sequences for cache-friendly rendering or serialization.
//   - Converting nested UI/menu hierarchies or outlines into flat ordered
//     lists while reusing existing node allocations.
//   - Preorder linearization of expression trees for bytecode emission.
//
// Related LeetCode Problems:
//   - #144 Binary Tree Preorder Traversal [new] (defines the exact order of the flattened list)
//   - #430 Flatten a Multilevel Doubly Linked List [done] (in-place flattening of nested child lists)
//   - #897 Increasing Order Search Tree [done] (rewires a BST into a right-skewed chain in inorder)
//   - #426 Convert Binary Search Tree to Sorted Doubly Linked List [done] (in-place tree-to-list pointer rewiring)
//   - #109 Convert Sorted List to Binary Search Tree [done] (inverse operation: list to balanced tree)

#include "framework/lc_framework.h"

#include <deque>
#include <vector>

namespace {

// Standard binary tree node.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int value) : val(value) {}
  TreeNode(int value, TreeNode* left_child, TreeNode* right_child)
      : val(value), left(left_child), right(right_child) {}
};

// Arena owning TreeNode instances for leak-free test tree construction.
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
  // Flattens the binary tree in-place into a preorder right-linked list using
  // an explicit stack.
  //
  // Invariant: at the end of each iteration, all previously popped nodes form a
  // valid right-linked chain with null left pointers, and `stack.back()` (when
  // non-empty) is the exact next node in preorder traversal.
  // Time: O(N), Space: O(H).
  void flatten(TreeNode* root) {
    if (root == nullptr) {
      return;
    }

    std::vector<TreeNode*> stack;
    stack.push_back(root);

    while (!stack.empty()) {
      TreeNode* curr = stack.back();
      stack.pop_back();

      // Push right child before left child so left is popped first (preorder).
      if (curr->right != nullptr) {
        stack.push_back(curr->right);
      }
      if (curr->left != nullptr) {
        stack.push_back(curr->left);
      }

      // Rewire curr to point directly to the next preorder node.
      curr->left = nullptr;
      curr->right = stack.empty() ? nullptr : stack.back();
    }
  }

  // Flattens the binary tree in-place using recursive tail-returning DFS.
  //
  // Invariant: dfs(node) flattens the subtree rooted at `node` in-place and
  // returns a pointer to the tail (last node) of that flattened list.
  // Time: O(N), Space: O(H).
  void flattenRecursive(TreeNode* root) {
    if (root == nullptr) {
      return;
    }

    auto dfs = [](this auto&& self, TreeNode* node) -> TreeNode* {
      // Save the original right subtree before rewiring `node->right`.
      TreeNode* right_sub = node->right;
      TreeNode* left_tail = nullptr;

      if (node->left != nullptr) {
        left_tail = self(node->left);
        // Splice the flattened left list between `node` and `right_sub`.
        node->right = node->left;
        node->left = nullptr;
        left_tail->right = right_sub;
      }

      // If a right subtree exists, flattening it yields the overall tail.
      if (right_sub != nullptr) {
        return self(right_sub);
      }

      // Otherwise the tail is either the left subtree's tail or `node` itself.
      return left_tail != nullptr ? left_tail : node;
    };

    dfs(root);
  }
};

// Collects node values along the right-linked chain while verifying that every
// node's left pointer has been cleared to nullptr.
std::vector<int> extractFlattenedAndVerify(const TreeNode* root,
                                           lc::TestContext& ctx) {
  std::vector<int> vals;
  for (const TreeNode* curr = root; curr != nullptr; curr = curr->right) {
    ctx.expect_true(curr->left == nullptr, "flattened node must have null left");
    vals.push_back(curr->val);
  }
  return vals;
}

LC_PROBLEM(114, "Flatten Binary Tree To Linked List", "tree", ctx) {
  Solution sol;

  ctx.test("leetcode example 1 (both iterative and recursive)", [&] {
    //     1
    //    / \
    //   2   5
    //  / \   \
    // 3   4   6
    auto make_example = [](TreeArena& arena) -> TreeNode* {
      return arena.make(1, arena.make(2, arena.make(3), arena.make(4)),
                        arena.make(5, nullptr, arena.make(6)));
    };
    const std::vector<int> expected = {1, 2, 3, 4, 5, 6};

    TreeArena arena_iter;
    TreeNode* root_iter = make_example(arena_iter);
    sol.flatten(root_iter);
    ctx.expect_eq(extractFlattenedAndVerify(root_iter, ctx), expected);

    TreeArena arena_rec;
    TreeNode* root_rec = make_example(arena_rec);
    sol.flattenRecursive(root_rec);
    ctx.expect_eq(extractFlattenedAndVerify(root_rec, ctx), expected);
  });

  ctx.test("left-skewed chain from python build()", [&] {
    //     1
    //    /
    //   2
    //  /
    // 3
    auto make_chain = [](TreeArena& arena) -> TreeNode* {
      return arena.make(1, arena.make(2, arena.make(3)));
    };
    const std::vector<int> expected = {1, 2, 3};

    TreeArena arena_iter;
    TreeNode* root_iter = make_chain(arena_iter);
    sol.flatten(root_iter);
    ctx.expect_eq(extractFlattenedAndVerify(root_iter, ctx), expected);

    TreeArena arena_rec;
    TreeNode* root_rec = make_chain(arena_rec);
    sol.flattenRecursive(root_rec);
    ctx.expect_eq(extractFlattenedAndVerify(root_rec, ctx), expected);
  });

  ctx.test("right-skewed tree stays unchanged", [&] {
    TreeArena arena;
    TreeNode* root =
        arena.make(1, nullptr, arena.make(2, nullptr, arena.make(3)));
    sol.flatten(root);
    ctx.expect_eq(extractFlattenedAndVerify(root, ctx),
                  std::vector<int>{1, 2, 3});
  });

  ctx.test("empty tree and single node", [&] {
    sol.flatten(nullptr);
    sol.flattenRecursive(nullptr);

    TreeArena arena;
    TreeNode* single_iter = arena.make(42);
    sol.flatten(single_iter);
    ctx.expect_eq(extractFlattenedAndVerify(single_iter, ctx),
                  std::vector<int>{42});

    TreeNode* single_rec = arena.make(42);
    sol.flattenRecursive(single_rec);
    ctx.expect_eq(extractFlattenedAndVerify(single_rec, ctx),
                  std::vector<int>{42});
  });
}

}  // namespace
