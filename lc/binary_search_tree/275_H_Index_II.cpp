// LeetCode 275: H Index II
// https://leetcode.com/problems/h-index-ii/
//
// Problem:
//   Given an array `citations` of non-negative integers sorted in ascending
//   order where `citations[i]` is the citation count of a researcher's i-th
//   paper, compute the researcher's h-index in logarithmic time.
//   The h-index is the maximum value `h` such that the researcher has at least
//   `h` papers that have each been cited at least `h` times.
//
// Algorithm:
//   Binary search for the leftmost index `i` in `[0, n]` such that the suffix
//   `citations[i .. n - 1]` of length `h = n - i` satisfies
//   `citations[i] >= n - i`:
//   - Because `citations` is sorted ascending, `citations[i]` is monotonic
//     non-decreasing while `n - i` is strictly decreasing; thus the predicate
//     `citations[i] >= n - i` is monotonic (false prefix followed by true
//     suffix).
//   - Using a half-open search range `[left, right)` initialized to `[0, n)`,
//     when `citations[mid] >= n - mid`, `mid` is feasible and we shrink `right`
//     to `mid` to see if an even larger `h` (smaller index) is feasible;
//     otherwise we advance `left = mid + 1`.
//   - Upon termination, `left` is the smallest valid index, and the h-index is
//     `n - left`.
//   - Time Complexity:  O(log n), where n = citations.size().
//   - Space Complexity: O(1).
//
// Data Structures:
//   - Contiguous sorted array (`std::vector<int>`) accessed via random-access
//     binary search indices (`left`, `right`, `mid`).
//
// Real-World Applications:
//   - Academic bibliometrics engines (Google Scholar, Semantic Scholar, OpenAlex)
//     computing researcher and journal impact metrics on pre-sorted citation
//     lists.
//   - Service-level objective (SLO) threshold detection (finding the largest k
//     such that at least k instances exceed latency or throughput threshold k).
//   - Partitioning sorted telemetry histograms at fixed-point rank thresholds.
//
// Related LeetCode Problems:
//   - #274 H-Index [done] (unsorted variant solved via counting sort in O(n))
//   - #704 Binary Search [done] (fundamental binary search template)
//   - #35 Search Insert Position [done] (lower-bound binary search on a sorted array)
//   - #278 First Bad Version [done] (binary search on a monotonic boolean predicate)

#include "framework/lc_framework.h"

#include <cstddef>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the researcher's h-index from a non-decreasing `citations` array
  // via lower-bound binary search over the monotonic predicate
  // `citations[i] >= n - i`.
  // Time: O(log n), Space: O(1).
  int hIndex(const std::vector<int>& citations) {
    const std::size_t n = citations.size();
    // Search half-open interval [left, right) to avoid unsigned underflow when
    // the optimal split index is 0.
    std::size_t left = 0uz;
    std::size_t right = n;

    while (left < right) {
      const std::size_t mid = left + ((right - left) / 2uz);
      // Number of papers from `mid` through `n - 1` (all have >= citations[mid]
      // citations because the array is sorted ascending).
      const std::size_t papers_at_or_above = n - mid;

      if (std::cmp_greater_equal(citations[mid], papers_at_or_above)) {
        // `mid` satisfies the h-index condition; search left for a larger h.
        right = mid;
      } else {
        // Too few citations at `mid`; discard `mid` and everything left of it.
        left = mid + 1uz;
      }
    }

    // `left` is the first index where citations[left] >= n - left (or `n` if
    // no paper qualifies, yielding h = 0).
    return static_cast<int>(n - left);
  }
};

LC_PROBLEM(275, "H Index II", "binary_search_tree", ctx) {
  Solution sol;

  ctx.test("example_1_standard_sorted",
           [&] { ctx.expect_eq(sol.hIndex({0, 1, 3, 5, 6}), 3); });

  ctx.test("example_2_three_papers",
           [&] { ctx.expect_eq(sol.hIndex({1, 2, 100}), 2); });

  ctx.test("python_build_inputs", [&] {
    ctx.expect_eq(sol.hIndex({1, 2}), 1);
    ctx.expect_eq(sol.hIndex({1}), 1);
    ctx.expect_eq(sol.hIndex({0}), 0);
    ctx.expect_eq(sol.hIndex({100}), 1);
  });

  ctx.test("all_zeros_and_all_high", [&] {
    ctx.expect_eq(sol.hIndex({0, 0, 0, 0}), 0);
    ctx.expect_eq(sol.hIndex({5, 5, 5, 5}), 4);
    ctx.expect_eq(sol.hIndex({4, 4, 4, 4}), 4);
    ctx.expect_eq(sol.hIndex({3, 3, 3, 3}), 3);
  });

  ctx.test("empty_citations", [&] { ctx.expect_eq(sol.hIndex({}), 0); });
}

}  // namespace
