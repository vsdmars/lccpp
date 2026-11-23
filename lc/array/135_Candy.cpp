// LeetCode 135: Candy
// https://leetcode.com/problems/candy/
//
// Problem:
//   There are n children standing in a line, each assigned an integer rating
//   in the array `ratings`. Distribute candies to the children subject to two
//   rules:
//     1. Every child must receive at least 1 candy.
//     2. Any child with a strictly higher rating than an immediate neighbor
//        must receive strictly more candies than that neighbor.
//   Return the minimum total number of candies required.
//
// Examples:
//   Example 1:
//     Input:  ratings = [1, 0, 2]
//     Output: 5
//     Explanation:
//       - Initialize candies = [1, 1, 1] so every child has at least 1 candy.
//       - Left-to-right pass leaves index 1 at 1 (0 <= 1) and increments index 2 (2
//         > 0) to candies = [1, 1, 2].
//       - Right-to-left pass leaves index 1 at 1 (0 <= 2) and updates index 0 (1 >
//         0) to max(1, 1 + 1) = 2, giving [2, 1, 2].
//       - Sum candies to return 2 + 1 + 2 = 5.
//
//   Example 2:
//     Input:  ratings = [1, 2, 2]
//     Output: 4
//     Explanation:
//       - Initialize candies = [1, 1, 1].
//       - Left-to-right pass sets candies[1] = 2 (2 > 1) and leaves candies[2] = 1
//         because 2 <= 2, giving [1, 2, 1].
//       - Right-to-left pass makes no changes since neither 2 > 2 nor 1 > 2 holds,
//         and summing [1, 2, 1] returns 4.
//
//   Example 3:
//     Input:  ratings = [1, 3, 4, 5, 2]
//     Output: 11
//     Explanation:
//       - Initialize candies = [1, 1, 1, 1, 1].
//       - Left-to-right pass builds the ascending run 1 < 3 < 4 < 5 into [1, 2, 3,
//         4, 1], leaving index 4 at 1 (2 <= 5).
//       - Right-to-left pass checks index 3 (5 > 2) and keeps max(4, 1 + 1) = 4,
//         preserving the peak; earlier pairs do not decrease left.
//       - Sum [1, 2, 3, 4, 1] to return 11.
//
// Algorithm:
//   Two-Pass Greedy Scan:
//     Initialize every child with 1 candy to satisfy the baseline constraint.
//     Decompose the bilateral neighbor condition into two independent one-sided
//     constraints:
//       - Left-to-right pass: enforce the left-neighbor constraint. Whenever
//         `ratings[i] > ratings[i - 1]`, set `candies[i] = candies[i - 1] + 1`.
//       - Right-to-left pass: enforce the right-neighbor constraint without
//         violating the left-neighbor constraint. Whenever
//         `ratings[i] > ratings[i + 1]`, set
//         `candies[i] = max(candies[i], candies[i + 1] + 1)`.
//     Because taking the maximum only ever increases a child's allocation when
//     forced by a descending slope, every left-neighbor inequality already
//     established in pass 1 remains valid. Summing `candies` gives the global
//     minimum.
//   Time Complexity:  O(n), where n = ratings.size() (two linear passes + sum).
//   Space Complexity: O(n) for the per-child candy count array.
//
// Data Structures:
//   - std::vector<int> candies: stores the minimal candy allocation for each
//     child across the two greedy passes.
//
// Real-World Applications:
//   - Fair tier-based compensation and bonus allocation with relative ranking
//     invariants among adjacent employees or cohorts.
//   - Signal smoothing and monotonic constraint satisfaction in control
//     systems (e.g., velocity profile planning with forward/backward braking
//     passes along a track).
//   - Priority-weighted resource provisioning along linear network topologies.
//
// Related LeetCode Problems:
//   - #42 Trapping Rain Water [done] (prefix/suffix two-pass constraint scan)
//   - #134 Gas Station [done] (linear greedy invariant over an array)
//   - #238 Product of Array Except Self [done] (left-to-right and right-to-left passes)
//   - #455 Assign Cookies [new] (greedy allocation with threshold constraints)
//   - #2100 Find Good Days to Rob the Bank [new] (bidirectional monotonic run counts)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the minimum total candies needed to satisfy both neighbor rules.
  // Invariant after pass 1: for all i > 0, ratings[i] > ratings[i - 1] implies
  // candies[i] > candies[i - 1].
  // Invariant after pass 2: both left- and right-neighbor rules hold at every
  // index with the smallest possible integer values >= 1.
  // Time: O(n), Space: O(n).
  int candy(const std::vector<int>& ratings) {
    const std::size_t n = ratings.size();
    if (n == 0uz) {
      return 0;
    }

    // Rule 1 baseline: every child receives at least one candy.
    std::vector<int> candies(n, 1);

    // Pass 1 (left -> right): guarantee strictly higher rating than the left
    // neighbor receives at least one more candy than that left neighbor.
    for (std::size_t i = 1uz; i < n; ++i) {
      if (ratings[i] > ratings[i - 1]) {
        candies[i] = candies[i - 1] + 1;
      }
    }

    // Pass 2 (right -> left): guarantee strictly higher rating than the right
    // neighbor receives at least one more candy, while preserving Pass 1 via
    // std::max so peaks satisfy both ascending and descending slopes.
    for (std::size_t i = n - 1; i > 0uz; --i) {
      const std::size_t left = i - 1;
      if (ratings[left] > ratings[i]) {
        candies[left] = std::max(candies[left], candies[i] + 1);
      }
    }

    // Accumulate the minimal total candies distributed.
    int total = 0;
    for (const int count : candies) {
      total += count;
    }
    return total;
  }
};

LC_PROBLEM(135, "Candy", "array", ctx) {
  Solution sol;

  ctx.test("example_1_valley", [&] {
    // Optimal distribution: [2, 1, 2] -> 5.
    ctx.expect_eq(sol.candy({1, 0, 2}), 5);
  });

  ctx.test("example_2_plateau", [&] {
    // Equal adjacent ratings need not get equal candies: [1, 2, 1] -> 4.
    ctx.expect_eq(sol.candy({1, 2, 2}), 4);
  });

  ctx.test("empty_and_single", [&] {
    ctx.expect_eq(sol.candy({}), 0);
    ctx.expect_eq(sol.candy({7}), 1);
  });

  ctx.test("strictly_monotonic", [&] {
    // [1, 2, 3, 4] -> 10, [4, 3, 2, 1] -> 10.
    ctx.expect_eq(sol.candy({1, 2, 3, 4}), 10);
    ctx.expect_eq(sol.candy({4, 3, 2, 1}), 10);
  });

  ctx.test("all_equal_ratings", [&] {
    ctx.expect_eq(sol.candy({5, 5, 5, 5}), 4);
  });

  ctx.test("asymmetric_peak_and_plateau", [&] {
    // Distribution: [1, 2, 3, 1, 3, 2, 1] -> 13.
    ctx.expect_eq(sol.candy({1, 2, 87, 87, 87, 2, 1}), 13);
    // Distribution: [1, 2, 3, 4, 1] -> 11.
    ctx.expect_eq(sol.candy({1, 3, 4, 5, 2}), 11);
  });
}

}  // namespace
