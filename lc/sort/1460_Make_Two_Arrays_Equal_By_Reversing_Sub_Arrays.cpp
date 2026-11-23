// LeetCode 1460: Make Two Arrays Equal By Reversing Sub Arrays
// https://leetcode.com/problems/make-two-arrays-equal-by-reversing-sub-arrays/
//
// Problem:
//   Given two integer arrays `target` and `arr` of equal length, you may select
//   any non-empty contiguous subarray of `arr` and reverse it as many times as
//   desired. Return `true` if `arr` can be transformed into `target`, and
//   `false` otherwise.
//   Constraints: target.length == arr.length, 1 <= length <= 1000,
//   1 <= target[i], arr[i] <= 1000.
//
// Examples:
//   Example 1:
//     Input:  target = [1, 2, 3, 4], arr = [2, 4, 1, 3]
//     Output: true
//     Explanation:
//       - Verify target.size() == arr.size() == 4, then build counts = {1: 1, 2: 1,
//         3: 1, 4: 1} from target.
//       - Scan arr = [2, 4, 1, 3], finding each value in counts with positive
//         remaining count and decrementing counts[2], counts[4], counts[1], and
//         counts[3] from 1 to 0.
//       - Finish scanning arr without encountering a missing or exhausted entry and
//         return true.
//
//   Example 2:
//     Input:  target = [3, 7, 9], arr = [3, 7, 11]
//     Output: false
//     Explanation:
//       - Confirm both arrays have length 3 and populate counts = {3: 1, 7: 1, 9:
//         1} from target.
//       - Match arr[0] = 3 (decrementing counts[3] to 0) and arr[1] = 7
//         (decrementing counts[7] to 0).
//       - Look up arr[2] = 11 in counts, observe it == counts.end(), and return
//         false immediately.
//
//   Example 3:
//     Input:  target = [1, 2, 2], arr = [1, 1, 2]
//     Output: false
//     Explanation:
//       - Confirm both arrays have length 3 and tally target into counts = {1: 1,
//         2: 2}.
//       - Match arr[0] = 1, decrementing counts[1] from 1 to 0.
//       - Check arr[1] = 1, find it->second == 0 (no remaining budget for 1), and
//         return false.
//
// Algorithm:
//   Reversing a 2-element subarray is an adjacent swap, and adjacent swaps
//   generate every permutation (as in bubble sort). Conversely, reversing a
//   subarray never alters element multiplicities. Therefore, `arr` can be
//   transformed into `target` if and only if both arrays represent the exact
//   same multiset of values.
//   Count occurrences of every integer in `target` using a hash map, then
//   decrement counts while scanning `arr`. If any element in `arr` is missing
//   or has already exhausted its count, return `false`. Because the two arrays
//   have equal length and no count ever drops below zero, reaching the end of
//   `arr` guarantees identical multisets.
//   Time Complexity:  O(n) average time.
//   Space Complexity: O(n) for the frequency hash map.
//
// Data Structures:
//   - `std::unordered_map<int, int>`: multiset frequency counter mapping each
//     value to its remaining unmatched occurrences in `target`.
//
// Real-World Applications:
//   - Multiset equivalence checking in database query engines (verifying that
//     an unordered result set matches expected rows regardless of scan order).
//   - Genome rearrangement analysis (checking whether two chromosomal marker
//     sequences have identical gene content before computing reversal distance).
//   - Inventory / ledger reconciliation verifying that two transaction streams
//     contain the same items with matching counts.
//
// Related LeetCode Problems:
//   - #242 Valid Anagram [done] (multiset equality test on character strings)
//   - #969 Pancake Sorting [done] (constructively sorts an array using prefix reversals)
//   - #1657 Determine If Two Strings Are Close [new] (character-frequency equivalence under swaps and relabeling)
//   - #1331 Rank Transform Of An Array [new] (order-based array equivalence)

#include "framework/lc_framework.h"

#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Returns true iff `arr` is a permutation of `target` (equivalently, can be
  // transformed into `target` by a sequence of subarray reversals).
  // Time: O(n) average, Space: O(n).
  bool canBeEqual(const std::vector<int>& target, const std::vector<int>& arr) {
    if (target.size() != arr.size()) {
      return false;
    }

    // Build frequency counts from `target`.
    std::unordered_map<int, int> counts;
    counts.reserve(target.size());
    for (const int val : target) {
      ++counts[val];
    }

    // Match every element of `arr` against `target`'s remaining budget.
    for (const int val : arr) {
      const auto it = counts.find(val);
      if (it == counts.end() || it->second == 0) {
        return false;
      }
      --it->second;
    }

    // Equal lengths + non-negative remaining counts implies all counts are 0.
    return true;
  }
};

LC_PROBLEM(1460, "Make Two Arrays Equal By Reversing Sub Arrays", "sort", ctx) {
  Solution sol;

  ctx.test("example1_convertible_permutation", [&] {
    ctx.expect_true(sol.canBeEqual({1, 2, 3, 4}, {2, 4, 1, 3}));
  });

  ctx.test("example2_single_element", [&] {
    ctx.expect_true(sol.canBeEqual({7}, {7}));
  });

  ctx.test("example3_two_elements_reversed", [&] {
    ctx.expect_true(sol.canBeEqual({1, 12}, {12, 1}));
  });

  ctx.test("example4_mismatched_value", [&] {
    ctx.expect_false(sol.canBeEqual({3, 7, 9}, {3, 7, 11}));
  });

  ctx.test("example5_all_duplicates", [&] {
    ctx.expect_true(sol.canBeEqual({1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}));
  });

  ctx.test("same_set_different_multiplicities", [&] {
    ctx.expect_false(sol.canBeEqual({1, 2, 2}, {1, 1, 2}));
  });
}

}  // namespace
