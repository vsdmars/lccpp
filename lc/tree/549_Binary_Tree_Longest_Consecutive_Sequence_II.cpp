// LeetCode 549: Binary Tree Longest Consecutive Sequence II
// https://leetcode.com/problems/binary-tree-longest-consecutive-sequence-ii/
//
// Problem:
//   Given the root of a binary tree, return the length (in nodes) of the
//   longest consecutive path in the tree. A consecutive path is a sequence of
//   connected nodes where every adjacent pair differs by 1 in the same
//   direction (strictly increasing by +1 or strictly decreasing by -1). Unlike
//   version I (#298), the path may go in child-parent-child order (turning at
//   an ancestor node) rather than only parent-to-child.
//
// Algorithm:
//   Use a single bottom-up post-order DFS `dfs(node, parent)` returning a pair
//   `{dec, inc}` relative to `parent`:
//   - `dec`: length of the longest consecutive downward path entering `node`
//     where values decrease by 1 at each step from `parent` (non-zero only if
//     `node->val == parent->val - 1`).
//   - `inc`: length of the longest consecutive downward path entering `node`
//     where values increase by 1 at each step from `parent` (non-zero only if
//     `node->val == parent->val + 1`).
//   At each `node`, after querying its left and right children with `node` as
//   their parent:
//   - A valid path turning at `node` can combine a decreasing branch from one
//     child with an increasing branch from the other child (plus `node`
//     itself): `left.dec + right.inc + 1` and `right.dec + left.inc + 1`.
//     Update the global maximum with both candidates.
//   - Propagate `{1 + max(left.dec, right.dec), 0}` upward if `node->val ==
//     parent->val - 1`, `{0, 1 + max(left.inc, right.inc)}` if `node->val ==
//     parent->val + 1`, or `{0, 0}` otherwise.
//   Time Complexity: O(n), visiting each node exactly once.
//   Space Complexity: O(h), where h is the tree height for the recursion stack.
//
// Data Structures:
//   - Binary tree (`TreeNode`).
//   - Lightweight value struct (`ConsecutiveLengths`) returned up the DFS call
//     stack to carry downward decreasing and increasing chain lengths.
//
// Real-World Applications:
//   - Detecting longest contiguous version/sequence chains across branching
//     VCS commit trees or hierarchical event logs.
//   - Finding monotonic contiguous progression paths in decision trees or
//     game-state search trees.
//   - Analyzing sequential packet/frame runs across tree-structured telemetry
//     hierarchies.
//
// Related LeetCode Problems:
//   - #298 Binary Tree Longest Consecutive Sequence [done] (parent-to-child increasing paths only)
//   - #543 Diameter of Binary Tree [done] (post-order turning-point path maximization)
//   - #124 Binary Tree Maximum Path Sum [done] (child-parent-child path DP on trees)
//   - #687 Longest Univalue Path [done] (same turning-point tree DP with equal adjacent values)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstdint>
#include <deque>

namespace {

// Standard binary tree node used by LeetCode tree problems.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;
};

// Lengths of downward consecutive chains starting at a child node relative to
// its parent: `dec` counts values stepping by -1 from parent; `inc` counts
// values stepping by +1 from parent.
struct ConsecutiveLengths {
  int dec = 0;
  int inc = 0;
};

class Solution {
 public:
  // Computes the length (in nodes) of the longest increasing or decreasing
  // consecutive path, allowing child-parent-child turns.
  // Time: O(n), Space: O(h).
  int longestConsecutive(const TreeNode* root) {
    if (root == nullptr) {
      return 0;
    }

    int best = 0;

    // Post-order DFS computing consecutive chain lengths extending from
    // `parent` into `node` while updating `best` for paths turning at `node`.
    auto dfs = [&](this auto&& self, const TreeNode* node,
                   const TreeNode* parent) -> ConsecutiveLengths {
      if (node == nullptr) {
        return {};
      }

      const ConsecutiveLengths left = self(node->left, node);
      const ConsecutiveLengths right = self(node->right, node);

      // A path through `node` can descend by -1 on one side and +1 on the
      // other (reading end-to-end as strictly monotonic consecutive values).
      const int left_dec_right_inc = left.dec + right.inc + 1;
      const int right_dec_left_inc = right.dec + left.inc + 1;
      best = std::max({best, left_dec_right_inc, right_dec_left_inc});

      // Use 64-bit arithmetic to avoid signed overflow near INT_MIN / INT_MAX.
      const auto node_val = static_cast<std::int64_t>(node->val);
      const auto parent_val = static_cast<std::int64_t>(parent->val);

      if (node_val == parent_val - 1) {
        return {.dec = 1 + std::max(left.dec, right.dec), .inc = 0};
      }
      if (node_val == parent_val + 1) {
        return {.dec = 0, .inc = 1 + std::max(left.inc, right.inc)};
      }
      return {};
    };

    // Passing `root` as its own parent ensures the root contributes to `best`
    // and cleanly returns {0, 0} without needing a separate null-parent branch.
    dfs(root, root);
    return best;
  }
};

// RAII arena that owns test TreeNode instances without raw heap allocations.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    nodes_.push_back(TreeNode{.val = val, .left = left, .right = right});
    return &nodes_.back();
  }

 private:
  std::deque<TreeNode> nodes_;
};

LC_PROBLEM(549, "Binary Tree Longest Consecutive Sequence II", "tree", ctx) {
  Solution sol;

  ctx.test("example_1_root_to_child", [&] {
    //     1
    //    / \
    //   2   3
    // Longest consecutive path: [1, 2] or [2, 1] -> length 2.
    TreeArena arena;
    TreeNode* root = arena.make(1, arena.make(2), arena.make(3));
    ctx.expect_eq(sol.longestConsecutive(root), 2);
  });

  ctx.test("example_2_child_parent_child_turn", [&] {
    //     2
    //    / \
    //   1   3
    // Longest consecutive path: [1, 2, 3] or [3, 2, 1] -> length 3.
    TreeArena arena;
    TreeNode* root = arena.make(2, arena.make(1), arena.make(3));
    ctx.expect_eq(sol.longestConsecutive(root), 3);
  });

  ctx.test("python_build_four_node_turn", [&] {
    //       3
    //      / \
    //     2   4
    //        / \
    //       5   7
    // Longest consecutive path: [2, 3, 4, 5] -> length 4.
    TreeArena arena;
    TreeNode* n4 = arena.make(4, arena.make(5), arena.make(7));
    TreeNode* root = arena.make(3, arena.make(2), n4);
    ctx.expect_eq(sol.longestConsecutive(root), 4);
  });

  ctx.test("single_node_and_empty_tree", [&] {
    TreeArena arena;
    ctx.expect_eq(sol.longestConsecutive(arena.make(10)), 1);
    ctx.expect_eq(sol.longestConsecutive(nullptr), 0);
  });

  ctx.test("negative_values_and_all_equal", [&] {
    //      -2
    //      / \
    //    -3  -1
    //    /
    //  -4
    // Longest consecutive path: [-4, -3, -2, -1] -> length 4.
    TreeArena arena;
    TreeNode* left = arena.make(-3, arena.make(-4), nullptr);
    TreeNode* root = arena.make(-2, left, arena.make(-1));
    ctx.expect_eq(sol.longestConsecutive(root), 4);

    // All equal values -> longest consecutive path is a single node (1).
    TreeNode* flat = arena.make(5, arena.make(5), arena.make(5));
    ctx.expect_eq(sol.longestConsecutive(flat), 1);
  });
}

}  // namespace
