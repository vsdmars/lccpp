// LeetCode 80: Remove Duplicates From Sorted Array II
// https://leetcode.com/problems/remove-duplicates-from-sorted-array-ii/
//
// Problem:
//   Given an integer array `nums` sorted in non-decreasing order, remove
//   duplicates in-place so that each distinct value appears at most twice,
//   preserving relative order. Place the compacted result in the first `k`
//   slots of `nums` and return `k`. Extra memory must be O(1).
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 1, 1, 2, 2, 3]
//     Output: k = 5, nums[:k] = [1, 1, 2, 2, 3]
//     Explanation:
//       - Initialize write = 0 (kMaxCopies = 2); the first two 1s pass write < 2,
//         writing nums[0] = 1 and nums[1] = 1 and advancing write to 2.
//       - Skip the third 1 because 1 > nums[write - 2] (1 > nums[0] = 1) is false,
//         keeping write at 2.
//       - Write both 2s since 2 > nums[0] (1) and 2 > nums[1] (1), placing them at
//         nums[2] and nums[3] and advancing write to 4.
//       - Write 3 because 3 > nums[2] (2), setting nums[4] = 3 and returning write
//         = 5 with kept prefix [1, 1, 2, 2, 3].
//
//   Example 2:
//     Input:  nums = [0, 0, 1, 1, 1, 1, 2, 3, 3]
//     Output: k = 7, nums[:k] = [0, 0, 1, 1, 2, 3, 3]
//     Explanation:
//       - Keep the first two 0s via write < 2 so nums[0..2) = [0, 0] and write = 2.
//       - Keep the first two 1s because 1 > nums[0] (0) and 1 > nums[1] (0),
//         updating nums[2] = 1, nums[3] = 1, and write = 4; skip the third and
//         fourth 1s since 1 > nums[2] (1) is false.
//       - Accept 2 (2 > nums[2] = 1) into nums[4], then accept both 3s (3 > nums[3]
//         = 1 and 3 > nums[4] = 2) into nums[5] and nums[6], advancing write to 7.
//       - Return k = 7 with compacted prefix [0, 0, 1, 1, 2, 3, 3].
//
//   Example 3:
//     Input:  nums = [7, 7, 7, 7, 7]
//     Output: k = 2, nums[:k] = [7, 7]
//     Explanation:
//       - Keep the first two 7s via write < 2, setting nums[0] = 7, nums[1] = 7,
//         and write = 2.
//       - Skip each of the remaining three 7s because val > nums[write - 2] checks
//         7 > nums[0] (7), which is false.
//       - Return k = 2 with kept prefix [7, 7].
//
// Algorithm:
//   Slow/fast two-pointer compaction exploiting the non-decreasing sort order.
//   Maintain a write index `write` marking the next output slot. For every
//   element `val` scanned left-to-right:
//     - The first two elements (`write < 2`) are always kept.
//     - Otherwise, since `nums[0 .. write)` is sorted and already satisfies
//       the at-most-two invariant, `val` has appeared at least twice in the
//       output prefix if and only if `val == nums[write - 2]` (equivalently,
//       `val` can be appended if and only if `val > nums[write - 2]`).
//   Notice the pattern generalizes trivially to "at most K duplicates" by
//   replacing the constant 2 with K.
//   Time Complexity:  O(n) single pass.
//   Space Complexity: O(1) extra space (in-place overwrites).
//
// Data Structures:
//   In-place `std::vector<int>` with a slow write cursor (`std::size_t`) and
//   sequential forward scan over the elements.
//
// Real-World Applications:
//   - Log / telemetry stream deduplication capping burst repetitions of
//     consecutive identical sorted events (rate-limited run-length capping).
//   - Columnar database / Parquet sorted-column compaction when enforcing
//     bounded multiplicity constraints in-place.
//   - Time-series downsampling and sensor noise suppression in fixed-memory
//     embedded buffers.
//
// Related LeetCode Problems:
//   - #26 Remove Duplicates from Sorted Array [done] (at most 1 occurrence; K = 1 special case)
//   - #27 Remove Element [done] (in-place slow/fast write-pointer filtering)
//   - #283 Move Zeroes [done] (in-place compaction preserving relative order)
//   - #82 Remove Duplicates from Sorted List II [done] (duplicate removal on sorted linked lists)
//   - #83 Remove Duplicates from Sorted List [done] (single-occurrence deduplication on linked lists)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Compacts a sorted array in-place so each unique value appears at most
  // `kMaxCopies` (2) times, returning the new logical length `k`.
  //
  // Invariant: `nums[0 .. write)` is sorted non-decreasingly and contains no
  // value more than twice; all elements read so far have been processed.
  //
  // Complexity: O(n) time, O(1) extra space.
  int removeDuplicates(std::vector<int>& nums) {
    static constexpr std::size_t kMaxCopies = 2uz;
    std::size_t write = 0uz;

    for (const int val : nums) {
      // Because `nums[0 .. write)` is non-decreasing, `val > nums[write - 2]`
      // holds iff `val` has not yet appeared twice at the tail of the kept
      // prefix.
      if (write < kMaxCopies || val > nums[write - kMaxCopies]) {
        nums[write] = val;
        ++write;
      }
    }

    return static_cast<int>(write);
  }
};

LC_PROBLEM(80, "Remove Duplicates From Sorted Array II", "two_pointers", ctx) {
  Solution sol;

  auto kept_prefix = [](const std::vector<int>& nums, int k) {
    return std::vector<int>(nums.begin(), nums.begin() + k);
  };

  ctx.test("python_build_case_1", [&] {
    std::vector<int> nums{1, 2, 2, 2, 4, 5, 5, 5, 6};
    const int k = sol.removeDuplicates(nums);
    ctx.expect_eq(k, 7);
    ctx.expect_eq(kept_prefix(nums, k), std::vector<int>{1, 2, 2, 4, 5, 5, 6});
  });

  ctx.test("leetcode_example_1", [&] {
    std::vector<int> nums{1, 1, 1, 2, 2, 3};
    const int k = sol.removeDuplicates(nums);
    ctx.expect_eq(k, 5);
    ctx.expect_eq(kept_prefix(nums, k), std::vector<int>{1, 1, 2, 2, 3});
  });

  ctx.test("leetcode_example_2", [&] {
    std::vector<int> nums{0, 0, 1, 1, 1, 1, 2, 3, 3};
    const int k = sol.removeDuplicates(nums);
    ctx.expect_eq(k, 7);
    ctx.expect_eq(kept_prefix(nums, k), std::vector<int>{0, 0, 1, 1, 2, 3, 3});
  });

  ctx.test("short_and_empty_arrays", [&] {
    std::vector<int> empty;
    ctx.expect_eq(sol.removeDuplicates(empty), 0);

    std::vector<int> single{42};
    ctx.expect_eq(sol.removeDuplicates(single), 1);
    ctx.expect_eq(kept_prefix(single, 1), std::vector<int>{42});

    std::vector<int> pair{5, 5};
    ctx.expect_eq(sol.removeDuplicates(pair), 2);
    ctx.expect_eq(kept_prefix(pair, 2), std::vector<int>{5, 5});
  });

  ctx.test("all_identical_and_all_unique", [&] {
    std::vector<int> identical{7, 7, 7, 7, 7};
    const int k1 = sol.removeDuplicates(identical);
    ctx.expect_eq(k1, 2);
    ctx.expect_eq(kept_prefix(identical, k1), std::vector<int>{7, 7});

    std::vector<int> unique{-5, -2, 0, 3, 9};
    const int k2 = sol.removeDuplicates(unique);
    ctx.expect_eq(k2, 5);
    ctx.expect_eq(kept_prefix(unique, k2), std::vector<int>{-5, -2, 0, 3, 9});
  });

  ctx.test("negative_numbers_with_runs", [&] {
    std::vector<int> nums{-3, -3, -3, -1, -1, 0, 0, 0};
    const int k = sol.removeDuplicates(nums);
    ctx.expect_eq(k, 6);
    ctx.expect_eq(kept_prefix(nums, k), std::vector<int>{-3, -3, -1, -1, 0, 0});
  });
}

}  // namespace
