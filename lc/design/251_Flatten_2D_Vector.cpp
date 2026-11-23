// LeetCode 251: Flatten 2D Vector
// https://leetcode.com/problems/flatten-2d-vector/
//
// Problem:
//   Design an iterator `Vector2D` that flattens a 2D vector of integers into
//   a 1D row-major stream, supporting:
//     - `next()`: Returns the next element and advances the iterator.
//     - `hasNext()`: Returns true if there are still elements remaining.
//   Inner vectors may be empty (including leading, consecutive, or trailing
//   empty rows).
//
// Examples:
//   Example 1:
//     Input:  vec = [[1, 2], [3], [4, 5, 6]], iterate while hasNext()
//     Output: [1, 2, 3, 4, 5, 6]
//     Explanation:
//       - Construction sets `(row_, col_) = (0, 0)` since `data_[0]` is non-empty.
//       - The first two `next()` calls yield `data_[0][0] = 1` (advancing `col_` to
//         `1`) and `data_[0][1] = 2`, after which `advance_to_valid()` sees `col_
//         == 2 == data_[0].size()` and moves to `(1, 0)`.
//       - Subsequent `next()` calls yield `data_[1][0] = 3` (moving to `(2, 0)`)
//         and then `4`, `5`, and `6` across row `2`, advancing `row_` to `3 ==
//         data_.size()` so `hasNext()` returns `false`.
//
//   Example 2:
//     Input:  vec = [[], [1], [], [], [2, 3], [], [4], []], iterate while hasNext()
//     Output: [1, 2, 3, 4]
//     Explanation:
//       - In the constructor, `advance_to_valid()` skips empty row `0` (`col_ == 0
//         == data_[0].size()`) and positions `(row_, col_)` at `(1, 0)`.
//       - Calling `next()` returns `1` and increments `col_` to `1 ==
//         data_[1].size()`, so `advance_to_valid()` skips empty rows `2` and `3` to
//         land at `(4, 0)`.
//       - The next two `next()` calls return `2` and `3` from row `4`, and
//         `advance_to_valid()` skips empty row `5` to reach `(6, 0)`.
//       - The final `next()` returns `4`, and `advance_to_valid()` skips trailing
//         empty row `7` so `row_ = 8 == data_.size()` and `hasNext()` returns
//         `false`.
//
//   Example 3:
//     Input:  vec = [[], [], []], call hasNext()
//     Output: false
//     Explanation:
//       - Construction initializes `(row_, col_) = (0, 0)` and calls
//         `advance_to_valid()`.
//       - Because `col_ == 0 == data_[row_].size()` for rows `0`, `1`, and `2`, the
//         loop increments `row_` until `row_ = 3 == data_.size()`.
//       - `hasNext()` evaluates `row_ < data_.size()` (`3 < 3`) and returns
//         `false`.
//
// Algorithm:
//   Two-Index Lazy Cursor with Eager Empty-Row Skipping:
//   - Store the 2D vector alongside a cursor `(row_, col_)`.
//   - Maintain a strong class invariant via `advance_to_valid()`: after
//     construction and after every `next()` call, either `row_ == data_.size()`
//     (iterator exhausted) or `col_ < data_[row_].size()` (cursor points
//     directly at the next element to yield).
//   - `advance_to_valid()` skips any empty rows (`col_ == data_[row_].size()`),
//     resetting `col_ = 0` when moving to the next row.
//   - Because the invariant always holds, `hasNext()` is a strict O(1) `const`
//     check (`row_ < data_.size()`), and `next()` works safely even if callers
//     invoke `next()` directly without calling `hasNext()` first.
//   - Time Complexity: Amortized O(1) per `next()` call (each row and element
//     is visited once across the lifetime of the iterator); O(1) for `hasNext()`.
//     Constructor + full traversal takes O(R + N) where R is row count and N is
//     total element count.
//   - Space Complexity: O(1) auxiliary space beyond storing the input 2D vector.
//
// Data Structures:
//   - Owned 2D vector (`std::vector<std::vector<int>>`) and two cursor indices
//     (`row_`, `col_` of type `std::size_t`).
//
// Real-World Applications:
//   - C++23 `std::views::join` and streaming query engines flattening nested
//     batches/partitions without materializing a combined buffer.
//   - Paginated API / database cursor adapters presenting chunked result pages
//     as a uniform record iterator.
//   - Sparse matrix CSR/adjacency-list traversal where empty buckets must be
//     skipped transparently.
//
// Related LeetCode Problems:
//   - #281 Zigzag Iterator [done] (multi-vector cyclic/alternating iterator design)
//   - #341 Flatten Nested List Iterator [done] (stack-based iterator over arbitrarily nested lists)
//   - #173 Binary Search Tree Iterator [done] (lazy in-order iterator with controlled memory)
//   - #284 Peeking Iterator [done] (wrapper iterator adding lookahead buffering)

#include "framework/lc_framework.h"

#include <cstddef>
#include <utility>
#include <vector>

namespace {

// Row-major lazy iterator over a 2D vector, skipping empty inner vectors.
class Vector2D {
 public:
  // Takes ownership of `vec` and positions the cursor on the first element.
  explicit Vector2D(std::vector<std::vector<int>> vec)
      : data_(std::move(vec)) {
    advance_to_valid();
  }

  // Returns the current element and advances the cursor to the next valid
  // element (precondition: `hasNext()` is true).
  // Time: Amortized O(1).
  int next() {
    const int val = data_[row_][col_];
    ++col_;
    advance_to_valid();
    return val;
  }

  // Returns true if at least one unvisited element remains.
  // Time: O(1).
  [[nodiscard]] bool hasNext() const noexcept { return row_ < data_.size(); }

 private:
  // Advances `(row_, col_)` past any exhausted or empty rows so the invariant
  // (`row_ == data_.size()` or `col_ < data_[row_].size()`) always holds.
  void advance_to_valid() noexcept {
    while (row_ < data_.size() && col_ == data_[row_].size()) {
      ++row_;
      col_ = 0uz;
    }
  }

  std::vector<std::vector<int>> data_;
  std::size_t row_ = 0uz;
  std::size_t col_ = 0uz;
};

LC_PROBLEM(251, "Flatten 2D Vector", "design", ctx) {
  ctx.test("example_1_standard_2d_vector", [&] {
    Vector2D it({{1, 2}, {3}, {4, 5, 6}});
    std::vector<int> out;
    while (it.hasNext()) {
      out.push_back(it.next());
    }
    ctx.expect_eq(out, std::vector<int>({1, 2, 3, 4, 5, 6}));
  });

  ctx.test("python_build_leading_empty_row", [&] {
    Vector2D it({{}, {3}});
    std::vector<int> out;
    while (it.hasNext()) {
      out.push_back(it.next());
    }
    ctx.expect_eq(out, std::vector<int>({3}));
  });

  ctx.test("sparse_empty_rows_everywhere", [&] {
    Vector2D it({{}, {1}, {}, {}, {2, 3}, {}, {4}, {}});
    std::vector<int> out;
    while (it.hasNext()) {
      out.push_back(it.next());
    }
    ctx.expect_eq(out, std::vector<int>({1, 2, 3, 4}));
  });

  ctx.test("all_empty_vectors", [&] {
    Vector2D it1(std::vector<std::vector<int>>{});
    ctx.expect_false(it1.hasNext());

    Vector2D it2({{}, {}, {}});
    ctx.expect_false(it2.hasNext());
  });

  ctx.test("next_called_without_prior_hasNext", [&] {
    Vector2D it({{}, {10, 20}, {}, {30}});
    ctx.expect_eq(it.next(), 10);
    ctx.expect_eq(it.next(), 20);
    ctx.expect_eq(it.next(), 30);
    ctx.expect_false(it.hasNext());
  });
}

}  // namespace
