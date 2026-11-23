// LeetCode 366: Find Leaves Of Binary Tree
// https://leetcode.com/problems/find-leaves-of-binary-tree/
//
// Problem:
//   Given the `root` of a binary tree, collect all node values in layers as if
//   repeatedly collecting and removing all current leaf nodes until the tree is
//   empty. Return a list of lists where index `i` contains the values of nodes
//   removed in round `i`.
//
// Examples:
//   Example 1:
//     Input:  root = [1, 2, 3, 4, 5]
//     Output: [[4, 5, 3], [2], [1]]
//     Explanation:
//       - Post-order DFS visits leaves 4 and 5 first; both have null children
//         returning -1, so each gets height = 1 + max(-1, -1) = 0 and enters
//         layers[0].
//       - Unwinding to node 2 with left_h = 0 and right_h = 0 gives height = 1 +
//         max(0, 0) = 1, placing 2 into layers[1].
//       - Visiting root->right (node 3) yields height = 0 and appends 3 to
//         layers[0], making layers[0] = [4, 5, 3].
//       - At root 1, left_h = 1 and right_h = 0 give height = 1 + max(1, 0) = 2,
//         placing 1 into layers[2] and returning [[4, 5, 3], [2], [1]].
//
//   Example 2:
//     Input:  root = [1, 2, 3, 4, null, null, null, 6]
//     Output: [[6, 3], [4], [2], [1]]
//     Explanation:
//       - Post-order traversal down the left spine assigns height 0 to leaf 6,
//         height 1 to node 4, and height 2 to node 2, populating layers[0..2].
//       - Visiting the right child 3 computes height = 0, appending 3 into
//         layers[0] alongside 6 despite their different depths from the root.
//       - At root 1, height = 1 + max(2, 0) = 3 appends 1 into layers[3], yielding
//         [[6, 3], [4], [2], [1]].
//
//   Example 3:
//     Input:  root = []
//     Output: []
//     Explanation:
//       - computeHeight(nullptr) immediately returns -1 without appending to
//         layers.
//       - findLeaves returns the empty layers vector [].
//
// Algorithm:
//   Single-pass post-order DFS computing each node's bottom-up height from the
//   deepest leaf in its subtree:
//   - A null child has height -1.
//   - Any non-null node has height `1 + max(height(left), height(right))`.
//   Because a node becomes a leaf right after both of its subtrees have been
//   completely peeled away, its removal round is strictly equal to its bottom-
//   up height (leaves at height 0, their parents at height 1, etc.). Appending
//   `node->val` to `layers[height]` during post-order traversal collects every
//   layer in O(N) time without mutating the tree.
//   Time Complexity: O(N), where N is the number of nodes in the tree.
//   Space Complexity: O(H) recursion stack space (H is tree height, O(N) worst
//     case for a skewed tree), plus O(N) for the returned layers.
//
// Data Structures:
//   - Binary tree (`TreeNode` with left/right child pointers).
//   - std::vector<std::vector<int>>: buckets nodes directly by their bottom-up
//     height (removal round).
//   - std::deque<TreeNode> (in test harness): RAII arena ensuring stable node
//     addresses with zero manual memory management.
//
// Real-World Applications:
//   - Build-system and task-scheduler wave planning (grouping dependency-tree
//     tasks into parallel execution waves from leaves upward to the root).
//   - Compiler expression-tree scheduling and bottom-up register-pressure
//     analysis (Sethi-Ullman numbering).
//   - Distributed aggregation / MapReduce reduce-tree staging where leaf
//     workers report before intermediate aggregators can fire.
//
// Related LeetCode Problems:
//   - #104 Maximum Depth of Binary Tree [done] (computing tree height with post-order DFS)
//   - #110 Balanced Binary Tree [done] (post-order subtree height checks)
//   - #102 Binary Tree Level Order Traversal [done] (grouping nodes top-down by depth)
//   - #310 Minimum Height Trees [done] (iterative leaf peeling on an undirected tree)
//   - #543 Diameter of Binary Tree [done] (post-order height aggregation across subtrees)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <deque>
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

// RAII arena that owns test TreeNode instances with pointer stability.
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
  // Collects tree nodes grouped by the round in which they would be peeled as
  // leaves (0-indexed from the outermost leaves inward to the root).
  //
  // Invariant: `height(node)` equals the exact 0-based round when `node`
  // becomes a leaf after all descendants are removed.
  //
  // Time: O(N); Space: O(H) auxiliary recursion stack.
  std::vector<std::vector<int>> findLeaves(const TreeNode* root) {
    std::vector<std::vector<int>> layers;

    auto computeHeight = [&layers](this auto&& self,
                                   const TreeNode* node) -> int {
      // Empty subtrees have height -1 so actual leaves receive height 0.
      if (node == nullptr) {
        return -1;
      }

      const int left_h = self(node->left);
      const int right_h = self(node->right);
      const int height = 1 + std::max(left_h, right_h);

      const auto bucket = static_cast<std::size_t>(height);
      if (layers.size() <= bucket) {
        layers.emplace_back();
      }
      layers[bucket].push_back(node->val);
      return height;
    };

    computeHeight(root);
    return layers;
  }
};

LC_PROBLEM(366, "Find Leaves Of Binary Tree", "dfs", ctx) {
  ctx.test("example_tree", [&] {
    //       1
    //      / \
    //     2   3
    //    / \
    //   4   5
    TreeArena arena;
    TreeNode* root = arena.make(
        1, arena.make(2, arena.make(4), arena.make(5)), arena.make(3));

    Solution sol;
    const std::vector<std::vector<int>> expected = {{4, 5, 3}, {2}, {1}};
    ctx.expect_eq(sol.findLeaves(root), expected);
  });

  ctx.test("empty_tree", [&] {
    Solution sol;
    ctx.expect_eq(sol.findLeaves(nullptr), std::vector<std::vector<int>>{});
  });

  ctx.test("single_node", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(42);
    Solution sol;
    ctx.expect_eq(sol.findLeaves(root), std::vector<std::vector<int>>{{42}});
  });

  ctx.test("skewed_chain", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(1, arena.make(2, arena.make(3), nullptr),
                                nullptr);
    Solution sol;
    const std::vector<std::vector<int>> expected = {{3}, {2}, {1}};
    ctx.expect_eq(sol.findLeaves(root), expected);
  });

  ctx.test("asymmetric_heights", [&] {
    //         1
    //        / \
    //       2   3
    //      /
    //     4
    //    /
    //   6
    TreeArena arena;
    TreeNode* root = arena.make(
        1, arena.make(2, arena.make(4, arena.make(6), nullptr), nullptr),
        arena.make(3));

    Solution sol;
    const std::vector<std::vector<int>> expected = {{6, 3}, {4}, {2}, {1}};
    ctx.expect_eq(sol.findLeaves(root), expected);
  });
}

}  // namespace
