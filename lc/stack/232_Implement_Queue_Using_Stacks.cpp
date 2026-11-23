// LeetCode 232: Implement Queue Using Stacks
// https://leetcode.com/problems/implement-queue-using-stacks/
//
// Problem:
//   Implement a first-in-first-out (FIFO) queue using only two standard LIFO
//   stacks. The queue must support push(x) to append to the back, pop() to
//   remove and return the front element, peek() to inspect the front element,
//   and empty() to check whether the queue contains any elements. Only valid
//   stack operations (push, top, pop, size, empty) are permitted.
//
// Examples:
//   Example 1:
//     Input:  ["MyQueue", "empty", "push", "push", "peek", "pop", "empty", "pop",
//             "empty"], [[], [], [1], [2], [], [], [], [], []]
//     Output: [null, true, null, null, 1, 1, false, 2, true]
//     Explanation:
//       - Initially both in_stack_ and out_stack_ are empty, so empty() returns
//         true.
//       - push(1) and push(2) push onto in_stack_, leaving in_stack_ = [1, 2] (top
//         2) and out_stack_ = [].
//       - peek() calls transfer_if_needed(), which pours in_stack_ into out_stack_
//         = [2, 1] (top 1), and returns 1.
//       - pop() pops 1 from out_stack_ (leaving out_stack_ = [2]), empty() returns
//         false, and the second pop() removes and returns 2, leaving both stacks
//         empty so empty() returns true.
//
//   Example 2:
//     Input:  ["MyQueue", "push", "push", "pop", "push", "push", "peek", "pop",
//             "pop", "pop", "empty"], [[], [10], [20], [], [30], [40], [], [], [],
//             [], []]
//     Output: [null, null, null, 10, null, null, 20, 20, 30, 40, true]
//     Explanation:
//       - push(10) and push(20) load in_stack_ = [10, 20]; pop() transfers both to
//         out_stack_ = [20, 10] and pops 10, leaving out_stack_ = [20].
//       - push(30) and push(40) append to in_stack_ = [30, 40] without disturbing
//         out_stack_ = [20].
//       - peek() and the next pop() see out_stack_ is non-empty, so
//         transfer_if_needed() does nothing and they read then pop 20, leaving
//         out_stack_ = [].
//       - The next pop() finds out_stack_ empty and transfers in_stack_ = [30, 40]
//         into out_stack_ = [40, 30], popping 30 then 40 before empty() returns
//         true.
//
//   Example 3:
//     Input:  ["MyQueue", "push", "peek", "pop", "empty", "push", "push", "push",
//             "pop", "pop", "pop"], [[], [1], [], [], [], [-5], [0], [7], [], [],
//             []]
//     Output: [null, null, 1, 1, true, null, null, null, -5, 0, 7]
//     Explanation:
//       - push(1) puts 1 in in_stack_; peek() transfers it to out_stack_ and
//         returns 1, then pop() removes it so empty() returns true.
//       - After draining, push(-5), push(0), and push(7) push onto in_stack_ = [-5,
//         0, 7].
//       - The first subsequent pop() transfers all three into out_stack_ = [7, 0,
//         -5], and the three pop() calls return -5, 0, and 7 in FIFO order.
//
// Algorithm:
//   Use lazy two-stack transfer. Maintain an input stack (`in_stack_`) for
//   incoming pushes and an output stack (`out_stack_`) for front reads/pops.
//   - push(x): push directly onto `in_stack_` in O(1) time.
//   - peek() / pop(): if `out_stack_` is empty, pour all elements from
//     `in_stack_` into `out_stack_`, reversing LIFO order into FIFO order.
//     Then read or pop the top of `out_stack_`.
//   - empty(): true iff both stacks are empty.
//   Because every element is pushed onto `in_stack_` once, moved to
//   `out_stack_` at most once, and popped from `out_stack_` once, all
//   operations run in amortized O(1) time (worst-case O(N) on a single
//   transfer) and O(N) total space for N queued elements.
//
// Data Structures:
//   - Two `std::stack<int>` containers (`in_stack_` and `out_stack_`), enforcing
//     strict LIFO access semantics at compile time.
//
// Real-World Applications:
//   - Functional programming languages (Haskell, OCaml, Erlang) implementing
//     persistent amortized-O(1) FIFO queues using two singly linked lists.
//   - Bounded message buffers and undo/redo pipelines where producers append to
//     one stack while consumers drain the reversed front stack.
//   - Batching network or disk write buffers that accumulate entries before
//     flushing in arrival order.
//
// Related LeetCode Problems:
//   - #225 Implement Stack using Queues [new] (dual design simulating LIFO using FIFO queues)
//   - #155 Min Stack [done] (stack design supporting O(1) auxiliary queries)
//   - #622 Design Circular Queue [done] (fixed-capacity ring-buffer FIFO queue)
//   - #641 Design Circular Deque [done] (double-ended queue with O(1) operations at both ends)
//   - #716 Max Stack [done] (augmented stack supporting extended element retrieval)

#include "framework/lc_framework.h"

#include <stack>
#include <vector>

namespace {

// FIFO queue implemented strictly in terms of two LIFO stacks using lazy
// transfer for amortized O(1) time per operation and O(N) space.
class MyQueue {
 public:
  // Appends `x` to the back of the queue. Time: O(1).
  void push(int x) { in_stack_.push(x); }

  // Removes and returns the front element of the queue.
  // Precondition: queue is non-empty. Time: amortized O(1).
  int pop() {
    transfer_if_needed();
    const int front = out_stack_.top();
    out_stack_.pop();
    return front;
  }

  // Returns the front element without removing it.
  // Precondition: queue is non-empty. Time: amortized O(1).
  int peek() {
    transfer_if_needed();
    return out_stack_.top();
  }

  // Returns true if no elements are currently stored in either stack. Time: O(1).
  [[nodiscard]] bool empty() const {
    return in_stack_.empty() && out_stack_.empty();
  }

 private:
  // Lazily reverses `in_stack_` into `out_stack_` only when `out_stack_` is
  // exhausted, preserving strict FIFO order across interleaved pushes and pops.
  void transfer_if_needed() {
    if (!out_stack_.empty()) {
      return;
    }
    while (!in_stack_.empty()) {
      out_stack_.push(in_stack_.top());
      in_stack_.pop();
    }
  }

  std::stack<int> in_stack_;
  std::stack<int> out_stack_;
};

LC_PROBLEM(232, "Implement Queue Using Stacks", "stack", ctx) {
  ctx.test("leetcode_example_1", [&] {
    MyQueue queue;
    ctx.expect_true(queue.empty());
    queue.push(1);
    queue.push(2);
    ctx.expect_eq(queue.peek(), 1);
    ctx.expect_eq(queue.pop(), 1);
    ctx.expect_false(queue.empty());
    ctx.expect_eq(queue.pop(), 2);
    ctx.expect_true(queue.empty());
  });

  ctx.test("interleaved_push_and_pop", [&] {
    MyQueue queue;
    queue.push(10);
    queue.push(20);
    ctx.expect_eq(queue.pop(), 10);

    // Pushing while out_stack_ still holds 20 must not disrupt FIFO order.
    queue.push(30);
    queue.push(40);
    ctx.expect_eq(queue.peek(), 20);
    ctx.expect_eq(queue.pop(), 20);
    ctx.expect_eq(queue.pop(), 30);
    ctx.expect_eq(queue.pop(), 40);
    ctx.expect_true(queue.empty());
  });

  ctx.test("single_element_and_reuse_after_empty", [&] {
    MyQueue queue;
    queue.push(1);
    ctx.expect_eq(queue.peek(), 1);
    ctx.expect_eq(queue.pop(), 1);
    ctx.expect_true(queue.empty());

    // Queue should work cleanly after being completely drained.
    queue.push(-5);
    queue.push(0);
    queue.push(7);
    std::vector<int> drained;
    while (!queue.empty()) {
      drained.push_back(queue.pop());
    }
    ctx.expect_eq(drained, std::vector<int>{-5, 0, 7});
  });

  ctx.test("repeated_peeks_do_not_mutate_front", [&] {
    MyQueue queue;
    for (int i = 1; i <= 5; ++i) {
      queue.push(i);
    }
    ctx.expect_eq(queue.peek(), 1);
    ctx.expect_eq(queue.peek(), 1);
    ctx.expect_eq(queue.pop(), 1);
    ctx.expect_eq(queue.peek(), 2);
  });
}

}  // namespace
