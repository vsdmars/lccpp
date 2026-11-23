// LeetCode 226: Invert Binary Tree
// https://leetcode.com/problems/invert-binary-tree/
//
// Problem:
//   Given the root of a binary tree, invert (mirror) the tree by swapping the
//   left and right subtrees of every node, and return the root. The tree has
//   between 0 and 100 nodes with values in [-100, 100].
//
// Examples:
//   Example 1:
//     Input:  root = [4, 2, 7, 1, 3, 6, 9]
//     Output: [4, 7, 2, 9, 6, 3, 1]
//     Explanation:
//       - invertTree pushes root node 4 onto FIFO queue q.
//       - Popping 4 swaps its left and right pointers so 4->left = 7 and 4->right =
//         2, then enqueues 7 and 2.
//       - Popping 7 swaps its children to 9 (left) and 6 (right) and enqueues them,
//         and popping 2 swaps its children to 3 (left) and 1 (right) and enqueues
//         them.
//       - Popping leaf nodes 9, 6, 3, and 1 swaps their nullptr children with no
//         new nodes enqueued, leaving the level-order tree [4, 7, 2, 9, 6, 3, 1].
//
//   Example 2:
//     Input:  root = [1, 2, 3, null, 4, null, 5]
//     Output: [1, 3, 2, 5, null, 4]
//     Explanation:
//       - invertTree pushes root node 1 onto q and pops it, swapping left child 2
//         and right child 3 so 1->left = 3 and 1->right = 2 before enqueuing 3 and
//         2.
//       - Popping 3 swaps its left (nullptr) and right (5) pointers so 3->left = 5
//         and 3->right = nullptr, then enqueues 5.
//       - Popping 2 swaps its left (nullptr) and right (4) pointers so 2->left = 4
//         and 2->right = nullptr, then enqueues 4.
//       - Popping leaves 5 and 4 finishes the queue, producing the serialized
//         level-order result [1, 3, 2, 5, null, 4].
//
//   Example 3:
//     Input:  root = []
//     Output: []
//     Explanation:
//       - invertTree checks if (root == nullptr) upon entry.
//       - Because root is nullptr, it immediately returns nullptr (empty tree [])
//         without allocating or using a queue.
//
// Algorithm:
//   We provide two linear-time traversal approaches:
//   1. Iterative BFS (`invertTree`): Push `root` onto a FIFO queue. While the
//      queue is non-empty, pop `curr`, swap `curr->left` and `curr->right` in
//      place, and enqueue whichever children are non-null. Swapping every node
//      exactly once produces the exact mirror image without recursion stack
//      risk.
//   2. Recursive DFS (`invertTreeDfs`): Base case returns `nullptr`. Otherwise
//      recursively invert both subtrees and assign the inverted right subtree
//      to `left` and the inverted left subtree to `right`.
//   Time Complexity:  O(N), where N is the number of nodes in the tree.
//   Space Complexity: O(W) <= O(N) for BFS queue (maximum tree width), or
//                     O(H) <= O(N) for DFS call stack (tree height).
//
// Data Structures:
//   - `std::queue<TreeNode*>`: Drives iterative level-order node visitation and
//     in-place child pointer swapping.
//   - `TreeNode`: Binary tree node with `left` and `right` child pointers.
//
// Real-World Applications:
//   - Right-to-left (RTL) internationalization mirroring of hierarchical UI
//     layout trees and scene graphs.
//   - Flipping binary space partitioning (BSP) or bounding-volume hierarchy
//     (BVH) trees across a symmetry plane in computer graphics.
//   - Reversing comparator order in binary expression or tournament trees.
//
// Related LeetCode Problems:
//   - #101 Symmetric Tree [done] (checks if a tree equals its own inversion)
//   - #100 Same Tree [done] (structural equality check on binary trees)
//   - #2415 Reverse Odd Levels of Binary Tree [new] (partial BFS tree flip)
//   - #951 Flip Equivalent Binary Trees [done] (selective subtree inversions)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <optional>
#include <queue>
#include <string>
#include <utility>
#include <vector>

namespace {

// Standard LeetCode binary tree node.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int v, TreeNode* l = nullptr, TreeNode* r = nullptr)
      : val(v), left(l), right(r) {}
};

// Builds a binary tree from LeetCode level-order representation inside an RAII
// `std::deque` arena (pointer-stable, zero manual new/delete).
TreeNode* build_tree(std::deque<TreeNode>& arena,
                     const std::vector<std::optional<int>>& level_order) {
  if (level_order.empty() || !level_order[0].has_value()) {
    return nullptr;
  }

  arena.emplace_back(*level_order[0]);
  TreeNode* root = &arena.back();
  std::queue<TreeNode*> q;
  q.push(root);

  std::size_t idx = 1uz;
  while (!q.empty() && idx < level_order.size()) {
    TreeNode* curr = q.front();
    q.pop();

    if (level_order[idx].has_value()) {
      arena.emplace_back(*level_order[idx]);
      curr->left = &arena.back();
      q.push(curr->left);
    }
    ++idx;

    if (idx < level_order.size() && level_order[idx].has_value()) {
      arena.emplace_back(*level_order[idx]);
      curr->right = &arena.back();
      q.push(curr->right);
    }
    ++idx;
  }
  return root;
}

// Serializes a binary tree into trimmed LeetCode level-order tokens so test
// assertions produce readable diffs on mismatch.
std::vector<std::string> serialize_tree(const TreeNode* root) {
  if (root == nullptr) {
    return {};
  }

  std::vector<std::string> tokens;
  std::queue<const TreeNode*> q;
  q.push(root);

  while (!q.empty()) {
    const TreeNode* curr = q.front();
    q.pop();
    if (curr == nullptr) {
      tokens.emplace_back("null");
      continue;
    }
    tokens.push_back(std::to_string(curr->val));
    q.push(curr->left);
    q.push(curr->right);
  }

  // Strip trailing "null" sentinels matching canonical LeetCode formatting.
  while (!tokens.empty() && tokens.back() == "null") {
    tokens.pop_back();
  }
  return tokens;
}

class Solution {
 public:
  // Inverts the binary tree rooted at `root` in place using iterative BFS.
  // Invariant: Every node popped from `q` has its immediate children swapped
  // exactly once; all descendants are subsequently queued and inverted.
  // Time: O(N), Space: O(N).
  TreeNode* invertTree(TreeNode* root) {
    if (root == nullptr) {
      return nullptr;
    }

    std::queue<TreeNode*> q;
    q.push(root);

    while (!q.empty()) {
      TreeNode* curr = q.front();
      q.pop();

      std::swap(curr->left, curr->right);

      if (curr->left != nullptr) {
        q.push(curr->left);
      }
      if (curr->right != nullptr) {
        q.push(curr->right);
      }
    }

    return root;
  }

  // Recursive DFS alternative matching the Python `dfs()` method.
  // Time: O(N), Space: O(H).
  TreeNode* invertTreeDfs(TreeNode* root) {
    auto dfs = [](this auto&& self, TreeNode* node) -> TreeNode* {
      if (node == nullptr) {
        return nullptr;
      }
      TreeNode* new_left = self(node->right);
      TreeNode* new_right = self(node->left);
      node->left = new_left;
      node->right = new_right;
      return node;
    };
    return dfs(root);
  }
};

LC_PROBLEM(226, "Invert Binary Tree", "bfs", ctx) {
  Solution sol;

  ctx.test("example_1_full_tree", [&] {
    std::deque<TreeNode> arena_bfs;
    TreeNode* root_bfs = build_tree(arena_bfs, {4, 2, 7, 1, 3, 6, 9});
    const std::vector<std::string> expected = {"4", "7", "2", "9",
                                               "6", "3", "1"};
    ctx.expect_eq(serialize_tree(sol.invertTree(root_bfs)), expected);

    std::deque<TreeNode> arena_dfs;
    TreeNode* root_dfs = build_tree(arena_dfs, {4, 2, 7, 1, 3, 6, 9});
    ctx.expect_eq(serialize_tree(sol.invertTreeDfs(root_dfs)), expected);
  });

  ctx.test("example_2_three_nodes", [&] {
    std::deque<TreeNode> arena;
    TreeNode* root = build_tree(arena, {2, 1, 3});
    const std::vector<std::string> expected = {"2", "3", "1"};
    ctx.expect_eq(serialize_tree(sol.invertTree(root)), expected);
  });

  ctx.test("example_3_empty_tree", [&] {
    ctx.expect_true(sol.invertTree(nullptr) == nullptr);
    ctx.expect_true(sol.invertTreeDfs(nullptr) == nullptr);
  });

  ctx.test("asymmetric_tree_from_python_build", [&] {
    // Original: 1 -> (2 -> (null, 4), 3 -> (null, 5))
    // Inverted: 1 -> (3 -> (5, null), 2 -> (4, null))
    std::deque<TreeNode> arena_bfs;
    TreeNode* root_bfs =
        build_tree(arena_bfs, {1, 2, 3, std::nullopt, 4, std::nullopt, 5});
    const std::vector<std::string> expected = {"1", "3", "2", "5", "null", "4"};
    ctx.expect_eq(serialize_tree(sol.invertTree(root_bfs)), expected);

    std::deque<TreeNode> arena_dfs;
    TreeNode* root_dfs =
        build_tree(arena_dfs, {1, 2, 3, std::nullopt, 4, std::nullopt, 5});
    ctx.expect_eq(serialize_tree(sol.invertTreeDfs(root_dfs)), expected);
  });

  ctx.test("single_node_and_involution", [&] {
    std::deque<TreeNode> arena;
    TreeNode* single = build_tree(arena, {7});
    ctx.expect_eq(serialize_tree(sol.invertTree(single)),
                  std::vector<std::string>{"7"});

    // Inverting twice restores the original left-skewed chain.
    TreeNode* chain = build_tree(arena, {1, 2, std::nullopt, 3});
    const auto original = serialize_tree(chain);
    sol.invertTree(chain);
    ctx.expect_eq(serialize_tree(chain),
                  std::vector<std::string>{"1", "null", "2", "null", "3"});
    sol.invertTreeDfs(chain);
    ctx.expect_eq(serialize_tree(chain), original);
  });
}

}  // namespace
