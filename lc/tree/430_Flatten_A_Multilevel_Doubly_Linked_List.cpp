// LeetCode 430: Flatten A Multilevel Doubly Linked List
// https://leetcode.com/problems/flatten-a-multilevel-doubly-linked-list/
//
// Problem:
//   You are given the head of a doubly linked list where each node has `val`,
//   `prev`, `next`, and an optional `child` pointer to another multilevel
//   doubly linked list. Flatten the structure in-place into a single-level
//   doubly linked list such that any node's child list appears immediately
//   after that node and before its original `next` node. Every node's `child`
//   pointer must be set to `nullptr` in the resulting list.
//
// Algorithm:
//   Viewing `child` as a left branch and `next` as a right branch makes the
//   desired output order a standard Preorder DFS traversal. Two O(N) in-place
//   implementations are provided:
//     1. Iterative Preorder Stack (`flatten`):
//        Push `head` onto an explicit stack. Repeatedly pop `node`, link it
//        after `prev` (`prev->next = node; node->prev = prev`), push
//        `node->next` first (if non-null) and `node->child` second (if
//        non-null) so the child list is popped and visited before `next`, and
//        clear `node->child = nullptr`. Finally set `tail->next = nullptr` and
//        `head->prev = nullptr`.
//     2. Recursive Tail-Returning DFS (`flattenRecursive`):
//        A helper flattens the sublist starting at `node` and returns its tail.
//        When `node->child` exists, splice the flattened child sublist between
//        `node` and `original_next`, clearing `node->child`, then continue
//        flattening from `original_next`.
//   Time Complexity: O(N), where N is the total number of nodes across all
//   levels (each node is visited and linked once).
//   Space Complexity: O(N) worst case for the stack (e.g., deeply nested child
//   chains) and O(D) for typical nesting depth D.
//
// Data Structures:
//   - Multilevel doubly linked list (`Node` with `val`, `prev`, `next`, and
//     `child` pointers).
//   - Explicit DFS stack (`std::vector<Node*>`) in the iterative solver, or
//     recursion stack in the recursive solver.
//
// Real-World Applications:
//   - Document outliners, nested comment threads, and rich-text editors
//     flattening hierarchical sections into a linear cursor-navigation list.
//   - Memory allocators and task schedulers linearizing hierarchical work
//     queues or nested coroutine continuations.
//   - File-system and UI accessibility focus-order traversal across nested
//     container views.
//
// Related LeetCode Problems:
//   - #114 Flatten Binary Tree to Linked List [done] (in-place preorder flattening of a binary tree)
//   - #426 Convert Binary Search Tree to Sorted Doubly Linked List [done] (in-place doubly linked list rewiring)
//   - #341 Flatten Nested List Iterator [done] (stack-based flattening of arbitrarily nested lists)
//   - #1660 Correct a Binary Tree [new] (pointer surgery on linked tree nodes)

#include "framework/lc_framework.h"

#include <deque>
#include <vector>

namespace {

// Multilevel doubly linked list node.
struct Node {
  int val = 0;
  Node* prev = nullptr;
  Node* next = nullptr;
  Node* child = nullptr;
};

class Solution {
 public:
  // Flattens a multilevel doubly linked list in-place using an explicit
  // preorder DFS stack.
  // Time: O(N), Space: O(N) worst case.
  Node* flatten(Node* head) {
    if (head == nullptr) {
      return nullptr;
    }

    std::vector<Node*> stack;
    stack.push_back(head);
    Node* prev = nullptr;

    while (!stack.empty()) {
      Node* node = stack.back();
      stack.pop_back();

      // Stitch `node` onto the tail of the flattened list.
      if (prev != nullptr) {
        prev->next = node;
        node->prev = prev;
      }

      // Push `next` before `child` so LIFO order visits the child list first.
      if (node->next != nullptr) {
        stack.push_back(node->next);
      }
      if (node->child != nullptr) {
        stack.push_back(node->child);
        node->child = nullptr;
      }

      prev = node;
    }

    // Terminate both ends cleanly.
    if (prev != nullptr) {
      prev->next = nullptr;
    }
    head->prev = nullptr;
    return head;
  }

  // Recursive in-place variant that returns the tail of each flattened segment.
  // Time: O(N), Space: O(N) worst case.
  Node* flattenRecursive(Node* head) {
    if (head == nullptr) {
      return nullptr;
    }

    // Flattens the list starting at `node` and returns its final tail node.
    auto dfs = [](this auto&& self, Node* node) -> Node* {
      Node* original_next = node->next;
      Node* tail = node;

      if (node->child != nullptr) {
        Node* child_head = node->child;
        Node* child_tail = self(child_head);

        // Splice `[child_head ... child_tail]` between `node` and
        // `original_next`.
        node->next = child_head;
        child_head->prev = node;
        node->child = nullptr;

        child_tail->next = original_next;
        if (original_next != nullptr) {
          original_next->prev = child_tail;
        }
        tail = child_tail;
      }

      if (original_next != nullptr) {
        return self(original_next);
      }
      return tail;
    };

    dfs(head);
    return head;
  }
};

// RAII arena that owns test Nodes without raw new/delete.
class NodeArena {
 public:
  Node* make(int val) { return &nodes_.emplace_back(Node{.val = val}); }

  // Helper that builds a flat doubly linked list from `vals` and returns its
  // head (storing pointers into `out_nodes` if requested).
  Node* make_list(const std::vector<int>& vals,
                  std::vector<Node*>* out_nodes = nullptr) {
    Node* head = nullptr;
    Node* prev = nullptr;
    for (const int v : vals) {
      Node* node = make(v);
      if (head == nullptr) {
        head = node;
      }
      if (prev != nullptr) {
        prev->next = node;
        node->prev = prev;
      }
      if (out_nodes != nullptr) {
        out_nodes->push_back(node);
      }
      prev = node;
    }
    return head;
  }

 private:
  std::deque<Node> nodes_;
};

// Builds the 3-level canonical example:
//   1-2-3-4-5-6
//       |
//       7-8-9-10
//         |
//         11-12
Node* build_example_list(NodeArena& arena) {
  std::vector<Node*> level1;
  std::vector<Node*> level2;
  Node* head = arena.make_list({1, 2, 3, 4, 5, 6}, &level1);
  Node* l2 = arena.make_list({7, 8, 9, 10}, &level2);
  Node* l3 = arena.make_list({11, 12});

  level1[2]->child = l2;  // 3 -> child 7
  level2[1]->child = l3;  // 8 -> child 11
  return head;
}

// Verifies forward traversal, reverse traversal via `prev`, `head->prev ==
// nullptr`, and that every `child` pointer is `nullptr`.
void verify_flattened(lc::TestContext& ctx, const Node* head,
                      const std::vector<int>& expected) {
  if (expected.empty()) {
    ctx.expect_true(head == nullptr);
    return;
  }

  ctx.expect_true(head != nullptr);
  if (head == nullptr) {
    return;
  }
  ctx.expect_true(head->prev == nullptr);

  std::vector<int> forward;
  const Node* prev = nullptr;
  const Node* curr = head;
  while (curr != nullptr) {
    ctx.expect_true(curr->child == nullptr);
    ctx.expect_true(curr->prev == prev);
    forward.push_back(curr->val);
    prev = curr;
    curr = curr->next;
  }
  ctx.expect_eq(forward, expected);

  std::vector<int> backward;
  while (prev != nullptr) {
    backward.push_back(prev->val);
    prev = prev->prev;
  }
  const std::vector<int> expected_rev(expected.rbegin(), expected.rend());
  ctx.expect_eq(backward, expected_rev);
}

LC_PROBLEM(430, "Flatten A Multilevel Doubly Linked List", "tree", ctx) {
  ctx.test("example_1_three_levels", [&] {
    const std::vector<int> expected = {1, 2, 3, 7, 8, 11, 12, 9, 10, 4, 5, 6};
    Solution sol;

    NodeArena arena1;
    verify_flattened(ctx, sol.flatten(build_example_list(arena1)), expected);

    NodeArena arena2;
    verify_flattened(ctx, sol.flattenRecursive(build_example_list(arena2)),
                     expected);
  });

  ctx.test("empty_and_single_node", [&] {
    Solution sol;
    verify_flattened(ctx, sol.flatten(nullptr), {});
    verify_flattened(ctx, sol.flattenRecursive(nullptr), {});

    NodeArena arena1;
    verify_flattened(ctx, sol.flatten(arena1.make(9)), {9});

    NodeArena arena2;
    verify_flattened(ctx, sol.flattenRecursive(arena2.make(9)), {9});
  });

  ctx.test("vertical_child_only_chain", [&] {
    // 1 -> child 2 -> child 3 -> child 4 (no next pointers at all)
    const std::vector<int> expected = {1, 2, 3, 4};
    Solution sol;

    auto make_chain = [](NodeArena& arena) {
      Node* n1 = arena.make(1);
      Node* n2 = arena.make(2);
      Node* n3 = arena.make(3);
      Node* n4 = arena.make(4);
      n1->child = n2;
      n2->child = n3;
      n3->child = n4;
      return n1;
    };

    NodeArena arena1;
    verify_flattened(ctx, sol.flatten(make_chain(arena1)), expected);

    NodeArena arena2;
    verify_flattened(ctx, sol.flattenRecursive(make_chain(arena2)), expected);
  });

  ctx.test("child_on_tail_node", [&] {
    // 1-2 -> child on tail 2: 3-4
    const std::vector<int> expected = {1, 2, 3, 4};
    Solution sol;

    auto make_case = [](NodeArena& arena) {
      std::vector<Node*> top;
      Node* head = arena.make_list({1, 2}, &top);
      top[1]->child = arena.make_list({3, 4});
      return head;
    };

    NodeArena arena1;
    verify_flattened(ctx, sol.flatten(make_case(arena1)), expected);

    NodeArena arena2;
    verify_flattened(ctx, sol.flattenRecursive(make_case(arena2)), expected);
  });
}

}  // namespace
