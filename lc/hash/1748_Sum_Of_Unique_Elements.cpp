// LeetCode 1748: Sum Of Unique Elements
// https://leetcode.com/problems/sum-of-unique-elements/
//
// Problem:
//   Given an integer array `nums`, define a unique element as a value that
//   appears strictly once in `nums`. Return the sum of all unique elements in
//   `nums` (or 0 if none exist).
//   Constraints: 1 <= nums.length <= 100; 1 <= nums[i] <= 100.
//
// Algorithm:
//   Two-pass hash frequency counting:
//   1. Traverse `nums` and tally the occurrence count of every value in an
//      unordered hash map.
//   2. Traverse the frequency map and add each key whose recorded count equals
//      1 to the running sum.
//   Time Complexity: O(n) average, where n = nums.size().
//   Space Complexity: O(u) where u <= n is the number of distinct elements.
//
// Data Structures:
//   - std::unordered_map<int, int>: Maps each distinct integer in `nums` to
//     its total frequency so uniqueness can be tested in O(1) average time.
//
// Real-World Applications:
//   - Financial reconciliation and billing audits isolating non-duplicated
//     transactions or single-occurrence line items.
//   - Log and telemetry deduplication pipelines identifying isolated events
//     versus repeated anomalies.
//   - Database GROUP BY ... HAVING COUNT(*) = 1 aggregation queries.
//
// Related LeetCode Problems:
//   - #136 Single Number [done] (finding a unique element using XOR/hash)
//   - #217 Contains Duplicate [done] (detecting repeated elements with a hash set)
//   - #387 First Unique Character in a String [done] (frequency counting for count == 1)
//   - #1207 Unique Number of Occurrences [done] (hash-map frequency analysis)

#include "framework/lc_framework.h"

#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the sum of all values in `nums` that occur exactly once.
  // Time: O(n) average, Space: O(u) where u is the count of distinct elements.
  int sumOfUnique(const std::vector<int>& nums) {
    // Phase 1: Count occurrences of every value in `nums`.
    std::unordered_map<int, int> freq;
    freq.reserve(nums.size());
    for (const int num : nums) {
      ++freq[num];
    }

    // Phase 2: Sum only those values whose frequency is strictly 1.
    int sum = 0;
    for (const auto& [val, count] : freq) {
      if (count == 1) {
        sum += val;
      }
    }
    return sum;
  }
};

LC_PROBLEM(1748, "Sum Of Unique Elements", "hash", ctx) {
  Solution sol;

  ctx.test("example_1_mixed", [&] {
    ctx.expect_eq(sol.sumOfUnique({1, 2, 3, 2}), 4);
  });

  ctx.test("example_2_all_duplicates", [&] {
    ctx.expect_eq(sol.sumOfUnique({1, 1, 1, 1, 1}), 0);
  });

  ctx.test("example_3_all_unique", [&] {
    ctx.expect_eq(sol.sumOfUnique({1, 2, 3, 4, 5}), 15);
  });

  ctx.test("single_element", [&] {
    ctx.expect_eq(sol.sumOfUnique({42}), 42);
  });

  ctx.test("empty_and_negative_values", [&] {
    ctx.expect_eq(sol.sumOfUnique({}), 0);
    ctx.expect_eq(sol.sumOfUnique({-3, 5, -3, 10, 10, 7}), 12);
  });
}

}  // namespace
