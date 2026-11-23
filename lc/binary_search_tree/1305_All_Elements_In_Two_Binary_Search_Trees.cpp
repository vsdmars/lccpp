// LeetCode 1305: All Elements In Two Binary Search Trees
// https://leetcode.com/problems/all-elements-in-two-binary-search-trees/
//
// Problem:
//   Given the roots `root1` and `root2` of two binary search trees (BSTs),
//   return a list containing all integer values from both trees sorted in
//   non-decreasing (ascending) order. Either or both trees may be empty, and
//   duplicate values may appear across the two trees.
//
// Examples:
//   Example 1:
//     Input:  root1 = [2, 1, 4], root2 = [1, 0, 3]
//     Output: [0, 1, 1, 2, 3, 4]
//     Explanation:
//       - InorderIterator it1 initializes its stack with root1's left spine [2, 1]
//         (peek = 1), while it2 initializes with root2's left spine [1, 0] (peek =
//         0).
//       - The merge loop compares peek values: it pops 0 from it2, then pops 1 from
//         it1 (1 <= 1), then pops 1 from it2 and pushes its right child 3.
//       - Comparing remaining peeks, it pops 2 from it1 and pushes right child 4,
//         then pops 3 from it2 (3 < 4), exhausting it2.
//       - The drain loop pops the leftover 4 from it1, returning [0, 1, 1, 2, 3,
//         4].
//
//   Example 2:
//     Input:  root1 = [0, -10, 10], root2 = [5, 1, 7, 0, 2]
//     Output: [-10, 0, 0, 1, 2, 5, 7, 10]
//     Explanation:
//       - it1 starts with left spine [0, -10] (peek = -10) and it2 starts with left
//         spine [5, 1, 0] (peek = 0).
//       - Because -10 < 0, it1 pops -10; then 0 <= 0 pops 0 from it1 (pushing right
//         child 10 onto it1) before 0 < 10 pops 0 from it2.
//       - While it1.peek() stays at 10, it2 yields 1, pushes and yields 2, yields
//         5, and pushes and yields 7, exhausting it2.
//       - The drain loop emits 10 from it1, producing [-10, 0, 0, 1, 2, 5, 7, 10].
//
//   Example 3:
//     Input:  root1 = [], root2 = [5, 1, 7, 0, 2]
//     Output: [0, 1, 2, 5, 7]
//     Explanation:
//       - it1 is constructed on nullptr with an empty stack (has_next() is false),
//         while it2 pushes [5, 1, 0].
//       - The two-way merge loop is skipped immediately because it1.has_next() is
//         false.
//       - The drain loop for it2 yields its in-order sequence [0, 1, 2, 5, 7]
//         without extra allocations.
//
// Algorithm:
//   Streaming Two-Way Merge via Lazy Iterative In-Order Iterators:
//   Mirror the Python lazy-generator + merge approach without materializing
//   intermediate per-tree sorted arrays:
//     1. Wrap each BST in a lightweight `InorderIterator` that maintains only
//        the current left spine on an explicit stack. Its `peek()` returns the
//        smallest unconsumed value in O(1), and `next()` advances to the next
//        in-order element in amortized O(1).
//     2. Repeatedly compare `it1.peek()` and `it2.peek()`, appending the
//        smaller value to `result` and advancing that iterator.
//     3. Drain whichever iterator still has remaining elements.
//   Time Complexity: O(N1 + N2) — each node is pushed and popped once.
//   Space Complexity: O(H1 + H2) auxiliary iterator stack space (excluding the
//   O(N1 + N2) output vector), where H1 and H2 are the heights of the trees.
//
// Data Structures:
//   - Lazy BST In-Order Iterator (`InorderIterator` backed by
//     `std::vector<const TreeNode*>`): yields elements of each BST on demand in
//     sorted order using O(H) memory.
//   - Output Vector (`std::vector<int>`): collects the merged sorted stream.
//
// Real-World Applications:
//   - LSM-tree (RocksDB / LevelDB) memtable and SSTable compaction merging
//     sorted key iterators from multiple ordered trees in a single linear pass.
//   - Database sort-merge joins combining two B-tree index scans without
//     buffering either relation completely in memory.
//   - Distributed log aggregation merging chronologically ordered event
//     streams from multiple shards.
//
// Related LeetCode Problems:
//   - #173 Binary Search Tree Iterator [done] (defines the exact O(H)-space lazy BST in-order iterator used here)
//   - #88 Merge Sorted Array [done] (two-pointer merge of two sorted sequences)
//   - #23 Merge k Sorted Lists [done] (generalizes two-way stream merging to k sorted iterators with a min-heap)
//   - #897 Increasing Order Search Tree [done] (iterative left-spine stack traversal of a BST)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <optional>
#include <queue>
#include <vector>

namespace {

// Standard binary tree node for LeetCode tree problems.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int x) : val(x) {}
};

// Lazy in-order iterator over a BST using O(H) stack space and amortized O(1)
// advancement, analogous to a Python generator yielding keys in sorted order.
class InorderIterator {
 public:
  explicit InorderIterator(const TreeNode* root) { push_left_spine(root); }

  [[nodiscard]] bool has_next() const noexcept { return !stack_.empty(); }

  // Returns the current smallest unconsumed key without advancing.
  // Precondition: has_next() is true.
  [[nodiscard]] int peek() const { return stack_.back()->val; }

  // Consumes and returns the current smallest key, then advances to its
  // in-order successor. Precondition: has_next() is true.
  int next() {
    const TreeNode* node = stack_.back();
    stack_.pop_back();
    push_left_spine(node->right);
    return node->val;
  }

 private:
  void push_left_spine(const TreeNode* node) {
    while (node != nullptr) {
      stack_.push_back(node);
      node = node->left;
    }
  }

  std::vector<const TreeNode*> stack_;
};

class Solution {
 public:
  // Merges all elements of `root1` and `root2` in ascending order using two
  // synchronized lazy in-order iterators.
  // Time: O(N1 + N2), Space: O(H1 + H2) auxiliary (+ O(N1 + N2) output).
  std::vector<int> getAllElements(const TreeNode* root1,
                                  const TreeNode* root2) {
    std::vector<int> merged;
    InorderIterator it1(root1);
    InorderIterator it2(root2);

    // Classic two-way merge while both BST streams have remaining elements.
    while (it1.has_next() && it2.has_next()) {
      if (it1.peek() <= it2.peek()) {
        merged.push_back(it1.next());
      } else {
        merged.push_back(it2.next());
      }
    }

    // Drain any leftover elements from whichever tree has not finished.
    while (it1.has_next()) {
      merged.push_back(it1.next());
    }
    while (it2.has_next()) {
      merged.push_back(it2.next());
    }

    return merged;
  }
};

// RAII arena owning test TreeNode instances to guarantee zero memory leaks.
class TreeArena {
 public:
  // Builds a binary tree from LeetCode's level-order vector (std::nullopt =
  // missing child).
  TreeNode* build(const std::vector<std::optional<int>>& level_order) {
    if (level_order.empty() || !level_order[0].has_value()) {
      return nullptr;
    }

    TreeNode* root = create(*level_order[0]);
    std::queue<TreeNode*> pending;
    pending.push(root);

    std::size_t idx = 1uz;
    while (!pending.empty() && idx < level_order.size()) {
      TreeNode* parent = pending.front();
      pending.pop();

      if (level_order[idx].has_value()) {
        parent->left = create(*level_order[idx]);
        pending.push(parent->left);
      }
      ++idx;

      if (idx < level_order.size() && level_order[idx].has_value()) {
        parent->right = create(*level_order[idx]);
        pending.push(parent->right);
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

LC_PROBLEM(1305, "All Elements In Two Binary Search Trees",
           "binary_search_tree", ctx) {
  Solution sol;

  ctx.test("example_1_from_python_build", [&] {
    TreeArena arena;
    TreeNode* root1 = arena.build({2, 1, 4});
    TreeNode* root2 = arena.build({1, 0, 3});
    ctx.expect_eq(sol.getAllElements(root1, root2),
                  std::vector<int>{0, 1, 1, 2, 3, 4});
  });

  ctx.test("example_2_negative_and_duplicate_values", [&] {
    TreeArena arena;
    TreeNode* root1 = arena.build({0, -10, 10});
    TreeNode* root2 = arena.build({5, 1, 7, 0, 2});
    ctx.expect_eq(sol.getAllElements(root1, root2),
                  std::vector<int>{-10, 0, 0, 1, 2, 5, 7, 10});
  });

  ctx.test("example_3_and_example_4_one_tree_empty", [&] {
    TreeArena arena;
    TreeNode* root2 = arena.build({5, 1, 7, 0, 2});
    ctx.expect_eq(sol.getAllElements(nullptr, root2),
                  std::vector<int>{0, 1, 2, 5, 7});

    TreeNode* root1 = arena.build({0, -10, 10});
    ctx.expect_eq(sol.getAllElements(root1, nullptr),
                  std::vector<int>{-10, 0, 10});
  });

  ctx.test("example_5_identical_keys_skewed", [&] {
    TreeArena arena;
    TreeNode* root1 = arena.build({1, std::nullopt, 8});
    TreeNode* root2 = arena.build({8, 1});
    ctx.expect_eq(sol.getAllElements(root1, root2),
                  std::vector<int>{1, 1, 8, 8});
  });

  ctx.test("both_empty_and_disjoint_ranges", [&] {
    TreeArena arena;
    ctx.expect_eq(sol.getAllElements(nullptr, nullptr), std::vector<int>{});

    TreeNode* low_tree = arena.build({-5, -10, -1});
    TreeNode* high_tree = arena.build({50, 20, 100});
    ctx.expect_eq(sol.getAllElements(low_tree, high_tree),
                  std::vector<int>{-10, -5, -1, 20, 50, 100});
  });
}

}  // namespace
