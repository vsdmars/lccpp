// LeetCode 143: Reorder List
// https://leetcode.com/problems/reorder-list/
//
// Problem:
//   Given the head of a singly linked list L0 -> L1 -> ... -> Ln-1 -> Ln,
//   reorder the list in-place into L0 -> Ln -> L1 -> Ln-1 -> L2 -> Ln-2 -> ...
//   Only node links may be modified; node values themselves must not be
//   altered.
//
// Algorithm:
//   Primary approach (O(1)-space in-place split, reverse, and interleave):
//   1. Split at middle: Advance `slow` by 1 and `fast` by 2 until `fast` reaches
//      the penultimate or last node. `slow` now points to the end of the first
//      half (length ceil(n/2)), and `slow->next` starts the second half (length
//      floor(n/2)). Cut the list by setting `slow->next = nullptr`.
//   2. Reverse second half: Reverse the second half in-place using standard
//      three-pointer reversal (`prev`, `curr`, `next_node`).
//   3. Interleave halves: Repeatedly splice the front node of the reversed
//      second half directly after the current node of the first half until the
//      second half is exhausted.
//   Time Complexity: O(n).
//   Space Complexity: O(1) auxiliary space.
//
//   Secondary approach (`reorderListStack`, matching the stack variant):
//   Push all node pointers onto a vector/stack, then pop the last `n / 2` nodes
//   one by one and weave them between the first `n / 2` nodes, terminating the
//   final node's `next` pointer with `nullptr`.
//   Time Complexity: O(n), Space Complexity: O(n).
//
// Data Structures:
//   Singly linked list manipulated in-place via pointer relinking (primary),
//   and a `std::vector<ListNode*>` used as a LIFO stack (secondary variant).
//
// Real-World Applications:
//   Booklet/signature page imposition (interleaving front and back pages for
//   duplex printing), in-place buffer folding in signal processing and audio
//   crossfading, and cache-friendly intrusive list reorganization in systems
//   schedulers.
//
// Related LeetCode Problems:
//   - #206 Reverse Linked List [done] (core in-place reversal subroutine)
//   - #234 Palindrome Linked List [done] (same split-at-middle + reverse-second-half pattern)
//   - #876 Middle of the Linked List [done] (fast/slow midpoint finding)
//   - #21 Merge Two Sorted Lists [done] (two-list pointer weaving/merging)
//   - #328 Odd Even Linked List [done] (in-place linked list partitioning and rejoining)

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
  // Reorders the linked list in-place into L0 -> Ln -> L1 -> Ln-1 -> ...
  // using the optimal O(1)-space split, reverse, and interleave algorithm.
  // Time: O(n), Space: O(1).
  void reorderList(ListNode* head) {
    if (head == nullptr || head->next == nullptr) {
      return;
    }

    // Phase 1: Locate the end of the first half via slow/fast pointers.
    // Stopping when fast->next or fast->next->next is null ensures the first
    // half holds ceil(n / 2) nodes and the second half holds floor(n / 2).
    ListNode* slow = head;
    ListNode* fast = head;
    while (fast->next != nullptr && fast->next->next != nullptr) {
      slow = slow->next;
      fast = fast->next->next;
    }

    // Phase 2: Detach and reverse the second half in-place.
    ListNode* second = reverse_list(slow->next);
    slow->next = nullptr;

    // Phase 3: Interleave nodes from `first` and reversed `second`.
    // Invariant: remaining length of `first` >= remaining length of `second`.
    ListNode* first = head;
    while (first != nullptr && second != nullptr) {
      ListNode* next_first = first->next;
      ListNode* next_second = second->next;

      first->next = second;
      second->next = next_first;

      first = next_first;
      second = next_second;
    }
  }

  // Alternative stack-based reordering using O(n) extra space to access tail
  // nodes in reverse order without reversing the second half first.
  // Time: O(n), Space: O(n).
  void reorderListStack(ListNode* head) {
    if (head == nullptr || head->next == nullptr) {
      return;
    }

    std::vector<ListNode*> stack;
    for (ListNode* curr = head; curr != nullptr; curr = curr->next) {
      stack.push_back(curr);
    }

    // Interleave the last floor(n / 2) nodes from the stack into the front.
    const std::size_t half_count = stack.size() / 2uz;
    ListNode* curr = head;
    for (std::size_t i = 0uz; i < half_count && curr != nullptr; ++i) {
      ListNode* back_node = stack.back();
      stack.pop_back();

      ListNode* next_node = curr->next;
      curr->next = back_node;
      back_node->next = next_node;
      curr = next_node;
    }

    // Terminate the reordered list to prevent a cycle on the middle node.
    if (curr != nullptr) {
      curr->next = nullptr;
    }
  }

 private:
  // Reverses a singly linked list in-place and returns its new head.
  static ListNode* reverse_list(ListNode* head) {
    ListNode* prev = nullptr;
    ListNode* curr = head;
    while (curr != nullptr) {
      ListNode* next_node = curr->next;
      curr->next = prev;
      prev = curr;
      curr = next_node;
    }
    return prev;
  }
};

// RAII arena managing ListNode storage for tests without heap leaks.
class ListArena {
 public:
  // Constructs a linked list from `values` and returns its head pointer.
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

// Converts a singly linked list into a vector of values for assertion checks.
std::vector<int> to_vector(const ListNode* head) {
  std::vector<int> out;
  for (const ListNode* curr = head; curr != nullptr; curr = curr->next) {
    out.push_back(curr->val);
  }
  return out;
}

LC_PROBLEM(143, "Reorder List", "linked_list", ctx) {
  ctx.test("example_1_even_length", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3, 4});
    Solution sol;
    sol.reorderList(head);
    ctx.expect_eq(to_vector(head), std::vector<int>{1, 4, 2, 3});
  });

  ctx.test("example_2_odd_length", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3, 4, 5});
    Solution sol;
    sol.reorderList(head);
    ctx.expect_eq(to_vector(head), std::vector<int>{1, 5, 2, 4, 3});
  });

  ctx.test("empty_and_small_lists", [&] {
    Solution sol;
    sol.reorderList(nullptr);

    ListArena arena1;
    ListNode* single = arena1.build({7});
    sol.reorderList(single);
    ctx.expect_eq(to_vector(single), std::vector<int>{7});

    ListArena arena2;
    ListNode* pair = arena2.build({1, 2});
    sol.reorderList(pair);
    ctx.expect_eq(to_vector(pair), std::vector<int>{1, 2});

    ListArena arena3;
    ListNode* triple = arena3.build({1, 2, 3});
    sol.reorderList(triple);
    ctx.expect_eq(to_vector(triple), std::vector<int>{1, 3, 2});
  });

  ctx.test("stack_variant_even_and_odd", [&] {
    Solution sol;

    ListArena arena_even;
    ListNode* even_head = arena_even.build({1, 2, 3, 4, 5, 6});
    sol.reorderListStack(even_head);
    ctx.expect_eq(to_vector(even_head), std::vector<int>{1, 6, 2, 5, 3, 4});

    ListArena arena_odd;
    ListNode* odd_head = arena_odd.build({1, 2, 3, 4, 5});
    sol.reorderListStack(odd_head);
    ctx.expect_eq(to_vector(odd_head), std::vector<int>{1, 5, 2, 4, 3});

    ListArena arena_two;
    ListNode* two_head = arena_two.build({10, 20});
    sol.reorderListStack(two_head);
    ctx.expect_eq(to_vector(two_head), std::vector<int>{10, 20});
  });
}

}  // namespace
