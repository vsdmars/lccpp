// LeetCode 155: Min Stack
// https://leetcode.com/problems/min-stack/
//
// Problem:
//   Design a stack that supports `push(val)`, `pop()`, `top()`, and
//   `getMin()` — retrieving the minimum element currently in the stack — all
//   in O(1) time.
//
// Algorithm:
//   Augment each pushed element with the running minimum of the stack up to
//   and including that element: store `Entry{value, min_so_far}`.
//   - `push(val)`: compute `next_min = stack.empty() ? val : min(val,
//     stack.back().min_so_far)` and push `{val, next_min}`.
//   - `pop()`: pop the top `Entry`; the previous running minimum is restored
//     automatically at the new stack top.
//   - `top()`: return `stack.back().value`.
//   - `getMin()`: return `stack.back().min_so_far`.
//   Time complexity: O(1) for every operation (`push`, `pop`, `top`,
//   `getMin`).
//   Space complexity: O(n), where n is the number of elements currently in
//   the stack.
//
// Data Structures:
//   - std::vector<Entry> serving as a contiguous LIFO stack where each entry
//     pairs the raw value with the prefix minimum down to the bottom of the
//     stack.
//
// Real-World Applications:
//   - Sliding-window minimum/maximum queries (two MinStacks implement a
//     MinQueue in amortized O(1) per operation).
//   - Undo/redo history stacks in editors and financial-ledger snapshots that
//     need O(1) aggregate invariants (running min/max/watermark) at any state.
//   - Runtime profilers and embedded telemetry monitors tracking minimum
//     available memory or latency across nested call frames.
//
// Related LeetCode Problems:
//   - #232 Implement Queue using Stacks [done] (two-stack queue construction)
//   - #239 Sliding Window Maximum [done] (window extrema via monotonic deque)
//   - #716 Max Stack [done] (max stack with arbitrary popMax support)
//   - #20 Valid Parentheses [done] (fundamental LIFO stack usage)

#include "framework/lc_framework.h"

#include <algorithm>
#include <limits>
#include <vector>

namespace {

// Stack supporting O(1) push, pop, top, and minimum-element retrieval by
// co-locating each pushed value with the prefix minimum up to that depth.
class MinStack {
 public:
  // Pushes `val` onto the stack and records the updated prefix minimum.
  // Time: O(1) amortized.
  void push(int val) {
    const int min_so_far =
        stack_.empty() ? val : std::min(val, stack_.back().min_so_far);
    stack_.push_back(Entry{.value = val, .min_so_far = min_so_far});
  }

  // Removes the element on top of the stack (precondition: non-empty).
  // Time: O(1).
  void pop() { stack_.pop_back(); }

  // Returns the element on top of the stack (precondition: non-empty).
  // Time: O(1).
  [[nodiscard]] int top() const { return stack_.back().value; }

  // Returns the minimum element currently in the stack (precondition:
  // non-empty). Time: O(1).
  [[nodiscard]] int getMin() const { return stack_.back().min_so_far; }

 private:
  struct Entry {
    int value = 0;
    int min_so_far = 0;
  };

  // Invariant: for every index i, `stack_[i].min_so_far` equals
  // `min(stack_[0..i].value)`.
  std::vector<Entry> stack_;
};

LC_PROBLEM(155, "Min Stack", "stack", ctx) {
  ctx.test("leetcode_example", [&] {
    MinStack min_stack;
    min_stack.push(-2);
    min_stack.push(0);
    min_stack.push(-3);
    ctx.expect_eq(min_stack.getMin(), -3);
    min_stack.pop();
    ctx.expect_eq(min_stack.top(), 0);
    ctx.expect_eq(min_stack.getMin(), -2);
  });

  ctx.test("source_main_example", [&] {
    MinStack min_stack;
    min_stack.push(-10);
    min_stack.push(3);
    min_stack.push(-27);
    min_stack.push(7);
    min_stack.pop();
    ctx.expect_eq(min_stack.getMin(), -27);
    ctx.expect_eq(min_stack.top(), -27);
  });

  ctx.test("duplicate_minimums_and_extremes", [&] {
    MinStack min_stack;
    constexpr int kMinInt = std::numeric_limits<int>::min();
    constexpr int kMaxInt = std::numeric_limits<int>::max();

    min_stack.push(kMaxInt);
    min_stack.push(1);
    min_stack.push(1);
    min_stack.push(kMinInt);
    ctx.expect_eq(min_stack.getMin(), kMinInt);

    min_stack.pop();
    ctx.expect_eq(min_stack.getMin(), 1);
    ctx.expect_eq(min_stack.top(), 1);

    min_stack.pop();
    // Second copy of 1 is still on the stack.
    ctx.expect_eq(min_stack.getMin(), 1);

    min_stack.pop();
    ctx.expect_eq(min_stack.getMin(), kMaxInt);
    ctx.expect_eq(min_stack.top(), kMaxInt);
  });
}

}  // namespace
