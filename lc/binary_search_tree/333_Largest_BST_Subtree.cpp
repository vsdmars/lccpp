// LeetCode 333: Largest BST Subtree
// https://leetcode.com/problems/largest-bst-subtree/
//
// Problem:
//   Given the root of a binary tree, find the largest subtree that is also a
//   valid Binary Search Tree (BST), where "largest" means containing the
//   maximum number of nodes. A subtree must include a node together with all
//   of its descendants. A valid BST requires every node's value to be strictly
//   greater than all values in its left subtree and strictly less than all
//   values in its right subtree. Return the node count of that largest BST
//   subtree (0 for an empty tree).
//
// Examples:
//   Example 1:
//     Input:  root = [10, 5, 15, 1, 8, null, 7]
//     Output: 3
//     Explanation:
//       - Post-order DFS evaluates leaves 1 and 8, each returning a valid BST
//         summary of size 1 with min_val = max_val = val.
//       - At node 5, 1 < 5 < 8 holds, so subtree [5, 1, 8] is a valid BST returning
//         {is_bst = true, min_val = 1, max_val = 8, largest_size = 3}.
//       - At node 15, right child 7 violates 15 < right.min_val (7), so node 15
//         returns {is_bst = false, largest_size = 1}.
//       - At root 10, right.is_bst is false, so it returns {is_bst = false,
//         largest_size = max(3, 1) = 3}.
//
//   Example 2:
//     Input:  root = [4, 1, 6, null, null, 5, null, 2]
//     Output: 3
//     Explanation:
//       - Post-order DFS evaluates leaf 2 with size 1, and node 5 validates 2 < 5
//         to return {is_bst = true, min_val = 2, max_val = 5, largest_size = 2}.
//       - At node 6, left.max_val (5) < 6 holds, so subtree [6, 5, 2] returns
//         {is_bst = true, min_val = 2, max_val = 6, largest_size = 3}.
//       - At root 4, left child 1 returns size 1, while right child 6 has
//         right.min_val = 2, violating 4 < 2.
//       - Root 4 therefore marks is_bst = false and propagates largest_size =
//         max(1, 3) = 3.
//
//   Example 3:
//     Input:  root = [2, 2, 2]
//     Output: 1
//     Explanation:
//       - Both leaves with value 2 return {is_bst = true, min_val = 2, max_val = 2,
//         largest_size = 1}.
//       - At root 2, strict BST checks left.max_val (2) < 2 and 2 < right.min_val
//         (2) both fail.
//       - Root 2 marks is_bst = false and returns largest_size = max(1, 1) = 1.
//
// Algorithm:
//   Use a single bottom-up post-order DFS. Each recursive call evaluates the
//   left and right subtrees first and returns a compact summary record:
//   whether the subtree is itself a valid BST, its minimum and maximum node
//   values (when valid), and the size of the largest BST subtree found
//   anywhere inside it. A node forms a valid BST with its children if and only
//   if both children are valid BSTs, the left subtree's maximum is strictly
//   less than node->val (when left exists), and node->val is strictly less
//   than the right subtree's minimum (when right exists). When valid, the
//   combined size is 1 + left.size + right.size; otherwise, the node cannot be
//   part of any larger BST ancestor, so we mark is_bst = false and propagate
//   max(left.size, right.size) upward.
//   Time Complexity: O(n), visiting every node exactly once.
//   Space Complexity: O(h) for the recursion stack, where h is the tree height
//   (O(log n) for balanced trees, O(n) worst case for skewed trees).
//
// Data Structures:
//   - Binary tree (`TreeNode` linked structure) traversed recursively.
//   - `SubtreeInfo` value struct returned up the call stack to aggregate
//     per-subtree validity, bounds, and best BST size without global state.
//
// Real-World Applications:
//   - Database and storage-engine integrity verification: locating the largest
//     uncorrupted ordered index subtree after a partial B-tree/BST write fault.
//   - Compiler AST and expression-tree analysis: bottom-up synthesized
//     attribute propagation to validate ordering invariants across subtrees.
//   - Hierarchical interval/range partitioning validation in spatial indices.
//
// Related LeetCode Problems:
//   - #98 Validate Binary Search Tree [done] (top-down/bottom-up BST bounds)
//   - #1373 Maximum Sum BST in Binary Tree [new] (same bottom-up DFS optimizing sum instead of size)
//   - #99 Recover Binary Search Tree [done] (detecting and fixing violated BST order)
//   - #530 Minimum Absolute Difference in BST [done] (subtree min/max bounds in BSTs)

#include "framework/lc_framework.h"

#include <algorithm>
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

// RAII arena that owns all test TreeNodes so pointer graphs never leak memory.
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

class Solution {
 public:
  // Returns the node count of the largest subtree in `root` that is a valid
  // strict BST.
  // Time: O(n), Space: O(h).
  int largestBSTSubtree(const TreeNode* root) {
    // Synthesized bottom-up summary for a subtree.
    // Using an explicit `is_bst` flag avoids sentinel collisions when node
    // values equal INT_MIN or INT_MAX.
    struct SubtreeInfo {
      bool is_bst = true;
      int min_val = 0;
      int max_val = 0;
      int largest_size = 0;
    };

    auto dfs = [](this auto&& self, const TreeNode* node) -> SubtreeInfo {
      // An empty subtree is trivially a valid BST of size 0.
      if (node == nullptr) {
        return SubtreeInfo{.is_bst = true,
                           .min_val = 0,
                           .max_val = 0,
                           .largest_size = 0};
      }

      // Post-order: evaluate both children before deciding about `node`.
      const SubtreeInfo left = self(node->left);
      const SubtreeInfo right = self(node->right);

      // Check strict BST ordering against the nearest bounds from each side.
      const bool left_ok =
          left.is_bst && (node->left == nullptr || left.max_val < node->val);
      const bool right_ok =
          right.is_bst && (node->right == nullptr || node->val < right.min_val);

      if (left_ok && right_ok) {
        // The entire subtree rooted at `node` is a valid BST; expand its
        // [min_val, max_val] range and sum both child sizes plus `node`.
        const int cur_min = (node->left != nullptr) ? left.min_val : node->val;
        const int cur_max =
            (node->right != nullptr) ? right.max_val : node->val;
        return SubtreeInfo{
            .is_bst = true,
            .min_val = cur_min,
            .max_val = cur_max,
            .largest_size = 1 + left.largest_size + right.largest_size,
        };
      }

      // Otherwise, no ancestor containing `node` can be a valid BST; carry up
      // the largest valid BST size seen in either child subtree.
      return SubtreeInfo{
          .is_bst = false,
          .min_val = 0,
          .max_val = 0,
          .largest_size = std::max(left.largest_size, right.largest_size),
      };
    };

    return dfs(root).largest_size;
  }
};

LC_PROBLEM(333, "Largest BST Subtree", "binary_search_tree", ctx) {
  Solution sol;

  ctx.test("leetcode_example", [&] {
    TreeArena arena;
    //     10
    //    /  \
    //   5   15
    //  / \    \
    // 1   8    7
    // Largest BST subtree is [5, 1, 8] with size 3.
    const TreeNode* root = arena.build({10, 5, 15, 1, 8, std::nullopt, 7});
    ctx.expect_eq(sol.largestBSTSubtree(root), 3);
  });

  ctx.test("python_build_nested_violation", [&] {
    TreeArena arena;
    //     4
    //    / \
    //   1   6
    //      /
    //     5
    //    /
    //   2
    // Subtree rooted at 6 -> [2, 5, 6] is a valid BST of size 3, while root 4
    // fails because 2 is in its right subtree.
    TreeNode* n2 = arena.make(2);
    TreeNode* n5 = arena.make(5, n2, nullptr);
    TreeNode* n6 = arena.make(6, n5, nullptr);
    TreeNode* n1 = arena.make(1);
    const TreeNode* root = arena.make(4, n1, n6);
    ctx.expect_eq(sol.largestBSTSubtree(root), 3);
  });

  ctx.test("empty_and_single_node", [&] {
    TreeArena arena;
    ctx.expect_eq(sol.largestBSTSubtree(nullptr), 0);
    const TreeNode* single = arena.make(42);
    ctx.expect_eq(sol.largestBSTSubtree(single), 1);
  });

  ctx.test("entire_tree_is_valid_bst", [&] {
    TreeArena arena;
    const TreeNode* root = arena.build({8, 4, 12, 2, 6, 10, 14});
    ctx.expect_eq(sol.largestBSTSubtree(root), 7);
  });

  ctx.test("duplicates_violate_strict_bst", [&] {
    TreeArena arena;
    // Equal values violate strict `<` BST inequality, so each leaf is size 1.
    const TreeNode* root = arena.build({2, 2, 2});
    ctx.expect_eq(sol.largestBSTSubtree(root), 1);
  });

  ctx.test("extreme_int_bounds", [&] {
    TreeArena arena;
    constexpr int kMin = std::numeric_limits<int>::min();
    constexpr int kMax = std::numeric_limits<int>::max();
    const TreeNode* root = arena.build({0, kMin, kMax});
    ctx.expect_eq(sol.largestBSTSubtree(root), 3);
  });
}

}  // namespace
