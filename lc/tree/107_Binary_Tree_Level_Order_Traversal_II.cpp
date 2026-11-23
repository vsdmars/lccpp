// LeetCode 107: Binary Tree Level Order Traversal II
// https://leetcode.com/problems/binary-tree-level-order-traversal-ii/
//
// Problem:
//   Given the root of a binary tree, return the bottom-up level order
//   traversal of its nodes' values (i.e., left to right within each level,
//   ordered from the deepest leaf level up to the root). An empty tree
//   returns an empty 2D vector.
//
// Algorithm:
//   1. Iterative BFS + Reverse (`levelOrderBottom`): Perform standard
//      top-down BFS level by level using a FIFO queue, collecting each depth's
//      values into `result`, then reverse the outer vector in place via
//      `std::ranges::reverse(result)` (O(H) vector-header swaps).
//   2. Recursive Level-Unwind (`levelOrderBottomRecursive`): Collect the next
//      frontier vector from the current level, recurse on the next frontier
//      first, and append the current level's values as the call stack unwinds,
//      yielding bottom-up order without an explicit reversal.
//   - Time Complexity: O(N), where N is the number of nodes.
//   - Space Complexity: O(N) for the BFS frontier / level buffers and output.
//
// Data Structures:
//   - `std::queue<const TreeNode*>` for iterative level-by-level BFS.
//   - `std::vector<std::vector<int>>` for storing per-level values and
//     reversing level order in O(H) move-swaps.
//
// Real-World Applications:
//   - Bottom-up build and teardown orchestration (e.g., shutting down leaf
//     microservices or deleting nested resources before their parent
//     containers).
//   - Hierarchical aggregation / roll-up queries in analytics engines where
//     leaf metrics must be rolled up into parent summaries layer by layer.
//   - Bottom-up garbage collection and dependency cleanup passes in tree
//     structured object graphs.
//
// Related LeetCode Problems:
//   - #102 Binary Tree Level Order Traversal [done] (top-down variant without the final level reversal)
//   - #103 Binary Tree Zigzag Level Order Traversal [done] (alternates direction within each level)
//   - #637 Average of Levels in Binary Tree [done] (computes per-level averages using the same BFS skeleton)
//   - #199 Binary Tree Right Side View [done] (selects one boundary node per BFS level)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <deque>
#include <optional>
#include <queue>
#include <utility>
#include <vector>

namespace {

// Standard LeetCode binary tree node representation.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int v) : val(v) {}
  TreeNode(int v, TreeNode* l, TreeNode* r) : val(v), left(l), right(r) {}
};

class Solution {
 public:
  // Returns bottom-up level order traversal using iterative BFS followed by an
  // in-place reversal of the outer levels vector.
  // Time: O(N), Space: O(N).
  std::vector<std::vector<int>> levelOrderBottom(const TreeNode* root) {
    std::vector<std::vector<int>> result;
    if (root == nullptr) {
      return result;
    }

    std::queue<const TreeNode*> frontier;
    frontier.push(root);

    while (!frontier.empty()) {
      const std::size_t level_size = frontier.size();
      std::vector<int> level;
      level.reserve(level_size);

      for (std::size_t i = 0uz; i < level_size; ++i) {
        const TreeNode* node = frontier.front();
        frontier.pop();
        level.push_back(node->val);

        if (node->left != nullptr) {
          frontier.push(node->left);
        }
        if (node->right != nullptr) {
          frontier.push(node->right);
        }
      }

      result.push_back(std::move(level));
    }

    // Reversing the outer vector swaps vector headers in O(H) without copying
    // individual integer elements.
    std::ranges::reverse(result);
    return result;
  }

  // Recursive level-unwind variant (mirroring the primary Python solution):
  // recurses down to the deepest frontier first and appends each level on
  // unwind so levels naturally appear from bottom to top.
  // Time: O(N), Space: O(N).
  std::vector<std::vector<int>> levelOrderBottomRecursive(
      const TreeNode* root) {
    std::vector<std::vector<int>> result;
    if (root == nullptr) {
      return result;
    }

    auto dfs = [&](this auto&& self,
                   const std::vector<const TreeNode*>& nodes) -> void {
      if (nodes.empty()) {
        return;
      }

      std::vector<const TreeNode*> next_level;
      next_level.reserve(nodes.size() * 2uz);
      for (const TreeNode* node : nodes) {
        if (node->left != nullptr) {
          next_level.push_back(node->left);
        }
        if (node->right != nullptr) {
          next_level.push_back(node->right);
        }
      }

      // Recurse into deeper levels before emitting the current level.
      self(next_level);

      std::vector<int> values;
      values.reserve(nodes.size());
      for (const TreeNode* node : nodes) {
        values.push_back(node->val);
      }
      result.push_back(std::move(values));
    };

    dfs({root});
    return result;
  }
};

// RAII node arena backed by std::deque for leak-free test tree construction.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    nodes_.emplace_back(val, left, right);
    return &nodes_.back();
  }

  TreeNode* from_level_order(const std::vector<std::optional<int>>& values) {
    if (values.empty() || !values[0].has_value()) {
      return nullptr;
    }
    TreeNode* root = make(*values[0]);
    std::queue<TreeNode*> pending;
    pending.push(root);
    std::size_t idx = 1uz;

    while (!pending.empty() && idx < values.size()) {
      TreeNode* curr = pending.front();
      pending.pop();

      if (idx < values.size() && values[idx].has_value()) {
        curr->left = make(*values[idx]);
        pending.push(curr->left);
      }
      ++idx;

      if (idx < values.size() && values[idx].has_value()) {
        curr->right = make(*values[idx]);
        pending.push(curr->right);
      }
      ++idx;
    }
    return root;
  }

 private:
  std::deque<TreeNode> nodes_;
};

LC_PROBLEM(107, "Binary Tree Level Order Traversal II", "tree", ctx) {
  Solution sol;

  ctx.test("example_1_and_python_build", [&] {
    TreeArena arena;
    //     3
    //    / \
    //   9  20
    //     /  \
    //    15   7
    TreeNode* root =
        arena.from_level_order({3, 9, 20, std::nullopt, std::nullopt, 15, 7});
    const std::vector<std::vector<int>> expected = {{15, 7}, {9, 20}, {3}};
    ctx.expect_eq(sol.levelOrderBottom(root), expected);
    ctx.expect_eq(sol.levelOrderBottomRecursive(root), expected);
  });

  ctx.test("empty_tree", [&] {
    const std::vector<std::vector<int>> expected = {};
    ctx.expect_eq(sol.levelOrderBottom(nullptr), expected);
    ctx.expect_eq(sol.levelOrderBottomRecursive(nullptr), expected);
  });

  ctx.test("single_node", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(1);
    const std::vector<std::vector<int>> expected = {{1}};
    ctx.expect_eq(sol.levelOrderBottom(root), expected);
    ctx.expect_eq(sol.levelOrderBottomRecursive(root), expected);
  });

  ctx.test("sparse_unbalanced_tree", [&] {
    TreeArena arena;
    //       1
    //      / \
    //     2   3
    //    /     \
    //   4       5
    TreeNode* root =
        arena.from_level_order({1, 2, 3, 4, std::nullopt, std::nullopt, 5});
    const std::vector<std::vector<int>> expected = {{4, 5}, {2, 3}, {1}};
    ctx.expect_eq(sol.levelOrderBottom(root), expected);
    ctx.expect_eq(sol.levelOrderBottomRecursive(root), expected);
  });

  ctx.test("right_skewed_chain", [&] {
    TreeArena arena;
    TreeNode* root =
        arena.make(1, nullptr, arena.make(2, nullptr, arena.make(3)));
    const std::vector<std::vector<int>> expected = {{3}, {2}, {1}};
    ctx.expect_eq(sol.levelOrderBottom(root), expected);
    ctx.expect_eq(sol.levelOrderBottomRecursive(root), expected);
  });
}

}  // namespace
