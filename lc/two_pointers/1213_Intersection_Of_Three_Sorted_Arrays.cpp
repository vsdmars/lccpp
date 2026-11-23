// LeetCode 1213: Intersection Of Three Sorted Arrays
// https://leetcode.com/problems/intersection-of-three-sorted-arrays/
//
// Problem:
//   Given three integer arrays `arr1`, `arr2`, and `arr3`, each sorted in
//   strictly increasing order, return a sorted array containing only the
//   integers that appear in all three arrays.
//   Constraints:
//     - 1 <= arr1.length, arr2.length, arr3.length <= 1000
//     - 1 <= arr1[i], arr2[i], arr3[i] <= 2000
//
// Algorithm:
//   Use three synchronized pointers (i, j, k) initialized to the start of
//   arr1, arr2, and arr3 respectively. Because every array is strictly
//   increasing, at each step:
//     1. If arr1[i] == arr2[j] == arr3[k], the value is present in all three
//        arrays; record it and advance all three pointers.
//     2. Otherwise, let `target = max(arr1[i], arr2[j], arr3[k])`. Any pointer
//        whose current element is strictly less than `target` cannot possibly
//        match `target` (or any larger future element) at its current index,
//        so advance every pointer whose element is `< target`.
//   The loop terminates as soon as any array is exhausted, since no remaining
//   element can appear in all three arrays.
//   Time Complexity:  O(n1 + n2 + n3), where n1, n2, n3 are the array lengths.
//   Space Complexity: O(1) auxiliary space (excluding the output vector).
//
//   An alternative hash-set approach (also included as `arraysIntersectionHash`)
//   inserts arr2 and arr3 into hash sets and scans arr1 in sorted order in
//   O(n1 + n2 + n3) average time and O(n2 + n3) extra space.
//
// Data Structures:
//   - Three `std::size_t` index pointers for single-pass synchronized scanning
//     without extra storage.
//   - `std::vector<int>` to accumulate the sorted intersection.
//   - `std::unordered_set<int>` (in the alternative hash-set solution) for O(1)
//     average membership queries.
//
// Real-World Applications:
//   - Search engine inverted-index posting list intersection (multi-term AND
//     queries over sorted document ID lists).
//   - Database merge-join operators across multiple sorted index scans.
//   - Log correlation and time-series alignment across distributed services
//     producing sorted timestamp streams.
//
// Related LeetCode Problems:
//   - #349 Intersection of Two Arrays [done] (two-array unique intersection)
//   - #350 Intersection of Two Arrays II [done] (intersection with multiplicity)
//   - #2248 Intersection of Multiple Arrays [new] (generalized to k arrays)
//   - #1868 Product of Two Run-Length Encoded Arrays [done] (synchronized pointer advancement over sorted/ordered runs)
//   - #21 Merge Two Sorted Lists [done] (classic multi-pointer merge traversal)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <unordered_set>
#include <vector>

namespace {

class Solution {
 public:
  // Finds all integers common to three strictly increasing arrays using three
  // synchronized pointers.
  //
  // Invariant: Every common element smaller than max(arr1[i], arr2[j], arr3[k])
  // has already been emitted to `result`.
  // Time: O(n1 + n2 + n3), Space: O(1) auxiliary.
  std::vector<int> arraysIntersection(const std::vector<int>& arr1,
                                      const std::vector<int>& arr2,
                                      const std::vector<int>& arr3) {
    std::size_t i = 0uz;
    std::size_t j = 0uz;
    std::size_t k = 0uz;
    std::vector<int> result;

    // Reserve at most the length of the shortest input array to reduce
    // reallocations while keeping memory bounded.
    result.reserve(std::min({arr1.size(), arr2.size(), arr3.size()}));

    while (i < arr1.size() && j < arr2.size() && k < arr3.size()) {
      const int v1 = arr1[i];
      const int v2 = arr2[j];
      const int v3 = arr3[k];

      if (v1 == v2 && v2 == v3) {
        // Common element found; since inputs are strictly increasing, no
        // duplicates exist and all three pointers must advance together.
        result.push_back(v1);
        ++i;
        ++j;
        ++k;
        continue;
      }

      // Advance every lagging pointer toward the current maximum value.
      const int target = std::max({v1, v2, v3});
      if (v1 < target) {
        ++i;
      }
      if (v2 < target) {
        ++j;
      }
      if (v3 < target) {
        ++k;
      }
    }

    return result;
  }

  // Hash-set alternative: builds membership sets for arr2 and arr3 and filters
  // arr1 in order (preserving sortedness without re-sorting).
  // Time: O(n1 + n2 + n3) average, Space: O(n2 + n3).
  std::vector<int> arraysIntersectionHash(const std::vector<int>& arr1,
                                          const std::vector<int>& arr2,
                                          const std::vector<int>& arr3) {
    const std::unordered_set<int> set2(arr2.begin(), arr2.end());
    const std::unordered_set<int> set3(arr3.begin(), arr3.end());
    std::vector<int> result;

    for (const int num : arr1) {
      if (set2.contains(num) && set3.contains(num)) {
        result.push_back(num);
      }
    }
    return result;
  }
};

LC_PROBLEM(1213, "Intersection Of Three Sorted Arrays", "two_pointers", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    const std::vector<int> arr1 = {1, 2, 3, 4, 5};
    const std::vector<int> arr2 = {1, 2, 5, 7, 9};
    const std::vector<int> arr3 = {1, 3, 4, 5, 8};
    const std::vector<int> expected = {1, 5};
    ctx.expect_eq(sol.arraysIntersection(arr1, arr2, arr3), expected);
    ctx.expect_eq(sol.arraysIntersectionHash(arr1, arr2, arr3), expected);
  });

  ctx.test("python_build_case", [&] {
    const std::vector<int> arr1 = {6, 9, 100};
    const std::vector<int> arr2 = {1, 3, 6, 9};
    const std::vector<int> arr3 = {9, 10, 11};
    const std::vector<int> expected = {9};
    ctx.expect_eq(sol.arraysIntersection(arr1, arr2, arr3), expected);
    ctx.expect_eq(sol.arraysIntersectionHash(arr1, arr2, arr3), expected);
  });

  ctx.test("no_common_elements", [&] {
    const std::vector<int> arr1 = {1, 4, 7};
    const std::vector<int> arr2 = {2, 5, 8};
    const std::vector<int> arr3 = {3, 6, 9};
    const std::vector<int> expected = {};
    ctx.expect_eq(sol.arraysIntersection(arr1, arr2, arr3), expected);
    ctx.expect_eq(sol.arraysIntersectionHash(arr1, arr2, arr3), expected);
  });

  ctx.test("identical_arrays", [&] {
    const std::vector<int> arr = {2, 4, 6, 8, 10};
    ctx.expect_eq(sol.arraysIntersection(arr, arr, arr), arr);
    ctx.expect_eq(sol.arraysIntersectionHash(arr, arr, arr), arr);
  });

  ctx.test("single_element_match_and_mismatch", [&] {
    ctx.expect_eq(sol.arraysIntersection({42}, {42}, {42}),
                  std::vector<int>{42});
    ctx.expect_eq(sol.arraysIntersection({42}, {42}, {43}), std::vector<int>{});
  });

  ctx.test("empty_input_guard", [&] {
    ctx.expect_eq(sol.arraysIntersection({}, {1, 2}, {1, 2}),
                  std::vector<int>{});
  });
}

}  // namespace
