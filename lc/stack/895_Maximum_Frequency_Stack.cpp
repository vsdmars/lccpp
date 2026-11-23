// LeetCode 895: Maximum Frequency Stack
// https://leetcode.com/problems/maximum-frequency-stack/
//
// Problem:
//   Design a stack-like data structure `FreqStack` supporting two operations:
//   - `push(int val)`: pushes integer `val` onto the top of the stack.
//   - `pop()`: removes and returns the most frequent element currently in the
//     stack. If multiple elements tie for maximum frequency, remove and return
//     the one most recently pushed (closest to the top of the stack).
//   It is guaranteed that `pop()` is only called when at least one element is
//   in the stack.
//
// Algorithm:
//   Maintain a hash map `freq_` tracking each value's current occurrence count,
//   and a vector of frequency buckets `groups_` where `groups_[f - 1]` is a
//   stack (LIFO vector) of all values that reached frequency `f`, ordered by
//   the time they reached frequency `f`.
//   - `push(val)`: increment `f = ++freq_[val]`. If `f > groups_.size()`,
//     append a new empty bucket. Push `val` onto `groups_[f - 1]`. Notice we
//     do NOT remove `val` from lower buckets `groups_[0..f-2]`: when `val` is
//     later popped from frequency `f`, its earlier copy in `groups_[f - 2]`
//     remains in exact recency order!
//   - `pop()`: pop the top element `val` from the highest non-empty bucket
//     `groups_.back()` (automatically breaking ties by most recent push). If
//     that bucket becomes empty, pop it from `groups_` so `groups_.size()`
//     always equals the current maximum frequency. Decrement `freq_[val]`
//     (erasing it when it drops to zero) and return `val`.
//   Time Complexity: O(1) amortized for both `push` and `pop`.
//   Space Complexity: O(N), where N is the number of active elements pushed
//   (each push appends exactly one integer to one bucket).
//
// Data Structures:
//   - `std::unordered_map<int, int> freq_`: maps each active value to its
//     current frequency count.
//   - `std::vector<std::vector<int>> groups_`: frequency-indexed array of LIFO
//     stacks; `groups_.size()` tracks the current maximum frequency in O(1).
//
// Real-World Applications:
//   - LFU-like cache eviction and hot-key prioritization structures where ties
//     among equally frequent keys are broken by recency (LIFO/LRU).
//   - Real-time telemetry and anomaly alerting queues prioritizing the most
//     frequently firing, most recently seen alert signatures.
//   - Job schedulers and autocomplete suggestion buffers combining frequency
//     with recency tie-breaking.
//
// Related LeetCode Problems:
//   - #155 Min Stack [done] (augmented O(1) stack queries)
//   - #460 LFU Cache [done] (frequency-bucketed O(1) data structure)
//   - #146 LRU Cache [done] (hash map + recency-ordered container)
//   - #432 All O`one Data Structure [done] (O(1) max/min frequency tracking)
//   - #716 Max Stack [done] (stack supporting max retrieval and removal)

#include "framework/lc_framework.h"

#include <cstddef>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

// Stack-like container returning the most frequent element on `pop()`,
// breaking ties in favor of the element pushed most recently.
class FreqStack {
 public:
  // Pushes `val` onto the stack and records it in its new frequency bucket.
  // Time: O(1) amortized, Space: O(1) per push.
  void push(int val) {
    const int next_freq = ++freq_[val];
    if (std::cmp_greater(next_freq, groups_.size())) {
      // Frequencies grow strictly by +1, so at most one new bucket is needed.
      groups_.emplace_back();
    }
    groups_[static_cast<std::size_t>(next_freq - 1)].push_back(val);
  }

  // Removes and returns the most frequent (and among ties, most recent) value.
  // Precondition: the stack is non-empty.
  // Time: O(1) amortized, Space: O(1).
  int pop() {
    auto& max_bucket = groups_.back();
    const int val = max_bucket.back();
    max_bucket.pop_back();

    // Keep `groups_.back()` non-empty so `groups_.size()` always equals maxf.
    if (max_bucket.empty()) {
      groups_.pop_back();
    }

    if (--freq_[val] == 0) {
      freq_.erase(val);
    }
    return val;
  }

  // Returns whether the frequency stack currently holds no elements.
  [[nodiscard]] bool empty() const noexcept { return groups_.empty(); }

 private:
  // Maps value -> current frequency in the stack.
  std::unordered_map<int, int> freq_;
  // `groups_[f - 1]` is a LIFO stack of values at frequency `f`.
  std::vector<std::vector<int>> groups_;
};

LC_PROBLEM(895, "Maximum Frequency Stack", "stack", ctx) {
  ctx.test("example 1 from prompt", [&] {
    FreqStack fs;
    fs.push(5);
    fs.push(7);
    fs.push(5);
    fs.push(7);
    fs.push(4);
    fs.push(5);

    ctx.expect_eq(fs.pop(), 5);
    ctx.expect_eq(fs.pop(), 7);
    ctx.expect_eq(fs.pop(), 5);
    ctx.expect_eq(fs.pop(), 4);
    ctx.expect_eq(fs.pop(), 7);
    ctx.expect_eq(fs.pop(), 5);
    ctx.expect_true(fs.empty());
  });

  ctx.test("all distinct elements behave like standard LIFO stack", [&] {
    FreqStack fs;
    fs.push(1);
    fs.push(2);
    fs.push(3);
    ctx.expect_eq(fs.pop(), 3);
    ctx.expect_eq(fs.pop(), 2);
    ctx.expect_eq(fs.pop(), 1);
    ctx.expect_true(fs.empty());
  });

  ctx.test("interleaved pushes and pops after draining", [&] {
    FreqStack fs;
    fs.push(4);
    fs.push(0);
    fs.push(9);
    fs.push(3);
    fs.push(4);
    fs.push(2);
    ctx.expect_eq(fs.pop(), 4);

    fs.push(6);
    ctx.expect_eq(fs.pop(), 6);

    fs.push(1);
    ctx.expect_eq(fs.pop(), 1);

    fs.push(1);
    ctx.expect_eq(fs.pop(), 1);

    fs.push(4);
    ctx.expect_eq(fs.pop(), 4);
    ctx.expect_eq(fs.pop(), 2);
    ctx.expect_eq(fs.pop(), 3);
    ctx.expect_eq(fs.pop(), 9);
    ctx.expect_eq(fs.pop(), 0);
    ctx.expect_eq(fs.pop(), 4);
    ctx.expect_true(fs.empty());
  });

  ctx.test("reuse after becoming completely empty", [&] {
    FreqStack fs;
    fs.push(10);
    ctx.expect_eq(fs.pop(), 10);
    ctx.expect_true(fs.empty());

    fs.push(20);
    fs.push(20);
    fs.push(30);
    ctx.expect_eq(fs.pop(), 20);
    ctx.expect_eq(fs.pop(), 30);
    ctx.expect_eq(fs.pop(), 20);
    ctx.expect_true(fs.empty());
  });
}

}  // namespace
