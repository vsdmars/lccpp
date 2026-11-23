// LeetCode 254: Factor Combinations
// https://leetcode.com/problems/factor-combinations/
//
// Problem:
//   Numbers can be regarded as the product of their factors (each strictly
//   greater than 1 and strictly less than `n`). Given an integer `n`, return
//   all possible combinations of its factors in any order.
//   Constraints: 1 <= n <= 10^7. Prime numbers and `n = 1` have no valid
//   factor combinations and return an empty list.
//
// Algorithm:
//   Backtracking with Square-Root Pruning and Non-Decreasing Invariant:
//   Maintain the current `remaining` quotient, the minimum allowed next factor
//   `start` (initially 2), and the partial `path` of factors chosen so far.
//   Iterate `factor` from `start` while `factor <= remaining / factor`
//   (overflow-safe equivalent of `factor * factor <= remaining`):
//     - If `remaining % factor == 0`:
//       1. Emit the valid completion `path + [factor, remaining / factor]`.
//          Because `factor <= remaining / factor`, the pair stays sorted and
//          both factors lie in `[2, n - 1]`.
//       2. Push `factor` onto `path`, recurse on `(remaining / factor, factor)`
//          to decompose `remaining / factor` further into factors >= `factor`,
//          and pop `factor` on return.
//   Invariant: enforcing `next_factor >= prev_factor` ensures every multiset
//   of factors is generated in non-decreasing order exactly once (e.g.,
//   `[2, 2, 3]` is emitted, while `[2, 3, 2]` and `[3, 2, 2]` are never visited).
//   Time Complexity:  O(n^(1/2) * #combinations) worst-case; bounded tightly
//                     because recursion depth is at most log2(n) and every
//                     divisor check stops at sqrt(remaining).
//   Space Complexity: O(log n) auxiliary space for the recursion stack and
//                     mutable `path` buffer (excluding output storage).
//
// Data Structures:
//   - Mutable factor buffer (`std::vector<int>`) pushed and popped in place to
//     avoid copying partial lists during DFS traversal.
//   - Output collection (`std::vector<std::vector<int>>`).
//
// Real-World Applications:
//   - Tensor reshaping and distributed array sharding: enumerating all valid
//     multi-dimensional grid shapes `(d1, d2, ..., dk)` whose product matches
//     a total number of elements or GPUs.
//   - FFT Cooley-Tukey radix decomposition: choosing mixed-radix factorizations
//     of transform length N for optimal cache and SIMD utilization.
//   - Cryptography and computer algebra systems exploring multiplicative
//     partitions of integers.
//
// Related LeetCode Problems:
//   - #39 Combination Sum [done] (additive counterpart with non-decreasing elements)
//   - #77 Combinations [done] (non-decreasing backtracking with start index)
//   - #78 Subsets [done] (DFS backtracking over ordered choices)
//   - #279 Perfect Squares [done] (minimum decomposition into square summands)
//   - #650 2 Keys Keyboard [done] (factorization structure of minimum copy-paste steps)

#include "framework/lc_framework.h"

#include <algorithm>
#include <vector>

namespace {

class Solution {
 public:
  // Returns all factor combinations of `n` where every factor lies in (1, n)
  // and each combination is in non-decreasing order.
  // Time: bounded by O(sqrt(n) * M) where M is the number of factorizations;
  // Space: O(log n) auxiliary stack/path space.
  std::vector<std::vector<int>> getFactors(int n) {
    std::vector<std::vector<int>> results;
    std::vector<int> path;

    // Backtracks over non-decreasing divisors `factor >= start` with
    // `factor * factor <= remaining` (written as `factor <= remaining / factor`
    // to avoid signed 32-bit integer overflow).
    auto dfs = [&](this auto&& self, int remaining, int start) -> void {
      for (int factor = start; factor <= remaining / factor; ++factor) {
        if (remaining % factor != 0) {
          continue;
        }
        const int quotient = remaining / factor;

        // Directly record the complete factorization ending in `(factor, quotient)`.
        path.push_back(factor);
        path.push_back(quotient);
        results.push_back(path);
        path.pop_back();

        // Explore further factorizations of `quotient` starting from `factor`
        // so all combinations stay non-decreasing (no duplicate permutations).
        self(quotient, factor);
        path.pop_back();
      }
    };

    dfs(n, 2);
    return results;
  }
};

LC_PROBLEM(254, "Factor Combinations", "backtracking", ctx) {
  Solution sol;

  ctx.test("base_and_prime_inputs", [&] {
    ctx.expect_true(sol.getFactors(1).empty());
    ctx.expect_true(sol.getFactors(2).empty());
    ctx.expect_true(sol.getFactors(37).empty());
  });

  ctx.test("example_12", [&] {
    auto actual = sol.getFactors(12);
    std::ranges::sort(actual);
    const std::vector<std::vector<int>> expected = {
        {2, 2, 3},
        {2, 6},
        {3, 4},
    };
    ctx.expect_eq(actual, expected);
  });

  ctx.test("example_32", [&] {
    auto actual = sol.getFactors(32);
    std::ranges::sort(actual);
    const std::vector<std::vector<int>> expected = {
        {2, 2, 2, 2, 2}, {2, 2, 2, 4}, {2, 2, 8},
        {2, 4, 4},       {2, 16},      {4, 8},
    };
    ctx.expect_eq(actual, expected);
  });

  ctx.test("example_8_and_perfect_square", [&] {
    auto actual_8 = sol.getFactors(8);
    std::ranges::sort(actual_8);
    const std::vector<std::vector<int>> expected_8 = {
        {2, 2, 2},
        {2, 4},
    };
    ctx.expect_eq(actual_8, expected_8);

    auto actual_9 = sol.getFactors(9);
    const std::vector<std::vector<int>> expected_9 = {{3, 3}};
    ctx.expect_eq(actual_9, expected_9);
  });
}

}  // namespace
