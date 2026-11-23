// LeetCode 662: Maximum Width Of Binary Tree
// https://leetcode.com/problems/maximum-width-of-binary-tree/
//
// Problem:
//   Given the root of a binary tree, return the maximum width among all levels.
//   The width of a single level is defined as the distance between its leftmost
//   and rightmost non-null nodes (inclusive), counting any null gaps between
//   them as if the tree were embedded in a full binary tree. Tree size is in
//   [1, 3000] and the answer is guaranteed to fit in a 32-bit signed integer.
//
// Algorithm:
//   Perform level-order BFS where each node carries its 0-based complete binary
//   tree position index `idx`: its left child sits at `2 * idx` and its right
//   child sits at `2 * idx + 1`.
//   - On every level, the width is `(level.back().first - level.front().first) + 1`.
//   - Overflow prevention: raw indices double on every level and would overflow
//     64-bit integers past depth 63 on deep skewed trees (unlike Python's
//     arbitrary-precision ints). Subtract `base = level.front().first` from
//     every index on the current level before computing children's indices:
//     `norm = idx - base`, left = `norm * 2`, right = `norm * 2 + 1`. Since the
//     true width fits in 32-bit signed `int`, `norm` stays < 2^31 and child
//     indices never overflow `std::uint64_t`.
//   Time: O(N), visiting every non-null node exactly once.
//   Space: O(W), where W <= N is the maximum number of non-null nodes on any
//   single level.
//
// Data Structures:
//   - `std::vector<std::pair<std::uint64_t, const TreeNode*>>` (double-buffered
//     BFS level frontier): provides O(1) access to `front()` (leftmost node)
//     and `back()` (rightmost node) on each level.
//
// Real-World Applications:
//   - Bounding horizontal canvas width in layered tree/DAG visualization and
//     graph-drawing engines (e.g., org charts, AST/flamegraph renderers).
//   - Estimating worst-case array storage overhead when packing sparse binary
//     trees (like segment trees or heaps) into flat Eytzinger / heap arrays.
//   - Analyzing peak fan-out span across time slots in hierarchical scheduling
//     or broadcast routing trees.
//
// Related LeetCode Problems:
//   - #102 Binary Tree Level Order Traversal [done] (foundational level-by-level BFS)
//   - #104 Maximum Depth of Binary Tree [done] (vertical dimension vs horizontal width)
//   - #655 Print Binary Tree [done] (full-binary-tree horizontal coordinate layout)
//   - #958 Check Completeness of a Binary Tree [done] (complete binary tree index invariants)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <optional>
#include <queue>
#include <utility>
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

class Solution {
 public:
  // Computes the maximum horizontal width across all levels using level-order
  // BFS with per-level index normalization to prevent 64-bit overflow.
  // Time: O(N), Space: O(W) where W is the maximum number of nodes on a level.
  int widthOfBinaryTree(const TreeNode* root) {
    if (root == nullptr) {
      return 0;
    }

    // Each entry stores `(normalized_position_index, node_pointer)` in
    // strictly increasing left-to-right positional order.
    std::vector<std::pair<std::uint64_t, const TreeNode*>> current_level = {
        {0ULL, root}};
    std::uint64_t max_width = 0ULL;

    while (!current_level.empty()) {
      const std::uint64_t base = current_level.front().first;
      const std::uint64_t last = current_level.back().first;
      max_width = std::max(max_width, (last - base) + 1ULL);

      std::vector<std::pair<std::uint64_t, const TreeNode*>> next_level;
      next_level.reserve(current_level.size() * 2uz);

      for (const auto& [idx, node] : current_level) {
        // Re-base indices at 0 on every level so deep skewed trees (depth > 64)
        // never overflow 64-bit arithmetic.
        const std::uint64_t norm = idx - base;
        if (node->left != nullptr) {
          next_level.emplace_back(norm * 2ULL, node->left);
        }
        if (node->right != nullptr) {
          next_level.emplace_back((norm * 2ULL) + 1ULL, node->right);
        }
      }

      current_level = std::move(next_level);
    }

    return static_cast<int>(max_width);
  }
};

// RAII arena that owns all test tree nodes with zero manual `delete`.
class TreeArena {
 public:
  TreeNode* create(int val) {
    nodes_.emplace_back(val);
    return &nodes_.back();
  }

  TreeNode* build(const std::vector<std::optional<int>>& vals) {
    if (vals.empty() || !vals[0].has_value()) {
      return nullptr;
    }

    TreeNode* root = create(*vals[0]);
    std::queue<TreeNode*> q;
    q.push(root);

    std::size_t idx = 1;
    while (!q.empty() && idx < vals.size()) {
      TreeNode* curr = q.front();
      q.pop();

      if (vals[idx].has_value()) {
        curr->left = create(*vals[idx]);
        q.push(curr->left);
      }
      ++idx;

      if (idx < vals.size() && vals[idx].has_value()) {
        curr->right = create(*vals[idx]);
        q.push(curr->right);
      }
      ++idx;
    }
    return root;
  }

 private:
  std::deque<TreeNode> nodes_;
};

LC_PROBLEM(662, "Maximum Width Of Binary Tree", "tree", ctx) {
  Solution sol;

  ctx.test("example_1_width_4", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({1, 3, 2, 5, 3, std::nullopt, 9});
    ctx.expect_eq(sol.widthOfBinaryTree(root), 4);
  });

  ctx.test("example_2_left_only_width_2", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({1, 3, std::nullopt, 5, 3});
    ctx.expect_eq(sol.widthOfBinaryTree(root), 2);
  });

  ctx.test("example_3_width_2", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({1, 3, 2, 5});
    ctx.expect_eq(sol.widthOfBinaryTree(root), 2);
  });

  ctx.test("example_4_outer_edges_width_8", [&] {
    // Matches the original Python `build()` test case.
    TreeArena arena;
    TreeNode* root = arena.build(
        {1, 3, 2, 5, std::nullopt, std::nullopt, 9, 6, std::nullopt, std::nullopt, 7});
    ctx.expect_eq(sol.widthOfBinaryTree(root), 8);
  });

  ctx.test("single_node_and_empty", [&] {
    TreeArena arena;
    ctx.expect_eq(sol.widthOfBinaryTree(arena.build({1})), 1);
    ctx.expect_eq(sol.widthOfBinaryTree(nullptr), 0);
  });

  ctx.test("deep_skewed_tree_overflow_safety", [&] {
    // Depth-100 right-skewed chain ending in a 2-child fork: without per-level
    // normalization, 2^100 would overflow uint64_t.
    TreeArena arena;
    TreeNode* root = arena.create(0);
    TreeNode* curr = root;
    for (int i = 1; i <= 100; ++i) {
      curr->right = arena.create(i);
      curr = curr->right;
    }
    curr->left = arena.create(101);
    curr->right = arena.create(102);

    ctx.expect_eq(sol.widthOfBinaryTree(root), 2);
  });
}

}  // namespace
