// LeetCode 450: Delete Node In A BST
// https://leetcode.com/problems/delete-node-in-a-bst/
//
// Problem:
//   Given the root of a Binary Search Tree (BST) with unique keys and a target
//   `key`, remove the node whose value equals `key` (if present) while
//   preserving the BST invariant, and return the possibly updated root
//   reference. The required time complexity is O(h), where h is the tree
//   height.
//
// Algorithm:
//   Use recursive BST search-and-splice in two phases:
//   1. Search: Compare `key` with `node->val`. Recurse right when
//      `node->val < key`, or recurse left when `node->val > key`, assigning
//      the returned subtree root back to the corresponding child pointer.
//   2. Delete (when `node->val == key`):
//      - If `node->left == nullptr`, bypass `node` by returning `node->right`.
//      - If `node->right == nullptr`, bypass `node` by returning `node->left`.
//      - If both subtrees exist, find the rightmost node in `node->left`
//        (the in-order predecessor of `node`, which has no right child and is
//        strictly smaller than every node in `node->right`), attach
//        `node->right` as its right child, and return `node->left` as the new
//        subtree root.
//   Because every key in `node->left` is strictly less than `key` and every
//   key in `node->right` is strictly greater than `key`, attaching `right` to
//   the maximum node of `left` preserves the global BST ordering without
//   mutating any node values.
//   Time Complexity: O(h), traversing at most one root-to-leaf path.
//   Space Complexity: O(h) for the recursion stack.
//
// Data Structures:
//   - Binary Search Tree (`TreeNode` linked structure).
//   - Call stack for recursive parent-link updates.
//
// Real-World Applications:
//   - Ordered associative containers (`std::map`, `std::set`) and self-
//     balancing search trees (Red-Black, AVL, Treap) implementing key erasure.
//   - Database index maintenance (B-tree / BST record deletion).
//   - Event-loop timer wheels and priority schedulers cancelling scheduled
//     events by key.
//
// Related LeetCode Problems:
//   - #701 Insert into a Binary Search Tree [new] (companion BST mutation in O(h))
//   - #700 Search in a Binary Search Tree [new] (search phase of BST operations)
//   - #285 Inorder Successor in BST [done] (finding successor/predecessor for deletion)
//   - #669 Trim a Binary Search Tree [done] (pruning nodes outside a key range)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <limits>
#include <optional>
#include <queue>
#include <vector>

namespace {

// Standard LeetCode binary tree node definition.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int v, TreeNode* l = nullptr, TreeNode* r = nullptr)
      : val(v), left(l), right(r) {}
};

// RAII arena that owns all test TreeNodes; `deleteNode` only unlinks nodes so
// all storage is reclaimed automatically when the arena goes out of scope.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    nodes_.emplace_back(val, left, right);
    return &nodes_.back();
  }

  // Constructs a binary tree from LeetCode's level-order array representation.
  TreeNode* build(const std::vector<std::optional<int>>& level_order) {
    if (level_order.empty() || !level_order[0].has_value()) {
      return nullptr;
    }
    TreeNode* root = make(*level_order[0]);
    std::queue<TreeNode*> q;
    q.push(root);
    std::size_t idx = 1uz;
    while (!q.empty() && idx < level_order.size()) {
      TreeNode* cur = q.front();
      q.pop();
      if (level_order[idx].has_value()) {
        cur->left = make(*level_order[idx]);
        q.push(cur->left);
      }
      ++idx;
      if (idx < level_order.size() && level_order[idx].has_value()) {
        cur->right = make(*level_order[idx]);
        q.push(cur->right);
      }
      ++idx;
    }
    return root;
  }

 private:
  std::deque<TreeNode> nodes_;
};

// Collects the in-order traversal values of a binary tree for test assertions.
std::vector<int> inorder_values(const TreeNode* root) {
  std::vector<int> out;
  auto dfs = [&](this auto&& self, const TreeNode* node) -> void {
    if (node == nullptr) {
      return;
    }
    self(node->left);
    out.push_back(node->val);
    self(node->right);
  };
  dfs(root);
  return out;
}

// Verifies that `root` satisfies the strict BST invariant at every node.
bool is_valid_bst(const TreeNode* root) {
  auto dfs = [](this auto&& self, const TreeNode* node,
                std::optional<int> low, std::optional<int> high) -> bool {
    if (node == nullptr) {
      return true;
    }
    if (low.has_value() && node->val <= *low) {
      return false;
    }
    if (high.has_value() && node->val >= *high) {
      return false;
    }
    return self(node->left, low, node->val) &&
           self(node->right, node->val, high);
  };
  return dfs(root, std::nullopt, std::nullopt);
}

class Solution {
 public:
  // Deletes the node with value `key` from the BST rooted at `root` and
  // returns the (possibly updated) root pointer. Does not free the unlinked
  // node so callers using arena or external ownership manage lifetime safely.
  // Time: O(h), Space: O(h).
  TreeNode* deleteNode(TreeNode* root, int key) {
    if (root == nullptr) {
      return nullptr;
    }

    if (root->val < key) {
      // Key can only live in the right subtree.
      root->right = deleteNode(root->right, key);
      return root;
    }
    if (root->val > key) {
      // Key can only live in the left subtree.
      root->left = deleteNode(root->left, key);
      return root;
    }

    // Found the target node (`root->val == key`).
    if (root->left == nullptr) {
      return root->right;
    }
    if (root->right == nullptr) {
      return root->left;
    }

    // Both children exist: attach the entire right subtree beneath the
    // rightmost node (in-order predecessor) of the left subtree, then promote
    // `root->left` into `root`'s position.
    TreeNode* predecessor = root->left;
    while (predecessor->right != nullptr) {
      predecessor = predecessor->right;
    }
    predecessor->right = root->right;
    return root->left;
  }
};

LC_PROBLEM(450, "Delete Node In A BST", "binary_search_tree", ctx) {
  Solution sol;

  ctx.test("leetcode_example_delete_internal_node", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({5, 3, 6, 2, 4, std::nullopt, 7});
    TreeNode* updated = sol.deleteNode(root, 3);
    ctx.expect_true(is_valid_bst(updated));
    ctx.expect_eq(inorder_values(updated), std::vector<int>{2, 4, 5, 6, 7});
  });

  ctx.test("delete_root_with_two_children", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({5, 3, 6, 2, 4, std::nullopt, 7});
    TreeNode* updated = sol.deleteNode(root, 5);
    ctx.expect_true(is_valid_bst(updated));
    ctx.expect_eq(inorder_values(updated), std::vector<int>{2, 3, 4, 6, 7});
  });

  ctx.test("delete_leaf_and_one_child_node", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({5, 3, 6, 2, 4, std::nullopt, 7});
    root = sol.deleteNode(root, 2);  // leaf
    ctx.expect_true(is_valid_bst(root));
    ctx.expect_eq(inorder_values(root), std::vector<int>{3, 4, 5, 6, 7});

    root = sol.deleteNode(root, 6);  // single right child
    ctx.expect_true(is_valid_bst(root));
    ctx.expect_eq(inorder_values(root), std::vector<int>{3, 4, 5, 7});
  });

  ctx.test("delete_missing_key_leaves_tree_unchanged", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({5, 3, 6, 2, 4, std::nullopt, 7});
    TreeNode* updated = sol.deleteNode(root, 99);
    ctx.expect_true(is_valid_bst(updated));
    ctx.expect_eq(inorder_values(updated), std::vector<int>{2, 3, 4, 5, 6, 7});
  });

  ctx.test("empty_and_single_node_tree", [&] {
    TreeArena arena;
    ctx.expect_eq(sol.deleteNode(nullptr, 10), nullptr);

    TreeNode* single = arena.make(10);
    ctx.expect_eq(sol.deleteNode(single, 10), nullptr);
  });

  ctx.test("extreme_int_values", [&] {
    TreeArena arena;
    constexpr int kMin = std::numeric_limits<int>::min();
    constexpr int kMax = std::numeric_limits<int>::max();
    TreeNode* root = arena.build({0, kMin, kMax});
    root = sol.deleteNode(root, 0);
    ctx.expect_true(is_valid_bst(root));
    ctx.expect_eq(inorder_values(root), std::vector<int>{kMin, kMax});
  });
}

}  // namespace
