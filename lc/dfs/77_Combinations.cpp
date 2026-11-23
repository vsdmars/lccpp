// LeetCode 77: Combinations
// https://leetcode.com/problems/combinations/
//
// Problem:
//   Given two integers `n` and `k`, return all possible combinations of `k`
//   numbers chosen from the inclusive range [1, n]. You may return the answer
//   in any order.
//   Constraints: 1 <= n <= 20, 1 <= k <= n.
//
// Algorithm:
//   Backtracking DFS with upper-bound branch pruning:
//     - Maintain `current` (strictly increasing chosen integers) and recurse
//       with `(start, remaining)`, where `start` is the smallest integer still
//       eligible to be picked and `remaining` is how many numbers are left to
//       select.
//     - Base case: when `remaining == 0`, snapshot `current` into `results`.
//     - Upper-bound pruning: to choose `remaining` distinct integers from
//       `[num, n]`, there must be at least `remaining` values left, meaning
//       `n - num + 1 >= remaining`, i.e., `num <= (n - remaining) + 1`.
//       Looping only up to `max_start = (n - remaining) + 1` eliminates every
//       dead branch that cannot possibly reach size `k`.
//   Time Complexity:  O(k * C(n, k)) — each of the C(n, k) combinations is
//                     copied in O(k) time, and pruning guarantees no wasted
//                     leaves.
//   Space Complexity: O(k) recursion depth and path buffer (excluding output).
//
// Data Structures:
//   - Path stack (`std::vector<int>`): stores the current combination via O(1)
//     push_back / pop_back, pre-reserved to size `k`.
//   - Result list (`std::vector<std::vector<int>>`): collects all C(n, k)
//     combinations.
//
// Real-World Applications:
//   - Feature-subset selection and k-variable ablation studies in machine
//     learning and experimental design.
//   - Pairwise and k-way combinatorial software testing (generating k-tuples
//     of configuration flags or API parameters).
//   - Portfolio subset screening (evaluating all k-asset portfolios from an
//     n-asset universe).
//   - Distributed systems quorum and committee enumeration (choosing k replicas
//     out of n nodes).
//
// Related LeetCode Problems:
//   - #39 Combination Sum [done] (combinations summing to a target with reuse)
//   - #40 Combination Sum II [done] (unique combinations summing to a target)
//   - #46 Permutations [done] (ordered arrangements of all elements)
//   - #78 Subsets [done] (union of combinations across all sizes k = 0..n)
//   - #216 Combination Sum III [new] (k-combinations from 1..9 with target sum)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Returns all C(n, k) combinations of `k` integers chosen from [1, n].
  // Time: O(k * C(n, k)), Space: O(k) auxiliary (excluding output).
  std::vector<std::vector<int>> combine(int n, int k) {
    if (n < 0 || k < 0 || k > n) {
      return {};
    }

    std::vector<std::vector<int>> results;
    std::vector<int> current;
    current.reserve(static_cast<std::size_t>(k));

    // DFS invariant: `current` holds `k - remaining` strictly increasing
    // values in [1, start - 1].
    auto dfs = [&](this auto&& self, int start, int remaining) -> void {
      if (remaining == 0) {
        results.push_back(current);
        return;
      }

      // Pruning bound: picking `num > (n - remaining) + 1` leaves fewer than
      // `remaining` values in [num, n], making completion impossible.
      const int max_start = (n - remaining) + 1;
      for (int num = start; num <= max_start; ++num) {
        current.push_back(num);
        self(num + 1, remaining - 1);
        current.pop_back();
      }
    };

    dfs(1, k);
    return results;
  }
};

LC_PROBLEM(77, "Combinations", "dfs", ctx) {
  Solution sol;

  ctx.test("example_1_n4_k2", [&] {
    const std::vector<std::vector<int>> expected = {
        {1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4},
    };
    ctx.expect_eq(sol.combine(4, 2), expected);
  });

  ctx.test("example_2_n1_k1", [&] {
    const std::vector<std::vector<int>> expected = {{1}};
    ctx.expect_eq(sol.combine(1, 1), expected);
  });

  ctx.test("k_equals_n", [&] {
    const std::vector<std::vector<int>> expected = {{1, 2, 3}};
    ctx.expect_eq(sol.combine(3, 3), expected);
  });

  ctx.test("k_equals_1", [&] {
    const std::vector<std::vector<int>> expected = {{1}, {2}, {3}, {4}};
    ctx.expect_eq(sol.combine(4, 1), expected);
  });

  ctx.test("n5_k3_ten_combinations", [&] {
    const std::vector<std::vector<int>> expected = {
        {1, 2, 3}, {1, 2, 4}, {1, 2, 5}, {1, 3, 4}, {1, 3, 5},
        {1, 4, 5}, {2, 3, 4}, {2, 3, 5}, {2, 4, 5}, {3, 4, 5},
    };
    ctx.expect_eq(sol.combine(5, 3), expected);
  });

  ctx.test("invalid_k_greater_than_n", [&] {
    const std::vector<std::vector<int>> expected = {};
    ctx.expect_eq(sol.combine(3, 4), expected);
  });
}

}  // namespace
