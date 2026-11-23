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
