// LeetCode 142: Linked List Cycle II
// https://leetcode.com/problems/linked-list-cycle-ii/
//
// Problem:
//   Given the head of a singly linked list, return the node where the cycle
//   begins. If there is no cycle, return nullptr. A cycle exists when some node
//   in the list can be reached again by continuously following the `next`
//   pointer. The linked list must not be modified.
//
// Algorithm:
//   Floyd's Tortoise and Hare (two-phase cycle detection):
//   Phase 1 (Detect cycle): Advance `slow` by 1 step and `fast` by 2 steps from
//   `head`. If `fast` or `fast->next` reaches `nullptr`, the list is acyclic.
//   Otherwise `slow` and `fast` eventually meet inside the cycle.
//   Phase 2 (Locate cycle entry): Let `a` be the distance from `head` to the
//   cycle entry, `b` be the distance from the entry to the meeting point, and
//   `L` be the cycle length. At the meeting point, `fast` has traveled twice as
//   far as `slow`, so `2 * (a + b) = a + b + k * L`, which implies
//   `a = k * L - b = (k - 1) * L + (L - b)`. Therefore, resetting one pointer
//   to `head` and advancing both pointers 1 step at a time guarantees they
//   collide at the exact cycle entry after `a` steps.
//   Time Complexity: O(n), where n is the number of nodes in the list.
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   Singly linked list traversed in-place with two raw pointers (`slow`, `fast`)
//   and a third entry-finding pointer (`entry`); no hash set or extra storage
//   is needed.
//
// Real-World Applications:
//   Cycle detection in pointer-based memory graphs (garbage collection and leak
//   diagnostics), detecting periodic states in pseudo-random number generators
//   and finite state machines, Pollard's rho integer factorization, and
//   deadlock/loop detection in distributed routing tables.
//
// Related LeetCode Problems:
//   - #141 Linked List Cycle [done] (Phase 1 boolean cycle detection only)
//   - #287 Find the Duplicate Number [done] (applies Floyd's cycle entry search to an implicit array graph)
//   - #160 Intersection Of Two Linked Lists [done] (another O(1)-space two-pointer structural invariant)
//   - #202 Happy Number [done] (Floyd's cycle detection over a functional sequence)
//   - #457 Circular Array Loop [new] (directed cycle detection with index arithmetic)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <utility>
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
  // Finds and returns the first node of the cycle in the linked list starting
  // at `head`, or `nullptr` if the list contains no cycle.
  // Time: O(n), Space: O(1).
  ListNode* detectCycle(ListNode* head) {
    if (head == nullptr || head->next == nullptr) {
      return nullptr;
    }

    // Phase 1: Advance slow by 1 step and fast by 2 steps to detect a cycle.
    ListNode* slow = head;
    ListNode* fast = head;
    bool has_cycle = false;

    while (fast != nullptr && fast->next != nullptr) {
      slow = slow->next;
      fast = fast->next->next;
      if (slow == fast) {
        has_cycle = true;
        break;
      }
    }

    if (!has_cycle) {
      return nullptr;
    }

    // Phase 2: Because distance(head -> entry) == distance(meeting -> entry)
    // modulo cycle length, moving both pointers 1 step at a time meets at the
    // exact cycle entry node.
    ListNode* entry = head;
    while (entry != slow) {
      entry = entry->next;
      slow = slow->next;
    }

    return entry;
  }
};

// RAII arena that owns test ListNodes in a std::deque so cyclic `next` links
// never cause leaks or infinite recursion during teardown.
class ListArena {
 public:
  // Builds a linked list from `values` and optionally connects the tail node
  // back to the 0-indexed node at `cycle_pos` (pass -1 for an acyclic list).
  // Returns {head, expected_cycle_entry}.
  std::pair<ListNode*, ListNode*> build_with_cycle(
      const std::vector<int>& values, int cycle_pos) {
    if (values.empty()) {
      return {nullptr, nullptr};
    }

    std::vector<ListNode*> created;
    created.reserve(values.size());
    for (const int val : values) {
      created.push_back(&nodes_.emplace_back(val));
    }

    for (std::size_t i = 0uz; (i + 1uz) < created.size(); ++i) {
      created[i]->next = created[i + 1uz];
    }

    ListNode* entry = nullptr;
    if (cycle_pos >= 0 && std::cmp_less(cycle_pos, created.size())) {
      entry = created[static_cast<std::size_t>(cycle_pos)];
      created.back()->next = entry;
    }

    return {created.front(), entry};
  }

 private:
  std::deque<ListNode> nodes_;
};

LC_PROBLEM(142, "Linked List Cycle II", "linked_list", ctx) {
  ctx.test("example_1_cycle_at_index_1", [&] {
    ListArena arena;
    auto [head, expected] = arena.build_with_cycle({3, 2, 0, -4}, 1);
    Solution sol;
    ListNode* actual = sol.detectCycle(head);
    ctx.expect_eq(actual, expected);
    ctx.expect_true(actual != nullptr);
    if (actual != nullptr) {
      ctx.expect_eq(actual->val, 2);
    }
  });

  ctx.test("example_2_cycle_at_head", [&] {
    ListArena arena;
    auto [head, expected] = arena.build_with_cycle({1, 2}, 0);
    Solution sol;
    ListNode* actual = sol.detectCycle(head);
    ctx.expect_eq(actual, expected);
    ctx.expect_true(actual != nullptr);
    if (actual != nullptr) {
      ctx.expect_eq(actual->val, 1);
    }
  });

  ctx.test("example_3_single_node_no_cycle", [&] {
    ListArena arena;
    auto [head, expected] = arena.build_with_cycle({1}, -1);
    Solution sol;
    ctx.expect_eq(sol.detectCycle(head), expected);
    ctx.expect_eq(sol.detectCycle(head), nullptr);
  });

  ctx.test("python_build_cycle_at_node_3", [&] {
    ListArena arena;
    auto [head, expected] = arena.build_with_cycle({1, 2, 3, 4, 5}, 2);
    Solution sol;
    ListNode* actual = sol.detectCycle(head);
    ctx.expect_eq(actual, expected);
    ctx.expect_true(actual != nullptr);
    if (actual != nullptr) {
      ctx.expect_eq(actual->val, 3);
    }
  });

  ctx.test("empty_list", [&] {
    Solution sol;
    ctx.expect_eq(sol.detectCycle(nullptr), nullptr);
  });

  ctx.test("single_node_self_loop", [&] {
    ListArena arena;
    auto [head, expected] = arena.build_with_cycle({42}, 0);
    Solution sol;
    ctx.expect_eq(sol.detectCycle(head), expected);
  });

  ctx.test("tail_self_loop", [&] {
    ListArena arena;
    auto [head, expected] = arena.build_with_cycle({10, 20, 30}, 2);
    Solution sol;
    ListNode* actual = sol.detectCycle(head);
    ctx.expect_eq(actual, expected);
    ctx.expect_true(actual != nullptr);
    if (actual != nullptr) {
      ctx.expect_eq(actual->val, 30);
    }
  });

  ctx.test("longer_acyclic_list", [&] {
    ListArena arena;
    auto [head, expected] = arena.build_with_cycle({1, 2, 3, 4, 5, 6}, -1);
    Solution sol;
    ctx.expect_eq(sol.detectCycle(head), expected);
  });
}

}  // namespace
