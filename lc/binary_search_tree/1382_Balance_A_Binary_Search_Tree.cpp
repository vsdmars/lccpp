// LeetCode 1382: Balance A Binary Search Tree
// https://leetcode.com/problems/balance-a-binary-search-tree/
//
// Problem:
//   Given the root of a binary search tree (BST) containing distinct integer
//   values, return a height-balanced BST with the exact same node values. A
//   binary tree is height-balanced if the depths of the left and right
//   subtrees of every node never differ by more than 1. If multiple valid
//   balanced BSTs exist, any one of them may be returned.
//
// Examples:
//   Example 1:
//     Input:  root = [1, null, 2, null, 3, null, 4]
//     Output: [3, 2, 4, 1] (height 3, in-order [1, 2, 3, 4])
//     Explanation:
//       - collect_inorder traverses the right-skewed chain and records the sorted
//         node pointers [1, 2, 3, 4].
//       - build_balanced(nodes, 0, 4) selects mid = 0 + (4 - 0) / 2 = 2 (node 3) as
//         the new root.
//       - The left slice [0, 2) selects mid = 1 (node 2) with left child nodes[0]
//         (node 1) and null right child, while the right slice [3, 4) selects node
//         4.
//       - The resulting tree [3, 2, 4, 1] preserves in-order [1, 2, 3, 4] with
//         balanced height 3.
//
//   Example 2:
//     Input:  root = [7, 6, null, 5, null, 4, null, 3, null, 2, null, 1]
//     Output: [4, 2, 6, 1, 3, 5, 7] (height 3, in-order [1, 2, 3, 4, 5, 6, 7])
//     Explanation:
//       - collect_inorder walks the left-skewed chain from bottom to top, producing
//         sorted pointers [1, 2, 3, 4, 5, 6, 7].
//       - build_balanced(nodes, 0, 7) picks mid = 3 (node 4) as the root, splitting
//         the remaining nodes into [0, 3) and [4, 7).
//       - The left slice [0, 3) builds subtree [2, 1, 3] and the right slice [4, 7)
//         builds subtree [6, 5, 7], yielding a complete height-3 tree.
//
//   Example 3:
//     Input:  root = [42]
//     Output: [42] (height 1)
//     Explanation:
//       - collect_inorder visits the single node and stores [42] in sorted_nodes.
//       - build_balanced(nodes, 0, 1) chooses mid = 0 (node 42) and attaches null
//         for empty slices [0, 0) and [1, 1), returning node 42 with height 1.
//
// Algorithm:
//   1. Perform an in-order traversal (left -> node -> right) of the input BST
//      to collect all node pointers in strictly ascending value order.
//   2. Recursively rebuild a minimum-height BST over the half-open slice
//      [lo, hi) of the sorted node list by picking the middle index
//      mid = lo + (hi - lo) / 2 as the root, recursively attaching [lo, mid)
//      as its left subtree and [mid + 1, hi) as its right subtree.
//   Because every recursive split divides the remaining nodes as evenly as
//   possible (sizes differ by at most 1), the resulting tree's left and right
//   subtree heights differ by at most 1 at every node, and reusing existing
//   TreeNode objects avoids extra heap allocations.
//   Time Complexity: O(N) — each node is visited once during in-order
//   traversal and once during balanced reconstruction.
//   Space Complexity: O(N) — O(N) for the flat node pointer array plus O(H)
//   for the in-order recursion stack (where H is the original tree height,
//   O(N) worst case) and O(log N) for the balanced rebuild recursion stack.
//
// Data Structures:
//   - TreeNode (intrusive binary tree node with left/right pointers):
//     re-linked in place to form the balanced output BST.
//   - std::vector<TreeNode*>: stores the in-order sequence of nodes so any
//     midpoint can be accessed in O(1) time during divide-and-conquer rebuild.
//
// Real-World Applications:
//   - Database B-tree / in-memory index bulk-loading and periodic rebalancing
//     after skewed insertions degrade search latency from O(log N) to O(N).
//   - Compiler symbol tables and interval/spatial trees rebuilt from sorted
//     snapshots for optimal cache-friendly lookup depth.
//   - Static search-tree construction for read-heavy routing and lookup tables.
//
// Related LeetCode Problems:
//   - #108 Convert Sorted Array to Binary Search Tree [done] (exact second phase: building a height-balanced BST from a sorted array)
//   - #109 Convert Sorted List to Binary Search Tree [done] (builds a balanced BST from sorted linked-list nodes)
//   - #110 Balanced Binary Tree [done] (verifies the height-balance invariant in O(N) bottom-up DFS)
//   - #98 Validate Binary Search Tree [done] (verifies the BST ordering invariant)
//   - #94 Binary Tree Inorder Traversal [done] (extracts sorted node order from a BST)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <deque>
#include <vector>

namespace {

// Binary tree node matching LeetCode's standard representation.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;
};

class Solution {
 public:
  // Rebalances an arbitrary BST into a height-balanced BST containing the same
  // nodes by extracting its sorted in-order sequence and recursively choosing
  // midpoints as subtree roots.
  // Time: O(N), Space: O(N).
  TreeNode* balanceBST(TreeNode* root) {
    if (root == nullptr) {
      return nullptr;
    }

    // Phase 1: Flatten the BST into sorted ascending order via in-order DFS.
    std::vector<TreeNode*> sorted_nodes;
    collect_inorder(root, sorted_nodes);

    // Phase 2: Re-link the sorted nodes into a minimum-height balanced BST.
    return build_balanced(sorted_nodes, 0uz, sorted_nodes.size());
  }

 private:
  // Appends all nodes of the subtree rooted at `node` in ascending key order.
  static void collect_inorder(TreeNode* node,
                              std::vector<TreeNode*>& sorted_nodes) {
    if (node == nullptr) {
      return;
    }
    collect_inorder(node->left, sorted_nodes);
    sorted_nodes.push_back(node);
    collect_inorder(node->right, sorted_nodes);
  }

  // Recursively builds a height-balanced BST from `nodes[lo..hi)` by selecting
  // the midpoint as root so left and right subtree sizes differ by at most 1.
  static TreeNode* build_balanced(const std::vector<TreeNode*>& nodes,
                                  std::size_t lo, std::size_t hi) {
    if (lo >= hi) {
      return nullptr;
    }

    const std::size_t mid = lo + ((hi - lo) / 2uz);
    TreeNode* root = nodes[mid];

    // Re-attach children directly onto the existing node; stale links from the
    // original unbalanced tree are overwritten on every node in the range.
    root->left = build_balanced(nodes, lo, mid);
    root->right = build_balanced(nodes, mid + 1uz, hi);
    return root;
  }
};

// RAII arena that owns TreeNode instances with stable pointer addresses
// (std::deque never invalidates references/pointers on push_back).
class TreeArena {
 public:
  TreeNode* make_node(int val, TreeNode* left = nullptr,
                      TreeNode* right = nullptr) {
    nodes_.push_back(TreeNode{.val = val, .left = left, .right = right});
    return &nodes_.back();
  }

 private:
  std::deque<TreeNode> nodes_;
};

// Collects node values in-order to verify BST ordering and value preservation.
void collect_values_inorder(const TreeNode* node, std::vector<int>& out) {
  if (node == nullptr) {
    return;
  }
  collect_values_inorder(node->left, out);
  out.push_back(node->val);
  collect_values_inorder(node->right, out);
}

// Returns the tree height if height-balanced, or -1 if any subtree violates
// |height(left) - height(right)| <= 1.
int check_balanced_height(const TreeNode* node) {
  if (node == nullptr) {
    return 0;
  }
  const int left_h = check_balanced_height(node->left);
  if (left_h == -1) {
    return -1;
  }
  const int right_h = check_balanced_height(node->right);
  if (right_h == -1) {
    return -1;
  }
  if (std::abs(left_h - right_h) > 1) {
    return -1;
  }
  return 1 + std::max(left_h, right_h);
}

LC_PROBLEM(1382, "Balance A Binary Search Tree", "binary_search_tree", ctx) {
  ctx.test("example_1_right_skewed_chain", [&] {
    TreeArena arena;
    TreeNode* n4 = arena.make_node(4);
    TreeNode* n3 = arena.make_node(3, nullptr, n4);
    TreeNode* n2 = arena.make_node(2, nullptr, n3);
    TreeNode* n1 = arena.make_node(1, nullptr, n2);

    Solution sol;
    TreeNode* balanced = sol.balanceBST(n1);

    std::vector<int> values;
    collect_values_inorder(balanced, values);
    ctx.expect_eq(values, std::vector<int>({1, 2, 3, 4}));
    const int height = check_balanced_height(balanced);
    ctx.expect_ne(height, -1);
    ctx.expect_eq(height, 3);
  });

  ctx.test("already_balanced_tree", [&] {
    TreeArena arena;
    TreeNode* n1 = arena.make_node(1);
    TreeNode* n3 = arena.make_node(3);
    TreeNode* n2 = arena.make_node(2, n1, n3);

    Solution sol;
    TreeNode* balanced = sol.balanceBST(n2);

    std::vector<int> values;
    collect_values_inorder(balanced, values);
    ctx.expect_eq(values, std::vector<int>({1, 2, 3}));
    ctx.expect_eq(check_balanced_height(balanced), 2);
  });

  ctx.test("left_skewed_chain", [&] {
    TreeArena arena;
    TreeNode* cur = nullptr;
    for (int v = 1; v <= 7; ++v) {
      cur = arena.make_node(v, cur, nullptr);
    }

    Solution sol;
    TreeNode* balanced = sol.balanceBST(cur);

    std::vector<int> values;
    collect_values_inorder(balanced, values);
    ctx.expect_eq(values, std::vector<int>({1, 2, 3, 4, 5, 6, 7}));
    ctx.expect_eq(check_balanced_height(balanced), 3);
  });

  ctx.test("single_node_and_empty", [&] {
    TreeArena arena;
    TreeNode* single = arena.make_node(42);

    Solution sol;
    TreeNode* balanced_single = sol.balanceBST(single);
    ctx.expect_true(balanced_single != nullptr);
    ctx.expect_eq(balanced_single->val, 42);
    ctx.expect_eq(check_balanced_height(balanced_single), 1);

    ctx.expect_true(sol.balanceBST(nullptr) == nullptr);
  });

  ctx.test("degenerate_fifteen_nodes_reaches_optimal_height", [&] {
    TreeArena arena;
    TreeNode* root = arena.make_node(1);
    TreeNode* tail = root;
    std::vector<int> expected = {1};
    for (int v = 2; v <= 15; ++v) {
      tail->right = arena.make_node(v);
      tail = tail->right;
      expected.push_back(v);
    }

    Solution sol;
    TreeNode* balanced = sol.balanceBST(root);

    std::vector<int> values;
    collect_values_inorder(balanced, values);
    ctx.expect_eq(values, expected);
    ctx.expect_eq(check_balanced_height(balanced), 4);
  });
}

}  // namespace
