// LeetCode 996: Number Of Squareful Arrays
// https://leetcode.com/problems/number-of-squareful-arrays/
//
// Problem:
//   An integer array is called "squareful" if the sum of every adjacent pair
//   of elements is a perfect square. Given an integer array `nums` of length
//   n (1 <= n <= 12, 0 <= nums[i] <= 10^9), return the number of distinct
//   permutations of `nums` that are squareful. Two permutations are considered
//   different if they differ at any index.
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 17, 8]
//     Output: 2
//     Explanation:
//       - numSquarefulPerms sorts nums to [1, 8, 17] and initializes used = [0, 0,
//         0].
//       - Starting with 1 at depth = 0, 1 + 8 = 9 (3^2) extends to 8, while 1 + 17
//         = 18 is pruned; from [1, 8], 8 + 17 = 25 (5^2) completes [1, 8, 17] at
//         depth = 3 for count 1.
//       - Starting with 8 at depth = 0 reaches [8, 1] and [8, 17], both pruned at
//         depth = 2 because 1 + 17 = 18 is not a perfect square.
//       - Starting with 17 at depth = 0 prunes 17 + 1 = 18, extends to 8 (17 + 8 =
//         25), and completes [17, 8, 1] (8 + 1 = 9) for a total of 2.
//
//   Example 2:
//     Input:  nums = [0, 0, 0, 1]
//     Output: 4
//     Explanation:
//       - Sorting leaves nums = [0, 0, 0, 1]; at depth = 0, duplicate pruning
//         (nums[i] == nums[i - 1] && used[i - 1] == 0) allows only index 0 (0) and
//         index 3 (1).
//       - Since 0 + 0 = 0^2 and 0 + 1 = 1^2 are both perfect squares, starting with
//         index 0 explores all 3 distinct placements of the single 1 among the
//         remaining positions: [0, 0, 0, 1], [0, 0, 1, 0], and [0, 1, 0, 0].
//       - Starting with index 3 (1) forces the three remaining 0s to be chosen in a
//         single canonical order due to duplicate pruning, adding [1, 0, 0, 0] for
//         3 + 1 = 4 valid permutations.
//
//   Example 3:
//     Input:  nums = [1, 2, 4]
//     Output: 0
//     Explanation:
//       - After sorting, nums remains [1, 2, 4] and dfs starts at depth = 0.
//       - Picking 1 prunes both 2 (1 + 2 = 3) and 4 (1 + 4 = 5); picking 2 prunes 1
//         (3) and 4 (2 + 4 = 6); picking 4 prunes 1 (5) and 2 (6).
//       - No branch reaches depth = 3, so dfs(0, 0) returns 0.
//
// Algorithm:
//   Sort `nums` ascending so duplicate values sit adjacently, then run
//   backtracking DFS to build permutations one position at a time. At each
//   recursion depth we iterate over unused elements and prune two cases:
//     1. Duplicate symmetry pruning: if `nums[i] == nums[i - 1]` and index
//        `i - 1` is not currently used, placing `nums[i]` at this depth would
//        reproduce the exact subtree already explored by placing `nums[i - 1]`.
//     2. Adjacency constraint pruning: when `depth > 0`, `prev_val + nums[i]`
//        must be a perfect square (checked in 64-bit arithmetic to avoid
//        32-bit signed overflow when two elements near 10^9 are summed).
//   Reaching `depth == n` counts one valid squareful permutation.
//
//   Time Complexity:  O(n * n!) worst-case (strongly pruned in practice; n <= 12)
//   Space Complexity: O(n) for the recursion stack and `used` marker array.
//
// Data Structures:
//   - Sorted `std::vector<int>` to group equal values for O(1) duplicate
//     pruning at each backtracking level.
//   - `std::vector<std::uint8_t>` (`used`) bit-flag table tracking which
//     indices are currently placed in the active partial permutation.
//
// Real-World Applications:
//   - Constraint-satisfaction scheduling where consecutive jobs must satisfy
//     pairwise compatibility rules (e.g., thermal/power transition limits or
//     chemical-batch sequencing on a shared manufacturing line).
//   - Hamiltonian path counting on small compatibility graphs in hardware
//     pipeline verification and combinatorial test generation.
//
// Related LeetCode Problems:
//   - #47 Permutations II [done] (same sorted duplicate-pruning backtracking template)
//   - #46 Permutations [done] (unconstrained permutation generation via DFS)
//   - #367 Valid Perfect Square [done] (perfect square verification helper)
//   - #526 Beautiful Arrangement [done] (constrained permutation counting with DFS)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Counts the number of distinct permutations of `nums` in which every pair
  // of adjacent elements sums to a perfect square.
  //
  // Complexity: O(n * n!) worst-case time (with heavy pruning), O(n) space.
  int numSquarefulPerms(std::vector<int> nums) {
    const std::size_t n = nums.size();
    if (n == 0uz) {
      return 0;
    }

    // Sorting brings duplicate values together so we can skip symmetric
    // branches in O(1) without allocating a hash set at every depth.
    std::ranges::sort(nums);
    std::vector<std::uint8_t> used(n, 0);

    // Backtracking DFS: `depth` is the number of elements placed so far, and
    // `prev_val` is the most recently placed element (valid when depth > 0).
    auto dfs = [&](this auto&& self, std::size_t depth, int prev_val) -> int {
      // Base case: all n elements are placed with every adjacent pair valid.
      if (depth == n) {
        return 1;
      }

      int count = 0;
      for (std::size_t i = 0uz; i < n; ++i) {
        if (used[i] != 0) {
          continue;
        }
        // Duplicate pruning: if the previous identical value was not chosen at
        // an earlier depth, it has already been tried (and backtracked) at this
        // depth, so picking `nums[i]` here would yield duplicate permutations.
        if (i > 0uz && nums[i] == nums[i - 1uz] && used[i - 1uz] == 0) {
          continue;
        }
        // Enforce the squareful invariant against the immediately preceding
        // element in the partial permutation.
        if (depth > 0uz && !is_perfect_square(prev_val, nums[i])) {
          continue;
        }

        used[i] = 1;
        count += self(depth + 1uz, nums[i]);
        used[i] = 0;
      }
      return count;
    };

    return dfs(0uz, 0);
  }

 private:
  // Returns true iff `a + b` is a non-negative perfect square.
  // Uses 64-bit arithmetic so sums up to 2 * 10^9 cannot overflow signed int.
  [[nodiscard]] static bool is_perfect_square(int a, int b) noexcept {
    const std::int64_t sum =
        static_cast<std::int64_t>(a) + static_cast<std::int64_t>(b);
    if (sum < 0) {
      return false;
    }
    const auto root =
        static_cast<std::int64_t>(std::sqrt(static_cast<double>(sum)));
    return (root * root) == sum || ((root + 1) * (root + 1)) == sum;
  }
};

LC_PROBLEM(996, "Number Of Squareful Arrays", "dfs", ctx) {
  Solution sol;

  ctx.test("example_1_three_elements", [&] {
    // Valid permutations: [1, 8, 17] and [17, 8, 1].
    ctx.expect_eq(sol.numSquarefulPerms({1, 17, 8}), 2);
  });

  ctx.test("example_2_all_duplicates_valid", [&] {
    // Only one distinct permutation [2, 2, 2], and 2 + 2 = 4 = 2^2.
    ctx.expect_eq(sol.numSquarefulPerms({2, 2, 2}), 1);
  });

  ctx.test("single_element_vacuously_squareful", [&] {
    ctx.expect_eq(sol.numSquarefulPerms({7}), 1);
  });

  ctx.test("no_valid_permutation", [&] {
    ctx.expect_eq(sol.numSquarefulPerms({1, 2, 4}), 0);
  });

  ctx.test("zeros_sum_to_zero_square", [&] {
    // 0 + 0 = 0^2 and 0 + 1 = 1^2, so all 4 distinct permutations are valid.
    ctx.expect_eq(sol.numSquarefulPerms({0, 0, 0, 1}), 4);
  });

  ctx.test("max_length_all_duplicates_pruning", [&] {
    // Verifies duplicate pruning handles n = 12 identical elements instantly.
    ctx.expect_eq(sol.numSquarefulPerms(std::vector<int>(12uz, 2)), 1);
  });

  ctx.test("large_values_no_overflow", [&] {
    // 10^9 + 10^9 = 2 * 10^9 (not a square); 500000000 + 500000000 = 10^9 (not
    // a square); 450000000 + 450000000 = 9 * 10^8 = 30000^2 (square).
    ctx.expect_eq(sol.numSquarefulPerms({450000000, 450000000}), 1);
  });
}

}  // namespace
