// LeetCode 281: Zigzag Iterator
// https://leetcode.com/problems/zigzag-iterator/
//
// Problem:
//   Given two 1D integer vectors `v1` and `v2`, design an iterator that yields
//   their elements alternately (e.g., [1, 2] and [3, 4, 5, 6] -> [1, 3, 2, 4,
//   5, 6]). Follow-up: generalize cleanly to `k` 1D vectors traversed in
//   cyclic round-robin order.
//
// Examples:
//   Example 1:
//     Input:  v1 = [1, 2], v2 = [3, 4, 5, 6], iterate while hasNext()
//     Output: [1, 3, 2, 4, 5, 6]
//     Explanation:
//       - Construction stores `vectors_ = [[1, 2], [3, 4, 5, 6]]` and pushes
//         initial cursors `(0, 0)` and `(1, 0)` into `active_`.
//       - The first two `next()` calls pop `(0, 0)` to yield `1` (re-enqueuing `(0,
//         1)`) and `(1, 0)` to yield `3` (re-enqueuing `(1, 1)`).
//       - The third `next()` pops `(0, 1)` to yield `2` and drops vector `0`
//         because `elem_idx + 1 = 2 == vectors_[0].size()`.
//       - The remaining calls pop `(1, 1)`, `(1, 2)`, and `(1, 3)` in order,
//         yielding `4`, `5`, and `6` before `active_` becomes empty and `hasNext()`
//         returns `false`.
//
//   Example 2:
//     Input:  vectors = [[1, 2, 3], [4, 5, 6, 7], [8, 9]], iterate while hasNext()
//     Output: [1, 4, 8, 2, 5, 9, 3, 6, 7]
//     Explanation:
//       - Construction enqueues `(0, 0)`, `(1, 0)`, and `(2, 0)` into `active_` for
//         the three non-empty vectors.
//       - Round 1 pops each front cursor to yield `1`, `4`, and `8`, re-enqueuing
//         `(0, 1)`, `(1, 1)`, and `(2, 1)`.
//       - Round 2 yields `2`, `5`, and `9`, re-enqueuing `(0, 2)` and `(1, 2)`
//         while dropping vector `2` since `elem_idx + 1 = 2 == vectors_[2].size()`.
//       - Round 3 yields `3` (dropping vector `0`) and `6` (re-enqueuing `(1, 3)`),
//         and the final `next()` pops `(1, 3)` to yield `7` until `active_` is
//         empty.
//
//   Example 3:
//     Input:  vectors = [[], [10, 20], [], [30], []], iterate while hasNext()
//     Output: [10, 30, 20]
//     Explanation:
//       - The constructor skips empty rows `0`, `2`, and `4`, enqueuing only `(1,
//         0)` and `(3, 0)` into `active_`.
//       - The first `next()` pops `(1, 0)` to yield `10` and pushes `(1, 1)` to the
//         back of `active_`.
//       - The second `next()` pops `(3, 0)` to yield `30` (dropping exhausted
//         vector `3`), and the final `next()` pops `(1, 1)` to yield `20` before
//         `active_` becomes empty.
//
// Algorithm:
//   Round-Robin Queue of Active Vector Cursors:
//   - Store the input vectors in `vectors_` and push a cursor
//     `(vec_index, elem_index = 0)` into a FIFO `std::deque` for every
//     non-empty vector.
//   - `hasNext()` checks whether the cursor queue is non-empty in O(1) time.
//   - `next()` pops the front cursor `(vec_idx, elem_idx)`, reads
//     `vectors_[vec_idx][elem_idx]`, and if `elem_idx + 1` is still within
//     that vector's bounds, pushes `(vec_idx, elem_idx + 1)` to the back of
//     the queue.
//   - Because exhausted vectors are dropped immediately rather than re-queued,
//     every `next()` call runs in strict O(1) time even when `k` is large and
//     most vectors have already finished (avoiding O(k) scanning or O(n) list
//     slicing).
//   - Time Complexity: O(k) construction; strict O(1) for both `next()` and
//     `hasNext()`.
//   - Space Complexity: O(k) auxiliary space for the active cursor queue
//     (beyond storing the input vectors).
//
// Data Structures:
//   - Owned vector list (`std::vector<std::vector<int>>`) holding the input
//     sequences.
//   - FIFO cursor queue (`std::deque<Cursor>`) scheduling active non-empty
//     vectors in round-robin order.
//
// Real-World Applications:
//   - Round-robin task/packet scheduling across multiple fair queues (e.g., OS
//     CPU schedulers, network router weighted/deficit round-robin queues).
//   - Interleaved multi-shard log/event stream readers ensuring no single
//     producer starves others.
//   - Striped batch data loaders in ML training pipelines pulling evenly from
//     multiple dataset shards.
//
// Related LeetCode Problems:
//   - #251 Flatten 2D Vector [done] (row-major lazy 2D vector iterator)
//   - #341 Flatten Nested List Iterator [done] (lazy iterator over nested collections)
//   - #173 Binary Search Tree Iterator [done] (stateful iterator design pattern)
//   - #23 Merge k Sorted Lists [done] (combining k streams using a priority queue instead of FIFO)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <utility>
#include <vector>

namespace {

// Cyclic round-robin iterator supporting both the 2-vector LeetCode interface
// and the k-vector follow-up in strict O(1) time per step.
class ZigzagIterator {
 public:
  // Standard 2-vector constructor delegating to the general k-vector design.
  ZigzagIterator(std::vector<int> v1, std::vector<int> v2)
      : ZigzagIterator(make_pair_vectors(std::move(v1), std::move(v2))) {}

  // General k-vector constructor enqueuing cursors for all non-empty vectors.
  // Time: O(k).
  explicit ZigzagIterator(std::vector<std::vector<int>> vectors)
      : vectors_(std::move(vectors)) {
    for (std::size_t i = 0uz; i < vectors_.size(); ++i) {
      if (!vectors_[i].empty()) {
        active_.emplace_back(i, 0uz);
      }
    }
  }

  // Yields the next element in cyclic order and re-enqueues the vector cursor
  // if more elements remain in that vector (precondition: `hasNext()` is true).
  // Time: O(1).
  int next() {
    const Cursor curr = active_.front();
    active_.pop_front();

    const int val = vectors_[curr.vec_idx][curr.elem_idx];
    const std::size_t next_elem = curr.elem_idx + 1uz;
    if (next_elem < vectors_[curr.vec_idx].size()) {
      active_.emplace_back(curr.vec_idx, next_elem);
    }
    return val;
  }

  // Returns true if any input vector still has unvisited elements.
  // Time: O(1).
  [[nodiscard]] bool hasNext() const noexcept { return !active_.empty(); }

 private:
  struct Cursor {
    std::size_t vec_idx = 0uz;
    std::size_t elem_idx = 0uz;

    Cursor(std::size_t v, std::size_t e) : vec_idx(v), elem_idx(e) {}
  };

  static std::vector<std::vector<int>> make_pair_vectors(std::vector<int> v1,
                                                         std::vector<int> v2) {
    std::vector<std::vector<int>> combined;
    combined.reserve(2uz);
    combined.push_back(std::move(v1));
    combined.push_back(std::move(v2));
    return combined;
  }

  std::vector<std::vector<int>> vectors_;
  std::deque<Cursor> active_;
};

LC_PROBLEM(281, "Zigzag Iterator", "design", ctx) {
  ctx.test("example_1_two_vectors_unequal_length", [&] {
    ZigzagIterator it({1, 2}, {3, 4, 5, 6});
    std::vector<int> out;
    while (it.hasNext()) {
      out.push_back(it.next());
    }
    ctx.expect_eq(out, std::vector<int>({1, 3, 2, 4, 5, 6}));
  });

  ctx.test("python_build_first_longer", [&] {
    ZigzagIterator it({1, 2, 3}, {4});
    std::vector<int> out;
    while (it.hasNext()) {
      out.push_back(it.next());
    }
    ctx.expect_eq(out, std::vector<int>({1, 4, 2, 3}));
  });

  ctx.test("one_vector_empty", [&] {
    ZigzagIterator it({}, {1});
    std::vector<int> out;
    while (it.hasNext()) {
      out.push_back(it.next());
    }
    ctx.expect_eq(out, std::vector<int>({1}));
  });

  ctx.test("both_vectors_empty", [&] {
    ZigzagIterator it({}, {});
    ctx.expect_false(it.hasNext());
  });

  ctx.test("followup_k_vectors_cyclic", [&] {
    ZigzagIterator it(
        std::vector<std::vector<int>>{{1, 2, 3}, {4, 5, 6, 7}, {8, 9}});
    std::vector<int> out;
    while (it.hasNext()) {
      out.push_back(it.next());
    }
    ctx.expect_eq(out, std::vector<int>({1, 4, 8, 2, 5, 9, 3, 6, 7}));
  });

  ctx.test("followup_k_vectors_with_empty_rows", [&] {
    ZigzagIterator it(
        std::vector<std::vector<int>>{{}, {10, 20}, {}, {30}, {}});
    std::vector<int> out;
    while (it.hasNext()) {
      out.push_back(it.next());
    }
    ctx.expect_eq(out, std::vector<int>({10, 30, 20}));
  });
}

}  // namespace
