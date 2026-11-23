// LeetCode 1019: Next Greater Node In Linked List
// https://leetcode.com/problems/next-greater-node-in-linked-list/
//
// Problem:
//   Given the head of a singly linked list with nodes numbered 0, 1, ..., n-1,
//   for each node find the value of the first subsequent node strictly greater
//   than it. If no such strictly greater node exists to its right, record 0.
//   Return an integer array `answer` where `answer[i]` is the next greater
//   value for node `i`.
//
// Algorithm:
//   Single-pass monotonic decreasing stack storing `(index, value)` pairs:
//   1. Traverse the linked list from head to tail while growing `answer`
//      dynamically (defaulting each new slot to 0).
//   2. Maintain a stack of unresolved `(index, val)` entries whose values are
//      monotonically non-increasing from bottom to top.
//   3. At each node with value `curr->val`, pop every stack entry whose stored
//      value is strictly less than `curr->val` and backfill
//      `answer[entry.index] = curr->val`. Because the list is scanned left to
//      right, `curr->val` is guaranteed to be the nearest strictly greater
//      value for every popped entry.
//   4. Push `{answer.size(), curr->val}` onto the stack and append `0` to
//      `answer`. Any entries remaining on the stack at the end have no greater
//      element to their right and naturally retain their default `0`.
//   Time Complexity: O(n) amortized — each node is pushed once and popped at
//   most once.
//   Space Complexity: O(n) for the monotonic stack and output array.
//
// Data Structures:
//   - Monotonic decreasing stack (`std::vector<PendingNode>`): tracks indices
//     and values of earlier nodes still awaiting a strictly larger successor.
//   - Dynamic output array (`std::vector<int>`): stores resolved next-greater
//     values indexed by 0-based position in the linked list.
//
// Real-World Applications:
//   - Financial time-series analysis ("time until next price breakout" or
//     stock span / waiting-period queries over streaming quotes).
//   - Weather and telemetry anomaly monitoring (next warmer reading / next
//     latency spike above current threshold).
//   - Compiler / layout engines resolving nearest enclosing dominance bounds
//     in linear time.
//
// Related LeetCode Problems:
//   - #496 Next Greater Element I [done] (classic monotonic stack next-greater query)
//   - #503 Next Greater Element II [done] (circular array next-greater element)
//   - #739 Daily Temperatures [done] (distance to next strictly greater element)
//   - #2487 Remove Nodes From Linked List [new] (filtering linked list nodes using next-greater dominance)

#include "framework/lc_framework.h"

#include <cstddef>
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

class Solution {
 public:
  // Computes the nearest strictly greater successor value for each node in a
  // single left-to-right pass using a monotonic non-increasing stack.
  // Time: O(n), Space: O(n).
  std::vector<int> nextLargerNodes(ListNode* head) {
    // Represents a previously visited node still waiting for a strictly larger
    // successor to appear later in the list.
    struct PendingNode {
      std::size_t index = 0uz;
      int val = 0;
    };

    std::vector<int> answer;
    std::vector<PendingNode> mono_stack;

    for (const ListNode* curr = head; curr != nullptr; curr = curr->next) {
      // Resolve every earlier pending node that is strictly smaller than the
      // current value; stack monotonicity ensures all popped entries get their
      // closest strictly greater successor.
      while (!mono_stack.empty() && mono_stack.back().val < curr->val) {
        answer[mono_stack.back().index] = curr->val;
        mono_stack.pop_back();
      }

      // Record the current node's position and default its answer slot to 0 in
      // case no larger node ever follows.
      mono_stack.push_back(PendingNode{.index = answer.size(), .val = curr->val});
      answer.push_back(0);
    }

    return answer;
  }
};

LC_PROBLEM(1019, "Next Greater Node In Linked List", "linked_list", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    ListArena arena;
    ListNode* head = arena.build({2, 1, 5});
    ctx.expect_eq(sol.nextLargerNodes(head), std::vector<int>({5, 5, 0}));
  });

  ctx.test("example_2", [&] {
    ListArena arena;
    ListNode* head = arena.build({2, 7, 4, 3, 5});
    ctx.expect_eq(sol.nextLargerNodes(head),
                  std::vector<int>({7, 0, 5, 5, 0}));
  });

  ctx.test("example_3", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 7, 5, 1, 9, 2, 5, 1});
    ctx.expect_eq(sol.nextLargerNodes(head),
                  std::vector<int>({7, 9, 9, 9, 0, 5, 0, 0}));
  });

  ctx.test("empty_list", [&] {
    ctx.expect_eq(sol.nextLargerNodes(nullptr), std::vector<int>({}));
  });

  ctx.test("single_node", [&] {
    ListArena arena;
    ListNode* head = arena.build({42});
    ctx.expect_eq(sol.nextLargerNodes(head), std::vector<int>({0}));
  });

  ctx.test("strictly_increasing", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3, 4});
    ctx.expect_eq(sol.nextLargerNodes(head), std::vector<int>({2, 3, 4, 0}));
  });

  ctx.test("strictly_decreasing_and_duplicates", [&] {
    ListArena arena;
    ListNode* dec = arena.build({5, 4, 3, 2});
    ctx.expect_eq(sol.nextLargerNodes(dec), std::vector<int>({0, 0, 0, 0}));

    ListNode* eq = arena.build({3, 3, 3});
    ctx.expect_eq(sol.nextLargerNodes(eq), std::vector<int>({0, 0, 0}));
  });
}

}  // namespace
