// LeetCode 565: Array Nesting
// https://leetcode.com/problems/array-nesting/
//
// Problem:
//   Given a zero-indexed integer array `nums` of length `n` that is a
//   permutation of the integers `0..n-1`, define for each starting index `i`
//   the set `S[i] = {nums[i], nums[nums[i]], nums[nums[nums[i]]], ...}`
//   constructed by following index pointers until right before a duplicate
//   element would occur. Return the maximum size of `S[i]` over all `i`.
//
// Examples:
//   Example 1:
//     Input:  nums = [5, 4, 0, 3, 1, 6, 2]
//     Output: 4
//     Explanation:
//       - Starting at i = 0, follow 0 -> 5 -> 6 -> 2 -> 0 while setting those slots
//         to -1, recording cycle_len = 4 and max_len = 4.
//       - At i = 1, follow 1 -> 4 -> 1 while setting slots 1 and 4 to -1, yielding
//         cycle_len = 2 and keeping max_len = 4.
//       - Skip i = 2 because nums[2] == -1, then at i = 3 traverse self-loop 3 -> 3
//         (cycle_len = 1) and skip remaining -1 slots at indices 4, 5, and 6 to
//         return 4.
//
//   Example 2:
//     Input:  nums = [1, 0, 3, 4, 2]
//     Output: 3
//     Explanation:
//       - Starting at i = 0, follow cycle 0 -> 1 -> 0, marking indices 0 and 1 as
//         -1 and setting max_len = 2.
//       - Skip i = 1 since nums[1] == -1, then at i = 2 follow cycle 2 -> 3 -> 4 ->
//         2, marking indices 2, 3, and 4 as -1 and updating max_len = 3.
//       - Skip visited indices 3 and 4, then return max_len = 3.
//
//   Example 3:
//     Input:  nums = [0, 1, 2]
//     Output: 1
//     Explanation:
//       - At i = 0, traverse self-loop 0 -> 0, mark nums[0] = -1, and update
//         max_len = 1.
//       - At i = 1 and i = 2, each index forms a self-loop of length 1 and is
//         marked -1, leaving max_len = 1 to be returned.
//
// Algorithm:
//   Because `nums` is a permutation of `[0, n - 1]`, the directed graph
//   `i -> nums[i]` has in-degree 1 and out-degree 1 at every vertex, so it
//   decomposes completely into vertex-disjoint directed cycles. Starting at any
//   vertex in a cycle visits every vertex of that cycle and returns to the
//   start, so every index in a cycle yields the exact same set size (the cycle
//   length).
//
//   Scan every index `i` from `0` to `n - 1`:
//     - If `nums[i] < 0`, index `i` was already visited as part of an earlier
//       cycle; skip it.
//     - Otherwise, follow `cur = nums[cur]` until reaching a visited cell,
//       marking each traversed cell with `-1` in place and counting steps.
//     - Update `max_len` with the discovered cycle length.
//
//   Since each element is marked `-1` exactly once, the total number of
//   pointer hops across the entire run is `n`.
//
//   Time Complexity:  O(n) — each vertex is visited and marked once.
//   Space Complexity: O(1) — visited state is stored in place via `-1`.
//
// Data Structures:
//   - Input `std::vector<int>` (`nums`): simultaneously represents the
//     permutation functional graph (`cur -> nums[cur]`) and an in-place
//     visited marker (`-1` for visited vertices).
//
// Real-World Applications:
//   - Cycle decomposition of permutations for in-place matrix transposition
//     and tensor reshapes without auxiliary buffers.
//   - Computing the order / longest orbit of substitution boxes (S-boxes) and
//     permutation ciphers in cryptography.
//   - Analyzing cycle lengths in memory-remapping tables and pointer-chasing
//     garbage-collection benchmarks.
//
// Related LeetCode Problems:
//   - #141 Linked List Cycle [done] (cycle detection in functional pointer chains)
//   - #287 Find the Duplicate Number [done] (cycle finding in an index-pointer array)
//   - #442 Find All Duplicates In An Array [done] (in-place negative marking on bounded integer arrays)
//   - #2360 Longest Cycle in a Graph [new] (longest cycle in a general functional graph with out-degree <= 1)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the length of the longest cycle in the permutation `nums`.
  //
  // Invariant: Every index `k` with `nums[k] == -1` belongs to a disjoint
  // cycle whose exact length has already been folded into `max_len`.
  //
  // Time: O(n), Space: O(1) auxiliary (in-place marking).
  int arrayNesting(std::vector<int>& nums) {
    int max_len = 0;

    for (std::size_t i = 0uz; i < nums.size(); ++i) {
      // Skip nodes belonging to a cycle we have already measured.
      if (nums[i] < 0) {
        continue;
      }

      int cycle_len = 0;
      std::size_t cur = i;

      // Traverse the full disjoint cycle starting at `i`, marking each slot
      // with -1 so it is never traversed again.
      while (nums[cur] >= 0) {
        const auto next = static_cast<std::size_t>(nums[cur]);
        nums[cur] = -1;
        cur = next;
        ++cycle_len;
      }

      max_len = std::max(max_len, cycle_len);
    }

    return max_len;
  }
};

LC_PROBLEM(565, "Array Nesting", "array", ctx) {
  ctx.test("example_1_standard", [&] {
    Solution sol;
    std::vector<int> nums = {5, 4, 0, 3, 1, 6, 2};
    ctx.expect_eq(sol.arrayNesting(nums), 4);
  });

  ctx.test("example_2_all_self_loops", [&] {
    Solution sol;
    std::vector<int> nums = {0, 1, 2};
    ctx.expect_eq(sol.arrayNesting(nums), 1);
  });

  ctx.test("single_full_cycle", [&] {
    Solution sol;
    std::vector<int> nums = {1, 2, 3, 4, 0};
    ctx.expect_eq(sol.arrayNesting(nums), 5);
  });

  ctx.test("two_disjoint_cycles", [&] {
    Solution sol;
    std::vector<int> nums = {1, 0, 3, 4, 2};
    ctx.expect_eq(sol.arrayNesting(nums), 3);
  });

  ctx.test("single_element_and_empty", [&] {
    Solution sol;
    std::vector<int> single = {0};
    std::vector<int> empty = {};
    ctx.expect_eq(sol.arrayNesting(single), 1);
    ctx.expect_eq(sol.arrayNesting(empty), 0);
  });
}

}  // namespace
