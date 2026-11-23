// LeetCode 280: Wiggle Sort
// https://leetcode.com/problems/wiggle-sort/
//
// Problem:
//   Given an unsorted integer array `nums`, reorder it in-place so that it
//   satisfies the alternating inequality pattern:
//     nums[0] <= nums[1] >= nums[2] <= nums[3] >= nums[4] ...
//   Multiple valid reorderings may exist; any valid permutation is accepted.
//
// Examples:
//   Example 1:
//     Input:  nums = [3, 5, 2, 1, 6, 4]
//     Output: nums = [3, 5, 1, 6, 2, 4]
//     Explanation:
//       - Start with expect_up = true and inspect idx = 1: nums[0] = 3 <= nums[1] =
//         5 holds, so no swap occurs and expect_up flips to false.
//       - At idx = 2 with expect_up = false, nums[1] = 5 >= nums[2] = 2 holds, so
//         no swap occurs and expect_up flips to true.
//       - At idx = 3 with expect_up = true, nums[2] = 2 > nums[3] = 1 fails the
//         condition, so swap them to get [3, 5, 1, 2, 6, 4] and flip expect_up to
//         false.
//       - At idx = 4 with expect_up = false, nums[3] = 2 < nums[4] = 6 fails the
//         condition, so swap them to get [3, 5, 1, 6, 2, 4] and flip expect_up to
//         true.
//       - At idx = 5 with expect_up = true, nums[4] = 2 <= nums[5] = 4 holds,
//         leaving the final valid wiggle array [3, 5, 1, 6, 2, 4].
//
//   Example 2:
//     Input:  nums = [5, 2, 0, -1, -4, -9]
//     Output: nums = [2, 5, -1, 0, -9, -4]
//     Explanation:
//       - At idx = 1 with expect_up = true, nums[0] = 5 > nums[1] = 2, so swap them
//         to produce [2, 5, 0, -1, -4, -9].
//       - At idx = 2 with expect_up = false, nums[1] = 5 >= nums[2] = 0 holds with
//         no swap.
//       - At idx = 3 with expect_up = true, nums[2] = 0 > nums[3] = -1, so swap
//         them to produce [2, 5, -1, 0, -4, -9].
//       - At idx = 4 with expect_up = false, nums[3] = 0 >= nums[4] = -4 holds with
//         no swap.
//       - At idx = 5 with expect_up = true, nums[4] = -4 > nums[5] = -9, so swap
//         them to finish with [2, 5, -1, 0, -9, -4].
//
//   Example 3:
//     Input:  nums = [7]
//     Output: nums = [7]
//     Explanation:
//       - Check the early-return guard nums.size() < 2uz, which is true for length
//         1.
//       - Return immediately without modifying nums, leaving [7] unchanged.
//
// Algorithm:
//   Single-pass greedy adjacent swap:
//   Traverse index `i` from `1` to `n - 1` while alternating the expected
//   comparison direction (`expect_up` starts `true` for `nums[0] <= nums[1]`):
//   - When `expect_up` is true (odd `i`), enforce `nums[i - 1] <= nums[i]`;
//     if violated (`nums[i - 1] > nums[i]`), swap `nums[i - 1]` and `nums[i]`.
//   - When `expect_up` is false (even `i`), enforce `nums[i - 1] >= nums[i]`;
//     if violated (`nums[i - 1] < nums[i]`), swap `nums[i - 1]` and `nums[i]`.
//   Correctness Invariant:
//     Swapping `nums[i - 1]` and `nums[i]` never breaks the already-established
//     relation between `nums[i - 2]` and `nums[i - 1]`. For odd `i`, we already
//     had `nums[i - 2] >= nums[i - 1]`, and swapping replaces `nums[i - 1]`
//     with the strictly smaller `nums[i]`, preserving `>=`. Symmetrically, for
//     even `i`, swapping replaces `nums[i - 1]` with the strictly larger
//     `nums[i]`, preserving `<=`.
//   Time Complexity:  O(n) — single linear scan with O(1) work per element.
//   Space Complexity: O(1) — strictly in-place swaps.
//
// Data Structures:
//   - `std::vector<int>&` (`nums`): mutated directly in-place with no auxiliary
//     storage.
//
// Real-World Applications:
//   - Load-balancing task queues or alternating high/low resource-intensity
//     jobs to avoid sustained thermal or bandwidth spikes.
//   - Waveform / alternating visual layout generation where adjacent items must
//     alternate between local valleys and peaks.
//   - Teaching local greedy exchange arguments in algorithm design where local
//     repairs strictly strengthen prior invariants.
//
// Related LeetCode Problems:
//   - #324 Wiggle Sort II [done] (strict inequality `nums[0] < nums[1] > nums[2] < ...` via median partition)
//   - #376 Wiggle Subsequence [new] (longest alternating subsequence via greedy/DP)
//   - #75 Sort Colors [done] (in-place linear-time array reordering with swaps)
//   - #2149 Rearrange Array Elements by Sign [new] (alternating-property array rearrangement)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Reorders `nums` in-place so that `nums[0] <= nums[1] >= nums[2] <= ...`.
  //
  // Time Complexity:  O(n)
  // Space Complexity: O(1)
  void wiggleSort(std::vector<int>& nums) {
    if (nums.size() < 2uz) {
      return;
    }

    // `expect_up == true`  means we need `nums[i - 1] <= nums[i]` (odd `i`).
    // `expect_up == false` means we need `nums[i - 1] >= nums[i]` (even `i`).
    bool expect_up = true;
    for (std::size_t idx = 1uz; idx < nums.size(); ++idx) {
      const bool needs_swap = (expect_up && nums[idx - 1uz] > nums[idx]) ||
                              (!expect_up && nums[idx - 1uz] < nums[idx]);
      if (needs_swap) {
        std::swap(nums[idx - 1uz], nums[idx]);
      }
      expect_up = !expect_up;
    }
  }
};

// Verifies that `nums` obeys `nums[0] <= nums[1] >= nums[2] <= ...` and is a
// permutation of `original`.
bool isValidWiggle(const std::vector<int>& nums,
                   const std::vector<int>& original) {
  if (nums.size() != original.size()) {
    return false;
  }

  std::vector<int> sorted_actual = nums;
  std::vector<int> sorted_orig = original;
  std::ranges::sort(sorted_actual);
  std::ranges::sort(sorted_orig);
  if (sorted_actual != sorted_orig) {
    return false;
  }

  for (std::size_t idx = 1uz; idx < nums.size(); ++idx) {
    if ((idx % 2uz) == 1uz) {
      if (nums[idx - 1uz] > nums[idx]) {
        return false;
      }
    } else if (nums[idx - 1uz] < nums[idx]) {
      return false;
    }
  }
  return true;
}

LC_PROBLEM(280, "Wiggle Sort", "array", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    const std::vector<int> original = {3, 5, 2, 1, 6, 4};
    std::vector<int> nums = original;
    sol.wiggleSort(nums);
    ctx.expect_true(isValidWiggle(nums, original));
  });

  ctx.test("example_2_duplicates", [&] {
    const std::vector<int> original = {6, 6, 5, 6, 3, 8};
    std::vector<int> nums = original;
    sol.wiggleSort(nums);
    ctx.expect_true(isValidWiggle(nums, original));
  });

  ctx.test("already_sorted_ascending", [&] {
    const std::vector<int> original = {1, 2, 3, 4, 5, 6, 7};
    std::vector<int> nums = original;
    sol.wiggleSort(nums);
    ctx.expect_true(isValidWiggle(nums, original));
  });

  ctx.test("sorted_descending_with_negatives", [&] {
    const std::vector<int> original = {5, 2, 0, -1, -4, -9};
    std::vector<int> nums = original;
    sol.wiggleSort(nums);
    ctx.expect_true(isValidWiggle(nums, original));
  });

  ctx.test("edge_cases_small_and_uniform", [&] {
    std::vector<int> empty_vec;
    sol.wiggleSort(empty_vec);
    ctx.expect_eq(empty_vec, std::vector<int>{});

    std::vector<int> single = {7};
    sol.wiggleSort(single);
    ctx.expect_eq(single, std::vector<int>{7});

    const std::vector<int> uniform_orig = {4, 4, 4, 4};
    std::vector<int> uniform = uniform_orig;
    sol.wiggleSort(uniform);
    ctx.expect_true(isValidWiggle(uniform, uniform_orig));
  });
}

}  // namespace
