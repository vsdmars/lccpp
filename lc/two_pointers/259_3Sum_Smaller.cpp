// LeetCode 259: 3Sum Smaller
// https://leetcode.com/problems/3sum-smaller/
//
// Problem:
//   Given an integer array `nums` of length n and an integer `target`, return
//   the number of index triplets (i, j, k) with 0 <= i < j < k < n such that
//   nums[i] + nums[j] + nums[k] < target. Duplicate values at distinct indices
//   count as distinct triplets.
//
// Examples:
//   Example 1:
//     Input:  nums = [-2, 0, 1, 3], target = 2
//     Output: 2
//     Explanation:
//       - nums is already sorted as [-2, 0, 1, 3] and count starts at 0.
//       - For k = 2 (nums[2] = 1) with left = 0, right = 1, sum = -2 + 0 + 1 = -1 <
//         2 adds right - left = 1 to count and advances left to 1, ending the inner
//         loop.
//       - For k = 3 (nums[3] = 3) with left = 0, right = 2, sum = -2 + 1 + 3 = 2 is
//         not < 2, so right decrements to 1.
//       - With left = 0, right = 1, sum = -2 + 0 + 3 = 1 < 2 adds 1 - 0 = 1 to
//         count (now 2) and advances left to 1, returning 2.
//
//   Example 2:
//     Input:  nums = [-1, 1, -1, -1], target = -1
//     Output: 1
//     Explanation:
//       - Sorting nums in-place produces [-1, -1, -1, 1].
//       - For k = 2 (nums[2] = -1) with left = 0, right = 1, sum = -1 + (-1) + (-1)
//         = -3 < -1 adds 1 - 0 = 1 to count and advances left to 1.
//       - For k = 3 (nums[3] = 1), left = 0, right = 2 yields sum = -1 + (-1) + 1 =
//         -1 (not < -1), decrementing right to 1; left = 0, right = 1 again yields
//         sum = -1 and decrements right to 0, returning 1.
//
//   Example 3:
//     Input:  nums = [1, 2, 3], target = 6
//     Output: 0
//     Explanation:
//       - nums = [1, 2, 3] is already sorted, leaving only k = 2 (nums[2] = 3) with
//         left = 0 and right = 1.
//       - The single candidate triplet gives sum = 1 + 2 + 3 = 6, which fails the
//         strict check sum < 6 and decrements right to 0, returning count = 0.
//
// Algorithm:
//   Sort `nums` in ascending order (triplet counts depend only on values, not
//   original index ordering, because any distinct triple of indices maps
//   uniquely to one sorted order i < j < k). Then fix the largest index `k`
//   from 2 to n - 1 and run an inward two-pointer sweep over [0, k - 1] with
//   `left = 0` and `right = k - 1`:
//     - If nums[left] + nums[right] + nums[k] < target, then because the array
//       is sorted, every middle index m in (left, right] also satisfies
//       nums[left] + nums[m] + nums[k] <= nums[left] + nums[right] + nums[k]
//       < target. Add all (right - left) valid triplets in O(1) and advance
//       `left` to explore larger first elements.
//     - Otherwise the sum is at least `target`, so decrement `right` to reduce
//       the sum.
//   Time complexity: O(n^2) (O(n log n) sort plus O(n^2) two-pointer sweeps).
//   Space complexity: O(1) auxiliary space (in-place sort, O(log n) stack).
//
// Data Structures:
//   - Sorted std::vector<int>: enables monotonicity so an entire range of valid
//     pairs can be counted in constant time without enumerating each triplet.
//   - Two index pointers (`left`, `right`): shrink the search interval linearly
//     for each fixed upper element `k`.
//
// Real-World Applications:
//   - Budget-constrained bundle selection in e-commerce or cloud resource
//     allocation (counting valid 3-item combinations under a cost ceiling).
//   - Portfolio risk screening: counting 3-asset combinations whose combined
//     exposure or variance proxy stays below a regulatory threshold.
//   - Geometric and scheduling feasibility checks (e.g., counting valid triples
//     of job durations that fit within a single time window).
//
// Related LeetCode Problems:
//   - #15 3Sum [done] (exact sum == 0 with deduplication via sort + two pointers)
//   - #16 3Sum Closest [done] (minimize |sum - target| using the same sweep)
//   - #18 4Sum [done] (extends sorted k-sum two-pointer reduction to quadruplets)
//   - #611 Valid Triangle Number [done] (counts triplets satisfying a + b > c)
//   - #2824 Count Pairs Whose Sum is Less than Target [new] (2Sum analogue)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Counts index triplets (i, j, k) with i < j < k whose sum is strictly less
  // than `target`.
  //
  // Sorts `nums` in-place and, for each upper element index `k`, uses inward
  // two pointers (`left`, `right`) to batch-count all valid pairs in [0, k - 1].
  //
  // Time: O(n^2), Space: O(1) auxiliary (O(log n) sort stack).
  int threeSumSmaller(std::vector<int>& nums, int target) {
    if (nums.size() < 3) {
      return 0;
    }

    // Sorting preserves the exact multiset of values; every unordered triple of
    // distinct indices maps bijectively to one ordered triple i < j < k.
    std::ranges::sort(nums);

    const auto target64 = static_cast<std::int64_t>(target);
    int count = 0;

    // Fix the largest element at index `k` and search pairs in [0, k - 1].
    for (std::size_t k = 2uz; k < nums.size(); ++k) {
      std::size_t left = 0;
      std::size_t right = k - 1;

      // Invariant: every valid triplet (i, j, k) with i < left or j > right has
      // already been counted or ruled out.
      while (left < right) {
        // Accumulate in 64-bit integer to avoid signed overflow on extreme values.
        const std::int64_t sum = static_cast<std::int64_t>(nums[left]) +
                                 static_cast<std::int64_t>(nums[right]) +
                                 static_cast<std::int64_t>(nums[k]);
        if (sum < target64) {
          // Since nums is sorted, every j in (left, right] also satisfies
          // nums[left] + nums[j] + nums[k] < target.
          count += static_cast<int>(right - left);
          ++left;
        } else {
          // Sum is too large; decrement `right` to try a smaller middle element.
          --right;
        }
      }
    }

    return count;
  }
};

LC_PROBLEM(259, "3Sum Smaller", "two_pointers", ctx) {
  Solution sol;

  ctx.test("example_basic", [&] {
    std::vector<int> nums{-2, 0, 1, 3};
    ctx.expect_eq(sol.threeSumSmaller(nums, 2), 2);
  });

  ctx.test("fewer_than_three_elements", [&] {
    std::vector<int> empty{};
    std::vector<int> single{0};
    std::vector<int> pair{1, 2};
    ctx.expect_eq(sol.threeSumSmaller(empty, 0), 0);
    ctx.expect_eq(sol.threeSumSmaller(single, 0), 0);
    ctx.expect_eq(sol.threeSumSmaller(pair, 10), 0);
  });

  ctx.test("strict_inequality_boundary", [&] {
    std::vector<int> nums{1, 2, 3};
    ctx.expect_eq(sol.threeSumSmaller(nums, 6), 0);
    ctx.expect_eq(sol.threeSumSmaller(nums, 7), 1);
  });

  ctx.test("duplicates_and_negatives", [&] {
    std::vector<int> nums{-1, 1, -1, -1};
    // Only (-1, -1, -1) sums to -3 < -1; the three (-1, -1, 1) triplets sum to -1.
    ctx.expect_eq(sol.threeSumSmaller(nums, -1), 1);
  });

  ctx.test("all_triplets_qualify", [&] {
    std::vector<int> nums{1, 1, 1, 1};
    // C(4, 3) = 4 triplets, all summing to 3 < 5.
    ctx.expect_eq(sol.threeSumSmaller(nums, 5), 4);
  });

  ctx.test("all_negative_values", [&] {
    std::vector<int> nums{-5, -4, -3, -2, -1};
    ctx.expect_eq(sol.threeSumSmaller(nums, -8), 6);
  });
}

}  // namespace
