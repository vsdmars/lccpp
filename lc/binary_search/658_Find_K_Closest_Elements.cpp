// LeetCode 658: Find K Closest Elements
// https://leetcode.com/problems/find-k-closest-elements/
//
// Problem:
//   Given a sorted integer array `arr` and two integers `k` and `x`, return the
//   `k` closest integers to `x` in `arr`, sorted in ascending order. An integer
//   `a` is closer to `x` than `b` if `|a - x| < |b - x|`, or `|a - x| == |b - x|`
//   and `a < b` (ties always prefer the smaller element).
//   Constraints: 1 <= k <= arr.length <= 10^4, -10^4 <= arr[i], x <= 10^4.
//
// Algorithm:
//   Because `arr` is sorted, the `k` closest elements must form a contiguous
//   subarray `arr[lhs .. lhs + k - 1]`. Instead of searching for `x` and
//   expanding outward, binary-search directly on the window's left endpoint
//   `lhs` over `[0, n - k]`.
//   At midpoint `mid`, compare the candidate window starting at `mid` against
//   the window shifted right by one (which drops `arr[mid]` and adds
//   `arr[mid + k]`). Using signed differences `x - arr[mid] > arr[mid + k] - x`
//   (rather than `std::abs`) handles duplicate elements cleanly and naturally
//   breaks ties in favor of the smaller (left) window:
//     - If `x - arr[mid] > arr[mid + k] - x`, `arr[mid + k]` is strictly better
//       than `arr[mid]`, so the optimal start must be `>= mid + 1`.
//     - Otherwise, `mid` is at least as good as `mid + 1`, so search `[lhs, mid]`.
//   Time Complexity: O(log(n - k) + k) — binary search plus copying `k` items.
//   Space Complexity: O(1) auxiliary space (excluding the returned `k`-element
//   output vector).
//   (A secondary O(n + k log n) min-heap baseline from the Python source is
//   also included for cross-verification.)
//
// Data Structures:
//   - Direct index range `[lhs, rhs]` over the sorted input `std::vector<int>`
//     for the primary binary-search window algorithm.
//   - `std::priority_queue` min-heap of `(abs_diff, value)` pairs for the
//     secondary heap-based reference implementation.
//
// Real-World Applications:
//   - 1D k-nearest-neighbor (k-NN) lookups on sorted time-series timestamps or
//     indexed scalar features.
//   - Order-book depth queries finding the `k` price levels closest to the
//     current mid-market price.
//   - Audio/sensor windowing around a target event timestamp in sorted logs.
//
// Related LeetCode Problems:
//   - #34 Find First and Last Position of Element in Sorted Array [done] (binary search on sorted boundaries)
//   - #270 Closest Binary Search Tree Value [done] (single closest value in ordered structure)
//   - #272 Closest Binary Search Tree Value II [done] (k closest values in a BST)
//   - #704 Binary Search [done] (standard lower-bound binary search)
//   - #744 Find Smallest Letter Greater Than Target [done] (upper-bound binary search on sorted array)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <queue>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the `k` elements in sorted `arr` closest to `x` via binary search on
  // the starting index of the contiguous length-`k` window.
  // Time: O(log(n - k) + k), Space: O(1) auxiliary.
  std::vector<int> findClosestElements(const std::vector<int>& arr, int k,
                                       int x) {
    if (k <= 0 || arr.empty()) {
      return {};
    }
    const auto window_size = static_cast<std::size_t>(k);
    if (window_size >= arr.size()) {
      return arr;
    }

    // Valid window start indices lie in the closed range [0, arr.size() - k].
    // Invariant: the optimal left boundary always lies within [lhs, rhs].
    std::size_t lhs = 0uz;
    std::size_t rhs = arr.size() - window_size;
    while (lhs < rhs) {
      const std::size_t mid = lhs + ((rhs - lhs) / 2uz);

      // Compare signed distances (using 64-bit arithmetic to avoid overflow):
      // `x - arr[mid] > arr[mid + k] - x` is equivalent to placing `x` strictly
      // to the right of the midpoint between `arr[mid]` and `arr[mid + k]`.
      // Unlike `std::abs`, this correctly handles plateaus of duplicate values
      // (e.g., [1, 2, 3, 7, 7, 7, 7, 7]) and breaks ties toward the left.
      const std::int64_t left_diff =
          static_cast<std::int64_t>(x) - static_cast<std::int64_t>(arr[mid]);
      const std::int64_t right_diff =
          static_cast<std::int64_t>(arr[mid + window_size]) -
          static_cast<std::int64_t>(x);
      if (left_diff > right_diff) {
        lhs = mid + 1uz;
      } else {
        rhs = mid;
      }
    }

    const auto start_it = arr.begin() + static_cast<std::ptrdiff_t>(lhs);
    const auto end_it = start_it + static_cast<std::ptrdiff_t>(window_size);
    return {start_it, end_it};
  }

  // Alternative min-heap approach from the Python source: ranks all elements by
  // `(abs(a - x), a)`, pops the `k` smallest pairs, and sorts the result.
  // Time: O(n + k log n), Space: O(n).
  std::vector<int> findClosestElementsHeap(const std::vector<int>& arr, int k,
                                           int x) {
    if (k <= 0 || arr.empty()) {
      return {};
    }
    using Entry = std::pair<std::int64_t, int>;
    std::vector<Entry> entries;
    entries.reserve(arr.size());
    for (const int val : arr) {
      const std::int64_t dist = std::abs(static_cast<std::int64_t>(val) -
                                         static_cast<std::int64_t>(x));
      entries.emplace_back(dist, val);
    }

    std::priority_queue<Entry, std::vector<Entry>, std::greater<>> min_heap(
        std::greater<>{}, std::move(entries));

    std::vector<int> result;
    const auto limit = std::min(static_cast<std::size_t>(k), arr.size());
    result.reserve(limit);
    for (std::size_t i = 0uz; i < limit; ++i) {
      result.push_back(min_heap.top().second);
      min_heap.pop();
    }
    std::ranges::sort(result);
    return result;
  }
};

LC_PROBLEM(658, "Find K Closest Elements", "binary_search", ctx) {
  auto verify_both = [&](const std::vector<int>& arr, int k, int x,
                         const std::vector<int>& expected) {
    Solution sol;
    ctx.expect_eq(sol.findClosestElements(arr, k, x), expected);
    ctx.expect_eq(sol.findClosestElementsHeap(arr, k, x), expected);
  };

  ctx.test("example_1_centered_tie_prefers_smaller", [&] {
    verify_both({1, 2, 3, 4, 5}, 4, 3, {1, 2, 3, 4});
  });

  ctx.test("example_2_target_below_min", [&] {
    verify_both({1, 2, 3, 4, 5}, 4, -1, {1, 2, 3, 4});
  });

  ctx.test("target_above_max", [&] {
    verify_both({1, 2, 3, 4, 5}, 3, 10, {3, 4, 5});
  });

  ctx.test("python_build_duplicates", [&] {
    verify_both({1, 1, 2, 2, 2, 2, 2, 3, 3}, 3, 3, {2, 3, 3});
  });

  ctx.test("python_build_negative_outlier", [&] {
    verify_both({-100, 1, 2, 3, 4, 5}, 2, 0, {1, 2});
  });

  ctx.test("duplicate_plateau_signed_diff_case", [&] {
    verify_both({1, 2, 3, 7, 7, 7, 7, 7, 7}, 4, 3, {1, 2, 3, 7});
    verify_both({0, 0, 1, 2, 3, 3, 4, 7, 7, 8}, 3, 5, {3, 3, 4});
  });

  ctx.test("full_window_and_single_element", [&] {
    verify_both({10, 20, 30}, 3, 15, {10, 20, 30});
    verify_both({42}, 1, 0, {42});
  });
}

}  // namespace
