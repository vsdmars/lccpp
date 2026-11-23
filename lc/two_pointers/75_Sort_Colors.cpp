// LeetCode 75: Sort Colors
// https://leetcode.com/problems/sort-colors/
//
// Problem:
//   Given an integer array `nums` containing only the values 0 (red), 1
//   (white), and 2 (blue), sort the array in-place so that objects of the same
//   color are adjacent in the order 0, 1, then 2. You must solve the problem
//   without using the standard library's sort function, ideally in a single
//   pass using O(1) extra space.
//
// Examples:
//   Example 1:
//     Input:  nums = [2, 0, 2, 1, 1, 0]
//     Output: [0, 0, 1, 1, 2, 2]
//     Explanation:
//       - Start with small_idx = 0, mv_idx = 0, and large_idx = 6; at mv_idx = 0,
//         nums[0] = 2 > 1 decrements large_idx to 5 and swaps nums[0] with nums[5],
//         giving [0, 0, 2, 1, 1, 2] with mv_idx still 0.
//       - Inspect mv_idx = 0 (nums[0] = 0 < 1) and mv_idx = 1 (nums[1] = 0 < 1),
//         swapping each 0 in place and advancing small_idx and mv_idx both to 2.
//       - At mv_idx = 2, nums[2] = 2 > 1 decrements large_idx to 4 and swaps
//         nums[2] with nums[4], producing [0, 0, 1, 1, 2, 2] while mv_idx stays at
//         2.
//       - At mv_idx = 2 and mv_idx = 3, both elements equal 1 so mv_idx advances to
//         4, ending the loop because mv_idx == large_idx == 4 with nums = [0, 0, 1,
//         1, 2, 2].
//
//   Example 2:
//     Input:  nums = [2, 0, 1]
//     Output: [0, 1, 2]
//     Explanation:
//       - Initialize small_idx = 0, mv_idx = 0, and large_idx = 3; nums[0] = 2 > 1
//         decrements large_idx to 2 and swaps nums[0] with nums[2], giving [1, 0,
//         2] with mv_idx = 0.
//       - Re-inspect mv_idx = 0 where nums[0] = 1 now equals 1, so advance mv_idx
//         to 1.
//       - At mv_idx = 1, nums[1] = 0 < 1 swaps nums[0] and nums[1] to form [0, 1,
//         2] and increments small_idx to 1 and mv_idx to 2, terminating since
//         mv_idx == large_idx == 2.
//
//   Example 3:
//     Input:  nums = [1, 0]
//     Output: [0, 1]
//     Explanation:
//       - Initialize small_idx = 0, mv_idx = 0, and large_idx = 2; at mv_idx = 0,
//         nums[0] = 1 keeps the element in the middle region and advances mv_idx to
//         1.
//       - At mv_idx = 1, nums[1] = 0 < 1 swaps nums[0] and nums[1] to yield [0, 1]
//         and advances small_idx to 1 and mv_idx to 2.
//       - Stop the loop at mv_idx == large_idx == 2 with nums sorted as [0, 1].
//
// Algorithm:
//   Dutch National Flag (three-way partitioning) using three pointers:
//   `small_idx`, `mv_idx`, and `large_idx`.
//   Loop invariant maintained at every step:
//     - nums[0 .. small_idx)          all equal 0
//     - nums[small_idx .. mv_idx)     all equal 1
//     - nums[mv_idx .. large_idx)     unclassified elements
//     - nums[large_idx .. n)          all equal 2
//   Inspect `nums[mv_idx]`:
//     - If 0: swap with `nums[small_idx]` and advance both `small_idx` and
//       `mv_idx` (the element swapped from `small_idx` is guaranteed to be 1
//       or the same index, so it needs no re-inspection).
//     - If 2: decrement `large_idx` and swap with `nums[large_idx]`; do NOT
//       advance `mv_idx` because the newly swapped-in element has not been
//       examined yet.
//     - If 1: simply advance `mv_idx`.
//   Time Complexity:  O(n) single pass; every iteration either increases
//                     `mv_idx` or decreases `large_idx`.
//   Space Complexity: O(1) auxiliary space (in-place swaps only).
//
// Data Structures:
//   In-place `std::vector<int>` partitioned by three `std::size_t` boundary
//   indices (`small_idx`, `mv_idx`, `large_idx`).
//
// Real-World Applications:
//   - Three-way quicksort partitioning (Bentley-McIlroy / Dual-Pivot Quicksort)
//     to handle inputs with many duplicate keys without O(n^2) degradation.
//   - Priority-tiered packet or job scheduling buffers (high / normal / low
//     priority queues partitioned in-place).
//   - In-place memory compaction / garbage collection segregating live,
//     pinned, and free objects in linear time.
//
// Related LeetCode Problems:
//   - #283 Move Zeroes [done] (two-way in-place partitioning with two pointers)
//   - #26 Remove Duplicates from Sorted Array [done] (in-place write-pointer compaction)
//   - #215 Kth Largest Element in an Array [done] (Quickselect using partitioning)
//   - #280 Wiggle Sort [done] (in-place linear-time rearrangement with local swaps)
//   - #2161 Partition Array According to Given Pivot [new] (three-way partitioning around a pivot)

#include "framework/lc_framework.h"

#include <cstddef>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Sorts an array of 0s, 1s, and 2s in-place via Dutch National Flag
  // three-way partitioning.
  //
  // Invariants before each iteration of the main loop:
  //   - [0, small_idx)          contains only 0s
  //   - [small_idx, mv_idx)     contains only 1s
  //   - [mv_idx, large_idx)     contains unexamined elements
  //   - [large_idx, nums.size()) contains only 2s
  //
  // Complexity: O(n) time, O(1) extra space.
  void sortColors(std::vector<int>& nums) {
    std::size_t small_idx = 0uz;
    std::size_t large_idx = nums.size();
    std::size_t mv_idx = 0uz;

    // Shrink the unclassified region [mv_idx, large_idx) until it is empty.
    while (mv_idx < large_idx) {
      if (nums[mv_idx] < 1) {
        // Place 0 at the end of the 0-region; the element coming from
        // `small_idx` was already inspected (it is either 1 or `mv_idx`
        // itself), so both pointers advance.
        std::swap(nums[small_idx], nums[mv_idx]);
        ++small_idx;
        ++mv_idx;
      } else if (nums[mv_idx] > 1) {
        // Expand the 2-region leftward and swap the 2 into it. Keep `mv_idx`
        // stationary so the swapped-in element gets inspected next.
        --large_idx;
        std::swap(nums[mv_idx], nums[large_idx]);
      } else {
        // Element is 1: already belongs in the middle region.
        ++mv_idx;
      }
    }
  }
};

LC_PROBLEM(75, "Sort Colors", "two_pointers", ctx) {
  Solution sol;

  ctx.test("python_build_mixed", [&] {
    std::vector<int> nums{2, 1, 2, 1, 0, 1, 2};
    sol.sortColors(nums);
    ctx.expect_eq(nums, std::vector<int>{0, 1, 1, 1, 2, 2, 2});
  });

  ctx.test("leetcode_example_1", [&] {
    std::vector<int> nums{2, 0, 2, 1, 1, 0};
    sol.sortColors(nums);
    ctx.expect_eq(nums, std::vector<int>{0, 0, 1, 1, 2, 2});
  });

  ctx.test("leetcode_example_2", [&] {
    std::vector<int> nums{2, 0, 1};
    sol.sortColors(nums);
    ctx.expect_eq(nums, std::vector<int>{0, 1, 2});
  });

  ctx.test("three_elements_1_2_0", [&] {
    std::vector<int> nums{1, 2, 0};
    sol.sortColors(nums);
    ctx.expect_eq(nums, std::vector<int>{0, 1, 2});
  });

  ctx.test("two_elements_1_0", [&] {
    std::vector<int> nums{1, 0};
    sol.sortColors(nums);
    ctx.expect_eq(nums, std::vector<int>{0, 1});
  });

  ctx.test("empty_and_single_element", [&] {
    std::vector<int> empty;
    sol.sortColors(empty);
    ctx.expect_eq(empty, std::vector<int>{});

    for (const int color : {0, 1, 2}) {
      std::vector<int> single{color};
      sol.sortColors(single);
      ctx.expect_eq(single, std::vector<int>{color});
    }
  });

  ctx.test("all_same_and_reverse_sorted", [&] {
    std::vector<int> all_twos{2, 2, 2, 2};
    sol.sortColors(all_twos);
    ctx.expect_eq(all_twos, std::vector<int>{2, 2, 2, 2});

    std::vector<int> reversed{2, 2, 1, 1, 0, 0};
    sol.sortColors(reversed);
    ctx.expect_eq(reversed, std::vector<int>{0, 0, 1, 1, 2, 2});
  });
}

}  // namespace
