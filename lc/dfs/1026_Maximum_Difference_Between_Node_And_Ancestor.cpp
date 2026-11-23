// LeetCode 1026: Maximum Difference Between Node And Ancestor
// https://leetcode.com/problems/maximum-difference-between-node-and-ancestor/
//
// Problem:
//   Given the root of a binary tree with between 2 and 5000 nodes (each node
//   holding an integer value in [0, 10^5]), find the maximum value V for which
//   there exist two different nodes A and B such that A is an ancestor of B
//   and V = |A.val - B.val|.
//
// Examples:
//   Example 1:
//     Input:  root = [8, 3, 10, 1, 6, null, 14, null, null, 4, 7, 13]
//     Output: 7
//     Explanation:
//       - Start DFS at root 8 with path_min = 8 and path_max = 8, branching into
//         subtrees rooted at 3 and 10.
//       - Along path 8 -> 3 -> 1, update [next_min, next_max] to [3, 8] at node 3
//         and [1, 8] at leaf 1; its null children return 8 - 1 = 7.
//       - Along paths 8 -> 3 -> 6 -> 4 and 8 -> 3 -> 6 -> 7, [next_min, next_max]
//         stays [3, 8], so leaves 4 and 7 return 8 - 3 = 5, giving max(7, 5) = 7
//         for the left subtree of 8.
//       - Along path 8 -> 10 -> 14 -> 13, [next_min, next_max] becomes [8, 10] at
//         10 (null left child returns 2) and [8, 14] at 14 and 13 (returning 14 - 8
//         = 6), giving 6 for the right subtree.
//       - Combine both subtrees at the root via std::max(7, 6) to return 7.
//
//   Example 2:
//     Input:  root = [1, null, 2, null, 0, 3]
//     Output: 3
//     Explanation:
//       - Start DFS at root 1 with path_min = 1 and path_max = 1; its null left
//         child returns 1 - 1 = 0 while its right call passes [1, 1] to node 2.
//       - At node 2, update [next_min, next_max] to [1, 2]; its null left child
//         returns 1 and its right call passes [1, 2] to node 0.
//       - At node 0, lower next_min to 0 with next_max = 2; its null right child
//         returns 2 - 0 = 2 and its left call passes [0, 2] to leaf 3.
//       - At leaf 3, raise next_max to 3 with next_min = 0; its null children
//         return 3 - 0 = 3, which bubbles up as the maximum difference 3.
//
//   Example 3:
//     Input:  root = [3]
//     Output: 0
//     Explanation:
//       - Start DFS at the single node 3 with path_min = 3 and path_max = 3,
//         keeping next_min = 3 and next_max = 3.
//       - Both null children return path_max - path_min = 3 - 3 = 0, so std::max(0,
//         0) returns 0.
//
// Algorithm:
//   Any ancestor-descendant pair lies on some root-to-leaf path. Because
//   maximizing |A.val - B.val| along a path is equivalent to finding
//   (max_on_path - min_on_path) along that path, we perform a single top-down
//   DFS carrying the running minimum `path_min` and running maximum `path_max`
//   encountered from the root down to the current node.
//   - At each non-null node, update `next_min = min(path_min, node->val)` and
//     `next_max = max(path_max, node->val)` and recurse into both children.
//   - When reaching a null child (end of a root-to-leaf branch), return
//     `path_max - path_min`.
//   - Combine child results via `std::max(left_diff, right_diff)`.
//
//   Time Complexity:  O(n), visiting each node exactly once.
//   Space Complexity: O(h) recursion stack, where h is the tree height
//                     (O(log n) balanced, O(n) skewed).
//
// Data Structures:
//   - Binary tree (`TreeNode` with integer value and left/right pointers).
//   - Implicit DFS call stack propagating two scalar path extrema (`path_min`
//     and `path_max`) per frame.
//
// Real-World Applications:
//   - Financial time-series / scenario-tree drawdown analysis: computing the
//     worst peak-to-trough swing along any branching execution path.
//   - Call-graph / distributed-trace profiling: finding the largest latency or
//     memory-usage delta between a parent span and any nested descendant span.
//   - File-system / hierarchical-configuration auditing: detecting the largest
//     version or quota drift along any inheritance chain.
//
// Related LeetCode Problems:
//   - #104 Maximum Depth of Binary Tree [done] (foundational top-down/bottom-up tree DFS)
//   - #124 Binary Tree Maximum Path Sum [done] (tree DFS maximizing a path metric)
//   - #112 Path Sum [done] (root-to-leaf path state propagation via DFS)
//   - #1448 Count Good Nodes in Binary Tree [new] (tracking running path maximum in DFS)

#include "framework/lc_framework.h"

#include <algorithm>
#include <deque>

namespace {

// Binary tree node matching LeetCode's standard representation.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;
};

// RAII arena for allocating test TreeNodes without raw new/delete leaks.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    nodes_.push_back(TreeNode{.val = val, .left = left, .right = right});
    return &nodes_.back();
  }

 private:
  std::deque<TreeNode> nodes_;
};

class Solution {
 public:
  // Returns the maximum |ancestor.val - descendant.val| over all
  // ancestor-descendant pairs in the binary tree rooted at `root`.
  //
  // Complexity: O(n) time, O(h) auxiliary stack space.
  int maxAncestorDiff(const TreeNode* root) {
    if (root == nullptr) {
      return 0;
    }

    // Top-down DFS invariant: `path_min` and `path_max` are the minimum and
    // maximum values seen along the prefix from the root to `node`'s parent.
    auto dfs = [](this auto&& self, const TreeNode* node, int path_min,
                  int path_max) -> int {
      // Base case: reaching past a leaf finalizes the root-to-leaf span.
      if (node == nullptr) {
        return path_max - path_min;
      }

      const int next_min = std::min(path_min, node->val);
      const int next_max = std::max(path_max, node->val);

      const int left_best = self(node->left, next_min, next_max);
      const int right_best = self(node->right, next_min, next_max);
      return std::max(left_best, right_best);
    };

    return dfs(root, root->val, root->val);
  }
};

LC_PROBLEM(1026, "Maximum Difference Between Node And Ancestor", "dfs", ctx) {
  Solution sol;

  ctx.test("example_1_standard_bst_shape", [&] {
    // Tree: [8,3,10,1,6,null,14,null,null,4,7,13] -> |8 - 1| = 7.
    TreeArena arena;
    TreeNode* n4 = arena.make(4);
    TreeNode* n7 = arena.make(7);
    TreeNode* n6 = arena.make(6, n4, n7);
    TreeNode* n1 = arena.make(1);
    TreeNode* n3 = arena.make(3, n1, n6);
    TreeNode* n13 = arena.make(13);
    TreeNode* n14 = arena.make(14, n13, nullptr);
    TreeNode* n10 = arena.make(10, nullptr, n14);
    TreeNode* root = arena.make(8, n3, n10);

    ctx.expect_eq(sol.maxAncestorDiff(root), 7);
  });

  ctx.test("example_2_skewed_chain", [&] {
    // Tree: [1,null,2,null,0,3] -> |0 - 3| = 3.
    TreeArena arena;
    TreeNode* n3 = arena.make(3);
    TreeNode* n0 = arena.make(0, n3, nullptr);
    TreeNode* n2 = arena.make(2, nullptr, n0);
    TreeNode* root = arena.make(1, nullptr, n2);

    ctx.expect_eq(sol.maxAncestorDiff(root), 3);
  });

  ctx.test("single_node_zero_diff", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(3);
    ctx.expect_eq(sol.maxAncestorDiff(root), 0);
  });

  ctx.test("empty_tree_returns_zero", [&] {
    ctx.expect_eq(sol.maxAncestorDiff(nullptr), 0);
  });

  ctx.test("all_equal_values", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(5, arena.make(5), arena.make(5));
    ctx.expect_eq(sol.maxAncestorDiff(root), 0);
  });

  ctx.test("large_value_span", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(0, arena.make(100000), arena.make(50000));
    ctx.expect_eq(sol.maxAncestorDiff(root), 100000);
  });
}

}  // namespace
