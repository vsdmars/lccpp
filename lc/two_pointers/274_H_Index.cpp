// LeetCode 274: H Index
// https://leetcode.com/problems/h-index/
//
// Problem:
//   Given an integer array `citations` where citations[i] is the number of
//   citations a researcher received for their i-th paper, return the
//   researcher's h-index. By definition, the h-index is the maximum value h
//   such that the researcher has published at least h papers that have each
//   been cited at least h times.
//
// Examples:
//   Example 1:
//     Input:  citations = [3, 0, 6, 1, 5]
//     Output: 3
//     Explanation:
//       - Sort citations in descending order to obtain [6, 5, 3, 1, 0].
//       - At h = 0, citations[0] = 6 >= 1, so advance h to 1; at h = 1,
//         citations[1] = 5 >= 2, so advance h to 2.
//       - At h = 2, citations[2] = 3 >= 3, so advance h to 3.
//       - At h = 3, citations[3] = 1 < 4, so the scan stops and returns h = 3.
//
//   Example 2:
//     Input:  citations = [1, 3, 1]
//     Output: 1
//     Explanation:
//       - Sort citations in descending order to obtain [3, 1, 1].
//       - At h = 0, citations[0] = 3 >= 1, so advance h to 1.
//       - At h = 1, citations[1] = 1 < 2, so the loop exits and returns h = 1.
//
//   Example 3:
//     Input:  citations = [100]
//     Output: 1
//     Explanation:
//       - With n = 1, hIndex keeps [100] sorted descending and sees citations[0] =
//         100 >= 1 at h = 0, advancing h to 1.
//       - Now h = 1 equals citations.size(), so the loop ends and returns 1 (and
//         hIndexCounting similarly clamps 100 into buckets[1] = 1 and returns h =
//         1).
//
// Algorithm:
//   1. Descending Sort + Linear Scan (`hIndex`):
//      Sort `citations` in descending order. After sorting, the first (i + 1)
//      papers all have at least citations[i] citations. Scan from index 0 and
//      advance `h` while citations[h] >= h + 1; the first index where this
//      fails (or n if all succeed) is the maximum valid h-index.
//      Time complexity: O(n log n). Space complexity: O(1) auxiliary.
//   2. Counting / Bucket Sort (`hIndexCounting`):
//      Because h cannot exceed n = citations.size(), clamp every citation count
//      greater than n into bucket n. Traverse buckets from n down to 1 while
//      accumulating the suffix count of papers with at least h citations; the
//      first h where papers >= h is the h-index.
//      Time complexity: O(n). Space complexity: O(n).
//
// Data Structures:
//   - std::vector<int> sorted descending: exposes the monotonic threshold
//     condition citations[h] >= h + 1 directly by rank.
//   - std::vector<std::size_t> frequency buckets (in `hIndexCounting`): maps
//     clamped citation counts in [0, n] to paper counts for linear-time lookup.
//
// Real-World Applications:
//   - Academic bibliometrics (Google Scholar, Scopus, Web of Science) measuring
//     both publication productivity and citation impact in a single robust score.
//   - Content platform creator impact metrics (e.g., h videos with at least h
//     thousand views) resistant to single viral outliers.
//   - Open-source maintainer and package ecosystem reputation scoring.
//
// Related LeetCode Problems:
//   - #275 H-Index II [done] (input already sorted ascending; solved in O(log n))
//   - #1608 Special Array With X Elements Greater Than or Equal X [new] (same rank-threshold structure)
//   - #215 Kth Largest Element in an Array [done] (order-statistic / selection threshold)
//   - #347 Top K Frequent Elements [done] (uses the same O(n) bucket-count trick)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the h-index by sorting citations descending and finding the
  // largest prefix length `h` whose minimum element is at least `h`.
  //
  // Invariant: after `h` iterations, the first `h` papers each have >= `h`
  // citations, and all remaining papers have <= citations[h - 1] citations.
  //
  // Time: O(n log n), Space: O(1) auxiliary (O(log n) sort stack).
  int hIndex(std::vector<int>& citations) {
    // Sort descending so rank `h + 1` (0-based index `h`) gives the h-th
    // largest citation count directly.
    std::ranges::sort(citations, std::greater<>{});

    std::size_t h = 0;
    while (h < citations.size() && std::cmp_greater_equal(citations[h], h + 1)) {
      ++h;
    }

    return static_cast<int>(h);
  }

  // Linear-time alternative using counting sort buckets capped at n.
  //
  // Any paper with > n citations contributes equally to every candidate h in
  // [1, n], so clamping counts to n preserves the exact h-index in O(n) time.
  //
  // Time: O(n), Space: O(n).
  int hIndexCounting(const std::vector<int>& citations) {
    const std::size_t n = citations.size();
    if (n == 0) {
      return 0;
    }

    // buckets[c] holds the number of papers with exactly c citations (or >= n
    // citations when c == n).
    std::vector<std::size_t> buckets(n + 1, 0);
    for (const int count : citations) {
      const auto clamped =
          std::min(static_cast<std::size_t>(std::max(count, 0)), n);
      ++buckets[clamped];
    }

    // Accumulate papers with >= h citations from largest possible h downward.
    std::size_t papers_at_least_h = 0;
    for (std::size_t h = n; h > 0; --h) {
      papers_at_least_h += buckets[h];
      if (papers_at_least_h >= h) {
        return static_cast<int>(h);
      }
    }

    return 0;
  }
};

LC_PROBLEM(274, "H Index", "two_pointers", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    std::vector<int> citations{3, 0, 6, 1, 5};
    ctx.expect_eq(sol.hIndexCounting(citations), 3);
    ctx.expect_eq(sol.hIndex(citations), 3);
  });

  ctx.test("example_2", [&] {
    std::vector<int> citations{1, 3, 1};
    ctx.expect_eq(sol.hIndexCounting(citations), 1);
    ctx.expect_eq(sol.hIndex(citations), 1);
  });

  ctx.test("single_zero", [&] {
    std::vector<int> citations{0};
    ctx.expect_eq(sol.hIndexCounting(citations), 0);
    ctx.expect_eq(sol.hIndex(citations), 0);
  });

  ctx.test("single_high_citation", [&] {
    std::vector<int> citations{100};
    ctx.expect_eq(sol.hIndexCounting(citations), 1);
    ctx.expect_eq(sol.hIndex(citations), 1);
  });

  ctx.test("empty_and_all_zeroes", [&] {
    std::vector<int> empty{};
    std::vector<int> zeros{0, 0, 0, 0};
    ctx.expect_eq(sol.hIndexCounting(empty), 0);
    ctx.expect_eq(sol.hIndex(empty), 0);
    ctx.expect_eq(sol.hIndexCounting(zeros), 0);
    ctx.expect_eq(sol.hIndex(zeros), 0);
  });

  ctx.test("all_papers_qualify", [&] {
    std::vector<int> citations{5, 5, 5, 5, 5};
    ctx.expect_eq(sol.hIndexCounting(citations), 5);
    ctx.expect_eq(sol.hIndex(citations), 5);
  });
}

}  // namespace
