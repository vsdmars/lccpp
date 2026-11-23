// LeetCode 697: Degree Of An Array
// https://leetcode.com/problems/degree-of-an-array/
//
// Problem:
//   Given a non-empty array of non-negative integers `nums`, the degree of the
//   array is defined as the maximum frequency of any single element in `nums`.
//   Find the length of the smallest contiguous subarray of `nums` that has the
//   exact same degree as `nums`.
//   Constraints: 1 <= nums.size() <= 50,000; 0 <= nums[i] <= 49,999.
//
// Algorithm:
//   Any shortest subarray achieving the array's degree `d` must start at the
//   first occurrence and end at the last occurrence of some element whose total
//   frequency is `d`. We compute this in a single left-to-right pass: for each
//   value `x`, record its first index the first time we see it and increment
//   its running frequency. Whenever `x`'s frequency strictly exceeds the best
//   degree seen so far, update both the degree and the shortest length to
//   `i - first_index[x] + 1`. Whenever `x`'s frequency ties the current degree,
//   take the minimum of the current shortest length and `i - first_index[x] + 1`.
//   Time Complexity:  O(n) expected, visiting each element once.
//   Space Complexity: O(u), where u <= n is the number of distinct values.
//
// Data Structures:
//   - Hash map (`std::unordered_map<int, ElementStats>`): maps each distinct
//     value to its running frequency and first-seen index so both updates and
//     span calculations run in O(1) expected time.
//
// Real-World Applications:
//   - Log and telemetry burst analysis: identifying the tightest time window
//     capturing peak error or event bursts for the most frequent fault code.
//   - Network packet inspection: finding the shortest packet window containing
//     all occurrences of the dominant traffic flow signature.
//   - Genomic motif localization: locating the shortest sequence span covering
//     all repeats of the most abundant k-mer.
//
// Related LeetCode Problems:
//   - #53 Maximum Subarray [done] (contiguous subarray optimization in one pass)
//   - #209 Minimum Size Subarray Sum [done] (shortest contiguous subarray meeting a target property)
//   - #347 Top K Frequent Elements [done] (frequency counting with hash maps)
//   - #169 Majority Element [done] (dominant element frequency analysis)
//   - #2260 Minimum Consecutive Cards to Pick Up [new] (shortest span between repeated values)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <unordered_map>
#include <vector>

namespace {

// Tracks the frequency and earliest index of a distinct value in `nums`.
struct ElementStats {
  int count = 0;
  std::size_t first_idx = 0uz;
};

class Solution {
 public:
  // Finds the minimum length of a contiguous subarray with the same degree as
  // `nums` in a single pass.
  // Time: O(n) expected, Space: O(u) where u is the count of distinct elements.
  int findShortestSubArray(const std::vector<int>& nums) {
    if (nums.empty()) {
      return 0;
    }

    std::unordered_map<int, ElementStats> stats;
    stats.reserve(nums.size());

    int max_degree = 0;
    int shortest_len = 0;

    for (std::size_t idx = 0uz; idx < nums.size(); ++idx) {
      const int value = nums[idx];

      // Insert with `first_idx = idx` only on the first sighting of `value`;
      // subsequent occurrences leave `first_idx` untouched.
      const auto it =
          stats.try_emplace(value, ElementStats{.count = 0, .first_idx = idx})
              .first;

      ++it->second.count;
      const int current_count = it->second.count;
      // Span from the first occurrence of `value` through the current index.
      const int span = static_cast<int>(idx - it->second.first_idx) + 1;

      if (current_count > max_degree) {
        // Strictly higher degree supersedes any earlier candidate subarrays.
        max_degree = current_count;
        shortest_len = span;
      } else if (current_count == max_degree) {
        // Tied degree: keep whichever element spans the tighter window.
        shortest_len = std::min(shortest_len, span);
      }
    }

    return shortest_len;
  }
};

LC_PROBLEM(697, "Degree Of An Array", "array", ctx) {
  Solution sol;

  ctx.test("example_1_tied_degree_shorter_inner_span", [&] {
    ctx.expect_eq(sol.findShortestSubArray({1, 2, 2, 3, 1}), 2);
  });

  ctx.test("example_2_unique_degree_element", [&] {
    ctx.expect_eq(sol.findShortestSubArray({1, 2, 2, 3, 1, 4, 2}), 6);
  });

  ctx.test("single_element", [&] {
    ctx.expect_eq(sol.findShortestSubArray({42}), 1);
  });

  ctx.test("all_unique_elements", [&] {
    ctx.expect_eq(sol.findShortestSubArray({1, 2, 3, 4, 5}), 1);
  });

  ctx.test("all_identical_elements", [&] {
    ctx.expect_eq(sol.findShortestSubArray({7, 7, 7, 7}), 4);
  });

  ctx.test("multiple_tied_degrees_middle_wins", [&] {
    // 1 spans 6, 3 spans 4, 2 spans 2.
    ctx.expect_eq(sol.findShortestSubArray({1, 3, 2, 2, 3, 1}), 2);
  });

  ctx.test("later_element_breaks_degree_tie", [&] {
    // 1 reaches degree 2 with span 4, then 2 reaches degree 3 with span 5.
    ctx.expect_eq(sol.findShortestSubArray({1, 2, 3, 1, 2, 2}), 5);
  });
}

}  // namespace
