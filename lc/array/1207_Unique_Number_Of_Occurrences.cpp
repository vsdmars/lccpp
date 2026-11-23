// LeetCode 1207: Unique Number Of Occurrences
// https://leetcode.com/problems/unique-number-of-occurrences/
//
// Problem:
//   Given an integer array `arr`, return true if the frequency (number of
//   occurrences) of every distinct value in `arr` is unique, and false
//   otherwise.
//   Constraints: 1 <= arr.length <= 1000, -1000 <= arr[i] <= 1000.
//
// Examples:
//   Example 1:
//     Input:  arr = [1, 2, 2, 1, 1, 3]
//     Output: true
//     Explanation:
//       - Phase 1 counts every element into the hash map counts: {1: 3, 2: 2, 3:
//         1}.
//       - Phase 2 inserts frequencies 3, 2, and 1 into seen_frequencies, and all
//         three insertions succeed without duplicates.
//       - Since all occurrence counts are pairwise distinct, the function returns
//         true.
//
//   Example 2:
//     Input:  arr = [1, 2]
//     Output: false
//     Explanation:
//       - Phase 1 records counts = {1: 1, 2: 1}.
//       - Phase 2 inserts the first frequency 1 into seen_frequencies, then
//         attempts to insert 1 again for the second entry and sees .second ==
//         false.
//       - The duplicate frequency causes an immediate return of false.
//
//   Example 3:
//     Input:  arr = [-3, 0, 1, -3, 1, 1, 1, -3, 10, 0]
//     Output: true
//     Explanation:
//       - Phase 1 tallies each distinct integer, including negative values and
//         zero, into counts = {-3: 3, 0: 2, 1: 4, 10: 1}.
//       - Phase 2 inserts the four frequencies 3, 2, 4, and 1 into seen_frequencies
//         with no collision, returning true.
//
// Algorithm:
//   Two-Stage Frequency Counting & Uniqueness Check:
//   1. Traverse `arr` and tally each integer's occurrence count in a hash map
//      `counts`.
//   2. Traverse the resulting `(value, freq)` entries and insert each `freq`
//      into a hash set `seen_frequencies`. If any insertion reports that
//      `freq` was already present, two distinct values share the same
//      occurrence count, so return false immediately.
//   3. If every frequency is distinct, return true.
//   Time Complexity:  O(n) expected time (two linear passes with O(1) average
//                     hash operations).
//   Space Complexity: O(u) where u <= n is the number of distinct values.
//
// Data Structures:
//   - `std::unordered_map<int, int>`: maps each distinct element in `arr` to
//     its total occurrence count.
//   - `std::unordered_set<int>`: tracks observed frequency counts to detect
//     duplicates in O(1) average time.
//
// Real-World Applications:
//   - Telemetry and log histogram analysis (verifying distinct event tiers or
//     detecting colliding bucket cardinalities).
//   - Data quality validation in ETL pipelines (checking whether group-by
//     partition sizes have unambiguous rankings without ties).
//   - Cryptanalysis and frequency-analysis heuristics (checking whether symbol
//     frequencies uniquely identify each character in a substitution cipher).
//
// Related LeetCode Problems:
//   - #1647 Minimum Deletions to Make Character Frequencies Unique [new] (repairing duplicate frequencies)
//   - #1941 Check if All Characters Have Equal Number of Occurrences [new] (opposite frequency invariant)
//   - #217 Contains Duplicate [done] (basic hash-set uniqueness check)
//   - #347 Top K Frequent Elements [done] (frequency map over array elements)

#include "framework/lc_framework.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

class Solution {
 public:
  // Returns true iff every distinct value in `arr` occurs a unique number of
  // times.
  // Time: O(n) average, Space: O(n).
  bool uniqueOccurrences(const std::vector<int>& arr) {
    // Phase 1: count occurrences of each distinct value.
    std::unordered_map<int, int> counts;
    counts.reserve(arr.size());
    for (const int num : arr) {
      ++counts[num];
    }

    // Phase 2: ensure all occurrence counts are pairwise distinct.
    std::unordered_set<int> seen_frequencies;
    seen_frequencies.reserve(counts.size());
    for (const auto& entry : counts) {
      if (!seen_frequencies.insert(entry.second).second) {
        return false;
      }
    }

    return true;
  }
};

LC_PROBLEM(1207, "Unique Number Of Occurrences", "array", ctx) {
  Solution sol;

  ctx.test("example_1_unique_counts", [&] {
    ctx.expect_true(sol.uniqueOccurrences({1, 2, 2, 1, 1, 3}));
  });

  ctx.test("example_2_duplicate_counts", [&] {
    ctx.expect_false(sol.uniqueOccurrences({1, 2}));
  });

  ctx.test("example_3_negative_and_zero_values", [&] {
    ctx.expect_true(
        sol.uniqueOccurrences({-3, 0, 1, -3, 1, 1, 1, -3, 10, 0}));
  });

  ctx.test("edge_cases_single_and_all_equal", [&] {
    ctx.expect_true(sol.uniqueOccurrences({7}));
    ctx.expect_true(sol.uniqueOccurrences({-1000, -1000, -1000}));
    ctx.expect_false(sol.uniqueOccurrences({5, 5, 6, 6}));
  });
}

}  // namespace
