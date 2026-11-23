// LeetCode 716: Max Stack
// https://leetcode.com/problems/max-stack/
//
// Problem:
//   Design a max stack data structure supporting five operations:
//     - `push(x)`: push integer `x` onto the top of the stack.
//     - `pop()`: remove and return the element on top of the stack.
//     - `top()`: return the element on top of the stack without removing it.
//     - `peekMax()`: return the maximum element currently in the stack.
//     - `popMax()`: remove and return the maximum element currently in the
//       stack; if multiple elements tie for the maximum, remove the top-most
//       (most recently pushed) one.
//   The last four operations are guaranteed to be called only when non-empty.
//
// Examples:
//   Example 1:
//     Input:  ["MaxStack", "push", "push", "push", "top", "popMax", "top",
//             "peekMax", "pop", "top"], [[], [5], [1], [5], [], [], [], [], [], []]
//     Output: [null, null, null, null, 5, 5, 1, 5, 1, 5]
//     Explanation:
//       - The three push calls assign IDs 0, 1, 2, producing stack_ = [(5,0),
//         (1,1), (5,2)] and max_heap_ topped by (5,2) due to larger ID
//         tie-breaking.
//       - top() reads 5 from (5,2), then popMax() removes (5,2) from max_heap_,
//         inserts id = 2 into removed_, and returns 5.
//       - The second top() calls prune_stack(), discarding stale (5,2) and erasing
//         id = 2 from removed_, then returns 1 from (1,1).
//       - peekMax() returns 5 from (5,0) at the heap root, pop() removes (1,1) and
//         marks id = 1 in removed_ returning 1, and the final top() returns 5 from
//         (5,0).
//
//   Example 2:
//     Input:  ["MaxStack", "push", "push", "push", "popMax", "top", "push",
//             "popMax", "pop", "top"], [[], [10], [20], [10], [], [], [10], [], [],
//             []]
//     Output: [null, null, null, null, 20, 10, null, 10, 10, 10]
//     Explanation:
//       - Pushing 10, 20, 10 inserts entries (10,0), (20,1), (10,2); popMax() pops
//         (20,1) from max_heap_, inserts id = 1 into removed_, and returns 20.
//       - top() returns 10 from (10,2), then push(10) appends (10,3) with a fresh
//         universal ID to both stack_ and max_heap_.
//       - popMax() pops (10,3) (highest ID among the 10s) and inserts id = 3 into
//         removed_, returning 10.
//       - pop() prunes stale (10,3) and pops (10,2) (adding id = 2 to removed_)
//         returning 10, and top() returns 10 from (10,0).
//
//   Example 3:
//     Input:  ["MaxStack", "push", "peekMax", "push", "peekMax", "push", "pop",
//             "pop", "push", "peekMax", "push", "popMax", "top", "push", "push",
//             "peekMax", "popMax", "popMax", "top", "peekMax"], [[], [92], [],
//             [54], [], [22], [], [], [-57], [], [-24], [], [], [26], [-71], [],
//             [], [], [], []]
//     Output: [null, null, 92, null, 92, null, 22, 54, null, 92, null, 92, -24,
//             null, null, 26, 26, -24, -71, -57]
//     Explanation:
//       - Pushing 92 (id 0), 54 (id 1), and 22 (id 2) keeps peekMax() at 92, then
//         two pop() calls return 22 and 54 while recording IDs 2 and 1 in removed_.
//       - Pushing -57 (id 3) and -24 (id 4) keeps (92,0) at the heap root, so
//         peekMax() returns 92, popMax() removes (92,0) (marking id = 0 removed),
//         and top() returns -24.
//       - Pushing 26 (id 5) and -71 (id 6) places (26,5) at the heap root, so
//         peekMax() returns 26 and consecutive popMax() calls remove (26,5) and
//         (-24,4) (marking IDs 5 and 4 removed).
//       - top() returns -71 from (-71,6), and peekMax() calls prune_heap() to
//         discard stale (26,5) and (-24,4) before returning -57 from (-57,3).
//
// Algorithm:
//   Assign every pushed element a monotonically increasing universal ID
//   (`next_id_++`) and insert `(value, id)` into both a LIFO stack and a
//   max-heap (ordered by larger `value`, tie-breaking by larger `id` so the
//   top-most maximum always rises to the heap root). Because removing from one
//   structure cannot efficiently remove from the middle of the other, use lazy
//   deletion backed by a tombstone set `removed_` of deleted IDs:
//     - Before reading/popping from `stack_`, discard any top entries whose ID
//       is in `removed_` (and erase those IDs from `removed_`).
//     - Before reading/popping from `max_heap_`, discard any root entries whose
//       ID is in `removed_` (and erase those IDs from `removed_`).
//     - When `pop()` or `popMax()` removes a live entry, record its ID in
//       `removed_` so the opposite container ignores it later.
//   Time Complexity:
//     - `push`, `peekMax`, `popMax`: O(log n) amortized.
//     - `top`, `pop`: O(1) amortized (each pushed entry is pruned at most once).
//   Space Complexity: O(n) for the stack, max-heap, and tombstone set.
//
// Data Structures:
//   - Primary LIFO stack (`std::vector<Entry>`): tracks insertion order.
//   - Max-heap (`std::priority_queue<Entry>`): retrieves maximum `(val, id)` in
//     logarithmic time with recency tie-breaking.
//   - Tombstone set (`std::unordered_set<int>`): coordinates lazy deletion
//     between the stack and the max-heap.
//
// Real-World Applications:
//   - Undo/redo editors and transactional logs that also need priority eviction
//     (e.g., dropping the highest-cost pending action).
//   - Memory / cache managers combining LIFO recency stacks with max-size
//     eviction policies.
//   - Order-book or priority-queue systems using lazy tombstone invalidation to
//     keep secondary indices synchronized without intrusive pointers.
//
// Related LeetCode Problems:
//   - #155 Min Stack [done] (O(1) stack with minimum tracking, no popMin)
//   - #239 Sliding Window Maximum [done] (max tracking with lazy/deque eviction)
//   - #480 Sliding Window Median [done] (dual-heap lazy deletion pattern)
//   - #895 Maximum Frequency Stack [done] (stack with frequency-priority pop)
//   - #2349 Design a Number Container System [new] (indexed lazy-deletion heap)

#include "framework/lc_framework.h"

#include <compare>
#include <queue>
#include <unordered_set>
#include <vector>

namespace {

// MaxStack supporting O(log n) amortized push/peekMax/popMax and O(1) amortized
// top/pop via paired stack + max-heap with lazy tombstone deletion.
class MaxStack {
 public:
  // Pushes `x` onto the stack with a fresh monotonically increasing unique ID.
  // Time: O(log n).
  void push(int x) {
    const Entry entry{.val = x, .id = next_id_++};
    stack_.push_back(entry);
    max_heap_.push(entry);
  }

  // Removes and returns the top live element of the stack.
  // Time: O(1) amortized.
  int pop() {
    prune_stack();
    const Entry entry = stack_.back();
    stack_.pop_back();
    removed_.insert(entry.id);
    return entry.val;
  }

  // Returns the top live element of the stack without removing it.
  // Time: O(1) amortized.
  [[nodiscard]] int top() {
    prune_stack();
    return stack_.back().val;
  }

  // Returns the maximum live element currently in the stack.
  // Time: O(log n) amortized.
  [[nodiscard]] int peekMax() {
    prune_heap();
    return max_heap_.top().val;
  }

  // Removes and returns the top-most (largest ID) maximum live element.
  // Time: O(log n) amortized.
  int popMax() {
    prune_heap();
    const Entry entry = max_heap_.top();
    max_heap_.pop();
    removed_.insert(entry.id);
    return entry.val;
  }

 private:
  // Lexicographical `<=>` compares `val` first, then `id`, so `priority_queue`
  // naturally prioritizes larger values and breaks ties by most recent push.
  struct Entry {
    int val = 0;
    int id = 0;

    auto operator<=>(const Entry&) const = default;
  };

  // Discards stale stack entries already removed by `popMax()`.
  void prune_stack() {
    while (!stack_.empty() && removed_.contains(stack_.back().id)) {
      removed_.erase(stack_.back().id);
      stack_.pop_back();
    }
  }

  // Discards stale heap entries already removed by `pop()`.
  void prune_heap() {
    while (!max_heap_.empty() && removed_.contains(max_heap_.top().id)) {
      removed_.erase(max_heap_.top().id);
      max_heap_.pop();
    }
  }

  std::vector<Entry> stack_;
  std::priority_queue<Entry> max_heap_;
  std::unordered_set<int> removed_;
  int next_id_ = 0;
};

LC_PROBLEM(716, "Max Stack", "stack", ctx) {
  ctx.test("example_tie_breaking_topmost_max", [&] {
    MaxStack st;
    st.push(5);
    st.push(1);
    st.push(5);
    ctx.expect_eq(st.top(), 5);
    ctx.expect_eq(st.popMax(), 5);
    ctx.expect_eq(st.top(), 1);
    ctx.expect_eq(st.peekMax(), 5);
    ctx.expect_eq(st.pop(), 1);
    ctx.expect_eq(st.top(), 5);
  });

  ctx.test("source_main_operation_sequence", [&] {
    MaxStack st;
    st.push(92);
    ctx.expect_eq(st.peekMax(), 92);
    st.push(54);
    ctx.expect_eq(st.peekMax(), 92);
    st.push(22);
    ctx.expect_eq(st.pop(), 22);
    ctx.expect_eq(st.pop(), 54);
    st.push(-57);
    ctx.expect_eq(st.peekMax(), 92);
    st.push(-24);
    ctx.expect_eq(st.popMax(), 92);
    ctx.expect_eq(st.top(), -24);
    st.push(26);
    st.push(-71);
    ctx.expect_eq(st.peekMax(), 26);
    ctx.expect_eq(st.popMax(), 26);
    ctx.expect_eq(st.popMax(), -24);
    ctx.expect_eq(st.top(), -71);
    ctx.expect_eq(st.peekMax(), -57);
  });

  ctx.test("interleaved_duplicate_max_and_repush", [&] {
    MaxStack st;
    st.push(10);
    st.push(20);
    st.push(10);
    ctx.expect_eq(st.popMax(), 20);
    ctx.expect_eq(st.top(), 10);
    // Re-pushing after a pop must get a fresh universal ID (not reuse index).
    st.push(10);
    ctx.expect_eq(st.popMax(), 10);
    ctx.expect_eq(st.pop(), 10);
    ctx.expect_eq(st.top(), 10);
  });
}

}  // namespace
