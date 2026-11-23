// LeetCode 15: 3Sum
// https://leetcode.com/problems/3sum/
//
// Problem:
//   Given an integer array `nums`, return all unique triplets
//   `[nums[i], nums[j], nums[k]]` such that `i != j`, `i != k`, `j != k`,
//   and `nums[i] + nums[j] + nums[k] == 0`. The result set must not contain
//   duplicate triplets.
//
// Examples:
//   Example 1:
//     Input:  nums = [-1, 0, 1, 2, -1, -4]
//     Output: [[-1, -1, 2], [-1, 0, 1]]
//     Explanation:
//       - Sort nums ascending to [-4, -1, -1, 0, 1, 2].
//       - For anchor i = 0 (-4), left starts at 1 (-1) and right at 5 (2); even at
//         left = 4 (1) and right = 5 (2) the sum -4 + 1 + 2 = -1 < 0, so no triplet
//         is found.
//       - For anchor i = 1 (-1), left = 2 (-1) and right = 5 (2) sum to 0,
//         recording [-1, -1, 2] and advancing to left = 3 (0), right = 4 (1), which
//         also sums to 0 and records [-1, 0, 1].
//       - Anchor i = 2 (-1) is skipped because nums[2] == nums[1], and anchor i = 3
//         (0) yields 0 + 1 + 2 = 3 > 0, returning [[-1, -1, 2], [-1, 0, 1]].
//
//   Example 2:
//     Input:  nums = [-2, 0, 0, 2, 2]
//     Output: [[-2, 0, 2]]
//     Explanation:
//       - The sorted array is [-2, 0, 0, 2, 2], and anchor i = 0 (-2) begins with
//         left = 1 (0) and right = 4 (2).
//       - The triplet -2 + 0 + 2 = 0 is recorded as [-2, 0, 2], then left
//         increments past the duplicate 0 at index 2 and right decrements past the
//         duplicate 2 at index 3 until left >= right.
//       - Anchor i = 1 (0) produces only sums >= 2 and anchor i = 2 (0) is skipped
//         as a duplicate, returning [[-2, 0, 2]].
//
//   Example 3:
//     Input:  nums = [0, 1, 1]
//     Output: []
//     Explanation:
//       - The sorted array remains [0, 1, 1], with last_anchor = nums.size() - 2 =
//         1.
//       - At anchor i = 0 (0), left = 1 (1) and right = 2 (1) give sum = 0 + 1 + 1
//         = 2 > 0, decrementing right to 1 and ending the two-pointer scan.
//       - The outer loop terminates with no zero-sum triplets found and returns [].
//
// Algorithm:
//   Sort-and-two-pointers. Sort `nums` in ascending order so duplicates sit
//   adjacent and target sums can be searched monotonically. Fix the smallest
//   element `nums[i]` in an outer loop:
//   - If `nums[i] > 0`, stop early because no three positive numbers sum to 0.
//   - Skip duplicate anchor values (`nums[i] == nums[i - 1]`).
//   For each unique anchor, scan the suffix `[i + 1, n - 1]` with two
//   converging pointers `left` and `right`:
//   - If the triplet sum is 0, record it and skip consecutive duplicates on
//     both `left` and `right`.
//   - If the sum is too large, decrement `right`; if too small, increment
//     `left`.
//   Time Complexity:  O(n^2) (O(n log n) sort + O(n^2) two-pointer scans).
//   Space Complexity: O(log n) to O(n) for sorting depending on implementation,
//                     excluding the output vector.
//
// Data Structures:
//   - Sorted `std::vector<int>`: enables O(1) duplicate skipping and linear
//     two-pointer pair search per fixed anchor.
//   - Result list (`std::vector<std::vector<int>>`): stores unique sorted
//     zero-sum triplets.
//
// Real-World Applications:
//   - Computational geometry (collinearity detection of points in 2D reduces
//     directly to the classic 3SUM problem).
//   - Financial ledger reconciliation (finding balancing multi-entry
//     transactions that net to zero).
//   - Portfolio hedging and chemical stoichiometry (selecting three signed
//     weights or charges that sum to equilibrium).
//
// Related LeetCode Problems:
//   - #1 Two Sum [done] (foundational hash-table pair sum problem)
//   - #16 3Sum Closest [done] (same sort + two-pointer scan minimizing distance to target)
//   - #18 4Sum [done] (extends sort + two pointers to quadruplets)
//   - #167 Two Sum II [done] - Input Array Is Sorted (inner two-pointer subproblem)
//   - #259 3Sum Smaller [done] (counts triplets whose sum is below a threshold)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Finds all unique triplets in `nums` that sum to zero by sorting and
  // reducing each distinct anchor to a linear two-pointer scan.
  //
  // Time: O(n^2), Space: O(log n) auxiliary (excluding output).
  std::vector<std::vector<int>> threeSum(std::vector<int> nums) {
    std::vector<std::vector<int>> results;
    if (nums.size() < 3uz) {
      return results;
    }

    std::ranges::sort(nums);

    const std::size_t last_anchor = nums.size() - 2uz;
    for (std::size_t i = 0uz; i < last_anchor; ++i) {
      // Since `nums` is sorted, a positive anchor can never reach sum 0.
      if (nums[i] > 0) {
        break;
      }

      // Skip identical anchor values to avoid producing duplicate triplets.
      if (i > 0uz && nums[i] == nums[i - 1uz]) {
        continue;
      }

      findTwoSumPairs(nums, i, results);
    }

    return results;
  }

 private:
  // Runs a converging two-pointer scan over `nums[anchor_idx + 1 .. n - 1]`
  // to collect all unique pairs summing to `-nums[anchor_idx]`.
  static void findTwoSumPairs(const std::vector<int>& nums,
                              std::size_t anchor_idx,
                              std::vector<std::vector<int>>& results) {
    const int anchor = nums[anchor_idx];
    std::size_t left = anchor_idx + 1uz;
    std::size_t right = nums.size() - 1uz;

    while (left < right) {
      const int sum = anchor + nums[left] + nums[right];

      if (sum == 0) {
        results.push_back({anchor, nums[left], nums[right]});
        ++left;
        --right;

        // Skip duplicate values on both sides so every recorded triplet is
        // strictly distinct.
        while (left < right && nums[left] == nums[left - 1uz]) {
          ++left;
        }
        while (left < right && nums[right] == nums[right + 1uz]) {
          --right;
        }
      } else if (sum > 0) {
        --right;
      } else {
        ++left;
      }
    }
  }
};

LC_PROBLEM(15, "3Sum", "two_pointers", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    const std::vector<std::vector<int>> expected = {{-1, -1, 2}, {-1, 0, 1}};
    ctx.expect_eq(sol.threeSum({-1, 0, 1, 2, -1, -4}), expected);
  });

  ctx.test("duplicate_zeros_and_twos", [&] {
    const std::vector<std::vector<int>> expected = {{-2, 0, 2}};
    ctx.expect_eq(sol.threeSum({-2, 0, 0, 2, 2}), expected);
  });

  ctx.test("all_zeros", [&] {
    const std::vector<std::vector<int>> expected = {{0, 0, 0}};
    ctx.expect_eq(sol.threeSum({0, 0, 0}), expected);
    ctx.expect_eq(sol.threeSum({0, 0, 0, 0}), expected);
  });

  ctx.test("no_solution_and_short_inputs", [&] {
    const std::vector<std::vector<int>> empty;
    ctx.expect_eq(sol.threeSum({0, 1, 1}), empty);
    ctx.expect_eq(sol.threeSum({1, 2, 3, 4}), empty);
    ctx.expect_eq(sol.threeSum({1, -1}), empty);
    ctx.expect_eq(sol.threeSum({}), empty);
  });
}

}  // namespace
