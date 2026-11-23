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
// Examples:
//   Example 1:
//     Input:  operations = ["FreqStack", "push(5)", "push(7)", "push(5)",
//             "push(7)", "push(4)", "push(5)", "pop", "pop", "pop", "pop", "pop",
//             "pop"]
//     Output: [null, null, null, null, null, null, null, 5, 7, 5, 4, 7, 5]
//     Explanation:
//       - The six push calls populate freq_ = {5: 3, 7: 2, 4: 1} and
//         frequency-indexed LIFO buckets groups_ = [[5, 7, 4], [5, 7], [5]].
//       - First pop() pops 5 from groups_[2], removes the now-empty groups_[2]
//         bucket, and decrements freq_[5] to 2.
//       - Second pop() pops 7 from the top of groups_[1] = [5, 7] to break the
//         frequency-2 tie by most recent push, setting freq_[7] = 1.
//       - Third pop() pops 5 from groups_[1] = [5], pops the empty bucket so
//         groups_ = [[5, 7, 4]], and sets freq_[5] = 1.
//       - The final three pop() calls drain groups_[0] = [5, 7, 4] in LIFO order,
//         returning 4, 7, and 5 while erasing each key from freq_ and leaving
//         groups_ empty.
//
//   Example 2:
//     Input:  operations = ["FreqStack", "push(1)", "push(2)", "push(3)", "pop",
//             "pop", "pop"]
//     Output: [null, null, null, null, 3, 2, 1]
//     Explanation:
//       - Pushing distinct values 1, 2, and 3 sets freq_ = {1: 1, 2: 1, 3: 1} and
//         places all three in frequency-1 bucket groups_ = [[1, 2, 3]].
//       - Because all three elements tie at maximum frequency 1, the three pop()
//         calls pop from groups_[0] in LIFO order, returning 3, 2, and 1 and
//         leaving groups_ empty.
//
//   Example 3:
//     Input:  operations = ["FreqStack", "push(10)", "pop", "push(20)", "push(20)",
//             "push(30)", "pop", "pop", "pop"]
//     Output: [null, null, 10, null, null, null, 20, 30, 20]
//     Explanation:
//       - push(10) creates freq_ = {10: 1} and groups_ = [[10]], and pop() returns
//         10 while removing both the empty bucket and the key 10 from freq_.
//       - Reusing the empty FreqStack, push(20), push(20), and push(30) build freq_
//         = {20: 2, 30: 1} and groups_ = [[20, 30], [20]].
//       - The next pop() removes 20 from groups_[1], drops empty groups_[1], and
//         sets freq_[20] = 1; the last two pops drain groups_[0] = [20, 30] to
//         return 30 then 20.
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
