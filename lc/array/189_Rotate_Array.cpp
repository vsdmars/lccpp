// LeetCode 189: Rotate Array
// https://leetcode.com/problems/rotate-array/
//
// Problem:
//   Given an integer array `nums`, rotate the array to the right by `k` steps
//   in-place, where `k` is non-negative. That is, every element originally at
//   index `i` moves to index `(i + k) % n`.
//
// Algorithm:
//   Three-Reversal In-Place Rotation:
//     First normalize `shift = k % n`; if `n <= 1` or `shift == 0`, the array
//     is already in its final state.
//     Writing the array as `A B` where `B` is the suffix of length `shift`
//     that must wrap around to the front, we want `B A`:
//       1. Reverse the entire array: `(A B)^R = B^R A^R`.
//       2. Reverse the first `shift` elements: `(B^R)^R A^R = B A^R`.
//       3. Reverse the remaining `n - shift` elements: `B (A^R)^R = B A`.
//     Every element is swapped at most twice, achieving linear time and
//     strictly O(1) extra memory.
//   Time Complexity:  O(n), where n = nums.size().
//   Space Complexity: O(1) auxiliary space (in-place).
//
// Data Structures:
//   - Operates directly in-place on `std::vector<int>` via random-access
//     iterators and `std::ranges::reverse`.
//
// Real-World Applications:
//   - Circular ring-buffer realignment and sliding-window reordering in audio,
//     networking, and embedded telemetry buffers.
//   - Gap-buffer / rope text-editor block moves (the classic "block swap" /
//     three-reversal primitive used in `std::rotate`).
//   - In-place cyclic permutation of image scanlines or FFT shift operations.
//
// Related LeetCode Problems:
//   - #61 Rotate List [done] (singly linked-list cyclic right rotation)
//   - #186 Reverse Words in a String II [done] (same whole-then-segment reversal trick)
//   - #796 Rotate String [new] (testing whether one string is a rotation of another)
//   - #396 Rotate Function [done] (O(n) incremental score across all rotations)
//   - #33 Search in Rotated Sorted Array [done] (searching an array produced by rotation)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Rotates `nums` in-place to the right by `k` positions using three
  // reversals.
  // Invariant: splitting nums into prefix A (size n - shift) and suffix B
  // (size shift), reversing all -> B^R A^R, then reversing each half restores
  // their internal order to yield B A.
  // Time: O(n), Space: O(1).
  void rotate(std::vector<int>& nums, int k) {
    if (nums.size() <= 1uz || k <= 0) {
      return;
    }

    // Effective right shift after removing full n-step cycles.
    const std::size_t shift = static_cast<std::size_t>(k) % nums.size();
    if (shift == 0uz) {
      return;
    }

    // Step 1: Reverse the full array so the last `shift` elements move to the
    // front (in reversed order).
    std::ranges::reverse(nums);

    const auto split = nums.begin() + static_cast<std::ptrdiff_t>(shift);

    // Step 2: Restore the original relative order of the wrapped prefix.
    std::ranges::reverse(nums.begin(), split);

    // Step 3: Restore the original relative order of the shifted suffix.
    std::ranges::reverse(split, nums.end());
  }
};

LC_PROBLEM(189, "Rotate Array", "array", ctx) {
  Solution sol;

  ctx.test("example_1_standard", [&] {
    std::vector<int> nums{1, 2, 3, 4, 5, 6, 7};
    sol.rotate(nums, 3);
    ctx.expect_eq(nums, std::vector<int>{5, 6, 7, 1, 2, 3, 4});
  });

  ctx.test("example_2_even_length", [&] {
    std::vector<int> nums{-1, -100, 3, 99};
    sol.rotate(nums, 2);
    ctx.expect_eq(nums, std::vector<int>{3, 99, -1, -100});
  });

  ctx.test("source_build_case", [&] {
    std::vector<int> nums{-1, -100, 3, 99, 7, 8, 11};
    sol.rotate(nums, 3);
    ctx.expect_eq(nums, std::vector<int>{7, 8, 11, -1, -100, 3, 99});
  });

  ctx.test("k_zero_and_multiples", [&] {
    std::vector<int> nums{1, 2, 3, 4};
    sol.rotate(nums, 0);
    ctx.expect_eq(nums, std::vector<int>{1, 2, 3, 4});

    sol.rotate(nums, 8);
    ctx.expect_eq(nums, std::vector<int>{1, 2, 3, 4});
  });

  ctx.test("k_larger_than_size", [&] {
    std::vector<int> nums{1, 2, 3};
    sol.rotate(nums, 5);  // Equivalent to k = 2.
    ctx.expect_eq(nums, std::vector<int>{2, 3, 1});
  });

  ctx.test("empty_and_single_element", [&] {
    std::vector<int> empty;
    sol.rotate(empty, 3);
    ctx.expect_eq(empty, std::vector<int>{});

    std::vector<int> single{42};
    sol.rotate(single, 10);
    ctx.expect_eq(single, std::vector<int>{42});
  });
}

}  // namespace
