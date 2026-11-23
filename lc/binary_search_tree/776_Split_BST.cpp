// LeetCode 776: Split BST
// https://leetcode.com/problems/split-bst/
//
// Problem:
//   Given the root of a binary search tree (BST) and an integer target, split
//   the tree into two valid BSTs:
//     - result[0] contains all nodes with value <= target.
//     - result[1] contains all nodes with value > target.
//   The target does not need to exist in the tree. Most of the original tree
//   structure must be preserved: if parent P and child C both end up in the
//   same resulting subtree, C must remain a child of P.
//
// Algorithm:
//   Single-path recursive BST split:
//   At each node along the search path for `target`:
//     - If root == nullptr, return {nullptr, nullptr}.
//     - If root->val <= target: root and its entire left subtree belong to the
//       <= target tree, while root->right may straddle the boundary. Recursively
//       split root->right into {small, large}, reattach root->right = small so
//       all <= nodes stay under root, and return {root, large}.
//     - If root->val > target: root and its entire right subtree belong to the
//       > target tree, while root->left may straddle the boundary. Recursively
//       split root->left into {small, large}, reattach root->left = large so
//       all > nodes stay under root, and return {small, root}.
//   Time Complexity: O(H), where H is the height of the BST (O(log N) balanced,
//   O(N) skewed), because recursion descends only one branch per level.
//   Space Complexity: O(H) for the recursion stack.
//
// Data Structures:
//   - Binary Search Tree (TreeNode): partitioned in-place by rewiring at most
//     H child pointers along the root-to-leaf search path.
//   - Two-element result vector (std::vector<TreeNode*>): returns {<=, >} roots.
//
// Real-World Applications:
//   - Persistent / functional BSTs (treaps, ropes, AVL trees) implementing
//     split-and-join primitives for text editors, concurrent snapshots, and
//     parallel divide-and-conquer merges.
//   - Database sharding and B-tree page splitting when partitioning an ordered
//     key range across two storage nodes at a pivot key.
//   - Interval and event-queue partitioning around a timestamp watermark.
//
// Related LeetCode Problems:
//   - #669 Trim A Binary Search Tree [done] (discards subtrees outside [low, high] using BST pruning)
//   - #450 Delete Node in a BST [done] (rewires BST pointers around a target key)
//   - #701 Insert into a Binary Search Tree [new] (descends a single O(H) BST search path)
//   - #938 Range Sum Of BST [done] (prunes BST traversal using key thresholds)

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
  // Splits `root` in-place into `{small_root, large_root}` where `small_root`
  // holds all nodes with `val <= target` and `large_root` holds all nodes with
  // `val > target`, preserving original parent-child links within each part.
  // Time: O(H), Space: O(H) where H is the height of the BST.
  std::vector<TreeNode*> splitBST(TreeNode* root, int target) {
    // Base case: splitting an empty tree yields two empty subtrees.
    if (root == nullptr) {
      return {nullptr, nullptr};
    }

    // When root->val <= target, root and its entire left subtree belong to the
    // <= partition. Only root->right can contain nodes > target, so split it
    // and attach its <= portion back as root->right.
    if (root->val <= target) {
      std::vector<TreeNode*> right_split = splitBST(root->right, target);
      root->right = right_split[0];
      return {root, right_split[1]};
    }

    // When root->val > target, root and its entire right subtree belong to the
    // > partition. Only root->left can contain nodes <= target, so split it
    // and attach its > portion back as root->left.
    std::vector<TreeNode*> left_split = splitBST(root->left, target);
    root->left = left_split[1];
    return {left_split[0], root};
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

// Serializes a binary tree into LeetCode's compact level-order representation.
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

LC_PROBLEM(776, "Split BST", "binary_search_tree", ctx) {
  Solution sol;

  ctx.test("example_1_split_at_two", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({4, 2, 6, 1, 3, 5, 7});
    const std::vector<TreeNode*> parts = sol.splitBST(root, 2);

    ctx.expect_eq(parts.size(), 2uz);
    const std::vector<std::optional<int>> expected_small{2, 1};
    const std::vector<std::optional<int>> expected_large{
        4, 3, 6, std::nullopt, std::nullopt, 5, 7};
    ctx.expect_eq(to_level_order(parts[0]), expected_small);
    ctx.expect_eq(to_level_order(parts[1]), expected_large);
  });

  ctx.test("split_at_absent_mid_value", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({10, 5, 15, 2, 8, 12, 20});
    const std::vector<TreeNode*> parts = sol.splitBST(root, 7);

    const std::vector<std::optional<int>> expected_small{5, 2};
    const std::vector<std::optional<int>> expected_large{
        10, 8, 15, std::nullopt, std::nullopt, 12, 20};
    ctx.expect_eq(to_level_order(parts[0]), expected_small);
    ctx.expect_eq(to_level_order(parts[1]), expected_large);
  });

  ctx.test("split_at_root_value", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({4, 2, 6, 1, 3, 5, 7});
    const std::vector<TreeNode*> parts = sol.splitBST(root, 4);

    const std::vector<std::optional<int>> expected_small{
        4, 2, std::nullopt, 1, 3};
    const std::vector<std::optional<int>> expected_large{6, 5, 7};
    ctx.expect_eq(to_level_order(parts[0]), expected_small);
    ctx.expect_eq(to_level_order(parts[1]), expected_large);
  });

  ctx.test("target_outside_tree_range", [&] {
    TreeArena arena;
    const std::vector<std::optional<int>> spec{4, 2, 6, 1, 3, 5, 7};

    TreeNode* root_low = arena.build(spec);
    const std::vector<TreeNode*> low_parts = sol.splitBST(root_low, 0);
    ctx.expect_true(low_parts[0] == nullptr);
    ctx.expect_eq(to_level_order(low_parts[1]), spec);

    TreeNode* root_high = arena.build(spec);
    const std::vector<TreeNode*> high_parts = sol.splitBST(root_high, 10);
    ctx.expect_eq(to_level_order(high_parts[0]), spec);
    ctx.expect_true(high_parts[1] == nullptr);
  });

  ctx.test("single_node_and_empty_tree", [&] {
    TreeArena arena;
    TreeNode* single_a = arena.build({5});
    const std::vector<TreeNode*> eq_parts = sol.splitBST(single_a, 5);
    ctx.expect_eq(to_level_order(eq_parts[0]),
                  std::vector<std::optional<int>>{5});
    ctx.expect_true(eq_parts[1] == nullptr);

    TreeNode* single_b = arena.build({5});
    const std::vector<TreeNode*> lt_parts = sol.splitBST(single_b, 4);
    ctx.expect_true(lt_parts[0] == nullptr);
    ctx.expect_eq(to_level_order(lt_parts[1]),
                  std::vector<std::optional<int>>{5});

    const std::vector<TreeNode*> empty_parts = sol.splitBST(nullptr, 3);
    ctx.expect_true(empty_parts[0] == nullptr);
    ctx.expect_true(empty_parts[1] == nullptr);
  });
}

}  // namespace
