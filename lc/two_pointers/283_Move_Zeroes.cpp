// LeetCode 283: Move Zeroes
// https://leetcode.com/problems/move-zeroes/
//
// Problem:
//   Given an integer array `nums`, move all 0s to the end of the array in-place
//   while maintaining the relative order of all non-zero elements. No extra
//   copy of the array may be allocated, and the number of element operations
//   should be minimized.
//
// Examples:
//   Example 1:
//     Input:  nums = [0, 1, 0, 3, 12]
//     Output: [1, 3, 12, 0, 0]
//     Explanation:
//       - Initialize write_idx = 0; at read_idx = 0, nums[0] is 0, so skip it.
//       - At read_idx = 1 (nums[1] = 1 != 0), swap nums[0] and nums[1] to get [1,
//         0, 0, 3, 12] and increment write_idx to 1.
//       - Skip read_idx = 2 (0); at read_idx = 3 (nums[3] = 3), swap nums[1] and
//         nums[3] to get [1, 3, 0, 0, 12] and advance write_idx to 2.
//       - At read_idx = 4 (nums[4] = 12), swap nums[2] and nums[4] to produce [1,
//         3, 12, 0, 0] with write_idx = 3.
//
//   Example 2:
//     Input:  nums = [0, 0, -3, 0, 7, -1]
//     Output: [-3, 7, -1, 0, 0, 0]
//     Explanation:
//       - Skip read_idx = 0 and read_idx = 1 because both hold 0, keeping write_idx
//         = 0.
//       - At read_idx = 2 (nums[2] = -3 != 0), swap nums[0] and nums[2] to get [-3,
//         0, 0, 0, 7, -1] and set write_idx = 1.
//       - Skip read_idx = 3 (0); at read_idx = 4 (nums[4] = 7), swap nums[1] and
//         nums[4] to get [-3, 7, 0, 0, 0, -1] and set write_idx = 2.
//       - At read_idx = 5 (nums[5] = -1), swap nums[2] and nums[5] to produce [-3,
//         7, -1, 0, 0, 0] and advance write_idx to 3.
//
//   Example 3:
//     Input:  nums = [1, 2, 3, 4]
//     Output: [1, 2, 3, 4]
//     Explanation:
//       - Initialize write_idx = 0 and scan read_idx from 0 through 3.
//       - At every index, nums[read_idx] != 0 and write_idx == read_idx, so the
//         guard skips self-swaps and advances write_idx alongside read_idx.
//       - Finish the pass with write_idx = 4 and nums unchanged as [1, 2, 3, 4].
//
// Algorithm:
//   Maintain a slow/fast two-pointer partition (stable compaction):
//     - `write_idx` points to the next position where a non-zero element should
//       be placed.
//     - `read_idx` scans through the array from left to right.
//   Loop invariant: at the start of every iteration, nums[0..write_idx - 1]
//   contains all non-zero elements seen so far in their original relative
//   order, and nums[write_idx..read_idx - 1] contains only zeroes.
//   Whenever nums[read_idx] != 0, swap nums[write_idx] with nums[read_idx]
//   (skipping the no-op swap when write_idx == read_idx) and advance
//   `write_idx`. After one pass, all zeroes are compacted at the tail.
//   Time complexity: O(n). Space complexity: O(1).
//
// Data Structures:
//   - In-place std::vector<int> with two index pointers (`write_idx`,
//     `read_idx`): performs stable partitioning without auxiliary storage.
//
// Real-World Applications:
//   - Mark-and-compact garbage collectors sliding live heap objects forward to
//     eliminate memory fragmentation while preserving allocation order.
//   - Columnar database / log compaction removing tombstone entries in-place.
//   - Sparse buffer packing in audio/video or SIMD pipelines where inactive
//     lanes or empty slots are pushed to the tail.
//
// Related LeetCode Problems:
//   - #27 Remove Element [done] (in-place compaction removing a target value)
//   - #26 Remove Duplicates from Sorted Array [done] (read/write pointer compaction)
//   - #75 Sort Colors [done] (three-way in-place Dutch National Flag partition)
//   - #905 Sort Array By Parity [done] (in-place predicate partitioning)
//   - #2460 Apply Operations to an Array [new] (adjacent merge followed by Move Zeroes)

#include "framework/lc_framework.h"

#include <cstddef>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Moves all zeroes in `nums` to the end in-place while preserving the
  // relative order of non-zero elements.
  //
  // Invariant: nums[0..write_idx - 1] holds every non-zero element seen before
  // `read_idx` in original order, and nums[write_idx..read_idx - 1] is all 0s.
  //
  // Time: O(n), Space: O(1).
  void moveZeroes(std::vector<int>& nums) {
    std::size_t write_idx = 0;

    for (std::size_t read_idx = 0uz; read_idx < nums.size(); ++read_idx) {
      if (nums[read_idx] != 0) {
        // Avoid redundant self-swaps when no zero has been encountered yet.
        if (write_idx != read_idx) {
          std::swap(nums[write_idx], nums[read_idx]);
        }
        ++write_idx;
      }
    }
  }
};

LC_PROBLEM(283, "Move Zeroes", "two_pointers", ctx) {
  Solution sol;

  ctx.test("example_basic", [&] {
    std::vector<int> nums{0, 1, 0, 3, 12};
    sol.moveZeroes(nums);
    ctx.expect_eq(nums, std::vector<int>{1, 3, 12, 0, 0});
  });

  ctx.test("single_zero_and_empty", [&] {
    std::vector<int> single{0};
    std::vector<int> empty{};
    sol.moveZeroes(single);
    sol.moveZeroes(empty);
    ctx.expect_eq(single, std::vector<int>{0});
    ctx.expect_eq(empty, std::vector<int>{});
  });

  ctx.test("all_zeroes", [&] {
    std::vector<int> nums{0, 0, 0, 0};
    sol.moveZeroes(nums);
    ctx.expect_eq(nums, std::vector<int>{0, 0, 0, 0});
  });

  ctx.test("no_zeroes", [&] {
    std::vector<int> nums{1, 2, 3, 4};
    sol.moveZeroes(nums);
    ctx.expect_eq(nums, std::vector<int>{1, 2, 3, 4});
  });

  ctx.test("already_partitioned", [&] {
    std::vector<int> nums{5, -1, 2, 0, 0};
    sol.moveZeroes(nums);
    ctx.expect_eq(nums, std::vector<int>{5, -1, 2, 0, 0});
  });

  ctx.test("leading_zeroes_with_negatives", [&] {
    std::vector<int> nums{0, 0, -3, 0, 7, -1};
    sol.moveZeroes(nums);
    ctx.expect_eq(nums, std::vector<int>{-3, 7, -1, 0, 0, 0});
  });
}

}  // namespace
