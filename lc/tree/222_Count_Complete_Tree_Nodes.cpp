// LeetCode 222: Count Complete Tree Nodes
// https://leetcode.com/problems/count-complete-tree-nodes/
//
// Problem:
//   Given the root of a complete binary tree, return the total number of nodes
//   in the tree in strictly better than O(n) time complexity. In a complete
//   binary tree, every level except possibly the last is completely filled, and
//   all nodes on the last level are positioned as far left as possible.
//
// Algorithm:
//   Exploit the structural invariant of complete binary trees by comparing the
//   leftmost depth of the left subtree (`left_depth`) with the leftmost depth
//   of the right subtree (`right_depth`):
//     - Case 1 (`left_depth == right_depth`): The last level reaches at least
//       the leftmost leaf of the right subtree, which guarantees that the
//       entire left subtree is a perfect binary tree of height `left_depth`.
//       Its node count plus the current root is `(2^left_depth - 1) + 1 =
//       2^left_depth`. We then recurse only into `node->right`.
//     - Case 2 (`left_depth > right_depth`, i.e., `left_depth == right_depth +
//       1`): The last level ends inside the left subtree, which guarantees that
//       the entire right subtree is a perfect binary tree of height
//       `right_depth`. Its node count plus the current root is `2^right_depth`.
//       We then recurse only into `node->left`.
//   At each step we descend one level and compute two leftmost depths in O(h)
//   time, eliminating half of the remaining tree without visiting it.
//   Time Complexity:  O((log n)^2) — O(log n) recursive levels, each taking
//                     O(log n) pointer hops to measure leftmost depth.
//   Space Complexity: O(log n) recursion stack (tree height h = floor(log2 n)).
//
// Data Structures:
//   - Binary tree (`TreeNode` pointers) navigated along leftmost spines.
//   - Implicit call stack of depth O(log n).
//
// Real-World Applications:
//   - Fast cardinality queries on pointer-based complete-tree indexes and
//     distributed binary-heap / tournament-tree structures where size metadata
//     is not maintained centrally.
//   - Counting active leaves in Merkle trees and cryptographic hash trees with
//     left-packed payloads.
//   - Binary search over implicit complete-tree address spaces in storage
//     engines and buddy memory allocators.
//
// Related LeetCode Problems:
//   - #104 Maximum Depth of Binary Tree [done] (computing tree height recursively)
//   - #958 Check Completeness of a Binary Tree [done] (verifying the complete-tree structural invariant)
//   - #919 Complete Binary Tree Inserter [done] (maintaining completeness during insertions)
//   - #297 Serialize And Deserialize Binary Tree [done] (encoding and reconstructing binary tree topology)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <vector>

namespace {

struct TreeNode {
  int val;
  TreeNode* left;
  TreeNode* right;

  explicit TreeNode(int v, TreeNode* l = nullptr, TreeNode* r = nullptr)
      : val(v), left(l), right(r) {}
};

class Solution {
 public:
  // Counts nodes in a complete binary tree in O((log n)^2) time.
  // Time: O((log n)^2), Space: O(log n) recursion stack.
  int countNodes(const TreeNode* root) {
    if (root == nullptr) {
      return 0;
    }

    const int left_depth = left_spine_depth(root->left);
    const int right_depth = left_spine_depth(root->right);

    if (left_depth == right_depth) {
      // Left subtree is a perfect binary tree of height `left_depth`:
      // (2^left_depth - 1) nodes in left subtree + 1 for `root` = 2^left_depth.
      return (1 << left_depth) + countNodes(root->right);
    }

    // `left_depth > right_depth`: right subtree is a perfect binary tree of
    // height `right_depth`: (2^right_depth - 1) + 1 for `root` = 2^right_depth.
    return (1 << right_depth) + countNodes(root->left);
  }

 private:
  // Returns the height of a complete subtree by walking its leftmost spine.
  // Time: O(log n), Space: O(1).
  static int left_spine_depth(const TreeNode* node) {
    int depth = 0;
    while (node != nullptr) {
      ++depth;
      node = node->left;
    }
    return depth;
  }
};

// Builds a complete binary tree with `n` nodes labeled 1..n inside `arena`.
TreeNode* build_complete_tree(std::deque<TreeNode>& arena, int n) {
  if (n <= 0) {
    return nullptr;
  }

  std::vector<TreeNode*> nodes;
  nodes.reserve(static_cast<std::size_t>(n));
  for (int i = 1; i <= n; ++i) {
    nodes.push_back(&arena.emplace_back(i));
  }

  for (std::size_t i = 0uz; i < nodes.size(); ++i) {
    const std::size_t left_idx = (2uz * i) + 1uz;
    const std::size_t right_idx = (2uz * i) + 2uz;
    if (left_idx < nodes.size()) {
      nodes[i]->left = nodes[left_idx];
    }
    if (right_idx < nodes.size()) {
      nodes[i]->right = nodes[right_idx];
    }
  }
  return nodes[0];
}

LC_PROBLEM(222, "Count Complete Tree Nodes", "tree", ctx) {
  Solution sol;

  ctx.test("leetcode_example_6_nodes", [&] {
    std::deque<TreeNode> arena;
    TreeNode* root = build_complete_tree(arena, 6);
    ctx.expect_eq(sol.countNodes(root), 6);
  });

  ctx.test("source_build_8_nodes", [&] {
    std::deque<TreeNode> arena;
    TreeNode* root = build_complete_tree(arena, 8);
    ctx.expect_eq(sol.countNodes(root), 8);
  });

  ctx.test("empty_and_single_node", [&] {
    ctx.expect_eq(sol.countNodes(nullptr), 0);
    TreeNode single(1);
    ctx.expect_eq(sol.countNodes(&single), 1);
  });

  ctx.test("all_sizes_0_through_33", [&] {
    // Exhaustively covers perfect trees (1, 3, 7, 15, 31) and every partial
    // last-level boundary on both left and right halves.
    for (int n = 0; n <= 33; ++n) {
      std::deque<TreeNode> arena;
      TreeNode* root = build_complete_tree(arena, n);
      ctx.expect_eq(sol.countNodes(root), n);
    }
  });
}

}  // namespace
