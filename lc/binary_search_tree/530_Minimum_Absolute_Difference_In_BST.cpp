// LeetCode 530: Minimum Absolute Difference In BST
// https://leetcode.com/problems/minimum-absolute-difference-in-bst/
//
// Problem:
//   Given the root of a Binary Search Tree (BST) containing at least two nodes
//   with non-negative integer values, return the minimum absolute difference
//   between the values of any two distinct nodes in the tree.
//
// Algorithm:
//   Two complementary O(n) algorithms are implemented (matching both the
//   primary and `review()` solutions in the Python reference):
//   1. Iterative In-Order Traversal (`getMinimumDifference`):
//      Because an in-order traversal of a BST yields values in sorted
//      non-decreasing order, the minimum absolute difference between any two
//      nodes must occur between two consecutive nodes in the in-order sequence.
//      Use an explicit stack to iterate in-order while tracking only the
//      immediately preceding visited node `prev_node`, updating
//      `min_diff = min(min_diff, cur->val - prev_node->val)` at every step.
//   2. Bottom-Up Subtree Bounds DFS (`getMinimumDifferenceSubtreeBounds`):
//      Recursively return each subtree's `(min_val, max_val)` pair. For any
//      node, its closest smaller value in the subtree is `left_max` and its
//      closest larger value is `right_min`, so checking `node->val - left_max`
//      and `right_min - node->val` at every node finds the global minimum.
//   Time Complexity: O(n) for both approaches, visiting each node once.
//   Space Complexity: O(h), where h is the tree height (explicit stack or
//   recursion stack).
//
// Data Structures:
//   - Binary Search Tree (`TreeNode` linked structure).
//   - Explicit `std::stack<const TreeNode*>` for iterative in-order traversal.
//   - `std::pair<int, int>` subtree `(min, max)` bounds for the post-order DFS.
//
// Real-World Applications:
//   - Closest-pair / collision-threshold detection in 1D sorted indices
//     (e.g., minimum inter-arrival time between ordered timestamps).
//   - Financial order-book analytics: measuring the tightest price tick gap
//     across active limit orders stored in a BST.
//   - Signal quantization and numerical precision checks over ordered samples.
//
// Related LeetCode Problems:
//   - #783 Minimum Distance Between BST Nodes [done] (identical problem statement)
//   - #98 Validate Binary Search Tree [done] (in-order BST traversal and bounds)
//   - #501 Find Mode in Binary Search Tree [done] (adjacent-pair tracking in BST in-order)
//   - #532 K-diff Pairs in an Array [done] (absolute differences in sorted sequences)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <deque>
#include <limits>
#include <optional>
#include <queue>
#include <stack>
#include <utility>
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
  // Primary approach: iterative in-order traversal using an explicit stack.
  // Compares each visited node against its immediate in-order predecessor.
  // Time: O(n), Space: O(h).
  int getMinimumDifference(const TreeNode* root) {
    if (root == nullptr) {
      return 0;
    }

    int min_diff = std::numeric_limits<int>::max();
    std::stack<const TreeNode*> st;
    const TreeNode* prev_node = nullptr;
    const TreeNode* cur = root;

    // Seed the stack with the leftmost spine starting at `root`.
    while (cur != nullptr) {
      st.push(cur);
      cur = cur->left;
    }

    while (!st.empty()) {
      const TreeNode* node = st.top();
      st.pop();

      // Adjacent in-order values are guaranteed to contain the global minimum
      // gap because the sequence is sorted.
      if (prev_node != nullptr) {
        min_diff = std::min(min_diff, std::abs(node->val - prev_node->val));
      }
      prev_node = node;

      // Descend the leftmost spine of the right subtree next.
      cur = node->right;
      while (cur != nullptr) {
        st.push(cur);
        cur = cur->left;
      }
    }

    return (min_diff == std::numeric_limits<int>::max()) ? 0 : min_diff;
  }

  // Alternative approach (from `review()`): post-order DFS returning each
  // subtree's (min_val, max_val) and checking gaps to the predecessor
  // (`left_max`) and successor (`right_min`) at every node.
  // Time: O(n), Space: O(h).
  int getMinimumDifferenceSubtreeBounds(const TreeNode* root) {
    if (root == nullptr) {
      return 0;
    }

    int min_diff = std::numeric_limits<int>::max();

    auto dfs = [&](this auto&& self,
                   const TreeNode* node) -> std::pair<int, int> {
      int sub_min = node->val;
      int sub_max = node->val;

      if (node->left != nullptr) {
        const auto [left_min, left_max] = self(node->left);
        sub_min = left_min;
        min_diff = std::min(min_diff, std::abs(node->val - left_max));
      }

      if (node->right != nullptr) {
        const auto [right_min, right_max] = self(node->right);
        sub_max = right_max;
        min_diff = std::min(min_diff, std::abs(right_min - node->val));
      }

      return {sub_min, sub_max};
    };

    dfs(root);
    return (min_diff == std::numeric_limits<int>::max()) ? 0 : min_diff;
  }
};

LC_PROBLEM(530, "Minimum Absolute Difference In BST", "binary_search_tree",
           ctx) {
  Solution sol;

  auto check_both = [&](const TreeNode* root, int expected) {
    ctx.expect_eq(sol.getMinimumDifference(root), expected);
    ctx.expect_eq(sol.getMinimumDifferenceSubtreeBounds(root), expected);
  };

  ctx.test("leetcode_example_1", [&] {
    TreeArena arena;
    //   1
    //    \
    //     3
    //    /
    //   2
    const TreeNode* root = arena.build({1, std::nullopt, 3, 2});
    check_both(root, 1);
  });

  ctx.test("python_build_balanced_bst", [&] {
    TreeArena arena;
    const TreeNode* root = arena.build({4, 2, 6, 1, 3});
    check_both(root, 1);
  });

  ctx.test("leetcode_example_2", [&] {
    TreeArena arena;
    const TreeNode* root =
        arena.build({1, 0, 48, std::nullopt, std::nullopt, 12, 49});
    check_both(root, 1);
  });

  ctx.test("closest_pair_across_ancestor_and_deep_descendant", [&] {
    TreeArena arena;
    // Closest pair is 236 (root) and 227 (right child of left child) -> diff 9.
    const TreeNode* root =
        arena.build({236, 104, 701, std::nullopt, 227, std::nullopt, 911});
    check_both(root, 9);
  });

  ctx.test("two_nodes_and_duplicate_values", [&] {
    TreeArena arena;
    check_both(arena.build({10, 3}), 7);
    check_both(arena.build({5, 5}), 0);
  });
}

}  // namespace
