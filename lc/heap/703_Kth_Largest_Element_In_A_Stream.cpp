// LeetCode 703: Kth Largest Element In A Stream
// https://leetcode.com/problems/kth-largest-element-in-a-stream/
//
// Problem:
//   Design a class `KthLargest` that tracks the k-th largest element in a
//   continuous stream of integers (counting duplicates, not k-th distinct).
//   - `KthLargest(int k, vector<int>& nums)` initializes the object with `k`
//     and the initial stream elements `nums`.
//   - `int add(int val)` appends `val` to the stream and returns the element
//     representing the k-th largest value seen so far.
//   Constraints guarantee `k >= 1` and at least `k` elements exist whenever
//   `add` returns.
//
// Algorithm:
//   Fixed-Capacity Min-Heap of Size k:
//   Maintain a min-heap storing only the k largest elements seen so far.
//   - On initialization, push each initial number into the min-heap and evict
//     the smallest element whenever the heap size exceeds k.
//   - On `add(val)`, insert `val` (or skip if the heap already has k elements
//     and `val <= min_heap.top()`), evicting the smallest element if size
//     exceeds k. The heap root is always the smallest of the k largest values,
//     which is by definition the k-th largest element overall.
//   Time:  O(n * log(k)) for constructor, O(log(k)) per `add` call.
//   Space: O(k) for the bounded min-heap.
//
// Data Structures:
//   - Min-heap (`std::priority_queue<int, std::vector<int>, std::greater<>>`)
//     capped at k elements so the k-th largest element is accessible at `top()`
//     in O(1) time.
//
// Real-World Applications:
//   - Real-time leaderboards and gaming scoreboards tracking the cutoff score
//     for the top-k players.
//   - Streaming telemetry / SLA monitoring maintaining rolling top-k latency
//     outliers with bounded memory.
//   - Online recommendation systems maintaining a top-k candidate buffer.
//
// Related LeetCode Problems:
//   - #215 Kth Largest Element in an Array [done] (offline static version)
//   - #295 Find Median from Data Stream [done] (two-heap streaming order statistics)
//   - #347 Top K Frequent Elements [done] (bounded min-heap for top-k selection)
//   - #973 K Closest Points to Origin [done] (bounded heap over streaming points)

#include "framework/lc_framework.h"

#include <cstddef>
#include <functional>
#include <queue>
#include <vector>

namespace {

// Tracks the k-th largest element in an append-only integer stream using a
// min-heap capped at size k.
// Constructor: O(n * log(k)) time; `add`: O(log(k)) time; Space: O(k).
class KthLargest {
 public:
  KthLargest(int k, const std::vector<int>& nums)
      : k_(k > 0 ? static_cast<std::size_t>(k) : 0uz) {
    for (const int num : nums) {
      insert(num);
    }
  }

  // Appends `val` to the stream and returns the current k-th largest element.
  int add(int val) {
    insert(val);
    return min_heap_.top();
  }

 private:
  // Maintains the invariant that `min_heap_` holds the at most `k_` largest
  // elements seen so far, with the smallest of them at `top()`.
  void insert(int val) {
    if (k_ == 0uz) {
      return;
    }
    if (min_heap_.size() < k_) {
      min_heap_.push(val);
      return;
    }
    // Only values strictly larger than the current k-th largest can change
    // the top-k set; smaller or equal values can be discarded immediately.
    if (val > min_heap_.top()) {
      min_heap_.pop();
      min_heap_.push(val);
    }
  }

  std::size_t k_ = 0;
  std::priority_queue<int, std::vector<int>, std::greater<>> min_heap_;
};

LC_PROBLEM(703, "Kth Largest Element In A Stream", "heap", ctx) {
  ctx.test("example_1", [&] {
    KthLargest kth(3, {4, 5, 8, 2});
    ctx.expect_eq(kth.add(3), 4);
    ctx.expect_eq(kth.add(5), 5);
    ctx.expect_eq(kth.add(10), 5);
    ctx.expect_eq(kth.add(9), 8);
    ctx.expect_eq(kth.add(4), 8);
  });

  ctx.test("empty_initial_stream_k1", [&] {
    KthLargest kth(1, {});
    ctx.expect_eq(kth.add(-3), -3);
    ctx.expect_eq(kth.add(-2), -2);
    ctx.expect_eq(kth.add(-4), -2);
    ctx.expect_eq(kth.add(0), 0);
    ctx.expect_eq(kth.add(4), 4);
  });

  ctx.test("k_minus_one_initial_elements", [&] {
    KthLargest kth(3, {5, -1});
    ctx.expect_eq(kth.add(2), -1);
    ctx.expect_eq(kth.add(1), 1);
    ctx.expect_eq(kth.add(-1), 1);
    ctx.expect_eq(kth.add(3), 2);
    ctx.expect_eq(kth.add(4), 3);
  });

  ctx.test("duplicates_preserved", [&] {
    KthLargest kth(2, {5, 5, 5});
    ctx.expect_eq(kth.add(1), 5);
    ctx.expect_eq(kth.add(10), 5);
    ctx.expect_eq(kth.add(10), 10);
  });
}

}  // namespace
