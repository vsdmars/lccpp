// LeetCode 1008: Construct Binary Search Tree From Preorder Traversal
// https://leetcode.com/problems/construct-binary-search-tree-from-preorder-traversal/
//
// Problem:
//   Given an array of distinct integers `preorder` representing the preorder
//   traversal (root, left, right) of a binary search tree, reconstruct and
//   return the root of that unique BST.
//
// Algorithm:
//   1. Bound-Guided Single-Pass DFS (primary approach):
//      Maintain a shared index `idx` advancing through `preorder` from left to
//      right, together with open value bounds `(lower, upper)` that every key
//      in the current subtree must strictly satisfy.
//        - If `idx == preorder.size()` or `preorder[idx]` falls outside
//          `(lower, upper)`, return `nullptr` without consuming `preorder[idx]`
//          (it belongs to an ancestor's right subtree).
//        - Otherwise consume `val = preorder[idx++]`, create the node, build
//          its left subtree within `(lower, val)`, and build its right subtree
//          within `(val, upper)`.
//      Using 64-bit bounds handles extreme 32-bit values (`INT_MIN`/`INT_MAX`)
//      without sentinel collisions.
//      Time Complexity: O(N) — each element is checked at most twice and
//      consumed once. Space Complexity: O(H) call stack (+ O(N) output nodes).
//   2. Monotonic Decreasing Stack (iterative O(N) alternative):
//      Maintain the chain of open ancestors on a stack. For each value `val`:
//        - If `val < stack.back()->val`, attach as left child.
//        - Otherwise pop until the stack is empty or `stack.back()->val > val`,
//          and attach as the right child of the last popped node.
//      Time Complexity: O(N). Space Complexity: O(H) stack (+ O(N) output).
//
// Data Structures:
//   - Binary Search Tree (TreeNode) backed by an RAII node arena
//     (`std::deque<TreeNode>`) to ensure leak-free ownership.
//   - Shared cursor index + recursive call stack (or monotonic stack for the
//     iterative variant).
//
// Real-World Applications:
//   - Compact binary serialization and deserialization of BSTs and decision
//     trees: because a BST's preorder alone uniquely determines its shape, no
//     null markers or separate inorder sequence need to be transmitted.
//   - Rebuilding range-partition metadata trees on node startup from compact
//     checkpoint logs.
//   - Reconstructing Cartesian-like / ordered expression structures in linear
//     time during parsing.
//
// Related LeetCode Problems:
//   - #449 Serialize and Deserialize BST [done] (directly applies preorder-only BST reconstruction for compact encoding)
//   - #105 Construct Binary Tree from Preorder and Inorder Traversal [done] (general binary tree reconstruction requiring both orders)
//   - #255 Verify Preorder Sequence in Binary Search Tree [done] (validates whether an array is a valid BST preorder using a monotonic stack)
//   - #108 Convert Sorted Array to Binary Search Tree [done] (constructs a height-balanced BST from sorted keys)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <deque>
#include <limits>
#include <optional>
#include <queue>
#include <vector>

namespace {

// Standard binary tree node for LeetCode tree problems.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int x) : val(x) {}
};

class Solution {
 public:
  // Reconstructs the unique BST from its preorder traversal using open value
  // bounds and a single left-to-right cursor.
  // Time: O(N), Space: O(H) call stack + O(N) arena-owned nodes.
  TreeNode* bstFromPreorder(const std::vector<int>& preorder) {
    std::size_t idx = 0uz;

    // Recursively consumes nodes from `preorder[idx..]` that lie strictly
    // inside `(lower, upper)`.
    auto build = [this, &preorder, &idx](this auto&& self, std::int64_t lower,
                                         std::int64_t upper) -> TreeNode* {
      if (idx >= preorder.size()) {
        return nullptr;
      }

      const auto candidate = static_cast<std::int64_t>(preorder[idx]);
      // Value does not belong in this subtree; leave `idx` untouched for an
      // ancestor's right branch to consume.
      if (candidate <= lower || candidate >= upper) {
        return nullptr;
      }

      TreeNode* node = allocate(preorder[idx]);
      ++idx;

      // Preorder visits root, then all left descendants (< candidate), then
      // all right descendants (> candidate).
      node->left = self(lower, candidate);
      node->right = self(candidate, upper);
      return node;
    };

    return build(std::numeric_limits<std::int64_t>::min(),
                 std::numeric_limits<std::int64_t>::max());
  }

  // Iterative reconstruction using a monotonic decreasing ancestor stack.
  // Time: O(N), Space: O(H) stack + O(N) arena-owned nodes.
  TreeNode* bstFromPreorderIterative(const std::vector<int>& preorder) {
    if (preorder.empty()) {
      return nullptr;
    }

    TreeNode* root = allocate(preorder[0]);
    // Invariant: stack holds ancestors along the current rightmost frontier in
    // strictly decreasing order of value from bottom to top.
    std::vector<TreeNode*> stack{root};

    for (std::size_t i = 1uz; i < preorder.size(); ++i) {
      TreeNode* node = allocate(preorder[i]);
      TreeNode* parent = stack.back();

      // Pop ancestors smaller than `node->val`; the last popped ancestor is the
      // exact parent whose right subtree starts at `node`.
      while (!stack.empty() && stack.back()->val < node->val) {
        parent = stack.back();
        stack.pop_back();
      }

      if (node->val < parent->val) {
        parent->left = node;
      } else {
        parent->right = node;
      }
      stack.push_back(node);
    }

    return root;
  }

 private:
  // Allocates a TreeNode inside the RAII arena (std::deque never invalidates
  // element addresses on emplace_back).
  TreeNode* allocate(int val) {
    arena_.emplace_back(val);
    return &arena_.back();
  }

  std::deque<TreeNode> arena_;
};

// Serializes a binary tree into LeetCode's compact level-order format for exact
// structural assertions.
std::vector<std::optional<int>> to_level_order(const TreeNode* root) {
  std::vector<std::optional<int>> result;
  if (root == nullptr) {
    return result;
  }

  std::queue<const TreeNode*> pending;
  pending.push(root);

  while (!pending.empty()) {
    const TreeNode* node = pending.front();
    pending.pop();

    if (node == nullptr) {
      result.emplace_back(std::nullopt);
      continue;
    }

    result.emplace_back(node->val);
    pending.push(node->left);
    pending.push(node->right);
  }

  while (!result.empty() && !result.back().has_value()) {
    result.pop_back();
  }
  return result;
}

// Collects the in-order traversal to verify strict BST ordering.
std::vector<int> to_inorder(const TreeNode* root) {
  std::vector<int> values;
  auto dfs = [&values](this auto&& self, const TreeNode* node) -> void {
    if (node == nullptr) {
      return;
    }
    self(node->left);
    values.push_back(node->val);
    self(node->right);
  };
  dfs(root);
  return values;
}

LC_PROBLEM(1008, "Construct Binary Search Tree From Preorder Traversal",
           "binary_search_tree", ctx) {
  auto verify = [&](const std::vector<int>& preorder,
                    const std::vector<std::optional<int>>& expected_level,
                    const std::vector<int>& expected_inorder) {
    Solution sol;
    TreeNode* rec_root = sol.bstFromPreorder(preorder);
    TreeNode* iter_root = sol.bstFromPreorderIterative(preorder);

    ctx.expect_eq(to_level_order(rec_root), expected_level);
    ctx.expect_eq(to_level_order(iter_root), expected_level);
    ctx.expect_eq(to_inorder(rec_root), expected_inorder);
    ctx.expect_eq(to_inorder(iter_root), expected_inorder);
  };

  ctx.test("example_1_balanced_bst", [&] {
    verify({8, 5, 1, 7, 10, 12}, {8, 5, 10, 1, 7, std::nullopt, 12},
           {1, 5, 7, 8, 10, 12});
  });

  ctx.test("python_build_two_nodes", [&] {
    verify({4, 2}, {4, 2}, {2, 4});
  });

  ctx.test("single_node_and_empty", [&] {
    verify({7}, {7}, {7});
    verify({}, {}, {});
  });

  ctx.test("strictly_increasing_right_skewed", [&] {
    verify({1, 2, 3, 4},
           {1, std::nullopt, 2, std::nullopt, 3, std::nullopt, 4},
           {1, 2, 3, 4});
  });

  ctx.test("strictly_decreasing_left_skewed", [&] {
    verify({4, 3, 2, 1}, {4, 3, std::nullopt, 2, std::nullopt, 1},
           {1, 2, 3, 4});
  });

  ctx.test("extreme_int_bounds", [&] {
    const int kMinInt = std::numeric_limits<int>::min();
    const int kMaxInt = std::numeric_limits<int>::max();
    verify({0, kMinInt, kMaxInt}, {0, kMinInt, kMaxInt}, {kMinInt, 0, kMaxInt});
  });
}

}  // namespace
