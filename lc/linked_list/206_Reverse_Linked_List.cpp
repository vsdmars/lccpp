// LeetCode 206: Reverse Linked List
// https://leetcode.com/problems/reverse-linked-list/
//
// Problem:
//   Given the head of a singly linked list, reverse the list in-place and
//   return the reversed list's new head.
//
// Algorithm:
//   1. Iterative Three-Pointer Reversal (`reverseList`):
//      Maintain `prev` (the head of the already-reversed prefix, initially
//      `nullptr`) and `curr` (the next unreversed node, initially `head`). On
//      each step, save `next_node = curr->next`, redirect `curr->next = prev`,
//      and advance `prev = curr`, `curr = next_node`. When `curr` becomes
//      `nullptr`, `prev` points to the new head.
//      Time Complexity: O(n).
//      Space Complexity: O(1) auxiliary space.
//
//   2. Recursive Reversal (`reverseListRecursive`):
//      Base case: an empty or single-node list is already reversed. Recursively
//      reverse `head->next` to obtain `new_head`, then flip the local edge via
//      `head->next->next = head` and `head->next = nullptr`.
//      Time Complexity: O(n).
//      Space Complexity: O(n) call-stack space.
//
// Data Structures:
//   Singly linked list mutated in-place using three raw traversal pointers
//   (`prev`, `curr`, `next_node`).
//
// Real-World Applications:
//   Reversing LIFO lock-free intrusive stacks (Treiber stack batch draining to
//   recover FIFO submission order), undo/redo history inversion, and serving as
//   a building block for in-place palindrome checking and k-group list
//   reordering.
//
// Related LeetCode Problems:
//   - #92 Reverse Linked List II [done] (reversing a contiguous sublist [left, right])
//   - #25 Reverse Nodes in k-Group [done] (iteratively reversing fixed-size chunks)
//   - #234 Palindrome Linked List [done] (uses in-place reversal on the second half)
//   - #143 Reorder List [done] (uses in-place reversal before interleaving halves)

#include "framework/lc_framework.h"

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
  // Reverses a singly linked list iteratively in-place and returns the new
  // head.
  // Time: O(n), Space: O(1).
  ListNode* reverseList(ListNode* head) {
    ListNode* prev = nullptr;
    ListNode* curr = head;

    // Invariant: `prev` is the head of the reversed prefix processed so far,
    // and `curr` is the head of the remaining unreversed suffix.
    while (curr != nullptr) {
      ListNode* next_node = curr->next;
      curr->next = prev;
      prev = curr;
      curr = next_node;
    }

    return prev;
  }

  // Reverses a singly linked list recursively and returns the new head.
  // Time: O(n), Space: O(n) call-stack depth.
  ListNode* reverseListRecursive(ListNode* head) {
    if (head == nullptr || head->next == nullptr) {
      return head;
    }

    // Recursively reverse the suffix starting at `head->next`; `head->next`
    // becomes the tail of that reversed suffix.
    ListNode* new_head = reverseListRecursive(head->next);
    head->next->next = head;
    head->next = nullptr;
    return new_head;
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

// Serializes a singly linked list into a vector of values for assertions.
std::vector<int> to_vector(const ListNode* head) {
  std::vector<int> out;
  for (const ListNode* curr = head; curr != nullptr; curr = curr->next) {
    out.push_back(curr->val);
  }
  return out;
}

LC_PROBLEM(206, "Reverse Linked List", "linked_list", ctx) {
  ctx.test("example_1_five_nodes", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3, 4, 5});
    Solution sol;
    ctx.expect_eq(to_vector(sol.reverseList(head)),
                  std::vector<int>{5, 4, 3, 2, 1});
  });

  ctx.test("example_2_two_nodes", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2});
    Solution sol;
    ctx.expect_eq(to_vector(sol.reverseList(head)), std::vector<int>{2, 1});
  });

  ctx.test("example_3_empty_and_single", [&] {
    Solution sol;
    ctx.expect_eq(sol.reverseList(nullptr), nullptr);

    ListArena arena;
    ListNode* single = arena.build({42});
    ctx.expect_eq(to_vector(sol.reverseList(single)), std::vector<int>{42});
  });

  ctx.test("negatives_and_duplicates", [&] {
    ListArena arena;
    ListNode* head = arena.build({-3, 0, -3, 7, 100});
    Solution sol;
    ctx.expect_eq(to_vector(sol.reverseList(head)),
                  std::vector<int>{100, 7, -3, 0, -3});
  });

  ctx.test("recursive_variant_matches_iterative", [&] {
    Solution sol;
    ctx.expect_eq(sol.reverseListRecursive(nullptr), nullptr);

    ListArena arena1;
    ListNode* h1 = arena1.build({1, 2, 3, 4, 5});
    ctx.expect_eq(to_vector(sol.reverseListRecursive(h1)),
                  std::vector<int>{5, 4, 3, 2, 1});

    ListArena arena2;
    ListNode* h2 = arena2.build({99});
    ctx.expect_eq(to_vector(sol.reverseListRecursive(h2)),
                  std::vector<int>{99});
  });
}

}  // namespace
