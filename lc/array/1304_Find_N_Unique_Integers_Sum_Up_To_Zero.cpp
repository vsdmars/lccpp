// LeetCode 1304: Find N Unique Integers Sum Up To Zero
// https://leetcode.com/problems/find-n-unique-integers-sum-up-to-zero/
//
// Problem:
//   Given an integer `n`, return any array containing `n` pairwise distinct
//   integers whose sum equals 0.
//   Constraints: 1 <= n <= 1000.
//
// Examples:
//   Example 1:
//     Input:  n = 5
//     Output: [0, 1, -1, 2, -2]
//     Explanation:
//       - Reserve result with capacity 5 and check parity: since 5 % 2 != 0, push 0
//         so result = [0].
//       - Compute half = 5 / 2 = 2 and iterate i = 1..2, appending (+1, -1) and
//         (+2, -2).
//       - Return [0, 1, -1, 2, -2], containing 5 distinct integers that sum to 0.
//
//   Example 2:
//     Input:  n = 4
//     Output: [1, -1, 2, -2]
//     Explanation:
//       - Reserve result with capacity 4; since 4 % 2 == 0, skip pushing 0.
//       - Compute half = 4 / 2 = 2 and iterate i = 1..2, appending (1, -1) and (2,
//         -2).
//       - Return [1, -1, 2, -2], containing 4 unique integers summing to 0.
//
//   Example 3:
//     Input:  n = 1
//     Output: [0]
//     Explanation:
//       - Reserve result with capacity 1; since 1 % 2 != 0, push 0 so result = [0].
//       - Compute half = 1 / 2 = 0, so the pair-generation loop runs zero times and
//         returns [0].
//
// Algorithm:
//   Symmetric Opposite-Pair Construction:
//   - Reserve an output vector of size `n`.
//   - If `n` is odd (`n % 2 != 0`), include `0` so the remaining count is even.
//   - For `i = 1, 2, ..., n / 2`, append both `+i` and `-i`.
//   Each pair `(+i, -i)` sums to 0, every element is strictly distinct because
//   all positive values, all negative values, and 0 occupy disjoint ranges, and
//   the total length is `(n % 2) + 2 * (n / 2) == n`.
//   Time Complexity:  O(n) to populate the output array.
//   Space Complexity: O(1) auxiliary space beyond the returned vector of size n.
//
// Data Structures:
//   - `std::vector<int>`: pre-reserved output container holding the symmetric
//     zero-sum elements.
//
// Real-World Applications:
//   - Synthetic test-data generation for double-entry accounting and financial
//     reconciliation systems (creating balanced debit/credit transaction sets).
//   - Zero-mean signal and numerical-filter kernel construction (e.g., balanced
//     finite-difference stencils or baseline-neutral calibration offsets).
//   - Load-balancing and differential benchmark fixtures that require
//     net-zero delta sequences with unique keys.
//
// Related LeetCode Problems:
//   - #1 Two Sum [done] (finding opposite/complement pairs summing to a target)
//   - #15 3Sum [done] (finding triplets summing to zero)
//   - #454 4Sum II [done] (counting tuples summing to zero)
//   - #1317 Convert Integer to the Sum of Two No-Zero Integers [new] (constructive integer decomposition)

#include "framework/lc_framework.h"

#include <cstddef>
#include <numeric>
#include <unordered_set>
#include <vector>

namespace {

class Solution {
 public:
  // Constructs `n` unique integers whose sum is 0 using symmetric `(+i, -i)`
  // pairs (plus `0` when `n` is odd).
  // Time: O(n), Space: O(1) auxiliary.
  std::vector<int> sumZero(int n) {
    if (n <= 0) {
      return {};
    }

    std::vector<int> result;
    result.reserve(static_cast<std::size_t>(n));

    // When `n` is odd, include 0 so the remaining elements form exact pairs.
    if ((n % 2) != 0) {
      result.push_back(0);
    }

    // Add `n / 2` symmetric pairs `(+i, -i)` whose pairwise sums cancel to 0.
    const int half = n / 2;
    for (int i = 1; i <= half; ++i) {
      result.push_back(i);
      result.push_back(-i);
    }

    return result;
  }
};

// Verifies the problem contract: exact length `n`, all unique, and sum == 0.
void expectValidZeroSum(lc::TestContext& ctx, const std::vector<int>& result,
                        int n) {
  ctx.expect_eq(result.size(), static_cast<std::size_t>(n));
  const std::unordered_set<int> unique_vals(result.begin(), result.end());
  ctx.expect_eq(unique_vals.size(), static_cast<std::size_t>(n));
  const int total = std::accumulate(result.begin(), result.end(), 0);
  ctx.expect_eq(total, 0);
}

LC_PROBLEM(1304, "Find N Unique Integers Sum Up To Zero", "array", ctx) {
  Solution sol;

  ctx.test("example_1_build_n_5", [&] {
    expectValidZeroSum(ctx, sol.sumZero(5), 5);
  });

  ctx.test("example_2_n_3", [&] {
    expectValidZeroSum(ctx, sol.sumZero(3), 3);
  });

  ctx.test("example_3_n_1", [&] {
    const std::vector<int> res = sol.sumZero(1);
    ctx.expect_eq(res, std::vector<int>{0});
    expectValidZeroSum(ctx, res, 1);
  });

  ctx.test("even_sizes", [&] {
    expectValidZeroSum(ctx, sol.sumZero(2), 2);
    expectValidZeroSum(ctx, sol.sumZero(4), 4);
    expectValidZeroSum(ctx, sol.sumZero(100), 100);
  });

  ctx.test("max_constraint_n_1000", [&] {
    expectValidZeroSum(ctx, sol.sumZero(1000), 1000);
  });
}

}  // namespace
