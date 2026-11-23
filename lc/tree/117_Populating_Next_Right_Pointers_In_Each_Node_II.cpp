// LeetCode 117: Populating Next Right Pointers In Each Node II
// https://leetcode.com/problems/populating-next-right-pointers-in-each-node-ii/
//
// Problem:
//   Given ANY binary tree (not necessarily perfect or complete) where each node
//   has `val`, `left`, `right`, and `next` (initially nullptr), populate each
//   `next` pointer to point to its next right node on the same level, or
//   nullptr if there is no next right node. Return the root of the tree.
//   Follow-up constraint: use O(1) constant extra space.
//
// Algorithm:
//   Two approaches (matching both methods in the Python source):
//     1. O(1)-Space Sentinel Level Threading (`connect`):
//        Unlike a perfect binary tree, the leftmost node of the next level is
//        not necessarily `level_head->left` (or even a child of `level_head`),
//        and consecutive siblings on the next level may be separated by
//        multiple childless parents. By iterating across the already-linked
//        current level via `curr = curr->next` and appending every non-null
//        `curr->left` and `curr->right` onto a stack-local `sentinel` list,
//        `sentinel.next` automatically captures the exact leftmost node of the
//        next level regardless of gaps. Repeat until `sentinel.next == nullptr`.
//     2. Level-Order BFS (`connectBfs`):
//        Collect all non-null nodes of each level into a vector, wire adjacent
//        elements `level[i]->next = level[i + 1]`, and build the next level.
//   Time Complexity:  O(N) for both approaches (each node visited once).
//   Space Complexity: O(1) auxiliary space for `connect`; O(W) <= O(N) for
//                     `connectBfs`, where W is the maximum level width.
//
// Data Structures:
//   - Binary tree nodes (`Node`) with horizontal `next` pointers.
//   - Stack-local dummy sentinel `Node` in `connect` to anchor and append the
//     next level's linked list cleanly in O(1) space.
//   - `std::vector<Node*>` level buffer in `connectBfs`.
//
// Real-World Applications:
//   - Linking sparse nodes at identical depth in unbalanced tries, radix trees,
//     or adaptive quadtrees for horizontal neighborhood scans.
//   - Threading irregular document/DOM trees by nesting level for parallel
//     layout or sibling-lookup passes.
//   - Connecting active states at the same depth in pruned search trees (e.g.
//     beam search frontiers).
//
// Related LeetCode Problems:
//   - #116 Populating Next Right Pointers in Each Node [done] (special case restricted to perfect binary trees)
//   - #102 Binary Tree Level Order Traversal [done] (collects nodes at each depth via BFS)
//   - #199 Binary Tree Right Side View [done] (finds the rightmost node on each level)
//   - #515 Find Largest Value in Each Tree Row [done] (level-order aggregation on arbitrary binary trees)
//   - #662 Maximum Width of Binary Tree [done] (handles sparse gaps across arbitrary binary tree levels)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <utility>
#include <vector>

namespace {

// Binary tree node augmented with a horizontal `next` right pointer.
struct Node {
  int val = 0;
  Node* left = nullptr;
  Node* right = nullptr;
  Node* next = nullptr;

  explicit Node(int value) : val(value) {}
  Node(int value, Node* left_child, Node* right_child)
      : val(value), left(left_child), right(right_child) {}
};

// Arena owning Node instances for leak-free test tree construction.
class NodeArena {
 public:
  Node* make(int val, Node* left = nullptr, Node* right = nullptr) {
    nodes_.emplace_back(val, left, right);
    return &nodes_.back();
  }

 private:
  std::deque<Node> nodes_;
};

class Solution {
 public:
  // Populates `next` pointers for an arbitrary binary tree in O(1) extra space.
  //
  // Invariant: at the start of each outer iteration, `level_head` is the
  // leftmost node of the current level and all `next` pointers on that level
  // are already finalized. The inner loop builds the complete `next` chain of
  // the following level starting at `sentinel.next`.
  // Time: O(N), Space: O(1).
  Node* connect(Node* root) {
    Node* level_head = root;

    while (level_head != nullptr) {
      // Fresh sentinel handles arbitrary missing subtrees cleanly: its `next`
      // becomes the leftmost existing child anywhere on the current level.
      Node sentinel(0);
      Node* tail = &sentinel;

      for (Node* curr = level_head; curr != nullptr; curr = curr->next) {
        if (curr->left != nullptr) {
          tail->next = curr->left;
          tail = tail->next;
        }
        if (curr->right != nullptr) {
          tail->next = curr->right;
          tail = tail->next;
        }
      }

      // Advance to the next level; nullptr when no node on this level had
      // children.
      level_head = sentinel.next;
    }

    return root;
  }

  // Alternative level-order BFS implementation matching Python's `bfsSolution`.
  // Time: O(N), Space: O(W) where W is the maximum level width.
  Node* connectBfs(Node* root) {
    if (root == nullptr) {
      return nullptr;
    }

    std::vector<Node*> level = {root};
    while (!level.empty()) {
      std::vector<Node*> next_level;
      for (std::size_t i = 0uz; i < level.size(); ++i) {
        level[i]->next = (i + 1 < level.size()) ? level[i + 1] : nullptr;
        if (level[i]->left != nullptr) {
          next_level.push_back(level[i]->left);
        }
        if (level[i]->right != nullptr) {
          next_level.push_back(level[i]->right);
        }
      }
      level = std::move(next_level);
    }

    return root;
  }
};

// Verifies that for every level of an arbitrary binary tree, walking `next`
// pointers from the level's leftmost node produces the exact BFS level order.
std::vector<std::vector<int>> verifyAndCollectLevels(const Node* root,
                                                     lc::TestContext& ctx) {
  std::vector<std::vector<int>> levels;
  if (root == nullptr) {
    return levels;
  }

  std::vector<const Node*> bfs_level = {root};
  while (!bfs_level.empty()) {
    std::vector<int> via_next;
    for (const Node* curr = bfs_level.front(); curr != nullptr;
         curr = curr->next) {
      via_next.push_back(curr->val);
    }

    std::vector<int> expected_row;
    expected_row.reserve(bfs_level.size());
    std::vector<const Node*> next_bfs;
    for (const Node* node : bfs_level) {
      expected_row.push_back(node->val);
      if (node->left != nullptr) {
        next_bfs.push_back(node->left);
      }
      if (node->right != nullptr) {
        next_bfs.push_back(node->right);
      }
    }

    ctx.expect_eq(via_next, expected_row);
    levels.push_back(std::move(via_next));
    bfs_level = std::move(next_bfs);
  }

  return levels;
}

LC_PROBLEM(117, "Populating Next Right Pointers In Each Node II", "tree", ctx) {
  Solution sol;

  ctx.test("leetcode example 1 and python build()", [&] {
    //        1 -> NULL
    //       / \
    //      2 -> 3 -> NULL
    //     / \    \
    //    4-> 5 -> 7 -> NULL
    auto make_tree = [](NodeArena& arena) -> Node* {
      return arena.make(1, arena.make(2, arena.make(4), arena.make(5)),
                        arena.make(3, nullptr, arena.make(7)));
    };
    const std::vector<std::vector<int>> expected = {{1}, {2, 3}, {4, 5, 7}};

    NodeArena arena_o1;
    Node* root_o1 = sol.connect(make_tree(arena_o1));
    ctx.expect_eq(verifyAndCollectLevels(root_o1, ctx), expected);

    NodeArena arena_bfs;
    Node* root_bfs = sol.connectBfs(make_tree(arena_bfs));
    ctx.expect_eq(verifyAndCollectLevels(root_bfs, ctx), expected);
  });

  ctx.test("empty tree and single node", [&] {
    ctx.expect_true(sol.connect(nullptr) == nullptr);
    ctx.expect_true(sol.connectBfs(nullptr) == nullptr);

    NodeArena arena;
    Node* single = sol.connect(arena.make(9));
    ctx.expect_eq(verifyAndCollectLevels(single, ctx),
                  std::vector<std::vector<int>>{{9}});
  });

  ctx.test("wide gap across cousins with childless leftmost branch", [&] {
    //          1
    //        /   \
    //       2     3
    //      / \     \
    //     4   5     6
    //                \
    //                 8
    // Level 2's leftmost nodes (4, 5) have no children; level 3 starts at 8.
    auto make_sparse = [](NodeArena& arena) -> Node* {
      return arena.make(
          1, arena.make(2, arena.make(4), arena.make(5)),
          arena.make(3, nullptr, arena.make(6, nullptr, arena.make(8))));
    };
    const std::vector<std::vector<int>> expected = {{1}, {2, 3}, {4, 5, 6}, {8}};

    NodeArena arena_o1;
    ctx.expect_eq(verifyAndCollectLevels(sol.connect(make_sparse(arena_o1)),
                                         ctx),
                  expected);

    NodeArena arena_bfs;
    ctx.expect_eq(
        verifyAndCollectLevels(sol.connectBfs(make_sparse(arena_bfs)), ctx),
        expected);
  });

  ctx.test("multi-parent gap bridging leftmost and rightmost leaves", [&] {
    //          1
    //        /   \
    //       2     3
    //      / \   / \
    //     4   5 6   7
    //    /           \
    //   8             9
    NodeArena arena;
    Node* root = arena.make(
        1, arena.make(2, arena.make(4, arena.make(8)), arena.make(5)),
        arena.make(3, arena.make(6), arena.make(7, nullptr, arena.make(9))));

    sol.connect(root);
    const std::vector<std::vector<int>> expected = {
        {1}, {2, 3}, {4, 5, 6, 7}, {8, 9}};
    ctx.expect_eq(verifyAndCollectLevels(root, ctx), expected);
  });
}

}  // namespace
