// LeetCode 663: Equal Tree Partition
// https://leetcode.com/problems/equal-tree-partition/
//
// Problem:
//   Given the root of a binary tree with n nodes (node values may be negative,
//   zero, or positive), determine whether it is possible to partition the tree
//   into two non-empty trees with equal sums of node values by removing
//   exactly one edge from the original tree.
//
// Examples:
//   Example 1:
//     Input:  root = [5, 10, 10, null, null, 2, 3]
//     Output: true
//     Explanation:
//       - Post-order DFS compute_sum evaluates left child 10, right subtree leaves
//         2 and 3, node 10 (10 + 2 + 3 = 15), and root 5 (5 + 10 + 15 = 30),
//         recording subtree_sums = [10, 2, 3, 15, 30].
//       - pop_back() removes the root sum 30, leaving proper subtree sums [10, 2,
//         3, 15] and total_sum = 30.
//       - Because total_sum = 30 is even and target = 30 / 2 = 15 appears in [10,
//         2, 3, 15], checkEqualTree returns true.
//
//   Example 2:
//     Input:  root = [1, 2, 10, null, null, 2, 20]
//     Output: false
//     Explanation:
//       - Post-order compute_sum records left child 2, right subtree sums 2, 20,
//         and 32 (10 + 2 + 20), and root sum 35 (1 + 2 + 32), giving subtree_sums =
//         [2, 2, 20, 32, 35].
//       - pop_back() removes root sum 35 so total_sum = 35.
//       - Since total_sum % 2 != 0, no single edge can partition the tree into two
//         equal integer sums, returning false.
//
//   Example 3:
//     Input:  root = [0, -1, 1]
//     Output: false
//     Explanation:
//       - Post-order compute_sum records left child -1, right child 1, and root 0 +
//         (-1) + 1 = 0, producing subtree_sums = [-1, 1, 0].
//       - pop_back() excludes the root's own sum 0 so cutting above the root is
//         disallowed, leaving proper subtree sums [-1, 1].
//       - Even though total_sum = 0 is even with target = 0 / 2 = 0,
//         std::ranges::find does not find 0 in [-1, 1] and returns false.
//
// Algorithm:
//   Removing a single edge above any proper (non-root) node u splits the tree
//   into u's subtree and the rest of the tree. Both halves have equal sum if
//   and only if the total tree sum is even and some proper subtree has sum
//   equal to total / 2.
//
//   We perform a single post-order DFS that computes and records every subtree
//   sum into a vector using 64-bit integers to avoid overflow. Because
//   post-order appends the root's own sum last, popping the final element
//   yields the total sum while leaving only proper subtree sums (crucial when
//   total == 0, where the root itself has sum 0 == total / 2 but cannot be
//   separated by removing an edge). If total is even, we check whether
//   total / 2 appears among the remaining proper subtree sums.
//   Time Complexity:  O(n), visiting every node once and scanning n - 1 sums.
//   Space Complexity: O(n) for the recorded subtree sums and O(h) recursion
//                     stack (h <= n).
//
// Data Structures:
//   - Binary tree nodes (`TreeNode`) representing the input hierarchy.
//   - Dynamic array (`std::vector<std::int64_t>`) storing all proper subtree
//     sums collected in post-order.
//
// Real-World Applications:
//   - Hierarchical workload balancing: splitting a tree-structured task graph
//     or directory hierarchy along a single link into two balanced partitions.
//   - Power-grid and network segmentation: finding a single feeder/link cut
//     that balances aggregate load across two isolated sub-networks.
//   - Database/storage shard splitting: identifying a single subtree split
//     point in an index or organizational hierarchy with equal weight.
//
// Related LeetCode Problems:
//   - #1339 Maximum Product of Splitted Binary Tree [new] (same single-edge subtree-sum cut technique, maximizing product)
//   - #508 Most Frequent Subtree Sum [done] (post-order collection and aggregation of all subtree sums)
//   - #563 Binary Tree Tilt [new] (post-order subtree sum computation on binary trees)
//   - #416 Partition Equal Subset Sum [done] (equal-sum two-way partitioning on flat arrays via DP)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstdint>
#include <deque>
#include <vector>

namespace {

// Standard binary tree node.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int v, TreeNode* l = nullptr, TreeNode* r = nullptr)
      : val(v), left(l), right(r) {}
};

// RAII arena that owns test TreeNode instances with stable addresses.
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
  // Returns true iff removing one edge partitions the tree into two subtrees
  // with equal node-value sums.
  // Time: O(n), Space: O(n).
  bool checkEqualTree(TreeNode* root) {
    // An empty tree has no edges to remove.
    if (root == nullptr) {
      return false;
    }

    // Collect every subtree sum in post-order so the full tree's sum lands
    // at the back of the vector.
    std::vector<std::int64_t> subtree_sums;
    auto compute_sum = [&subtree_sums](this auto&& self,
                                       const TreeNode* node) -> std::int64_t {
      if (node == nullptr) {
        return 0;
      }
      const std::int64_t left_sum = self(node->left);
      const std::int64_t right_sum = self(node->right);
      const std::int64_t total =
          static_cast<std::int64_t>(node->val) + left_sum + right_sum;
      subtree_sums.push_back(total);
      return total;
    };

    const std::int64_t total_sum = compute_sum(root);

    // Exclude the root's own sum: cutting "above" the root is not a valid
    // edge removal (especially critical when total_sum == 0).
    subtree_sums.pop_back();

    // An odd total sum cannot be split into two equal integer sums.
    if ((total_sum % 2) != 0) {
      return false;
    }

    const std::int64_t target = total_sum / 2;
    return std::ranges::find(subtree_sums, target) != subtree_sums.end();
  }
};

LC_PROBLEM(663, "Equal Tree Partition", "tree", ctx) {
  ctx.test("example1_valid_partition", [&] {
    //     5
    //    / \
    //   10 10
    //     /  \
    //    2    3
    TreeArena arena;
    TreeNode* right = arena.make(10, arena.make(2), arena.make(3));
    TreeNode* root = arena.make(5, arena.make(10), right);
    Solution sol;
    ctx.expect_true(sol.checkEqualTree(root));
  });

  ctx.test("example2_odd_total_cannot_partition", [&] {
    //     1
    //    / \
    //   2  10
    //     /  \
    //    2   20
    TreeArena arena;
    TreeNode* right = arena.make(10, arena.make(2), arena.make(20));
    TreeNode* root = arena.make(1, arena.make(2), right);
    Solution sol;
    ctx.expect_false(sol.checkEqualTree(root));
  });

  ctx.test("zero_total_with_zero_proper_subtree", [&] {
    //   0
    //    \
    //     0
    TreeArena arena;
    TreeNode* root = arena.make(0, nullptr, arena.make(0));
    Solution sol;
    ctx.expect_true(sol.checkEqualTree(root));
  });

  ctx.test("zero_total_only_at_root_must_fail", [&] {
    //    0
    //   / \
    // -1   1
    TreeArena arena;
    TreeNode* root = arena.make(0, arena.make(-1), arena.make(1));
    Solution sol;
    ctx.expect_false(sol.checkEqualTree(root));
  });

  ctx.test("right_skewed_even_total_no_half_subtree", [&] {
    // 2 -> 3 -> 4 -> 5 -> 6 (total = 20, proper sums = 6, 11, 15, 18)
    TreeArena arena;
    TreeNode* n6 = arena.make(6);
    TreeNode* n5 = arena.make(5, nullptr, n6);
    TreeNode* n4 = arena.make(4, nullptr, n5);
    TreeNode* n3 = arena.make(3, nullptr, n4);
    TreeNode* root = arena.make(2, nullptr, n3);
    Solution sol;
    ctx.expect_false(sol.checkEqualTree(root));
  });

  ctx.test("single_node_and_empty_tree", [&] {
    TreeArena arena;
    Solution sol;
    ctx.expect_false(sol.checkEqualTree(nullptr));
    ctx.expect_false(sol.checkEqualTree(arena.make(0)));
  });

  ctx.test("negative_values_valid_split", [&] {
    //    -3
    //   /  \
    // -1   -2
    //      /
    //    -2
    // Total = -8, right subtree sum = -4 == total / 2 -> true.
    TreeArena arena;
    TreeNode* right = arena.make(-2, arena.make(-2), nullptr);
    TreeNode* root = arena.make(-3, arena.make(-1), right);
    Solution sol;
    ctx.expect_true(sol.checkEqualTree(root));
  });
}

}  // namespace
