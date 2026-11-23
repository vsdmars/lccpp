// LeetCode 77: Combinations
// https://leetcode.com/problems/combinations/
//
// Problem:
//   Given two integers `n` and `k`, return all possible combinations of `k`
//   numbers chosen from the inclusive range [1, n]. You may return the answer
//   in any order.
//   Constraints: 1 <= n <= 20, 1 <= k <= n.
//
// Examples:
//   Example 1:
//     Input:  n = 4, k = 2
//     Output: [[1, 2], [1, 3], [1, 4], [2, 3], [2, 4], [3, 4]]
//     Explanation:
//       - Start dfs(start = 1, remaining = 2) with current = [] and max_start = (4
//         - 2) + 1 = 3.
//       - Pick num = 1 (current = [1]) and call dfs(2, 1) with max_start = 4, which
//         hits remaining == 0 to record [1, 2], [1, 3], and [1, 4].
//       - Backtrack and pick num = 2 (current = [2]), calling dfs(3, 1) to record
//         [2, 3] and [2, 4].
//       - Pick num = 3 (current = [3]) and call dfs(4, 1) to record [3, 4], then
//         stop because 4 exceeds max_start = 3.
//
//   Example 2:
//     Input:  n = 3, k = 3
//     Output: [[1, 2, 3]]
//     Explanation:
//       - At dfs(1, 3), max_start = (3 - 3) + 1 = 1, so only num = 1 is pushed into
//         current = [1].
//       - At dfs(2, 2), max_start = (3 - 2) + 1 = 2, so only num = 2 is pushed into
//         current = [1, 2].
//       - At dfs(3, 1), max_start = (3 - 1) + 1 = 3 pushes 3, then dfs(4, 0)
//         reaches remaining == 0 and records [1, 2, 3].
//
//   Example 3:
//     Input:  n = 3, k = 4
//     Output: []
//     Explanation:
//       - The entry guard checks (n < 0 || k < 0 || k > n), which is true since k =
//         4 > n = 3.
//       - combine immediately returns {} without running dfs.
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
