// LeetCode 237: Delete Node In A Linked List
// https://leetcode.com/problems/delete-node-in-a-linked-list/
//
// Problem:
//   There is a singly linked list `head` and we want to delete a given `node`
//   in it without having access to `head` or the predecessor of `node`. It is
//   guaranteed that `node` is not the tail node in the list. Deleting the node
//   means its value should no longer exist in the list, the list length
//   decreases by 1, and all values before and after `node` retain their
//   relative order.
//
// Algorithm:
//   Successor-copy and bypass (value overwrite trick):
//   Because a singly linked list provides no pointer back to the predecessor of
//   `node`, we cannot relink `prev->next` around `node`. Instead, we copy the
//   successor's value into `node` (`node->val = node->next->val`) and bypass
//   the successor node (`node->next = node->next->next`). Effectively, `node`
//   assumes the identity of its successor while the original successor is
//   unlinked from the chain.
//   Time Complexity: O(1).
//   Space Complexity: O(1).
//
// Data Structures:
//   Singly linked list modified in-place with direct `val` and `next` pointer
//   updates on `node` and `node->next`.
//
// Real-World Applications:
//   O(1) logical deletion in intrusive singly linked lists when only an
//   interior iterator/handle is available (a classic limitation contrasted with
//   doubly linked lists, explaining why OS kernels and `std::list` prefer
//   doubly linked structures or sentinel tails when arbitrary-handle removal is
//   needed, and why pointer-identity invariants break under value-copy
//   deletion).
//
// Related LeetCode Problems:
//   - #203 Remove Linked List Elements [done] (head-driven deletion by value)
//   - #19 Remove Nth Node From End of List [done] (predecessor-based node removal)
//   - #2095 Delete the Middle Node of a Linked List [new] (finding predecessor of middle node before unlinking)
//   - #83 Remove Duplicates from Sorted List [done] (in-place unlinking of redundant nodes)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <vector>

namespace {

// Standard LeetCode singly linked list node.
struct ListNode {
  int val = 0;
  ListNode* next = nullptr;

  ListNode() = default;
  explicit ListNode(int value) : val(value) {}
  ListNode(int value, ListNode* next_node) : val(value), next(next_node) {}
};

class Solution {
 public:
  // Logically deletes `node` from its singly linked list in O(1) time without
  // access to the head or predecessor by copying the successor's value into
  // `node` and unlinking the successor.
  // Precondition: `node` is non-null and is not the tail node (`node->next !=
  // nullptr`).
  // Time: O(1), Space: O(1).
  void deleteNode(ListNode* node) {
    if (node == nullptr || node->next == nullptr) {
      return;
    }

    // Overwrite `node` with its successor's payload and bypass the successor.
    // Note: In arena-owned tests the bypassed node is freed with the arena.
    ListNode* successor = node->next;
    node->val = successor->val;
    node->next = successor->next;
  }
};

// RAII arena that owns test ListNodes in a std::deque without manual deletes.
class ListArena {
 public:
  // Builds a linked list from `values` and returns its head pointer.
  ListNode* build(const std::vector<int>& values) {
    ListNode dummy(0);
    ListNode* tail = &dummy;
    for (const int val : values) {
      tail->next = &nodes_.emplace_back(val);
      tail = tail->next;
    }
    return dummy.next;
  }

 private:
  std::deque<ListNode> nodes_;
};

// Returns the node at 0-based `index` from `head`, or `nullptr` if out of
// bounds.
ListNode* node_at(ListNode* head, std::size_t index) {
  ListNode* curr = head;
  for (std::size_t i = 0uz; i < index && curr != nullptr; ++i) {
    curr = curr->next;
  }
  return curr;
}

// Serializes a singly linked list into a vector of values for assertions.
std::vector<int> to_vector(const ListNode* head) {
  std::vector<int> out;
  for (const ListNode* curr = head; curr != nullptr; curr = curr->next) {
    out.push_back(curr->val);
  }
  return out;
}

LC_PROBLEM(237, "Delete Node In A Linked List", "linked_list", ctx) {
  ctx.test("docstring_example_delete_third_node", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3, 4});
    Solution sol;
    sol.deleteNode(node_at(head, 2uz));
    ctx.expect_eq(to_vector(head), std::vector<int>{1, 2, 4});
  });

  ctx.test("leetcode_example_1_delete_second_node", [&] {
    ListArena arena;
    ListNode* head = arena.build({4, 5, 1, 9});
    Solution sol;
    sol.deleteNode(node_at(head, 1uz));
    ctx.expect_eq(to_vector(head), std::vector<int>{4, 1, 9});
  });

  ctx.test("leetcode_example_2_delete_third_node", [&] {
    ListArena arena;
    ListNode* head = arena.build({4, 5, 1, 9});
    Solution sol;
    sol.deleteNode(node_at(head, 2uz));
    ctx.expect_eq(to_vector(head), std::vector<int>{4, 5, 9});
  });

  ctx.test("delete_head_node", [&] {
    ListArena arena;
    ListNode* head = arena.build({4, 5, 1, 9});
    Solution sol;
    sol.deleteNode(head);
    ctx.expect_eq(to_vector(head), std::vector<int>{5, 1, 9});
  });

  ctx.test("two_node_list_delete_first", [&] {
    ListArena arena;
    ListNode* head = arena.build({10, 20});
    Solution sol;
    sol.deleteNode(head);
    ctx.expect_eq(to_vector(head), std::vector<int>{20});
  });
}

}  // namespace
