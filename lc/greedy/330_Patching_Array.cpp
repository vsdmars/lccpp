// LeetCode 330: Patching Array
// https://leetcode.com/problems/patching-array/
//
// Problem:
//   Given a sorted array of positive integers `nums` and a positive integer
//   `n`, add (patch) the minimum number of positive integers into `nums` so
//   that every integer in the closed interval [1, n] can be formed as the sum
//   of some subset of elements in the array.
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
