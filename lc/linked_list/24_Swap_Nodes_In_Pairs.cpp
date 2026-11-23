// LeetCode 24: Swap Nodes In Pairs
// https://leetcode.com/problems/swap-nodes-in-pairs/
//
// Problem:
//   Given the head of a singly-linked list, swap every two adjacent nodes and
//   return the modified list's head. Node values may not be altered—only the
//   node links themselves may be rewired—and the algorithm must use O(1) extra
//   space.
//
// Algorithm:
//   Iterative pairwise pointer rewiring anchored by a stack-allocated dummy
//   sentinel `dummy` before `head`:
//   - Maintain `prev` pointing to the node immediately before the next pair to
//     swap.
//   - While two nodes exist ahead (`prev->next != nullptr` and
//     `prev->next->next != nullptr`), let `first = prev->next` and
//     `second = first->next`.
//   - Rewire `prev -> first -> second -> rest` into
//     `prev -> second -> first -> rest` in three pointer assignments:
//       first->next = second->next;
//       second->next = first;
//       prev->next = second;
//   - Advance `prev = first` (now the tail of the swapped pair) and continue.
//   If an odd trailing node remains, the loop exits and leaves it in place.
//   Time Complexity: O(n), visiting each node once.
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - Singly-linked list (`ListNode`) modified in place.
//   - Stack-allocated dummy sentinel node to avoid special-casing the head swap.
//
// Real-World Applications:
//   - Local pointer-rewiring primitives inside intrusive kernel/allocator lists
//     and lock-free data structures where node identity/addresses must persist
//     (external handles point directly to nodes rather than copying payloads).
//   - Pairwise reordering passes in low-level packet/buffer descriptor chains.
//
// Related LeetCode Problems:
//   - #25 Reverse Nodes In K Group [done] (k-node generalization of k = 2)
//   - #206 Reverse Linked List [done] (fundamental in-place link reversal)
//   - #92 Reverse Linked List II [done] (reversing a bounded sublist)
//   - #1721 Swapping Nodes in a Linked List [new] (swapping symmetric nodes)
//   - #328 Odd Even Linked List [done] (in-place alternating link rewiring)

#include "framework/lc_framework.h"

#include <deque>
#include <vector>

namespace {

// Standard LeetCode singly-linked list node.
struct ListNode {
  int val = 0;
  ListNode* next = nullptr;

  explicit ListNode(int x, ListNode* nxt = nullptr) : val(x), next(nxt) {}
};

class Solution {
 public:
  // Swaps every two adjacent nodes in place by relinking pointers.
  // Time: O(n), Space: O(1).
  ListNode* swapPairs(ListNode* head) {
    ListNode dummy(0, head);
    ListNode* prev = &dummy;

    // Only swap when a complete pair (first, second) is available ahead.
    while (prev->next != nullptr && prev->next->next != nullptr) {
      ListNode* first = prev->next;
      ListNode* second = first->next;

      // Rewire: prev -> first -> second -> rest  ==>  prev -> second -> first -> rest
      first->next = second->next;
      second->next = first;
      prev->next = second;

      // `first` is now the second node in the swapped pair, right before the
      // next pair.
      prev = first;
    }

    return dummy.next;
  }
};

// Arena owning all test nodes via `std::deque` (stable pointers, zero leaks).
class ListArena {
 public:
  ListNode* build(const std::vector<int>& vals) {
    ListNode* head = nullptr;
    ListNode* tail = nullptr;
    for (const int v : vals) {
      nodes_.emplace_back(v);
      ListNode* curr = &nodes_.back();
      if (head == nullptr) {
        head = curr;
      } else {
        tail->next = curr;
      }
      tail = curr;
    }
    return head;
  }

 private:
  std::deque<ListNode> nodes_;
};

std::vector<int> to_vector(const ListNode* head) {
  std::vector<int> out;
  while (head != nullptr) {
    out.push_back(head->val);
    head = head->next;
  }
  return out;
}

LC_PROBLEM(24, "Swap Nodes In Pairs", "linked_list", ctx) {
  Solution sol;

  ctx.test("example_1_even_length", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3, 4});
    ctx.expect_eq(to_vector(sol.swapPairs(head)),
                  std::vector<int>{2, 1, 4, 3});
  });

  ctx.test("python_build_case", [&] {
    ListArena arena;
    ListNode* head = arena.build({7, 2, 4, 3});
    ctx.expect_eq(to_vector(sol.swapPairs(head)),
                  std::vector<int>{2, 7, 3, 4});
  });

  ctx.test("odd_length_preserves_last", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3, 4, 5});
    ctx.expect_eq(to_vector(sol.swapPairs(head)),
                  std::vector<int>{2, 1, 4, 3, 5});
  });

  ctx.test("single_node", [&] {
    ListArena arena;
    ListNode* head = arena.build({1});
    ctx.expect_eq(to_vector(sol.swapPairs(head)), std::vector<int>{1});
  });

  ctx.test("two_nodes", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2});
    ctx.expect_eq(to_vector(sol.swapPairs(head)), std::vector<int>{2, 1});
  });

  ctx.test("empty_list", [&] {
    ctx.expect_eq(sol.swapPairs(nullptr), nullptr);
  });
}

}  // namespace
