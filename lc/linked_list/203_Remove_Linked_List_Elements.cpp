// LeetCode 203: Remove Linked List Elements
// https://leetcode.com/problems/remove-linked-list-elements/
//
// Problem:
//   Given the head of a singly linked list and an integer `val`, remove all
//   nodes of the linked list that satisfy `node->val == val`, and return the
//   new head of the modified list.
//
// Examples:
//   Example 1:
//     Input:  head = [1, 2, 6, 3, 4, 5, 6], val = 6
//     Output: [1, 2, 3, 4, 5]
//     Explanation:
//       - Create sentinel dummy(0) pointing to node 1 and initialize prev = &dummy.
//       - Advance prev past nodes 1 and 2 since neither equals 6, then unlink node
//         6 after 2 by setting 2->next = 3 without advancing prev.
//       - Advance prev through nodes 3, 4, and 5, then unlink the trailing 6 by
//         setting 5->next = nullptr and return dummy.next ([1, 2, 3, 4, 5]).
//
//   Example 2:
//     Input:  head = [7, 7, 7, 7], val = 7
//     Output: nullptr
//     Explanation:
//       - Create sentinel dummy(0) pointing to the first 7 and set prev = &dummy.
//       - On four consecutive iterations, curr = prev->next has val == 7, so
//         prev->next is updated to curr->next while prev stays fixed at &dummy.
//       - Once all four 7 nodes are bypassed, dummy.next becomes nullptr, ending
//         the loop and returning nullptr.
//
//   Example 3:
//     Input:  head = [6, 6, 1, 6, 6, 2, 6], val = 6
//     Output: [1, 2]
//     Explanation:
//       - Start with prev at sentinel dummy(0) and bypass both leading 6 nodes in
//         place until dummy.next points to node 1.
//       - Advance prev to node 1, then bypass the two consecutive 6 nodes after 1
//         while keeping prev stationary until 1->next points to node 2.
//       - Advance prev to node 2, unlink the trailing 6 via 2->next = nullptr, and
//         return dummy.next ([1, 2]).
//
// Algorithm:
//   Sentinel (dummy) head with single-pass predecessor scanning:
//   Attach a stack-allocated sentinel node `dummy` pointing to `head`, which
//   eliminates special-case branching when the original head (or a prefix of
//   nodes) must be removed. Maintain a pointer `prev` initialized to `&dummy`.
//   While `prev->next != nullptr`:
//   - If `prev->next->val == val`, unlink `prev->next` by setting
//     `prev->next = prev->next->next`. Do NOT advance `prev`, because the new
//     `prev->next` has not yet been inspected (handles consecutive matches).
//   - Otherwise, keep `prev->next` and advance `prev = prev->next`.
//   Finally return `dummy.next`.
//   Time Complexity: O(n), visiting each node exactly once.
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   Singly linked list with a stack-allocated sentinel (`dummy`) ListNode and
//   one predecessor pointer (`prev`).
//
// Real-World Applications:
//   Purging expired or tombstoned entries from intrusive linked-list caches
//   and timer queues, filtering canceled tasks from OS scheduler runlists, and
//   pruning invalid records in streaming linked buffers.
//
// Related LeetCode Problems:
//   - #83 Remove Duplicates from Sorted List [done] (same predecessor-scanning unlink pattern)
//   - #82 Remove Duplicates from Sorted List II [done] (sentinel head + skipping runs of duplicate values)
//   - #237 Delete Node In A Linked List [done] (O(1) deletion when only the target node is given)
//   - #19 Remove Nth Node From End of List [done] (sentinel head + two-pointer predecessor lookup)
//   - #27 Remove Element [done] (in-place value-removal analogue on arrays)

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
  // Unlinks every node whose value equals `val` and returns the new list head.
  // Note: Unlinked nodes are not explicitly deleted here because ownership
  // belongs to the caller/arena.
  // Time: O(n), Space: O(1).
  ListNode* removeElements(ListNode* head, int val) {
    // Sentinel node unifies removing the head node with removing any interior
    // or tail node.
    ListNode dummy(0, head);
    ListNode* prev = &dummy;

    // Invariant: Every node from `dummy.next` up to and including `prev` has
    // already been verified to satisfy `node->val != val`.
    while (prev->next != nullptr) {
      ListNode* curr = prev->next;
      if (curr->val == val) {
        // Bypass `curr` and keep `prev` stationary so the next node is checked
        // on the following iteration.
        prev->next = curr->next;
      } else {
        prev = curr;
      }
    }

    return dummy.next;
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

LC_PROBLEM(203, "Remove Linked List Elements", "linked_list", ctx) {
  ctx.test("example_1_middle_and_tail_matches", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 6, 3, 4, 5, 6});
    Solution sol;
    ctx.expect_eq(to_vector(sol.removeElements(head, 6)),
                  std::vector<int>{1, 2, 3, 4, 5});
  });

  ctx.test("example_2_empty_list", [&] {
    Solution sol;
    ctx.expect_eq(sol.removeElements(nullptr, 1), nullptr);
  });

  ctx.test("example_3_all_nodes_removed", [&] {
    ListArena arena;
    ListNode* head = arena.build({7, 7, 7, 7});
    Solution sol;
    ctx.expect_eq(sol.removeElements(head, 7), nullptr);
  });

  ctx.test("leading_and_consecutive_matches", [&] {
    ListArena arena;
    ListNode* head = arena.build({6, 6, 1, 6, 6, 2, 6});
    Solution sol;
    ctx.expect_eq(to_vector(sol.removeElements(head, 6)),
                  std::vector<int>{1, 2});
  });

  ctx.test("no_matching_elements", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3, 4});
    Solution sol;
    ctx.expect_eq(to_vector(sol.removeElements(head, 99)),
                  std::vector<int>{1, 2, 3, 4});
  });
}

}  // namespace
