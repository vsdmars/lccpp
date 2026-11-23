// LeetCode 82: Remove Duplicates From Sorted List II
// https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
//
// Problem:
//   Given the head of a sorted singly linked list, delete all nodes that have
//   duplicate numbers, leaving only numbers that appeared strictly once in the
//   original list. Return the resulting linked list sorted in ascending order.
//   Constraints: 0 <= number of nodes <= 300, -100 <= Node.val <= 100.
//
// Algorithm:
//   Use a dummy sentinel node pointing to `head`, and maintain two pointers:
//   - `prev`: the last confirmed unique node appended to the result prefix.
//   - `cur`: the leading edge scanning the current run of equal values.
//   For each run starting at `run_start = cur`:
//   1. Advance `cur` across all adjacent nodes sharing the same value.
//   2. If `cur == run_start`, the value occurred only once, so it belongs in
//      the output: advance `prev = prev->next` (confirming `run_start`).
//   3. Otherwise, the value occurred multiple times: unlink the entire run
//      by setting `prev->next = cur->next` while keeping `prev` stationary so
//      consecutive duplicate runs are also safely bypassed.
//   4. Advance `cur = cur->next` to inspect the next distinct value group.
//   Time Complexity: O(n), visiting each node at most once.
//   Space Complexity: O(1) auxiliary space (in-place pointer relinking).
//
// Data Structures:
//   - Singly linked list (`ListNode`) with a stack-allocated dummy sentinel
//     to handle deletions at the head uniformly without special cases.
//   - Predecessor/scanner pointer pair (`prev`, `cur`) tracking confirmed vs.
//     candidate runs.
//
// Real-World Applications:
//   - Log and telemetry deduplication pipelines that filter out conflicting
//     or ambiguous duplicate keys from sorted event streams.
//   - Database merge-join / anti-join operators that emit only strictly
//     unique records from a pre-sorted cursor.
//   - Financial reconciliation jobs identifying unmatched single-entry
//     transactions in sorted ledger streams.
//
// Related LeetCode Problems:
//   - #83 Remove Duplicates From Sorted List [done] (keep one copy per value)
//   - #203 Remove Linked List Elements [done] (dummy-node predicate deletion)
//   - #86 Partition List [done] (stable in-place list filtering with sentinels)
//   - #1836 Remove Duplicates From an Unsorted Linked List [new] (unsorted hash variant)
//   - #26 Remove Duplicates from Sorted Array [done] (sorted array deduplication)

#include "framework/lc_framework.h"

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
  // Removes every node whose value appears more than once in a sorted list.
  // Invariant: every node from `dummy.next` through `prev` is confirmed unique,
  // and `prev->next` always points to the first unverified node (`cur`).
  // Time: O(n), Space: O(1).
  ListNode* deleteDuplicates(ListNode* head) {
    if (head == nullptr) {
      return nullptr;
    }

    // Sentinel handles the case where the original head itself is duplicated.
    ListNode dummy(0, head);
    ListNode* prev = &dummy;
    ListNode* cur = head;

    while (cur != nullptr) {
      // Remember the start of the current equal-value run.
      const ListNode* run_start = cur;

      // Consume the entire contiguous block of nodes equal to `run_start->val`.
      while (cur->next != nullptr && cur->val == cur->next->val) {
        cur = cur->next;
      }

      if (cur == run_start) {
        // Run length was 1: confirm this node by advancing `prev`.
        prev = prev->next;
        cur = cur->next;
      } else {
        // Run length >= 2: bypass the entire duplicate block; `prev` stays put
        // because the next block might also consist of duplicates.
        cur = cur->next;
        prev->next = cur;
      }
    }

    return dummy.next;
  }
};

// RAII node arena so unlinked duplicate nodes never leak during unit tests.
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

LC_PROBLEM(82, "Remove Duplicates From Sorted List II", "linked_list", ctx) {
  Solution sol;

  ctx.test("example_1_middle_duplicates", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3, 3, 4, 4, 5});
    ctx.expect_eq(to_vector(sol.deleteDuplicates(head)),
                  std::vector<int>{1, 2, 5});
  });

  ctx.test("example_2_leading_duplicates", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 1, 1, 2, 3});
    ctx.expect_eq(to_vector(sol.deleteDuplicates(head)),
                  std::vector<int>{2, 3});
  });

  ctx.test("empty_and_single_node", [&] {
    ctx.expect_eq(to_vector(sol.deleteDuplicates(nullptr)), std::vector<int>{});

    ListArena arena;
    ListNode* single = arena.build({7});
    ctx.expect_eq(to_vector(sol.deleteDuplicates(single)), std::vector<int>{7});
  });

  ctx.test("all_duplicates_removed", [&] {
    ListArena arena;
    ListNode* head = arena.build({2, 2, 2, 2});
    ctx.expect_eq(to_vector(sol.deleteDuplicates(head)), std::vector<int>{});
  });

  ctx.test("trailing_duplicates_and_negatives", [&] {
    ListArena arena;
    ListNode* head1 = arena.build({1, 2, 3, 3});
    ctx.expect_eq(to_vector(sol.deleteDuplicates(head1)),
                  std::vector<int>{1, 2});

    ListNode* head2 = arena.build({-3, -3, -1, 0, 0, 2});
    ctx.expect_eq(to_vector(sol.deleteDuplicates(head2)),
                  std::vector<int>{-1, 2});
  });

  ctx.test("already_all_distinct", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3, 4, 5});
    ctx.expect_eq(to_vector(sol.deleteDuplicates(head)),
                  std::vector<int>{1, 2, 3, 4, 5});
  });
}

}  // namespace
