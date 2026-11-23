// LeetCode 986: Interval List Intersections
// https://leetcode.com/problems/interval-list-intersections/
//
// Problem:
//   Given two lists of closed intervals `firstList` and `secondList`, where
//   each list is pairwise disjoint and sorted in ascending order, return the
//   list of all non-empty intersections of these two interval lists in sorted
//   order. A closed interval `[a, b]` intersects `[c, d]` iff `max(a, c) <=
//   min(b, d)`, producing `[max(a, c), min(b, d)]`.
//   Constraints: 0 <= firstList.length, secondList.length <= 1000,
//   0 <= start_i <= end_i <= 10^9.
//
// Examples:
//   Example 1:
//     Input:  firstList = [[0, 2], [5, 10], [13, 23], [24, 25]], secondList = [[1,
//             5], [8, 12], [15, 24], [25, 26]]
//     Output: [[1, 2], [5, 5], [8, 10], [15, 23], [24, 24], [25, 25]]
//     Explanation:
//       - Start with i = 0 ([0, 2]) and j = 0 ([1, 5]); lo = max(0, 1) = 1 and hi =
//         min(2, 5) = 2 satisfy lo <= hi, so append [1, 2] and advance i = 1 since
//         2 < 5.
//       - At i = 1 ([5, 10]) and j = 0 ([1, 5]), compute [lo, hi] = [5, 5], append
//         the point intersection [5, 5], and advance j = 1 since 10 < 5 is false.
//       - At i = 1 ([5, 10]) and j = 1 ([8, 12]), append [8, 10] and advance i = 2;
//         then at i = 2 ([13, 23]) and j = 1 ([8, 12]), lo = 13 > hi = 12 produces
//         no overlap and advances j = 2.
//       - At i = 2 ([13, 23]) and j = 2 ([15, 24]), append [15, 23] and advance i =
//         3; at i = 3 ([24, 25]) and j = 2 ([15, 24]), append [24, 24] and advance
//         j = 3.
//       - At i = 3 ([24, 25]) and j = 3 ([25, 26]), append [25, 25] and advance i =
//         4 (25 < 26), ending the loop with all six intersections.
//
//   Example 2:
//     Input:  firstList = [[3, 5], [9, 20]], secondList = [[4, 5], [7, 10], [11,
//             12], [14, 15], [16, 20]]
//     Output: [[4, 5], [9, 10], [11, 12], [14, 15], [16, 20]]
//     Explanation:
//       - At i = 0 ([3, 5]) and j = 0 ([4, 5]), lo = 4 and hi = 5 yield [4, 5];
//         because 5 < 5 is false, advance j = 1.
//       - At i = 0 ([3, 5]) and j = 1 ([7, 10]), lo = 7 > hi = 5 means no overlap,
//         so advance i = 1 since 5 < 10.
//       - While i stays at 1 ([9, 20]), j walks through [7, 10], [11, 12], [14,
//         15], and [16, 20] (each ending at or before 20), appending [9, 10], [11,
//         12], [14, 15], and [16, 20] before j reaches 5 and ends the scan.
//
//   Example 3:
//     Input:  firstList = [[0, 1], [4, 5]], secondList = [[2, 3], [6, 7]]
//     Output: []
//     Explanation:
//       - At i = 0 ([0, 1]) and j = 0 ([2, 3]), lo = max(0, 2) = 2 > hi = min(1, 3)
//         = 1, so nothing is appended and i advances to 1.
//       - At i = 1 ([4, 5]) and j = 0 ([2, 3]), lo = 4 > hi = 3 skips appending and
//         advances j to 1 since 5 < 3 is false.
//       - At i = 1 ([4, 5]) and j = 1 ([6, 7]), lo = 6 > hi = 5 skips appending and
//         advances i to 2, terminating the loop with [].
//
// Algorithm:
//   Two-pointer merge scan ("inchworm" sweep over both sorted lists):
//   - Maintain indices `i = 0` into `firstList` and `j = 0` into `secondList`.
//   - At each step, compute the potential overlap `[lo, hi] = [max(a0, b0),
//     min(a1, b1)]`. If `lo <= hi`, record `[lo, hi]` as an intersection.
//   - Advance whichever interval ends earlier (`a1 < b1 ? ++i : ++j`). That
//     interval cannot intersect any subsequent interval in the other list
//     because the other list is sorted and pairwise disjoint.
//   Time Complexity:  O(M + N), where M = firstList.size() and N = secondList.size().
//   Space Complexity: O(1) auxiliary space (O(M + N) for the returned list).
//
// Data Structures:
//   - Two index pointers (`i`, `j`) over sorted `std::vector<std::vector<int>>`
//     interval lists, appending overlaps into a result vector.
//
// Real-World Applications:
//   - Calendar availability intersection (finding mutual free slots across two
//     users' schedules).
//   - Temporal database range joins and genomic BED interval intersection
//     (finding overlapping exons/peaks across two sorted tracks).
//   - Distributed lock / lease window auditing and network maintenance window
//     overlap calculation.
//
// Related LeetCode Problems:
//   - #56 Merge Intervals [done] (uniting overlapping intervals in one list)
//   - #57 Insert Interval [done] (merging a new interval into a sorted disjoint list)
//   - #253 Meeting Rooms II [done] (sweep-line / interval overlap counting)
//   - #1229 Meeting Scheduler [new] (two-pointer interval intersection with minimum duration)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Computes all closed-interval intersections between two sorted, pairwise
  // disjoint interval lists.
  // Invariant: Every intersection involving any interval before index `i` in
  // `first_list` or before index `j` in `second_list` has already been emitted.
  // Time: O(M + N), Space: O(1) auxiliary.
  std::vector<std::vector<int>> intervalIntersection(
      const std::vector<std::vector<int>>& first_list,
      const std::vector<std::vector<int>>& second_list) {
    std::vector<std::vector<int>> result;
    std::size_t i = 0uz;
    std::size_t j = 0uz;

    while (i < first_list.size() && j < second_list.size()) {
      // Candidate intersection bounds for the current interval pair.
      const int lo = std::max(first_list[i][0], second_list[j][0]);
      const int hi = std::min(first_list[i][1], second_list[j][1]);

      // Closed intervals overlap (including single-point touch) iff lo <= hi.
      if (lo <= hi) {
        result.push_back({lo, hi});
      }

      // Discard whichever interval finishes first, as it can never intersect
      // any later interval in the opposite list.
      if (first_list[i][1] < second_list[j][1]) {
        ++i;
      } else {
        ++j;
      }
    }

    return result;
  }
};

LC_PROBLEM(986, "Interval List Intersections", "two_pointers", ctx) {
  Solution sol;

  ctx.test("example_1_with_point_intersections", [&] {
    const std::vector<std::vector<int>> a = {{0, 2}, {5, 10}, {13, 23}, {24, 25}};
    const std::vector<std::vector<int>> b = {{1, 5}, {8, 12}, {15, 24}, {25, 26}};
    const std::vector<std::vector<int>> expected = {
        {1, 2}, {5, 5}, {8, 10}, {15, 23}, {24, 24}, {25, 25}};
    ctx.expect_eq(sol.intervalIntersection(a, b), expected);
  });

  ctx.test("python_build_case_one_covers_many", [&] {
    const std::vector<std::vector<int>> a = {{3, 5}, {9, 20}};
    const std::vector<std::vector<int>> b = {
        {4, 5}, {7, 10}, {11, 12}, {14, 15}, {16, 20}};
    const std::vector<std::vector<int>> expected = {
        {4, 5}, {9, 10}, {11, 12}, {14, 15}, {16, 20}};
    ctx.expect_eq(sol.intervalIntersection(a, b), expected);
  });

  ctx.test("empty_lists", [&] {
    const std::vector<std::vector<int>> non_empty = {{1, 3}, {5, 9}};
    const std::vector<std::vector<int>> empty;
    ctx.expect_eq(sol.intervalIntersection(non_empty, empty), empty);
    ctx.expect_eq(sol.intervalIntersection(empty, non_empty), empty);
    ctx.expect_eq(sol.intervalIntersection(empty, empty), empty);
  });

  ctx.test("completely_disjoint", [&] {
    const std::vector<std::vector<int>> a = {{0, 1}, {4, 5}};
    const std::vector<std::vector<int>> b = {{2, 3}, {6, 7}};
    ctx.expect_eq(sol.intervalIntersection(a, b),
                  std::vector<std::vector<int>>{});
  });

  ctx.test("identical_lists", [&] {
    const std::vector<std::vector<int>> a = {{1, 4}, {7, 10}};
    ctx.expect_eq(sol.intervalIntersection(a, a), a);
  });
}

}  // namespace
