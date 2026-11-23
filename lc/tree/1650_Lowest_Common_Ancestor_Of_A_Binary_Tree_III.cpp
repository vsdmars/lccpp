// LeetCode 1650: Lowest Common Ancestor Of A Binary Tree III
// https://leetcode.com/problems/lowest-common-ancestor-of-a-binary-tree-iii/
//
// Problem:
//   Given two nodes `p` and `q` belonging to a binary tree where every node
//   stores a pointer to its `parent` (in addition to `val`, `left`, and
//   `right`), return their lowest common ancestor (LCA). The root node is not
//   passed directly. A node is considered a descendant of itself, all node
//   values are unique, `p != q`, and both `p` and `q` exist in the tree (with
//   between 2 and 10^5 nodes).
//
// Algorithm:
//   Because every node exposes its `parent` pointer, each node defines a
//   singly linked chain upward to the root, turning LCA into finding the first
//   intersection of two root-bound chains:
//     1. Hash Set (primary, matching the Python solution): walk from `p` to the
//        root inserting every visited node into an `unordered_set`, then walk
//        from `q` to the root and return the first node present in the set.
//        Time: O(H), Space: O(H), where H is the height of the tree (O(N) worst
//        case on a skewed tree, O(log N) on a balanced tree).
//     2. Two-Stack / Depth Alignment (matching Python `two_stack`): build the
//        root-to-node ancestor paths for `p` and `q`, trim the longer path so
//        both end at the same depth, then step upward in lockstep until they
//        meet. Time: O(H), Space: O(H).
//     3. Two-Pointer Path Equalization (O(1) space): advance `a` from `p` and
//        `b` from `q` via `parent`, switching `a` to `q` and `b` to `p` once
//        each passes the root (`nullptr`). Both pointers traverse identical
//        total distances and meet at the LCA (or `nullptr` if disjoint).
//        Time: O(H), Space: O(1).
//
// Data Structures:
//   - Binary tree `Node` with explicit `parent` link (`left`, `right`,
//     `parent`).
//   - `std::unordered_set<Node*>` to record `p`'s ancestor chain for O(1)
//     average membership checks.
//   - `std::vector<Node*>` (two-stack variant) to align root-to-node ancestor
//     paths by depth.
//   - `std::deque<Node>` test arena for leak-free RAII ownership of tree nodes
//     with stable pointer addresses.
//
// Real-World Applications:
//   - Version-control systems (`git merge-base`): finding the common ancestor
//     commit of two branches by walking parent commit pointers.
//   - DOM / UI event bubbling and layout trees: locating the nearest shared
//     ancestor element of two selected nodes to compute bounding regions or
//     selection ranges.
//   - Process trees and container/cgroup hierarchies: finding the nearest
//     common supervisor or namespace ancestor of two processes.
//   - Filesystem path resolution and object-oriented class hierarchies:
//     computing the lowest shared parent directory or base class.
//
// Related LeetCode Problems:
//   - #236 Lowest Common Ancestor of a Binary Tree [done] (standard LCA without parent pointers via post-order DFS)
//   - #235 Lowest Common Ancestor of a Binary Search Tree [done] (leverages BST ordering for O(H) time, O(1) space)
//   - #160 Intersection of Two Linked Lists [done] (isomorphic two-pointer O(1)-space intersection of two chains)
//   - #1644 Lowest Common Ancestor of a Binary Tree II [done] (LCA when p or q may not exist in the tree)
//   - #1676 Lowest Common Ancestor of a Binary Tree IV [new] (generalizes LCA to a set of k target nodes)

#include "framework/lc_framework.h"

#include <algorithm>
#include <deque>
#include <unordered_set>
#include <vector>

namespace {

// Binary tree node augmented with a pointer to its parent node.
struct Node {
  int val = 0;
  Node* left = nullptr;
  Node* right = nullptr;
  Node* parent = nullptr;

  explicit Node(int v) : val(v) {}
};

// RAII arena that owns test nodes in a std::deque so element addresses remain
// stable across insertions and all memory is freed automatically.
struct TreeArena {
  std::deque<Node> nodes;

  Node* make(int val) {
    nodes.emplace_back(val);
    return &nodes.back();
  }

  static void attach(Node* parent, Node* left, Node* right) {
    if (parent == nullptr) {
      return;
    }
    parent->left = left;
    parent->right = right;
    if (left != nullptr) {
      left->parent = parent;
    }
    if (right != nullptr) {
      right->parent = parent;
    }
  }
};

class Solution {
 public:
  // Finds the lowest common ancestor of `p` and `q` by hashing all ancestors
  // of `p` and returning the first ancestor of `q` that collides with the set.
  //
  // Time Complexity: O(H), where H is the tree height (worst case O(N)).
  // Space Complexity: O(H) to store `p`'s ancestor chain.
  Node* lowestCommonAncestor(Node* p, Node* q) {
    if (p == nullptr || q == nullptr) {
      return nullptr;
    }

    // Phase 1: Walk from p up to the root, recording every ancestor (including
    // p itself, because a node is a descendant of itself).
    std::unordered_set<Node*> ancestors;
    while (p != nullptr) {
      ancestors.insert(p);
      p = p->parent;
    }

    // Phase 2: Walk from q upward; the very first node already in `ancestors`
    // is guaranteed to be the lowest common ancestor.
    while (q != nullptr) {
      if (ancestors.contains(q)) {
        return q;
      }
      q = q->parent;
    }

    // Only reachable if p and q belong to disconnected trees.
    return nullptr;
  }

  // Alternative depth-aligned two-stack approach (matching Python `two_stack`).
  // Materializes both root-to-node paths, trims the deeper path to match
  // depths, and walks upward in lockstep until both paths converge.
  //
  // Time Complexity: O(H)
  // Space Complexity: O(H)
  Node* lowestCommonAncestorTwoStack(Node* p, Node* q) {
    if (p == nullptr || q == nullptr) {
      return nullptr;
    }

    // Build root-to-node path by walking up parent pointers and reversing so
    // index 0 is the root and the back element is the target node.
    auto build_path = [](Node* node) {
      std::vector<Node*> path;
      while (node != nullptr) {
        path.push_back(node);
        node = node->parent;
      }
      std::ranges::reverse(path);
      return path;
    };

    std::vector<Node*> p_path = build_path(p);
    std::vector<Node*> q_path = build_path(q);

    // Align depths by discarding the trailing nodes of whichever path is deeper.
    while (p_path.size() > q_path.size()) {
      p_path.pop_back();
    }
    while (q_path.size() > p_path.size()) {
      q_path.pop_back();
    }

    // Step both aligned pointers upward together until they point to the same
    // node (the LCA).
    while (!p_path.empty() && p_path.back() != q_path.back()) {
      p_path.pop_back();
      q_path.pop_back();
    }

    return p_path.empty() ? nullptr : p_path.back();
  }

  // Optimal constant-space two-pointer approach (isomorphic to LeetCode #160).
  // After passing the root, each pointer restarts at the opposite starting node
  // so both traverse equal path lengths and meet at the LCA.
  //
  // Time Complexity: O(H)
  // Space Complexity: O(1)
  Node* lowestCommonAncestorTwoPointer(Node* p, Node* q) {
    if (p == nullptr || q == nullptr) {
      return nullptr;
    }

    // Invariant: after switching chains once, both pointers have covered the
    // exact same number of non-shared edges and enter the shared ancestor
    // suffix simultaneously at the LCA (or both hit nullptr if disjoint).
    Node* a = p;
    Node* b = q;
    while (a != b) {
      a = (a != nullptr) ? a->parent : q;
      b = (b != nullptr) ? b->parent : p;
    }
    return a;
  }
};

// Helper that builds the standard example tree:
//          3
//        /   \
//       5     1
//      / \   / \
//     6   2 0   8
//        / \
//       7   4
struct SampleTree {
  TreeArena arena;
  Node* n3 = nullptr;
  Node* n5 = nullptr;
  Node* n1 = nullptr;
  Node* n6 = nullptr;
  Node* n2 = nullptr;
  Node* n0 = nullptr;
  Node* n8 = nullptr;
  Node* n7 = nullptr;
  Node* n4 = nullptr;

  SampleTree() {
    n3 = arena.make(3);
    n5 = arena.make(5);
    n1 = arena.make(1);
    n6 = arena.make(6);
    n2 = arena.make(2);
    n0 = arena.make(0);
    n8 = arena.make(8);
    n7 = arena.make(7);
    n4 = arena.make(4);

    TreeArena::attach(n3, n5, n1);
    TreeArena::attach(n5, n6, n2);
    TreeArena::attach(n2, n7, n4);
    TreeArena::attach(n1, n0, n8);
  }
};

LC_PROBLEM(1650, "Lowest Common Ancestor Of A Binary Tree III", "tree", ctx) {
  auto verify_all = [&](Node* p, Node* q, Node* expected) {
    Solution sol;
    ctx.expect_eq(sol.lowestCommonAncestor(p, q), expected);
    ctx.expect_eq(sol.lowestCommonAncestorTwoStack(p, q), expected);
    ctx.expect_eq(sol.lowestCommonAncestorTwoPointer(p, q), expected);
  };

  ctx.test("example_1_split_at_root", [&] {
    SampleTree t;
    verify_all(t.n5, t.n1, t.n3);
  });

  ctx.test("example_2_ancestor_is_self", [&] {
    SampleTree t;
    verify_all(t.n5, t.n4, t.n5);
  });

  ctx.test("python_build_different_depths", [&] {
    SampleTree t;
    // Matches build() in the Python source: LCA(6, 4) == 5.
    verify_all(t.n6, t.n4, t.n5);
  });

  ctx.test("example_3_two_node_tree", [&] {
    TreeArena arena;
    Node* n1 = arena.make(1);
    Node* n2 = arena.make(2);
    TreeArena::attach(n1, n2, nullptr);
    verify_all(n1, n2, n1);
    verify_all(n2, n1, n1);
  });

  ctx.test("deep_skewed_chain", [&] {
    TreeArena arena;
    Node* prev = arena.make(0);
    Node* mid = nullptr;
    Node* leaf = nullptr;
    for (int i = 1; i <= 20; ++i) {
      Node* cur = arena.make(i);
      TreeArena::attach(prev, cur, nullptr);
      if (i == 7) {
        mid = cur;
      }
      if (i == 20) {
        leaf = cur;
      }
      prev = cur;
    }
    verify_all(mid, leaf, mid);
    verify_all(leaf, mid, mid);
  });

  ctx.test("deep_leaves_on_opposite_subtrees", [&] {
    SampleTree t;
    verify_all(t.n7, t.n8, t.n3);
    verify_all(t.n7, t.n4, t.n2);
  });

  ctx.test("null_and_disjoint_edge_cases", [&] {
    TreeArena arena;
    Node* r1 = arena.make(10);
    Node* c1 = arena.make(11);
    Node* r2 = arena.make(20);
    TreeArena::attach(r1, c1, nullptr);

    verify_all(nullptr, c1, nullptr);
    verify_all(c1, nullptr, nullptr);
    verify_all(c1, r2, nullptr);
  });
}

}  // namespace
