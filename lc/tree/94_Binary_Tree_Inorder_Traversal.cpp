// LeetCode 94: Binary Tree Inorder Traversal
// https://leetcode.com/problems/binary-tree-inorder-traversal/
//
// Problem:
//   Given the root of a binary tree, return the inorder traversal of its
//   nodes' values (left subtree, then root, then right subtree). The tree may
//   be empty (0 to 100 nodes) with node values in [-100, 100].
//
// Algorithm:
//   1. Recursive DFS: Visit the left child recursively, record the current
//      node's value, and then visit the right child recursively. Base case is
//      a null pointer.
//   2. Iterative Explicit-Stack DFS: Maintain a pointer `curr` and an explicit
//      stack of ancestor nodes whose left subtrees are being explored. Walk
//      left as far as possible pushing each node onto the stack; when `curr`
//      becomes null, pop the top node, record its value, and move `curr` to
//      its right child.
//   - Time Complexity: O(N), where N is the number of nodes (every node is
//     visited and emitted exactly once).
//   - Space Complexity: O(H) auxiliary stack space where H is the tree height
//     (O(log N) for balanced trees, O(N) worst case for skewed trees), plus
//     O(N) for the returned result vector.
//
// Data Structures:
//   - Binary tree (`TreeNode` with `val`, `left`, `right` pointers).
//   - Implicit call stack (recursive DFS) or `std::stack<const TreeNode*>`
//     (iterative DFS) to remember pending ancestors while descending left.
//   - `std::vector<int>` to accumulate the visited values in inorder sequence.
//
// Real-World Applications:
//   - Producing sorted key output from Binary Search Trees (e.g., in-memory
//     ordered maps like `std::map` or database index scans).
//   - Emitting infix expressions from Abstract Syntax Trees (ASTs) in
//     compilers, calculators, and code formatters.
//   - Serializing or validating structural ordering invariants in tree-based
//     indexes and hierarchical filesystems.
//
// Related LeetCode Problems:
//   - #98 Validate Binary Search Tree [done] (inorder traversal of a BST must be strictly increasing)
//   - #144 Binary Tree Preorder Traversal [new] (sibling DFS order: root, left, right)
//   - #145 Binary Tree Postorder Traversal [done] (sibling DFS order: left, right, root)
//   - #173 Binary Search Tree Iterator [done] (pauses the iterative inorder stack between next() calls)
//   - #230 Kth Smallest Element in a BST [done] (stops inorder traversal at the k-th emitted node)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <optional>
#include <queue>
#include <stack>
#include <vector>

namespace {

// Standard LeetCode binary tree node representation.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int v) : val(v) {}
  TreeNode(int v, TreeNode* l, TreeNode* r) : val(v), left(l), right(r) {}
};

class Solution {
 public:
  // Returns the inorder (left -> root -> right) traversal using recursive DFS.
  // Time: O(N), Space: O(H) call stack + O(N) output.
  std::vector<int> inorderTraversal(const TreeNode* root) {
    std::vector<int> result;

    // Recursive helper capturing `result` by reference using C++23 deducing
    // `this` to avoid std::function overhead.
    auto dfs = [&](this auto&& self, const TreeNode* node) -> void {
      if (node == nullptr) {
        return;
      }
      self(node->left);
      result.push_back(node->val);
      self(node->right);
    };

    dfs(root);
    return result;
  }

  // Returns the inorder traversal iteratively with an explicit stack, avoiding
  // recursion-depth limits on deeply skewed trees.
  // Invariant: every node on `pending` has already been reached from above and
  // is waiting for its entire left subtree to finish before being emitted.
  // Time: O(N), Space: O(H) explicit stack + O(N) output.
  std::vector<int> inorderTraversalIterative(const TreeNode* root) {
    std::vector<int> result;
    std::stack<const TreeNode*> pending;
    const TreeNode* curr = root;

    while (curr != nullptr || !pending.empty()) {
      // Descend to the leftmost unvisited descendant, saving ancestors.
      while (curr != nullptr) {
        pending.push(curr);
        curr = curr->left;
      }

      // Left subtree is exhausted: emit the closest pending ancestor, then
      // shift traversal into its right subtree.
      curr = pending.top();
      pending.pop();
      result.push_back(curr->val);
      curr = curr->right;
    }

    return result;
  }
};

// RAII node arena backed by std::deque so element addresses stay stable across
// insertions and all allocated test nodes are freed automatically.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    nodes_.emplace_back(val, left, right);
    return &nodes_.back();
  }

  // Constructs a binary tree from LeetCode's level-order array encoding where
  // std::nullopt denotes an absent child.
  TreeNode* from_level_order(const std::vector<std::optional<int>>& values) {
    if (values.empty() || !values[0].has_value()) {
      return nullptr;
    }
    TreeNode* root = make(*values[0]);
    std::queue<TreeNode*> pending;
    pending.push(root);
    std::size_t idx = 1uz;

    while (!pending.empty() && idx < values.size()) {
      TreeNode* curr = pending.front();
      pending.pop();

      if (idx < values.size() && values[idx].has_value()) {
        curr->left = make(*values[idx]);
        pending.push(curr->left);
      }
      ++idx;

      if (idx < values.size() && values[idx].has_value()) {
        curr->right = make(*values[idx]);
        pending.push(curr->right);
      }
      ++idx;
    }
    return root;
  }

 private:
  std::deque<TreeNode> nodes_;
};

LC_PROBLEM(94, "Binary Tree Inorder Traversal", "tree", ctx) {
  Solution sol;

  ctx.test("example_1_right_then_left", [&] {
    TreeArena arena;
    // Tree: [1, null, 2, 3]
    TreeNode* root = arena.from_level_order({1, std::nullopt, 2, 3});
    const std::vector<int> expected = {1, 3, 2};
    ctx.expect_eq(sol.inorderTraversal(root), expected);
    ctx.expect_eq(sol.inorderTraversalIterative(root), expected);
  });

  ctx.test("python_build_tree", [&] {
    TreeArena arena;
    //       1
    //      / \
    //     2   2
    //      \ / \
    //      7 4  3
    TreeNode* left = arena.make(2, nullptr, arena.make(7));
    TreeNode* right = arena.make(2, arena.make(4), arena.make(3));
    TreeNode* root = arena.make(1, left, right);
    const std::vector<int> expected = {2, 7, 1, 4, 2, 3};
    ctx.expect_eq(sol.inorderTraversal(root), expected);
    ctx.expect_eq(sol.inorderTraversalIterative(root), expected);
  });

  ctx.test("empty_tree", [&] {
    const std::vector<int> expected = {};
    ctx.expect_eq(sol.inorderTraversal(nullptr), expected);
    ctx.expect_eq(sol.inorderTraversalIterative(nullptr), expected);
  });

  ctx.test("single_node", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(42);
    const std::vector<int> expected = {42};
    ctx.expect_eq(sol.inorderTraversal(root), expected);
    ctx.expect_eq(sol.inorderTraversalIterative(root), expected);
  });

  ctx.test("left_and_right_skewed_trees", [&] {
    TreeArena arena;
    // Left-skewed: 3 -> 2 -> 1
    TreeNode* left_skewed =
        arena.make(3, arena.make(2, arena.make(1), nullptr), nullptr);
    // Right-skewed: 1 -> 2 -> 3
    TreeNode* right_skewed =
        arena.make(1, nullptr, arena.make(2, nullptr, arena.make(3)));
    const std::vector<int> expected = {1, 2, 3};
    ctx.expect_eq(sol.inorderTraversal(left_skewed), expected);
    ctx.expect_eq(sol.inorderTraversalIterative(left_skewed), expected);
    ctx.expect_eq(sol.inorderTraversal(right_skewed), expected);
    ctx.expect_eq(sol.inorderTraversalIterative(right_skewed), expected);
  });
}

}  // namespace
