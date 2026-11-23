// LeetCode 4: Median Of Two Sorted Arrays
// https://leetcode.com/problems/median-of-two-sorted-arrays/
//
// Problem:
//   Given two sorted integer arrays `nums1` and `nums2` of sizes `m` and `n`
//   respectively, return the median of the combined sorted sequence of length
//   `m + n`. If `m + n` is odd, the median is the middle element; if even, it
//   is the arithmetic mean of the two middle elements. The required optimal
//   runtime complexity is O(log(m + n)).
//
// Examples:
//   Example 1:
//     Input:  nums1 = [1, 3], nums2 = [2]
//     Output: 2.0
//     Explanation:
//       - Assign shorter array a = [2] (m = 1) and longer array b = [1, 3] (n = 2),
//         giving total = 3 and half_len = 2.
//       - Start with lo = 0, hi = 1; at i = 0, j = 2, check b[1] = 3 > a[0] = 2
//         succeeds, advancing lo = 1.
//       - At i = 1, j = 1, both partition inequalities pass with left_a = a[0] = 2
//         and left_b = b[0] = 1.
//       - Compute max_left = max(2, 1) = 2 and return 2.0 because total = 3 is odd.
//
//   Example 2:
//     Input:  nums1 = [1, 2], nums2 = [3, 4]
//     Output: 2.5
//     Explanation:
//       - Set a = [1, 2] (m = 2), b = [3, 4] (n = 2), total = 4, and half_len = 2.
//       - At lo = 0, hi = 2 (i = 1, j = 1), b[0] = 3 > a[1] = 2 holds, advancing lo
//         = 2.
//       - At lo = 2, hi = 2 (i = 2, j = 0), the partition is valid with max_left =
//         a[1] = 2 and min_right = b[0] = 3.
//       - Since total = 4 is even, return (2.0 + 3.0) / 2.0 = 2.5.
//
//   Example 3:
//     Input:  nums1 = [], nums2 = [10, 20, 30]
//     Output: 20.0
//     Explanation:
//       - Assign shorter array a = [] (m = 0) and b = [10, 20, 30] (n = 3), giving
//         total = 3 and half_len = 2.
//       - With lo = 0, hi = 0, the sole candidate i = 0, j = 2 satisfies the
//         partition checks immediately.
//       - Compute max_left = max(INT_MIN, b[1] = 20) = 20 and return 20.0 because
//         total = 3 is odd.
//
// Algorithm:
//   Primary (Binary Search on Partition):
//     Ensure `a` is the shorter array (`m <= n`) so the search space is
//     bounded by `min(m, n)` and the complementary index `j` is always in
//     bounds. Partition `a` into `a[0..i)` / `a[i..m)` and `b` into `b[0..j)`
//     / `b[j..n)` such that `i + j = (m + n + 1) / 2` (so the combined left
//     half holds half the elements, or one extra when total length is odd).
//     Binary search `i` in `[0, m]` until every left element is <= every right
//     element: `a[i - 1] <= b[j]` and `b[j - 1] <= a[i]`.
//     - If `b[j - 1] > a[i]`, `i` is too small -> advance `lo = i + 1`.
//     - If `a[i - 1] > b[j]`, `i` is too large -> shrink `hi = i - 1`.
//     Once valid, the median is `max(left)` when `m + n` is odd, or
//     `(max(left) + min(right)) / 2.0` when `m + n` is even.
//     Time Complexity: O(log(min(m, n))).
//     Space Complexity: O(1).
//
//   Secondary (Min-Heap 2-Way Merge):
//     Preserves the Python reference's `heapq.merge` approach: seed a min-heap
//     with the heads of both sorted arrays and advance `(m + n) / 2 + 1` steps
//     to read the middle one or two elements without materializing the merged
//     array.
//     Time Complexity: O(m + n), Space Complexity: O(1).
//
// Data Structures:
//   - Partition indices (`lo`, `hi`, `i`, `j`) over read-only array slices
//     for the O(log(min(m, n))) solution.
//   - std::priority_queue (min-heap of at most 2 stream cursors) for the
//     alternative heap-merge method.
//
// Real-World Applications:
//   - Distributed quantile / percentile (p50) estimation across pre-sorted
//     shards in time-series databases and telemetry aggregators without
//     transferring or merging full datasets.
//   - Parallel sort-merge join partitioning in distributed SQL engines (e.g.,
//     finding splitters across sorted partitions in Spark/BigQuery).
//   - Order-statistic queries over multi-tier LSM-tree SSTable runs.
//
// Related LeetCode Problems:
//   - #295 Find Median From Data Stream [done] (maintains a running median over an unsorted online stream with two heaps)
//   - #215 Kth Largest Element In An Array [done] (order-statistic selection in an unsorted array)
//   - #23 Merge k Sorted Lists [done] (k-way sorted merge using a min-heap)
//   - #378 Kth Smallest Element in a Sorted Matrix [done] (binary search on value/order statistics over sorted structures)
//   - #480 Sliding Window Median [done] (maintains medians over fixed-size sliding windows)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cmath>
#include <compare>
#include <cstddef>
#include <functional>
#include <limits>
#include <queue>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the median of two sorted arrays in O(log(min(m, n))) time and O(1)
  // auxiliary space by binary-searching the cut point on the shorter array.
  double findMedianSortedArrays(const std::vector<int>& nums1,
                                const std::vector<int>& nums2) {
    // Always binary-search on the shorter array so `j = half_len - i` is
    // guaranteed to stay within [0, n].
    const std::vector<int>& a =
        (nums1.size() <= nums2.size()) ? nums1 : nums2;
    const std::vector<int>& b =
        (nums1.size() <= nums2.size()) ? nums2 : nums1;

    const std::size_t m = a.size();
    const std::size_t n = b.size();
    const std::size_t total = m + n;
    if (total == 0uz) {
      return 0.0;
    }

    // Total elements placed on the left side of the partition.
    // When `total` is odd, the left partition holds the extra middle element.
    const std::size_t half_len = (total + 1uz) / 2uz;
    std::size_t lo = 0uz;
    std::size_t hi = m;

    while (lo <= hi) {
      const std::size_t i = lo + ((hi - lo) / 2uz);
      const std::size_t j = half_len - i;

      if (i < m && b[j - 1uz] > a[i]) {
        // Cut in `a` is too far left: `a[i]` belongs on the left side.
        lo = i + 1uz;
        continue;
      }
      if (i > 0uz && a[i - 1uz] > b[j]) {
        // Cut in `a` is too far right: `a[i - 1]` belongs on the right side.
        hi = i - 1uz;
        continue;
      }

      // Valid partition found: max(left) <= min(right).
      const int left_a =
          (i == 0uz) ? std::numeric_limits<int>::min() : a[i - 1uz];
      const int left_b =
          (j == 0uz) ? std::numeric_limits<int>::min() : b[j - 1uz];
      const int max_left = std::max(left_a, left_b);

      if ((total % 2uz) == 1uz) {
        return static_cast<double>(max_left);
      }

      const int right_a =
          (i == m) ? std::numeric_limits<int>::max() : a[i];
      const int right_b =
          (j == n) ? std::numeric_limits<int>::max() : b[j];
      const int min_right = std::min(right_a, right_b);

      // Cast before adding to prevent 32-bit signed integer overflow.
      return (static_cast<double>(max_left) + static_cast<double>(min_right)) /
             2.0;
    }

    return 0.0;
  }

  // Alternative O(m + n) min-heap 2-way merge (matching the Python `heapq.merge`
  // approach) that streams elements in sorted order up to the midpoint.
  double findMedianViaHeapMerge(const std::vector<int>& nums1,
                                const std::vector<int>& nums2) {
    const std::size_t total = nums1.size() + nums2.size();
    if (total == 0uz) {
      return 0.0;
    }

    struct Cursor {
      int value = 0;
      int array_id = 0;
      std::size_t index = 0uz;

      auto operator<=>(const Cursor&) const = default;
    };

    std::priority_queue<Cursor, std::vector<Cursor>, std::greater<>> min_heap;
    if (!nums1.empty()) {
      min_heap.push(Cursor{.value = nums1[0], .array_id = 0, .index = 0uz});
    }
    if (!nums2.empty()) {
      min_heap.push(Cursor{.value = nums2[0], .array_id = 1, .index = 0uz});
    }

    const std::size_t steps = (total / 2uz) + 1uz;
    int prev = 0;
    int curr = 0;

    for (std::size_t step = 0uz; step < steps; ++step) {
      const Cursor top = min_heap.top();
      min_heap.pop();
      prev = curr;
      curr = top.value;

      const std::vector<int>& src = (top.array_id == 0) ? nums1 : nums2;
      const std::size_t next_idx = top.index + 1uz;
      if (next_idx < src.size()) {
        min_heap.push(Cursor{
            .value = src[next_idx],
            .array_id = top.array_id,
            .index = next_idx,
        });
      }
    }

    if ((total % 2uz) == 1uz) {
      return static_cast<double>(curr);
    }
    return (static_cast<double>(prev) + static_cast<double>(curr)) / 2.0;
  }
};

constexpr double kEps = 1e-5;

bool approx_eq(double actual, double expected) {
  return std::fabs(actual - expected) <= kEps;
}

LC_PROBLEM(4, "Median Of Two Sorted Arrays", "heap", ctx) {
  ctx.test("example_1_odd_total", [&] {
    Solution sol;
    const std::vector<int> a = {1, 3};
    const std::vector<int> b = {2};
    ctx.expect_true(approx_eq(sol.findMedianSortedArrays(a, b), 2.0));
    ctx.expect_true(approx_eq(sol.findMedianViaHeapMerge(a, b), 2.0));
  });

  ctx.test("example_2_even_total", [&] {
    Solution sol;
    const std::vector<int> a = {1, 2};
    const std::vector<int> b = {3, 4};
    ctx.expect_true(approx_eq(sol.findMedianSortedArrays(a, b), 2.5));
    ctx.expect_true(approx_eq(sol.findMedianViaHeapMerge(a, b), 2.5));
  });

  ctx.test("python_build_cases", [&] {
    Solution sol;
    const std::vector<int> a1 = {1, 2, 9};
    const std::vector<int> b1 = {3, 4, 5};
    ctx.expect_true(approx_eq(sol.findMedianSortedArrays(a1, b1), 3.5));
    ctx.expect_true(approx_eq(sol.findMedianViaHeapMerge(a1, b1), 3.5));

    const std::vector<int> a2 = {4, 5};
    const std::vector<int> b2 = {1, 2, 3};
    ctx.expect_true(approx_eq(sol.findMedianSortedArrays(a2, b2), 3.0));
    ctx.expect_true(approx_eq(sol.findMedianViaHeapMerge(a2, b2), 3.0));
  });

  ctx.test("one_array_empty", [&] {
    Solution sol;
    const std::vector<int> empty;
    const std::vector<int> odd = {10, 20, 30};
    const std::vector<int> even = {10, 20, 30, 40};

    ctx.expect_true(approx_eq(sol.findMedianSortedArrays(empty, odd), 20.0));
    ctx.expect_true(approx_eq(sol.findMedianViaHeapMerge(empty, odd), 20.0));
    ctx.expect_true(approx_eq(sol.findMedianSortedArrays(even, empty), 25.0));
    ctx.expect_true(approx_eq(sol.findMedianViaHeapMerge(even, empty), 25.0));
  });

  ctx.test("negatives_duplicates_and_int_extremes", [&] {
    Solution sol;
    const std::vector<int> a = {-5, -3, -3, 0};
    const std::vector<int> b = {-3, 1, 2};
    ctx.expect_true(approx_eq(sol.findMedianSortedArrays(a, b), -3.0));
    ctx.expect_true(approx_eq(sol.findMedianViaHeapMerge(a, b), -3.0));

    const int max_v = std::numeric_limits<int>::max();
    const std::vector<int> hi1 = {max_v};
    const std::vector<int> hi2 = {max_v};
    ctx.expect_true(approx_eq(sol.findMedianSortedArrays(hi1, hi2),
                              static_cast<double>(max_v)));
    ctx.expect_true(approx_eq(sol.findMedianViaHeapMerge(hi1, hi2),
                              static_cast<double>(max_v)));
  });
}

}  // namespace
