// LeetCode 426: Convert Binary Search Tree To Sorted Doubly Linked List
// https://leetcode.com/problems/convert-binary-search-tree-to-sorted-doubly-linked-list/
//
// Problem:
//   Convert a Binary Search Tree (BST) in-place into a sorted circular doubly
//   linked list. Reuse each node's `left` pointer as its predecessor and
//   `right` pointer as its successor. The predecessor of the smallest element
//   (head) must point to the largest element (tail), and the successor of the
//   tail must point back to the head. Return a pointer to the smallest element
//   (or `nullptr` if the tree is empty).
//
// Algorithm:
//   Two O(N) in-place approaches are provided:
//     1. Iterative In-Order Traversal (`treeToDoublyList`):
//        An in-order traversal of a BST visits nodes in strictly ascending
//        order. Push the leftmost spine onto an explicit stack, then pop nodes
//        one by one and append each to a stack-allocated sentinel dummy node,
//        wiring `prev->right = node` and `node->left = prev`. Whenever a popped
//        node has a right child, push that right subtree's leftmost spine.
//        Finally, close the ring by connecting `head` (`sentinel.right`) and
//        the final `prev` (tail).
//     2. Recursive Divide-and-Conquer (`treeToDoublyListRecursive`):
//        Each recursive call converts a subtree into a non-circular sorted
//        doubly linked list and returns its `{min_node, max_node}` endpoints,
//        splicing `left_max <-> node <-> right_min`. At the top level, close
//        the ring between the global minimum and maximum nodes.
//   Time Complexity: O(N), where N is the number of nodes (every node is
//   visited and re-linked once).
//   Space Complexity: O(H), where H is the tree height (explicit stack or
//   recursion call stack), O(N) worst case for a skewed tree.
//
// Data Structures:
//   - In-place tree / circular doubly linked list (`Node` with `val`, `left`,
//     and `right` pointers).
//   - Stack-allocated dummy sentinel `Node` and explicit `std::vector<Node*>`
//     stack (iterative approach), or recursion call stack (recursive approach).
//
// Real-World Applications:
//   - Database B-tree / index leaf-level linking: connecting ordered tree
//     entries into doubly linked chains for fast range scans and reverse
//     iteration without re-traversing internal nodes.
//   - Ordered in-memory caches and skip-list/tree hybrid indexes that flatten
//     tree snapshots into circular rings for O(1) wrap-around iteration.
//   - Embedded systems and kernel data structures performing zero-allocation
//     in-place container conversions.
//
// Related LeetCode Problems:
//   - #94 Binary Tree Inorder Traversal [done] (foundational iterative and recursive in-order traversal)
//   - #114 Flatten Binary Tree to Linked List [done] (in-place tree flattening into a linked list)
//   - #173 Binary Search Tree Iterator [done] (controlled stack-based in-order BST iteration)
//   - #109 Convert Sorted List to Binary Search Tree [done] (inverse problem: sorted list to balanced BST)

#include "framework/lc_framework.h"

#include <deque>
#include <utility>
#include <vector>

namespace {

// BST / circular doubly linked list node (`left` = prev, `right` = next).
struct Node {
  int val = 0;
  Node* left = nullptr;
  Node* right = nullptr;
};

class Solution {
 public:
  // Converts a BST in-place into a sorted circular doubly linked list via
  // iterative in-order traversal.
  // Time: O(N), Space: O(H).
  Node* treeToDoublyList(Node* root) {
    if (root == nullptr) {
      return nullptr;
    }

    // Stack-allocated sentinel avoids special-casing the first popped node.
    Node sentinel{};
    Node* prev = &sentinel;
    std::vector<Node*> stack;

    auto push_left_spine = [&](Node* curr) {
      while (curr != nullptr) {
        stack.push_back(curr);
        curr = curr->left;
      }
    };

    push_left_spine(root);

    while (!stack.empty()) {
      Node* node = stack.back();
      stack.pop_back();

      // Capture the right subtree before mutating `node->right` later.
      Node* right_child = node->right;

      // Link `node` immediately after `prev` in sorted order.
      prev->right = node;
      node->left = prev;
      prev = node;

      if (right_child != nullptr) {
        push_left_spine(right_child);
      }
    }

    // Close the circular links between the smallest (head) and largest (tail).
    Node* head = sentinel.right;
    if (head != nullptr) {
      head->left = prev;
      prev->right = head;
    }
    return head;
  }

  // Recursive divide-and-conquer variant returning `{min_node, max_node}` for
  // each subtree and closing the circular ring at the root.
  // Time: O(N), Space: O(H).
  Node* treeToDoublyListRecursive(Node* root) {
    if (root == nullptr) {
      return nullptr;
    }

    auto dfs = [](this auto&& self, Node* node) -> std::pair<Node*, Node*> {
      Node* sub_min = node;
      Node* sub_max = node;

      if (node->left != nullptr) {
        const auto [lmin, lmax] = self(node->left);
        sub_min = lmin;
        lmax->right = node;
        node->left = lmax;
      }

      if (node->right != nullptr) {
        const auto [rmin, rmax] = self(node->right);
        sub_max = rmax;
        node->right = rmin;
        rmin->left = node;
      }

      return {sub_min, sub_max};
    };

    const auto [head, tail] = dfs(root);
    head->left = tail;
    tail->right = head;
    return head;
  }
};

// RAII arena that owns all Nodes even after cyclic links are created.
class NodeArena {
 public:
  Node* make(int val, Node* left = nullptr, Node* right = nullptr) {
    return &nodes_.emplace_back(Node{.val = val, .left = left, .right = right});
  }

 private:
  std::deque<Node> nodes_;
};

// Traverses a circular doubly linked list forward and verifies bidirectional
// pointer consistency and wrap-around back to `head`.
std::vector<int> collect_forward(const Node* head, int max_steps) {
  std::vector<int> values;
  if (head == nullptr) {
    return values;
  }
  const Node* curr = head;
  for (int step = 0; step < max_steps; ++step) {
    values.push_back(curr->val);
    curr = curr->right;
    if (curr == head) {
      break;
    }
  }
  return values;
}

// Traverses a circular doubly linked list backward starting from `head->left`
// (the tail) down to `head`.
std::vector<int> collect_backward(const Node* head, int max_steps) {
  std::vector<int> values;
  if (head == nullptr) {
    return values;
  }
  const Node* tail = head->left;
  const Node* curr = tail;
  for (int step = 0; step < max_steps; ++step) {
    values.push_back(curr->val);
    curr = curr->left;
    if (curr == tail) {
      break;
    }
  }
  return values;
}

// Builds the canonical example BST: 4 -> (2 -> (1, 3), 5).
Node* build_example_bst(NodeArena& arena) {
  Node* n1 = arena.make(1);
  Node* n3 = arena.make(3);
  Node* n5 = arena.make(5);
  Node* n2 = arena.make(2, n1, n3);
  return arena.make(4, n2, n5);
}

LC_PROBLEM(426, "Convert Binary Search Tree To Sorted Doubly Linked List",
           "tree", ctx) {
  ctx.test("example_bst_iterative_and_recursive", [&] {
    Solution sol;
    const std::vector<int> forward_expected = {1, 2, 3, 4, 5};
    const std::vector<int> backward_expected = {5, 4, 3, 2, 1};

    NodeArena arena1;
    Node* head1 = sol.treeToDoublyList(build_example_bst(arena1));
    ctx.expect_eq(collect_forward(head1, 10), forward_expected);
    ctx.expect_eq(collect_backward(head1, 10), backward_expected);

    NodeArena arena2;
    Node* head2 = sol.treeToDoublyListRecursive(build_example_bst(arena2));
    ctx.expect_eq(collect_forward(head2, 10), forward_expected);
    ctx.expect_eq(collect_backward(head2, 10), backward_expected);
  });

  ctx.test("empty_and_single_node", [&] {
    Solution sol;
    ctx.expect_true(sol.treeToDoublyList(nullptr) == nullptr);
    ctx.expect_true(sol.treeToDoublyListRecursive(nullptr) == nullptr);

    NodeArena arena1;
    Node* single1 = sol.treeToDoublyList(arena1.make(42));
    ctx.expect_true(single1 != nullptr);
    if (single1 != nullptr) {
      ctx.expect_true(single1->left == single1);
      ctx.expect_true(single1->right == single1);
      ctx.expect_eq(single1->val, 42);
    }

    NodeArena arena2;
    Node* single2 = sol.treeToDoublyListRecursive(arena2.make(42));
    ctx.expect_true(single2 != nullptr);
    if (single2 != nullptr) {
      ctx.expect_true(single2->left == single2);
      ctx.expect_true(single2->right == single2);
      ctx.expect_eq(single2->val, 42);
    }
  });

  ctx.test("left_and_right_skewed_bsts", [&] {
    Solution sol;
    const std::vector<int> asc = {1, 2, 3};
    const std::vector<int> desc = {3, 2, 1};

    // Left-skewed: 3 -> left: 2 -> left: 1
    NodeArena arena1;
    Node* l_root = arena1.make(3, arena1.make(2, arena1.make(1), nullptr), nullptr);
    Node* l_head = sol.treeToDoublyList(l_root);
    ctx.expect_eq(collect_forward(l_head, 10), asc);
    ctx.expect_eq(collect_backward(l_head, 10), desc);

    // Right-skewed: 1 -> right: 2 -> right: 3
    NodeArena arena2;
    Node* r_root = arena2.make(1, nullptr, arena2.make(2, nullptr, arena2.make(3)));
    Node* r_head = sol.treeToDoublyListRecursive(r_root);
    ctx.expect_eq(collect_forward(r_head, 10), asc);
    ctx.expect_eq(collect_backward(r_head, 10), desc);
  });
}

}  // namespace
