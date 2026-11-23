// LeetCode 969: Pancake Sorting
// https://leetcode.com/problems/pancake-sorting/
//
// Problem:
//   Given an array `arr` that is a permutation of `[1, 2, ..., n]` (`1 <= n <= 100`),
//   you may perform pancake flips: choose an integer `k` (`1 <= k <= n`) and
//   reverse the prefix `arr[0..k-1]`. Return a sequence of `k`-values that
//   sorts `arr` in ascending order using at most `10 * n` flips.
//
// Algorithm:
//   Selection-style largest-to-back pancake sorting:
//   Place each target value `val` from `n` down to `2` into its final sorted
//   index `val - 1` using at most two flips per element:
//   1. Locate the current index `idx` of `val` within the unsorted prefix
//      `arr[0..val-1]`.
//   2. If `idx == val - 1`, `val` is already in its final position; skip it.
//   3. Otherwise, if `idx > 0`, flip prefix `idx + 1` so `val` moves to `arr[0]`.
//   4. Flip prefix `val` so `val` moves from `arr[0]` to `arr[val - 1]`.
//   Invariant:
//     After placing `val`, the suffix `arr[val - 1 .. n - 1]` contains the
//     values `val .. n` in sorted order and is never touched by subsequent flips
//     (which all have `k < val`). At most `2 * (n - 1)` flips are emitted, well
//     within the `10 * n` budget.
//   Time Complexity:  O(n^2) — `n - 1` iterations, each doing an O(n) linear
//                     search and up to two O(n) prefix reversals.
//   Space Complexity: O(n) for the returned flip sequence (O(1) extra work space).
//
// Data Structures:
//   - Contiguous array (`std::vector<int>`): supports cache-friendly linear
//     scans and in-place prefix reversals via `std::ranges::reverse`.
//
// Real-World Applications:
//   - Genomic rearrangement analysis (sorting by reversals / prefix reversals
//     to estimate evolutionary edit distance between chromosomes).
//   - Interconnection network routing on pancake graphs, where prefix reversals
//     model low-diameter, fault-tolerant processor topologies.
//   - Mechanical/robotic stack sorting where a manipulator can only invert a
//     top sub-stack of items at once (e.g., warehouse pallet or rail-yard
//     reordering).
//
// Related LeetCode Problems:
//   - #189 Rotate Array [done] (array manipulation via prefix/suffix reversals)
//   - #31 Next Permutation [done] (in-place suffix reversal for permutation order)
//   - #912 Sort an Array [new] (general comparison sorting algorithms)
//   - #75 Sort Colors [done] (in-place array partitioning and sorting)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <vector>

namespace {

class Solution {
 public:
  // Returns a sequence of at most `2 * (n - 1)` pancake flips (each `1 <= k <= n`)
  // that sorts permutation `arr` into `[1, 2, ..., n]`.
  //
  // Time: O(n^2), Space: O(n) for the output flip list.
  std::vector<int> pancakeSort(std::vector<int> arr) {
    std::vector<int> flips;
    flips.reserve(arr.size() * 2);

    // Place each value `target` from `n` down to `2` into index `target - 1`.
    // Once `2..n` are placed, `1` is automatically settled at index `0`.
    for (std::size_t target = arr.size(); target > 1; --target) {
      const auto prefix_end = arr.begin() + static_cast<std::ptrdiff_t>(target);
      const auto it =
          std::ranges::find(arr.begin(), prefix_end, static_cast<int>(target));
      const auto idx = static_cast<std::size_t>(std::distance(arr.begin(), it));

      // Skip if `target` is already at its final sorted position `target - 1`.
      if (idx + 1 == target) {
        continue;
      }

      // Step 1: Bring `target` to index 0 (only needed if not already at front).
      if (idx > 0) {
        std::ranges::reverse(arr.begin(),
                             arr.begin() + static_cast<std::ptrdiff_t>(idx + 1));
        flips.push_back(static_cast<int>(idx + 1));
      }

      // Step 2: Flip the entire unsorted prefix `[0..target-1]` to move `target`
      // from index 0 into its final slot `target - 1`.
      std::ranges::reverse(arr.begin(), prefix_end);
      flips.push_back(static_cast<int>(target));
    }

    return flips;
  }
};

// Replays `flips` on a copy of `input` and verifies LeetCode's judge criteria:
// flip count <= 10 * n, every `k` in `[1, n]`, and final array is sorted.
void verify_pancake_sort(lc::TestContext& ctx, std::vector<int> arr,
                         const std::vector<int>& flips) {
  ctx.expect_true(flips.size() <= arr.size() * 10,
                  "exceeded 10 * n pancake flip budget");

  for (const int k : flips) {
    ctx.expect_true(k >= 1 && static_cast<std::size_t>(k) <= arr.size(),
                    "flip length k out of range");
    if (k >= 1 && static_cast<std::size_t>(k) <= arr.size()) {
      std::ranges::reverse(arr.begin(), arr.begin() + k);
    }
  }

  ctx.expect_true(std::ranges::is_sorted(arr),
                  "array is not sorted after applying flips");
}

LC_PROBLEM(969, "Pancake Sorting", "sort", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    const std::vector<int> input = {3, 2, 4, 1};
    const auto flips = sol.pancakeSort(input);
    verify_pancake_sort(ctx, input, flips);
  });

  ctx.test("example_2_already_sorted", [&] {
    const std::vector<int> input = {1, 2, 3};
    const auto flips = sol.pancakeSort(input);
    ctx.expect_true(flips.empty());
    verify_pancake_sort(ctx, input, flips);
  });

  ctx.test("python_build_case", [&] {
    const std::vector<int> input = {4, 2, 3, 1};
    const auto flips = sol.pancakeSort(input);
    verify_pancake_sort(ctx, input, flips);
  });

  ctx.test("reverse_sorted", [&] {
    const std::vector<int> input = {5, 4, 3, 2, 1};
    const auto flips = sol.pancakeSort(input);
    verify_pancake_sort(ctx, input, flips);
  });

  ctx.test("single_element", [&] {
    const std::vector<int> input = {1};
    const auto flips = sol.pancakeSort(input);
    ctx.expect_true(flips.empty());
    verify_pancake_sort(ctx, input, flips);
  });
}

}  // namespace
