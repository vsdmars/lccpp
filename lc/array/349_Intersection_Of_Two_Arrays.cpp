// LeetCode 349: Intersection Of Two Arrays
// https://leetcode.com/problems/intersection-of-two-arrays/
//
// Problem:
//   Given two integer arrays `nums1` and `nums2`, return an array of their
//   set intersection. Every element in the result must be unique, and elements
//   may be returned in any order.
//
// Examples:
//   Example 1:
//     Input:  nums1 = [1, 2, 2, 1], nums2 = [2, 2]
//     Output: [2]
//     Explanation:
//       - Because nums2 (size 2) is smaller than nums1 (size 4), insert nums2 into
//         hash set seen = {2} and scan nums1.
//       - For the first element 1, seen.erase(1) returns 0, so nothing is appended.
//       - For the second element 2, seen.erase(2) removes 2 and returns 1,
//         appending 2 to result and emptying seen.
//       - For the remaining elements 2 and 1, seen.erase returns 0 because seen is
//         empty, so return [2].
//
//   Example 2:
//     Input:  nums1 = [4, 9, 5], nums2 = [9, 4, 9, 8, 4]
//     Output: [9, 4]
//     Explanation:
//       - Because nums1 (size 3) is smaller than nums2 (size 5), build seen = {4,
//         5, 9} from nums1 and scan nums2.
//       - At 9, seen.erase(9) succeeds, appending 9 to result and shrinking seen to
//         {4, 5}.
//       - At 4, seen.erase(4) succeeds, appending 4 to result and shrinking seen to
//         {5}.
//       - The remaining values 9, 8, and 4 are absent from seen, so return [9, 4]
//         (equivalent to [4, 9] since any order is accepted).
//
//   Example 3:
//     Input:  nums1 = [1, 3, 5], nums2 = [2, 4, 6]
//     Output: []
//     Explanation:
//       - Since both arrays have size 3, choose nums1 as smaller to build seen =
//         {1, 3, 5}.
//       - Scan nums2 ([2, 4, 6]): seen.erase(2), seen.erase(4), and seen.erase(6)
//         each return 0.
//       - No elements are appended to result, so return [].
//
// Algorithm:
//   Hash-set lookup with erase-on-match deduplication:
//   1. Pick the smaller of the two input arrays and insert all its elements
//      into a hash set `seen` (minimizing memory footprint to O(min(n, m))).
//   2. Scan the larger array: for each element `val`, call `seen.erase(val)`.
//      If `erase` removes an entry (returns 1), append `val` to `result`.
//      Erasing on the first match guarantees each common value is emitted
//      exactly once without allocating a second result set.
//   A secondary sorted two-pointer implementation (`intersectionTwoPointers`)
//   is also provided for the classic follow-up where inputs are sorted or
//   streaming with O(1) extra space.
//   Time Complexity:  O(n + m) expected time for the hash-set method.
//   Space Complexity: O(min(n, m)) auxiliary space.
//
// Data Structures:
//   - `std::unordered_set<int>` (`seen`): O(1) average-case membership lookup
//     and duplicate suppression via destructive erase.
//   - `std::vector<int>` (`result`): stores the unique intersecting elements.
//
// Real-World Applications:
//   - Relational database hash-join and `INTERSECT` query operators (building
//     the hash table on the smaller relation and probing with the larger one).
//   - Search-engine inverted-index posting-list intersection for multi-term
//     boolean `AND` queries.
//   - Social network mutual-friends and audience-segment overlap computation.
//
// Related LeetCode Problems:
//   - #350 Intersection of Two Arrays II [done] (multiset intersection preserving frequency)
//   - #1213 Intersection of Three Sorted Arrays [done] (three-pointer sorted intersection)
//   - #2215 Find the Difference of Two Arrays [new] (symmetric set difference with hash sets)
//   - #2248 Intersection of Multiple Arrays [new] (intersection across k arrays)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <unordered_set>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the unique set intersection of `nums1` and `nums2` using a hash
  // table built over the smaller array.
  //
  // Time Complexity:  O(n + m) average
  // Space Complexity: O(min(n, m))
  std::vector<int> intersection(const std::vector<int>& nums1,
                                const std::vector<int>& nums2) {
    // Build the hash set on the smaller input to minimize memory usage.
    const bool first_is_smaller = nums1.size() <= nums2.size();
    const std::vector<int>& smaller = first_is_smaller ? nums1 : nums2;
    const std::vector<int>& larger = first_is_smaller ? nums2 : nums1;

    std::unordered_set<int> seen(smaller.begin(), smaller.end());
    std::vector<int> result;

    // Erase on first hit so duplicates in `larger` are automatically skipped
    // without needing a second hash set.
    for (const int val : larger) {
      if (seen.erase(val) > 0uz) {
        result.push_back(val);
      }
    }

    return result;
  }

  // Follow-up approach: sorts copies of both arrays and intersects them with
  // two pointers, skipping duplicates in-place.
  //
  // Time Complexity:  O(n log n + m log m)
  // Space Complexity: O(n + m) for local sorted copies (O(1) if inputs are
  //                   already sorted and mutable)
  std::vector<int> intersectionTwoPointers(std::vector<int> nums1,
                                           std::vector<int> nums2) {
    std::ranges::sort(nums1);
    std::ranges::sort(nums2);

    std::vector<int> result;
    std::size_t i = 0uz;
    std::size_t j = 0uz;

    while (i < nums1.size() && j < nums2.size()) {
      if (nums1[i] < nums2[j]) {
        ++i;
      } else if (nums1[i] > nums2[j]) {
        ++j;
      } else {
        if (result.empty() || result.back() != nums1[i]) {
          result.push_back(nums1[i]);
        }
        ++i;
        ++j;
      }
    }

    return result;
  }
};

// Sorts a vector by value so order-independent outputs can be compared cleanly.
std::vector<int> sorted(std::vector<int> values) {
  std::ranges::sort(values);
  return values;
}

LC_PROBLEM(349, "Intersection Of Two Arrays", "array", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    const std::vector<int> a = {1, 2, 2, 1};
    const std::vector<int> b = {2, 2};
    ctx.expect_eq(sorted(sol.intersection(a, b)), std::vector<int>{2});
    ctx.expect_eq(sol.intersectionTwoPointers(a, b), std::vector<int>{2});
  });

  ctx.test("example_2", [&] {
    const std::vector<int> a = {4, 9, 5};
    const std::vector<int> b = {9, 4, 9, 8, 4};
    const std::vector<int> expected = {4, 9};
    ctx.expect_eq(sorted(sol.intersection(a, b)), expected);
    ctx.expect_eq(sol.intersectionTwoPointers(a, b), expected);
  });

  ctx.test("disjoint_arrays", [&] {
    const std::vector<int> a = {1, 3, 5};
    const std::vector<int> b = {2, 4, 6};
    ctx.expect_eq(sol.intersection(a, b), std::vector<int>{});
    ctx.expect_eq(sol.intersectionTwoPointers(a, b), std::vector<int>{});
  });

  ctx.test("empty_and_negatives", [&] {
    ctx.expect_eq(sol.intersection({}, {1, 2}), std::vector<int>{});
    ctx.expect_eq(sol.intersection({1, 2}, {}), std::vector<int>{});

    const std::vector<int> a = {-5, 0, -5, 3, 10};
    const std::vector<int> b = {10, -5, -5, 7};
    const std::vector<int> expected = {-5, 10};
    ctx.expect_eq(sorted(sol.intersection(a, b)), expected);
    ctx.expect_eq(sol.intersectionTwoPointers(a, b), expected);
  });
}

}  // namespace
