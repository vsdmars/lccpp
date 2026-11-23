// LeetCode 287: Find The Duplicate Number
// https://leetcode.com/problems/find-the-duplicate-number/
//
// Problem:
//   Given a read-only array `nums` of n + 1 integers where every integer lies
//   in the inclusive range [1, n], exactly one integer appears two or more
//   times (possibly more than twice). Return that repeated integer without
//   modifying `nums` and using only O(1) extra space.
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 3, 4, 2, 2]
//     Output: 2
//     Explanation:
//       - Start Phase 1 at slow = nums[0] = 1 and fast = nums[nums[0]] = nums[1] =
//         3.
//       - Advance slow by 1 step and fast by 2 steps: (1, 3) -> (3, 4) -> (2, 4) ->
//         (4, 4), meeting inside the cycle at 4.
//       - Start Phase 2 with finder = 0 and slow = 4, advancing both 1 step at a
//         time: (0, 4) -> (1, 2) -> (3, 4) -> (2, 2).
//       - Both pointers collide at cycle entry 2, so return 2.
//
//   Example 2:
//     Input:  nums = [2, 3, 4, 1, 1]
//     Output: 1
//     Explanation:
//       - Start Phase 1 at slow = nums[0] = 2 and fast = nums[nums[0]] = nums[2] =
//         4 along the stem 0 -> 2 -> 4 -> 1 -> 3 -> 1.
//       - Advance slow by 1 step and fast by 2 steps: (2, 4) -> (4, 3) -> (1, 3) ->
//         (3, 3), intersecting at 3.
//       - Reset finder = 0 while slow = 3 and step both forward by 1: (0, 3) -> (2,
//         1) -> (4, 3) -> (1, 1).
//       - Finder and slow meet at cycle entry 1, so return 1.
//
//   Example 3:
//     Input:  nums = [2, 2, 2, 2, 2]
//     Output: 2
//     Explanation:
//       - Initialize slow = nums[0] = 2 and fast = nums[nums[0]] = nums[2] = 2.
//       - Because slow == fast == 2 immediately, Phase 1 finishes without extra
//         iterations.
//       - In Phase 2, start finder = 0 and slow = 2; one step moves finder =
//         nums[0] = 2 and slow = nums[2] = 2, returning 2.
//
// Algorithm:
//   Floyd's Tortoise and Hare Cycle Detection on an implicit functional graph:
//     - Interpret the array as a directed graph where every index i in [0, n]
//       has a single outgoing edge i -> nums[i].
//     - Because all values lie in [1, n], index 0 has in-degree 0 (no edge
//       points to 0), while the duplicate value `d` has in-degree >= 2.
//     - Following edges from index 0 therefore traces a rho-shaped (ρ) path
//       whose cycle entry node is guaranteed to be the duplicate number `d`.
//     - Phase 1 (Intersection): advance `slow` by 1 step (nums[slow]) and
//       `fast` by 2 steps (nums[nums[fast]]) until they meet inside the cycle.
//     - Phase 2 (Cycle Entry): reset a second pointer `finder` to index 0 and
//       advance both `finder` and `slow` by 1 step at a time. Because the
//       distance from 0 to the cycle entry equals the remaining distance from
//       the meeting point to the entry (modulo cycle length), they collide
//       precisely at the duplicate value `d`.
//   Note: sign-flipping and XOR approaches fail here because `nums` is read-
//   only and the duplicate may appear more than twice (e.g., [2, 2, 2, 2, 2]).
//   Time complexity: O(n). Space complexity: O(1).
//
// Data Structures:
//   - Implicit functional graph represented by const std::vector<int>: each
//     element acts as a "next pointer" without allocating linked-list nodes.
//   - Two scalar integer pointers (`slow`, `fast` / `finder`): track positions
//     along the graph traversal in O(1) memory.
//
// Real-World Applications:
//   - Pseudorandom number generator (PRNG) period and cycle detection in
//     cryptography and Monte Carlo simulation quality testing.
//   - Pollard's rho algorithm for integer factorization and discrete logarithm
//     collision finding with constant memory.
//   - Deadlock / infinite-redirect loop detection in state machines and static
//     routing tables where memory is tightly constrained.
//
// Related LeetCode Problems:
//   - #141 Linked List Cycle [done] (Phase 1 of Floyd's tortoise-and-hare algorithm)
//   - #142 Linked List Cycle II [done] (exact linked-list analogue returning cycle entry)
//   - #268 Missing Number [done] (pigeonhole / index-value bijection on [0, n])
//   - #442 Find All Duplicates in an Array [done] (mutable array allows in-place marking)
//   - #41 First Missing Positive [done] (cyclic index placement on [1, n])

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the unique duplicated number in `nums` (values in [1, n], length
  // n + 1) without modifying the array, using Floyd's cycle-finding algorithm.
  //
  // Time: O(n), Space: O(1).
  int findDuplicate(const std::vector<int>& nums) {
    if (nums.size() < 2) {
      return -1;
    }

    // Phase 1: Move tortoise (`slow`) 1 step and hare (`fast`) 2 steps from
    // node 0 until they intersect inside the cycle induced by the duplicate.
    int slow = nums[0];
    int fast = nums[static_cast<std::size_t>(nums[0])];

    while (slow != fast) {
      slow = nums[static_cast<std::size_t>(slow)];
      const int fast_mid = nums[static_cast<std::size_t>(fast)];
      fast = nums[static_cast<std::size_t>(fast_mid)];
    }

    // Phase 2: Start `finder` at node 0 (outside the cycle) and advance both
    // pointers 1 step at a time; their meeting point is the cycle entrance,
    // which is the value referenced by at least two distinct indices.
    int finder = 0;
    while (finder != slow) {
      finder = nums[static_cast<std::size_t>(finder)];
      slow = nums[static_cast<std::size_t>(slow)];
    }

    return finder;
  }
};

LC_PROBLEM(287, "Find The Duplicate Number", "two_pointers", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    const std::vector<int> nums{1, 3, 4, 2, 2};
    ctx.expect_eq(sol.findDuplicate(nums), 2);
  });

  ctx.test("example_2", [&] {
    const std::vector<int> nums{3, 1, 3, 4, 2};
    ctx.expect_eq(sol.findDuplicate(nums), 3);
  });

  ctx.test("repeated_more_than_twice", [&] {
    // Verifies handling when the duplicate appears more than twice (where XOR
    // and parity-based sign-flipping approaches break).
    const std::vector<int> all_twos{2, 2, 2, 2, 2};
    const std::vector<int> triple{3, 3, 3, 3, 3};
    ctx.expect_eq(sol.findDuplicate(all_twos), 2);
    ctx.expect_eq(sol.findDuplicate(triple), 3);
  });

  ctx.test("minimum_size", [&] {
    const std::vector<int> nums{1, 1};
    ctx.expect_eq(sol.findDuplicate(nums), 1);
  });

  ctx.test("rho_shaped_cycle", [&] {
    // Non-cycle stem 0 -> 2 -> 4, cycle 1 -> 3 -> 1, entry at 1.
    const std::vector<int> nums{2, 3, 4, 1, 1};
    ctx.expect_eq(sol.findDuplicate(nums), 1);
  });

  ctx.test("duplicate_at_extremes", [&] {
    const std::vector<int> nums{4, 1, 2, 3, 4};
    ctx.expect_eq(sol.findDuplicate(nums), 4);
  });
}

}  // namespace
