// LeetCode 1188: Design Bounded Blocking Queue
// https://leetcode.com/problems/design-bounded-blocking-queue/
//
// Problem:
//   Implement a thread-safe bounded blocking FIFO queue supporting:
//     - `BoundedBlockingQueue(int capacity)`: initialize with fixed capacity.
//     - `void enqueue(int element)`: append `element` to the queue, blocking
//       the calling producer thread while the queue is at full capacity.
//     - `int dequeue()`: remove and return the oldest element in the queue,
//       blocking the calling consumer thread while the queue is empty.
//     - `int size()`: return the current number of stored elements safely.
//
// Examples:
//   Example 1:
//     Input:  operations = ["BoundedBlockingQueue", "size", "enqueue", "enqueue",
//             "size", "dequeue", "enqueue", "dequeue", "dequeue", "size"], args =
//             [[2], [], [10], [20], [], [], [30], [], [], []]
//     Output: [null, 0, null, null, 2, 10, null, 20, 30, 0]
//     Explanation:
//       - Constructing BoundedBlockingQueue(2) sets capacity_ = 2 with an empty
//         queue_, so the initial size() call acquires mutex_ and returns 0.
//       - enqueue(10) and enqueue(20) both satisfy queue_.size() < 2 without
//         blocking, leaving queue_ = [10, 20] and size() returning 2.
//       - dequeue() pops 10 from the front of queue_ and notifies not_full_,
//         allowing enqueue(30) to push 30 so queue_ becomes [20, 30].
//       - The final two dequeue() calls return 20 and 30 in FIFO order, leaving
//         queue_ empty so the last size() returns 0.
//
//   Example 2:
//     Input:  capacity = 2; producer thread enqueues [1, 0, 2, 3, 4]; consumer
//             thread calls dequeue() 3 times; then main calls size(), dequeue(),
//             dequeue(), size()
//     Output: consumer dequeued = [1, 0, 2], size() = 2, remaining dequeues = [3,
//             4], final size() = 0
//     Explanation:
//       - After the producer enqueues 1 and 0, queue_ reaches capacity_ = 2 and the
//         producer blocks in not_full_.wait until queue_.size() < 2 holds again.
//       - Each of the consumer thread's three dequeue() calls waits on not_empty_,
//         pops the front element, and calls not_full_.notify_one(), waking the
//         producer to push the remaining items and yielding dequeued = [1, 0, 2].
//       - Once both threads join, [3, 4] remain in queue_, so size() returns 2, two
//         more dequeue() calls return 3 and 4, and the final size() returns 0.
//
// Algorithm:
//   Use the classic C++ monitor pattern (mutex + two condition variables)
//   wrapping a FIFO `std::deque<int>`. This is the idiomatic C++23 counterpart
//   to a two-semaphore + lock implementation, while also keeping `size()`
//   strictly data-race-free:
//     - `enqueue(element)` acquires `mutex_`, waits on `not_full_` with a
//       predicate lambda (`queue_.size() < capacity_`) to guard against
//       spurious wakeups, pushes `element` to the back, unlocks before
//       signaling (to avoid wake-then-block lock convoying), and notifies one
//       waiting consumer via `not_empty_.notify_one()`.
//     - `dequeue()` acquires `mutex_`, waits on `not_empty_` until
//       `!queue_.empty()`, pops the front element, unlocks, and notifies one
//       waiting producer via `not_full_.notify_one()`.
//     - `size()` acquires `mutex_` with `std::scoped_lock` and returns
//       `queue_.size()`.
//   Time Complexity:  O(1) per `enqueue`, `dequeue`, and `size` (excluding
//                     blocking wait time when full/empty).
//   Space Complexity: O(capacity) for the bounded underlying deque.
//
// Data Structures:
//   - `std::deque<int> queue_`: O(1) push-back / pop-front FIFO buffer.
//   - `mutable std::mutex mutex_`: serializes all accesses to `queue_`.
//   - `std::condition_variable not_full_`, `not_empty_`: block and wake
//     producers and consumers without busy-waiting.
//
// Real-World Applications:
//   - Thread-pool work queues and producer-consumer pipelines with backpressure
//     (preventing unbounded memory growth under bursty load).
//   - OS kernel pipes, network socket send/receive ring buffers, and audio/video
//     streaming frame buffers.
//   - Log aggregation and telemetry batching queues in high-throughput servers.
//
// Related LeetCode Problems:
//   - #1114 Print in Order [new] (thread synchronization with condition variables)
//   - #1115 Print FooBar Alternately [new] (alternating producer/consumer barrier)
//   - #1242 Web Crawler Multithreaded [done] (concurrent BFS with thread-safe queue)
//   - #622 Design Circular Queue [done] (single-threaded fixed-capacity FIFO buffer)

#include "framework/lc_framework.h"

#include <algorithm>
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <mutex>
#include <thread>
#include <vector>

namespace {

// Thread-safe fixed-capacity FIFO queue blocking producers when full and
// consumers when empty.
class BoundedBlockingQueue {
 public:
  // Constructs an empty blocking queue with the given positive capacity.
  explicit BoundedBlockingQueue(int capacity)
      : capacity_(static_cast<std::size_t>(capacity)) {}

  // Enqueues `element` at the rear of the FIFO queue. Blocks while full.
  // Time: O(1) (excluding wait), Space: O(1).
  void enqueue(int element) {
    std::unique_lock<std::mutex> lock(mutex_);
    // Predicate loop protects against spurious wakeups and racing producers.
    not_full_.wait(lock, [this] { return queue_.size() < capacity_; });
    queue_.push_back(element);
    // Unlock before notifying so the woken consumer can acquire mutex_ cleanly.
    lock.unlock();
    not_empty_.notify_one();
  }

  // Removes and returns the oldest element from the queue. Blocks while empty.
  // Time: O(1) (excluding wait), Space: O(1).
  int dequeue() {
    std::unique_lock<std::mutex> lock(mutex_);
    not_empty_.wait(lock, [this] { return !queue_.empty(); });
    const int front = queue_.front();
    queue_.pop_front();
    lock.unlock();
    not_full_.notify_one();
    return front;
  }

  // Returns the instantaneous number of elements in the queue safely.
  // Time: O(1), Space: O(1).
  [[nodiscard]] int size() const {
    std::scoped_lock<std::mutex> lock(mutex_);
    return static_cast<int>(queue_.size());
  }

 private:
  std::size_t capacity_ = 0uz;
  mutable std::mutex mutex_;
  std::condition_variable not_full_;
  std::condition_variable not_empty_;
  std::deque<int> queue_;
};

LC_PROBLEM(1188, "Design Bounded Blocking Queue", "design", ctx) {
  ctx.test("single_thread_fifo_within_capacity", [&] {
    BoundedBlockingQueue q(2);
    ctx.expect_eq(q.size(), 0);
    q.enqueue(10);
    q.enqueue(20);
    ctx.expect_eq(q.size(), 2);
    ctx.expect_eq(q.dequeue(), 10);
    q.enqueue(30);
    ctx.expect_eq(q.dequeue(), 20);
    ctx.expect_eq(q.dequeue(), 30);
    ctx.expect_eq(q.size(), 0);
  });

  ctx.test("example_1_single_producer_single_consumer", [&] {
    // Mirrors Example 1: capacity 2, 5 enqueues [1, 0, 2, 3, 4], 3 dequeues.
    // Producer must block on capacity=2 until consumer drains items; final
    // queue size must be 2 and dequeued order must be [1, 0, 2].
    BoundedBlockingQueue q(2);
    std::vector<int> dequeued;
    dequeued.reserve(3uz);

    std::thread producer([&] {
      for (const int val : {1, 0, 2, 3, 4}) {
        q.enqueue(val);
      }
    });

    std::thread consumer([&] {
      for (int i = 0; i < 3; ++i) {
        dequeued.push_back(q.dequeue());
      }
    });

    producer.join();
    consumer.join();

    const std::vector<int> expected = {1, 0, 2};
    ctx.expect_eq(dequeued, expected);
    ctx.expect_eq(q.size(), 2);
    ctx.expect_eq(q.dequeue(), 3);
    ctx.expect_eq(q.dequeue(), 4);
    ctx.expect_eq(q.size(), 0);
  });

  ctx.test("multi_producer_multi_consumer_stress", [&] {
    constexpr int kProducers = 4;
    constexpr int kConsumers = 4;
    constexpr int kItemsPerWorker = 50;
    constexpr int kTotalItems = kProducers * kItemsPerWorker;

    BoundedBlockingQueue q(5);
    std::mutex results_mutex;
    std::vector<int> consumed;
    consumed.reserve(static_cast<std::size_t>(kTotalItems));

    std::vector<std::thread> workers;
    workers.reserve(static_cast<std::size_t>(kProducers) +
                    static_cast<std::size_t>(kConsumers));

    for (int p = 0; p < kProducers; ++p) {
      workers.emplace_back([&, p] {
        const int base = p * kItemsPerWorker;
        for (int i = 0; i < kItemsPerWorker; ++i) {
          q.enqueue(base + i);
        }
      });
    }

    for (int c = 0; c < kConsumers; ++c) {
      workers.emplace_back([&] {
        for (int i = 0; i < kItemsPerWorker; ++i) {
          const int val = q.dequeue();
          const std::scoped_lock<std::mutex> lock(results_mutex);
          consumed.push_back(val);
        }
      });
    }

    for (std::thread& worker : workers) {
      worker.join();
    }

    std::ranges::sort(consumed);
    std::vector<int> expected;
    expected.reserve(static_cast<std::size_t>(kTotalItems));
    for (int i = 0; i < kTotalItems; ++i) {
      expected.push_back(i);
    }

    ctx.expect_eq(consumed, expected);
    ctx.expect_eq(q.size(), 0);
  });
}

}  // namespace
