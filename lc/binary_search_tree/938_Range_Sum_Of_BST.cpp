// LeetCode 938: Range Sum Of BST
// https://leetcode.com/problems/range-sum-of-bst/
//
// Problem:
//   Given the root of a binary search tree (BST) with unique integer keys and
//   two integers `low` and `high` (low <= high), return the sum of values of
//   all nodes whose values fall within the inclusive interval [low, high].
//
// Algorithm:
//   Pruned BST Depth-First Search (Recursive and Iterative):
//   At each visited node, use the BST ordering invariant to skip subtrees that
//   cannot possibly intersect [low, high]:
//     - If node == nullptr, contribute 0.
//     - If node->val > high, every key in node->right is strictly greater than
//       high, so only explore node->left.
//     - If node->val < low, every key in node->left is strictly smaller than
//       low, so only explore node->right.
//     - Otherwise low <= node->val <= high: add node->val and explore both
//       subtrees.
//   An iterative stack variant performs the identical pruned traversal without
//   risk of stack overflow on deeply skewed trees.
//   Time Complexity: O(N) worst-case (when [low, high] covers the whole tree),
//   and O(K + H) in practice where K is the number of nodes in [low, high] and
//   H is tree height.
//   Space Complexity: O(H) for recursion or explicit stack.
//
// Data Structures:
//   - Binary Search Tree (TreeNode): provides key-range pruning at every node.
//   - Call Stack / Explicit Stack (std::vector<const TreeNode*>): tracks
//     remaining unpruned subtrees to inspect.
//
// Real-World Applications:
//   - Database range-aggregate queries (`SELECT SUM(val) WHERE key BETWEEN
//     low AND high`) over B-tree or in-memory ordered tree indexes.
//   - Financial order-book depth aggregation computing total liquidity between
//     two price levels.
//   - Telemetry and metrics rollups summing events whose timestamps fall inside
//     an inspection window.
//
// Related LeetCode Problems:
//   - #669 Trim A Binary Search Tree [done] (prunes the same out-of-range subtrees to return a trimmed BST)
//   - #270 Closest Binary Search Tree Value [done] (descends a BST using key comparisons to locate nearest value)
//   - #538 Convert BST to Greater Tree [done] (accumulates suffix sums across a BST)
//   - #1038 Binary Search Tree To Greater Sum Tree [done] (identical greater-sum accumulation on a BST)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
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
  // Computes the sum of all node values in [low, high] using pruned recursive
  // DFS.
  // Time: O(N) worst-case / O(K + H) pruned, Space: O(H).
  int rangeSumBST(const TreeNode* root, int low, int high) {
    if (root == nullptr) {
      return 0;
    }

    // Current value exceeds upper bound: entire right subtree is also > high
    // and can be skipped completely.
    if (root->val > high) {
      return rangeSumBST(root->left, low, high);
    }

    // Current value is below lower bound: entire left subtree is also < low
    // and can be skipped completely.
    if (root->val < low) {
      return rangeSumBST(root->right, low, high);
    }

    // Node lies inside [low, high]: include its value and inspect both sides.
    return root->val + rangeSumBST(root->left, low, high) +
           rangeSumBST(root->right, low, high);
  }

  // Iterative pruned DFS variant avoiding deep recursion on skewed BSTs.
  // Time: O(N) worst-case / O(K + H) pruned, Space: O(H).
  int rangeSumBSTIterative(const TreeNode* root, int low, int high) {
    if (root == nullptr) {
      return 0;
    }

    int total = 0;
    std::vector<const TreeNode*> stack{root};

    while (!stack.empty()) {
      const TreeNode* node = stack.back();
      stack.pop_back();

      if (node->val >= low && node->val <= high) {
        total += node->val;
      }

      // Only descend left if keys smaller than node->val can still be >= low.
      if (node->val > low && node->left != nullptr) {
        stack.push_back(node->left);
      }

      // Only descend right if keys larger than node->val can still be <= high.
      if (node->val < high && node->right != nullptr) {
        stack.push_back(node->right);
      }
    }

    return total;
  }
};

// RAII arena owning test TreeNode instances to guarantee zero memory leaks.
class TreeArena {
 public:
  // Builds a binary tree from LeetCode's level-order vector (std::nullopt =
  // missing child).
  TreeNode* build(const std::vector<std::optional<int>>& level_order) {
    if (level_order.empty() || !level_order[0].has_value()) {
      return nullptr;
    }

    TreeNode* root = create(*level_order[0]);
    std::queue<TreeNode*> pending;
    pending.push(root);

    std::size_t idx = 1uz;
    while (!pending.empty() && idx < level_order.size()) {
      TreeNode* parent = pending.front();
      pending.pop();

      if (level_order[idx].has_value()) {
        parent->left = create(*level_order[idx]);
        pending.push(parent->left);
      }
      ++idx;

      if (idx < level_order.size() && level_order[idx].has_value()) {
        parent->right = create(*level_order[idx]);
        pending.push(parent->right);
      }
      ++idx;
    }
    return root;
  }

 private:
  TreeNode* create(int val) {
    nodes_.emplace_back(val);
    return &nodes_.back();
  }

  std::deque<TreeNode> nodes_;
};

LC_PROBLEM(938, "Range Sum Of BST", "binary_search_tree", ctx) {
  Solution sol;

  auto expect_both = [&](const TreeNode* root, int low, int high,
                         int expected) {
    ctx.expect_eq(sol.rangeSumBST(root, low, high), expected);
    ctx.expect_eq(sol.rangeSumBSTIterative(root, low, high), expected);
  };

  ctx.test("example_1_from_python_build", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({10, 5, 15, 3, 7, std::nullopt, 18});
    // Nodes in [7, 15]: 7 + 10 + 15 = 32.
    expect_both(root, 7, 15, 32);
  });

  ctx.test("example_2_deeper_tree", [&] {
    TreeArena arena;
    TreeNode* root =
        arena.build({10, 5, 15, 3, 7, 13, 18, 1, std::nullopt, 6});
    // Nodes in [6, 10]: 6 + 7 + 10 = 23.
    expect_both(root, 6, 10, 23);
  });

  ctx.test("point_range_and_out_of_bounds", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({10, 5, 15, 3, 7, 13, 18});
    expect_both(root, 13, 13, 13);
    expect_both(root, 14, 14, 0);
    expect_both(root, -10, 2, 0);
    expect_both(root, 19, 100, 0);
  });

  ctx.test("full_tree_range_and_empty_tree", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({10, 5, 15, 3, 7, 13, 18});
    expect_both(root, 0, 100, 71);
    expect_both(nullptr, 0, 100, 0);
  });
}

}  // namespace
