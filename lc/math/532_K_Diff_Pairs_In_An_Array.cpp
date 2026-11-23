// LeetCode 532: K Diff Pairs In An Array
// https://leetcode.com/problems/k-diff-pairs-in-an-array/
//
// Problem:
//   Given an integer array `nums` and an integer `k`, return the number of
//   unique k-diff pairs (nums[i], nums[j]) such that 0 <= i, j < nums.size(),
//   i != j, and |nums[i] - nums[j]| == k. Pairs (a, b) and (b, a) are
//   considered identical, and duplicate value pairs are counted only once.
//   Constraints: 1 <= nums.size() <= 10^4, -10^7 <= nums[i] <= 10^7,
//   0 <= k <= 10^7.
//
// Algorithm:
//   Frequency hash-map lookup with canonical one-directional pairing.
//   If k < 0, no absolute difference can equal k, so return 0 immediately.
//   Otherwise, count the frequency of every distinct integer in `nums`.
//   Iterate over each unique value `val` in the frequency map:
//     - If k == 0, the pair (val, val) is valid iff `val` appears at least
//       twice (freq[val] >= 2).
//     - If k > 0, orient every unordered pair canonically as (val, val + k)
//       with val < val + k, and check whether (val + k) exists in the map.
//       Checking only the upward direction (+k) guarantees each unique pair
//       is counted at most once without mutating or deleting map entries.
//   Time Complexity:  O(n) average to populate and scan the frequency map.
//   Space Complexity: O(n) to store distinct element counts.
//
// Data Structures:
//   - `std::unordered_map<int, int>`: maps each distinct element in `nums` to
//     its occurrence count, enabling O(1) average-time duplicate detection
//     (k == 0) and partner lookup (k > 0).
//
// Real-World Applications:
//   - Signal processing and peak analysis: identifying unique pairs of
//     spectral frequencies separated by a fixed harmonic offset.
//   - Financial market surveillance: detecting unique price levels or orders
//     separated by a fixed spread without double-counting repeated quotes.
//   - Genomic variant analysis: finding unique loci separated by an exact
//     base-pair distance across a set of candidate positions.
//
// Related LeetCode Problems:
//   - #1 Two Sum [done] (complement lookup using a hash map)
//   - #2006 Count Number of Pairs With Absolute Difference K [new] (counts index pairs rather than unique value pairs)
//   - #719 Find K-th Smallest Pair Distance [new] (binary search on sorted pairwise distances)
//   - #2176 Count Equal and Divisible Pairs in an Array [new] (constrained pair counting)

#include "framework/lc_framework.h"

#include <cstdint>
#include <limits>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the number of unique value pairs {a, b} in `nums` (using distinct
  // indices) whose absolute difference equals `k`.
  //
  // Invariant: Iterating over unique keys in `freq` and checking only the
  // upward partner `val + k` ensures every unordered pair {val, val + k} is
  // counted exactly once.
  // Time: O(n) average, Space: O(n).
  int findPairs(const std::vector<int>& nums, int k) {
    // An absolute difference |a - b| is always non-negative.
    if (k < 0) {
      return 0;
    }

    // Build value frequencies; distinct keys naturally deduplicate pairs.
    std::unordered_map<int, int> freq;
    freq.reserve(nums.size());
    for (const int num : nums) {
      ++freq[num];
    }

    int unique_pairs = 0;
    for (const auto& [val, count] : freq) {
      if (k == 0) {
        // For k == 0, we need at least two occurrences of the same value.
        if (count >= 2) {
          ++unique_pairs;
        }
        continue;
      }

      // Compute val + k in 64-bit arithmetic to avoid signed overflow near
      // INT_MAX before probing the map.
      const std::int64_t partner = static_cast<std::int64_t>(val) + k;
      if (partner <= std::numeric_limits<int>::max() &&
          freq.contains(static_cast<int>(partner))) {
        ++unique_pairs;
      }
    }

    return unique_pairs;
  }
};

LC_PROBLEM(532, "K Diff Pairs In An Array", "math", ctx) {
  Solution sol;

  ctx.test("example_1_with_duplicates", [&] {
    ctx.expect_eq(sol.findPairs({3, 1, 4, 1, 5}, 2), 2);
  });

  ctx.test("example_2_consecutive", [&] {
    ctx.expect_eq(sol.findPairs({1, 2, 3, 4, 5}, 1), 4);
  });

  ctx.test("example_3_zero_diff", [&] {
    ctx.expect_eq(sol.findPairs({1, 3, 1, 5, 4}, 0), 1);
  });

  ctx.test("python_build_case", [&] {
    // Unique values: {0, 1, 2, 3, 4, 9}; 3-diff pairs are (0, 3) and (1, 4).
    ctx.expect_eq(sol.findPairs({1, 2, 4, 4, 3, 3, 0, 9, 2, 3}, 3), 2);
  });

  ctx.test("negative_k_returns_zero", [&] {
    ctx.expect_eq(sol.findPairs({1, 2, 3, 4}, -1), 0);
  });

  ctx.test("negative_elements", [&] {
    ctx.expect_eq(sol.findPairs({-1, -2, -3, -2}, 1), 2);
  });

  ctx.test("zero_diff_multiple_duplicates", [&] {
    ctx.expect_eq(sol.findPairs({1, 1, 1, 2, 2, 3}, 0), 2);
  });
}

}  // namespace
