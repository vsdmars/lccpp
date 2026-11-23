// LeetCode 86: Partition List
// https://leetcode.com/problems/partition-list/
//
// Problem:
//   Given the head of a singly linked list and a pivot value `x`, partition
//   the list so that all nodes with values strictly less than `x` come before
//   all nodes with values greater than or equal to `x`. The original relative
//   order of nodes within each of the two partitions must be preserved.
//   Constraints: 0 <= number of nodes <= 200, -100 <= Node.val <= 100,
//   -200 <= x <= 200.
//
// Algorithm:
//   Maintain two separate singly linked sublists anchored by stack-allocated
//   dummy heads:
//   - `less_dummy` / `less_tail`: collects all nodes with `val < x` in order.
//   - `ge_dummy` / `ge_tail`: collects all nodes with `val >= x` in order.
//   Traverse the original list once, appending each node directly to the
//   appropriate tail pointer (preserving stability). After the loop:
//   1. Terminate the second partition with `ge_tail->next = nullptr` to break
//      any stale forward link that could otherwise create a cycle (when the
//      last node of the original list belonged to the `< x` partition).
//   2. Splice the two partitions via `less_tail->next = ge_dummy.next`.
//   3. Return `less_dummy.next`.
//   Time Complexity: O(n), single linear pass.
//   Space Complexity: O(1) auxiliary space (relinks existing nodes in place).
//
// Data Structures:
//   - Two sentinel-headed singly linked lists (`less_dummy`, `ge_dummy`) with
//     maintained tail pointers for O(1) stable append and O(1) concatenation.
//
// Real-World Applications:
//   - Stable priority-queue bucketing in OS schedulers (separating high- and
//     low-priority tasks while preserving FIFO fairness within each class).
//   - Linked-list quicksort / radix-partition steps in memory-constrained
//     embedded systems where array allocation is undesirable.
//   - Network packet QoS classifiers splitting traffic streams around a
//     threshold while maintaining per-flow packet ordering.
//
// Related LeetCode Problems:
//   - #328 Odd Even Linked List [done] (two-sentinel stable list splitting by index)
//   - #21 Merge Two Sorted Lists [done] (sentinel-headed list construction)
//   - #148 Sort List [done] (linked-list divide-and-conquer sorting)
//   - #2161 Partition Array According to Given Pivot [new] (3-way stable array partition)

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
  // Stably partitions the list around `x` into `< x` followed by `>= x`.
  // Invariant: `less_tail` ends the `< x` chain and `ge_tail` ends the `>= x`
  // chain, both preserving original relative order.
  // Time: O(n), Space: O(1).
  ListNode* partition(ListNode* head, int x) {
    ListNode less_dummy(0);
    ListNode ge_dummy(0);
    ListNode* less_tail = &less_dummy;
    ListNode* ge_tail = &ge_dummy;

    // Distribute every node into its partition bucket in one pass.
    for (ListNode* cur = head; cur != nullptr; cur = cur->next) {
      if (cur->val < x) {
        less_tail->next = cur;
        less_tail = less_tail->next;
      } else {
        ge_tail->next = cur;
        ge_tail = ge_tail->next;
      }
    }

    // Critical: sever any lingering link from the `>= x` tail back into a
    // `< x` node, which would otherwise introduce a cycle.
    ge_tail->next = nullptr;

    // Concatenate the `< x` partition ahead of the `>= x` partition.
    less_tail->next = ge_dummy.next;
    return less_dummy.next;
  }
};

// RAII node arena for leak-free test list construction.
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

LC_PROBLEM(86, "Partition List", "linked_list", ctx) {
  Solution sol;

  ctx.test("example_1_mixed_values", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 4, 3, 2, 5, 2});
    ctx.expect_eq(to_vector(sol.partition(head, 3)),
                  std::vector<int>{1, 2, 2, 4, 3, 5});
  });

  ctx.test("example_2_two_nodes", [&] {
    ListArena arena;
    ListNode* head = arena.build({2, 1});
    ctx.expect_eq(to_vector(sol.partition(head, 2)), std::vector<int>{1, 2});
  });

  ctx.test("empty_and_single_node", [&] {
    ctx.expect_eq(to_vector(sol.partition(nullptr, 0)), std::vector<int>{});

    ListArena arena;
    ListNode* single1 = arena.build({1});
    ctx.expect_eq(to_vector(sol.partition(single1, 2)), std::vector<int>{1});

    ListNode* single2 = arena.build({1});
    ctx.expect_eq(to_vector(sol.partition(single2, 1)), std::vector<int>{1});
  });

  ctx.test("all_less_or_all_greater_equal", [&] {
    ListArena arena;
    ListNode* all_less = arena.build({1, 2, 3});
    ctx.expect_eq(to_vector(sol.partition(all_less, 10)),
                  std::vector<int>{1, 2, 3});

    ListNode* all_ge = arena.build({5, 3, 4});
    ctx.expect_eq(to_vector(sol.partition(all_ge, 1)),
                  std::vector<int>{5, 3, 4});
  });

  ctx.test("negative_values_and_duplicate_pivot", [&] {
    ListArena arena;
    ListNode* head = arena.build({-1, 3, 0, -2, 0, 5});
    ctx.expect_eq(to_vector(sol.partition(head, 0)),
                  std::vector<int>{-1, -2, 3, 0, 0, 5});
  });
}

}  // namespace
