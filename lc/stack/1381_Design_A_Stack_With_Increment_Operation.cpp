// LeetCode 1381: Design A Stack With Increment Operation
// https://leetcode.com/problems/design-a-stack-with-increment-operation/
//
// Problem:
//   Design a bounded stack `CustomStack(int maxSize)` supporting:
//     - `push(x)`: pushes `x` onto the stack if current size < `maxSize`.
//     - `pop()`: removes and returns the top element, or returns `-1` if empty.
//     - `increment(k, val)`: adds `val` to the bottom `k` elements of the stack
//       (or to all elements if the stack currently holds fewer than `k`).
//
// Examples:
//   Example 1:
//     Input:  ["CustomStack", "push", "push", "pop", "push", "push", "push",
//             "increment", "increment", "pop", "pop", "pop", "pop"], [[3], [1],
//             [2], [], [2], [3], [4], [5, 100], [2, 100], [], [], [], []]
//     Output: [null, null, null, 2, null, null, null, null, null, 103, 202, 201,
//             -1]
//     Explanation:
//       - push(1), push(2), pop() returns 2, then push(2) and push(3) leave stack_
//         = [{1, 0}, {2, 0}, {3, 0}]; push(4) is ignored because size == max_size_
//         (3).
//       - increment(5, 100) adds 100 at idx = min(5, 3) - 1 = 2 ({3, 100}), and
//         increment(2, 100) adds 100 at idx = 1 ({2, 100}).
//       - First pop() returns 3 + 100 = 103 and propagates lazy_inc = 100 to index
//         1, updating it to {2, 200}.
//       - Next pop() returns 2 + 200 = 202 and propagates 200 down to index 0 ({1,
//         200}), which pops 201, and the final pop() on the empty stack returns -1.
//
//   Example 2:
//     Input:  ["CustomStack", "push", "push", "push", "increment", "increment",
//             "pop", "push", "increment", "pop", "pop", "pop", "pop"], [[4], [10],
//             [20], [30], [1, 5], [3, 10], [], [99], [2, 1], [], [], [], []]
//     Output: [null, null, null, null, null, null, 40, null, null, 99, 31, 26, -1]
//     Explanation:
//       - Push 10, 20, and 30, then apply increment(1, 5) to stack_[0] ({10, 5})
//         and increment(3, 10) to stack_[2] ({30, 10}).
//       - pop() returns 30 + 10 = 40 and propagates lazy_inc = 10 to stack_[1]
//         ({20, 10}).
//       - push(99) appends {99, 0} at index 2 without inheriting earlier deltas,
//         and increment(2, 1) adds 1 to stack_[1] ({20, 11}).
//       - Remaining pops return 99 + 0 = 99, 20 + 11 = 31 (cascading 11 to
//         stack_[0] for lazy_inc = 16), 10 + 16 = 26, and -1.
//
//   Example 3:
//     Input:  ["CustomStack", "pop", "increment", "push", "pop", "pop"], [[2], [],
//             [5, 50], [7], [], []]
//     Output: [null, -1, null, null, 7, -1]
//     Explanation:
//       - Initial pop() sees stack_.empty() and returns -1.
//       - increment(5, 50) returns early on an empty stack so no delta is stored.
//       - push(7) inserts {7, 0}, pop() returns 7, and the final pop() returns -1.
//
// Algorithm:
//   Lazy Propagation on Stack Elements:
//     Instead of eagerly iterating over the bottom `k` elements during
//     `increment(k, val)` (which costs O(k)), attach a `lazy_inc` offset to
//     each stack slot representing a deferred delta that applies to that slot
//     AND every slot below it.
//       - `push(x)`: appends `{x, 0}` if `size < max_size_`.
//       - `increment(k, val)`: locates `idx = min(k, size) - 1` and adds `val`
//         to `stack_[idx].lazy_inc` in O(1).
//       - `pop()`: pops `{value, lazy_inc}`, propagates `lazy_inc` down to the
//         new top slot (`stack_.back().lazy_inc += lazy_inc`) so all elements
//         below still receive the deferred delta, and returns `value + lazy_inc`.
//   Time Complexity:  O(1) for `push`, `pop`, and `increment`.
//   Space Complexity: O(maxSize) for the backing array.
//
// Data Structures:
//   - Contiguous stack (`std::vector<Entry>`) where each `Entry` pairs the
//     pushed base value with a lazy prefix-increment accumulator.
//
// Real-World Applications:
//   - Lazy propagation in segment trees, Fenwick/difference arrays, and range
//     update buffers for financial order books and time-series windows.
//   - Deferred frame/offset adjustments in virtual machine call stacks and
//     undo/redo transaction stacks with bulk rebase operations.
//
// Related LeetCode Problems:
//   - #155 Min Stack [done] (augmenting each stack entry with auxiliary state)
//   - #716 Max Stack [done] (augmented stack supporting non-LIFO queries)
//   - #370 Range Addition [done] (difference-array / lazy prefix range updates)
//   - #1109 Corporate Flight Bookings [new] (batch prefix-sum range increments)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

// Bounded stack supporting O(1) push, O(1) pop, and O(1) bottom-k increments
// via lazy downward delta propagation.
class CustomStack {
 public:
  // Initializes an empty stack with capacity `max_size`.
  explicit CustomStack(int max_size)
      : max_size_(max_size > 0 ? static_cast<std::size_t>(max_size) : 0uz) {
    stack_.reserve(max_size_);
  }

  // Pushes `x` onto the top of the stack if capacity permits. Time: O(1).
  void push(int x) {
    if (stack_.size() < max_size_) {
      stack_.push_back(Entry{.value = x, .lazy_inc = 0});
    }
  }

  // Pops and returns the effective top value (base + accumulated lazy delta),
  // passing the lazy delta down to the new top element. Returns -1 if empty.
  // Time: O(1).
  int pop() {
    if (stack_.empty()) {
      return -1;
    }

    const Entry top = stack_.back();
    stack_.pop_back();

    // Propagate the deferred prefix increment to the element immediately below
    // so every element in the prefix still observes it when eventually popped.
    if (!stack_.empty()) {
      stack_.back().lazy_inc += top.lazy_inc;
    }
    return top.value + top.lazy_inc;
  }

  // Lazily adds `val` to the bottom `k` elements (or all elements if size < k).
  // Time: O(1).
  void increment(int k, int val) {
    if (stack_.empty() || k <= 0) {
      return;
    }

    // Record the delta only at the highest affected index; pops will cascade
    // it downward to indices 0 .. idx - 1.
    const std::size_t idx =
        std::min(static_cast<std::size_t>(k), stack_.size()) - 1uz;
    stack_[idx].lazy_inc += val;
  }

 private:
  struct Entry {
    int value = 0;
    int lazy_inc = 0;
  };

  std::vector<Entry> stack_;
  std::size_t max_size_ = 0;
};

LC_PROBLEM(1381, "Design A Stack With Increment Operation", "stack", ctx) {
  ctx.test("example_1", [&] {
    CustomStack stk(3);
    stk.push(1);
    stk.push(2);
    ctx.expect_eq(stk.pop(), 2);
    stk.push(2);
    stk.push(3);
    stk.push(4);  // Exceeds maxSize = 3; ignored.
    stk.increment(5, 100);
    stk.increment(2, 100);
    ctx.expect_eq(stk.pop(), 103);
    ctx.expect_eq(stk.pop(), 202);
    ctx.expect_eq(stk.pop(), 201);
    ctx.expect_eq(stk.pop(), -1);
  });

  ctx.test("empty_stack_operations", [&] {
    CustomStack stk(2);
    ctx.expect_eq(stk.pop(), -1);
    // Incrementing an empty stack must not affect elements pushed later.
    stk.increment(5, 50);
    stk.push(7);
    ctx.expect_eq(stk.pop(), 7);
    ctx.expect_eq(stk.pop(), -1);
  });

  ctx.test("overlapping_increments_and_interleaved_pushes", [&] {
    CustomStack stk(4);
    stk.push(10);
    stk.push(20);
    stk.push(30);
    stk.increment(1, 5);   // [15, 20, 30]
    stk.increment(3, 10);  // [25, 30, 40]
    ctx.expect_eq(stk.pop(), 40);
    // Pushing after a pop must NOT inherit prior lazy deltas below it.
    stk.push(99);          // [25, 30, 99]
    stk.increment(2, 1);   // [26, 31, 99]
    ctx.expect_eq(stk.pop(), 99);
    ctx.expect_eq(stk.pop(), 31);
    ctx.expect_eq(stk.pop(), 26);
    ctx.expect_eq(stk.pop(), -1);
  });

  ctx.test("capacity_one_and_negative_delta", [&] {
    CustomStack stk(1);
    stk.push(10);
    stk.push(20);  // Ignored.
    stk.increment(10, -3);
    stk.increment(0, 100);  // k <= 0 no-op guard.
    ctx.expect_eq(stk.pop(), 7);
    ctx.expect_eq(stk.pop(), -1);
  });
}

}  // namespace
