// LeetCode 26: Remove Duplicates From Sorted Array
// https://leetcode.com/problems/remove-duplicates-from-sorted-array/
//
// Problem:
//   Given an integer array `nums` sorted in non-decreasing order, remove
//   duplicates in-place so that each unique element appears only once while
//   preserving relative order. Return the number of unique elements `k` such
//   that the first `k` slots of `nums` hold the deduplicated values. Extra
//   memory must be O(1).
//
// Algorithm:
//   Slow/fast two pointers. Keep `write_idx` pointing to the last confirmed
//   unique element (initially 0) while `read_idx` scans from 1 to `n - 1`.
//   Because `nums` is sorted, all duplicates are contiguous: whenever
//   `nums[read_idx] != nums[write_idx]`, a new unique value has been found,
//   so increment `write_idx` and copy `nums[read_idx]` into `nums[write_idx]`.
//   Return `write_idx + 1` (or `0` when `nums` is empty).
//   Time Complexity:  O(n), where n = nums.size().
//   Space Complexity: O(1) in-place.
//
// Data Structures:
//   - In-place array (`std::vector<int>&`): modified directly with no
//     auxiliary buffer.
//   - Slow/fast indices (`write_idx`, `read_idx`): maintain the deduplicated
//     prefix `nums[0..write_idx]`.
//
// Real-World Applications:
//   - Standard library `std::unique` / `std::ranges::unique` compaction on
//     sorted buffers.
//   - Database query execution (`SELECT DISTINCT` / merge-dedup operators
//     following an external merge sort).
//   - Time-series and columnar storage engines (deduplicating sorted keys and
//     building dictionary encodings in-place).
//
// Related LeetCode Problems:
//   - #27 Remove Element [done] (in-place slow/fast compaction filtering a target value)
//   - #80 Remove Duplicates from Sorted Array II [done] (allows at most two copies per value)
//   - #83 Remove Duplicates from Sorted List [done] (sorted deduplication on a linked list)
//   - #283 Move Zeroes [done] (in-place stable two-pointer compaction)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Deduplicates sorted `nums` in-place and returns the count `k` of unique
  // elements placed in `nums[0..k - 1]`.
  //
  // Invariant: before each iteration `read_idx`, `nums[0..write_idx]` holds
  // all unique values from `nums[0..read_idx - 1]` in strictly increasing
  // order.
  //
  // Time: O(n), Space: O(1).
  int removeDuplicates(std::vector<int>& nums) {
    if (nums.empty()) {
      return 0;
    }

    std::size_t write_idx = 0uz;

    for (std::size_t read_idx = 1uz; read_idx < nums.size(); ++read_idx) {
      if (nums[read_idx] == nums[write_idx]) {
        continue;
      }
      ++write_idx;
      nums[write_idx] = nums[read_idx];
    }

    return static_cast<int>(write_idx + 1uz);
  }
};

LC_PROBLEM(26, "Remove Duplicates From Sorted Array", "two_pointers", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    std::vector<int> nums = {1, 1, 2};
    const int k = sol.removeDuplicates(nums);
    ctx.expect_eq(k, 2);
    ctx.expect_eq(std::vector<int>(nums.begin(), nums.begin() + k),
                  std::vector<int>({1, 2}));
  });

  ctx.test("multiple_runs_of_duplicates", [&] {
    std::vector<int> nums = {1, 1, 2, 2, 3, 3, 7, 8, 9, 9};
    const int k = sol.removeDuplicates(nums);
    ctx.expect_eq(k, 6);
    ctx.expect_eq(std::vector<int>(nums.begin(), nums.begin() + k),
                  std::vector<int>({1, 2, 3, 7, 8, 9}));
  });

  ctx.test("empty_and_single_element", [&] {
    std::vector<int> empty_nums;
    ctx.expect_eq(sol.removeDuplicates(empty_nums), 0);

    std::vector<int> single = {1};
    const int k = sol.removeDuplicates(single);
    ctx.expect_eq(k, 1);
    ctx.expect_eq(single[0], 1);
  });

  ctx.test("already_unique_and_all_equal", [&] {
    std::vector<int> unique = {1, 2, 3, 4};
    const int k1 = sol.removeDuplicates(unique);
    ctx.expect_eq(k1, 4);
    ctx.expect_eq(std::vector<int>(unique.begin(), unique.begin() + k1),
                  std::vector<int>({1, 2, 3, 4}));

    std::vector<int> all_same = {5, 5, 5, 5};
    const int k2 = sol.removeDuplicates(all_same);
    ctx.expect_eq(k2, 1);
    ctx.expect_eq(all_same[0], 5);
  });

  ctx.test("negative_values", [&] {
    std::vector<int> nums = {-3, -3, -1, 0, 0, 2};
    const int k = sol.removeDuplicates(nums);
    ctx.expect_eq(k, 4);
    ctx.expect_eq(std::vector<int>(nums.begin(), nums.begin() + k),
                  std::vector<int>({-3, -1, 0, 2}));
  });
}

}  // namespace
