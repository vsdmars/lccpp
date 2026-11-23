// LeetCode 687: Longest Univalue Path
// https://leetcode.com/problems/longest-univalue-path/
//
// Problem:
//   Given the root of a binary tree, return the length of the longest path
//   where every node along the path has the exact same value. The path may or
//   may not pass through the root, and its length is measured by the number of
//   edges between the endpoints.
//
// Algorithm:
//   Use post-order DFS where `dfs(node)` returns the length (in edges) of the
//   longest downward univalue path starting at `node` and extending into at
//   most one child branch:
//     1. Recursively compute the downward univalue lengths of `node->left` and
//        `node->right` (always recursing even when values differ so paths
//        entirely inside subtrees are still discovered).
//     2. If `node->left` exists and shares `node->val`, we can extend its
//        downward path by 1 edge (`left_arrow = left_len + 1`); else `0`.
//        Compute `right_arrow` symmetrically for `node->right`.
//     3. Any univalue path whose highest node (turning point) is `node` has
//        edge length `left_arrow + right_arrow`; update `max_edges` with it.
//     4. Return `std::max(left_arrow, right_arrow)` to the parent, because a
//        simple path continuing upward can only pick one downward branch.
//   Time Complexity:  O(n), visiting every node exactly once.
//   Space Complexity: O(h) recursion stack where h is the tree height.
//
// Data Structures:
//   - Binary tree nodes (`TreeNode`).
//   - Call-stack frames during post-order DFS plus a scalar accumulator for
//     the global maximum edge count.
//
// Real-World Applications:
//   - Phylogenetic tree analysis: finding the longest contiguous evolutionary
//     lineage sharing an identical genetic trait or marker.
//   - Network topology monitoring: detecting the longest contiguous chain of
//     routers/switches running the same firmware or fault state in a spanning
//     tree.
//   - Compiler AST / expression-tree optimization: identifying the longest
//     chain of identical associative operators for flattening.
//
// Related LeetCode Problems:
//   - #543 Diameter of Binary Tree [done] (same post-order left+right turning-point pattern without value matching)
//   - #124 Binary Tree Maximum Path Sum [done] (turning-point path optimization maximizing node sum)
//   - #250 Count Univalue Subtrees [new] (post-order univalue check across subtrees)
//   - #298 Binary Tree Longest Consecutive Sequence [done] (longest downward consecutive-value path)
//   - #549 Binary Tree Longest Consecutive Sequence II [done] (turning-point path allowing parent-child turns)

#include "framework/lc_framework.h"

#include <algorithm>
#include <deque>

namespace {

// Standard binary tree node.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int v, TreeNode* l = nullptr, TreeNode* r = nullptr)
      : val(v), left(l), right(r) {}
};

// RAII arena owning test TreeNode allocations.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    return &nodes_.emplace_back(val, left, right);
  }

 private:
  std::deque<TreeNode> nodes_;
};

class Solution {
 public:
  // Returns the maximum number of edges along any path whose nodes all share
  // the same value.
  // Time: O(n), Space: O(h).
  int longestUnivaluePath(TreeNode* root) {
    int max_edges = 0;

    // Returns the longest single-branch downward univalue path starting at
    // `node`, while updating `max_edges` with paths peaking at `node`.
    auto dfs = [&max_edges](this auto&& self, const TreeNode* node) -> int {
      if (node == nullptr) {
        return 0;
      }

      // Always explore both children so univalue paths inside subtrees with
      // different values are still evaluated.
      const int left_sub = self(node->left);
      const int right_sub = self(node->right);

      int left_arrow = 0;
      if (node->left != nullptr && node->left->val == node->val) {
        left_arrow = left_sub + 1;
      }

      int right_arrow = 0;
      if (node->right != nullptr && node->right->val == node->val) {
        right_arrow = right_sub + 1;
      }

      // A path that turns at `node` joins both matching downward branches.
      max_edges = std::max(max_edges, left_arrow + right_arrow);

      // A path continuing up to `node`'s parent can only use one branch.
      return std::max(left_arrow, right_arrow);
    };

    dfs(root);
    return max_edges;
  }
};

LC_PROBLEM(687, "Longest Univalue Path", "tree", ctx) {
  ctx.test("example1_right_chain_of_fives", [&] {
    //       5
    //      / \
    //     4   5
    //    / \   \
    //   1   1   5
    TreeArena arena;
    TreeNode* left = arena.make(4, arena.make(1), arena.make(1));
    TreeNode* right = arena.make(5, nullptr, arena.make(5));
    TreeNode* root = arena.make(5, left, right);
    Solution sol;
    ctx.expect_eq(sol.longestUnivaluePath(root), 2);
  });

  ctx.test("example2_v_shape_of_fours", [&] {
    //       1
    //      / \
    //     4   5
    //    / \   \
    //   4   4   5
    TreeArena arena;
    TreeNode* left = arena.make(4, arena.make(4), arena.make(4));
    TreeNode* right = arena.make(5, nullptr, arena.make(5));
    TreeNode* root = arena.make(1, left, right);
    Solution sol;
    ctx.expect_eq(sol.longestUnivaluePath(root), 2);
  });

  ctx.test("zigzag_deep_chain_beats_root_chain", [&] {
    // Deep zigzag of five 1s (4 edges) inside left subtree beats four 5s (3).
    TreeArena arena;
    TreeNode* n1_5 = arena.make(1);
    TreeNode* n1_4 = arena.make(1, nullptr, n1_5);
    TreeNode* n1_3 = arena.make(1, n1_4, nullptr);
    TreeNode* n1_2 = arena.make(1, nullptr, n1_3);
    TreeNode* n1_1 = arena.make(1, n1_2, nullptr);
    TreeNode* n4 = arena.make(4, n1_1, arena.make(1));
    TreeNode* n5_3 = arena.make(5, arena.make(5), nullptr);
    TreeNode* n5_2 = arena.make(5, nullptr, n5_3);
    TreeNode* root = arena.make(5, n4, n5_2);
    Solution sol;
    ctx.expect_eq(sol.longestUnivaluePath(root), 4);
  });

  ctx.test("empty_and_single_node", [&] {
    TreeArena arena;
    Solution sol;
    ctx.expect_eq(sol.longestUnivaluePath(nullptr), 0);
    ctx.expect_eq(sol.longestUnivaluePath(arena.make(7)), 0);
  });

  ctx.test("all_distinct_values", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(1, arena.make(2), arena.make(3));
    Solution sol;
    ctx.expect_eq(sol.longestUnivaluePath(root), 0);
  });

  ctx.test("negative_values_v_path_through_root", [&] {
    //       -3
    //      /  \
    //    -3   -3
    //    /      \
    //  -3       -3
    TreeArena arena;
    TreeNode* left = arena.make(-3, arena.make(-3), nullptr);
    TreeNode* right = arena.make(-3, nullptr, arena.make(-3));
    TreeNode* root = arena.make(-3, left, right);
    Solution sol;
    ctx.expect_eq(sol.longestUnivaluePath(root), 4);
  });
}

}  // namespace
