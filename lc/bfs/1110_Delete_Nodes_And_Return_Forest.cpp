// LeetCode 1110: Delete Nodes And Return Forest
// https://leetcode.com/problems/delete-nodes-and-return-forest/
//
// Problem:
//   Given the `root` of a binary tree where every node has a distinct integer
//   value, and a list `to_delete` of node values to remove, delete every node
//   whose value is in `to_delete`. The remaining nodes form a forest (a
//   disjoint union of trees). Return the roots of all trees in the remaining
//   forest in any order.
//   Constraints: 1 <= number of nodes <= 1000, distinct values in [1, 1000],
//   to_delete.size() <= 1000 with distinct values in [1, 1000].
//
// Algorithm:
//   Two clean O(N) approaches are provided:
//   1. Top-Down BFS with Parent Tracking (primary, `delNodes`):
//      Store `to_delete` in a hash set for O(1) membership queries. Traverse
//      every node via a queue storing `(node, parent, side)`:
//      - If `root` is not deleted, it becomes a forest root.
//      - When visiting a node scheduled for deletion: sever its incoming link
//        from `parent` (`parent->left = nullptr` or `parent->right = nullptr`),
//        promote any non-null, non-deleted child to a new forest root, and
//        detach the deleted node's own child pointers after queueing those
//        children so deeper deletions in their subtrees are still processed.
//   2. Bottom-Up Post-Order DFS (`delNodesDfs`):
//      Recursively prune left and right subtrees first so child links are
//      updated cleanly. If the current node is deleted, push any surviving
//      left/right child roots to `forest` and return `nullptr` to its parent.
//   Time Complexity:  O(N + D) — where N is the number of tree nodes and D is
//                     `to_delete.size()`.
//   Space Complexity: O(N + D) — hash set plus BFS queue or recursion stack.
//
// Data Structures:
//   - Hash set (`std::unordered_set<int>`): O(1) deletion lookup per node.
//   - FIFO queue (`std::queue<BfsItem>`): carries `(node, parent, side)` state
//     for BFS pointer rewiring without extra parent maps.
//   - Result vector (`std::vector<TreeNode*>`): collects surviving forest
//     roots.
//
// Real-World Applications:
//   - Version-control & dependency pruning: removing deprecated or revoked
//     nodes from a hierarchy and enumerating the resulting independent
//     sub-hierarchies.
//   - Distributed cluster / spanning-tree partitioning: determining the
//     surviving connected subtrees after a set of intermediate routers fail.
//   - UI DOM / scene-graph editing: detaching masked containers while
//     promoting unmasked descendants to top-level roots.
//
// Related LeetCode Problems:
//   - #102 Binary Tree Level Order Traversal [done] (BFS traversal of a binary tree)
//   - #450 Delete Node in a BST [done] (node deletion and parent-child pointer rewiring in a binary tree)
//   - #814 Binary Tree Pruning [done] (post-order pruning of subtrees based on node conditions)
//   - #1325 Delete Leaves With a Given Value [new] (bottom-up recursive deletion of matching tree nodes)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <queue>
#include <unordered_set>
#include <utility>
#include <vector>

namespace {

struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int x) : val(x) {}
};

// RAII arena that owns every TreeNode created in tests, preventing leaks even
// after `delNodes` severs links between deleted and surviving nodes.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    nodes_.emplace_back(val);
    auto* node = &nodes_.back();
    node->left = left;
    node->right = right;
    return node;
  }

 private:
  std::deque<TreeNode> nodes_;
};

enum class ChildSide : std::uint8_t { kNone, kLeft, kRight };

struct BfsItem {
  TreeNode* node = nullptr;
  TreeNode* parent = nullptr;
  ChildSide side = ChildSide::kNone;

  BfsItem(TreeNode* n, TreeNode* p, ChildSide s)
      : node(n), parent(p), side(s) {}
};

class Solution {
 public:
  // Primary BFS solution: deletes all nodes whose values appear in `to_delete`
  // and returns the surviving forest roots.
  // Time: O(N + D), Space: O(N + D).
  std::vector<TreeNode*> delNodes(TreeNode* root,
                                  const std::vector<int>& to_delete) {
    if (root == nullptr) {
      return {};
    }

    const std::unordered_set<int> delete_set(to_delete.begin(),
                                             to_delete.end());
    std::vector<TreeNode*> forest;
    if (!delete_set.contains(root->val)) {
      forest.push_back(root);
    }

    std::queue<BfsItem> frontier;
    frontier.emplace(root, nullptr, ChildSide::kNone);

    while (!frontier.empty()) {
      const BfsItem item = frontier.front();
      frontier.pop();

      TreeNode* node = item.node;
      TreeNode* left_child = node->left;
      TreeNode* right_child = node->right;

      // Always enqueue children before potentially severing `node`'s child
      // links so deletions deeper in the tree are still processed.
      if (left_child != nullptr) {
        frontier.emplace(left_child, node, ChildSide::kLeft);
      }
      if (right_child != nullptr) {
        frontier.emplace(right_child, node, ChildSide::kRight);
      }

      if (delete_set.contains(node->val)) {
        // Disconnect this deleted node from its parent.
        detachFromParent(item.parent, item.side);

        // Any surviving direct child loses its parent and becomes a new root.
        if (left_child != nullptr && !delete_set.contains(left_child->val)) {
          forest.push_back(left_child);
        }
        if (right_child != nullptr && !delete_set.contains(right_child->val)) {
          forest.push_back(right_child);
        }

        // Sever outgoing pointers on the deleted node itself for cleanliness.
        node->left = nullptr;
        node->right = nullptr;
      }
    }

    return forest;
  }

  // Alternative post-order DFS solution: prunes children bottom-up and emits
  // orphaned surviving children whenever their parent is deleted.
  // Time: O(N + D), Space: O(H + D).
  std::vector<TreeNode*> delNodesDfs(TreeNode* root,
                                     const std::vector<int>& to_delete) {
    const std::unordered_set<int> delete_set(to_delete.begin(),
                                             to_delete.end());
    std::vector<TreeNode*> forest;

    auto prune = [&](this auto&& self, TreeNode* node) -> TreeNode* {
      if (node == nullptr) {
        return nullptr;
      }
      node->left = self(node->left);
      node->right = self(node->right);

      if (delete_set.contains(node->val)) {
        if (node->left != nullptr) {
          forest.push_back(node->left);
          node->left = nullptr;
        }
        if (node->right != nullptr) {
          forest.push_back(node->right);
          node->right = nullptr;
        }
        return nullptr;
      }
      return node;
    };

    if (auto* surviving_root = prune(root); surviving_root != nullptr) {
      forest.push_back(surviving_root);
    }
    return forest;
  }

 private:
  // Clears the child link on `parent` corresponding to `side`.
  static void detachFromParent(TreeNode* parent, ChildSide side) {
    if (parent == nullptr) {
      return;
    }
    if (side == ChildSide::kLeft) {
      parent->left = nullptr;
    } else if (side == ChildSide::kRight) {
      parent->right = nullptr;
    }
  }
};

// Serializes a single tree into level-order values (`-1` marks null children
// within active levels, trailing nulls trimmed) for deterministic test checks.
std::vector<int> serializeTree(const TreeNode* root) {
  std::vector<int> out;
  if (root == nullptr) {
    return out;
  }
  std::queue<const TreeNode*> q;
  q.push(root);
  while (!q.empty()) {
    const auto* node = q.front();
    q.pop();
    if (node == nullptr) {
      out.push_back(-1);
    } else {
      out.push_back(node->val);
      q.push(node->left);
      q.push(node->right);
    }
  }
  while (!out.empty() && out.back() == -1) {
    out.pop_back();
  }
  return out;
}

// Serializes a forest into a canonically sorted list of serialized trees.
std::vector<std::vector<int>> serializeForest(
    const std::vector<TreeNode*>& forest) {
  std::vector<std::vector<int>> result;
  result.reserve(forest.size());
  for (const auto* root : forest) {
    result.push_back(serializeTree(root));
  }
  std::ranges::sort(result);
  return result;
}

LC_PROBLEM(1110, "Delete Nodes And Return Forest", "bfs", ctx) {
  Solution sol;

  ctx.test("example1_delete_3_and_5", [&] {
    //       1
    //      / \
    //     2   3
    //    / \ / \
    //   4  5 6  7
    auto make_tree = [](TreeArena& arena) {
      return arena.make(1, arena.make(2, arena.make(4), arena.make(5)),
                        arena.make(3, arena.make(6), arena.make(7)));
    };
    const std::vector<std::vector<int>> expected = {{1, 2, -1, 4}, {6}, {7}};

    TreeArena arena_bfs;
    ctx.expect_eq(
        serializeForest(sol.delNodes(make_tree(arena_bfs), {3, 5})), expected);

    TreeArena arena_dfs;
    ctx.expect_eq(
        serializeForest(sol.delNodesDfs(make_tree(arena_dfs), {3, 5})),
        expected);
  });

  ctx.test("delete_root_and_consecutive_child", [&] {
    // Delete both 1 (root) and its direct child 2; surviving roots are 4, 5, 3.
    auto make_tree = [](TreeArena& arena) {
      return arena.make(1, arena.make(2, arena.make(4), arena.make(5)),
                        arena.make(3));
    };
    const std::vector<std::vector<int>> expected = {{3}, {4}, {5}};

    TreeArena arena_bfs;
    ctx.expect_eq(
        serializeForest(sol.delNodes(make_tree(arena_bfs), {1, 2})), expected);

    TreeArena arena_dfs;
    ctx.expect_eq(
        serializeForest(sol.delNodesDfs(make_tree(arena_dfs), {1, 2})),
        expected);
  });

  ctx.test("delete_all_nodes_yields_empty_forest", [&] {
    TreeArena arena_bfs;
    auto* t1 = arena_bfs.make(1, arena_bfs.make(2), arena_bfs.make(3));
    ctx.expect_true(sol.delNodes(t1, {1, 2, 3}).empty());

    TreeArena arena_dfs;
    auto* t2 = arena_dfs.make(1, arena_dfs.make(2), arena_dfs.make(3));
    ctx.expect_true(sol.delNodesDfs(t2, {1, 2, 3}).empty());
  });

  ctx.test("delete_none_and_empty_tree", [&] {
    ctx.expect_true(sol.delNodes(nullptr, {1}).empty());
    ctx.expect_true(sol.delNodesDfs(nullptr, {1}).empty());

    TreeArena arena;
    auto* root = arena.make(1, arena.make(2), arena.make(3));
    const std::vector<std::vector<int>> expected = {{1, 2, 3}};
    ctx.expect_eq(serializeForest(sol.delNodes(root, {})), expected);
    ctx.expect_eq(serializeForest(sol.delNodesDfs(root, {})), expected);
  });
}

}  // namespace
