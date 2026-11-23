// LeetCode 1120: Maximum Average Subtree
// https://leetcode.com/problems/maximum-average-subtree/
//
// Problem:
//   Given the root of a binary tree, find the maximum average value of any
//   subtree of that tree. A subtree is any node together with all of its
//   descendants, and its average is the sum of its node values divided by its
//   node count.
//   Constraints: 1 to 5000 nodes; each node value is in [0, 100000]. Answers
//   within 10^-5 of the true value are accepted.
//
// Examples:
//   Example 1:
//     Input:  root = [5, 6, 1]
//     Output: 6.0
//     Explanation:
//       - Visit leaf 6 first in post-order DFS, returning {sum = 6, count = 1} and
//         setting max_avg = 6.0 / 1 = 6.0.
//       - Visit leaf 1, returning {sum = 1, count = 1} with avg = 1.0, leaving
//         max_avg at 6.0.
//       - Aggregate root 5 with total_sum = 5 + 6 + 1 = 12 and total_count = 3,
//         giving avg = 4.0 and returning max_avg = 6.0.
//
//   Example 2:
//     Input:  root = [5, 4, 100, 10, null, 2, 30]
//     Output: 44.0
//     Explanation:
//       - In the left subtree of 5, leaf 10 returns {10, 1} (avg = 10.0) and node 4
//         aggregates {14, 2} (avg = 7.0), setting max_avg = 10.0.
//       - In the right subtree of 5, leaves 2 and 30 return {2, 1} and {30, 1},
//         raising max_avg to 30.0.
//       - Node 100 aggregates {sum = 100 + 2 + 30 = 132, count = 3}, computing avg
//         = 132.0 / 3 = 44.0 and updating max_avg = 44.0.
//       - Root 5 aggregates {sum = 151, count = 6} with avg = 25.16667, so max_avg
//         remains 44.0.
//
//   Example 3:
//     Input:  root = [0, null, 1]
//     Output: 1.0
//     Explanation:
//       - Null left child of 0 returns {sum = 0, count = 0} without updating
//         max_avg.
//       - Right child leaf 1 returns {sum = 1, count = 1} with avg = 1.0, setting
//         max_avg = 1.0.
//       - Root 0 aggregates {sum = 1, count = 2} with avg = 0.5, returning max_avg
//         = 1.0.
//
// Algorithm:
//   Perform a bottom-up post-order DFS. Each recursive call computes and
//   returns a pair {sum, count} for the subtree rooted at the current node by
//   aggregating its left and right child summaries plus itself. At every node,
//   evaluate `static_cast<double>(sum) / static_cast<double>(count)` and
//   update the running global maximum `max_avg`. Using 64-bit integers for the
//   subtree sum eliminates any risk of signed 32-bit overflow.
//   Time Complexity:  O(N), visiting each node exactly once.
//   Space Complexity: O(H) for the recursion stack, where H is tree height
//                     (O(log N) balanced, O(N) worst-case skewed).
//
// Data Structures:
//   - SubtreeStats struct ({std::int64_t sum, int count}): lightweight value
//     returned up the recursion stack from each post-order call.
//
// Real-World Applications:
//   - Profiling call trees / flame graphs to locate the subtree with the
//     highest average latency or resource consumption per call.
//   - Organizational or retail hierarchy analytics (finding the division or
//     product category subtree with the highest average revenue per unit).
//   - Decision-tree pruning and cost-complexity scoring in machine learning.
//
// Related LeetCode Problems:
//   - #1161 Maximum Level Sum Of A Binary Tree [done] (per-level sum aggregation)
//   - #2265 Count Nodes Equal to Average of Subtree [new] (same post-order sum/count pattern)
//   - #508 Most Frequent Subtree Sum [done] (bottom-up subtree sum aggregation)
//   - #124 Binary Tree Maximum Path Sum [done] (post-order tree DP with global max)

#include "framework/lc_framework.h"

#include <algorithm>
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

// Floating-point comparison helper matching LeetCode's 10^-5 tolerance.
[[nodiscard]] bool approx_eq(double actual, double expected) {
  return std::fabs(actual - expected) <= kEps;
}

// Standard LeetCode binary tree node.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  TreeNode() = default;
  explicit TreeNode(int x) : val(x) {}
  TreeNode(int x, TreeNode* l, TreeNode* r) : val(x), left(l), right(r) {}
};

// RAII arena owning test TreeNode instances via pointer-stable std::deque.
class TreeArena {
 public:
  [[nodiscard]] TreeNode* make(int val) {
    nodes_.emplace_back(val);
    return &nodes_.back();
  }

  // Builds a binary tree from LeetCode level-order array representation.
  [[nodiscard]] TreeNode* build(const std::vector<std::optional<int>>& vals) {
    if (vals.empty() || !vals[0].has_value()) {
      return nullptr;
    }
    TreeNode* root = make(*vals[0]);
    std::queue<TreeNode*> q;
    q.push(root);
    std::size_t idx = 1uz;

    while (!q.empty() && idx < vals.size()) {
      auto* cur = q.front();
      q.pop();

      if (idx < vals.size() && vals[idx].has_value()) {
        cur->left = make(*vals[idx]);
        q.push(cur->left);
      }
      ++idx;

      if (idx < vals.size() && vals[idx].has_value()) {
        cur->right = make(*vals[idx]);
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
  // Computes the maximum average value among all non-empty subtrees of `root`.
  // Post-order DFS returns {sum, count} for each subtree and tracks the global
  // maximum average. Time: O(N), Space: O(H).
  double maximumAverageSubtree(const TreeNode* root) {
    if (root == nullptr) {
      return 0.0;
    }

    struct SubtreeStats {
      std::int64_t sum = 0;
      int count = 0;
    };

    double max_avg = std::numeric_limits<double>::lowest();

    auto dfs = [&](this auto&& self, const TreeNode* node) -> SubtreeStats {
      if (node == nullptr) {
        return SubtreeStats{};
      }

      // Compute child subtree aggregates first (post-order invariant).
      const SubtreeStats left = self(node->left);
      const SubtreeStats right = self(node->right);

      const std::int64_t total_sum =
          static_cast<std::int64_t>(node->val) + left.sum + right.sum;
      const int total_count = 1 + left.count + right.count;

      const double avg =
          static_cast<double>(total_sum) / static_cast<double>(total_count);
      max_avg = std::max(max_avg, avg);

      return SubtreeStats{.sum = total_sum, .count = total_count};
    };

    dfs(root);
    return max_avg;
  }
};

LC_PROBLEM(1120, "Maximum Average Subtree", "tree", ctx) {
  ctx.test("example_1", [&] {
    TreeArena arena;
    const TreeNode* root = arena.build({5, 6, 1});
    Solution sol;
    ctx.expect_true(approx_eq(sol.maximumAverageSubtree(root), 6.0));
  });

  ctx.test("example_2_zero_root_positive_child", [&] {
    TreeArena arena;
    const TreeNode* root = arena.build({0, std::nullopt, 1});
    Solution sol;
    ctx.expect_true(approx_eq(sol.maximumAverageSubtree(root), 1.0));
  });

  ctx.test("source_build_non_leaf_optimum", [&] {
    // Matches Python build():
    //         5
    //       /   \
    //      4    100
    //     /     / \
    //    10    2   30
    // Subtree rooted at 100 has sum = 132, count = 3 -> avg = 44.0 (maximum).
    TreeArena arena;
    TreeNode* n5 = arena.make(5);
    TreeNode* n4 = arena.make(4);
    TreeNode* n10 = arena.make(10);
    TreeNode* n100 = arena.make(100);
    TreeNode* n2 = arena.make(2);
    TreeNode* n30 = arena.make(30);
    n5->left = n4;
    n5->right = n100;
    n4->left = n10;
    n100->left = n2;
    n100->right = n30;

    Solution sol;
    ctx.expect_true(approx_eq(sol.maximumAverageSubtree(n5), 44.0));
  });

  ctx.test("whole_tree_is_optimal_fractional", [&] {
    TreeArena arena;
    const TreeNode* root = arena.build({10, 5, 5});
    Solution sol;
    // Root subtree average = 20 / 3 = 6.66667 > leaf averages of 5.0.
    ctx.expect_true(approx_eq(sol.maximumAverageSubtree(root), 20.0 / 3.0));
  });

  ctx.test("single_node_zero_and_positive", [&] {
    TreeArena arena;
    const TreeNode* zero = arena.make(0);
    const TreeNode* forty_two = arena.make(42);
    Solution sol;
    ctx.expect_true(approx_eq(sol.maximumAverageSubtree(zero), 0.0));
    ctx.expect_true(approx_eq(sol.maximumAverageSubtree(forty_two), 42.0));
  });
}

}  // namespace
