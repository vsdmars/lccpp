// LeetCode 88: Merge Sorted Array
// https://leetcode.com/problems/merge-sorted-array/
//
// Problem:
//   Given two integer arrays `nums1` and `nums2` sorted in non-decreasing
//   order, and integers `m` and `n` representing the number of initialized
//   elements in `nums1` and `nums2` respectively, merge `nums2` into `nums1`
//   in-place as a single non-decreasing array occupying the first `m + n`
//   slots of `nums1`. You may assume `nums1.size() >= m + n`.
//
// Examples:
//   Example 1:
//     Input:  nums1 = [1, 2, 3, 0, 0, 0], m = 3, nums2 = [2, 5, 6], n = 3
//     Output: nums1 = [1, 2, 2, 3, 5, 6]
//     Explanation:
//       - Initialize remaining-count cursors i = 3, j = 3, and write = 6.
//       - Compare nums1[2] = 3 against nums2[2] = 6 (write 6 at nums1[5], j = 2),
//         then nums1[2] = 3 against nums2[1] = 5 (write 5 at nums1[4], j = 1).
//       - Next nums1[2] = 3 > nums2[0] = 2 places 3 at nums1[3] (i = 2), then
//         nums1[1] = 2 > nums2[0] = 2 is false so 2 from nums2 is placed at
//         nums1[2] (j = 0).
//       - With j == 0, the trailing copy loop is skipped and nums1[0..1] ([1, 2])
//         stay in place, yielding [1, 2, 2, 3, 5, 6].
//
//   Example 2:
//     Input:  nums1 = [4, 5, 6, 0, 0, 0], m = 3, nums2 = [1, 2, 3], n = 3
//     Output: nums1 = [1, 2, 3, 4, 5, 6]
//     Explanation:
//       - Start with i = 3, j = 3, and write = 6.
//       - In the first loop, nums1[2] = 6, nums1[1] = 5, and nums1[0] = 4 all
//         exceed nums2[2] = 3, so they are written to indices 5, 4, and 3, reducing
//         i to 0 while write = 3 and j = 3.
//       - The second loop copies the remaining prefix nums2[0..2] (3, 2, 1)
//         backward into nums1[2], nums1[1], and nums1[0], yielding [1, 2, 3, 4, 5,
//         6].
//
//   Example 3:
//     Input:  nums1 = [0], m = 0, nums2 = [1], n = 1
//     Output: nums1 = [1]
//     Explanation:
//       - Initialize i = 0, j = 1, and write = 1.
//       - Since i == 0, the main merge loop while (i > 0uz && j > 0uz) does not
//         execute.
//       - The cleanup loop while (j > 0uz) decrements write and j to 0 and copies
//         nums2[0] = 1 into nums1[0], producing [1].
//
// Algorithm:
//   Reverse three-pointer merge starting from the back of the merged range.
//   Merging forward would overwrite unconsumed elements of `nums1` unless an
//   O(m) auxiliary copy were made. Filling backward from index `m + n - 1`
//   down to `0` eliminates that conflict because the trailing `n` slots of
//   `nums1` are unused scratch space and `write >= i` always holds.
//   Using 1-based remaining counts (`i = m`, `j = n`, `write = m + n`):
//     - While both `i > 0` and `j > 0`, place `max(nums1[i - 1], nums2[j - 1])`
//       at `nums1[--write]` and decrement the corresponding source count.
//     - If `j > 0` after `i` reaches 0, copy the remaining prefix `nums2[0..j)`
//       into `nums1[0..j)`.
//     - If `i > 0` after `j` reaches 0, those elements of `nums1` are already
//       in their exact final positions.
//   Time Complexity:  O(m + n).
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   Two `std::vector<int>` sequences traversed from the tail via three
//   `std::size_t` remaining-count cursors (`i`, `j`, `write`).
//
// Real-World Applications:
//   - Log-Structured Merge-Tree (LSM-tree) SSTable compaction in storage
//     engines (RocksDB, LevelDB, Cassandra).
//   - External merge sort combining sorted disk runs into pre-allocated output
//     buffers without extra copies.
//   - Merging sorted inverted-index posting lists or time-series chunks in
//     search and analytics engines.
//
// Related LeetCode Problems:
//   - #21 Merge Two Sorted Lists [done] (sorted two-way merge on linked lists)
//   - #23 Merge k Sorted Lists [done] (k-way merge using a min-heap or divide-and-conquer)
//   - #977 Squares of a Sorted Array [done] (reverse fill from both ends of a sorted array)
//   - #986 Interval List Intersections [done] (two-pointer scan across two sorted lists)
//   - #1608 Special Array With X Elements Greater Than or Equal X [new] (sorted-array boundary reasoning)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Merges the sorted prefix `nums2[0 .. n)` into `nums1[0 .. m)` in-place,
  // writing the combined sorted result into `nums1[0 .. m + n)`.
  //
  // Invariant: `write == i + j`, so `write >= i` at every step; writing to
  // `nums1[write - 1]` never clobbers any unread element in `nums1[0 .. i)`.
  //
  // Complexity: O(m + n) time, O(1) extra space.
  void merge(std::vector<int>& nums1, int m, const std::vector<int>& nums2,
             int n) {
    // Track remaining unread counts instead of signed indices to avoid
    // unsigned underflow cleanly.
    auto i = static_cast<std::size_t>(m);
    auto j = static_cast<std::size_t>(n);
    std::size_t write = i + j;

    // Pick the larger tail element on each step and place it at the back of
    // the unmerged portion of `nums1`.
    while (i > 0uz && j > 0uz) {
      --write;
      if (nums1[i - 1uz] > nums2[j - 1uz]) {
        --i;
        nums1[write] = nums1[i];
      } else {
        --j;
        nums1[write] = nums2[j];
      }
    }

    // If `nums2` still has smaller elements remaining, copy them into the
    // front of `nums1`. Any remaining `nums1` elements are already in place.
    while (j > 0uz) {
      --write;
      --j;
      nums1[write] = nums2[j];
    }
  }
};

LC_PROBLEM(88, "Merge Sorted Array", "two_pointers", ctx) {
  Solution sol;

  ctx.test("python_build_case", [&] {
    std::vector<int> nums1{1, 2, 3, 4, 7, 0, 0, 0, 0};
    const std::vector<int> nums2{3, 4, 5, 6};
    sol.merge(nums1, 5, nums2, 4);
    ctx.expect_eq(nums1, std::vector<int>{1, 2, 3, 3, 4, 4, 5, 6, 7});
  });

  ctx.test("leetcode_example_1", [&] {
    std::vector<int> nums1{1, 2, 3, 0, 0, 0};
    const std::vector<int> nums2{2, 5, 6};
    sol.merge(nums1, 3, nums2, 3);
    ctx.expect_eq(nums1, std::vector<int>{1, 2, 2, 3, 5, 6});
  });

  ctx.test("leetcode_example_2_empty_nums2", [&] {
    std::vector<int> nums1{1};
    const std::vector<int> nums2;
    sol.merge(nums1, 1, nums2, 0);
    ctx.expect_eq(nums1, std::vector<int>{1});
  });

  ctx.test("leetcode_example_3_empty_nums1_prefix", [&] {
    std::vector<int> nums1{0};
    const std::vector<int> nums2{1};
    sol.merge(nums1, 0, nums2, 1);
    ctx.expect_eq(nums1, std::vector<int>{1});
  });

  ctx.test("all_nums2_smaller_than_nums1", [&] {
    std::vector<int> nums1{4, 5, 6, 0, 0, 0};
    const std::vector<int> nums2{1, 2, 3};
    sol.merge(nums1, 3, nums2, 3);
    ctx.expect_eq(nums1, std::vector<int>{1, 2, 3, 4, 5, 6});
  });

  ctx.test("all_nums2_larger_than_nums1", [&] {
    std::vector<int> nums1{1, 2, 3, 0, 0, 0};
    const std::vector<int> nums2{4, 5, 6};
    sol.merge(nums1, 3, nums2, 3);
    ctx.expect_eq(nums1, std::vector<int>{1, 2, 3, 4, 5, 6});
  });

  ctx.test("negative_values_and_both_empty", [&] {
    std::vector<int> nums1{-5, -1, 2, 0, 0};
    const std::vector<int> nums2{-3, 0};
    sol.merge(nums1, 3, nums2, 2);
    ctx.expect_eq(nums1, std::vector<int>{-5, -3, -1, 0, 2});

    std::vector<int> empty1;
    const std::vector<int> empty2;
    sol.merge(empty1, 0, empty2, 0);
    ctx.expect_eq(empty1, std::vector<int>{});
  });
}

}  // namespace
