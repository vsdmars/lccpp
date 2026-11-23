// LeetCode 501: Find Mode In Binary Search Tree
// https://leetcode.com/problems/find-mode-in-binary-search-tree/
//
// Problem:
//   Given the root of a Binary Search Tree (BST) that may contain duplicate
//   values (left subtree keys <= node->val <= right subtree keys), return all
//   modes — the value(s) that appear with the highest frequency in the tree.
//   If multiple values tie for the maximum frequency, return all of them in
//   any order.
//
// Algorithm:
//   An in-order traversal (left -> node -> right) of a BST visits all node
//   values in non-decreasing sorted order, which groups every occurrence of
//   the same key into a contiguous run. Rather than building an O(n) frequency
//   hash map, perform a single in-order DFS while tracking only four O(1)
//   running variables: the previous value (`prev_val`), its current run length
//   (`current_count`), the highest run length seen so far (`max_count`), and
//   the candidate `modes` list:
//   - If the current node matches `prev_val`, increment `current_count`;
//     otherwise reset `current_count = 1` and update `prev_val`.
//   - If `current_count > max_count`, update `max_count`, clear `modes`, and
//     record the new mode.
//   - If `current_count == max_count`, append the value to `modes`.
//   Time Complexity: O(n), visiting each node once.
//   Space Complexity: O(h) for the recursion stack (O(1) auxiliary heap space
//   beyond the returned `modes` vector).
//
// Data Structures:
//   - Binary Search Tree (`TreeNode` linked structure).
//   - Scalar run-length trackers (`prev_val`, `current_count`, `max_count`)
//     and output `std::vector<int> modes`.
//
// Real-World Applications:
//   - Streaming run-length frequency aggregation over sorted database index
//     scans without allocating hash tables.
//   - Telemetry and log analytics: identifying the most frequent status codes
//     or error buckets during an in-order B-tree index sweep.
//   - Histogram peak detection on ordered key-value stores.
//
// Related LeetCode Problems:
//   - #98 Validate Binary Search Tree [done] (in-order monotonicity of BSTs)
//   - #530 Minimum Absolute Difference in BST [done] (tracking `prev_val` in BST in-order)
//   - #230 Kth Smallest Element in a BST [done] (in-order BST streaming)
//   - #347 Top K Frequent Elements [done] (general frequency mode finding without BST order)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <deque>
#include <optional>
#include <queue>
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

// RAII arena that owns all test TreeNodes so pointer graphs never leak memory.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    nodes_.emplace_back(val, left, right);
    return &nodes_.back();
  }

  // Constructs a binary tree from LeetCode's level-order array representation.
  TreeNode* build(const std::vector<std::optional<int>>& level_order) {
    if (level_order.empty() || !level_order[0].has_value()) {
      return nullptr;
    }
    TreeNode* root = make(*level_order[0]);
    std::queue<TreeNode*> q;
    q.push(root);
    std::size_t idx = 1uz;
    while (!q.empty() && idx < level_order.size()) {
      TreeNode* cur = q.front();
      q.pop();
      if (level_order[idx].has_value()) {
        cur->left = make(*level_order[idx]);
        q.push(cur->left);
      }
      ++idx;
      if (idx < level_order.size() && level_order[idx].has_value()) {
        cur->right = make(*level_order[idx]);
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
  // Finds all mode values in a BST (allowing duplicates) via a single in-order
  // pass with O(1) extra state beyond the recursion stack and output vector.
  // Time: O(n), Space: O(h).
  std::vector<int> findMode(const TreeNode* root) {
    std::vector<int> modes;
    std::optional<int> prev_val;
    int current_count = 0;
    int max_count = 0;

    auto inorder = [&](this auto&& self, const TreeNode* node) -> void {
      if (node == nullptr) {
        return;
      }

      // Visit left subtree first to maintain non-decreasing key order.
      self(node->left);

      // Update the contiguous run count for `node->val`.
      if (prev_val.has_value() && *prev_val == node->val) {
        ++current_count;
      } else {
        prev_val = node->val;
        current_count = 1;
      }

      // Maintain the set of values achieving the highest frequency so far.
      if (current_count > max_count) {
        max_count = current_count;
        modes.clear();
        modes.push_back(node->val);
      } else if (current_count == max_count) {
        modes.push_back(node->val);
      }

      self(node->right);
    };

    inorder(root);
    return modes;
  }
};

LC_PROBLEM(501, "Find Mode In Binary Search Tree", "binary_search_tree", ctx) {
  Solution sol;

  ctx.test("leetcode_example_1", [&] {
    TreeArena arena;
    //   1
    //    \
    //     2
    //    /
    //   2
    const TreeNode* root = arena.build({1, std::nullopt, 2, 2});
    ctx.expect_eq(sol.findMode(root), std::vector<int>{2});
  });

  ctx.test("single_node_and_empty", [&] {
    TreeArena arena;
    ctx.expect_eq(sol.findMode(nullptr), std::vector<int>{});
    ctx.expect_eq(sol.findMode(arena.make(0)), std::vector<int>{0});
  });

  ctx.test("python_build_multiple_modes_and_deep_duplicates", [&] {
    TreeArena arena;
    // BST with in-order [1, 1, 2, 2, 2] -> mode is [2].
    TreeNode* n2_leaf = arena.make(2);
    TreeNode* n1_inner = arena.make(1, nullptr, n2_leaf);
    TreeNode* n2_mid = arena.make(2, n1_inner, nullptr);
    TreeNode* n1_left = arena.make(1, nullptr, n2_mid);
    const TreeNode* tree1 = arena.make(2, n1_left, nullptr);
    ctx.expect_eq(sol.findMode(tree1), std::vector<int>{2});

    // BST with in-order [0, 2, 2, 4, 6, 6, 7, 8, 9] -> tied modes [2, 6].
    const TreeNode* tree2 =
        arena.build({6, 2, 8, 0, 4, 7, 9, std::nullopt, std::nullopt, 2, 6});
    std::vector<int> modes = sol.findMode(tree2);
    std::ranges::sort(modes);
    ctx.expect_eq(modes, std::vector<int>{2, 6});
  });

  ctx.test("all_unique_values_all_are_modes", [&] {
    TreeArena arena;
    const TreeNode* root = arena.build({1, std::nullopt, 2});
    ctx.expect_eq(sol.findMode(root), std::vector<int>{1, 2});
  });

  ctx.test("negative_duplicate_values", [&] {
    TreeArena arena;
    const TreeNode* root = arena.build({-3, -5, -2, -5, std::nullopt, -2});
    ctx.expect_eq(sol.findMode(root), std::vector<int>{-5, -2});
  });
}

}  // namespace
