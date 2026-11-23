// LeetCode 217: Contains Duplicate
// https://leetcode.com/problems/contains-duplicate/
//
// Problem:
//   Given an integer array `nums`, return `true` if any value appears at least
//   twice in the array, and return `false` if every element is distinct.
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 2, 3, 1]
//     Output: true
//     Explanation:
//       - Initialize an empty unordered_set<int> named seen.
//       - Insert 1, 2, and 3 in order; each seen.insert(num).second returns true,
//         leaving seen = {1, 2, 3}.
//       - At the fourth element 1, seen.insert(1).second returns false because 1 is
//         already in seen, so return true immediately.
//
//   Example 2:
//     Input:  nums = [1, 2, 3, 4]
//     Output: false
//     Explanation:
//       - Initialize an empty unordered_set<int> named seen.
//       - Iterate over 1, 2, 3, and 4: every seen.insert(num).second returns true,
//         growing seen to {1, 2, 3, 4}.
//       - Finish the loop without encountering a collision and return false.
//
//   Example 3:
//     Input:  nums = []
//     Output: false
//     Explanation:
//       - Initialize an empty unordered_set<int> named seen.
//       - The range-based loop over nums runs zero iterations because the vector is
//         empty, so return false.
//
// Algorithm:
//   Hash-Set Membership with Early Exit:
//     Iterate through `nums` once, inserting each value into an
//     `std::unordered_set<int>`. `insert(num).second` is `false` if and only
//     if `num` is already present in the set; when that occurs, return `true`
//     immediately without scanning the remainder of the array. If the entire
//     loop finishes without collision, all elements are distinct, so return
//     `false`.
//     (Note: sorting in-place achieves O(n log n) time and O(1) extra space if
//     memory is tightly constrained and mutating the input is acceptable.)
//   Time Complexity:  O(n) average (O(1) best case when an early duplicate
//                     exists).
//   Space Complexity: O(n) worst case when all elements are distinct.
//
// Data Structures:
//   - std::unordered_set<int> seen: provides O(1) average-time membership
//     testing and insertion for previously visited values.
//
// Real-World Applications:
//   - Primary-key / unique-constraint violation checks during bulk ETL imports
//     and database ingestion pipelines.
//   - Request idempotency and event-ID deduplication in distributed message
//     queues and payment processors.
//   - Fast uniqueness validation of configuration identifiers, ports, or
//     dependency lists in build tools.
//
// Related LeetCode Problems:
//   - #219 Contains Duplicate II [done] (sliding-window hash set within index distance k)
//   - #220 Contains Duplicate III [done] (value-bucket / ordered-set proximity duplicate)
//   - #287 Find the Duplicate Number [done] (O(1)-space cycle detection on [1..n])
//   - #442 Find All Duplicates in an Array [done] (in-place sign-marking for all duplicates)
//   - #26 Remove Duplicates from Sorted Array [done] (in-place deduplication on sorted input)

#include "framework/lc_framework.h"

#include <limits>
#include <unordered_set>
#include <vector>

namespace {

class Solution {
 public:
  // Returns true iff at least one value occurs more than once in `nums`.
  // Invariant: `seen` contains the exact set of distinct values visited so far.
  // Time: O(n) average, Space: O(n) worst case.
  bool containsDuplicate(const std::vector<int>& nums) {
    std::unordered_set<int> seen;

    for (const int num : nums) {
      // insert() returns {iterator, inserted}; false signals a duplicate.
      if (!seen.insert(num).second) {
        return true;
      }
    }

    return false;
  }
};

LC_PROBLEM(217, "Contains Duplicate", "array", ctx) {
  Solution sol;

  ctx.test("examples_with_and_without_duplicates", [&] {
    ctx.expect_true(sol.containsDuplicate({1, 2, 3, 1}));
    ctx.expect_false(sol.containsDuplicate({1, 2, 3, 4}));
    ctx.expect_true(sol.containsDuplicate({1, 1, 1, 3, 3, 4, 3, 2, 4, 2}));
  });

  ctx.test("source_build_cases", [&] {
    ctx.expect_false(sol.containsDuplicate({0}));
    ctx.expect_false(sol.containsDuplicate({}));
    ctx.expect_true(sol.containsDuplicate({9, 3, 15, 20, 3, 7}));
  });

  ctx.test("negative_and_extreme_values", [&] {
    constexpr int kMin = std::numeric_limits<int>::min();
    constexpr int kMax = std::numeric_limits<int>::max();

    ctx.expect_false(sol.containsDuplicate({kMin, -1, 0, 1, kMax}));
    ctx.expect_true(sol.containsDuplicate({kMin, 0, kMax, kMin}));
  });
}

}  // namespace
