// LeetCode 141: Linked List Cycle
// https://leetcode.com/problems/linked-list-cycle/
//
// Problem:
//   Given the head of a singly linked list, determine whether the linked list
//   contains a cycle (i.e., some node in the list can be reached again by
//   continuously following `next` pointers).
//   Follow-up: solve using O(1) constant auxiliary memory.
//   Constraints: 0 <= number of nodes <= 10^4, -10^5 <= Node.val <= 10^5.
//
// Examples:
//   Example 1:
//     Input:  head = [3, 2, 0, -4], pos = 1
//     Output: true
//     Explanation:
//       - Initialize `slow` and `fast` at index `0` (value `3`), where index `3`
//         (`-4`) links back to index `1` (`2`).
//       - Step 1 advances `slow` to index `1` (`2`) and `fast` to index `2` (`0`).
//       - Step 2 advances `slow` to index `2` (`0`) and `fast` across `-4` to index
//         `1` (`2`).
//       - Step 3 advances `slow` to index `3` (`-4`) and `fast` across `0` to index
//         `3` (`-4`), so `slow == fast` and `hasCycle` returns `true`.
//
//   Example 2:
//     Input:  head = [1, 2, 3, 4, 5], pos = -1
//     Output: false
//     Explanation:
//       - Initialize `slow` and `fast` at index `0` (value `1`) on the acyclic list
//         `1 -> 2 -> 3 -> 4 -> 5 -> nullptr`.
//       - Step 1 moves `slow` to `2` and `fast` to `3`; Step 2 moves `slow` to `3`
//         and `fast` to `5`.
//       - Now `fast->next == nullptr`, so the loop exits and returns `false`.
//
//   Example 3:
//     Input:  head = [1], pos = -1
//     Output: false
//     Explanation:
//       - Initialize `slow` and `fast` at the single node `1`, whose `next` is
//         `nullptr`.
//       - Since `fast->next == nullptr`, the `while` loop never executes and
//         `hasCycle` returns `false`.
//
// Algorithm:
//   Floyd's Tortoise and Hare (Cycle-Finding) Algorithm:
//   1. Initialize two pointers `slow` and `fast` at `head`.
//   2. Repeatedly advance `slow` by 1 step and `fast` by 2 steps while
//      `fast != nullptr && fast->next != nullptr`.
//   3. If the list is acyclic, `fast` reaches `nullptr` and we return `false`.
//   4. If the list contains a cycle of length `C`, once both pointers enter
//      the cycle the distance between `fast` and `slow` closes by 1 step per
//      iteration (relative speed 2 - 1 = 1), guaranteeing they meet inside the
//      cycle in at most `C` additional steps; when `slow == fast`, return
//      `true`.
//   Note: At the meeting point, `slow`'s total steps is a multiple of `C`;
//   resetting one pointer to `head` and stepping both by 1 finds the cycle
//   entry (LeetCode #142).
//   Time Complexity: O(n), where n is the number of nodes in the list.
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - Singly linked list (`ListNode`).
//   - Two runner pointers (`slow`, `fast`) advancing at speeds 1 and 2.
//
// Real-World Applications:
//   - Detecting circular references in garbage collectors, symbolic-link
//     resolvers, and kernel intrusive lists before infinite traversal occurs.
//   - Pollard's rho integer-factorization and discrete-logarithm algorithms
//     in computational number theory / cryptography.
//   - Detecting periodic states in pseudorandom number generators (PRNGs) and
//     finite-state transition graphs with minimal memory.
//
// Related LeetCode Problems:
//   - #142 Linked List Cycle II [done] (returns the exact cycle entry node)
//   - #202 Happy Number [done] (Floyd cycle detection on an implicit sequence)
//   - #287 Find the Duplicate Number [done] (array-index graph cycle finding)
//   - #876 Middle of the Linked List [done] (same slow/fast two-pointer cadence)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <optional>
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
  // Detects whether the singly linked list contains a cycle using Floyd's
  // Tortoise and Hare algorithm.
  // Invariant: if a cycle exists, once `slow` enters it, `fast` closes the
  // cyclic gap by 1 node per iteration and must collide with `slow`.
  // Time: O(n), Space: O(1).
  bool hasCycle(const ListNode* head) {
    const ListNode* slow = head;
    const ListNode* fast = head;

    while (fast != nullptr && fast->next != nullptr) {
      slow = slow->next;
      fast = fast->next->next;
      if (slow == fast) {
        return true;
      }
    }

    return false;
  }
};

// RAII node arena backed by std::deque so even cyclic test lists are destroyed
// cleanly in linear time without traversing cyclic `next` links.
struct ListArena {
  std::deque<ListNode> nodes;

  ListNode* build(const std::vector<int>& values,
                  std::optional<std::size_t> cycle_pos = std::nullopt) {
    if (values.empty()) {
      return nullptr;
    }
    std::vector<ListNode*> ptrs;
    ptrs.reserve(values.size());
    for (const int v : values) {
      nodes.emplace_back(v);
      ptrs.push_back(&nodes.back());
    }
    for (std::size_t i = 0uz; (i + 1uz) < ptrs.size(); ++i) {
      ptrs[i]->next = ptrs[i + 1uz];
    }
    if (cycle_pos.has_value()) {
      ptrs.back()->next = ptrs[*cycle_pos];
    }
    return ptrs.front();
  }
};

LC_PROBLEM(141, "Linked List Cycle", "linked_list", ctx) {
  Solution sol;

  ctx.test("example_1_cycle_at_index_1", [&] {
    ListArena arena;
    ListNode* head = arena.build({3, 2, 0, -4}, 1uz);
    ctx.expect_true(sol.hasCycle(head));
  });

  ctx.test("example_2_two_node_cycle", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2}, 0uz);
    ctx.expect_true(sol.hasCycle(head));
  });

  ctx.test("example_3_single_node_no_cycle", [&] {
    ListArena arena;
    ListNode* head = arena.build({1});
    ctx.expect_false(sol.hasCycle(head));
  });

  ctx.test("python_build_five_node_cycle", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3, 4, 5}, 2uz);
    ctx.expect_true(sol.hasCycle(head));
  });

  ctx.test("single_node_self_loop", [&] {
    ListArena arena;
    ListNode* head = arena.build({7}, 0uz);
    ctx.expect_true(sol.hasCycle(head));
  });

  ctx.test("empty_and_acyclic_lists", [&] {
    ctx.expect_false(sol.hasCycle(nullptr));

    ListArena arena;
    ListNode* two = arena.build({1, 2});
    ctx.expect_false(sol.hasCycle(two));

    ListNode* five = arena.build({1, 2, 3, 4, 5});
    ctx.expect_false(sol.hasCycle(five));
  });
}

}  // namespace
