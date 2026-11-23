// LeetCode 958: Check Completeness Of A Binary Tree
// https://leetcode.com/problems/check-completeness-of-a-binary-tree/
//
// Problem:
//   Given the root of a binary tree, determine whether it is a complete binary
//   tree. In a complete binary tree, every level except possibly the last is
//   completely filled, and all nodes on the last level are packed as far left
//   as possible (between 1 and 2^h nodes inclusive at the final depth h).
//   The tree contains between 1 and 100 nodes.
//
// Algorithm:
//   Perform a standard breadth-first (level-order) traversal that explicitly
//   enqueues both left and right child pointers, including nullptr children.
//   Key invariant: a binary tree is complete if and only if, in level-order
//   sequence, no non-null node ever appears after the first nullptr slot.
//   Track a boolean flag `seen_null`: once a nullptr is popped from the queue,
//   set `seen_null = true` and stop enqueuing its children; if any subsequent
//   popped entry is non-null, the tree has a gap and is not complete.
//   Time Complexity:  O(N), where N is the number of nodes (each node and its
//                     two child slots are visited at most once).
//   Space Complexity: O(N) for the BFS queue (O(W) where W <= N/2 + 1 is the
//                     maximum tree width).
//
// Data Structures:
//   - std::queue<const TreeNode*>: FIFO queue driving level-order traversal,
//     storing both valid node pointers and nullptr sentinels to detect gaps.
//
// Real-World Applications:
//   - Validating array-backed binary heaps and priority queues (which require
//     complete binary tree layout so child indices 2i+1 and 2i+2 stay dense).
//   - Verifying balanced tournament trees and fixed-stride memory layouts in
//     embedded and GPU-friendly spatial/search trees.
//   - Integrity checking serialized heap snapshots during crash recovery.
//
// Related LeetCode Problems:
//   - #102 Binary Tree Level Order Traversal [done] (foundational BFS template)
//   - #222 Count Complete Tree Nodes [done] (exploits completeness for O(log^2 N) counting)
//   - #662 Maximum Width of Binary Tree [done] (level-order index numbering)
//   - #919 Complete Binary Tree Inserter [done] (maintaining completeness on inserts)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <optional>
#include <queue>
#include <vector>

namespace {

// Standard LeetCode binary tree node.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  TreeNode() = default;
  explicit TreeNode(int x) : val(x) {}
  TreeNode(int x, TreeNode* l, TreeNode* r) : val(x), left(l), right(r) {}
};

// RAII arena that owns test TreeNode instances via std::deque (pointer-stable
// on emplace_back) so tests have zero heap leaks.
class TreeArena {
 public:
  // Allocates a single TreeNode owned by this arena.
  [[nodiscard]] TreeNode* make(int val) {
    nodes_.emplace_back(val);
    return &nodes_.back();
  }

  // Builds a binary tree from a LeetCode level-order array (std::nullopt = null).
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
  // Returns true iff the binary tree rooted at `root` is complete.
  // Invariant: in level-order traversal including null children, once a null
  // slot is observed, every remaining queued entry must also be null.
  // Time: O(N), Space: O(N).
  bool isCompleteTree(const TreeNode* root) {
    if (root == nullptr) {
      return true;
    }

    std::queue<const TreeNode*> q;
    q.push(root);
    bool seen_null = false;

    while (!q.empty()) {
      const auto* node = q.front();
      q.pop();

      if (node == nullptr) {
        // First (or subsequent) missing slot: no real node may follow.
        seen_null = true;
        continue;
      }

      if (seen_null) {
        // A non-null node appeared after a gap on the same or prior level.
        return false;
      }

      // Push both children (even if null) to preserve exact level-order slots.
      q.push(node->left);
      q.push(node->right);
    }

    return true;
  }
};

LC_PROBLEM(958, "Check Completeness Of A Binary Tree", "tree", ctx) {
  ctx.test("example_1_complete_tree", [&] {
    TreeArena arena;
    const TreeNode* root = arena.build({1, 2, 3, 4, 5, 6});
    Solution sol;
    ctx.expect_true(sol.isCompleteTree(root));
  });

  ctx.test("example_2_gap_in_last_level", [&] {
    TreeArena arena;
    const TreeNode* root = arena.build({1, 2, 3, 4, 5, std::nullopt, 7});
    Solution sol;
    ctx.expect_false(sol.isCompleteTree(root));
  });

  ctx.test("source_build_missing_middle_leaves", [&] {
    // Matches the Python build() fixture:
    //       1
    //      / \
    //     2   3
    //    /     \
    //   4       5
    TreeArena arena;
    TreeNode* root = arena.make(1);
    root->left = arena.make(2);
    root->right = arena.make(3);
    root->left->left = arena.make(4);
    root->right->right = arena.make(5);
    Solution sol;
    ctx.expect_false(sol.isCompleteTree(root));
  });

  ctx.test("single_node_and_empty", [&] {
    TreeArena arena;
    const TreeNode* single = arena.make(1);
    Solution sol;
    ctx.expect_true(sol.isCompleteTree(single));
    ctx.expect_true(sol.isCompleteTree(nullptr));
  });

  ctx.test("left_only_vs_right_only_child", [&] {
    TreeArena arena;
    const TreeNode* left_only = arena.build({1, 2});
    const TreeNode* right_only = arena.build({1, std::nullopt, 2});
    Solution sol;
    ctx.expect_true(sol.isCompleteTree(left_only));
    ctx.expect_false(sol.isCompleteTree(right_only));
  });

  ctx.test("perfect_binary_tree_and_missing_leftmost_leaf", [&] {
    TreeArena arena;
    const TreeNode* perfect = arena.build({1, 2, 3, 4, 5, 6, 7});
    const TreeNode* missing_leftmost = arena.build({1, 2, 3, std::nullopt, 5});
    Solution sol;
    ctx.expect_true(sol.isCompleteTree(perfect));
    ctx.expect_false(sol.isCompleteTree(missing_leftmost));
  });
}

}  // namespace
