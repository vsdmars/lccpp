// LeetCode 330: Patching Array
// https://leetcode.com/problems/patching-array/
//
// Problem:
//   Given a sorted array of positive integers `nums` and a positive integer
//   `n`, add (patch) the minimum number of positive integers into `nums` so
//   that every integer in the closed interval [1, n] can be formed as the sum
//   of some subset of elements in the array.
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 3], n = 6
//     Output: 1
//     Explanation:
//       - Initialize missing = 1, index = 0, and patches = 0 (achievable range is
//         [1, 1)).
//       - At missing = 1 <= 6, nums[0] = 1 <= 1, so consume it to advance missing =
//         2 and index = 1 (covering [1, 2)).
//       - At missing = 2 <= 6, nums[1] = 3 > 2, so patch 2: increment patches to 1
//         and double missing to 4 (covering [1, 4)).
//       - At missing = 4 <= 6, nums[1] = 3 <= 4, so consume 3 to reach missing = 7
//         and index = 2 (covering [1, 7)).
//       - Since missing = 7 > 6, the loop terminates and returns patches = 1.
//
//   Example 2:
//     Input:  nums = [1, 5, 10], n = 20
//     Output: 2
//     Explanation:
//       - Initialize missing = 1, index = 0, and patches = 0.
//       - At missing = 1, consume nums[0] = 1 so missing = 2 and index = 1.
//       - At missing = 2 and then missing = 4, nums[1] = 5 exceeds missing, so
//         patch 2 (missing = 4, patches = 1) and then patch 4 (missing = 8, patches
//         = 2).
//       - At missing = 8, consume nums[1] = 5 to reach missing = 13 (index = 2); at
//         missing = 13, consume nums[2] = 10 to reach missing = 23 (index = 3).
//       - Now missing = 23 > 20, so exit the loop and return patches = 2.
//
//   Example 3:
//     Input:  nums = [], n = 7
//     Output: 3
//     Explanation:
//       - Initialize missing = 1, index = 0, and patches = 0 with an empty nums
//         vector.
//       - Because index < nums.size() is always false, each iteration takes the
//         patch branch: patch 1 (missing = 2, patches = 1), patch 2 (missing = 4,
//         patches = 2), and patch 4 (missing = 8, patches = 3).
//       - Once missing = 8 > 7, all sums in [1, 7] are covered and the function
//         returns 3.
//
// Algorithm:
//   Maintain the greedy reach invariant: `missing` is the smallest positive
//   integer not yet guaranteed to be formable, meaning every sum in the
//   half-open interval [1, missing) is already achievable. Initialize
//   `missing = 1` (in 64-bit integer arithmetic to prevent signed 32-bit
//   overflow when `n` approaches INT_MAX) and `index = 0`.
//   While `missing <= n`:
//     - If `nums[index] <= missing`, incorporating `nums[index]` extends the
//       achievable sums from [1, missing) to [1, missing + nums[index]), so
//       advance `missing += nums[index]` and `++index`.
//     - Otherwise, no remaining element can bridge the gap at `missing`. Any
//       valid patch must be <= `missing`; choosing `missing` itself maximally
//       doubles the covered range to [1, 2 * missing). Increment `patches` and
//       set `missing += missing`.
//
//   Time Complexity:  O(m + log n), where m = nums.size(), since each step
//                     either consumes one element of `nums` or doubles
//                     `missing`.
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - Scalar state variables (`missing`, `index`, `patches`) scanning the
//     sorted input vector in a single pass.
//
// Real-World Applications:
//   - Currency denomination and coin-system design (minimal set of coin/stamp
//     values needed to make every exact payment up to a target ceiling).
//   - Hardware weight-set calibration and binary-weighted DAC / resistor
//     ladder design where minimal extra weights close coverage gaps.
//
// Related LeetCode Problems:
//   - #1798 Maximum Number of Consecutive Values You Can Make [new] (same greedy reach invariant without patching)
//   - #2952 Minimum Number of Coins to be Added [new] (direct reformulation with unsorted input)
//   - #55 Jump Game [done] (greedy farthest-reach invariant over a range)
//   - #45 Jump Game II [done] (minimizing steps to extend coverage to a target boundary)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the minimum number of elements to patch into sorted `nums` so that
  // every integer in [1, n] is representable as a subset sum.
  // Time: O(nums.size() + log n), Space: O(1).
  int minPatches(const std::vector<int>& nums, int n) {
    // Use 64-bit arithmetic because `missing` can exceed INT_MAX when doubling
    // near n = 2^31 - 1.
    std::int64_t missing = 1;
    const auto target = static_cast<std::int64_t>(n);
    std::size_t index = 0uz;
    int patches = 0;

    // Invariant: every integer in [1, missing) is currently achievable.
    while (missing <= target) {
      if (index < nums.size() &&
          static_cast<std::int64_t>(nums[index]) <= missing) {
        // Consume the next existing number to extend coverage without patching.
        missing += static_cast<std::int64_t>(nums[index]);
        ++index;
      } else {
        // No unused number <= `missing` exists; patching `missing` itself
        // doubles the reachable interval to [1, 2 * missing).
        missing += missing;
        ++patches;
      }
    }

    return patches;
  }
};

LC_PROBLEM(330, "Patching Array", "greedy", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    ctx.expect_eq(sol.minPatches({1, 3}, 6), 1);
  });

  ctx.test("example_2", [&] {
    Solution sol;
    ctx.expect_eq(sol.minPatches({1, 5, 10}, 20), 2);
  });

  ctx.test("example_3_no_patches_needed", [&] {
    Solution sol;
    ctx.expect_eq(sol.minPatches({1, 2, 2}, 5), 0);
  });

  ctx.test("python_build_input", [&] {
    Solution sol;
    ctx.expect_eq(sol.minPatches({3, 4, 6, 9, 11, 13}, 5000), 9);
  });

  ctx.test("empty_array", [&] {
    Solution sol;
    // Patching 1, 2, 4 covers [1, 7].
    ctx.expect_eq(sol.minPatches({}, 7), 3);
  });

  ctx.test("int_max_no_overflow", [&] {
    Solution sol;
    ctx.expect_eq(
        sol.minPatches({1, 2, 31, 33}, std::numeric_limits<int>::max()), 28);
  });
}

}  // namespace
