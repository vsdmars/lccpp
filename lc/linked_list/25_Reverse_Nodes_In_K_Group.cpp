// LeetCode 25: Reverse Nodes In K Group
// https://leetcode.com/problems/reverse-nodes-in-k-group/
//
// Problem:
//   Given the head of a singly-linked list and a positive integer `k`, reverse
//   the nodes of the list `k` at a time and return the modified list. If the
//   number of remaining nodes at the end is less than `k`, leave those nodes in
//   their original order. Only node links may be modified (not node values),
//   and the algorithm must run in O(1) extra memory.
//
// Examples:
//   Example 1:
//     Input:  head = [1, 2, 3, 4, 5], k = 2
//     Output: [2, 1, 4, 3, 5]
//     Explanation:
//       - Initialize dummy(0) pointing to node 1 and set group_prev = &dummy.
//       - Probe k = 2 steps to group_end = 2, save next_group = 3, reverse [1, 2]
//         with prev seeded to 3 to form dummy -> 2 -> 1 -> 3 -> 4 -> 5, and advance
//         group_prev to 1.
//       - Probe k = 2 steps from 1 to group_end = 4, reverse [3, 4] with next_group
//         = 5 to link 1 -> 4 -> 3 -> 5, and advance group_prev to 3.
//       - Probing k = 2 steps from 3 reaches nullptr after node 5, so the loop
//         breaks leaving 5 untouched and returning [2, 1, 4, 3, 5].
//
//   Example 2:
//     Input:  head = [1, 2, 3, 4, 5], k = 3
//     Output: [3, 2, 1, 4, 5]
//     Explanation:
//       - Initialize dummy(0) before node 1 with group_prev = &dummy.
//       - Probe k = 3 steps via advance_k to group_end = 3, saving group_start = 1
//         and next_group = 4.
//       - Reverse [1, 2, 3] with prev initialized to 4 to link dummy -> 3 -> 2 -> 1
//         -> 4 -> 5, and advance group_prev to 1.
//       - Probing k = 3 steps from 1 hits nullptr after 5 because only two nodes
//         remain, so the loop stops and returns [3, 2, 1, 4, 5].
//
//   Example 3:
//     Input:  head = [1, 2], k = 3
//     Output: [1, 2]
//     Explanation:
//       - Initialize dummy(0) pointing to node 1 and set group_prev = &dummy.
//       - Call advance_k(group_prev, 3), which steps through nodes 1 and 2 before
//         hitting nullptr on step 3.
//       - Because group_end is nullptr, break out of the loop immediately without
//         altering links and return [1, 2].
//
// Algorithm:
//   Iterative group-by-group in-place reversal anchored by a sentinel dummy:
//   Maintain `group_prev` pointing to the tail of the already-processed prefix.
//   In each iteration:
//   1. Look ahead `k` steps from `group_prev` to locate `group_end` (the k-th
//      node in the current group). If fewer than `k` nodes remain, stop and
//      return `dummy.next` so the incomplete tail stays intact.
//   2. Save `group_start = group_prev->next` and `next_group = group_end->next`.
//   3. Reverse the `k` nodes in `[group_start, group_end]` by initializing
//      `prev = next_group` and walking `curr` from `group_start` until
//      `curr == next_group`. Initializing `prev = next_group` automatically
//      stitches the new group tail (`group_start`) directly onto `next_group`.
//   4. Connect `group_prev->next = group_end` (the new group head) and advance
//      `group_prev = group_start` for the next iteration.
//   Time Complexity: O(n), as every node is visited at most twice (once during
//   the k-step probe and once during pointer reversal).
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - Singly-linked list (`ListNode`) reversed in place.
//   - Stack-allocated dummy sentinel plus constant boundary pointers
//     (`group_prev`, `group_start`, `group_end`, `next_group`).
//
// Real-World Applications:
//   - Fixed-size block/chunk reordering in streaming network packet buffers,
//     crypto block-chaining pipelines, and DMA scatter-gather descriptor lists.
//   - Batched undo/rollback of fixed-size transaction frames in linked journals.
//
// Related LeetCode Problems:
//   - #24 Swap Nodes In Pairs [done] (special case where k = 2)
//   - #206 Reverse Linked List [done] (core in-place link reversal primitive)
//   - #92 Reverse Linked List II [done] (reversing a single [left, right] range)
//   - #2074 Reverse Nodes in Even Length Groups [new] (variable-size group reversal)
//   - #61 Rotate List [done] (k-offset list boundary splitting and relinking)

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
  // Reverses consecutive groups of `k` nodes in place, leaving any trailing
  // group with fewer than `k` nodes untouched.
  // Time: O(n), Space: O(1).
  ListNode* reverseKGroup(ListNode* head, int k) {
    if (head == nullptr || k <= 1) {
      return head;
    }

    ListNode dummy(0, head);
    ListNode* group_prev = &dummy;

    while (true) {
      // Probe k nodes ahead to confirm a complete group exists.
      ListNode* group_end = advance_k(group_prev, k);
      if (group_end == nullptr) {
        break;
      }

      ListNode* group_start = group_prev->next;
      ListNode* next_group = group_end->next;

      // Reverse the k nodes [group_start .. group_end]. Seeding `prev` with
      // `next_group` links `group_start` (the new tail) straight to the rest
      // of the list on the very first iteration.
      ListNode* prev = next_group;
      ListNode* curr = group_start;
      while (curr != next_group) {
        ListNode* next_node = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next_node;
      }

      // Stitch the preceding section to the reversed group's new head (`prev`,
      // which equals `group_end`) and advance `group_prev` to its new tail
      // (`group_start`).
      group_prev->next = prev;
      group_prev = group_start;
    }

    return dummy.next;
  }

 private:
  // Advances `k` links forward from `start`, returning nullptr if fewer than
  // `k` nodes follow `start`.
  static ListNode* advance_k(ListNode* start, int k) {
    ListNode* node = start;
    for (int step = 0; step < k; ++step) {
      node = node->next;
      if (node == nullptr) {
        return nullptr;
      }
    }
    return node;
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

LC_PROBLEM(25, "Reverse Nodes In K Group", "linked_list", ctx) {
  Solution sol;

  ctx.test("example_1_k_equals_2", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3, 4, 5});
    ctx.expect_eq(to_vector(sol.reverseKGroup(head, 2)),
                  std::vector<int>{2, 1, 4, 3, 5});
  });

  ctx.test("example_2_k_equals_3", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3, 4, 5});
    ctx.expect_eq(to_vector(sol.reverseKGroup(head, 3)),
                  std::vector<int>{3, 2, 1, 4, 5});
  });

  ctx.test("exact_multiple_of_k", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3, 4, 5, 6});
    ctx.expect_eq(to_vector(sol.reverseKGroup(head, 3)),
                  std::vector<int>{3, 2, 1, 6, 5, 4});
  });

  ctx.test("k_equals_1_unchanged", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3});
    ctx.expect_eq(to_vector(sol.reverseKGroup(head, 1)),
                  std::vector<int>{1, 2, 3});
  });

  ctx.test("k_equals_length_full_reversal", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3, 4});
    ctx.expect_eq(to_vector(sol.reverseKGroup(head, 4)),
                  std::vector<int>{4, 3, 2, 1});
  });

  ctx.test("k_greater_than_length_unchanged", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2});
    ctx.expect_eq(to_vector(sol.reverseKGroup(head, 3)),
                  std::vector<int>{1, 2});
  });

  ctx.test("single_node_and_empty", [&] {
    ListArena arena;
    ListNode* single = arena.build({7});
    ctx.expect_eq(to_vector(sol.reverseKGroup(single, 1)), std::vector<int>{7});
    ctx.expect_eq(sol.reverseKGroup(nullptr, 2), nullptr);
  });
}

}  // namespace
