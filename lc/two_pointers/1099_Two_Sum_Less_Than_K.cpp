// LeetCode 1099: Two Sum Less Than K
// https://leetcode.com/problems/two-sum-less-than-k/
//
// Problem:
//   Given an integer array `nums` and an integer `k`, return the maximum sum
//   `nums[i] + nums[j]` over all index pairs `i < j` such that
//   `nums[i] + nums[j] < k`. If no such pair exists, return `-1`.
//   Constraints: 1 <= nums.length <= 100, 1 <= nums[i] <= 1000, 1 <= k <= 2000.
//
// Algorithm:
//   Sort + converging two pointers:
//   1. If `nums.size() < 2`, no pair exists; return `-1`.
//   2. Sort `nums` in non-decreasing order.
//   3. Initialize `left = 0` and `right = nums.size() - 1`, and `best = -1`.
//   4. While `left < right`:
//      - Compute `sum = nums[left] + nums[right]`.
//      - If `sum < k`, `sum` is the largest valid pair sum achievable with
//        `nums[left]` (since `right` is at the maximum feasible partner), so
//        update `best = max(best, sum)` and increment `left` to try larger sums.
//      - Otherwise (`sum >= k`), `nums[right]` is too large even when paired
//        with the smallest remaining value `nums[left]`, so decrement `right`.
//   Time Complexity:  O(N log N) for sorting, plus O(N) two-pointer scan.
//   Space Complexity: O(log N) stack space for `std::ranges::sort` (in-place).
//
// Data Structures:
//   - Sorted `std::vector<int>` (`nums`) traversed by two converging pointers
//     (`left`, `right`).
//
// Real-World Applications:
//   - E-commerce bundle/coupon optimization (choosing two items whose combined
//     price stays strictly below a budget or gift-card threshold).
//   - Media/flight playlist pairing (selecting two tracks/movies whose total
//     duration fits strictly under a trip duration).
//   - Network packet aggregation (pairing two payloads under a strict MTU
//     ceiling).
//
// Related LeetCode Problems:
//   - #1 Two Sum [done] (exact target pair sum via hash map)
//   - #167 Two Sum II [done] - Input Array Is Sorted (converging two pointers for exact sum)
//   - #259 3Sum Smaller [done] (counting triples with sum strictly less than target)
//   - #881 Boats To Save People [done] (sorted two-pointer pairing under a weight ceiling)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the maximum pair sum strictly less than `k`, or -1 if no pair fits.
  // Invariant: Every pair `(p, q)` with `p < left` or `q > right` has either
  // already been accounted for in `best` or has sum >= `k`.
  // Time: O(N log N), Space: O(log N) for in-place sort.
  int twoSumLessThanK(std::vector<int> nums, int k) {
    if (nums.size() < 2uz) {
      return -1;
    }

    std::ranges::sort(nums);

    std::size_t left = 0uz;
    std::size_t right = nums.size() - 1uz;
    int best = -1;

    while (left < right) {
      const int sum = nums[left] + nums[right];
      if (sum < k) {
        // `nums[right]` is the largest valid partner for `nums[left]`; record
        // this sum and advance `left` to explore strictly larger candidates.
        best = std::max(best, sum);
        ++left;
      } else {
        // Even with the smallest remaining `nums[left]`, `nums[right]` is too
        // large, so eliminate `nums[right]`.
        --right;
      }
    }

    return best;
  }
};

LC_PROBLEM(1099, "Two Sum Less Than K", "two_pointers", ctx) {
  Solution sol;

  ctx.test("example_1_best_is_58", [&] {
    ctx.expect_eq(sol.twoSumLessThanK({34, 23, 1, 24, 75, 33, 54, 8}, 60), 58);
  });

  ctx.test("example_2_no_valid_pair", [&] {
    ctx.expect_eq(sol.twoSumLessThanK({10, 20, 30}, 15), -1);
  });

  ctx.test("strict_inequality_excludes_exact_k", [&] {
    // 10 + 20 == 30 is NOT strictly less than 30; best must be -1.
    ctx.expect_eq(sol.twoSumLessThanK({10, 20}, 30), -1);
    ctx.expect_eq(sol.twoSumLessThanK({10, 20}, 31), 30);
  });

  ctx.test("single_element_and_duplicates", [&] {
    ctx.expect_eq(sol.twoSumLessThanK({5}, 100), -1);
    ctx.expect_eq(sol.twoSumLessThanK({5, 5, 5}, 11), 10);
    ctx.expect_eq(sol.twoSumLessThanK({5, 5, 5}, 10), -1);
  });
}

}  // namespace
