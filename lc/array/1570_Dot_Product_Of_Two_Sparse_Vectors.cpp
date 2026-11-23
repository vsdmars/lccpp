// LeetCode 1570: Dot Product Of Two Sparse Vectors
// https://leetcode.com/problems/dot-product-of-two-sparse-vectors/
//
// Problem:
//   Design a `SparseVector` class that efficiently stores a mostly-zero
//   integer vector `nums` (1 <= nums.size() <= 10^5, 0 <= nums[i] <= 100) and
//   computes the dot product between two equal-length `SparseVector` instances
//   via `dotProduct(vec)`.
//   Follow-up: What if only one of the two vectors is sparse?
//
// Algorithm:
//   Compressed Non-Zero Index-Value List + Two-Pointer Merge:
//   1. Construction: scan `nums` once and retain only `(index, value)` pairs
//      where `value != 0`. Because we iterate in ascending index order, the
//      stored list is naturally sorted by index without extra sorting.
//   2. Dot product: run a classic sorted two-pointer merge over both sparse
//      vectors' non-zero entry lists. Whenever both pointers refer to the same
//      index, accumulate `val1 * val2` and advance both; otherwise advance the
//      pointer with the smaller index.
//   (For the follow-up where one vector has L1 << L2 non-zero entries, binary
//   searching each of the L1 indices in the larger sorted array runs in
//   O(L1 * log L2) time; the sorted-pair representation supports both.)
//   Time Complexity:  O(n) construction; O(L1 + L2) dot product where L1, L2
//                     are the counts of non-zero elements in each vector.
//   Space Complexity: O(L) per vector to store non-zero pairs; O(1) auxiliary
//                     space during `dotProduct`.
//
// Data Structures:
//   - `std::vector<std::pair<std::size_t, int>>`: compact cache-friendly
//     coordinate-list (COO) representation storing only non-zero entries in
//     strictly increasing index order, avoiding hash-table overhead.
//
// Real-World Applications:
//   - Information retrieval and search engines computing cosine similarity
//     between high-dimensional TF-IDF or BM25 document vectors.
//   - Recommendation systems evaluating inner products of sparse user-item
//     interaction rows and embedding gradients.
//   - Scientific computing and finite-element solvers performing sparse
//     matrix-vector products (SpMV) in CSR/COO formats.
//
// Related LeetCode Problems:
//   - #311 Sparse Matrix Multiplication [done] (extending sparse dot products to 2D matrices)
//   - #349 Intersection of Two Arrays [done] (sorted two-pointer index intersection)
//   - #1229 Meeting Scheduler [new] (two-pointer scan over sorted intervals/indices)
//   - #1868 Product of Two Run-Length Encoded Arrays [done] (two-pointer product over compressed arrays)

#include "framework/lc_framework.h"

#include <cstddef>
#include <utility>
#include <vector>

namespace {

// Stores a sparse integer vector as a sorted list of (index, value) pairs for
// all non-zero entries, supporting O(L1 + L2) cache-friendly dot products.
class SparseVector {
 public:
  // Extracts all non-zero (index, value) pairs in ascending index order.
  // Time: O(n), Space: O(L) where L is the number of non-zero elements.
  explicit SparseVector(const std::vector<int>& nums) {
    for (std::size_t idx = 0uz; idx < nums.size(); ++idx) {
      if (nums[idx] != 0) {
        entries_.emplace_back(idx, nums[idx]);
      }
    }
  }

  // Computes the dot product with another SparseVector via two-pointer merge.
  //
  // Invariant: All matching non-zero indices strictly less than
  // min(entries_[i].first, vec.entries_[j].first) have already been added to
  // `product`.
  //
  // Time: O(L1 + L2), Space: O(1).
  [[nodiscard]] int dotProduct(const SparseVector& vec) const {
    int product = 0;
    std::size_t i = 0uz;
    std::size_t j = 0uz;

    while (i < entries_.size() && j < vec.entries_.size()) {
      const auto [idx1, val1] = entries_[i];
      const auto [idx2, val2] = vec.entries_[j];

      if (idx1 == idx2) {
        // Matching non-zero coordinate contributes to the inner product.
        product += val1 * val2;
        ++i;
        ++j;
      } else if (idx1 < idx2) {
        // Advance the lagging pointer until it catches up or passes `idx2`.
        ++i;
      } else {
        ++j;
      }
    }

    return product;
  }

  // Exposes the number of stored non-zero entries (for testing compression).
  [[nodiscard]] std::size_t non_zero_count() const noexcept {
    return entries_.size();
  }

 private:
  // Sorted (index, value) pairs for every non-zero coordinate.
  std::vector<std::pair<std::size_t, int>> entries_;
};

LC_PROBLEM(1570, "Dot Product Of Two Sparse Vectors", "array", ctx) {
  ctx.test("example_1_partial_overlap", [&] {
    const SparseVector v1({1, 0, 0, 2, 3});
    const SparseVector v2({0, 3, 0, 4, 0});
    ctx.expect_eq(v1.non_zero_count(), 3uz);
    ctx.expect_eq(v2.non_zero_count(), 2uz);
    ctx.expect_eq(v1.dotProduct(v2), 8);
    ctx.expect_eq(v2.dotProduct(v1), 8);
  });

  ctx.test("example_2_disjoint_non_zeros", [&] {
    const SparseVector v1({0, 1, 0, 0, 0});
    const SparseVector v2({0, 0, 0, 0, 2});
    ctx.expect_eq(v1.dotProduct(v2), 0);
  });

  ctx.test("example_3_single_shared_index", [&] {
    const SparseVector v1({0, 1, 0, 0, 2, 0, 0});
    const SparseVector v2({1, 0, 0, 0, 3, 0, 4});
    ctx.expect_eq(v1.dotProduct(v2), 6);
  });

  ctx.test("all_zeros_vectors", [&] {
    const SparseVector v1({0, 0, 0, 0});
    const SparseVector v2({0, 0, 0, 0});
    ctx.expect_eq(v1.non_zero_count(), 0uz);
    ctx.expect_eq(v1.dotProduct(v2), 0);
  });

  ctx.test("one_sparse_one_dense_followup", [&] {
    std::vector<int> sparse(1000uz, 0);
    sparse[10uz] = 7;
    sparse[999uz] = 5;
    const std::vector<int> dense(1000uz, 3);

    const SparseVector v_sparse(sparse);
    const SparseVector v_dense(dense);
    ctx.expect_eq(v_sparse.non_zero_count(), 2uz);
    ctx.expect_eq(v_sparse.dotProduct(v_dense), (7 * 3) + (5 * 3));
  });
}

}  // namespace
