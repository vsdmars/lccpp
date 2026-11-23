// LeetCode 1287: Element Appearing More Than 25 In Sorted Array
// https://leetcode.com/problems/element-appearing-more-than-25-in-sorted-array/
//
// Problem:
//   Given an integer array `arr` sorted in non-decreasing order, there is
//   guaranteed to be exactly one integer in `arr` that occurs strictly more
//   than 25% of the time. Return that integer.
//   Constraints: 1 <= arr.length <= 10^4, 0 <= arr[i] <= 10^5.
//
// Examples:
//   Example 1:
//     Input:  arr = [1, 2, 2, 6, 6, 6, 6, 7, 10]
//     Output: 6
//     Explanation:
//       - Compute n = 9 and threshold = 9 / 4 = 2, and set quarter checkpoints at
//         indices {2, 4, 6}.
//       - Probe idx = 2 where arr[2] = 2; equal_range spans [1, 3) with length 2,
//         which is not > 2.
//       - Probe idx = 4 where arr[4] = 6; equal_range spans [3, 7) with length 4 >
//         2, so return 6.
//
//   Example 2:
//     Input:  arr = [1, 2, 3, 4, 4, 4, 5, 6]
//     Output: 4
//     Explanation:
//       - Compute n = 8 and threshold = 8 / 4 = 2, and set quarter checkpoints at
//         indices {2, 4, 6}.
//       - Probe idx = 2 where arr[2] = 3; equal_range spans [2, 3) with length 1 <=
//         2.
//       - Probe idx = 4 where arr[4] = 4; equal_range spans [3, 6) with length 3 >
//         2, so return 4.
//
//   Example 3:
//     Input:  arr = [42]
//     Output: 42
//     Explanation:
//       - Compute n = 1 and threshold = 1 / 4 = 0, so the quarter checkpoints {n/4,
//         n/2, 3n/4} evaluate to {0, 0, 0}.
//       - Probe idx = 0 where arr[0] = 42; equal_range spans [0, 1) with length 1 >
//         0, so return 42.
//
// Algorithm:
//   1. Frequency Counting (`findSpecialInteger`, matching the Python source):
//      Count occurrences of each value in a hash map and return the value
//      whose count exceeds `n / 4` (using exact integer arithmetic: for any
//      integers `c` and `n`, `c > n / 4.0` holds iff `c > n / 4`).
//      Time Complexity:  O(n) expected.
//      Space Complexity: O(n).
//
//   2. Sorted-Array Quarter-Checkpoint Binary Search
//      (`findSpecialIntegerBinarySearch`):
//      Because `arr` is sorted and the target occupies a contiguous block of
//      length strictly greater than `n / 4`, it MUST cover at least one of the
//      three quarter-boundary indices `{n / 4, n / 2, (3 * n) / 4}`. For each
//      candidate value at those indices, compute its exact span via binary
//      search (`std::ranges::equal_range`) and return the first whose span
//      exceeds `n / 4`.
//      Time Complexity:  O(log n) — at most three binary searches.
//      Space Complexity: O(1).
//
// Data Structures:
//   - `std::unordered_map<int, std::size_t>` for the O(n) frequency-count
//     solution.
//   - Fixed `std::array<std::size_t, 3>` of quarter checkpoint indices for the
//     O(log n) sorted binary-search solution.
//
// Real-World Applications:
//   - Heavy-hitter detection on sorted columnar database blocks and Parquet
//     dictionary-encoded pages (identifying dominant values for RLE or
//     default-value compression in O(log n) probes).
//   - Log-stream and distributed telemetry analytics where pre-sorted shards
//     are sampled at fixed quantiles to spot runaway error codes.
//   - Quorum and consensus threshold verification over ordered vote logs.
//
// Related LeetCode Problems:
//   - #169 Majority Element [done] (element appearing > 50% of the time)
//   - #229 Majority Element II [new] (elements appearing > 33% of the time)
//   - #34 Find First and Last Position of Element in Sorted Array [done] (equal_range binary search)
//   - #1150 Check If a Number Is Majority Element in a Sorted Array [new] (sorted-array binary search threshold)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the unique integer appearing strictly more than 25% of the time by
  // counting element frequencies (mirrors the reference Python solution).
  // Time: O(n) average, Space: O(n).
  int findSpecialInteger(const std::vector<int>& arr) {
    if (arr.empty()) {
      return -1;
    }

    // For integers `count` and `n`, `count > n / 4.0` iff `count > n / 4`.
    const std::size_t threshold = arr.size() / 4uz;
    std::unordered_map<int, std::size_t> counts;
    counts.reserve(arr.size());

    for (const int num : arr) {
      const std::size_t freq = ++counts[num];
      if (freq > threshold) {
        return num;
      }
    }

    return -1;
  }

  // Optimal sorted-array solution: probes the three quarter-boundary positions
  // and verifies each candidate's contiguous range with binary search.
  // Time: O(log n), Space: O(1).
  int findSpecialIntegerBinarySearch(const std::vector<int>& arr) {
    const std::size_t n = arr.size();
    if (n == 0uz) {
      return -1;
    }

    const std::size_t threshold = n / 4uz;
    // Any contiguous block of length > n/4 must cover at least one of these
    // three indices (when n < 4, n / 4 == 0 probes the single dominant value).
    const std::array<std::size_t, 3> checkpoints = {
        n / 4uz,
        n / 2uz,
        (3uz * n) / 4uz,
    };

    for (const std::size_t idx : checkpoints) {
      const int candidate = arr[idx];
      const auto range = std::ranges::equal_range(arr, candidate);
      const auto span = static_cast<std::size_t>(range.end() - range.begin());
      if (span > threshold) {
        return candidate;
      }
    }

    return -1;
  }
};

LC_PROBLEM(1287, "Element Appearing More Than 25 In Sorted Array", "array",
           ctx) {
  Solution sol;

  ctx.test("example_1_build_case", [&] {
    const std::vector<int> arr = {1, 2, 2, 6, 6, 6, 6, 7, 10};
    ctx.expect_eq(sol.findSpecialInteger(arr), 6);
    ctx.expect_eq(sol.findSpecialIntegerBinarySearch(arr), 6);
  });

  ctx.test("example_2_two_identical", [&] {
    const std::vector<int> arr = {1, 1};
    ctx.expect_eq(sol.findSpecialInteger(arr), 1);
    ctx.expect_eq(sol.findSpecialIntegerBinarySearch(arr), 1);
  });

  ctx.test("edge_single_element", [&] {
    const std::vector<int> arr = {42};
    ctx.expect_eq(sol.findSpecialInteger(arr), 42);
    ctx.expect_eq(sol.findSpecialIntegerBinarySearch(arr), 42);
  });

  ctx.test("dominant_at_start_and_end", [&] {
    const std::vector<int> start_dom = {0, 0, 1, 2, 3};
    ctx.expect_eq(sol.findSpecialInteger(start_dom), 0);
    ctx.expect_eq(sol.findSpecialIntegerBinarySearch(start_dom), 0);

    const std::vector<int> end_dom = {1, 2, 3, 9, 9};
    ctx.expect_eq(sol.findSpecialInteger(end_dom), 9);
    ctx.expect_eq(sol.findSpecialIntegerBinarySearch(end_dom), 9);
  });

  ctx.test("exact_boundary_length_8", [&] {
    // n = 8 -> threshold = 2, so target must appear at least 3 times.
    const std::vector<int> arr = {1, 2, 3, 4, 4, 4, 5, 6};
    ctx.expect_eq(sol.findSpecialInteger(arr), 4);
    ctx.expect_eq(sol.findSpecialIntegerBinarySearch(arr), 4);
  });
}

}  // namespace
