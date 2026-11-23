// LeetCode 61: Rotate List
// https://leetcode.com/problems/rotate-list/
//
// Problem:
//   Given the head of a singly linked list and a non-negative integer k,
//   rotate the list to the right by k places and return the new head.
//   A single right rotation moves the last node to the front of the list.
//   Constraints: 0 <= number of nodes <= 500, -100 <= Node.val <= 100,
//   0 <= k <= 2 * 10^9.
//
// Algorithm:
//   1. Handle trivial lists (empty or single-node) or k == 0 immediately.
//   2. Compute the list length `n` in one linear pass and reduce `k %= n`,
//      since rotating by a multiple of `n` yields the original list. If the
//      reduced `k` is 0, return `head` unchanged.
//   3. Attach a stack-allocated dummy sentinel before `head` and place two
//      pointers (`slow` and `fast`) at the dummy. Advance `fast` by `k` steps
//      so `fast` leads `slow` by exactly `k` nodes.
//   4. Advance both pointers in lockstep until `fast->next == nullptr`. At
//      that point, `fast` is the old tail, `slow` is the new tail, and
//      `slow->next` is the new head (the k-th node from the end).
//   5. Rewire three pointers in O(1): save `new_head = slow->next`, cut the
//      list with `slow->next = nullptr`, and splice `fast->next = head`.
//   Time Complexity: O(n), where n is the number of nodes (two linear scans).
//   Space Complexity: O(1) auxiliary space (in-place pointer rewiring).
//
// Data Structures:
//   - Singly linked list (`ListNode`) with a stack-allocated dummy sentinel
//     to simplify boundary pointer updates without heap allocation.
//   - Two-pointer window (`slow`, `fast`) separated by `k % n` nodes to
//     locate the split point and original tail simultaneously.
//
// Real-World Applications:
//   - Round-robin process/task schedulers rotating an intrusive ready queue
//     by a quantum offset without copying task descriptors.
//   - Ring-buffer and circular playlist cursor shifts when advancing or
//     rewinding playback windows in media engines.
//   - Carousel/viewport pagination over linked page caches in UI toolkits.
//
// Related LeetCode Problems:
//   - #19 Remove Nth Node From End of List [done] (same k-gap two-pointer locator)
//   - #92 Reverse Linked List II [done] (in-place linked-list splicing with dummy)
//   - #189 Rotate Array [done] (array analogue of right rotation by k)
//   - #725 Split Linked List in Parts [done] (length measurement + list splitting)
//   - #143 Reorder List [done] (multi-pass list partitioning and rewiring)

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
  explicit ListNode(int x) : val(x) {}
  ListNode(int x, ListNode* nxt) : val(x), next(nxt) {}
};

class Solution {
 public:
  // Rotates the linked list to the right by `k` places in-place.
  // Invariant: after reducing `k %= len`, `fast` stays `k` nodes ahead of
  // `slow` so that when `fast` reaches the tail, `slow->next` is the new head.
  // Time: O(n), Space: O(1).
  ListNode* rotateRight(ListNode* head, int k) {
    // Empty, single-node, or zero-shift lists require no pointer changes.
    if (head == nullptr || head->next == nullptr || k == 0) {
      return head;
    }

    // Pass 1: count the total number of nodes so we can fold large k modulo n.
    int length = 0;
    for (const ListNode* cur = head; cur != nullptr; cur = cur->next) {
      ++length;
    }

    const int shift = k % length;
    if (shift == 0) {
      return head;
    }

    // Pass 2: maintain a fixed gap of `shift` nodes between `slow` and `fast`.
    ListNode dummy(0, head);
    ListNode* slow = &dummy;
    ListNode* fast = &dummy;

    for (int i = 0; i < shift && fast != nullptr; ++i) {
      fast = fast->next;
    }

    if (fast == nullptr) {
      return head;
    }

    // Move both pointers together until `fast` rests on the original tail.
    while (fast->next != nullptr) {
      fast = fast->next;
      slow = slow->next;
    }

    // Break the list after the new tail (`slow`) and attach the old tail to
    // the original head to complete the rotation.
    ListNode* new_head = slow->next;
    slow->next = nullptr;
    fast->next = head;

    return new_head;
  }
};

// RAII node arena backed by std::deque so node addresses remain stable and all
// allocated nodes are automatically freed at scope exit.
struct ListArena {
  std::deque<ListNode> nodes;

  ListNode* build(const std::vector<int>& values) {
    ListNode dummy(0);
    ListNode* tail = &dummy;
    for (const int v : values) {
      nodes.emplace_back(v);
      tail->next = &nodes.back();
      tail = tail->next;
    }
    return dummy.next;
  }
};

std::vector<int> to_vector(const ListNode* head) {
  std::vector<int> out;
  for (const ListNode* cur = head; cur != nullptr; cur = cur->next) {
    out.push_back(cur->val);
  }
  return out;
}

LC_PROBLEM(61, "Rotate List", "linked_list", ctx) {
  Solution sol;

  ctx.test("example_1_rotate_by_2", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3, 4, 5});
    ctx.expect_eq(to_vector(sol.rotateRight(head, 2)),
                  std::vector<int>{4, 5, 1, 2, 3});
  });

  ctx.test("example_2_k_greater_than_length", [&] {
    ListArena arena;
    ListNode* head = arena.build({0, 1, 2});
    ctx.expect_eq(to_vector(sol.rotateRight(head, 4)),
                  std::vector<int>{2, 0, 1});
  });

  ctx.test("python_build_multiple_of_length", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3});
    ctx.expect_eq(to_vector(sol.rotateRight(head, 198)),
                  std::vector<int>{1, 2, 3});
  });

  ctx.test("empty_and_single_node", [&] {
    ctx.expect_eq(to_vector(sol.rotateRight(nullptr, 5)), std::vector<int>{});

    ListArena arena;
    ListNode* single = arena.build({42});
    ctx.expect_eq(to_vector(sol.rotateRight(single, 100)),
                  std::vector<int>{42});
  });

  ctx.test("zero_rotation_and_huge_k", [&] {
    ListArena arena;
    ListNode* head1 = arena.build({10, 20, 30, 40});
    ctx.expect_eq(to_vector(sol.rotateRight(head1, 0)),
                  std::vector<int>{10, 20, 30, 40});

    ListNode* head2 = arena.build({10, 20, 30, 40});
    // 2'000'000'001 % 4 == 1
    ctx.expect_eq(to_vector(sol.rotateRight(head2, 2'000'000'001)),
                  std::vector<int>{40, 10, 20, 30});
  });
}

}  // namespace
