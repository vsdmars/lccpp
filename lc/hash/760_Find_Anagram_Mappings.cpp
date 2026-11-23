// LeetCode 760: Find Anagram Mappings
// https://leetcode.com/problems/find-anagram-mappings/
//
// Problem:
//   Given two integer arrays `nums1` (A) and `nums2` (B) of equal length where
//   `nums2` is an anagram (permutation) of `nums1`, return an index mapping
//   array `P` such that `P[i] = j` implies `nums1[i] == nums2[j]`. Both arrays
//   may contain duplicate values; if multiple valid mappings exist, return any
//   of them.
//   Constraints: 1 <= nums1.length == nums2.length <= 100,
//   0 <= nums1[i], nums2[i] <= 10^5.
//
// Examples:
//   Example 1:
//     Input:  nums1 = [12, 28, 46, 32, 50], nums2 = [50, 12, 32, 46, 28]
//     Output: [1, 4, 3, 2, 0]
//     Explanation:
//       - Index nums2 into positions: 50 -> [0], 12 -> [1], 32 -> [2], 46 -> [3],
//         28 -> [4].
//       - Scan nums1 and pop from each value's vector: 12 yields 1, 28 yields 4, 46
//         yields 3, 32 yields 2, and 50 yields 0.
//       - Return mapping = [1, 4, 3, 2, 0], satisfying nums2[mapping[i]] ==
//         nums1[i] for every index i.
//
//   Example 2:
//     Input:  nums1 = [50, 12, 28, 46, 32, 50], nums2 = [50, 12, 32, 46, 28, 50]
//     Output: [5, 1, 4, 3, 2, 0]
//     Explanation:
//       - Index nums2 into positions: 50 -> [0, 5], 12 -> [1], 32 -> [2], 46 ->
//         [3], 28 -> [4].
//       - For nums1[0] = 50, pop the back of positions[50], recording index 5 and
//         leaving [0].
//       - Pop indices for 12 (1), 28 (4), 46 (3), and 32 (2) in order.
//       - For nums1[5] = 50, pop the remaining index 0 from positions[50],
//         returning the valid bijection [5, 1, 4, 3, 2, 0].
//
//   Example 3:
//     Input:  nums1 = [42], nums2 = [42]
//     Output: [0]
//     Explanation:
//       - Index the single element of nums2 into positions[42] = [0].
//       - Scan nums1[0] = 42, pop 0 from positions[42], and return [0].
//
// Algorithm:
//   1. Index every element of `nums2` into a hash map `positions` that maps
//      each value `v` to a stack (`std::vector<int>`) of all indices where `v`
//      appears in `nums2`.
//   2. Iterate through `nums1`: for each value `v`, pop an index from
//      `positions[v]` and append it to the result vector. Popping guarantees
//      that duplicate values map to distinct indices in `nums2` (a true
//      bijection / permutation of `0 .. n - 1`).
//   Time Complexity: O(N) average, where N = nums1.size().
//   Space Complexity: O(N) for the value-to-indices hash map and result.
//
// Data Structures:
//   - Hash map of stacks (`std::unordered_map<int, std::vector<int>>`):
//     provides O(1) amortized lookup and pop per element while cleanly
//     handling duplicate values.
//
// Real-World Applications:
//   - Record linkage and column permutation alignment when matching reordered
//     rows/fields across two snapshots of a dataset.
//   - Diffing and patch generation for reordered UI lists (keyed reconciliation
//     in virtual DOM frameworks).
//   - Graph/matrix permutation tracking when aligning vertices or rows before
//     and after sorting or reordering passes.
//
// Related LeetCode Problems:
//   - #242 Valid Anagram [done] (verifying whether two sequences are anagrams)
//   - #49 Group Anagrams [done] (partitioning strings into anagram equivalence classes)
//   - #1 Two Sum [done] (value-to-index hash map lookup)
//   - #1331 Rank Transform of an Array [new] (value-to-position mapping via sorting + hash map)

#include "framework/lc_framework.h"

#include <cstddef>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Computes a bijective index mapping `P` such that `nums2[P[i]] == nums1[i]`
  // for every index `i`.
  // Time: O(N) average, Space: O(N).
  std::vector<int> anagramMappings(const std::vector<int>& nums1,
                                   const std::vector<int>& nums2) {
    // Record every occurrence index in `nums2` per value so duplicates receive
    // distinct target indices when popped.
    std::unordered_map<int, std::vector<int>> positions;
    positions.reserve(nums2.size());
    for (std::size_t i = 0uz; i < nums2.size(); ++i) {
      positions[nums2[i]].push_back(static_cast<int>(i));
    }

    // Build the mapping by popping one available `nums2` index per `nums1` val.
    std::vector<int> mapping;
    mapping.reserve(nums1.size());
    for (const int val : nums1) {
      auto& bucket = positions[val];
      mapping.push_back(bucket.back());
      bucket.pop_back();
    }

    return mapping;
  }
};

// Verifies that `mapping` is a valid bijective anagram mapping from `a` to `b`.
bool isValidBijectiveMapping(const std::vector<int>& a,
                             const std::vector<int>& b,
                             const std::vector<int>& mapping) {
  if (mapping.size() != a.size() || a.size() != b.size()) {
    return false;
  }

  std::vector<bool> used(b.size(), false);
  for (std::size_t i = 0uz; i < a.size(); ++i) {
    if (mapping[i] < 0) {
      return false;
    }
    const auto idx = static_cast<std::size_t>(mapping[i]);
    if (idx >= b.size() || used[idx] || b[idx] != a[i]) {
      return false;
    }
    used[idx] = true;
  }
  return true;
}

LC_PROBLEM(760, "Find Anagram Mappings", "hash", ctx) {
  Solution sol;

  ctx.test("leetcode_example_unique_elements", [&] {
    const std::vector<int> a = {12, 28, 46, 32, 50};
    const std::vector<int> b = {50, 12, 32, 46, 28};
    const std::vector<int> expected = {1, 4, 3, 2, 0};
    ctx.expect_eq(sol.anagramMappings(a, b), expected);
  });

  ctx.test("duplicates_from_build_produce_bijection", [&] {
    const std::vector<int> a = {50, 12, 28, 46, 32, 50};
    const std::vector<int> b = {50, 12, 32, 46, 28, 50};
    const std::vector<int> result = sol.anagramMappings(a, b);
    ctx.expect_true(isValidBijectiveMapping(a, b, result));
  });

  ctx.test("all_identical_elements", [&] {
    const std::vector<int> a = {7, 7, 7};
    const std::vector<int> b = {7, 7, 7};
    const std::vector<int> result = sol.anagramMappings(a, b);
    ctx.expect_true(isValidBijectiveMapping(a, b, result));
  });

  ctx.test("single_and_empty_inputs", [&] {
    ctx.expect_eq(sol.anagramMappings({42}, {42}), std::vector<int>{0});
    ctx.expect_eq(sol.anagramMappings({}, {}), std::vector<int>{});
  });
}

}  // namespace
