// LeetCode 905: Sort Array By Parity
// https://leetcode.com/problems/sort-array-by-parity/
//
// Problem:
//   Given an integer array `nums` of non-negative integers, move all even
//   integers to the beginning of the array followed by all odd integers. Any
//   ordering within the even group and within the odd group is valid.
//   Constraints: 1 <= nums.length <= 5000, 0 <= nums[i] <= 5000.
//
// Examples:
//   Example 1:
//     Input:  nums = [3, 1, 2, 4]
//     Output: [4, 2, 1, 3]
//     Explanation:
//       - Initialize scan = 0 and odd_bound = 4.
//       - At scan = 0, nums[0] = 3 is odd, so decrement odd_bound to 3 and swap
//         nums[0] with nums[3] to get [4, 1, 2, 3] without advancing scan.
//       - At scan = 0, nums[0] = 4 is even, so advance scan to 1.
//       - At scan = 1, nums[1] = 1 is odd, so decrement odd_bound to 2 and swap
//         nums[1] with nums[2] to get [4, 2, 1, 3]; then nums[1] = 2 is even,
//         advancing scan to 2.
//       - Now scan == odd_bound (both 2), ending the loop and returning the valid
//         parity partition [4, 2, 1, 3].
//
//   Example 2:
//     Input:  nums = [9, 7, 5, 3, 2, 4, 6, 8]
//     Output: [8, 6, 4, 2, 3, 5, 7, 9]
//     Explanation:
//       - Initialize scan = 0 and odd_bound = 8.
//       - Each odd element encountered at scan = 0, 1, 2, and 3 (values 9, 7, 5, 3)
//         decrements odd_bound and swaps with the tail (values 8, 6, 4, 2) before
//         rechecking the same scan index.
//       - Each swapped-in even element (8, 6, 4, 2) passes the parity check and
//         increments scan from 0 to 4 while odd_bound shrinks to 4.
//       - The loop terminates when scan == odd_bound == 4 and returns [8, 6, 4, 2,
//         3, 5, 7, 9].
//
//   Example 3:
//     Input:  nums = [1, 3, 5, 7]
//     Output: [3, 5, 7, 1]
//     Explanation:
//       - Initialize scan = 0 and odd_bound = 4.
//       - Because every element is odd, scan stays at 0 across all 4 iterations
//         while odd_bound decrements from 4 down to 0, swapping nums[0] with
//         nums[3], nums[2], nums[1], and finally nums[0].
//       - The loop exits when scan == odd_bound == 0, returning [3, 5, 7, 1] where
//         all odd elements occupy the array.
//
// Algorithm:
//   In-place two-pointer partition (Hoare / Lomuto-style boundary shrinking):
//   - Maintain `scan = 0` and an exclusive `odd_bound = nums.size()` such that:
//       * `nums[0 .. scan - 1]` are all confirmed even.
//       * `nums[odd_bound .. n - 1]` are all confirmed odd.
//       * `nums[scan .. odd_bound - 1]` are unprocessed.
//   - While `scan < odd_bound`:
//       * If `nums[scan]` is odd, decrement `odd_bound` and swap `nums[scan]`
//         with `nums[odd_bound]`. Do NOT advance `scan` because the newly
//         swapped-in element has not been inspected yet.
//       * If `nums[scan]` is even, advance `scan`.
//   Time Complexity:  O(N) — each element is examined at most once and swapped
//                     at most once.
//   Space Complexity: O(1) — partitioned in place.
//
// Data Structures:
//   - Input `std::vector<int>` (`nums`) mutated in place via two boundary
//     indices (`scan`, `odd_bound`).
//
// Real-World Applications:
//   - Quicksort / quickselect in-place binary partitioning around a predicate
//     (`std::partition` in C++ STL).
//   - Garbage-collector compaction and memory-pool defragmentation separating
//     live objects from dead/free slots in O(1) auxiliary space.
//   - Network packet and job queue classification segregating high-priority vs.
//     background items in-place.
//
// Related LeetCode Problems:
//   - #922 Sort Array By Parity II [new] (interleaves evens at even indices and odds at odd indices)
//   - #283 Move Zeroes [done] (in-place two-pointer partition preserving relative order)
//   - #75 Sort Colors [done] (three-way Dutch National Flag in-place partitioning)
//   - #27 Remove Element [done] (shrinking-tail in-place partition dropping matching elements)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

// Test helper verifying that `result` has all evens before all odds and is a
// permutation of `original`.
bool isValidParityPartition(const std::vector<int>& original,
                            const std::vector<int>& result) {
  if (original.size() != result.size()) {
    return false;
  }

  // Verify partition invariant: once an odd value appears, no even may follow.
  bool seen_odd = false;
  for (const int val : result) {
    if (val % 2 != 0) {
      seen_odd = true;
    } else if (seen_odd) {
      return false;
    }
  }

  // Verify exact multiset preservation.
  std::vector<int> sorted_orig = original;
  std::vector<int> sorted_res = result;
  std::ranges::sort(sorted_orig);
  std::ranges::sort(sorted_res);
  return sorted_orig == sorted_res;
}

class Solution {
 public:
  // Partitions `nums` in place so all even integers precede all odd integers.
  // Invariant: `nums[0..scan)` contains only even elements and
  // `nums[odd_bound..n)` contains only odd elements.
  // Time: O(N), Space: O(1).
  std::vector<int> sortArrayByParity(std::vector<int> nums) {
    std::size_t scan = 0uz;
    std::size_t odd_bound = nums.size();

    while (scan < odd_bound) {
      if (nums[scan] % 2 != 0) {
        // Move odd element into the tail partition; keep `scan` unchanged so
        // the element swapped from `odd_bound` is checked on the next pass.
        --odd_bound;
        std::swap(nums[scan], nums[odd_bound]);
        continue;
      }
      ++scan;
    }

    return nums;
  }
};

LC_PROBLEM(905, "Sort Array By Parity", "two_pointers", ctx) {
  Solution sol;

  ctx.test("example_1_mixed", [&] {
    const std::vector<int> input = {3, 1, 2, 4};
    const auto out = sol.sortArrayByParity(input);
    ctx.expect_true(isValidParityPartition(input, out));
  });

  ctx.test("single_zero_and_single_odd", [&] {
    ctx.expect_eq(sol.sortArrayByParity({0}), std::vector<int>{0});
    ctx.expect_eq(sol.sortArrayByParity({7}), std::vector<int>{7});
  });

  ctx.test("all_even_and_all_odd", [&] {
    const std::vector<int> evens = {2, 4, 6, 0};
    ctx.expect_true(
        isValidParityPartition(evens, sol.sortArrayByParity(evens)));

    const std::vector<int> odds = {1, 3, 5, 7};
    ctx.expect_true(isValidParityPartition(odds, sol.sortArrayByParity(odds)));
  });

  ctx.test("already_partitioned_and_reverse_partitioned", [&] {
    const std::vector<int> already = {0, 2, 4, 1, 3, 5};
    ctx.expect_true(
        isValidParityPartition(already, sol.sortArrayByParity(already)));

    const std::vector<int> reversed = {9, 7, 5, 3, 2, 4, 6, 8};
    ctx.expect_true(
        isValidParityPartition(reversed, sol.sortArrayByParity(reversed)));
  });
}

}  // namespace
