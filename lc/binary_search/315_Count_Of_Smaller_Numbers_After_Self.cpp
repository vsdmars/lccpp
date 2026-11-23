// LeetCode 315: Count Of Smaller Numbers After Self
// https://leetcode.com/problems/count-of-smaller-numbers-after-self/
//
// Problem:
//   Given an integer array nums of length n, return an integer array counts of
//   the same length where counts[i] is the number of elements strictly smaller
//   than nums[i] that appear to the right of index i (i.e., j > i with
//   nums[j] < nums[i]).
//
// Examples:
//   Example 1:
//     Input:  nums = [5, 2, 6, 1]
//     Output: [2, 1, 1, 0]
//     Explanation:
//       - Pair each value with its original index: [(5, 0), (2, 1), (6, 2), (1,
//         3)], and split at mid = 2.
//       - Merge left halves [(5, 0)] and [(2, 1)]: (2, 1) emits first
//         (right_smaller = 1), so (5, 0) adds 1 to counts[0], yielding [(2, 1), (5,
//         0)].
//       - Merge right halves [(6, 2)] and [(1, 3)]: (1, 3) emits first
//         (right_smaller = 1), so (6, 2) adds 1 to counts[2], yielding [(1, 3), (6,
//         2)].
//       - Merge [(2, 1), (5, 0)] and [(1, 3), (6, 2)]: (1, 3) emits first
//         (right_smaller = 1), then (2, 1) and (5, 0) each add 1 to counts[1] and
//         counts[0] before (6, 2) emits, returning [2, 1, 1, 0].
//
//   Example 2:
//     Input:  nums = [2, 0, 1, 2, 0]
//     Output: [3, 0, 1, 1, 0]
//     Explanation:
//       - Split [0, 5) at mid = 2: sorting [0, 2) pairs [(0, 1), (2, 0)] with
//         counts[0] = 1, and sorting [2, 5) produces [(0, 4), (1, 2), (2, 3)] with
//         counts[2] = 1 and counts[3] = 1.
//       - In the top-level merge, compare (0, 4) vs (0, 1): equality takes the left
//         element (0, 1) first so equal right values are not counted, adding 0 to
//         counts[1].
//       - Next, (0, 4) < (2, 0) and (1, 2) < (2, 0) emit from the right,
//         incrementing right_smaller to 2.
//       - Compare (2, 3) vs (2, 0): equality again takes the left element (2, 0)
//         first, adding right_smaller = 2 to counts[0] (1 + 2 = 3) and returning
//         [3, 0, 1, 1, 0].
//
//   Example 3:
//     Input:  nums = [-1, -1]
//     Output: [0, 0]
//     Explanation:
//       - Pair elements with indices as [(-1, 0), (-1, 1)] and split at mid = 1
//         into [(-1, 0)] and [(-1, 1)].
//       - During mergeRanges, items[right_idx].value < items[left_idx].value (-1 <
//         -1) is false, so the left item (-1, 0) is emitted first while
//         right_smaller = 0.
//       - The remaining right item (-1, 1) is emitted without incrementing any
//         counts, returning [0, 0].
//
// Algorithm:
//   Divide-and-conquer Merge Sort with Original Indices:
//   Pair every element with its original index `(value, index)` and perform a
//   stable ascending merge sort over [lo, hi) using a single preallocated
//   scratch buffer:
//     - Recursively sort the left half [lo, mid) and right half [mid, hi).
//       Every element in the right half originally appeared to the right of
//       every element in the left half.
//     - While merging the two sorted halves, maintain `right_smaller`, the
//       count of right-half elements already placed into the merged output.
//     - Whenever `right[j].value < left[i].value`, place `right[j]` next and
//       increment `right_smaller`.
//     - Otherwise (`left[i].value <= right[j].value`), all `right_smaller`
//       right-half elements placed so far are strictly smaller than `left[i]`
//       (and every remaining right-half element is >= `left[i]`), so add
//       `right_smaller` to `counts[left[i].index]` and place `left[i]`.
//     - Any leftover left-half elements are strictly larger than all elements
//       that were taken from the right half, so each receives `right_smaller`.
//   Time: O(n log n), Space: O(n).
//
// Data Structures:
//   - `std::vector<IndexedValue> items`: pairs each value with its original
//     index so counts can be accumulated in place during sorting.
//   - `std::vector<IndexedValue> buffer`: reusable merge scratch buffer of
//     size n to avoid O(n log n) heap allocations.
//   - `std::vector<int> counts`: output array accumulating per-index inversion
//     counts.
//
// Real-World Applications:
//   - Computing Kendall tau rank correlation and inversion metrics between two
//     rankings (search engine result evaluation, recommendation systems).
//   - Order-book / time-series dominance queries counting future drops below
//     the current price level.
//   - 2D dominance counting in computational geometry and database skyline
//     queries.
//
// Related LeetCode Problems:
//   - #493 Reverse Pairs [new] (merge-sort counting with nums[i] > 2 * nums[j])
//   - #327 Count of Range Sum [new] (merge-sort counting on prefix sums in [lower, upper])
//   - #1649 Create Sorted Array through Instructions [new] (Fenwick tree / merge-sort smaller & larger counts)
//   - #2426 Number of Pairs Satisfying Inequality [new] (rearranged difference inversion count via merge sort)
//   - #300 Longest Increasing Subsequence [done] (order-statistic / binary-search sequence analysis)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Computes for each index i the number of j > i with nums[j] < nums[i].
  // Time: O(n log n), Space: O(n).
  std::vector<int> countSmaller(const std::vector<int>& nums) {
    const std::size_t n = nums.size();
    std::vector<int> counts(n, 0);
    if (n <= 1uz) {
      return counts;
    }

    // Attach original indices so each element's answer can be updated even as
    // merge sort rearranges positions.
    std::vector<IndexedValue> items;
    items.reserve(n);
    for (std::size_t i = 0uz; i < n; ++i) {
      items.push_back(IndexedValue{.value = nums[i], .index = i});
    }

    // Single reusable scratch buffer shared across all merge calls.
    std::vector<IndexedValue> buffer(n);
    mergeSort(items, buffer, counts, 0uz, n);
    return counts;
  }

 private:
  struct IndexedValue {
    int value = 0;
    std::size_t index = 0uz;
  };

  // Recursively sorts items[lo..hi) in ascending order by value while
  // accumulating cross-half smaller-element counts into `counts`.
  static void mergeSort(std::vector<IndexedValue>& items,
                        std::vector<IndexedValue>& buffer,
                        std::vector<int>& counts, std::size_t lo,
                        std::size_t hi) {
    if (hi - lo <= 1uz) {
      return;
    }

    const std::size_t mid = lo + ((hi - lo) / 2uz);
    mergeSort(items, buffer, counts, lo, mid);
    mergeSort(items, buffer, counts, mid, hi);
    mergeRanges(items, buffer, counts, lo, mid, hi);
  }

  // Merges sorted halves items[lo..mid) and items[mid..hi).
  //
  // Invariant: `right_smaller` equals the exact number of elements from
  // [mid, hi) already emitted into `buffer`, every one of which is strictly
  // smaller than items[left_idx] (and all remaining elements in [lo, mid)).
  static void mergeRanges(std::vector<IndexedValue>& items,
                          std::vector<IndexedValue>& buffer,
                          std::vector<int>& counts, std::size_t lo,
                          std::size_t mid, std::size_t hi) {
    std::size_t left_idx = lo;
    std::size_t right_idx = mid;
    std::size_t out_idx = lo;
    int right_smaller = 0;

    while (left_idx < mid && right_idx < hi) {
      if (items[right_idx].value < items[left_idx].value) {
        // Right element is strictly smaller than left_idx (and all subsequent
        // left elements); emit it and bump the counter.
        buffer[out_idx++] = items[right_idx++];
        ++right_smaller;
      } else {
        // Equality takes the left element first so equal elements on the right
        // are NOT counted as strictly smaller.
        counts[items[left_idx].index] += right_smaller;
        buffer[out_idx++] = items[left_idx++];
      }
    }

    // Remaining left elements are larger than all right elements already placed.
    while (left_idx < mid) {
      counts[items[left_idx].index] += right_smaller;
      buffer[out_idx++] = items[left_idx++];
    }

    // Remaining right elements have no remaining left elements to affect.
    while (right_idx < hi) {
      buffer[out_idx++] = items[right_idx++];
    }

    // Copy the merged sorted slice back into `items`.
    for (std::size_t idx = lo; idx < hi; ++idx) {
      items[idx] = buffer[idx];
    }
  }
};

LC_PROBLEM(315, "Count Of Smaller Numbers After Self", "binary_search", ctx) {
  ctx.test("docstring_example", [&] {
    Solution sol;
    ctx.expect_eq(sol.countSmaller({5, 2, 6, 1}), std::vector<int>({2, 1, 1, 0}));
  });

  ctx.test("duplicates_and_negatives", [&] {
    Solution sol;
    ctx.expect_eq(sol.countSmaller({-1, -1}), std::vector<int>({0, 0}));
    ctx.expect_eq(sol.countSmaller({2, 0, 1, 2, 0}),
                  std::vector<int>({3, 0, 1, 1, 0}));
  });

  ctx.test("empty_single_and_monotonic", [&] {
    Solution sol;
    ctx.expect_eq(sol.countSmaller({}), std::vector<int>({}));
    ctx.expect_eq(sol.countSmaller({-1}), std::vector<int>({0}));
    ctx.expect_eq(sol.countSmaller({1, 2, 3, 4, 5}),
                  std::vector<int>({0, 0, 0, 0, 0}));
    ctx.expect_eq(sol.countSmaller({5, 4, 3, 2, 1}),
                  std::vector<int>({4, 3, 2, 1, 0}));
  });
}

}  // namespace
