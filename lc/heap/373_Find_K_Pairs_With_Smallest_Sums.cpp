// LeetCode 373: Find K Pairs With Smallest Sums
// https://leetcode.com/problems/find-k-pairs-with-smallest-sums/
//
// Problem:
//   Given two integer arrays `nums1` and `nums2` sorted in non-decreasing
//   order and an integer `k`, define a pair `(u, v)` with one element from
//   `nums1` and one element from `nums2`. Return the `k` pairs with the
//   smallest sums `u + v` (or all `nums1.size() * nums2.size()` pairs if fewer
//   than `k` exist).
//
// Algorithm:
//   Best-first search over the virtual sorted `m x n` sum matrix using a
//   min-heap without needing a `visited` hash set:
//   Because both arrays are sorted, cell `(i, j)` has sum <= `(i, j + 1)` and
//   <= `(i + 1, j)`. Every cell `(i, j)` can be assigned a unique parent in a
//   spanning tree rooted at `(0, 0)`:
//   - If `j > 0`, its unique parent is `(i, j - 1)` (expand right).
//   - If `j == 0` and `i > 0`, its unique parent is `(i - 1, 0)` (expand down
//     only along column 0).
//   1. Push `(nums1[0] + nums2[0], 0, 0)` onto the min-heap.
//   2. Repeatedly pop the minimum-sum entry `(sum, i, j)` up to `k` times,
//      recording `{nums1[i], nums2[j]}`:
//      - Push `(i, j + 1)` if `j + 1 < nums2.size()`.
//      - If `j == 0`, also push `(i + 1, 0)` if `i + 1 < nums1.size()`.
//   Because every coordinate `(i, j)` has exactly one parent, no index pair is
//   ever pushed twice, and the heap size never exceeds `min(k, m) + 1`.
//   Sums are accumulated in `std::int64_t` to avoid 32-bit overflow.
//   Time Complexity: O(k log(min(k, m))) — at most `k` pops and `2k` pushes on
//   a heap of size O(min(k, m)).
//   Space Complexity: O(min(k, m)) auxiliary space for the min-heap (excluding
//   the O(k) output list).
//
// Data Structures:
//   - std::priority_queue<PairEntry, ..., std::greater<>> (min-heap): extracts
//     the next globally smallest pair sum in O(log(min(k, m))) time while
//     lazily advancing row/column frontiers.
//
// Real-World Applications:
//   - Top-K join queries in relational databases (e.g., finding the `k`
//     cheapest flight + hotel combinations from pre-sorted price tables without
//     materializing the Cartesian product).
//   - Beam search / N-best hypothesis decoding in NLP and speech recognition
//     combining sorted score lists.
//   - Recommender system candidate generation over pre-ranked multi-source
//     item lists.
//
// Related LeetCode Problems:
//   - #378 Kth Smallest Element in a Sorted Matrix [done] (min-heap traversal over a row/column-sorted matrix)
//   - #719 Find K-th Smallest Pair Distance [new] (binary search + two pointers for k-th pair difference)
//   - #23 Merge k Sorted Lists [done] (k-way sorted merge using a min-heap)
//   - #313 Super Ugly Number [done] (lazy k-way min-heap expansion over generated candidates)
//   - #1439 Find the Kth Smallest Sum of a Matrix With Sorted Rows [done] (iteratively applies top-k pair sums row by row)

#include "framework/lc_framework.h"

#include <compare>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <queue>
#include <vector>

namespace {

class Solution {
 public:
  // Finds up to `k` pairs `(nums1[i], nums2[j])` with the smallest sums by
  // lazily expanding a unique-parent spanning tree of the sorted sum matrix.
  // Time: O(k log(min(k, m))), Space: O(min(k, m)) auxiliary.
  std::vector<std::vector<int>> kSmallestPairs(const std::vector<int>& nums1,
                                               const std::vector<int>& nums2,
                                               int k) {
    if (nums1.empty() || nums2.empty() || k <= 0) {
      return {};
    }

    struct PairEntry {
      std::int64_t sum = 0;
      std::size_t i = 0uz;
      std::size_t j = 0uz;

      auto operator<=>(const PairEntry&) const = default;
    };

    std::priority_queue<PairEntry, std::vector<PairEntry>, std::greater<>>
        min_heap;

    // Pushes valid matrix coordinate `(i, j)` onto the min-heap with 64-bit sum.
    const auto push = [&](std::size_t i, std::size_t j) {
      if (i < nums1.size() && j < nums2.size()) {
        const std::int64_t sum = static_cast<std::int64_t>(nums1[i]) +
                                 static_cast<std::int64_t>(nums2[j]);
        min_heap.push(PairEntry{.sum = sum, .i = i, .j = j});
      }
    };

    const auto limit = static_cast<std::size_t>(k);
    const std::size_t reserve_count =
        (nums1.size() > (limit / nums2.size())) ? limit
                                                : (nums1.size() * nums2.size());
    std::vector<std::vector<int>> result;
    result.reserve(reserve_count);

    // Root of the spanning tree is always `(0, 0)`.
    push(0uz, 0uz);

    while (!min_heap.empty() && result.size() < limit) {
      const PairEntry top = min_heap.top();
      min_heap.pop();

      result.push_back({nums1[top.i], nums2[top.j]});

      // Every node expands right along its row.
      push(top.i, top.j + 1uz);

      // Only column-0 nodes expand down to the next row, guaranteeing that
      // every cell `(i, j)` is enqueued at most once without a visited set.
      if (top.j == 0uz) {
        push(top.i + 1uz, 0uz);
      }
    }

    return result;
  }
};

LC_PROBLEM(373, "Find K Pairs With Smallest Sums", "heap", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    const std::vector<std::vector<int>> expected = {{1, 2}, {1, 4}, {1, 6}};
    ctx.expect_eq(sol.kSmallestPairs({1, 7, 11}, {2, 4, 6}, 3), expected);
  });

  ctx.test("example_2_duplicates", [&] {
    Solution sol;
    const std::vector<std::vector<int>> expected = {{1, 1}, {1, 1}};
    ctx.expect_eq(sol.kSmallestPairs({1, 1, 2}, {1, 2, 3}, 2), expected);
  });

  ctx.test("example_3_k_exceeds_total_pairs", [&] {
    Solution sol;
    const std::vector<std::vector<int>> expected = {{1, 3}, {2, 3}};
    ctx.expect_eq(sol.kSmallestPairs({1, 2}, {3}, 3), expected);
  });

  ctx.test("python_build_case_with_negatives", [&] {
    Solution sol;
    const std::vector<int> l1 = {1, 2, 3, 3, 3};
    const std::vector<int> l2 = {-3, 22, 35, 56, 70, 100, 123, 200};
    const std::vector<std::vector<int>> expected = {
        {1, -3}, {2, -3}, {3, -3}, {3, -3}, {3, -3},
        {1, 22}, {2, 22}, {3, 22}, {3, 22}, {3, 22},
    };
    ctx.expect_eq(sol.kSmallestPairs(l1, l2, 10), expected);
  });

  ctx.test("empty_and_zero_k", [&] {
    Solution sol;
    ctx.expect_eq(sol.kSmallestPairs({}, {1, 2}, 3),
                  std::vector<std::vector<int>>{});
    ctx.expect_eq(sol.kSmallestPairs({1, 2}, {}, 3),
                  std::vector<std::vector<int>>{});
    ctx.expect_eq(sol.kSmallestPairs({1, 2}, {3, 4}, 0),
                  std::vector<std::vector<int>>{});
  });
}

}  // namespace
