// LeetCode 230: Kth Smallest Element In A BST
// https://leetcode.com/problems/kth-smallest-element-in-a-bst/
//
// Problem:
//   Given the root of a binary search tree (BST) and a 1-indexed integer `k`
//   (1 <= k <= number of nodes), return the k-th smallest value among all node
//   values in the BST. Returns -1 defensively if `root` is null or `k` is out
//   of range.
//
// Examples:
//   Example 1:
//     Input:  root = [3, 1, 4, null, 2], k = 2
//     Output: 2
//     Explanation:
//       - kthSmallest seeds stack with the left spine from 3, leaving stack = [3,
//         1].
//       - First iteration pops 1; since k = 2 != 1, k decrements to 1 and 1's right
//         child 2 is pushed onto stack = [3, 2].
//       - Second iteration pops 2; since k == 1, the function returns 2 immediately
//         without visiting 3 or 4.
//
//   Example 2:
//     Input:  root = [5, 3, 6, 2, 4, null, null, 1], k = 3
//     Output: 3
//     Explanation:
//       - The initial left-spine descent pushes [5, 3, 2, 1] onto stack.
//       - First pop visits 1 (k becomes 2, null right child), and second pop visits
//         2 (k becomes 1, null right child).
//       - Third pop visits 3 with k == 1, returning 3 immediately without
//         traversing nodes 4, 5, or 6.
//
//   Example 3:
//     Input:  root = [5], k = 5
//     Output: -1
//     Explanation:
//       - stack is seeded with the single node [5].
//       - Popping 5 checks k == 1 (false), decrements k to 4, and finds no right
//         child, leaving stack empty.
//       - The loop terminates without reaching k == 1 and returns the defensive
//         sentinel -1.
//
// Algorithm:
//   Because an inorder traversal (Left -> Node -> Right) of a BST yields values
//   in strictly ascending order, the k-th visited node is the k-th smallest
//   element. Rather than traversing the entire tree, we stop immediately once
//   the k-th node is reached:
//   1. `kthSmallest` (iterative explicit stack):
//      Push the left spine from `root`. Repeatedly pop the top node, decrement
//      the remaining count, and return immediately when it hits zero; otherwise
//      push the left spine of the popped node's right child.
//   2. `kthSmallestRecursive` (recursive inorder DFS with early exit):
//      Traverses left subtree, visits node, and short-circuits all remaining
//      recursion as soon as the k-th node is found.
//   (Follow-up note: if insertions/deletions and k-th queries are frequent,
//    augment each BST node with its subtree size for O(h) order-statistic
//    selection.)
//   - Time Complexity:  O(h + k), where h is the tree height (we descend h
//     levels to the minimum, then visit k nodes).
//   - Space Complexity: O(h) for the explicit stack or call stack.
//
// Data Structures:
//   - Explicit stack (`std::vector<const TreeNode*>`) storing the active
//     left-spine path during iterative inorder traversal.
//   - Binary tree nodes (`TreeNode`).
//
// Real-World Applications:
//   - Percentile and quantile queries (e.g., p50, p95, p99 latency) over
//     ordered metric trees or order-statistic trees.
//   - Database `ORDER BY ... LIMIT 1 OFFSET k-1` index scans on B-trees.
//   - Financial order-book depth queries (finding the k-th best bid/ask price
//     level).
//
// Related LeetCode Problems:
//   - #173 Binary Search Tree Iterator [done] (same left-spine stack mechanism)
//   - #98 Validate Binary Search Tree [done] (BST ordering invariant)
//   - #285 Inorder Successor In BST [done] (next element in inorder sequence)
//   - #671 Second Minimum Node In a Binary Tree [done] (order-statistic query on a specialized tree)
//   - #215 Kth Largest Element in an Array [done] (k-th order statistic on unsorted arrays)

#include "framework/lc_framework.h"

#include <deque>
#include <vector>

namespace {

// Binary tree node definition matching LeetCode's standard representation.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int v) : val(v) {}
  TreeNode(int v, TreeNode* l, TreeNode* r) : val(v), left(l), right(r) {}
};

// RAII arena for test trees; std::deque guarantees pointer stability on growth.
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
  // Finds the 1-indexed k-th smallest element via iterative inorder traversal.
  // Returns -1 if `root` is null or `k` is out of bounds.
  // Time: O(h + k), Space: O(h).
  int kthSmallest(const TreeNode* root, int k) {
    if (root == nullptr || k <= 0) {
      return -1;
    }

    std::vector<const TreeNode*> stack;
    const TreeNode* cur = root;

    // Seed the stack with the leftmost path so the global minimum is on top.
    while (cur != nullptr) {
      stack.push_back(cur);
      cur = cur->left;
    }

    while (!stack.empty()) {
      const TreeNode* node = stack.back();
      stack.pop_back();

      // Each pop corresponds to the next smallest element in sorted order.
      if (k == 1) {
        return node->val;
      }
      --k;

      // Stage the left spine of the right subtree, whose values immediately
      // follow `node->val` in inorder order.
      const TreeNode* next = node->right;
      while (next != nullptr) {
        stack.push_back(next);
        next = next->left;
      }
    }

    return -1;
  }

  // Recursive inorder DFS counterpart with immediate early termination once the
  // k-th element is visited.
  // Time: O(h + k), Space: O(h).
  int kthSmallestRecursive(const TreeNode* root, int k) {
    if (root == nullptr || k <= 0) {
      return -1;
    }

    int remaining = k;
    int answer = -1;
    bool found = false;

    auto dfs = [&](this auto&& self, const TreeNode* node) -> void {
      // Stop descending immediately once the target rank has been captured.
      if (node == nullptr || found) {
        return;
      }

      self(node->left);
      if (found) {
        return;
      }

      --remaining;
      if (remaining == 0) {
        answer = node->val;
        found = true;
        return;
      }

      self(node->right);
    };

    dfs(root);
    return answer;
  }
};

LC_PROBLEM(230, "Kth Smallest Element In A BST", "binary_search_tree", ctx) {
  Solution sol;

  ctx.test("example_1_small_tree", [&] {
    TreeArena arena;
    //    3
    //   / \
    //  1   4
    //   \
    //    2
    TreeNode* left = arena.make(1, nullptr, arena.make(2));
    TreeNode* root = arena.make(3, left, arena.make(4));

    for (int k = 1; k <= 4; ++k) {
      ctx.expect_eq(sol.kthSmallest(root, k), k);
      ctx.expect_eq(sol.kthSmallestRecursive(root, k), k);
    }
  });

  ctx.test("example_2_and_python_build", [&] {
    TreeArena arena;
    //         5
    //        / \
    //       3   6
    //      / \
    //     2   4
    //    /
    //   1
    TreeNode* n2 = arena.make(2, arena.make(1), nullptr);
    TreeNode* n3 = arena.make(3, n2, arena.make(4));
    TreeNode* root = arena.make(5, n3, arena.make(6));

    // Example 2: k = 3 -> 3; Python build(): k = 4 -> 4.
    ctx.expect_eq(sol.kthSmallest(root, 3), 3);
    ctx.expect_eq(sol.kthSmallestRecursive(root, 3), 3);
    ctx.expect_eq(sol.kthSmallest(root, 4), 4);
    ctx.expect_eq(sol.kthSmallestRecursive(root, 4), 4);
    ctx.expect_eq(sol.kthSmallest(root, 6), 6);
    ctx.expect_eq(sol.kthSmallestRecursive(root, 6), 6);
  });

  ctx.test("single_node_and_right_skewed", [&] {
    TreeArena arena;
    TreeNode* single = arena.make(10);
    ctx.expect_eq(sol.kthSmallest(single, 1), 10);
    ctx.expect_eq(sol.kthSmallestRecursive(single, 1), 10);

    // Right-skewed BST: 10 -> 20 -> 30
    TreeNode* skewed =
        arena.make(10, nullptr, arena.make(20, nullptr, arena.make(30)));
    ctx.expect_eq(sol.kthSmallest(skewed, 2), 20);
    ctx.expect_eq(sol.kthSmallestRecursive(skewed, 2), 20);
    ctx.expect_eq(sol.kthSmallest(skewed, 3), 30);
    ctx.expect_eq(sol.kthSmallestRecursive(skewed, 3), 30);
  });

  ctx.test("defensive_invalid_inputs", [&] {
    TreeArena arena;
    TreeNode* single = arena.make(5);
    ctx.expect_eq(sol.kthSmallest(nullptr, 1), -1);
    ctx.expect_eq(sol.kthSmallestRecursive(nullptr, 1), -1);
    ctx.expect_eq(sol.kthSmallest(single, 0), -1);
    ctx.expect_eq(sol.kthSmallestRecursive(single, 0), -1);
    ctx.expect_eq(sol.kthSmallest(single, 5), -1);
    ctx.expect_eq(sol.kthSmallestRecursive(single, 5), -1);
  });
}

}  // namespace
