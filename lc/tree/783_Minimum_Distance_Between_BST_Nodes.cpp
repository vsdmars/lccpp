// LeetCode 783: Minimum Distance Between BST Nodes
// https://leetcode.com/problems/minimum-distance-between-bst-nodes/
//
// Problem:
//   Given the root of a Binary Search Tree (BST) containing at least two nodes,
//   return the minimum absolute difference between the values of any two
//   different nodes in the tree.
//
// Examples:
//   Example 1:
//     Input:  root = [4, 2, 6, 1, 3]
//     Output: 1
//     Explanation:
//       - In-order DFS visits the BST keys in sorted sequence 1 -> 2 -> 3 -> 4 ->
//         6.
//       - Visiting 1 sets prev_val = 1, and visiting 2 updates min_diff = 2 - 1 = 1
//         and prev_val = 2.
//       - Remaining adjacent differences are 3 - 2 = 1, 4 - 3 = 1, and 6 - 4 = 2,
//         so min_diff remains 1.
//
//   Example 2:
//     Input:  root = [90, 69, null, 49, 89, null, 52]
//     Output: 1
//     Explanation:
//       - In-order DFS visits nodes in sorted order 49 -> 52 -> 69 -> 89 -> 90
//         while tracking prev_val.
//       - Differences across 49 -> 52 -> 69 -> 89 are 52 - 49 = 3, 69 - 52 = 17,
//         and 89 - 69 = 20, keeping min_diff = 3.
//       - Visiting root 90 after its left-subtree maximum 89 computes 90 - 89 = 1,
//         updating min_diff to 1.
//
//   Example 3:
//     Input:  root = [5, 5, 10]
//     Output: 0
//     Explanation:
//       - In-order DFS visits the left child 5 first and sets prev_val = 5.
//       - Next, visiting root 5 computes 5 - 5 = 0, updating min_diff to 0 and
//         prev_val = 5.
//       - Visiting right child 10 yields 10 - 5 = 5, leaving min_diff = 0.
//
// Algorithm:
//   Approach 1 — In-Order Traversal (`minDiffInBST`):
//     An in-order traversal (left -> node -> right) visits BST keys in sorted
//     non-decreasing order. In any sorted sequence, the minimum difference
//     between any pair of elements must occur between two adjacent elements.
//     Maintaining only the previously visited value (`std::optional<int>`) lets
//     us compute all adjacent gaps in a single pass without materializing an
//     array of all values.
//
//   Approach 2 — Subtree Min/Max Bounds (`minDiffInBSTByBounds`):
//     Post-order DFS returns `(subtree_min, subtree_max)` for each subtree. For
//     any node, its closest predecessor in its left subtree is `left_max` and
//     its closest successor in its right subtree is `right_min`; comparing both
//     gaps captures all adjacent pairs in the BST.
//   Time Complexity:  O(n) for both approaches, visiting each node once.
//   Space Complexity: O(h) recursion stack where h is the height of the BST.
//
// Data Structures:
//   - Binary Search Tree (`TreeNode`).
//   - `std::optional<int>` holding the immediately preceding in-order value.
//
// Real-World Applications:
//   - Financial order books / time-series indices (B-trees, RB-trees): finding
//     the tightest price spread or smallest inter-event interval across sorted
//     keys.
//   - Collision / proximity detection in 1D spatial indices (interval/sweep
//     trees) where closest neighbors are adjacent in key order.
//   - Database range-index statistics: measuring minimum key separation for
//     histogram bucket sizing.
//
// Related LeetCode Problems:
//   - #530 Minimum Absolute Difference in BST [done] (identical problem statement and solution)
//   - #98 Validate Binary Search Tree [done] (in-order sortedness and subtree min/max bounds)
//   - #230 Kth Smallest Element in a BST [done] (in-order BST traversal)
//   - #270 Closest Binary Search Tree Value [done] (searching nearest key in a BST)
//   - #94 Binary Tree Inorder Traversal [done] (foundational in-order traversal)

#include "framework/lc_framework.h"

#include <algorithm>
#include <deque>
#include <limits>
#include <optional>

namespace {

// Standard binary search tree node.
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
  // In-order DFS: visits BST values in sorted order and tracks the minimum gap
  // between consecutive nodes.
  // Time: O(n), Space: O(h).
  int minDiffInBST(TreeNode* root) {
    std::optional<int> prev_val;
    int min_diff = std::numeric_limits<int>::max();

    auto inorder = [&prev_val, &min_diff](this auto&& self,
                                          const TreeNode* node) -> void {
      if (node == nullptr) {
        return;
      }

      self(node->left);

      if (prev_val.has_value()) {
        min_diff = std::min(min_diff, node->val - *prev_val);
      }
      prev_val = node->val;

      self(node->right);
    };

    inorder(root);
    return min_diff;
  }

  // Post-order DFS returning each subtree's (min_val, max_val) and checking
  // each node against its in-order predecessor (left max) and successor
  // (right min).
  // Time: O(n), Space: O(h).
  int minDiffInBSTByBounds(TreeNode* root) {
    struct Bounds {
      int min_val = 0;
      int max_val = 0;
    };

    int min_diff = std::numeric_limits<int>::max();

    auto dfs = [&min_diff](this auto&& self, const TreeNode* node) -> Bounds {
      int low = node->val;
      int high = node->val;

      if (node->left != nullptr) {
        const Bounds left = self(node->left);
        min_diff = std::min(min_diff, node->val - left.max_val);
        low = left.min_val;
      }

      if (node->right != nullptr) {
        const Bounds right = self(node->right);
        min_diff = std::min(min_diff, right.min_val - node->val);
        high = right.max_val;
      }

      return {.min_val = low, .max_val = high};
    };

    if (root != nullptr) {
      dfs(root);
    }
    return min_diff;
  }
};

LC_PROBLEM(783, "Minimum Distance Between BST Nodes", "tree", ctx) {
  ctx.test("example1_balanced_bst", [&] {
    //       4
    //      / \
    //     2   6
    //    / \
    //   1   3
    TreeArena arena;
    TreeNode* left = arena.make(2, arena.make(1), arena.make(3));
    TreeNode* root = arena.make(4, left, arena.make(6));
    Solution sol;
    ctx.expect_eq(sol.minDiffInBST(root), 1);
    ctx.expect_eq(sol.minDiffInBSTByBounds(root), 1);
  });

  ctx.test("example2_right_subtree_minimum", [&] {
    //       1
    //      / \
    //     0   48
    //        /  \
    //      12    49
    TreeArena arena;
    TreeNode* right = arena.make(48, arena.make(12), arena.make(49));
    TreeNode* root = arena.make(1, arena.make(0), right);
    Solution sol;
    ctx.expect_eq(sol.minDiffInBST(root), 1);
    ctx.expect_eq(sol.minDiffInBSTByBounds(root), 1);
  });

  ctx.test("closest_pair_is_root_and_deep_descendant", [&] {
    //         90
    //        /
    //      69
    //     /  \
    //   49    89
    //     \
    //      52
    // Minimum diff is 90 - 89 = 1 (not a direct parent-child edge).
    TreeArena arena;
    TreeNode* n49 = arena.make(49, nullptr, arena.make(52));
    TreeNode* n69 = arena.make(69, n49, arena.make(89));
    TreeNode* root = arena.make(90, n69, nullptr);
    Solution sol;
    ctx.expect_eq(sol.minDiffInBST(root), 1);
    ctx.expect_eq(sol.minDiffInBSTByBounds(root), 1);
  });

  ctx.test("two_node_bst", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(10, nullptr, arena.make(25));
    Solution sol;
    ctx.expect_eq(sol.minDiffInBST(root), 15);
    ctx.expect_eq(sol.minDiffInBSTByBounds(root), 15);
  });

  ctx.test("duplicate_values_give_zero_diff", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(5, arena.make(5), arena.make(10));
    Solution sol;
    ctx.expect_eq(sol.minDiffInBST(root), 0);
    ctx.expect_eq(sol.minDiffInBSTByBounds(root), 0);
  });
}

}  // namespace
