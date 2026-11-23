// LeetCode 1636: Sort Array By Increasing Frequency
// https://leetcode.com/problems/sort-array-by-increasing-frequency/
//
// Problem:
//   Given an integer array `nums`, sort the array in increasing order based on
//   the frequency of each value. If multiple values have the same frequency,
//   sort them in decreasing order of their numeric value.
//   Constraints: 1 <= nums.length <= 100, -100 <= nums[i] <= 100.
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 1, 2, 2, 2, 3]
//     Output: [3, 1, 1, 2, 2, 2]
//     Explanation:
//       - Tally frequencies into freq = {3: 1, 1: 2, 2: 3}.
//       - Sort nums by ascending freq.at(val), placing 3 (freq 1) before 1 (freq 2)
//         and 2 (freq 3).
//       - Return the sorted vector [3, 1, 1, 2, 2, 2].
//
//   Example 2:
//     Input:  nums = [2, 3, 1, 3, 2]
//     Output: [1, 3, 3, 2, 2]
//     Explanation:
//       - Count each value in freq = {1: 1, 2: 2, 3: 2}.
//       - Place 1 first because its frequency (1) is smaller than 2 and 3 (both
//         frequency 2).
//       - Break the frequency-2 tie between 3 and 2 by descending value (3 > 2),
//         yielding [1, 3, 3, 2, 2].
//
//   Example 3:
//     Input:  nums = [-1, 1, -6, 4, 5, -6, 1, 4, 1]
//     Output: [5, -1, 4, 4, -6, -6, 1, 1, 1]
//     Explanation:
//       - Count occurrences into freq = {5: 1, -1: 1, 4: 2, -6: 2, 1: 3}.
//       - Sort the frequency-1 values descending (5 > -1) to produce [5, -1] and
//         the frequency-2 values descending (4 > -6) to produce [4, 4, -6, -6].
//       - Place the frequency-3 elements [1, 1, 1] last, returning [5, -1, 4, 4,
//         -6, -6, 1, 1, 1].
//
// Algorithm:
//   1. Count the frequency of every distinct integer in `nums` using a hash map
//      (`std::unordered_map<int, int>`).
//   2. Sort `nums` in place with `std::ranges::sort` using a comparator that
//      orders by `freq[a] < freq[b]` first, and breaks frequency ties by
//      `a > b` (descending value).
//   Time Complexity:  O(n log n) for comparison sorting (O(n) to build counts).
//   Space Complexity: O(n) for the frequency hash map and O(log n) sort stack.
//
// Data Structures:
//   - `std::unordered_map<int, int>`: maps each distinct value to its total
//     occurrence count in `nums`.
//   - `std::vector<int>`: sorted in place using the frequency table.
//
// Real-World Applications:
//   - Anomaly detection and log triage (surfacing rare error codes first while
//     ordering ties by highest severity code).
//   - Huffman coding and statistical compression (sorting symbols by ascending
//     frequency before building an optimal prefix tree).
//   - Cache eviction / LFU (Least Frequently Used) candidate ranking with
//     deterministic tie-breaking.
//
// Related LeetCode Problems:
//   - #451 Sort Characters By Frequency [done] (sorts string characters by decreasing frequency)
//   - #347 Top K Frequent Elements [done] (extracts the k highest-frequency values)
//   - #692 Top K Frequent Words [done] (frequency sorting with lexicographical tie-breaking)
//   - #1356 Sort Integers By The Number Of 1 Bits [done] (two-key custom comparator sorting)

#include "framework/lc_framework.h"

#include <algorithm>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Sorts `nums` by ascending frequency, breaking ties by descending value.
  // Time: O(n log n), Space: O(n).
  std::vector<int> frequencySort(std::vector<int> nums) {
    // Phase 1: tally frequency of each distinct value.
    std::unordered_map<int, int> freq;
    freq.reserve(nums.size());
    for (const int val : nums) {
      ++freq[val];
    }

    // Phase 2: sort in place by (frequency ascending, value descending).
    std::ranges::sort(nums, [&freq](int lhs, int rhs) {
      const int lhs_freq = freq.at(lhs);
      const int rhs_freq = freq.at(rhs);
      if (lhs_freq != rhs_freq) {
        return lhs_freq < rhs_freq;
      }
      return lhs > rhs;
    });

    return nums;
  }
};

LC_PROBLEM(1636, "Sort Array By Increasing Frequency", "sort", ctx) {
  Solution sol;

  ctx.test("example1_distinct_frequencies", [&] {
    const std::vector<int> input = {1, 1, 2, 2, 2, 3};
    const std::vector<int> expected = {3, 1, 1, 2, 2, 2};
    ctx.expect_eq(sol.frequencySort(input), expected);
  });

  ctx.test("example2_tie_broken_descending", [&] {
    const std::vector<int> input = {2, 3, 1, 3, 2};
    const std::vector<int> expected = {1, 3, 3, 2, 2};
    ctx.expect_eq(sol.frequencySort(input), expected);
  });

  ctx.test("example3_negative_values", [&] {
    const std::vector<int> input = {-1, 1, -6, 4, 5, -6, 1, 4, 1};
    const std::vector<int> expected = {5, -1, 4, 4, -6, -6, 1, 1, 1};
    ctx.expect_eq(sol.frequencySort(input), expected);
  });

  ctx.test("all_unique_and_all_identical", [&] {
    ctx.expect_eq(sol.frequencySort({1, 2, 3, 4}),
                  std::vector<int>{4, 3, 2, 1});
    ctx.expect_eq(sol.frequencySort({5, 5, 5}), std::vector<int>{5, 5, 5});
    ctx.expect_eq(sol.frequencySort({-100}), std::vector<int>{-100});
  });
}

}  // namespace
