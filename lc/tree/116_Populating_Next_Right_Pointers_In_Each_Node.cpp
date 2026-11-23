// LeetCode 116: Populating Next Right Pointers In Each Node
// https://leetcode.com/problems/populating-next-right-pointers-in-each-node/
//
// Problem:
//   Given a perfect binary tree where all leaves are on the same level and
//   every parent has two children, each node contains `val`, `left`, `right`,
//   and `next` (initially nullptr). Populate each `next` pointer to point to
//   its next right node on the same level, or nullptr if there is no next
//   right node. Return the root of the modified tree.
//   Constraint follow-up: solve using O(1) constant extra space.
//
// Examples:
//   Example 1:
//     Input:  root = [1, 2, 3, 4, 5, 6, 7]
//     Output: [[1], [2, 3], [4, 5, 6, 7]] (next pointers per level: 1 -> nullptr, 2
//             -> 3 -> nullptr, 4 -> 5 -> 6 -> 7 -> nullptr)
//     Explanation:
//       - Start with level_head = node 1 (1->next is nullptr) and create a
//         stack-local sentinel node with tail = &sentinel.
//       - Walk curr = 1, appending curr->left (2) and curr->right (3) to tail->next
//         so sentinel.next is 2 -> 3 -> nullptr, then set level_head = 2.
//       - At level 2 with a fresh sentinel, walk curr = 2 (appending 4, 5) then
//         curr = curr->next = 3 (appending 6, 7), forming 4 -> 5 -> 6 -> 7 ->
//         nullptr and advancing level_head = 4.
//       - At level 3, walk 4 -> 5 -> 6 -> 7; all leaves have null children so
//         sentinel.next remains nullptr, setting level_head = nullptr and returning
//         root.
//
//   Example 2:
//     Input:  root = [1]
//     Output: [[1]] (1 -> nullptr)
//     Explanation:
//       - Set level_head = node 1 and initialize a fresh stack-local sentinel with
//         tail = &sentinel.
//       - Visit curr = 1; both curr->left and curr->right are nullptr, so no
//         children are appended and sentinel.next stays nullptr.
//       - Advance level_head = sentinel.next (nullptr), terminating the loop and
//         returning root with root->next == nullptr.
//
//   Example 3:
//     Input:  root = [] (nullptr)
//     Output: nullptr
//     Explanation:
//       - Initialize level_head = root (nullptr) in connect (or hit the root ==
//         nullptr early return in connectBfs).
//       - Because level_head == nullptr, skip the outer while loop immediately and
//         return nullptr.
//
// Algorithm:
//   Two approaches (matching both methods in the Python source):
//     1. O(1)-Space Level Threading (`connect`):
//        Because the current level is already linked via `next` pointers
//        (trivially true for level 0 containing only `root`), we can walk the
//        current level horizontally without a queue. For each level, allocate
//        a stack-local sentinel node (`sentinel`) and a `tail` pointer to
//        build the singly linked list for the next level. Append `curr->left`
//        and `curr->right` to `tail->next` as `curr` advances via `curr->next`.
//        When the current level finishes, advance `level_head = sentinel.next`.
//        Stop when `level_head == nullptr`.
//     2. Level-Order BFS (`connectBfs`):
//        Maintain a vector of nodes at the current depth, link consecutive
//        entries `level[i - 1]->next = level[i]`, and collect all children for
//        the next depth.
//   Time Complexity:  O(N) for both approaches (each node visited once).
//   Space Complexity: O(1) auxiliary space for `connect`; O(N) for `connectBfs`
//                     (maximum level width is (N + 1) / 2).
//
// Data Structures:
//   - Binary tree nodes (`Node`) augmented with horizontal `next` sibling
//     pointers.
//   - Stack-local dummy sentinel `Node` in `connect` to simplify list-building
//     without special-casing the first node of the next level.
//   - `std::vector<Node*>` level buffer in `connectBfs`.
//
// Real-World Applications:
//   - B+ tree internal/leaf sibling linking to enable fast horizontal range
//     scans without re-traversing parent pointers.
//   - Quadtree / spatial-index neighbor linking for level-of-detail (LOD)
//     rendering and geospatial tile streaming.
//   - Hierarchical UI/layout engines that need left-to-right sibling traversal
//     across nodes at the same depth.
//
// Related LeetCode Problems:
//   - #117 Populating Next Right Pointers in Each Node II [done] (generalizes to arbitrary binary trees)
//   - #102 Binary Tree Level Order Traversal [done] (standard BFS level-by-level traversal)
//   - #199 Binary Tree Right Side View [done] (extracts the rightmost node on every level)
//   - #637 Average of Levels in Binary Tree [done] (aggregates values across each tree level)
//   - #103 Binary Tree Zigzag Level Order Traversal [done] (level-order traversal with alternating direction)

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
  // Populates `next` pointers level-by-level in O(1) extra space by walking the
  // already-linked parent level and threading children onto a sentinel list.
  //
  // Invariant: at the start of each outer loop iteration, every node on the
  // level starting at `level_head` already has its final `next` pointer set.
  // Time: O(N), Space: O(1).
  Node* connect(Node* root) {
    Node* level_head = root;

    while (level_head != nullptr) {
      // Fresh sentinel per level ensures `sentinel.next` starts as nullptr and
      // naturally terminates the outer loop after the leaf level.
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

      // Move down to the leftmost node of the newly linked child level.
      level_head = sentinel.next;
    }

    return root;
  }

  // Alternative BFS solution using an explicit per-level node buffer.
  // Time: O(N), Space: O(N).
  Node* connectBfs(Node* root) {
    if (root == nullptr) {
      return nullptr;
    }

    std::vector<Node*> level = {root};
    while (!level.empty()) {
      std::vector<Node*> next_level;
      next_level.reserve(level.size() * 2);

      for (std::size_t i = 0uz; i < level.size(); ++i) {
        if (i + 1 < level.size()) {
          level[i]->next = level[i + 1];
        } else {
          level[i]->next = nullptr;
        }
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

// Traverses a connected perfect binary tree by walking down the leftmost spine
// and following `next` pointers across each level.
std::vector<std::vector<int>> collectLevelsViaNext(const Node* root) {
  std::vector<std::vector<int>> levels;
  for (const Node* level_head = root; level_head != nullptr;
       level_head = level_head->left) {
    std::vector<int> row;
    for (const Node* curr = level_head; curr != nullptr; curr = curr->next) {
      row.push_back(curr->val);
    }
    levels.push_back(std::move(row));
  }
  return levels;
}

LC_PROBLEM(116, "Populating Next Right Pointers In Each Node", "tree", ctx) {
  Solution sol;

  ctx.test("leetcode example 1 and python build() (depth 3 perfect tree)", [&] {
    //       1 -> NULL
    //      / \
    //     2 -> 3 -> NULL
    //    / \  / \
    //   4->5->6->7 -> NULL
    auto make_tree = [](NodeArena& arena) -> Node* {
      return arena.make(1, arena.make(2, arena.make(4), arena.make(5)),
                        arena.make(3, arena.make(6), arena.make(7)));
    };
    const std::vector<std::vector<int>> expected = {{1}, {2, 3}, {4, 5, 6, 7}};

    NodeArena arena_o1;
    Node* root_o1 = sol.connect(make_tree(arena_o1));
    ctx.expect_eq(collectLevelsViaNext(root_o1), expected);

    NodeArena arena_bfs;
    Node* root_bfs = sol.connectBfs(make_tree(arena_bfs));
    ctx.expect_eq(collectLevelsViaNext(root_bfs), expected);
  });

  ctx.test("empty tree returns nullptr", [&] {
    ctx.expect_true(sol.connect(nullptr) == nullptr);
    ctx.expect_true(sol.connectBfs(nullptr) == nullptr);
  });

  ctx.test("single node tree", [&] {
    NodeArena arena;
    Node* root = arena.make(1);
    Node* connected = sol.connect(root);
    ctx.expect_true(connected == root);
    ctx.expect_true(root->next == nullptr);
    ctx.expect_eq(collectLevelsViaNext(root),
                  std::vector<std::vector<int>>{{1}});
  });

  ctx.test("depth 4 perfect binary tree (15 nodes)", [&] {
    NodeArena arena;
    Node* n8 = arena.make(8);
    Node* n9 = arena.make(9);
    Node* n10 = arena.make(10);
    Node* n11 = arena.make(11);
    Node* n12 = arena.make(12);
    Node* n13 = arena.make(13);
    Node* n14 = arena.make(14);
    Node* n15 = arena.make(15);
    Node* root = arena.make(
        1,
        arena.make(2, arena.make(4, n8, n9), arena.make(5, n10, n11)),
        arena.make(3, arena.make(6, n12, n13), arena.make(7, n14, n15)));

    sol.connect(root);
    const std::vector<std::vector<int>> expected = {
        {1}, {2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11, 12, 13, 14, 15}};
    ctx.expect_eq(collectLevelsViaNext(root), expected);
  });
}

}  // namespace
