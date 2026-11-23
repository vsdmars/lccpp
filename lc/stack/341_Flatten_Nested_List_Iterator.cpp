// LeetCode 341: Flatten Nested List Iterator
// https://leetcode.com/problems/flatten-nested-list-iterator/
//
// Problem:
//   Given a nested list of integers `nestedList`, where each element is either
//   a single integer or a list whose elements may themselves be integers or
//   other lists, implement a lazy iterator (`NestedIterator`) with `next()`
//   and `hasNext()` that yields all integers in left-to-right flattened order.
//
// Algorithm:
//   Lazy explicit-stack traversal using `(list_ptr, index)` cursor frames:
//   - Maintain a stack of active list frames, initialized with the top-level
//     list at index `0`.
//   - `hasNext()` advances lazily until either the stack is empty or the top
//     frame points directly to an integer:
//     * If the top frame reached the end of its list, pop it.
//     * If the current element is an integer, stop and return `true` (keeping
//       the cursor positioned on that integer so repeated `hasNext()` calls are
//       idempotent).
//     * If the current element is a nested list, advance the parent frame's
//       index and push a new frame for the nested list starting at index `0`.
//   - `next()` invokes `hasNext()` to guarantee positioning on an integer,
//     reads the integer, and advances the top frame's index by 1.
//   This avoids eagerly copying or flattening the entire input up front and
//   cleanly skips arbitrarily nested empty lists such as `[[]]`.
//   Time Complexity: Amortized O(1) per `next()` / `hasNext()` call (O(N + L)
//   total across N integers and L nested lists).
//   Space Complexity: O(D), where D is the maximum nesting depth of the list.
//
// Data Structures:
//   - `NestedInteger`: recursive value type holding either an `int` or a
//     `std::vector<NestedInteger>`.
//   - `std::vector<Frame>`: explicit call stack of non-owning list pointers and
//     element cursors (`O(D)` space).
//
// Real-World Applications:
//   - Streaming JSON/YAML/XML tree iterators that yield leaf scalar values on
//     demand without materializing a flattened copy in memory.
//   - Filesystem directory walkers and composite-pattern tree iterators.
//   - Database query executors implementing Volcano-style lazy iterators over
//     nested/repeated column schemas (e.g., Parquet, Protobuf, BigQuery).
//
// Related LeetCode Problems:
//   - #251 Flatten 2D Vector [done] (lazy iterator over a 2D ragged array)
//   - #281 Zigzag Iterator [done] (interleaving lazy iterator across multiple sequences)
//   - #339 Nested List Weight Sum [done] (depth-weighted traversal over NestedInteger)
//   - #364 Nested List Weight Sum II [done] (bottom-up depth-weighted sum over NestedInteger)
//   - #385 Mini Parser [new] (deserializing string syntax into NestedInteger trees)
//   - #173 Binary Search Tree Iterator [done] (O(H)-space lazy iterator using an explicit stack)

#include "framework/lc_framework.h"

#include <cstddef>
#include <utility>
#include <vector>

namespace {

// Mock of LeetCode's NestedInteger interface: holds either a single integer
// or a list of NestedInteger objects.
class NestedInteger {
 public:
  // Constructs a NestedInteger holding a single integer value.
  explicit NestedInteger(int value) : is_integer_(true), value_(value) {}

  // Constructs a NestedInteger holding a nested list of elements.
  explicit NestedInteger(std::vector<NestedInteger> list)
      : list_(std::move(list)) {}

  [[nodiscard]] bool isInteger() const noexcept { return is_integer_; }

  [[nodiscard]] int getInteger() const noexcept { return value_; }

  [[nodiscard]] const std::vector<NestedInteger>& getList() const noexcept {
    return list_;
  }

 private:
  bool is_integer_ = false;
  int value_ = 0;
  std::vector<NestedInteger> list_;
};

// Lazy depth-first iterator over a nested list of NestedInteger elements.
// Keeps non-owning references into `nested_list`, which must outlive the
// iterator.
// Time: amortized O(1) per next()/hasNext(), Space: O(D) for max depth D.
class NestedIterator {
 public:
  explicit NestedIterator(const std::vector<NestedInteger>& nested_list) {
    if (!nested_list.empty()) {
      stack_.push_back(Frame{.list = &nested_list, .index = 0uz});
    }
  }

  // Returns the next integer in flattened order and advances the cursor.
  int next() {
    if (!hasNext()) {
      return 0;
    }
    Frame& top = stack_.back();
    const int result = (*top.list)[top.index].getInteger();
    ++top.index;
    return result;
  }

  // Advances past any exhausted or empty nested lists until the top cursor
  // points at an integer (returning true) or all elements are consumed (false).
  bool hasNext() {
    while (!stack_.empty()) {
      Frame& top = stack_.back();
      if (top.index == top.list->size()) {
        // Finished iterating this list; return to its parent frame.
        stack_.pop_back();
        continue;
      }

      const NestedInteger& current = (*top.list)[top.index];
      if (current.isInteger()) {
        // Leave `top.index` on this integer so repeated hasNext() calls remain
        // idempotent until next() consumes it.
        return true;
      }

      // Advance parent index before pushing child frame (push_back may
      // reallocate `stack_`, invalidating `top`).
      ++top.index;
      stack_.push_back(Frame{.list = &current.getList(), .index = 0uz});
    }
    return false;
  }

 private:
  struct Frame {
    const std::vector<NestedInteger>* list = nullptr;
    std::size_t index = 0uz;
  };

  std::vector<Frame> stack_;
};

// Helper to drain a NestedIterator into a flat vector for concise assertions.
std::vector<int> collect_all(const std::vector<NestedInteger>& nested_list) {
  NestedIterator iter(nested_list);
  std::vector<int> out;
  while (iter.hasNext()) {
    out.push_back(iter.next());
  }
  return out;
}

// Convenience constructors for readable test fixtures.
NestedInteger make_int(int value) { return NestedInteger(value); }

NestedInteger make_list(std::vector<NestedInteger> items) {
  return NestedInteger(std::move(items));
}

LC_PROBLEM(341, "Flatten Nested List Iterator", "stack", ctx) {
  ctx.test("leetcode_example_1", [&] {
    // [[1, 1], 2, [1, 1]] -> [1, 1, 2, 1, 1]
    std::vector<NestedInteger> input;
    input.push_back(make_list({make_int(1), make_int(1)}));
    input.push_back(make_int(2));
    input.push_back(make_list({make_int(1), make_int(1)}));
    ctx.expect_eq(collect_all(input), std::vector<int>{1, 1, 2, 1, 1});
  });

  ctx.test("leetcode_example_2", [&] {
    // [1, [4, [6]]] -> [1, 4, 6]
    std::vector<NestedInteger> inner6;
    inner6.push_back(make_int(6));
    std::vector<NestedInteger> inner4;
    inner4.push_back(make_int(4));
    inner4.push_back(make_list(std::move(inner6)));
    std::vector<NestedInteger> input;
    input.push_back(make_int(1));
    input.push_back(make_list(std::move(inner4)));
    ctx.expect_eq(collect_all(input), std::vector<int>{1, 4, 6});
  });

  ctx.test("source_build_deeply_nested", [&] {
    // [[1, [3, 2, [1, 2]]], 2, [1, 3]] -> [1, 3, 2, 1, 2, 2, 1, 3]
    std::vector<NestedInteger> level3;
    level3.push_back(make_int(1));
    level3.push_back(make_int(2));

    std::vector<NestedInteger> level2;
    level2.push_back(make_int(3));
    level2.push_back(make_int(2));
    level2.push_back(make_list(std::move(level3)));

    std::vector<NestedInteger> level1;
    level1.push_back(make_int(1));
    level1.push_back(make_list(std::move(level2)));

    std::vector<NestedInteger> tail;
    tail.push_back(make_int(1));
    tail.push_back(make_int(3));

    std::vector<NestedInteger> input;
    input.push_back(make_list(std::move(level1)));
    input.push_back(make_int(2));
    input.push_back(make_list(std::move(tail)));

    ctx.expect_eq(collect_all(input),
                  std::vector<int>{1, 3, 2, 1, 2, 2, 1, 3});
  });

  ctx.test("empty_and_nested_empty_lists", [&] {
    // Completely empty top-level list.
    const std::vector<NestedInteger> empty_top;
    ctx.expect_eq(collect_all(empty_top), std::vector<int>{});

    // [[], [[]], []] contains no integers at all.
    std::vector<NestedInteger> nested_empties;
    nested_empties.push_back(make_list({}));
    std::vector<NestedInteger> inner_empty;
    inner_empty.push_back(make_list({}));
    nested_empties.push_back(make_list(std::move(inner_empty)));
    nested_empties.push_back(make_list({}));
    ctx.expect_eq(collect_all(nested_empties), std::vector<int>{});

    // Interleaved empty lists around a single integer: [[], [42], [[]]].
    std::vector<NestedInteger> mixed;
    mixed.push_back(make_list({}));
    mixed.push_back(make_list({make_int(42)}));
    std::vector<NestedInteger> trailing;
    trailing.push_back(make_list({}));
    mixed.push_back(make_list(std::move(trailing)));

    NestedIterator iter(mixed);
    // Repeated hasNext() calls must be idempotent.
    ctx.expect_true(iter.hasNext());
    ctx.expect_true(iter.hasNext());
    ctx.expect_eq(iter.next(), 42);
    ctx.expect_false(iter.hasNext());
  });
}

}  // namespace
