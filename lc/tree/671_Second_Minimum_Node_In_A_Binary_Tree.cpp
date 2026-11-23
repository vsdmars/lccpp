// LeetCode 671: Second Minimum Node In A Binary Tree
// https://leetcode.com/problems/second-minimum-node-in-a-binary-tree/
//
// Problem:
//   Given a non-empty special binary tree where every node has either zero or
//   two children, and every internal node satisfies
//   `node.val == min(node.left.val, node.right.val)`, return the second
//   smallest distinct value among all nodes in the tree. If no second smallest
//   value exists (all nodes share the same value), return -1.
//
// Examples:
//   Example 1:
//     Input:  root = [2, 2, 5, null, null, 5, 7]
//     Output: 5
//     Explanation:
//       - Initialize global_min = root->val = 2 and call dfs(root).
//       - The left child has val == 2 == global_min, so dfs recurses into its
//         nullptr children and returns std::nullopt.
//       - The right child has val == 5 > global_min, so dfs prunes its subtree (5,
//         7) and immediately returns 5.
//       - Combining std::nullopt and 5 at the root yields 5.
//
//   Example 2:
//     Input:  root = [2, 2, 5, 2, 3, 5, 7]
//     Output: 3
//     Explanation:
//       - Set global_min = 2; because the left child also has val == 2, dfs
//         descends into its children 2 and 3.
//       - Leaf 2 matches global_min and returns std::nullopt, while leaf 3 > 2
//         returns 3, producing 3 for the left subtree.
//       - On the right child 5 > 2, dfs prunes its children (5, 7) and returns 5.
//       - At the root, std::min(3, 5) selects 3 as the second minimum value.
//
//   Example 3:
//     Input:  root = [2, 2, 2]
//     Output: -1
//     Explanation:
//       - Set global_min = 2; both children have val == 2 == global_min, so dfs
//         recurses to their nullptr children.
//       - No node satisfies node->val > global_min, so dfs(root) returns
//         std::nullopt and .value_or(-1) yields -1.
//
// Algorithm:
//   Because every parent is the minimum of its children, the root value
//   `min_val = root->val` is guaranteed to be the global minimum of the entire
//   tree, and every subtree rooted at a node `u` contains only values >= u->val.
//
//   Pruned DFS (`findSecondMinimumValue`):
//     - If `node == nullptr`, return `std::nullopt`.
//     - If `node->val > min_val`, `node->val` is the smallest value anywhere in
//       this subtree, so we immediately prune and return `node->val` without
//       visiting its descendants.
//     - Otherwise (`node->val == min_val`), recursively search both children
//       and return the smaller of the non-empty candidate results.
//   Using `std::optional<int>` cleanly avoids sentinel collisions when node
//   values equal `INT_MAX`. A full-scan DFS variant (`findSecondMinimumValueScan`)
//   is also included for comparison.
//   Time Complexity:  O(n) in the worst case (when many nodes equal root->val),
//                     with early pruning on subtrees strictly above root->val.
//   Space Complexity: O(h) recursion stack where h is the tree height.
//
// Data Structures:
//   - Binary tree nodes (`TreeNode`) satisfying the tournament-tree min heap
//     property.
//   - `std::optional<int>` representing the best candidate strictly greater
//     than the global minimum.
//
// Real-World Applications:
//   - Tournament-tree selection (winner/loser trees) used in external k-way
//     merge sort to find the runner-up key in O(log n) comparisons.
//   - Min-heap / priority-queue diagnostics and auction second-price (Vickrey)
//     winner determination over hierarchical tournament brackets.
//
// Related LeetCode Problems:
//   - #230 Kth Smallest Element in a BST [done] (order-statistic query in a binary tree)
//   - #414 Third Maximum Number [done] (distinct rank-order tracking without sorting)
//   - #104 Maximum Depth of Binary Tree [done] (foundational recursive DFS on trees)
//   - #783 Minimum Distance Between BST Nodes [done] (DFS order-property traversal)

#include "framework/lc_framework.h"

#include <algorithm>
#include <deque>
#include <limits>
#include <optional>

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

class Solution {
 public:
  // Pruned DFS exploiting the tournament-tree invariant: any node with
  // value > root->val is the minimum of its entire subtree, so we never need
  // to descend below it.
  // Time: O(n) worst case, Space: O(h).
  int findSecondMinimumValue(TreeNode* root) {
    if (root == nullptr) {
      return -1;
    }

    const int global_min = root->val;

    auto dfs = [global_min](this auto&& self,
                            const TreeNode* node) -> std::optional<int> {
      if (node == nullptr) {
        return std::nullopt;
      }
      // Every descendant has value >= node->val, so node->val is the tightest
      // candidate > global_min in this entire subtree.
      if (node->val > global_min) {
        return node->val;
      }

      const std::optional<int> left_cand = self(node->left);
      const std::optional<int> right_cand = self(node->right);

      if (left_cand.has_value() && right_cand.has_value()) {
        return std::min(*left_cand, *right_cand);
      }
      if (left_cand.has_value()) {
        return left_cand;
      }
      return right_cand;
    };

    return dfs(root).value_or(-1);
  }

  // Straightforward full-tree DFS variant tracking the minimum value strictly
  // larger than root->val.
  // Time: O(n), Space: O(h).
  int findSecondMinimumValueScan(TreeNode* root) {
    if (root == nullptr) {
      return -1;
    }

    const int global_min = root->val;
    std::optional<int> best;

    auto traverse = [global_min, &best](this auto&& self,
                                        const TreeNode* node) -> void {
      if (node == nullptr) {
        return;
      }
      if (node->val > global_min && (!best.has_value() || node->val < *best)) {
        best = node->val;
      }
      self(node->left);
      self(node->right);
    };

    traverse(root);
    return best.value_or(-1);
  }
};

LC_PROBLEM(671, "Second Minimum Node In A Binary Tree", "tree", ctx) {
  ctx.test("example1_standard_tree", [&] {
    //     2
    //    / \
    //   2   5
    //      / \
    //     5   7
    TreeArena arena;
    TreeNode* right = arena.make(5, arena.make(5), arena.make(7));
    TreeNode* root = arena.make(2, arena.make(2), right);
    Solution sol;
    ctx.expect_eq(sol.findSecondMinimumValue(root), 5);
    ctx.expect_eq(sol.findSecondMinimumValueScan(root), 5);
  });

  ctx.test("example2_simple_three_nodes", [&] {
    //     5
    //    / \
    //   5   6
    TreeArena arena;
    TreeNode* root = arena.make(5, arena.make(5), arena.make(6));
    Solution sol;
    ctx.expect_eq(sol.findSecondMinimumValue(root), 6);
    ctx.expect_eq(sol.findSecondMinimumValueScan(root), 6);
  });

  ctx.test("all_equal_returns_negative_one", [&] {
    //     2
    //    / \
    //   2   2
    TreeArena arena;
    TreeNode* root = arena.make(2, arena.make(2), arena.make(2));
    Solution sol;
    ctx.expect_eq(sol.findSecondMinimumValue(root), -1);
    ctx.expect_eq(sol.findSecondMinimumValueScan(root), -1);
  });

  ctx.test("int_max_as_valid_second_minimum", [&] {
    // Regression check: INT_MAX must not collide with an infinity sentinel.
    constexpr int kMaxInt = std::numeric_limits<int>::max();
    TreeArena arena;
    TreeNode* root = arena.make(2, arena.make(2), arena.make(kMaxInt));
    Solution sol;
    ctx.expect_eq(sol.findSecondMinimumValue(root), kMaxInt);
    ctx.expect_eq(sol.findSecondMinimumValueScan(root), kMaxInt);
  });

  ctx.test("deeper_second_minimum_under_tied_child", [&] {
    //       2
    //      / \
    //     2   5
    //    / \ / \
    //   2  3 5  7
    TreeArena arena;
    TreeNode* left = arena.make(2, arena.make(2), arena.make(3));
    TreeNode* right = arena.make(5, arena.make(5), arena.make(7));
    TreeNode* root = arena.make(2, left, right);
    Solution sol;
    ctx.expect_eq(sol.findSecondMinimumValue(root), 3);
    ctx.expect_eq(sol.findSecondMinimumValueScan(root), 3);
  });

  ctx.test("single_node_and_empty_tree", [&] {
    TreeArena arena;
    Solution sol;
    ctx.expect_eq(sol.findSecondMinimumValue(arena.make(4)), -1);
    ctx.expect_eq(sol.findSecondMinimumValue(nullptr), -1);
  });
}

}  // namespace
