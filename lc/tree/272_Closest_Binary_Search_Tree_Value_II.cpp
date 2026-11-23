// LeetCode 272: Closest Binary Search Tree Value II
// https://leetcode.com/problems/closest-binary-search-tree-value-ii/
//
// Problem:
//   Given the root of a binary search tree (BST), a floating-point `target`
//   value, and an integer `k` (1 <= k <= n), return the `k` values in the BST
//   that are closest to `target` (in any order). It is guaranteed that there is
//   a unique set of `k` closest values.
//
// Examples:
//   Example 1:
//     Input:  root = [4, 2, 5, 1, 3], target = 3.714286, k = 2
//     Output: [4, 3]
//     Explanation:
//       - Walk down from 4 to 2 to 3 against target = 3.714286, populating
//         pred_stack = [2, 3] (values <= target) and succ_stack = [4] (values >
//         target).
//       - Compare top predecessor 3 (|3 - 3.714286| = 0.714286) with top successor
//         4 (|4 - 3.714286| = 0.285714); append 4 and advance succ_stack by pushing
//         4->right (5).
//       - Compare predecessor 3 (0.714286) with successor 5 (1.285714); append 3 to
//         reach k = 2 values, returning [4, 3] (which sorts to [3, 4]).
//
//   Example 2:
//     Input:  root = [4, 2, 5, 1, 3], target = -10.5, k = 3
//     Output: [1, 2, 3]
//     Explanation:
//       - Walk down the left spine from 4 to 2 to 1; since every node exceeds
//         -10.5, pred_stack stays empty and succ_stack = [4, 2, 1].
//       - Pop successor 1 (no right child) and append 1, then pop successor 2 and
//         push its right child 3 onto succ_stack = [4, 3].
//       - Pop successor 3 and append it to complete k = 3 values, returning [1, 2,
//         3].
//
//   Example 3:
//     Input:  root = [4, 2, 5, 1, 3], target = 3.0, k = 3
//     Output: [3, 2, 4]
//     Explanation:
//       - Partition the search path for 3.0 into pred_stack = [2, 3] and succ_stack
//         = [4].
//       - Pick 3 first since |3 - 3.0| = 0 <= |4 - 3.0| = 1, then on the tie |2 -
//         3.0| = 1 <= |4 - 3.0| = 1 pick 2 and push 2->left (1) onto pred_stack.
//       - Compare predecessor 1 (|1 - 3.0| = 2) against successor 4 (|4 - 3.0| = 1)
//         and append 4, returning [3, 2, 4] (which sorts to [2, 3, 4]).
//
// Algorithm:
//   1. Lazy Two-Stack Predecessor/Successor Search (primary, answers follow-up):
//      Maintain two BST iterator stacks initialized along the single root-to-
//      leaf search path for `target` in O(h) time:
//        - `pred_stack`: ancestors with `val <= target`, top is the immediate
//          predecessor (largest value <= target). Advancing pops the top node
//          and pushes the rightmost spine of its left subtree.
//        - `succ_stack`: ancestors with `val > target`, top is the immediate
//          successor (smallest value > target). Advancing pops the top node and
//          pushes the leftmost spine of its right subtree.
//      Repeat `k` times: compare `|pred - target|` vs `|succ - target|` and
//      emit + advance the closer iterator. Each tree edge is traversed at most
//      once across all `k` advances.
//      Time Complexity:  O(h + k) — O(log n + k) on a balanced BST, strictly
//                        faster than O(n) when k << n.
//      Space Complexity: O(h) for the two iterator stacks.
//   2. Iterative Inorder Sliding Window (secondary):
//      Traverse the BST in ascending inorder with one stack while maintaining a
//      `std::deque<int>` window of at most `k` values. Once the window has `k`
//      elements, if the new node is closer to `target` than `window.front()`,
//      evict the front and push the new node; otherwise stop immediately
//      because all subsequent inorder values are even farther from `target`.
//      Time Complexity:  O(n) worst case.
//      Space Complexity: O(h + k).
//
// Data Structures:
//   - Two `std::vector<const TreeNode*>` BST iterator stacks (`pred_stack` and
//     `succ_stack`) implementing lazy bidirectional BST iteration.
//   - `std::deque<int>` sliding window + `std::vector<const TreeNode*>` inorder
//     stack for the single-pass alternative.
//
// Real-World Applications:
//   - 1D k-nearest-neighbor (k-NN) range queries in ordered database indexes
//     and time-series engines (e.g., finding the k timestamps closest to an
//     event).
//   - Order-book price-level matching in financial trading systems (selecting
//     the k closest bid/ask levels around a mid-price).
//   - Autocomplete / nearest-bucket lookup in calibrated sensor histograms.
//
// Related LeetCode Problems:
//   - #270 Closest Binary Search Tree Value [done] (k = 1 special case in O(h) time)
//   - #658 Find K Closest Elements [done] (k closest values in a sorted array)
//   - #173 Binary Search Tree Iterator [done] (controlled stack-based lazy BST iteration)
//   - #235 Lowest Common Ancestor Of A Binary Search Tree [done] (O(h) BST search-path pruning)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <deque>
#include <vector>

namespace {

struct TreeNode {
  int val;
  TreeNode* left;
  TreeNode* right;

  explicit TreeNode(int v, TreeNode* l = nullptr, TreeNode* r = nullptr)
      : val(v), left(l), right(r) {}
};

class Solution {
 public:
  // Finds the `k` closest BST values to `target` using lazy predecessor and
  // successor iterator stacks.
  // Time: O(h + k), Space: O(h).
  std::vector<int> closestKValues(const TreeNode* root, double target, int k) {
    if (root == nullptr || k <= 0) {
      return {};
    }

    // Phase 1: Walk down from `root` toward `target`, partitioning path nodes
    // into `pred_stack` (values <= target) and `succ_stack` (values > target).
    std::vector<const TreeNode*> pred_stack;
    std::vector<const TreeNode*> succ_stack;
    const TreeNode* curr = root;
    while (curr != nullptr) {
      if (static_cast<double>(curr->val) <= target) {
        pred_stack.push_back(curr);
        curr = curr->right;
      } else {
        succ_stack.push_back(curr);
        curr = curr->left;
      }
    }

    // Phase 2: Greedily pick whichever iterator top is closer to `target` and
    // lazily advance that iterator, up to `k` values.
    const auto limit = static_cast<std::size_t>(k);
    std::vector<int> result;
    result.reserve(limit);
    while (result.size() < limit &&
           (!pred_stack.empty() || !succ_stack.empty())) {
      if (pred_stack.empty()) {
        result.push_back(pop_successor(succ_stack));
      } else if (succ_stack.empty()) {
        result.push_back(pop_predecessor(pred_stack));
      } else {
        const double pred_diff =
            std::fabs(static_cast<double>(pred_stack.back()->val) - target);
        const double succ_diff =
            std::fabs(static_cast<double>(succ_stack.back()->val) - target);
        if (pred_diff <= succ_diff) {
          result.push_back(pop_predecessor(pred_stack));
        } else {
          result.push_back(pop_successor(succ_stack));
        }
      }
    }

    return result;
  }

  // Finds the `k` closest BST values via single-stack inorder traversal with a
  // sliding deque window of size `k` and early termination.
  // Time: O(n), Space: O(h + k).
  std::vector<int> closestKValuesSlidingWindow(const TreeNode* root,
                                               double target, int k) {
    if (root == nullptr || k <= 0) {
      return {};
    }

    const auto limit = static_cast<std::size_t>(k);
    std::deque<int> window;
    std::vector<const TreeNode*> stack;

    // Seed the inorder stack with the leftmost spine.
    const TreeNode* curr = root;
    while (curr != nullptr) {
      stack.push_back(curr);
      curr = curr->left;
    }

    while (!stack.empty()) {
      const TreeNode* node = stack.back();
      stack.pop_back();

      if (window.size() < limit) {
        window.push_back(node->val);
      } else if (std::fabs(static_cast<double>(window.front()) - target) >
                 std::fabs(static_cast<double>(node->val) - target)) {
        // Because inorder traversal is strictly increasing, sliding the window
        // right strictly improves closeness to `target`.
        window.pop_front();
        window.push_back(node->val);
      } else {
        // Early exit: all future inorder values are >= `node->val` and cannot
        // beat `window.front()`.
        break;
      }

      // Advance inorder iterator into `node->right`'s leftmost spine.
      const TreeNode* right_curr = node->right;
      while (right_curr != nullptr) {
        stack.push_back(right_curr);
        right_curr = right_curr->left;
      }
    }

    return {window.begin(), window.end()};
  }

 private:
  // Pops the current predecessor (largest remaining value <= target) and
  // advances `pred_stack` to the next smaller BST node.
  static int pop_predecessor(std::vector<const TreeNode*>& pred_stack) {
    const TreeNode* node = pred_stack.back();
    pred_stack.pop_back();
    const TreeNode* next = node->left;
    while (next != nullptr) {
      pred_stack.push_back(next);
      next = next->right;
    }
    return node->val;
  }

  // Pops the current successor (smallest remaining value > target) and
  // advances `succ_stack` to the next larger BST node.
  static int pop_successor(std::vector<const TreeNode*>& succ_stack) {
    const TreeNode* node = succ_stack.back();
    succ_stack.pop_back();
    const TreeNode* next = node->right;
    while (next != nullptr) {
      succ_stack.push_back(next);
      next = next->left;
    }
    return node->val;
  }
};

// Returns a sorted copy of `vals` for order-independent comparison.
std::vector<int> sorted_copy(std::vector<int> vals) {
  std::ranges::sort(vals);
  return vals;
}

LC_PROBLEM(272, "Closest Binary Search Tree Value II", "tree", ctx) {
  Solution sol;

  // Shared sample BST:
  //       4
  //      / \
  //     2   5
  //    / \
  //   1   3
  auto build_sample = [](std::deque<TreeNode>& arena) {
    auto make = [&](int v) -> TreeNode* { return &arena.emplace_back(v); };
    TreeNode* root = make(4);
    root->left = make(2);
    root->right = make(5);
    root->left->left = make(1);
    root->left->right = make(3);
    return root;
  };

  ctx.test("leetcode_example_and_source_build", [&] {
    std::deque<TreeNode> arena;
    TreeNode* root = build_sample(arena);

    const std::vector<int> expected = {3, 4};
    ctx.expect_eq(sorted_copy(sol.closestKValues(root, 3.714286, 2)), expected);
    ctx.expect_eq(
        sorted_copy(sol.closestKValuesSlidingWindow(root, 3.714286, 2)),
        expected);

    // Target from source build(): 3.14, k = 2.
    ctx.expect_eq(sorted_copy(sol.closestKValues(root, 3.14, 2)), expected);
    ctx.expect_eq(sorted_copy(sol.closestKValuesSlidingWindow(root, 3.14, 2)),
                  expected);
  });

  ctx.test("target_outside_tree_bounds", [&] {
    std::deque<TreeNode> arena;
    TreeNode* root = build_sample(arena);

    // Target smaller than minimum node.
    const std::vector<int> exp_low = {1, 2, 3};
    ctx.expect_eq(sorted_copy(sol.closestKValues(root, -10.5, 3)), exp_low);
    ctx.expect_eq(sorted_copy(sol.closestKValuesSlidingWindow(root, -10.5, 3)),
                  exp_low);

    // Target larger than maximum node.
    const std::vector<int> exp_high = {4, 5};
    ctx.expect_eq(sorted_copy(sol.closestKValues(root, 99.9, 2)), exp_high);
    ctx.expect_eq(sorted_copy(sol.closestKValuesSlidingWindow(root, 99.9, 2)),
                  exp_high);
  });

  ctx.test("exact_hit_and_full_tree", [&] {
    std::deque<TreeNode> arena;
    TreeNode* root = build_sample(arena);

    const std::vector<int> exp_exact = {2, 3, 4};
    ctx.expect_eq(sorted_copy(sol.closestKValues(root, 3.0, 3)), exp_exact);
    ctx.expect_eq(sorted_copy(sol.closestKValuesSlidingWindow(root, 3.0, 3)),
                  exp_exact);

    const std::vector<int> exp_all = {1, 2, 3, 4, 5};
    ctx.expect_eq(sorted_copy(sol.closestKValues(root, 2.2, 5)), exp_all);
    ctx.expect_eq(sorted_copy(sol.closestKValuesSlidingWindow(root, 2.2, 5)),
                  exp_all);
  });

  ctx.test("edge_cases_single_empty_zero_k", [&] {
    TreeNode single(1);
    ctx.expect_eq(sol.closestKValues(&single, 0.0, 1), std::vector<int>{1});
    ctx.expect_eq(sol.closestKValuesSlidingWindow(&single, 0.0, 1),
                  std::vector<int>{1});

    ctx.expect_eq(sol.closestKValues(nullptr, 1.0, 1), std::vector<int>{});
    ctx.expect_eq(sol.closestKValuesSlidingWindow(nullptr, 1.0, 1),
                  std::vector<int>{});
    ctx.expect_eq(sol.closestKValues(&single, 1.0, 0), std::vector<int>{});
    ctx.expect_eq(sol.closestKValuesSlidingWindow(&single, 1.0, 0),
                  std::vector<int>{});
  });
}

}  // namespace
