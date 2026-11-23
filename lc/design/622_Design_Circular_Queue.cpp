// LeetCode 622: Design Circular Queue
// https://leetcode.com/problems/design-circular-queue/
//
// Problem:
//   Design a fixed-capacity FIFO circular queue (ring buffer) without using a
//   built-in queue container, supporting:
//     - MyCircularQueue(k): Initialize the queue with capacity `k`.
//     - enQueue(value): Insert `value` at the rear; return true if successful,
//       false if the queue is full.
//     - deQueue(): Remove the front element; return true if successful, false
//       if the queue is empty.
//     - Front(): Return the front element, or -1 if the queue is empty.
//     - Rear(): Return the last element, or -1 if the queue is empty.
//     - isEmpty(): Return whether the queue holds zero elements.
//     - isFull(): Return whether the queue has reached capacity `k`.
//
// Algorithm:
//   Back the queue with a contiguous fixed-size array of length `k`, tracking
//   the index `head_` of the front element and the active element count
//   `size_`. Maintaining `size_` cleanly disambiguates the empty (`size_ == 0`)
//   and full (`size_ == capacity_`) states without wasting a sentinel slot or
//   storing a separate boolean flag. The write position is `(head_ + size_) %
//   capacity_` and the rear element sits at `(head_ + size_ - 1) % capacity_`,
//   so every operation wraps around in O(1) modular arithmetic with zero heap
//   allocations after construction.
//   Time Complexity:  O(1) worst-case for every operation.
//   Space Complexity: O(k) contiguous buffer storage.
//
// Data Structures:
//   - std::vector<int>: Fixed-size circular ring buffer allocated once at
//     construction.
//   - Modular head index (`head_`) + element count (`size_`): O(1) FIFO ring
//     pointers.
//
// Real-World Applications:
//   - OS kernel I/O rings (Linux `io_uring`, NIC RX/TX descriptor rings, UART
//     and audio DMA sample buffers).
//   - Lock-free single-producer/single-consumer (SPSC) message queues in
//     high-frequency trading and real-time telemetry pipelines.
//   - Sliding-window stream buffers and bounded work queues in thread pools.
//
// Related LeetCode Problems:
//   - #641 Design Circular Deque [done] (double-ended ring buffer extension)
//   - #232 Implement Queue using Stacks [done] (FIFO queue via two stacks)
//   - #225 Implement Stack using Queues [new] (LIFO stack via FIFO queues)
//   - #346 Moving Average from Data Stream [done] (fixed-window ring buffer)
//   - #933 Number of Recent Calls [new] (bounded sliding-window queue)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

// Fixed-capacity FIFO ring buffer with O(1) operations and no post-init
// allocations.
class MyCircularQueue {
 public:
  explicit MyCircularQueue(int k)
      : capacity_(k > 0 ? static_cast<std::size_t>(k) : 0uz),
        buf_(capacity_, 0) {}

  // Inserts `value` at the rear of the ring buffer; returns false when full.
  // Time: O(1). Space: O(1).
  bool enQueue(int value) {
    if (isFull()) {
      return false;
    }
    const std::size_t tail = (head_ + size_) % capacity_;
    buf_[tail] = value;
    ++size_;
    return true;
  }

  // Removes the front element from the ring buffer; returns false when empty.
  // Time: O(1). Space: O(1).
  bool deQueue() {
    if (isEmpty()) {
      return false;
    }
    head_ = (head_ + 1uz) % capacity_;
    --size_;
    return true;
  }

  // Returns the front element, or -1 if the queue is empty.
  // Time: O(1). Space: O(1).
  [[nodiscard]] int Front() const {
    if (isEmpty()) {
      return -1;
    }
    return buf_[head_];
  }

  // Returns the rear element, or -1 if the queue is empty.
  // Time: O(1). Space: O(1).
  [[nodiscard]] int Rear() const {
    if (isEmpty()) {
      return -1;
    }
    const std::size_t rear = (head_ + size_ - 1uz) % capacity_;
    return buf_[rear];
  }

  // Returns true if the queue currently contains no elements.
  [[nodiscard]] bool isEmpty() const { return size_ == 0uz; }

  // Returns true if the queue has reached its fixed capacity.
  [[nodiscard]] bool isFull() const { return size_ == capacity_; }

 private:
  std::size_t capacity_ = 0uz;
  std::vector<int> buf_;
  std::size_t head_ = 0uz;
  std::size_t size_ = 0uz;
};

LC_PROBLEM(622, "Design Circular Queue", "design", ctx) {
  ctx.test("leetcode_example_1", [&] {
    MyCircularQueue q(3);
    ctx.expect_true(q.isEmpty());
    ctx.expect_false(q.isFull());
    ctx.expect_eq(q.Front(), -1);
    ctx.expect_eq(q.Rear(), -1);

    ctx.expect_true(q.enQueue(1));
    ctx.expect_true(q.enQueue(2));
    ctx.expect_true(q.enQueue(3));
    ctx.expect_false(q.enQueue(4));
    ctx.expect_eq(q.Rear(), 3);
    ctx.expect_true(q.isFull());
    ctx.expect_true(q.deQueue());
    ctx.expect_true(q.enQueue(4));
    ctx.expect_eq(q.Rear(), 4);
    ctx.expect_eq(q.Front(), 2);
  });

  ctx.test("python_main_trace", [&] {
    MyCircularQueue q(6);
    ctx.expect_true(q.enQueue(6));
    ctx.expect_eq(q.Rear(), 6);
    ctx.expect_eq(q.Rear(), 6);
    ctx.expect_true(q.deQueue());
    ctx.expect_true(q.enQueue(5));
    ctx.expect_eq(q.Rear(), 5);
    ctx.expect_true(q.deQueue());
    ctx.expect_eq(q.Front(), -1);
    ctx.expect_false(q.deQueue());
    ctx.expect_false(q.deQueue());
    ctx.expect_false(q.deQueue());
  });

  ctx.test("wraparound_multiple_cycles", [&] {
    MyCircularQueue q(4);
    ctx.expect_true(q.enQueue(3));
    ctx.expect_eq(q.Front(), 3);
    ctx.expect_false(q.isFull());
    ctx.expect_true(q.enQueue(7));
    ctx.expect_true(q.enQueue(2));
    ctx.expect_true(q.enQueue(5));
    ctx.expect_true(q.isFull());
    ctx.expect_true(q.deQueue());
    ctx.expect_true(q.enQueue(4));
    ctx.expect_false(q.enQueue(2));
    ctx.expect_false(q.isEmpty());
    ctx.expect_eq(q.Front(), 7);
    ctx.expect_eq(q.Rear(), 4);
  });

  ctx.test("capacity_one", [&] {
    MyCircularQueue q(1);
    ctx.expect_true(q.enQueue(42));
    ctx.expect_true(q.isFull());
    ctx.expect_eq(q.Front(), 42);
    ctx.expect_eq(q.Rear(), 42);
    ctx.expect_false(q.enQueue(99));
    ctx.expect_true(q.deQueue());
    ctx.expect_true(q.isEmpty());
    ctx.expect_true(q.enQueue(99));
    ctx.expect_eq(q.Front(), 99);
    ctx.expect_eq(q.Rear(), 99);
  });
}

}  // namespace
