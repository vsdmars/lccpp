// LeetCode 104: Maximum Depth Of Binary Tree
// https://leetcode.com/problems/maximum-depth-of-binary-tree/
//
// Problem:
//   Given the root of a binary tree, return its maximum depth — defined as the
//   number of nodes along the longest downward path from the root node to any
//   leaf node (0 for an empty tree).
//
// Examples:
//   Example 1:
//     Input:  root = [3, 9, 20, null, null, 15, 7]
//     Output: 3
//     Explanation:
//       - At leaf 9, both null children return 0, so maxDepth(9) = 1 + max(0, 0) =
//         1.
//       - At leaves 15 and 7, both return depth 1, so node 20 computes 1 + max(1,
//         1) = 2.
//       - At root 3, combine left depth 1 and right depth 2 to return 1 + max(1, 2)
//         = 3.
//
//   Example 2:
//     Input:  root = [1, null, 2, null, 3, null, 4]
//     Output: 4
//     Explanation:
//       - Recurse down the right-skewed chain 1 -> 2 -> 3 -> 4 until hitting
//         nullptr at depth 0; every left child is nullptr and returns 0.
//       - Unwind post-order from leaf 4 to root 1: node 4 returns 1 + max(0, 0) =
//         1, node 3 returns 1 + max(0, 1) = 2, node 2 returns 3, and root 1 returns
//         4.
//
//   Example 3:
//     Input:  root = [1]
//     Output: 1
//     Explanation:
//       - Visit single node 1 and recurse into its null left and right children,
//         each hitting root == nullptr and returning 0.
//       - Combine both child depths at node 1 via 1 + std::max(0, 0) to return 1.
//
// Algorithm:
//   Post-order Depth-First Search (DFS) using the recursive height recurrence:
//     depth(null) = 0
//     depth(node) = 1 + max(depth(node->left), depth(node->right))
//   Each node computes the maximum height of its two subtrees bottom-up and
//   adds 1 for itself.
//   Time Complexity:  O(N), visiting every node in the tree exactly once.
//   Space Complexity: O(H) recursion call stack, where H is the tree height
//                     (O(log N) for balanced trees, O(N) for a degenerate chain).
//
// Data Structures:
//   - Binary tree nodes (`TreeNode` with value, left, and right pointers).
//   - Implicit DFS call stack propagating subtree heights bottom-up.
//   - RAII `TreeArena` (`std::deque<TreeNode>`) in tests for safe allocation.
//
// Real-World Applications:
//   - Measuring worst-case lookup latency and balance health in BSTs, AVL
//     trees, B-trees, and spatial partitioning trees (BVH / k-d trees).
//   - Computing critical path depth of expression ASTs and digital circuit
//     DAGs for compiler scheduling and timing analysis.
//   - Enforcing maximum nesting depth limits in JSON/XML/HTML parsers to
//     prevent stack-overflow denial-of-service attacks.
//
// Related LeetCode Problems:
//   - #111 Minimum Depth of Binary Tree [done] (shortest root-to-leaf path)
//   - #110 Balanced Binary Tree [done] (checks left/right height diff <= 1)
//   - #543 Diameter of Binary Tree [done] (longest path via subtree heights)
//   - #559 Maximum Depth of N-ary Tree [done] (generalization to k-ary trees)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <deque>
#include <optional>
#include <queue>
#include <vector>

namespace {

struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;
};

// RAII arena that owns TreeNode instances in a std::deque so raw pointers
// stay valid as new nodes are appended and are automatically freed on exit.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    TreeNode& node = nodes_.emplace_back();
    node.val = val;
    node.left = left;
    node.right = right;
    return &node;
  }

  // Constructs a binary tree from LeetCode's level-order array representation.
  TreeNode* fromLevelOrder(const std::vector<std::optional<int>>& vals) {
    if (vals.empty() || !vals[0].has_value()) {
      return nullptr;
    }
    TreeNode* root = make(*vals[0]);
    std::queue<TreeNode*> q;
    q.push(root);
    std::size_t idx = 1uz;
    while (!q.empty() && idx < vals.size()) {
      TreeNode* cur = q.front();
      q.pop();
      if (vals[idx].has_value()) {
        cur->left = make(*vals[idx]);
        q.push(cur->left);
      }
      ++idx;
      if (idx < vals.size() && vals[idx].has_value()) {
        cur->right = make(*vals[idx]);
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
  // Computes the maximum root-to-leaf node count via bottom-up post-order DFS.
  // Time: O(N), Space: O(H) recursion stack.
  int maxDepth(const TreeNode* root) {
    // An empty subtree contributes height 0.
    if (root == nullptr) {
      return 0;
    }
    // Combine left and right subtree heights plus the current root node.
    return 1 + std::max(maxDepth(root->left), maxDepth(root->right));
  }
};

LC_PROBLEM(104, "Maximum Depth Of Binary Tree", "dfs", ctx) {
  Solution sol;

  ctx.test("example_1_standard", [&] {
    TreeArena arena;
    const TreeNode* root =
        arena.fromLevelOrder({3, 9, 20, std::nullopt, std::nullopt, 15, 7});
    ctx.expect_eq(sol.maxDepth(root), 3);
  });

  ctx.test("python_build_tree", [&] {
    TreeArena arena;
    const TreeNode* root =
        arena.fromLevelOrder({1, 2, 3, 4, 5, std::nullopt, 7});
    ctx.expect_eq(sol.maxDepth(root), 3);
  });

  ctx.test("empty_and_single_node", [&] {
    TreeArena arena;
    ctx.expect_eq(sol.maxDepth(nullptr), 0);
    ctx.expect_eq(sol.maxDepth(arena.make(1)), 1);
  });

  ctx.test("right_skewed_chain", [&] {
    TreeArena arena;
    TreeNode* n4 = arena.make(4);
    TreeNode* n3 = arena.make(3, nullptr, n4);
    TreeNode* n2 = arena.make(2, nullptr, n3);
    const TreeNode* root = arena.make(1, nullptr, n2);
    ctx.expect_eq(sol.maxDepth(root), 4);
  });
}

}  // namespace
