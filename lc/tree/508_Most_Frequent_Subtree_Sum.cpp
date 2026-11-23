// LeetCode 508: Most Frequent Subtree Sum
// https://leetcode.com/problems/most-frequent-subtree-sum/
//
// Problem:
//   Given the root of a binary tree, return the most frequent subtree sum. The
//   subtree sum of a node is the sum of all node values formed by the subtree
//   rooted at that node (including the node itself). If multiple sums share
//   the highest frequency, return all of them in any order.
//
// Algorithm:
//   Perform a single post-order DFS traversal to compute each node's subtree
//   sum from the bottom up: subtree_sum(u) = u->val + subtree_sum(u->left) +
//   subtree_sum(u->right). As each subtree sum is computed, increment its
//   frequency in a hash map and maintain a running maximum frequency. After
//   the traversal completes, iterate once over the frequency table and collect
//   every sum whose count equals the maximum frequency.
//   Time Complexity: O(n), visiting each of the n nodes once and scanning at
//   most n distinct sums.
//   Space Complexity: O(n), for the hash map storing up to n subtree sums plus
//   O(h) recursion stack space (h = tree height).
//
// Data Structures:
//   - Binary tree (`TreeNode`) traversed via post-order recursion.
//   - Hash map (`std::unordered_map<int, int>`) mapping each subtree sum to
//     its occurrence count.
//   - Dynamic array (`std::vector<int>`) collecting all modal subtree sums.
//
// Real-World Applications:
//   - Profiling and cost aggregation in hierarchical call trees or flame
//     graphs to identify repeated resource footprints across sub-components.
//   - Detecting recurring directory size or budget rollups across
//     organizational / filesystem hierarchies.
//   - Subexpression or AST metric aggregation in compiler static analysis.
//
// Related LeetCode Problems:
//   - #124 Binary Tree Maximum Path Sum [done] (post-order subtree sum aggregation)
//   - #563 Binary Tree Tilt [new] (post-order subtree sum computation)
//   - #1120 Maximum Average Subtree [done] (post-order subtree sum and size rollup)
//   - #1339 Maximum Product of Splitted Binary Tree [new] (uses all subtree sums via post-order DFS)

#include "framework/lc_framework.h"

#include <algorithm>
#include <deque>
#include <unordered_map>
#include <vector>

namespace {

// Standard binary tree node used by LeetCode tree problems.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;
};

class Solution {
 public:
  // Computes all subtree sums via post-order DFS and returns those occurring
  // with the highest frequency (in arbitrary order).
  // Time: O(n), Space: O(n).
  std::vector<int> findFrequentTreeSum(const TreeNode* root) {
    if (root == nullptr) {
      return {};
    }

    // Map each subtree sum to how many nodes produce it, tracking the peak
    // frequency online so a single pass over the map suffices afterward.
    std::unordered_map<int, int> freq;
    int max_freq = 0;

    // Post-order DFS: returns the sum of all values in the subtree rooted at
    // `node`, recording its frequency once both children have been summed.
    auto compute_sum = [&](this auto&& self, const TreeNode* node) -> int {
      if (node == nullptr) {
        return 0;
      }

      const int left_sum = self(node->left);
      const int right_sum = self(node->right);
      const int subtree_sum = node->val + left_sum + right_sum;

      // Increment frequency and update global maximum frequency in O(1) avg.
      const int count = ++freq[subtree_sum];
      max_freq = std::max(max_freq, count);
      return subtree_sum;
    };

    compute_sum(root);

    // Gather every subtree sum that achieved the maximum frequency.
    std::vector<int> result;
    for (const auto& [sum, count] : freq) {
      if (count == max_freq) {
        result.push_back(sum);
      }
    }
    return result;
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

LC_PROBLEM(508, "Most Frequent Subtree Sum", "tree", ctx) {
  Solution sol;

  ctx.test("example_1_all_tied", [&] {
    //     5
    //    / \
    //   2  -3
    // Subtree sums: 2, -3, 4 (each occurs once).
    TreeArena arena;
    TreeNode* root = arena.make(5, arena.make(2), arena.make(-3));
    std::vector<int> got = sol.findFrequentTreeSum(root);
    std::ranges::sort(got);
    ctx.expect_eq(got, std::vector<int>{-3, 2, 4});
  });

  ctx.test("example_2_unique_mode", [&] {
    //     5
    //    / \
    //   2  -5
    // Subtree sums: 2 (twice), -5 (once).
    TreeArena arena;
    TreeNode* root = arena.make(5, arena.make(2), arena.make(-5));
    std::vector<int> got = sol.findFrequentTreeSum(root);
    std::ranges::sort(got);
    ctx.expect_eq(got, std::vector<int>{2});
  });

  ctx.test("single_node", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(-7);
    ctx.expect_eq(sol.findFrequentTreeSum(root), std::vector<int>{-7});
  });

  ctx.test("empty_tree", [&] {
    ctx.expect_eq(sol.findFrequentTreeSum(nullptr), std::vector<int>{});
  });

  ctx.test("multiple_tied_modes_in_deeper_tree", [&] {
    //        0
    //       / \
    //      1  -1
    //     / \ / \
    //    0  0 0  0
    // Subtree sums: four 0-leaves, left=1, right=-1, root=0 -> 0 occurs 5x.
    TreeArena arena;
    TreeNode* left = arena.make(1, arena.make(0), arena.make(0));
    TreeNode* right = arena.make(-1, arena.make(0), arena.make(0));
    TreeNode* root = arena.make(0, left, right);
    ctx.expect_eq(sol.findFrequentTreeSum(root), std::vector<int>{0});
  });

  ctx.test("two_modes_with_frequency_two", [&] {
    //          100
    //         /   \
    //       10     20
    //      /  \   /  \
    //     1    2 1    2
    // Subtree sums: 1 (x2), 2 (x2), 13 (x1), 23 (x1), 136 (x1).
    TreeArena arena;
    TreeNode* left = arena.make(10, arena.make(1), arena.make(2));
    TreeNode* right = arena.make(20, arena.make(1), arena.make(2));
    TreeNode* root = arena.make(100, left, right);
    std::vector<int> got = sol.findFrequentTreeSum(root);
    std::ranges::sort(got);
    ctx.expect_eq(got, std::vector<int>{1, 2});
  });
}

}  // namespace
