// LeetCode 897: Increasing Order Search Tree
// https://leetcode.com/problems/increasing-order-search-tree/
//
// Problem:
//   Given the root of a binary search tree (BST), rearrange its existing nodes
//   in-place in ascending (in-order) sequence so that the leftmost (smallest)
//   node becomes the new root, every node has no left child (`left == nullptr`),
//   and every node has at most one right child pointing to the next larger node.
//
// Examples:
//   Example 1:
//     Input:  root = [5, 1, 7]
//     Output: [1, null, 5, null, 7]
//     Explanation:
//       - push_left_spine(5) pushes 5 and 1 onto stack = [5, 1], with tail pointing
//         to sentinel dummy.
//       - Pop 1, clear 1->left = nullptr, attach dummy.right = 1, advance tail = 1,
//         and observe 1->right is null.
//       - Pop 5, clear 5->left = nullptr, attach 1->right = 5, advance tail = 5,
//         and call push_left_spine(7) so stack = [7].
//       - Pop 7, clear 7->left = nullptr, attach 5->right = 7, set 7->right =
//         nullptr, and return dummy.right ([1, null, 5, null, 7]).
//
//   Example 2:
//     Input:  root = [3, 2, 4, 1]
//     Output: [1, null, 2, null, 3, null, 4]
//     Explanation:
//       - push_left_spine(3) seeds stack = [3, 2, 1].
//       - Pop 1 then 2, clearing each left pointer and linking dummy -> 1 -> 2
//         (neither has a right child).
//       - Pop 3, clear 3->left = nullptr, attach 2->right = 3, and push 3->right
//         (4) onto the stack.
//       - Pop 4, attach 3->right = 4, terminate 4->right = nullptr, and return the
//         chain 1 -> 2 -> 3 -> 4.
//
//   Example 3:
//     Input:  root = []
//     Output: []
//     Explanation:
//       - push_left_spine(nullptr) pushes nothing, leaving stack empty and tail =
//         &dummy.
//       - The main while loop is skipped, tail->right = nullptr sets dummy.right =
//         nullptr, and returning dummy.right yields [].
//
// Algorithm:
//   Iterative In-Order Traversal with Sentinel Tail Rewiring:
//   Use an explicit stack to visit nodes in ascending BST in-order without
//   recursion. Maintain a stack-allocated sentinel `dummy` and a `tail` pointer
//   to the end of the right-skewed chain constructed so far:
//     1. Push the entire left spine from `root` onto `stack`.
//     2. Pop the smallest unvisited node, detach its `left` pointer (preventing
//        cycles or stale links), append it as `tail->right`, and advance `tail`.
//     3. Push the left spine of `node->right` onto `stack` so the next smallest
//        elements are processed in order.
//     4. Return `dummy.right`.
//   Time Complexity: O(N) — each node is pushed and popped exactly once.
//   Space Complexity: O(H) for the explicit stack, where H is tree height
//   (O(log N) balanced, O(N) skewed).
//
// Data Structures:
//   - Explicit Stack (std::vector<TreeNode*>): drives iterative in-order
//     traversal while holding pending ancestors along the left spine.
//   - Stack-Allocated Sentinel Node (TreeNode): eliminates special-case logic
//     for initializing the new head pointer.
//
// Real-World Applications:
//   - Flattening ordered tree indexes into sequential linked lists for linear
//     range scans, export pipelines, or streaming iterators.
//   - Day-Stout-Warren (DSW) BST rebalancing algorithm's first phase ("tree to
//     vine"), which converts an arbitrary BST into a right-leaning backbone
//     before performing rotations to achieve optimal balance.
//   - Converting hierarchical AST/expression lists into canonical linear chains.
//
// Related LeetCode Problems:
//   - #114 Flatten Binary Tree to Linked List [done] (rewires a binary tree into a right-skewed preorder chain)
//   - #426 Convert Binary Search Tree to Sorted Doubly Linked List [done] (in-order rewiring into a circular doubly linked list)
//   - #173 Binary Search Tree Iterator [done] (uses the exact same left-spine stack for lazy in-order traversal)
//   - #94 Binary Tree Inorder Traversal [done] (fundamental iterative and recursive in-order traversal)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <optional>
#include <queue>
#include <vector>

namespace {

// Standard binary tree node for LeetCode tree problems.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int x) : val(x) {}
};

class Solution {
 public:
  // Rewires `root` in-place into a right-skewed tree sorted in ascending order,
  // clearing every `left` pointer.
  // Time: O(N), Space: O(H) where H is the height of the BST.
  TreeNode* increasingBST(TreeNode* root) {
    std::vector<TreeNode*> stack;

    // Helper pushes a subtree's entire left spine so the smallest node sits on
    // top of the stack.
    auto push_left_spine = [&stack](TreeNode* curr) {
      while (curr != nullptr) {
        stack.push_back(curr);
        curr = curr->left;
      }
    };

    push_left_spine(root);

    // Stack-allocated sentinel avoids heap allocation and simplifies head
    // attachment when the first (smallest) node is popped.
    TreeNode dummy(0);
    TreeNode* tail = &dummy;

    while (!stack.empty()) {
      TreeNode* node = stack.back();
      stack.pop_back();

      // Capture original right subtree before rewiring `tail->right` and
      // detaching `node->left` so no old left pointers or cycles survive.
      TreeNode* next_right = node->right;
      node->left = nullptr;
      tail->right = node;
      tail = node;

      // Queue the left spine of the right subtree for subsequent in-order
      // visits.
      push_left_spine(next_right);
    }

    // Explicitly terminate the chain tail.
    tail->right = nullptr;
    return dummy.right;
  }
};

// RAII arena owning test TreeNode instances to guarantee zero memory leaks.
class TreeArena {
 public:
  // Builds a binary tree from LeetCode's level-order vector (std::nullopt =
  // missing child).
  TreeNode* build(const std::vector<std::optional<int>>& level_order) {
    if (level_order.empty() || !level_order[0].has_value()) {
      return nullptr;
    }

    TreeNode* root = create(*level_order[0]);
    std::queue<TreeNode*> pending;
    pending.push(root);

    std::size_t idx = 1uz;
    while (!pending.empty() && idx < level_order.size()) {
      TreeNode* parent = pending.front();
      pending.pop();

      if (level_order[idx].has_value()) {
        parent->left = create(*level_order[idx]);
        pending.push(parent->left);
      }
      ++idx;

      if (idx < level_order.size() && level_order[idx].has_value()) {
        parent->right = create(*level_order[idx]);
        pending.push(parent->right);
      }
      ++idx;
    }
    return root;
  }

 private:
  TreeNode* create(int val) {
    nodes_.emplace_back(val);
    return &nodes_.back();
  }

  std::deque<TreeNode> nodes_;
};

// Walks a right-skewed chain, asserting that every node has `left == nullptr`,
// and returns the collected values.
std::vector<int> extract_right_chain(const TreeNode* head,
                                     lc::TestContext& ctx) {
  std::vector<int> values;
  const TreeNode* curr = head;
  while (curr != nullptr) {
    ctx.expect_true(curr->left == nullptr,
                    "every node in increasing BST must have left == nullptr");
    values.push_back(curr->val);
    curr = curr->right;
  }
  return values;
}

LC_PROBLEM(897, "Increasing Order Search Tree", "binary_search_tree", ctx) {
  Solution sol;

  ctx.test("example_1_full_bst", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({5, 3, 6, 2, 4, std::nullopt, 8, 1,
                                  std::nullopt, std::nullopt, std::nullopt, 7,
                                  9});
    TreeNode* result = sol.increasingBST(root);
    ctx.expect_eq(extract_right_chain(result, ctx),
                  std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9});
  });

  ctx.test("example_2_three_nodes", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({5, 1, 7});
    TreeNode* result = sol.increasingBST(root);
    ctx.expect_eq(extract_right_chain(result, ctx), std::vector<int>{1, 5, 7});
  });

  ctx.test("python_build_four_nodes", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({3, 2, 4, 1});
    TreeNode* result = sol.increasingBST(root);
    ctx.expect_eq(extract_right_chain(result, ctx),
                  std::vector<int>{1, 2, 3, 4});
  });

  ctx.test("left_skewed_and_right_skewed_trees", [&] {
    TreeArena arena;
    TreeNode* left_skewed =
        arena.build({4, 3, std::nullopt, 2, std::nullopt, 1});
    ctx.expect_eq(extract_right_chain(sol.increasingBST(left_skewed), ctx),
                  std::vector<int>{1, 2, 3, 4});

    TreeNode* right_skewed =
        arena.build({1, std::nullopt, 2, std::nullopt, 3});
    ctx.expect_eq(extract_right_chain(sol.increasingBST(right_skewed), ctx),
                  std::vector<int>{1, 2, 3});
  });

  ctx.test("single_node_and_empty_tree", [&] {
    TreeArena arena;
    TreeNode* single = arena.build({42});
    ctx.expect_eq(extract_right_chain(sol.increasingBST(single), ctx),
                  std::vector<int>{42});

    ctx.expect_true(sol.increasingBST(nullptr) == nullptr);
  });
}

}  // namespace
