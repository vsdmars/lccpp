// LeetCode 83: Remove Duplicates From Sorted List
// https://leetcode.com/problems/remove-duplicates-from-sorted-list/
//
// Problem:
//   Given the head of a sorted singly linked list, delete all duplicate nodes
//   such that each distinct element appears only once, and return the sorted
//   linked list.
//   Constraints: 0 <= number of nodes <= 300, -100 <= Node.val <= 100.
//
// Examples:
//   Example 1:
//     Input:  head = [1, 1, 2]
//     Output: [1, 2]
//     Explanation:
//       - Attach dummy(0, head) with prev = &dummy and inspect cur = prev->next
//         (first 1).
//       - Because cur->val == cur->next->val (1 == 1), rewire cur->next =
//         cur->next->next to bypass the duplicate 1 and link directly to 2, then
//         advance prev to node 1.
//       - Next, cur = prev->next is node 2 with cur->next == nullptr, so prev
//         advances to 2, the loop terminates, and dummy.next yields [1, 2].
//
//   Example 2:
//     Input:  head = [1, 1, 2, 3, 3]
//     Output: [1, 2, 3]
//     Explanation:
//       - Starting with prev = &dummy, cur = 1 skips the duplicate 1 via cur->next
//         = 2 and commits node 1 by setting prev = 1.
//       - Next, cur = 2 has cur->next->val == 3 != 2, so no node is skipped and
//         prev advances to 2.
//       - Finally, cur = 3 matches cur->next->val == 3, rewires cur->next = nullptr
//         to drop the second 3, sets prev = 3, and returns dummy.next as [1, 2, 3].
//
//   Example 3:
//     Input:  head = [5, 5, 5, 5]
//     Output: [5]
//     Explanation:
//       - Attach dummy(0, head) with prev = &dummy and select cur = prev->next
//         (first 5).
//       - The inner loop matches cur->val == cur->next->val three consecutive
//         times, advancing cur->next past the second, third, and fourth 5 until
//         cur->next == nullptr.
//       - Setting prev = cur moves prev to the surviving 5 with prev->next ==
//         nullptr, ending the outer loop and returning [5].
//
// Algorithm:
//   Attach a dummy sentinel before `head` and walk the list with `prev`:
//   1. Let `cur = prev->next` be the first representative of the next value.
//   2. Repeatedly skip any immediate successor whose value equals `cur->val`
//      by rewiring `cur->next = cur->next->next`.
//   3. Once all consecutive duplicates of `cur->val` have been bypassed,
//      advance `prev = cur` so the next iteration processes the subsequent
//      distinct value.
//   Time Complexity: O(n), since every `next` link is inspected at most once.
//   Space Complexity: O(1) auxiliary space (in-place pointer relinking).
//
// Data Structures:
//   - Singly linked list (`ListNode`) with a stack-allocated dummy sentinel
//     keeping the loop structure uniform for empty and non-empty lists.
//
// Real-World Applications:
//   - Streaming `uniq` / run-length compression passes over sorted records.
//   - Deduplicating sorted inverted-index posting lists in search engines.
//   - Collapsing repeated timestamp entries in time-series ingestion buffers.
//
// Related LeetCode Problems:
//   - #82 Remove Duplicates From Sorted List II [done] (delete all nodes with duplicates)
//   - #26 Remove Duplicates from Sorted Array [done] (in-place array deduplication)
//   - #203 Remove Linked List Elements [done] (sentinel-based node removal)
//   - #1836 Remove Duplicates From an Unsorted Linked List [new] (unsorted hash-map variant)

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
  // Keeps exactly one node per distinct value in an ascending sorted list.
  // Invariant: all nodes in `dummy.next .. prev` are strictly increasing and
  // duplicate-free.
  // Time: O(n), Space: O(1).
  ListNode* deleteDuplicates(ListNode* head) {
    ListNode dummy(0, head);
    ListNode* prev = &dummy;

    while (prev->next != nullptr) {
      // Keep `cur` as the sole representative of its value group.
      ListNode* cur = prev->next;

      // Bypass every subsequent node that shares `cur->val`.
      while (cur->next != nullptr && cur->val == cur->next->val) {
        cur->next = cur->next->next;
      }

      // All duplicates of `cur->val` are gone; commit `cur` and move forward.
      prev = cur;
    }

    return dummy.next;
  }
};

// RAII node arena so bypassed duplicate nodes are automatically freed cleanly.
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

LC_PROBLEM(83, "Remove Duplicates From Sorted List", "linked_list", ctx) {
  Solution sol;

  ctx.test("example_1_leading_duplicate", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 1, 2});
    ctx.expect_eq(to_vector(sol.deleteDuplicates(head)),
                  std::vector<int>{1, 2});
  });

  ctx.test("example_2_multiple_duplicate_groups", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 1, 2, 3, 3});
    ctx.expect_eq(to_vector(sol.deleteDuplicates(head)),
                  std::vector<int>{1, 2, 3});
  });

  ctx.test("python_build_case", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 1, 1, 3, 3, 5});
    ctx.expect_eq(to_vector(sol.deleteDuplicates(head)),
                  std::vector<int>{1, 3, 5});
  });

  ctx.test("empty_and_single_node", [&] {
    ctx.expect_eq(to_vector(sol.deleteDuplicates(nullptr)), std::vector<int>{});

    ListArena arena;
    ListNode* single = arena.build({42});
    ctx.expect_eq(to_vector(sol.deleteDuplicates(single)),
                  std::vector<int>{42});
  });

  ctx.test("all_same_and_all_unique", [&] {
    ListArena arena;
    ListNode* all_same = arena.build({5, 5, 5, 5});
    ctx.expect_eq(to_vector(sol.deleteDuplicates(all_same)),
                  std::vector<int>{5});

    ListNode* unique = arena.build({-5, -2, 0, 4});
    ctx.expect_eq(to_vector(sol.deleteDuplicates(unique)),
                  std::vector<int>{-5, -2, 0, 4});
  });
}

}  // namespace
