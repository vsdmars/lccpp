// LeetCode 113: Path Sum II
// https://leetcode.com/problems/path-sum-ii/
//
// Problem:
//   Given the root of a binary tree and an integer target_sum, return all
//   root-to-leaf paths where the sum of the node values along the path equals
//   target_sum. Each path is returned as a list of node values from root to
//   leaf. A leaf is a node with no children.
//
// Algorithm:
//   Perform a depth-first search (DFS) with backtracking:
//     1. Maintain a single mutable `path` vector representing the node values
//        from the root down to the current node.
//     2. Upon entering a non-null node, push its value onto `path` and compute
//        `next_remaining = remaining - node->val`.
//     3. If the node is a leaf and `next_remaining == 0`, snapshot `path` into
//        `result`.
//     4. Otherwise, recursively explore the left and right subtrees with
//        `next_remaining`.
//     5. Before returning from the frame, pop the current node's value from
//        `path` to restore the ancestor state for sibling branches.
//   Time Complexity:  O(N^2) worst case (visiting all N nodes is O(N), and
//                     copying up to O(N) valid paths of length O(N) into the
//                     output can take O(N^2) total).
//   Space Complexity: O(H) auxiliary space for the recursion stack and active
//                     `path` buffer (excluding the output vector), where H is
//                     the tree height.
//
// Data Structures:
//   - Binary tree (`TreeNode`) representing the input hierarchy.
//   - `std::vector<int>` (`path`) used as a backtracking stack to avoid
//     allocating intermediate path copies at non-leaf nodes.
//   - `std::vector<std::vector<int>>` (`result`) storing all matching paths.
//
// Real-World Applications:
//   - Auditing and compliance reporting over hierarchical account/ledger trees
//     (enumerating every complete allocation path matching a target total).
//   - Compiler/planner trace generation that enumerates all valid execution
//     paths achieving an exact cost or latency budget.
//   - Combinatorial search and root-to-leaf scenario extraction in decision
//     trees.
//
// Related LeetCode Problems:
//   - #112 Path Sum [done] (decision version checking whether any matching path exists)
//   - #437 Path Sum III [done] (counts downward paths not restricted to root-to-leaf)
//   - #257 Binary Tree Paths [done] (enumerates all root-to-leaf paths via backtracking)
//   - #129 Sum Root to Leaf Numbers [done] (aggregates root-to-leaf paths into a single sum)
//   - #666 Path Sum IV [done] (computes path sums on a compactly encoded binary tree)

#include "framework/lc_framework.h"

#include <deque>
#include <vector>

namespace {

// Standard binary tree node.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int value) : val(value) {}
  TreeNode(int value, TreeNode* left_child, TreeNode* right_child)
      : val(value), left(left_child), right(right_child) {}
};

// Arena owning TreeNode instances for leak-free test tree construction.
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
  // Collects every root-to-leaf path whose node values sum to target_sum.
  //
  // Invariant: on entry to dfs(node, remaining), `path` holds the values of all
  // strict ancestors of `node` in root-to-parent order, and `remaining` equals
  // target_sum minus the sum of `path`. `path` is restored to that exact state
  // before dfs returns.
  // Time: O(N^2) worst case, Space: O(H) auxiliary.
  std::vector<std::vector<int>> pathSum(const TreeNode* root, int target_sum) {
    std::vector<std::vector<int>> result;
    std::vector<int> path;

    auto dfs = [&](this auto&& self, const TreeNode* node,
                   int remaining) -> void {
      if (node == nullptr) {
        return;
      }

      // Include the current node in the active root-to-current path.
      path.push_back(node->val);
      const int next_remaining = remaining - node->val;

      if (node->left == nullptr && node->right == nullptr) {
        // Record a snapshot only when a leaf completes the exact target sum.
        if (next_remaining == 0) {
          result.push_back(path);
        }
      } else {
        self(node->left, next_remaining);
        self(node->right, next_remaining);
      }

      // Backtrack so sibling branches see the exact parent prefix.
      path.pop_back();
    };

    dfs(root, target_sum);
    return result;
  }
};

LC_PROBLEM(113, "Path Sum II", "tree", ctx) {
  Solution sol;

  ctx.test("leetcode example 1 and python build() (two matching paths)", [&] {
    //         5
    //        / \
    //       4   8
    //      /   / \
    //     11  13  4
    //    /  \    / \
    //   7    2  5   1
    TreeArena arena;
    TreeNode* root = arena.make(
        5, arena.make(4, arena.make(11, arena.make(7), arena.make(2))),
        arena.make(8, arena.make(13),
                   arena.make(4, arena.make(5), arena.make(1))));

    const std::vector<std::vector<int>> expected = {{5, 4, 11, 2},
                                                    {5, 8, 4, 5}};
    ctx.expect_eq(sol.pathSum(root, 22), expected);
  });

  ctx.test("empty tree returns empty list", [&] {
    const std::vector<std::vector<int>> expected;
    ctx.expect_eq(sol.pathSum(nullptr, 0), expected);
  });

  ctx.test("no matching path returns empty list", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(1, arena.make(2), arena.make(3));
    const std::vector<std::vector<int>> expected;
    ctx.expect_eq(sol.pathSum(root, 5), expected);
  });

  ctx.test("single node matching and non-matching", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(7);
    ctx.expect_eq(sol.pathSum(root, 7), std::vector<std::vector<int>>{{7}});
    ctx.expect_eq(sol.pathSum(root, 3), std::vector<std::vector<int>>{});
  });

  ctx.test("negative values and zero-sum extensions", [&] {
    //      1
    //     / \
    //   -2   -3
    //   / \  /
    //  1  3 -2
    // /
    //-1
    TreeArena arena;
    TreeNode* root = arena.make(
        1, arena.make(-2, arena.make(1, arena.make(-1)), arena.make(3)),
        arena.make(-3, arena.make(-2)));

    // Path 1 -> -2 -> 1 -> -1 sums to -1.
    ctx.expect_eq(sol.pathSum(root, -1),
                  std::vector<std::vector<int>>{{1, -2, 1, -1}});
    // Path 1 -> -2 -> 3 sums to 2.
    ctx.expect_eq(sol.pathSum(root, 2),
                  std::vector<std::vector<int>>{{1, -2, 3}});
  });
}

}  // namespace
