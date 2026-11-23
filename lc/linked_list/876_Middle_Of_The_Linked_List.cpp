// LeetCode 876: Middle Of The Linked List
// https://leetcode.com/problems/middle-of-the-linked-list/
//
// Problem:
//   Given the head of a singly linked list, return a pointer to the middle
//   node of the list. If the list has an even number of nodes (hence two
//   middle nodes), return the second middle node.
//
// Algorithm:
//   Floyd's slow-and-fast two-pointer traversal (tortoise and hare):
//   1. Initialize both `slow` and `fast` at `head`.
//   2. While `fast != nullptr && fast->next != nullptr`, advance `slow` by 1
//      step and `fast` by 2 steps.
//   3. Invariant: after `k` iterations, `slow` sits at 0-based index `k` and
//      `fast` sits at index `2k`.
//      - For odd length `n = 2m + 1`, the loop runs `m` times until `fast`
//        reaches index `2m` (the last node, `fast->next == nullptr`), placing
//        `slow` at index `m` (the exact middle).
//      - For even length `n = 2m`, the loop runs `m` times until `fast` steps
//        past the tail to `nullptr`, placing `slow` at index `m` (the second
//        middle node).
//   Time Complexity: O(n), a single half-stride pass over the list.
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - Singly linked list (`ListNode`): inspected in place using two pointers
//     (`slow` and `fast`) without counting length up front.
//
// Real-World Applications:
//   - Top-down merge sort on linked lists (`std::list::sort`) to bisect lists
//     in O(n) time without random-access indexing.
//   - Balanced BST construction from a sorted singly linked list by locating
//     the median root at each recursion level.
//   - Streaming buffer splitting and palindrome verification over forward-only
//     iterators.
//
// Related LeetCode Problems:
//   - #141 Linked List Cycle [done] (fast/slow pointer cycle detection)
//   - #2095 Delete the Middle Node of a Linked List [new] (finding and unlinking the middle node)
//   - #234 Palindrome Linked List [done] (halving a list with fast/slow pointers before reversing)
//   - #148 Sort List [done] (merge sort using fast/slow midpoint bisection)
//   - #725 Split Linked List In Parts [done] (generalizing list bisection to k parts)

#include "framework/lc_framework.h"

#include <deque>
#include <vector>

namespace {

// Singly linked list node matching LeetCode's definition.
struct ListNode {
  int val = 0;
  ListNode* next = nullptr;

  ListNode() = default;
  explicit ListNode(int x) : val(x) {}
  ListNode(int x, ListNode* next_node) : val(x), next(next_node) {}
};

// RAII arena that owns test ListNodes without manual new/delete.
class ListArena {
 public:
  // Builds a linked list from `vals` and returns its head pointer.
  [[nodiscard]] ListNode* build(const std::vector<int>& vals) {
    ListNode dummy;
    ListNode* tail = &dummy;
    for (const int v : vals) {
      nodes_.emplace_back(v);
      tail->next = &nodes_.back();
      tail = tail->next;
    }
    return dummy.next;
  }

 private:
  std::deque<ListNode> nodes_;
};

// Serializes a singly linked list from `head` to tail for test assertions.
[[nodiscard]] std::vector<int> to_vector(const ListNode* head) {
  std::vector<int> result;
  for (const ListNode* curr = head; curr != nullptr; curr = curr->next) {
    result.push_back(curr->val);
  }
  return result;
}

class Solution {
 public:
  // Returns the middle node of `head` (or the second middle node on even
  // lengths) in a single pass.
  // Time: O(n), Space: O(1).
  ListNode* middleNode(ListNode* head) {
    ListNode* slow = head;
    const ListNode* fast = head;

    // Advance `fast` at twice the speed of `slow`; when `fast` reaches the end
    // (last node for odd length, nullptr for even length), `slow` sits at index
    // floor(n / 2), which is the unique or second middle node.
    while (fast != nullptr && fast->next != nullptr) {
      slow = slow->next;
      fast = fast->next->next;
    }

    return slow;
  }
};

LC_PROBLEM(876, "Middle Of The Linked List", "linked_list", ctx) {
  Solution sol;

  ctx.test("example_1_odd_length", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3, 4, 5});
    ctx.expect_eq(to_vector(sol.middleNode(head)),
                  std::vector<int>({3, 4, 5}));
  });

  ctx.test("example_2_even_length_returns_second_middle", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3, 4, 5, 6});
    ctx.expect_eq(to_vector(sol.middleNode(head)),
                  std::vector<int>({4, 5, 6}));
  });

  ctx.test("single_node", [&] {
    ListArena arena;
    ListNode* head = arena.build({7});
    ctx.expect_eq(to_vector(sol.middleNode(head)), std::vector<int>({7}));
  });

  ctx.test("two_nodes_returns_second", [&] {
    ListArena arena;
    ListNode* head = arena.build({10, 20});
    ctx.expect_eq(to_vector(sol.middleNode(head)), std::vector<int>({20}));
  });

  ctx.test("empty_list", [&] {
    ctx.expect_eq(sol.middleNode(nullptr), nullptr);
  });
}

}  // namespace
