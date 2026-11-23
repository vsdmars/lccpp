// LeetCode 284: Peeking Iterator
// https://leetcode.com/problems/peeking-iterator/
//
// Problem:
//   Design an iterator wrapper (`PeekingIterator`) that supports the standard
//   `next()` and `hasNext()` operations of an underlying forward iterator,
//   plus a `peek()` operation that returns the next element without advancing
//   the iterator. Follow-up: generalize the design to work with arbitrary
//   element types, not just integers.
//
// Algorithm:
//   Use one-element eager lookahead (buffering). On construction and after
//   every `next()` call, pull the next available element from the underlying
//   iterator into a `std::optional<T>` cache (or reset the cache if exhausted).
//   - `peek()` simply returns the cached value without touching the underlying
//     iterator.
//   - `next()` moves out the cached value, refills the cache from the
//     underlying iterator, and returns the saved value.
//   - `hasNext()` returns whether the cache currently holds a value.
//   Time Complexity: O(1) for construction, `peek()`, `next()`, and
//     `hasNext()`.
//   Space Complexity: O(1) auxiliary space beyond the underlying iterator.
//
// Data Structures:
//   - Underlying `Iterator<T>`: sequential cursor over a `std::vector<T>`.
//   - `std::optional<T>` lookahead buffer: stores at most one prefetched
//     element and cleanly encodes end-of-stream without sentinel values.
//
// Real-World Applications:
//   - LL(1) lexers and recursive-descent parsers that inspect the next token
//     before deciding which grammar rule to apply.
//   - Streaming merge engines (k-way merge in external sort / LSM-tree
//     compaction) that compare heads of sorted runs without consuming them.
//   - Protocol decoders and buffered I/O streams that inspect framing headers
//     before dispatching payload readers.
//
// Related LeetCode Problems:
//   - #173 Binary Search Tree Iterator [done] (stateful iterator with O(h) space)
//   - #251 Flatten 2D Vector [done] (custom forward iterator over nested data)
//   - #281 Zigzag Iterator [done] (interleaving multiple iterators cleanly)
//   - #341 Flatten Nested List Iterator [done] (lazy lookahead via hasNext)

#include "framework/lc_framework.h"

#include <cstddef>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace {

// Minimal forward iterator interface mirroring LeetCode's built-in `Iterator`,
// generalized via template parameter `T` to address the problem's follow-up.
template <typename T = int>
class Iterator {
 public:
  // Initializes the iterator at the beginning of `data`.
  // Time: O(1) (moves ownership), Space: O(n).
  explicit Iterator(std::vector<T> data) : data_(std::move(data)) {}

  // Returns true if at least one unread element remains.
  // Time: O(1), Space: O(1).
  [[nodiscard]] bool hasNext() const noexcept { return index_ < data_.size(); }

  // Consumes and returns the next element in the sequence.
  // Time: O(1), Space: O(1).
  T next() { return data_[index_++]; }

 private:
  std::vector<T> data_;
  std::size_t index_ = 0;
};

// Generic peeking iterator wrapper supporting O(1) lookahead via a single-slot
// `std::optional<T>` cache.
template <typename T = int>
class PeekingIterator {
 public:
  // Wraps `iterator` and eagerly prefetches the first element (if any).
  // Invariant: `cached_` holds the exact element that the next `next()` call
  // will return, or `std::nullopt` iff the stream is exhausted.
  // Time: O(1), Space: O(1).
  explicit PeekingIterator(Iterator<T> iterator)
      : iterator_(std::move(iterator)) {
    advance();
  }

  // Returns the next element without advancing the iterator.
  // Precondition: `hasNext()` is true.
  // Time: O(1), Space: O(1).
  [[nodiscard]] const T& peek() const { return cached_.value(); }

  // Returns the cached next element and advances the lookahead buffer by one.
  // Precondition: `hasNext()` is true.
  // Time: O(1), Space: O(1).
  T next() {
    T current = std::move(cached_.value());
    advance();
    return current;
  }

  // Returns true iff another element is available to peek or consume.
  // Time: O(1), Space: O(1).
  [[nodiscard]] bool hasNext() const noexcept { return cached_.has_value(); }

 private:
  // Refills `cached_` from the underlying iterator, or clears it at EOF.
  void advance() {
    if (iterator_.hasNext()) {
      cached_.emplace(iterator_.next());
    } else {
      cached_.reset();
    }
  }

  Iterator<T> iterator_;
  std::optional<T> cached_;
};

LC_PROBLEM(284, "Peeking Iterator", "design", ctx) {
  ctx.test("leetcode example [1, 2, 3]", [&] {
    PeekingIterator<> it(Iterator<>({1, 2, 3}));
    ctx.expect_true(it.hasNext());
    ctx.expect_eq(it.next(), 1);
    ctx.expect_eq(it.peek(), 2);
    ctx.expect_eq(it.next(), 2);
    ctx.expect_eq(it.next(), 3);
    ctx.expect_false(it.hasNext());
  });

  ctx.test("repeated peeks do not advance", [&] {
    PeekingIterator<> it(Iterator<>({10, 20}));
    ctx.expect_eq(it.peek(), 10);
    ctx.expect_eq(it.peek(), 10);
    ctx.expect_eq(it.peek(), 10);
    ctx.expect_true(it.hasNext());
    ctx.expect_eq(it.next(), 10);
    ctx.expect_eq(it.peek(), 20);
    ctx.expect_eq(it.next(), 20);
    ctx.expect_false(it.hasNext());
  });

  ctx.test("empty iterator", [&] {
    PeekingIterator<> it(Iterator<>({}));
    ctx.expect_false(it.hasNext());
  });

  ctx.test("single element iterator", [&] {
    PeekingIterator<> it(Iterator<>({42}));
    ctx.expect_true(it.hasNext());
    ctx.expect_eq(it.peek(), 42);
    ctx.expect_eq(it.next(), 42);
    ctx.expect_false(it.hasNext());
  });

  ctx.test("generic follow-up with strings", [&] {
    PeekingIterator<std::string> it(
        Iterator<std::string>({"alpha", "beta", "gamma"}));
    std::vector<std::string> collected;
    while (it.hasNext()) {
      const std::string peeked = it.peek();
      const std::string consumed = it.next();
      ctx.expect_eq(peeked, consumed);
      collected.push_back(consumed);
    }
    ctx.expect_eq(collected,
                  std::vector<std::string>{"alpha", "beta", "gamma"});
  });
}

}  // namespace
