// LeetCode 510: Inorder Successor In BST II
// https://leetcode.com/problems/inorder-successor-in-bst-ii/
//
// Problem:
//   Given a pointer to a node in a Binary Search Tree (BST) where every node
//   stores pointers to its left child, right child, and parent (and the root
//   is not directly provided), return the in-order successor of that node—the
//   node with the smallest key strictly greater than `node->val`. If no such
//   node exists (i.e., `node` is the maximum element in the BST), return
//   `nullptr`. All node values are unique.
//
// Examples:
//   Example 1:
//     Input:  tree = [10, null, 20, 15, null, 12], node = 10
//     Output: Node(12)
//     Explanation:
//       - inorderSuccessor(Node(10)) sees node->right (Node(20)) is non-null,
//         entering the right-subtree minimum search.
//       - It initializes curr = Node(20) and follows left pointers down the chain
//         20 -> 15 -> 12.
//       - At Node(12), curr->left is nullptr, so the loop stops and returns
//         Node(12).
//
//   Example 2:
//     Input:  tree = [5, 3, 6, 2, 4, null, null, 1], node = 4
//     Output: Node(5)
//     Explanation:
//       - inorderSuccessor(Node(4)) finds node->right == nullptr and starts
//         ascending via parent pointers from curr = Node(4).
//       - Because Node(4) is the right child of Node(3) (curr->parent->right ==
//         curr), the loop steps up to curr = Node(3).
//       - Node(3) is the left child of Node(5), so the condition
//         curr->parent->right == curr is false and the function returns
//         curr->parent (Node(5)).
//
//   Example 3:
//     Input:  tree = [5, 3, 6, 2, 4, null, null, 1], node = 6
//     Output: nullptr
//     Explanation:
//       - inorderSuccessor(Node(6)) finds node->right == nullptr and starts
//         ascending from curr = Node(6).
//       - Because Node(6) is the right child of Node(5), the loop advances curr to
//         Node(5).
//       - At Node(5), curr->parent is nullptr (the root), so the loop terminates
//         and returns nullptr.
//
// Algorithm:
//   Exploit the BST ordering property directly using structural links without
//   comparing values or climbing to the root first:
//   1. If `node` has a right child, its in-order successor is the leftmost
//      descendant of `node->right`: step once to the right, then follow `left`
//      pointers until reaching a node with no left child.
//   2. Otherwise (`node->right == nullptr`), `node` is the largest element in
//      its own subtree. Walk upward via `parent` pointers until we step up
//      from a left child (`curr == curr->parent->left`); that parent is the
//      first ancestor strictly larger than `node` and thus its in-order
//      successor. If we reach the root (`curr->parent == nullptr`) without
//      ever ascending from a left child, `node` is the rightmost node of the
//      entire BST and has no successor (`nullptr`).
//   Time Complexity: O(h), where h is the height of the BST (O(log n) for a
//   balanced tree, O(n) worst case for a skewed tree).
//   Space Complexity: O(1), using only a single pointer traversal.
//
// Data Structures:
//   - Threaded/parent-linked Binary Search Tree (`Node` with `left`, `right`,
//     and `parent` pointers).
//
// Real-World Applications:
//   - Implementing `operator++` on ordered tree iterators such as C++'s
//     `std::map` / `std::set` (red-black trees with parent pointers).
//   - Range scans and cursor advancement in in-memory database indexes and
//     ordered key-value stores.
//   - Event-queue scheduling where the next chronologically ordered event is
//     fetched from a balanced search tree.
//
// Related LeetCode Problems:
//   - #285 Inorder Successor in BST [done] (same goal given root instead of parent pointers)
//   - #173 Binary Search Tree Iterator [done] (sequential in-order traversal over a BST)
//   - #270 Closest Binary Search Tree Value [done] (BST path navigation by key order)
//   - #98 Validate Binary Search Tree [done] (core BST ordering invariant)

#include "framework/lc_framework.h"

#include <deque>

namespace {

// BST node with child and parent pointers as specified by LeetCode #510.
struct Node {
  int val = 0;
  Node* left = nullptr;
  Node* right = nullptr;
  Node* parent = nullptr;
};

class Solution {
 public:
  // Finds the in-order successor of `node` in a BST with parent pointers.
  // Returns `nullptr` if `node` is null or holds the maximum key in the BST.
  // Time: O(h), Space: O(1).
  Node* inorderSuccessor(Node* node) {
    if (node == nullptr) {
      return nullptr;
    }

    // Case 1: `node` has a right subtree. Its successor is the minimum element
    // in that right subtree (the leftmost node reachable from `node->right`).
    if (node->right != nullptr) {
      Node* curr = node->right;
      while (curr->left != nullptr) {
        curr = curr->left;
      }
      return curr;
    }

    // Case 2: No right subtree. Ascend until `curr` is the left child of its
    // parent; that parent is the lowest ancestor greater than `node`.
    Node* curr = node;
    while (curr->parent != nullptr && curr->parent->right == curr) {
      curr = curr->parent;
    }
    return curr->parent;
  }
};

// RAII arena that owns BST Node instances and automatically sets parent links.
class NodeArena {
 public:
  Node* make(int val, Node* left = nullptr, Node* right = nullptr) {
    nodes_.push_back(Node{.val = val, .left = left, .right = right});
    Node* node = &nodes_.back();
    if (left != nullptr) {
      left->parent = node;
    }
    if (right != nullptr) {
      right->parent = node;
    }
    return node;
  }

 private:
  std::deque<Node> nodes_;
};

LC_PROBLEM(510, "Inorder Successor In BST II", "tree", ctx) {
  Solution sol;

  ctx.test("example_1_small_bst", [&] {
    //     2
    //    / \
    //   1   3
    NodeArena arena;
    Node* n1 = arena.make(1);
    Node* n3 = arena.make(3);
    Node* n2 = arena.make(2, n1, n3);

    ctx.expect_eq(sol.inorderSuccessor(n1), n2);
    ctx.expect_eq(sol.inorderSuccessor(n2), n3);
    ctx.expect_eq(sol.inorderSuccessor(n3), nullptr);
  });

  ctx.test("example_2_max_node_and_deep_ancestor_climb", [&] {
    //         5
    //        / \
    //       3   6
    //      / \
    //     2   4
    //    /
    //   1
    NodeArena arena;
    Node* n1 = arena.make(1);
    Node* n2 = arena.make(2, n1, nullptr);
    Node* n4 = arena.make(4);
    Node* n3 = arena.make(3, n2, n4);
    Node* n6 = arena.make(6);
    Node* n5 = arena.make(5, n3, n6);

    ctx.expect_eq(sol.inorderSuccessor(n1), n2);
    ctx.expect_eq(sol.inorderSuccessor(n2), n3);
    ctx.expect_eq(sol.inorderSuccessor(n3), n4);
    ctx.expect_eq(sol.inorderSuccessor(n4), n5);
    ctx.expect_eq(sol.inorderSuccessor(n5), n6);
    ctx.expect_eq(sol.inorderSuccessor(n6), nullptr);
  });

  ctx.test("deep_leftmost_in_right_subtree", [&] {
    //      10
    //        \
    //        20
    //       /
    //      15
    //     /
    //    12
    NodeArena arena;
    Node* n12 = arena.make(12);
    Node* n15 = arena.make(15, n12, nullptr);
    Node* n20 = arena.make(20, n15, nullptr);
    Node* n10 = arena.make(10, nullptr, n20);

    ctx.expect_eq(sol.inorderSuccessor(n10), n12);
    ctx.expect_eq(sol.inorderSuccessor(n12), n15);
    ctx.expect_eq(sol.inorderSuccessor(n15), n20);
    ctx.expect_eq(sol.inorderSuccessor(n20), nullptr);
  });

  ctx.test("single_node_and_null", [&] {
    NodeArena arena;
    Node* solo = arena.make(42);
    ctx.expect_eq(sol.inorderSuccessor(solo), nullptr);
    ctx.expect_eq(sol.inorderSuccessor(nullptr), nullptr);
  });
}

}  // namespace
