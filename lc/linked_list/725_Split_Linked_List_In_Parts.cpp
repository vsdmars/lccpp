// LeetCode 725: Split Linked List In Parts
// https://leetcode.com/problems/split-linked-list-in-parts/
//
// Problem:
//   Given the head of a singly linked list and an integer `k`, split the linked
//   list into `k` consecutive linked list parts. The lengths of any two parts
//   must differ by at most 1, earlier parts must be at least as long as later
//   parts, and parts that receive zero nodes must be represented by `nullptr`.
//   Return an array of the `k` resulting part heads in original order.
//
// Examples:
//   Example 1:
//     Input:  head = [1, 2, 3], k = 5
//     Output: [[1], [2], [3], [], []]
//     Explanation:
//       - Pass 1 counts total_nodes = 3, computes base_size = 3 / 5 = 0 and
//         remainder = 3 % 5 = 3, and allocates 5 nullptr slots in parts.
//       - For i = 0, 1, 2 (each with i < remainder so part_size = 1), Pass 2
//         records parts[i] = curr, detaches curr->next = nullptr, and advances curr
//         to the next node (1, 2, then 3).
//       - After node 3 is detached, curr becomes nullptr and the loop ends, leaving
//         parts[3] and parts[4] as nullptr to return [[1], [2], [3], [], []].
//
//   Example 2:
//     Input:  head = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10], k = 3
//     Output: [[1, 2, 3, 4], [5, 6, 7], [8, 9, 10]]
//     Explanation:
//       - Pass 1 counts total_nodes = 10 and computes base_size = 10 / 3 = 3 with
//         remainder = 10 % 3 = 1.
//       - For i = 0 (i < remainder), part_size = 4: parts[0] starts at 1, curr
//         advances 3 steps to node 4, severs 4 -> nullptr, and moves to 5.
//       - For i = 1 (i >= remainder), part_size = 3: parts[1] starts at 5, curr
//         advances 2 steps to node 7, severs 7 -> nullptr, and moves to 8.
//       - For i = 2, part_size = 3: parts[2] starts at 8, curr advances 2 steps to
//         node 10, sets 10->next = nullptr, and returns [[1, 2, 3, 4], [5, 6, 7],
//         [8, 9, 10]].
//
//   Example 3:
//     Input:  head = [], k = 3
//     Output: [[], [], []]
//     Explanation:
//       - Pass 1 traverses from head = nullptr and records total_nodes = 0,
//         yielding base_size = 0 and remainder = 0.
//       - Allocate parts(3, nullptr) and set curr = nullptr.
//       - Because curr != nullptr is immediately false, Pass 2 performs zero
//         iterations and returns three empty (nullptr) parts [[], [], []].
//
// Algorithm:
//   Two-pass length-aware in-place splitting:
//   1. Pass 1: Traverse the list to count its total length `n`.
//   2. Compute `base_size = n / k` and `remainder = n % k`. By Euclidean
//      division, assigning `base_size + 1` nodes to the first `remainder` parts
//      and `base_size` nodes to the remaining `k - remainder` parts guarantees
//      all part lengths sum to `n` and differ by at most 1.
//   3. Pass 2: Preallocate `parts(k, nullptr)`. For each part index `i` while
//      nodes remain, record `parts[i] = curr`, advance `part_len - 1` steps to
//      the tail of the current part, save `curr->next`, sever the link via
//      `curr->next = nullptr`, and move `curr` to the saved next head.
//   Time Complexity: O(n + k), traversing the list twice and sizing the output
//   vector to `k`.
//   Space Complexity: O(1) auxiliary space beyond the returned vector of `k`
//   pointers (relinks existing nodes in place).
//
// Data Structures:
//   - Singly linked list (`ListNode`): partitioned in place by breaking `next`
//     links at exact segment boundaries.
//   - Output pointer array (`std::vector<ListNode*>`): holds the head pointer
//     (or `nullptr`) for each of the `k` parts.
//
// Real-World Applications:
//   - Parallel work-stealing / MapReduce batch sharding that distributes a
//     stream of tasks across `k` worker threads with <= 1 task imbalance.
//   - Database range pagination and chunked ETL pipeline slicing.
//   - Network MTU packet fragmentation of chained payload buffers.
//
// Related LeetCode Problems:
//   - #328 Odd Even Linked List [done] (in-place linked list partitioning)
//   - #61 Rotate List [done] (length counting and severing a linked list at an offset)
//   - #876 Middle Of The Linked List [done] (halving a linked list cleanly)
//   - #25 Reverse Nodes in k-Group [done] (slicing a linked list into consecutive groups)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <utility>
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

// Serializes a vector of split linked-list heads into a 2D vector for checks.
[[nodiscard]] std::vector<std::vector<int>> parts_to_vectors(
    const std::vector<ListNode*>& parts) {
  std::vector<std::vector<int>> result;
  result.reserve(parts.size());
  for (const ListNode* head : parts) {
    std::vector<int> vals;
    for (const ListNode* curr = head; curr != nullptr; curr = curr->next) {
      vals.push_back(curr->val);
    }
    result.push_back(std::move(vals));
  }
  return result;
}

class Solution {
 public:
  // Splits `head` in place into `k` consecutive parts whose sizes differ by at
  // most 1, with larger parts appearing earlier.
  // Time: O(n + k), Space: O(1) extra (excluding the output vector).
  std::vector<ListNode*> splitListToParts(ListNode* head, int k) {
    if (k <= 0) {
      return {};
    }

    // Pass 1: Measure total number of nodes so we know exact chunk sizes.
    int total_nodes = 0;
    for (const ListNode* curr = head; curr != nullptr; curr = curr->next) {
      ++total_nodes;
    }

    // Every part gets at least `base_size` nodes; the first `remainder` parts
    // each receive 1 extra node to distribute the surplus evenly.
    const int base_size = total_nodes / k;
    const int remainder = total_nodes % k;

    std::vector<ListNode*> parts(static_cast<std::size_t>(k), nullptr);
    ListNode* curr = head;

    // Pass 2: Carve each non-empty part by stepping to its tail and cutting.
    for (int i = 0; i < k && curr != nullptr; ++i) {
      parts[static_cast<std::size_t>(i)] = curr;
      const int part_size = base_size + (i < remainder ? 1 : 0);

      // Advance `part_size - 1` hops so `curr` lands on the last node of part i.
      for (int step = 1; step < part_size && curr != nullptr; ++step) {
        curr = curr->next;
      }

      // Detach this part from the rest of the list and advance to the next part.
      if (curr != nullptr) {
        ListNode* next_part = curr->next;
        curr->next = nullptr;
        curr = next_part;
      }
    }

    return parts;
  }
};

LC_PROBLEM(725, "Split Linked List In Parts", "linked_list", ctx) {
  Solution sol;

  ctx.test("example_1_more_parts_than_nodes", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3});
    const auto parts = sol.splitListToParts(head, 5);
    ctx.expect_eq(parts_to_vectors(parts),
                  std::vector<std::vector<int>>({{1}, {2}, {3}, {}, {}}));
  });

  ctx.test("example_2_uneven_split", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
    const auto parts = sol.splitListToParts(head, 3);
    ctx.expect_eq(
        parts_to_vectors(parts),
        std::vector<std::vector<int>>({{1, 2, 3, 4}, {5, 6, 7}, {8, 9, 10}}));
  });

  ctx.test("empty_list_returns_k_nulls", [&] {
    const auto parts = sol.splitListToParts(nullptr, 3);
    ctx.expect_eq(parts_to_vectors(parts),
                  std::vector<std::vector<int>>({{}, {}, {}}));
  });

  ctx.test("exact_divisible_split", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3, 4, 5, 6});
    const auto parts = sol.splitListToParts(head, 3);
    ctx.expect_eq(parts_to_vectors(parts),
                  std::vector<std::vector<int>>({{1, 2}, {3, 4}, {5, 6}}));
  });

  ctx.test("single_part_preserves_full_list", [&] {
    ListArena arena;
    ListNode* head = arena.build({10, 20, 30, 40});
    const auto parts = sol.splitListToParts(head, 1);
    ctx.expect_eq(parts_to_vectors(parts),
                  std::vector<std::vector<int>>({{10, 20, 30, 40}}));
  });

  ctx.test("eight_nodes_into_thirteen_parts", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3, 4, 5, 6, 7, 8});
    const auto parts = sol.splitListToParts(head, 13);
    ctx.expect_eq(parts_to_vectors(parts),
                  std::vector<std::vector<int>>(
                      {{1}, {2}, {3}, {4}, {5}, {6}, {7}, {8}, {}, {}, {}, {},
                       {}}));
  });
}

}  // namespace
