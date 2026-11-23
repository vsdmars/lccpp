// LeetCode 31: Next Permutation
// https://leetcode.com/problems/next-permutation/
//
// Problem:
//   Given an array of integers `nums`, rearrange its elements in-place into
//   the lexicographically next greater permutation. If no greater permutation
//   exists (i.e., the array is already sorted in non-increasing order),
//   rearrange `nums` into the lowest possible order (sorted ascending). The
//   transformation must use only O(1) extra memory.
//
// Algorithm:
//   Scan from right to left to find the rightmost "pivot" index `pivot` such
//   that `nums[pivot] < nums[pivot + 1]`. Every element strictly to the right
//   of `pivot` forms a non-increasing suffix, which is already at its largest
//   possible permutation.
//   - If no such pivot exists, the entire array is non-increasing; reverse the
//     whole array to wrap around to the lexicographically smallest permutation.
//   - Otherwise, reverse the non-increasing suffix `nums[pivot + 1 .. n - 1]`
//     in-place so it becomes sorted ascending, then binary-search that suffix
//     with `std::ranges::upper_bound` to locate the smallest element strictly
//     greater than `nums[pivot]`, and swap it with `nums[pivot]`. Because we
//     replace the first element strictly greater than `nums[pivot]` with
//     `nums[pivot]`, the suffix remains sorted in ascending order, yielding the
//     exact next lexicographical permutation.
//   Time Complexity: O(n) worst-case (linear scan + linear suffix reversal +
//   O(log n) binary search).
//   Space Complexity: O(1) auxiliary space (in-place mutations only).
//
// Data Structures:
//   In-place `std::vector<int>` manipulated via indices and random-access
//   iterators (`std::ranges::reverse`, `std::ranges::upper_bound`).
//
// Real-World Applications:
//   Combinatorial search space enumeration without recursion stack overhead,
//   exhaustive test-case generation for ordering/scheduling engines,
//   cryptographic key/anagram iteration, and branch-and-bound solvers.
//
// Related LeetCode Problems:
//   - #46 Permutations [done] (enumerates all permutations of distinct numbers)
//   - #47 Permutations II [done] (enumerates unique permutations with duplicates)
//   - #60 Permutation Sequence [new] (directly computes the k-th permutation)
//   - #556 Next Greater Element III [done] (applies next permutation to decimal digits)
//   - #1850 Minimum Adjacent Swaps to Reach the Kth Smallest Number [new] (repeated next-permutation steps)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Rearranges `nums` in-place into its lexicographically next greater
  // permutation, or wraps around to ascending order if already maximal.
  //
  // Invariant: Once `pivot` is found, `nums[pivot + 1 .. n - 1]` is guaranteed
  // to be non-increasing. Reversing it makes it ascending, and swapping
  // `nums[pivot]` with the first element strictly greater than `nums[pivot]`
  // preserves ascending order in the suffix while minimally increasing the
  // prefix at `pivot`.
  //
  // Time: O(n), Space: O(1).
  void nextPermutation(std::vector<int>& nums) {
    const std::size_t n = nums.size();
    if (n <= 1uz) {
      return;
    }

    // Phase 1: Walk backward to locate the rightmost index where an ascent
    // occurs (`nums[i] < nums[i + 1]`).
    std::size_t suffix_start = n - 1;
    while (suffix_start > 0uz && nums[suffix_start - 1] >= nums[suffix_start]) {
      --suffix_start;
    }

    // Reverse the non-increasing suffix so it becomes sorted in ascending
    // order. If `suffix_start == 0`, the entire array was non-increasing and
    // this reversal alone completes the wrap-around to the smallest order.
    const auto suffix_begin =
        nums.begin() + static_cast<std::ptrdiff_t>(suffix_start);
    std::ranges::reverse(suffix_begin, nums.end());

    if (suffix_start == 0uz) {
      return;
    }

    // Phase 2: Binary-search the now-ascending suffix for the first element
    // strictly greater than the pivot (`nums[suffix_start - 1]`) and swap.
    const std::size_t pivot = suffix_start - 1;
    const auto swap_it =
        std::ranges::upper_bound(suffix_begin, nums.end(), nums[pivot]);
    std::swap(nums[pivot], *swap_it);
  }
};

LC_PROBLEM(31, "Next Permutation", "binary_search", ctx) {
  Solution sol;

  ctx.test("example_1_basic_ascending", [&] {
    std::vector<int> nums = {1, 2, 3};
    sol.nextPermutation(nums);
    ctx.expect_eq(nums, std::vector<int>{1, 3, 2});
  });

  ctx.test("example_2_wrap_around", [&] {
    std::vector<int> nums = {3, 2, 1};
    sol.nextPermutation(nums);
    ctx.expect_eq(nums, std::vector<int>{1, 2, 3});
  });

  ctx.test("example_3_duplicates", [&] {
    std::vector<int> nums = {1, 1, 5};
    sol.nextPermutation(nums);
    ctx.expect_eq(nums, std::vector<int>{1, 5, 1});
  });

  ctx.test("build_cases_with_suffixes_and_duplicates", [&] {
    std::vector<int> a = {5, 4, 7, 5, 3, 2};
    sol.nextPermutation(a);
    ctx.expect_eq(a, std::vector<int>{5, 5, 2, 3, 4, 7});

    std::vector<int> b = {9, 8, 7, 9, 6};
    sol.nextPermutation(b);
    ctx.expect_eq(b, std::vector<int>{9, 8, 9, 6, 7});

    std::vector<int> c = {2, 9, 3, 2, 1};
    sol.nextPermutation(c);
    ctx.expect_eq(c, std::vector<int>{3, 1, 2, 2, 9});

    std::vector<int> d = {3, 4, 9, 5, 2, 1};
    sol.nextPermutation(d);
    ctx.expect_eq(d, std::vector<int>{3, 5, 1, 2, 4, 9});

    std::vector<int> e = {9, 2, 1};
    sol.nextPermutation(e);
    ctx.expect_eq(e, std::vector<int>{1, 2, 9});
  });

  ctx.test("edge_cases_small_and_uniform", [&] {
    std::vector<int> empty;
    sol.nextPermutation(empty);
    ctx.expect_eq(empty, std::vector<int>{});

    std::vector<int> single = {42};
    sol.nextPermutation(single);
    ctx.expect_eq(single, std::vector<int>{42});

    std::vector<int> uniform = {2, 2, 2};
    sol.nextPermutation(uniform);
    ctx.expect_eq(uniform, std::vector<int>{2, 2, 2});

    std::vector<int> pair_asc = {1, 2};
    sol.nextPermutation(pair_asc);
    ctx.expect_eq(pair_asc, std::vector<int>{2, 1});

    std::vector<int> pair_desc = {2, 1};
    sol.nextPermutation(pair_desc);
    ctx.expect_eq(pair_desc, std::vector<int>{1, 2});
  });
}

}  // namespace
