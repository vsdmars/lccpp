// LeetCode 543: Diameter Of Binary Tree
// https://leetcode.com/problems/diameter-of-binary-tree/
//
// Problem:
//   Given the root of a binary tree, return the length of the diameter of the
//   tree. The diameter is the length (number of edges) of the longest path
//   between any two nodes in the tree; this path may or may not pass through
//   the root.
//
// Algorithm:
//   Use a single post-order DFS that computes the height (longest downward
//   chain in nodes) of each subtree while simultaneously tracking the maximum
//   diameter seen so far. For any node `u` whose left and right subtrees have
//   heights `left_h` and `right_h`, the longest path with `u` as its highest
//   (turning) node has length `left_h + right_h` edges. Updating a shared
//   `max_diameter` with `left_h + right_h` at every node guarantees that we
//   consider both root-crossing paths and paths confined inside any subtree.
//   The recursion then returns `1 + max(left_h, right_h)` to `u`'s parent.
//   Time Complexity: O(n), visiting each node exactly once.
//   Space Complexity: O(h), where h is the tree height (O(log n) balanced,
//   O(n) worst-case skewed) for the recursion call stack.
//
// Data Structures:
//   - Binary tree (`TreeNode`) traversed via bottom-up post-order DFS.
//
// Real-World Applications:
//   - Network topology analysis: computing worst-case hop latency (diameter)
//     across a spanning tree or hierarchical datacenter routing fabric.
//   - Distributed consensus and broadcast trees: bounding worst-case message
//     propagation delay between any two peers.
//   - Phylogenetics and taxonomy trees: measuring maximum evolutionary
//     distance between leaf species.
//
// Related LeetCode Problems:
//   - #104 Maximum Depth of Binary Tree [done] (core height recurrence used here)
//   - #124 Binary Tree Maximum Path Sum [done] (weighted analog using node values instead of edge counts)
//   - #687 Longest Univalue Path [done] (same turning-point DFS with value-equality constraint)
//   - #1522 Diameter of N-Ary Tree [done] (generalization from binary to k-ary trees)

#include "framework/lc_framework.h"

#include <algorithm>
#include <deque>

namespace {

// Standard binary tree node used by LeetCode tree problems.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;
};

class Solution {
 public:
  // Computes the longest path length (in edges) between any two tree nodes.
  // Time: O(n), Space: O(h).
  int diameterOfBinaryTree(const TreeNode* root) {
    int max_diameter = 0;

    // Post-order DFS returning the height (in nodes) of `node`'s subtree.
    // Side effect: updates `max_diameter` with the longest path whose highest
    // vertex is `node`.
    auto height = [&](this auto&& self, const TreeNode* node) -> int {
      if (node == nullptr) {
        return 0;
      }

      const int left_h = self(node->left);
      const int right_h = self(node->right);

      // A path turning at `node` uses `left_h` edges down the left side plus
      // `right_h` edges down the right side.
      max_diameter = std::max(max_diameter, left_h + right_h);

      // Propagate the single longest downward branch upward to the parent.
      return 1 + std::max(left_h, right_h);
    };

    height(root);
    return max_diameter;
  }
};

// RAII arena that owns test TreeNode instances without raw heap allocations.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    nodes_.push_back(TreeNode{.val = val, .left = left, .right = right});
    return &nodes_.back();
  }

 private:
  std::deque<TreeNode> nodes_;
};

LC_PROBLEM(543, "Diameter Of Binary Tree", "tree", ctx) {
  Solution sol;

  ctx.test("example_1_through_root", [&] {
    //         1
    //        / \
    //       2   3
    //      / \
    //     4   5
    // Longest paths: [4,2,1,3] or [5,2,1,3] -> 3 edges.
    TreeArena arena;
    TreeNode* n2 = arena.make(2, arena.make(4), arena.make(5));
    TreeNode* root = arena.make(1, n2, arena.make(3));
    ctx.expect_eq(sol.diameterOfBinaryTree(root), 3);
  });

  ctx.test("example_2_two_nodes", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(1, arena.make(2), nullptr);
    ctx.expect_eq(sol.diameterOfBinaryTree(root), 1);
  });

  ctx.test("diameter_entirely_in_left_subtree", [&] {
    //           1
    //          /
    //         2
    //        / \
    //       3   4
    //      /     \
    //     5       6
    //    /         \
    //   7           8
    // Longest path is [7,5,3,2,4,6,8] = 6 edges (does not pass through root 1).
    TreeArena arena;
    TreeNode* left_chain =
        arena.make(3, arena.make(5, arena.make(7), nullptr), nullptr);
    TreeNode* right_chain =
        arena.make(4, nullptr, arena.make(6, nullptr, arena.make(8)));
    TreeNode* n2 = arena.make(2, left_chain, right_chain);
    TreeNode* root = arena.make(1, n2, nullptr);
    ctx.expect_eq(sol.diameterOfBinaryTree(root), 6);
  });

  ctx.test("single_node_and_empty_tree", [&] {
    TreeArena arena;
    TreeNode* solo = arena.make(1);
    ctx.expect_eq(sol.diameterOfBinaryTree(solo), 0);
    ctx.expect_eq(sol.diameterOfBinaryTree(nullptr), 0);
  });

  ctx.test("skewed_chain_of_five_nodes", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(
        1,
        arena.make(2, arena.make(3, arena.make(4, arena.make(5), nullptr),
                                 nullptr),
                   nullptr),
        nullptr);
    ctx.expect_eq(sol.diameterOfBinaryTree(root), 4);
  });
}

}  // namespace
