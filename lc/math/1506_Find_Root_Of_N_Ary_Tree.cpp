// LeetCode 1506: Find Root Of N Ary Tree
// https://leetcode.com/problems/find-root-of-n-ary-tree/
//
// Problem:
//   You are given all nodes of an N-ary tree as an unordered array `tree` of
//   `Node*` pointers, where every node has a unique integer value `val` and a
//   list of pointers to its `children`. Return a pointer to the root node of
//   the N-ary tree using O(1) extra space.
//   Constraints: 1 <= tree.size() <= 5 * 10^4, all node values are unique.
//
// Algorithm:
//   Use an additive in-degree cancellation invariant in O(1) auxiliary space:
//   - Every node in the tree appears exactly once in the top-level `tree` array.
//   - Every non-root node also appears exactly once inside some parent's
//     `children` list (in-degree = 1).
//   - The root node never appears in any `children` list (in-degree = 0).
//   Pass 1: Accumulate `val_sum` in a 64-bit signed integer (`std::int64_t` to
//   prevent 32-bit overflow across up to 5 * 10^4 nodes), adding `node->val`
//   for each node in `tree` and subtracting `child->val` for each of its
//   children. Every non-root value cancels out (`+v - v = 0`), leaving
//   `val_sum == root->val`. (Bitwise XOR achieves the same cancellation.)
//   Pass 2: Scan `tree` and return the unique node whose `val` equals `val_sum`.
//   Time Complexity:  O(n), where n is the number of nodes (each node is
//                     visited once as a parent and at most once as a child).
//   Space Complexity: O(1) auxiliary space (no hash set of seen children).
//
// Data Structures:
//   - N-ary tree `Node` (`int val`, `std::vector<Node*> children`): adjacency
//     representation of parent-to-child links.
//   - Single 64-bit scalar accumulator (`std::int64_t`): isolates the root's
//     value via algebraic cancellation without allocating auxiliary containers.
//
// Real-World Applications:
//   - Distributed tracing and dependency graph reconstruction: identifying the
//     entry/root span or top-level build target from an unordered dump of node
//     records in streaming O(1) memory.
//   - Filesystem and process-tree recovery (e.g., `fsck` / orphan detection):
//     locating root inodes or init processes from flat inode/process tables.
//   - Garbage collection and object-graph analysis: locating unreferenced root
//     objects in a serialized heap snapshot without building reverse-edge maps.
//
// Related LeetCode Problems:
//   - #136 Single Number [done] (same O(1)-space pair-cancellation invariant to isolate a unique value)
//   - #1516 Move Sub-Tree of N-Ary Tree [new] (companion N-ary tree structural manipulation)
//   - #429 N-ary Tree Level Order Traversal [new] (BFS traversal on the same N-ary Node structure)
//   - #589 N-ary Tree Preorder Traversal [new] (DFS traversal on N-ary trees)
//   - #1361 Validate Binary Tree Nodes [done] (identifies a unique root via zero in-degree)

#include "framework/lc_framework.h"

#include <cstdint>
#include <deque>
#include <utility>
#include <vector>

namespace {

// Definition for an N-ary tree node.
struct Node {
  int val = 0;
  std::vector<Node*> children;

  Node() = default;
  explicit Node(int v) : val(v) {}
  Node(int v, std::vector<Node*> ch) : val(v), children(std::move(ch)) {}
};

// RAII arena for test nodes: `std::deque` guarantees pointer stability across
// `emplace_back` calls and automatically destroys all nodes at scope exit.
class TreeArena {
 public:
  [[nodiscard]] Node* make(int val) {
    nodes_.emplace_back(val);
    return &nodes_.back();
  }

 private:
  std::deque<Node> nodes_;
};

class Solution {
 public:
  // Finds and returns the root of the N-ary tree given all its nodes in
  // arbitrary order.
  // Time: O(n), Space: O(1) auxiliary.
  Node* findRoot(const std::vector<Node*>& tree) {
    if (tree.empty()) {
      return nullptr;
    }

    // Use 64-bit arithmetic so intermediate sums never overflow 32-bit `int`.
    std::int64_t val_sum = 0;
    for (const Node* node : tree) {
      if (node == nullptr) {
        continue;
      }
      val_sum += node->val;
      for (const Node* child : node->children) {
        if (child != nullptr) {
          val_sum -= child->val;
        }
      }
    }

    // All non-root values have canceled out; locate the unique root node.
    for (Node* node : tree) {
      if (node != nullptr && static_cast<std::int64_t>(node->val) == val_sum) {
        return node;
      }
    }

    return nullptr;
  }
};

LC_PROBLEM(1506, "Find Root Of N Ary Tree", "math", ctx) {
  Solution sol;

  ctx.test("example_1_shuffled_nodes", [&] {
    // Tree: [1, null, 3, 2, 4, null, 5, 6]
    TreeArena arena;
    Node* n1 = arena.make(1);
    Node* n2 = arena.make(2);
    Node* n3 = arena.make(3);
    Node* n4 = arena.make(4);
    Node* n5 = arena.make(5);
    Node* n6 = arena.make(6);

    n1->children = {n3, n2, n4};
    n3->children = {n5, n6};

    // Pass nodes in non-level order to verify order independence.
    const std::vector<Node*> tree = {n5, n3, n6, n1, n4, n2};
    Node* root = sol.findRoot(tree);
    ctx.expect_eq(root, n1);
    if (root != nullptr) {
      ctx.expect_eq(root->val, 1);
    }
  });

  ctx.test("docstring_example_root_7", [&] {
    // Tree: [7, null, 3, 2, 4, null, 5, 6]
    TreeArena arena;
    Node* n7 = arena.make(7);
    Node* n3 = arena.make(3);
    Node* n2 = arena.make(2);
    Node* n4 = arena.make(4);
    Node* n5 = arena.make(5);
    Node* n6 = arena.make(6);

    n7->children = {n3, n2, n4};
    n3->children = {n5, n6};

    const std::vector<Node*> tree = {n2, n4, n6, n3, n5, n7};
    ctx.expect_eq(sol.findRoot(tree), n7);
  });

  ctx.test("multi_level_tree", [&] {
    // Tree: 1 -> {2, 3, 4, 5}, 3 -> {6, 7}, 4 -> {8}, 5 -> {9, 10},
    //       7 -> {11}, 8 -> {12}, 9 -> {13}, 11 -> {14}.
    TreeArena arena;
    std::vector<Node*> nodes;
    nodes.reserve(14);
    for (int v = 1; v <= 14; ++v) {
      nodes.push_back(arena.make(v));
    }
    nodes[0]->children = {nodes[1], nodes[2], nodes[3], nodes[4]};
    nodes[2]->children = {nodes[5], nodes[6]};
    nodes[3]->children = {nodes[7]};
    nodes[4]->children = {nodes[8], nodes[9]};
    nodes[6]->children = {nodes[10]};
    nodes[7]->children = {nodes[11]};
    nodes[8]->children = {nodes[12]};
    nodes[10]->children = {nodes[13]};

    // Reverse order so root is last in the input vector.
    const std::vector<Node*> reversed(nodes.rbegin(), nodes.rend());
    ctx.expect_eq(sol.findRoot(reversed), nodes[0]);
  });

  ctx.test("single_node_and_negative_values", [&] {
    TreeArena arena;
    Node* solo = arena.make(42);
    ctx.expect_eq(sol.findRoot({solo}), solo);

    Node* root_zero = arena.make(0);
    Node* neg_five = arena.make(-5);
    Node* pos_five = arena.make(5);
    Node* neg_ten = arena.make(-10);
    root_zero->children = {neg_five, pos_five};
    neg_five->children = {neg_ten};

    ctx.expect_eq(sol.findRoot({neg_ten, pos_five, root_zero, neg_five}),
                  root_zero);
  });

  ctx.test("empty_tree_returns_nullptr", [&] {
    ctx.expect_eq(sol.findRoot({}), nullptr);
  });
}

}  // namespace
