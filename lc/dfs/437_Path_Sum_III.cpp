// LeetCode 437: Path Sum III
// https://leetcode.com/problems/path-sum-iii/
//
// Problem:
//   Given the root of a binary tree and an integer targetSum, return the
//   number of downward paths where the sum of the node values along the path
//   equals targetSum. A path does not need to start at the root or end at a
//   leaf, but it must travel strictly downward from parent nodes to child
//   nodes. The tree has up to 1,000 nodes with values in [-10^9, 10^9], so
//   intermediate path sums require 64-bit signed integers to avoid overflow.
//
// Algorithm:
//   Use a single preorder DFS combined with prefix sums (the tree analogue of
//   Subarray Sum Equals K). Maintain the running 64-bit sum `curr_sum` from
//   the root to the current node and a hash map `prefix_counts` recording how
//   many times each prefix sum has occurred along the active root-to-node
//   path (seeded with `{0: 1}` to capture paths starting at the root). At
//   each node, any ancestor with prefix sum `curr_sum - target_sum` defines a
//   valid downward path ending at the current node, so we add
//   `prefix_counts[curr_sum - target_sum]` to the answer. We then increment
//   `prefix_counts[curr_sum]`, recurse into left and right subtrees, and
//   decrement (erasing if zero) on backtrack so sibling subtrees never see
//   unrelated prefix sums.
//   Time Complexity:  O(N) average, visiting each of the N nodes once with
//                     O(1) expected hash-map operations.
//   Space Complexity: O(H) for the recursion stack and active prefix-sum map,
//                     where H is the height of the tree (O(N) worst case).
//
// Data Structures:
//   - Binary tree (`TreeNode`): input hierarchy traversed via preorder DFS.
//   - Hash map (`std::unordered_map<std::int64_t, int>`): tracks frequencies
//     of prefix sums along the current root-to-node ancestor chain.
//
// Real-World Applications:
//   - Hierarchical budget / cost-center auditing: finding contiguous chains
//     of organizational units whose cumulative spend matches a target budget.
//   - Distributed tracing & flame-graph analysis: locating nested call-stack
//     segments whose combined latency or allocation delta hits a threshold.
//   - Financial cash-flow trees: identifying vertical sequences of ledger
//     adjustments that net to zero or a specific reconciliation amount.
//
// Related LeetCode Problems:
//   - #112 Path Sum [done] (root-to-leaf path matching a target sum)
//   - #113 Path Sum II [done] (collect all root-to-leaf paths summing to target)
//   - #124 Binary Tree Maximum Path Sum [done] (any-to-any max path sum in a tree)
//   - #560 Subarray Sum Equals K [done] (1D array prefix-sum counting analogue)
//   - #666 Path Sum IV [done] (path sums on an encoded compact binary tree)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <deque>
#include <optional>
#include <unordered_map>
#include <vector>

namespace {

// Standard binary tree node used by LeetCode tree problems.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int v) : val(v) {}
  TreeNode(int v, TreeNode* l, TreeNode* r) : val(v), left(l), right(r) {}
};

// RAII arena that owns TreeNode instances in a pointer-stable deque so test
// trees can be constructed from level-order vectors with zero memory leaks.
class TreeArena {
 public:
  // Builds a binary tree from LeetCode level-order representation where
  // std::nullopt denotes an absent child.
  TreeNode* build(const std::vector<std::optional<int>>& level_order) {
    if (level_order.empty() || !level_order[0].has_value()) {
      return nullptr;
    }

    TreeNode* root = create(*level_order[0]);
    std::deque<TreeNode*> queue;
    queue.push_back(root);

    std::size_t idx = 1uz;
    while (!queue.empty() && idx < level_order.size()) {
      TreeNode* curr = queue.front();
      queue.pop_front();

      if (level_order[idx].has_value()) {
        curr->left = create(*level_order[idx]);
        queue.push_back(curr->left);
      }
      ++idx;

      if (idx < level_order.size() && level_order[idx].has_value()) {
        curr->right = create(*level_order[idx]);
        queue.push_back(curr->right);
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

class Solution {
 public:
  // Counts all downward paths in `root` whose node values sum to `target_sum`.
  // Time: O(N) average | Space: O(H), where H is the tree height.
  int pathSum(const TreeNode* root, int target_sum) {
    if (root == nullptr) {
      return 0;
    }

    // Seed with prefix sum 0 occurring once before the root so any path that
    // starts directly at the root is counted when `curr_sum == target_sum`.
    std::unordered_map<std::int64_t, int> prefix_counts;
    prefix_counts.emplace(0, 1);

    const auto target = static_cast<std::int64_t>(target_sum);

    // Preorder DFS carrying the 64-bit cumulative sum from the root down to
    // the parent of `node`.
    auto dfs = [&](this auto&& self, const TreeNode* node,
                   std::int64_t running_sum) -> int {
      if (node == nullptr) {
        return 0;
      }

      const std::int64_t curr_sum =
          running_sum + static_cast<std::int64_t>(node->val);

      // Every ancestor with prefix sum `curr_sum - target` yields a valid
      // downward path ending at `node`.
      int count = 0;
      if (const auto it = prefix_counts.find(curr_sum - target);
          it != prefix_counts.end()) {
        count += it->second;
      }

      // Record `curr_sum` for descendants, explore both subtrees, then restore
      // the map on backtrack so sibling branches remain isolated.
      ++prefix_counts[curr_sum];
      count += self(node->left, curr_sum);
      count += self(node->right, curr_sum);

      auto curr_it = prefix_counts.find(curr_sum);
      --curr_it->second;
      if (curr_it->second == 0) {
        prefix_counts.erase(curr_it);
      }

      return count;
    };

    return dfs(root, 0);
  }
};

LC_PROBLEM(437, "Path Sum III", "dfs", ctx) {
  ctx.test("example_1_from_python_build", [&] {
    TreeArena arena;
    //       10
    //      /  \
    //     5   -3
    //    / \    \
    //   3   2   11
    //  / \   \
    // 3  -2   1
    const TreeNode* root = arena.build({10, 5, -3, 3, 2, std::nullopt, 11, 3,
                                        -2, std::nullopt, 1});
    Solution sol;
    ctx.expect_eq(sol.pathSum(root, 8), 3);
  });

  ctx.test("example_2_multiple_paths", [&] {
    TreeArena arena;
    const TreeNode* root =
        arena.build({5, 4, 8, 11, std::nullopt, 13, 4, 7, 2, std::nullopt,
                     std::nullopt, 5, 1});
    Solution sol;
    ctx.expect_eq(sol.pathSum(root, 22), 3);
  });

  ctx.test("empty_and_single_node", [&] {
    TreeArena arena;
    Solution sol;
    ctx.expect_eq(sol.pathSum(nullptr, 0), 0);

    const TreeNode* single = arena.build({7});
    ctx.expect_eq(sol.pathSum(single, 7), 1);
    ctx.expect_eq(sol.pathSum(single, 8), 0);
  });

  ctx.test("zero_target_overlapping_subpaths", [&] {
    TreeArena arena;
    // Chain 0 -> 0 -> 0 has 3 + 2 + 1 = 6 downward paths summing to 0.
    const TreeNode* root = arena.build({0, 0, std::nullopt, 0});
    Solution sol;
    ctx.expect_eq(sol.pathSum(root, 0), 6);
  });

  ctx.test("large_values_avoid_32bit_overflow", [&] {
    TreeArena arena;
    // Sum exceeds INT32_MAX; 32-bit arithmetic would wrap around to 0.
    const TreeNode* root =
        arena.build({1000000000, 1000000000, std::nullopt, 294967296,
                     std::nullopt, 1000000000, std::nullopt, 1000000000,
                     std::nullopt, 1000000000});
    Solution sol;
    ctx.expect_eq(sol.pathSum(root, 0), 0);
  });
}

}  // namespace
