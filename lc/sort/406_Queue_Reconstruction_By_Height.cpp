// LeetCode 406: Queue Reconstruction By Height
// https://leetcode.com/problems/queue-reconstruction-by-height/
//
// Problem:
//   You are given an array `people`, where each element `people[i] = [h_i, k_i]`
//   describes a person of height `h_i` who has exactly `k_i` other people
//   standing in front of them with height greater than or equal to `h_i`.
//   Reconstruct and return the queue so that every person's `(h_i, k_i)`
//   constraint is satisfied. The input is guaranteed to represent a valid queue
//   with `1 <= people.length <= 2000`.
//
// Algorithm:
//   Greedy tallest-first sorting with index insertion:
//   1. Sort `people` in descending order of height `h`; break ties in ascending
//      order of `k`.
//   2. Insert each person `(h, k)` into the result queue at index `k`.
//   Invariant:
//     When placing `(h, k)`, every person already in the queue has height >= h
//     (either strictly taller, or equal height with a smaller `k` value). Thus
//     inserting at index `k` puts exactly `k` people of height >= h in front of
//     this person. Every person inserted afterwards is either strictly shorter
//     (invisible to `(h, k)`) or has the same height with a larger `k` (placed
//     strictly behind `(h, k)`), so existing constraints are never violated.
//   Time Complexity:  O(n^2) — O(n log n) sort plus n vector insertions of O(n).
//   Space Complexity: O(n) for the reconstructed queue (plus O(log n) sort stack).
//
// Data Structures:
//   - Dynamic array (`std::vector<std::vector<int>>`): stores the sorted input
//     and supports positional insertion to build the reconstructed queue.
//
// Real-World Applications:
//   - Priority-constrained task scheduling where higher-priority jobs establish
//     fixed slots and lower-priority jobs fill relative offsets without
//     disturbing higher-priority ordering invariants.
//   - UI/layout z-order or ranked feed reconstruction from partial relative
//     visibility constraints (e.g., sponsored vs. organic item placement).
//   - Reconstructing partial-order rankings from pairwise/prefix dominance
//     counts in tournament and recommender systems.
//
// Related LeetCode Problems:
//   - #315 Count of Smaller Numbers After Self [done] (inversion / relative-rank counting with Fenwick tree or merge sort)
//   - #135 Candy [done] (greedy neighbor-constrained sequence reconstruction)
//   - #452 Minimum Number of Arrows to Burst Balloons [done] (greedy sorting by key before sequential placement)
//   - #1029 Two City Scheduling [done] (greedy sorting on a custom comparator)
//   - #2418 Sort the People [new] (sorting people records by height descending)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Reconstructs the queue by placing taller people first and inserting each
  // person at index `k`.
  //
  // Time: O(n^2), Space: O(n).
  std::vector<std::vector<int>> reconstructQueue(
      std::vector<std::vector<int>> people) {
    // Phase 1: Sort descending by height `h`, and ascending by `k` on ties.
    // Sorting tallest-first ensures that when we process `(h, k)`, all people
    // already placed in `result` have height >= h.
    std::ranges::sort(people, [](const std::vector<int>& lhs,
                                 const std::vector<int>& rhs) {
      if (lhs[0] != rhs[0]) {
        return lhs[0] > rhs[0];
      }
      return lhs[1] < rhs[1];
    });

    // Phase 2: Greedily insert each person at their required rank index `k`.
    // Later insertions of shorter people can push taller people rightward
    // without changing how many people >= h stand in front of any taller person.
    std::vector<std::vector<int>> result;
    result.reserve(people.size());
    for (const auto& person : people) {
      // Clamp defensively in case a caller passes an out-of-range `k`; for all
      // valid LeetCode inputs, `person[1] <= result.size()` always holds.
      const std::size_t insert_idx =
          std::min(static_cast<std::size_t>(person[1]), result.size());
      result.insert(result.begin() + static_cast<std::ptrdiff_t>(insert_idx),
                    person);
    }

    return result;
  }
};

LC_PROBLEM(406, "Queue Reconstruction By Height", "sort", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    const std::vector<std::vector<int>> people = {
        {7, 0}, {4, 4}, {7, 1}, {5, 0}, {6, 1}, {5, 2}};
    const std::vector<std::vector<int>> expected = {
        {5, 0}, {7, 0}, {5, 2}, {6, 1}, {4, 4}, {7, 1}};
    ctx.expect_eq(sol.reconstructQueue(people), expected);
  });

  ctx.test("example_2", [&] {
    const std::vector<std::vector<int>> people = {
        {6, 0}, {5, 0}, {4, 0}, {3, 2}, {2, 2}, {1, 4}};
    const std::vector<std::vector<int>> expected = {
        {4, 0}, {5, 0}, {2, 2}, {3, 2}, {1, 4}, {6, 0}};
    ctx.expect_eq(sol.reconstructQueue(people), expected);
  });

  ctx.test("single_person", [&] {
    const std::vector<std::vector<int>> people = {{5, 0}};
    const std::vector<std::vector<int>> expected = {{5, 0}};
    ctx.expect_eq(sol.reconstructQueue(people), expected);
  });

  ctx.test("all_same_height", [&] {
    const std::vector<std::vector<int>> people = {{5, 2}, {5, 0}, {5, 1}};
    const std::vector<std::vector<int>> expected = {{5, 0}, {5, 1}, {5, 2}};
    ctx.expect_eq(sol.reconstructQueue(people), expected);
  });

  ctx.test("strictly_ascending_heights_zero_k", [&] {
    const std::vector<std::vector<int>> people = {{3, 0}, {1, 0}, {2, 0}};
    const std::vector<std::vector<int>> expected = {{1, 0}, {2, 0}, {3, 0}};
    ctx.expect_eq(sol.reconstructQueue(people), expected);
  });

  ctx.test("empty_input", [&] {
    const std::vector<std::vector<int>> people = {};
    const std::vector<std::vector<int>> expected = {};
    ctx.expect_eq(sol.reconstructQueue(people), expected);
  });
}

}  // namespace
