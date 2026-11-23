// LeetCode 641: Design Circular Deque
// https://leetcode.com/problems/design-circular-deque/
//
// Problem:
//   Design a fixed-capacity circular double-ended queue (deque) without using
//   a built-in deque library, supporting:
//     - MyCircularDeque(k): Initialize the deque with maximum capacity `k`.
//     - insertFront(value) / insertLast(value): Insert an element at the front
//       or rear; return true on success, false if full.
//     - deleteFront() / deleteLast(): Remove the front or rear element; return
//       true on success, false if empty.
//     - getFront() / getRear(): Return the front or rear element, or -1 if
//       empty.
//     - isEmpty() / isFull(): Check whether the deque is empty or full.
//
// Examples:
//   Example 1:
//     Input:  MyCircularDeque(3), isEmpty(), getFront(), getRear(), insertLast(1),
//             insertLast(2), insertFront(3), insertFront(4), getRear(), isFull(),
//             deleteLast(), insertFront(4), getFront(), getRear()
//     Output: [null, true, -1, -1, true, true, true, false, 2, true, true, true, 4,
//             1]
//     Explanation:
//       - MyCircularDeque(3) allocates buf_ of size 3 with head_ = 0 and size_ = 0,
//         so isEmpty() returns true and getFront()/getRear() return -1.
//       - insertLast(1) and insertLast(2) write at (0 + 0) % 3 = 0 and (0 + 1) % 3
//         = 1, leaving size_ = 2 and buf_ = [1, 2, 0].
//       - insertFront(3) wraps head_ from 0 to 2 and writes 3 at buf_[2] with size_
//         = 3, causing insertFront(4) to return false because isFull() is true.
//       - getRear() reads index (2 + 3 - 1) % 3 = 1 (value 2), then deleteLast()
//         decrements size_ to 2 and drops 2 from the rear.
//       - insertFront(4) decrements head_ from 2 to 1 and sets buf_[1] = 4 (size_ =
//         3), so getFront() returns buf_[1] = 4 and getRear() reads (1 + 3 - 1) % 3
//         = 0 (value 1).
//
//   Example 2:
//     Input:  MyCircularDeque(3), insertFront(10), insertFront(20),
//             insertFront(30), isFull(), getFront(), getRear(), deleteFront(),
//             getFront(), deleteLast(), getFront(), getRear(), deleteFront(),
//             isEmpty(), deleteFront(), deleteLast()
//     Output: [null, true, true, true, true, 30, 10, true, 20, true, 20, 20, true,
//             true, false, false]
//     Explanation:
//       - Three insertFront calls wrap head_ from 0 -> 2 -> 1 -> 0, storing 10 at
//         buf_[2], 20 at buf_[1], and 30 at buf_[0] with size_ = 3 (isFull() =
//         true, getFront() = 30, getRear() = 10).
//       - deleteFront() advances head_ to (0 + 1) % 3 = 1 and sets size_ = 2,
//         making getFront() return buf_[1] = 20.
//       - deleteLast() decrements size_ to 1, leaving only buf_[1] = 20 so both
//         getFront() and getRear() read index 1 and return 20.
//       - deleteFront() advances head_ to 2 and sets size_ = 0, so isEmpty()
//         returns true and subsequent deleteFront()/deleteLast() calls return
//         false.
//
//   Example 3:
//     Input:  MyCircularDeque(1), insertFront(7), isFull(), insertLast(8),
//             getFront(), getRear(), deleteLast(), isEmpty(), insertLast(9),
//             getFront(), getRear()
//     Output: [null, true, true, false, 7, 7, true, true, true, 9, 9]
//     Explanation:
//       - insertFront(7) wraps head_ from 0 to capacity_ - 1 = 0 and writes buf_[0]
//         = 7 with size_ = 1, so isFull() is true and insertLast(8) returns false.
//       - getFront() and getRear() both read index 0 and return 7, then
//         deleteLast() sets size_ = 0 so isEmpty() becomes true.
//       - insertLast(9) computes tail = (0 + 0) % 1 = 0 and writes buf_[0] = 9 with
//         size_ = 1, so getFront() and getRear() both return 9.
//
// Algorithm:
//   Store elements in a contiguous fixed-size circular array (`std::vector`)
//   of length `k`, tracked by the front index `head_` and active count `size_`.
//   - insertFront decrements `head_` modulo `k` (wrapping `0 -> k - 1`) and
//     writes at `buf_[head_]`, then increments `size_`.
//   - insertLast writes at `buf_[(head_ + size_) % k]` and increments `size_`.
//   - deleteFront advances `head_ = (head_ + 1) % k` and decrements `size_`.
//   - deleteLast simply decrements `size_`.
//   Compared to a heap-allocated doubly linked list, the circular ring buffer
//   preserves strict O(1) time for every operation while eliminating per-node
//   allocations and maximizing CPU cache locality.
//   Time Complexity:  O(1) worst-case for all operations.
//   Space Complexity: O(k) contiguous buffer storage.
//
// Data Structures:
//   - std::vector<int>: Pre-allocated circular ring buffer of size `k`.
//   - Front index (`head_`) + active length (`size_`): Bidirectional O(1)
//     circular indexing with unambiguous empty/full checks.
//
// Real-World Applications:
//   - Work-stealing thread-pool schedulers (Chase-Lev deque), where owner
//     threads push/pop at one end and thief threads steal from the other.
//   - Monotonic deque sliding-window extrema trackers (e.g., streaming latency
//     min/max over bounded windows).
//   - Bounded undo/redo command histories and browser back/forward navigation
//     buffers that drop the oldest entry when full.
//
// Related LeetCode Problems:
//   - #622 Design Circular Queue [done] (single-ended FIFO ring buffer)
//   - #239 Sliding Window Maximum [done] (monotonic deque application)
//   - #1670 Design Front Middle Back Queue [new] (deque with middle operations)
//   - #146 LRU Cache [done] (O(1) double-ended list eviction structure)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

// Fixed-capacity circular double-ended queue backed by a contiguous ring
// buffer.
class MyCircularDeque {
 public:
  explicit MyCircularDeque(int k)
      : capacity_(k > 0 ? static_cast<std::size_t>(k) : 0uz),
        buf_(capacity_, 0) {}

  // Inserts `value` at the front of the deque; returns false if full.
  // Time: O(1). Space: O(1).
  bool insertFront(int value) {
    if (isFull()) {
      return false;
    }
    // Step `head_` backward by 1 slot with wraparound from 0 to capacity_ - 1.
    head_ = (head_ == 0uz) ? (capacity_ - 1uz) : (head_ - 1uz);
    buf_[head_] = value;
    ++size_;
    return true;
  }

  // Inserts `value` at the rear of the deque; returns false if full.
  // Time: O(1). Space: O(1).
  bool insertLast(int value) {
    if (isFull()) {
      return false;
    }
    const std::size_t tail = (head_ + size_) % capacity_;
    buf_[tail] = value;
    ++size_;
    return true;
  }

  // Removes the front element; returns false if empty.
  // Time: O(1). Space: O(1).
  bool deleteFront() {
    if (isEmpty()) {
      return false;
    }
    head_ = (head_ + 1uz) % capacity_;
    --size_;
    return true;
  }

  // Removes the rear element; returns false if empty.
  // Time: O(1). Space: O(1).
  bool deleteLast() {
    if (isEmpty()) {
      return false;
    }
    --size_;
    return true;
  }

  // Returns the front element, or -1 if empty.
  // Time: O(1). Space: O(1).
  [[nodiscard]] int getFront() const {
    if (isEmpty()) {
      return -1;
    }
    return buf_[head_];
  }

  // Returns the rear element, or -1 if empty.
  // Time: O(1). Space: O(1).
  [[nodiscard]] int getRear() const {
    if (isEmpty()) {
      return -1;
    }
    const std::size_t rear = (head_ + size_ - 1uz) % capacity_;
    return buf_[rear];
  }

  // Returns true if the deque contains no elements.
  [[nodiscard]] bool isEmpty() const { return size_ == 0uz; }

  // Returns true if the deque has reached its fixed capacity.
  [[nodiscard]] bool isFull() const { return size_ == capacity_; }

 private:
  std::size_t capacity_ = 0uz;
  std::vector<int> buf_;
  std::size_t head_ = 0uz;
  std::size_t size_ = 0uz;
};

LC_PROBLEM(641, "Design Circular Deque", "design", ctx) {
  ctx.test("leetcode_example_1", [&] {
    MyCircularDeque dq(3);
    ctx.expect_true(dq.isEmpty());
    ctx.expect_eq(dq.getFront(), -1);
    ctx.expect_eq(dq.getRear(), -1);

    ctx.expect_true(dq.insertLast(1));
    ctx.expect_true(dq.insertLast(2));
    ctx.expect_true(dq.insertFront(3));
    ctx.expect_false(dq.insertFront(4));
    ctx.expect_eq(dq.getRear(), 2);
    ctx.expect_true(dq.isFull());
    ctx.expect_true(dq.deleteLast());
    ctx.expect_true(dq.insertFront(4));
    ctx.expect_eq(dq.getFront(), 4);
    ctx.expect_eq(dq.getRear(), 1);
  });

  ctx.test("front_only_wraparound_and_drain", [&] {
    MyCircularDeque dq(3);
    ctx.expect_true(dq.insertFront(10));
    ctx.expect_true(dq.insertFront(20));
    ctx.expect_true(dq.insertFront(30));
    ctx.expect_true(dq.isFull());
    ctx.expect_eq(dq.getFront(), 30);
    ctx.expect_eq(dq.getRear(), 10);

    ctx.expect_true(dq.deleteFront());
    ctx.expect_eq(dq.getFront(), 20);
    ctx.expect_true(dq.deleteLast());
    ctx.expect_eq(dq.getFront(), 20);
    ctx.expect_eq(dq.getRear(), 20);
    ctx.expect_true(dq.deleteFront());
    ctx.expect_true(dq.isEmpty());
    ctx.expect_false(dq.deleteFront());
    ctx.expect_false(dq.deleteLast());
  });

  ctx.test("capacity_one", [&] {
    MyCircularDeque dq(1);
    ctx.expect_true(dq.insertFront(7));
    ctx.expect_true(dq.isFull());
    ctx.expect_false(dq.insertLast(8));
    ctx.expect_eq(dq.getFront(), 7);
    ctx.expect_eq(dq.getRear(), 7);
    ctx.expect_true(dq.deleteLast());
    ctx.expect_true(dq.isEmpty());
    ctx.expect_true(dq.insertLast(9));
    ctx.expect_eq(dq.getFront(), 9);
    ctx.expect_eq(dq.getRear(), 9);
  });
}

}  // namespace
