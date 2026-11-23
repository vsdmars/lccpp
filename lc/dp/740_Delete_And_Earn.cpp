// LeetCode 740: Delete And Earn
// https://leetcode.com/problems/delete-and-earn/
//
// Problem:
//   Given an integer array `nums`, you may repeatedly choose any element `x`,
//   delete it to earn `x` points, and simultaneously forfeit (delete) every
//   element equal to `x - 1` or `x + 1`. Return the maximum total points you
//   can earn.
//
// Examples:
//   Example 1:
//     Input:  nums = [3, 4, 2]
//     Output: 6
//     Explanation:
//       - Find max_val = 4 and accumulate value buckets into points = [0, 0, 2, 3,
//         4] for values 0..4.
//       - Process buckets 0 and 1 (gain = 0), keeping rolling state (prev, curr) =
//         (0, 0).
//       - Process bucket 2 (gain = 2): next = max(0, 0 + 2) = 2, updating (prev,
//         curr) to (0, 2).
//       - Process bucket 3 (gain = 3): next = max(2, 0 + 3) = 3, updating (prev,
//         curr) to (2, 3).
//       - Process bucket 4 (gain = 4): next = max(3, 2 + 4) = 6, updating curr = 6
//         and returning 6.
//
//   Example 2:
//     Input:  nums = [2, 2, 3, 3, 3, 4]
//     Output: 9
//     Explanation:
//       - Aggregate identical values into points = [0, 0, 4, 9, 4] up to max_val =
//         4.
//       - Keep (prev, curr) = (0, 0) through buckets 0 and 1, then at bucket 2
//         (gain = 4) set next = max(0, 0 + 4) = 4 so (prev, curr) = (0, 4).
//       - At bucket 3 (gain = 9), compute next = max(4, 0 + 9) = 9, updating (prev,
//         curr) to (4, 9).
//       - At bucket 4 (gain = 4), compute next = max(9, 4 + 4) = 9, keeping curr =
//         9 and returning 9.
//
//   Example 3:
//     Input:  nums = []
//     Output: 0
//     Explanation:
//       - Check nums.empty(), which evaluates to true for the empty input vector.
//       - Return 0 immediately without allocating the value-bucket vector or
//         executing the DP loop.
//
// Algorithm:
//   Observe that once you choose one copy of value `v`, all copies of `v - 1`
//   and `v + 1` are already eliminated, so it is always optimal to take every
//   remaining copy of `v` as well. Aggregating `points[v] = v * count(v)` over
//   the value domain `0..max(nums)` transforms the problem directly into House
//   Robber (#198): pick a subset of non-adjacent indices maximizing the sum.
//   Recurrence: `dp[v] = max(dp[v - 1], dp[v - 2] + points[v])`, computed in
//   O(1) extra rolling state (`prev`, `curr`).
//   Time complexity: O(N + M), where N = nums.size() and M = max(nums).
//   Space complexity: O(M) for the value-bucket points array.
//
// Data Structures:
//   - std::vector<int>: Bucket array `points` indexed by integer value `v`
//     storing the total points earned if value `v` is selected.
//   - Two scalar rolling DP variables (`prev`, `curr`) tracking `dp[v - 2]`
//     and `dp[v - 1]`.
//
// Real-World Applications:
//   - Non-adjacent resource/slot scheduling where choosing a time bucket or
//     frequency band blocks immediately neighboring bands (wireless spectrum
//     channel allocation).
//   - Conflict-aware bid or promotion selection on discretized price tiers.
//   - Weighted independent set on path graphs in compiler instruction
//     scheduling and layout optimization.
//
// Related LeetCode Problems:
//   - #198 House Robber [done] (canonical non-adjacent linear DP reduction)
//   - #213 House Robber II [done] (circular variant of non-adjacent DP)
//   - #337 House Robber III [new] (tree-structured independent set DP)
//   - #3186 Maximum Total Damage With Spell Casting [new] (generalized +/-2 gap)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the maximum points obtainable by deleting values while discarding
  // adjacent values `v - 1` and `v + 1`.
  //
  // Invariant: Before processing bucket `v`, `curr` holds the best score using
  // values in `[0, v - 1]` and `prev` holds the best score using `[0, v - 2]`.
  // Time: O(N + M); Space: O(M), where M = max(nums).
  int deleteAndEarn(const std::vector<int>& nums) {
    if (nums.empty()) {
      return 0;
    }

    // Accumulate total earned points per distinct value bucket.
    const int max_val = std::ranges::max(nums);
    std::vector<int> points(static_cast<std::size_t>(max_val) + 1uz, 0);
    for (const int num : nums) {
      points[static_cast<std::size_t>(num)] += num;
    }

    // House Robber rolling DP over value buckets 0..max_val:
    // either skip `v` (keep `curr`) or take all copies of `v` (`prev + gain`).
    int prev = 0;
    int curr = 0;
    for (const int gain : points) {
      const int next = std::max(curr, prev + gain);
      prev = curr;
      curr = next;
    }

    return curr;
  }
};

LC_PROBLEM(740, "Delete And Earn", "dp", ctx) {
  Solution sol;

  ctx.test("example_1_distinct_values", [&] {
    ctx.expect_eq(sol.deleteAndEarn({3, 4, 2}), 6);
  });

  ctx.test("example_2_duplicate_middle_wins", [&] {
    ctx.expect_eq(sol.deleteAndEarn({2, 2, 3, 3, 3, 4}), 9);
  });

  ctx.test("single_element", [&] {
    ctx.expect_eq(sol.deleteAndEarn({7}), 7);
  });

  ctx.test("all_identical_elements", [&] {
    ctx.expect_eq(sol.deleteAndEarn({4, 4, 4, 4}), 16);
  });

  ctx.test("non_adjacent_values_take_all", [&] {
    ctx.expect_eq(sol.deleteAndEarn({1, 3, 5, 7}), 16);
  });

  ctx.test("empty_input", [&] {
    ctx.expect_eq(sol.deleteAndEarn({}), 0);
  });
}

}  // namespace
