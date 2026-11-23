// LeetCode 27: Remove Element
// https://leetcode.com/problems/remove-element/
//
// Problem:
//   Given an integer array `nums` and an integer `val`, remove all
//   occurrences of `val` in `nums` in-place using O(1) extra space. Return
//   the count `k` of remaining elements such that the first `k` slots of
//   `nums` contain the elements not equal to `val` (in any order).
//
// Examples:
//   Example 1:
//     Input:  nums = [3, 2, 2, 3], val = 3
//     Output: 2, nums[0..1] = [2, 2]
//     Explanation:
//       - Initialize write_idx = 0 and scan nums from left to right.
//       - Skip index 0 (3 == val), then copy index 1 (2 != 3) to nums[0] and
//         increment write_idx to 1.
//       - Copy index 2 (2 != 3) to nums[1] and increment write_idx to 2, then skip
//         index 3 (3 == val).
//       - Return write_idx = 2 with the first 2 slots holding [2, 2].
//
//   Example 2:
//     Input:  nums = [0, 1, 2, 2, 3, 0, 4, 2], val = 2
//     Output: 5, nums[0..4] = [0, 1, 3, 0, 4]
//     Explanation:
//       - Start with write_idx = 0; indices 0 and 1 hold 0 and 1 (both != 2), so
//         nums[0..1] stay [0, 1] and write_idx becomes 2.
//       - Skip indices 2 and 3 because both equal val = 2, leaving write_idx = 2.
//       - Read index 4 (3 != 2) and index 5 (0 != 2), copying them into nums[2] and
//         nums[3] and advancing write_idx to 4.
//       - Copy index 6 (4 != 2) into nums[4] to reach write_idx = 5, skip index 7
//         (2 == val), and return 5.
//
//   Example 3:
//     Input:  nums = [2, 2, 2], val = 2
//     Output: 0 (nums[0..k - 1] is empty)
//     Explanation:
//       - Initialize write_idx = 0 and iterate through all three elements of nums.
//       - Every element equals val = 2, so the check num != val is never satisfied
//         and write_idx stays 0.
//       - Return write_idx = 0, leaving zero retained elements.
//
// Algorithm:
//   Slow/fast two pointers (equivalent to `std::remove`). Maintain a write
//   cursor `write_idx` starting at 0 while scanning every element `num` in
//   `nums`. Whenever `num != val`, copy `num` into `nums[write_idx]` and
//   advance `write_idx`. Because `write_idx` never outruns the read position,
//   no unread element is ever overwritten, and relative order of retained
//   elements is preserved.
//   Time Complexity:  O(n), where n = nums.size().
//   Space Complexity: O(1) in-place.
//
// Data Structures:
//   - In-place array (`std::vector<int>&`): compacted directly without
//     allocating a second buffer.
//   - Write pointer (`write_idx`): marks the end of the compacted valid
//     prefix `nums[0..write_idx - 1]`.
//
// Real-World Applications:
//   - C++ STL erase-remove idiom (`std::remove` / `std::erase`) for in-place
//     filtering of contiguous containers.
//   - Garbage collection and memory compaction (sliding live objects to the
//     front of a heap region to eliminate tombstones).
//   - Columnar/vectorized query engines (filtering out null or predicate-
//     failing rows in-place within fixed-size record batches).
//
// Related LeetCode Problems:
//   - #26 Remove Duplicates from Sorted Array [done] (in-place slow/fast compaction removing adjacent duplicates)
//   - #80 Remove Duplicates from Sorted Array II [done] (in-place compaction keeping at most two copies)
//   - #203 Remove Linked List Elements [done] (value-removal counterpart on singly linked lists)
//   - #283 Move Zeroes [done] (in-place compaction shifting zeros to the tail)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Removes all occurrences of `val` from `nums` in-place and returns the
  // count `k` of kept elements stored in `nums[0..k - 1]`.
  //
  // Invariant: after inspecting each element, `nums[0..write_idx - 1]`
  // contains every kept element seen so far in original order, and
  // `write_idx` is always <= the current read index.
  //
  // Time: O(n), Space: O(1).
  int removeElement(std::vector<int>& nums, int val) {
    std::size_t write_idx = 0uz;

    for (const int num : nums) {
      if (num != val) {
        nums[write_idx] = num;
        ++write_idx;
      }
    }

    return static_cast<int>(write_idx);
  }
};

LC_PROBLEM(27, "Remove Element", "two_pointers", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    std::vector<int> nums = {3, 2, 2, 3};
    const int k = sol.removeElement(nums, 3);
    ctx.expect_eq(k, 2);
    std::vector<int> kept(nums.begin(), nums.begin() + k);
    std::ranges::sort(kept);
    ctx.expect_eq(kept, std::vector<int>({2, 2}));
  });

  ctx.test("example_2", [&] {
    std::vector<int> nums = {0, 1, 2, 2, 3, 0, 4, 2};
    const int k = sol.removeElement(nums, 2);
    ctx.expect_eq(k, 5);
    std::vector<int> kept(nums.begin(), nums.begin() + k);
    std::ranges::sort(kept);
    ctx.expect_eq(kept, std::vector<int>({0, 0, 1, 3, 4}));
  });

  ctx.test("empty_and_all_removed", [&] {
    std::vector<int> empty_nums;
    ctx.expect_eq(sol.removeElement(empty_nums, 1), 0);

    std::vector<int> all_target = {2, 2, 2};
    ctx.expect_eq(sol.removeElement(all_target, 2), 0);
  });

  ctx.test("target_absent_and_single_element", [&] {
    std::vector<int> none_removed = {1, 2, 3};
    const int k1 = sol.removeElement(none_removed, 4);
    ctx.expect_eq(k1, 3);
    ctx.expect_eq(std::vector<int>(none_removed.begin(),
                                   none_removed.begin() + k1),
                  std::vector<int>({1, 2, 3}));

    std::vector<int> single_hit = {5};
    ctx.expect_eq(sol.removeElement(single_hit, 5), 0);

    std::vector<int> single_miss = {5};
    const int k2 = sol.removeElement(single_miss, 1);
    ctx.expect_eq(k2, 1);
    ctx.expect_eq(single_miss[0], 5);
  });
}

}  // namespace
