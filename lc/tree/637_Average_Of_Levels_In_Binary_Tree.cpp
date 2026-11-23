// LeetCode 637: Average Of Levels In Binary Tree
// https://leetcode.com/problems/average-of-levels-in-binary-tree/
//
// Problem:
//   Given the root of a binary tree, return the average value of the nodes on
//   each level in the form of an array of floating-point numbers (level 0 is
//   the root). Answers within 10^-5 of the actual answer are accepted. Node
//   count is in [1, 10^4] and node values span the full 32-bit signed range
//   [-2^31, 2^31 - 1].
//
// Algorithm:
//   Perform a standard breadth-first search (level-order traversal). Process
//   all nodes belonging to the current level in one inner loop:
//   - Accumulate node values into a 64-bit signed integer (`std::int64_t`) so
//     summing up to 10^4 `INT_MAX` values cannot overflow 32-bit `int` or lose
//     precision while summing.
//   - Append `static_cast<double>(level_sum) / static_cast<double>(level_size)`
//     to the result vector.
//   - Enqueue every non-null child for the next level.
//   Time: O(N), visiting each node exactly once.
//   Space: O(W), where W <= N is the maximum width of any level in the tree.
//
// Data Structures:
//   - `std::queue<const TreeNode*>`: FIFO frontier that naturally separates
//     tree levels via the snapshot `level_size = q.size()`.
//   - `std::vector<double>`: stores the per-level averages in top-down order.
//
// Real-World Applications:
//   - Computing per-tier latency, memory, or cost averages across a tree of
//     distributed microservices or compute cluster hierarchies.
//   - Analyzing mean fan-out or occupancy metrics at each depth of B-trees,
//     tries, or spatial partitioning trees in storage engines.
//   - Summarizing hierarchical organizational telemetry (e.g., average salary
//     or headcount per management layer).
//
// Related LeetCode Problems:
//   - #102 Binary Tree Level Order Traversal [done] (foundational level-by-level BFS)
//   - #107 Binary Tree Level Order Traversal II [done] (bottom-up level aggregation)
//   - #515 Find Largest Value in Each Tree Row [done] (per-level maximum instead of mean)
//   - #1161 Maximum Level Sum of a Binary Tree [done] (finding the level with largest sum)

#include "framework/lc_framework.h"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <limits>
#include <optional>
#include <queue>
#include <vector>

namespace {

constexpr double kEps = 1e-5;

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
  // Computes the arithmetic mean of node values on each level via BFS.
  // Time: O(N), Space: O(W) where W is the maximum level width.
  std::vector<double> averageOfLevels(const TreeNode* root) {
    if (root == nullptr) {
      return {};
    }

    std::vector<double> averages;
    std::queue<const TreeNode*> q;
    q.push(root);

    while (!q.empty()) {
      // Snapshot the number of nodes belonging strictly to the current level.
      const std::size_t level_size = q.size();
      // Accumulate in 64-bit integer to avoid 32-bit signed overflow when node
      // values are near INT_MAX / INT_MIN.
      std::int64_t level_sum = 0;

      for (std::size_t i = 0; i < level_size; ++i) {
        const TreeNode* node = q.front();
        q.pop();
        level_sum += static_cast<std::int64_t>(node->val);

        if (node->left != nullptr) {
          q.push(node->left);
        }
        if (node->right != nullptr) {
          q.push(node->right);
        }
      }

      averages.push_back(static_cast<double>(level_sum) /
                         static_cast<double>(level_size));
    }

    return averages;
  }
};

// RAII arena that owns all test tree nodes with zero manual memory management.
class TreeArena {
 public:
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
  TreeNode* create(int val) {
    nodes_.emplace_back(val);
    return &nodes_.back();
  }

  std::deque<TreeNode> nodes_;
};

// Compares two vectors of doubles element-wise within `kEps`.
bool approx_equal(const std::vector<double>& actual,
                  const std::vector<double>& expected) {
  if (actual.size() != expected.size()) {
    return false;
  }
  for (std::size_t i = 0; i < actual.size(); ++i) {
    if (std::fabs(actual[i] - expected[i]) > kEps) {
      return false;
    }
  }
  return true;
}

LC_PROBLEM(637, "Average Of Levels In Binary Tree", "tree", ctx) {
  Solution sol;

  ctx.test("example_1_standard_tree", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({3, 9, 20, std::nullopt, std::nullopt, 15, 7});
    ctx.expect_true(approx_equal(sol.averageOfLevels(root), {3.0, 14.5, 11.0}));
  });

  ctx.test("python_build_four_leaf_level", [&] {
    // Tree from the original Python `build()`: [3, 9, 20, 8, null, 15, 7].
    TreeArena arena;
    TreeNode* root = arena.build({3, 9, 20, 8, std::nullopt, 15, 7});
    ctx.expect_true(approx_equal(sol.averageOfLevels(root), {3.0, 14.5, 10.0}));
  });

  ctx.test("single_node", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({42});
    ctx.expect_true(approx_equal(sol.averageOfLevels(root), {42.0}));
  });

  ctx.test("int32_overflow_prevention", [&] {
    TreeArena arena;
    constexpr int kMaxInt = std::numeric_limits<int>::max();
    constexpr int kMinInt = std::numeric_limits<int>::min();
    TreeNode* root = arena.build({kMaxInt, kMaxInt, kMaxInt, kMinInt, kMinInt});
    ctx.expect_true(approx_equal(
        sol.averageOfLevels(root),
        {static_cast<double>(kMaxInt), static_cast<double>(kMaxInt),
         static_cast<double>(kMinInt)}));
  });

  ctx.test("empty_tree", [&] {
    ctx.expect_true(sol.averageOfLevels(nullptr).empty());
  });
}

}  // namespace
