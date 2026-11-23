// LeetCode 526: Beautiful Arrangement
// https://leetcode.com/problems/beautiful-arrangement/
//
// Problem:
//   Given a positive integer `n` (1 <= n <= 15), count the number of
//   permutations `perm` of the integers `1..n` (1-indexed) such that for every
//   position `i` (1 <= i <= n), either `perm[i]` is divisible by `i` or `i` is
//   divisible by `perm[i]`.
//
// Examples:
//   Example 1:
//     Input:  n = 2
//     Output: 2
//     Explanation:
//       - Allocate `memo` of size `2^2 = 4` initialized to `-1` and invoke `dfs(2,
//         0b00)` to fill positions from `2` down to `1`.
//       - At `pos = 2`, both `num = 1` (`2 % 1 == 0`) and `num = 2` (`2 % 2 == 0`)
//         satisfy the divisibility check.
//       - Branching on `num = 1` calls `dfs(1, 0b01)`, which hits the base case
//         `pos <= 1` and returns `1` for `[2, 1]`; branching on `num = 2` calls
//         `dfs(1, 0b10)` and returns `1` for `[1, 2]`.
//       - Sum both branches to cache `memo[0b00] = 2` and return `2`.
//
//   Example 2:
//     Input:  n = 3
//     Output: 3
//     Explanation:
//       - Start `dfs(3, 0b000)` at `pos = 3`, where `num = 1` (`3 % 1 == 0`) and
//         `num = 3` (`3 % 3 == 0`) pass while `num = 2` fails divisibility.
//       - Branch `num = 1` calls `dfs(2, 0b001)`, where only `num = 2` (`2 % 2 ==
//         0`) is valid at `pos = 2`, reaching `dfs(1, 0b011) = 1` and caching
//         `memo[0b001] = 1`.
//       - Branch `num = 3` calls `dfs(2, 0b100)`, where both `num = 1` (`2 % 1 ==
//         0`) and `num = 2` (`2 % 2 == 0`) reach `pos = 1`, caching `memo[0b100] =
//         2`.
//       - Add the branch totals `1 + 2 = 3`, store `memo[0b000] = 3`, and return
//         `3`.
//
//   Example 3:
//     Input:  n = 1
//     Output: 1
//     Explanation:
//       - Allocate `memo` of size `2^1 = 2` initialized to `-1` and invoke `dfs(1,
//         0u)`.
//       - Since `pos <= 1` holds immediately on entry, `dfs` returns `1` without
//         looping, counting the single valid arrangement `[1]`.
//
// Algorithm:
//   Top-down bitmask dynamic programming (memoized backtracking) filling
//   positions backwards from `pos = n` down to `pos = 1`.
//   - Filling from the highest position `n` down to `1` prunes aggressively
//     because larger indices have far fewer valid divisors/multiples in `1..n`.
//   - When `pos == 1`, any single remaining unused integer `x` satisfies
//     `x % 1 == 0`, so we immediately return `1`.
//   - The state is uniquely identified by `used_mask`, a bitmask of numbers in
//     `1..n` already placed in positions `pos + 1 .. n` (since `pos` equals
//     `n - popcount(used_mask)`). Memoizing on `used_mask` eliminates repeated
//     subproblems when different placement orders consume the same subset.
//
//   Time Complexity:  O(n * 2^n) worst case (at most 2^n bitmask states, each
//                     iterating over at most `n` candidate numbers, and much
//                     smaller in practice due to divisibility pruning).
//   Space Complexity: O(2^n) for the memoization table plus O(n) recursion
//                     depth.
//
// Data Structures:
//   - Bitmask (`std::uint32_t`): Compact O(1)-copy representation of which
//     numbers in `1..n` have already been placed.
//   - std::vector<int> (`memo`): Flat memoization table of size `2^n`
//     initialized to `-1` (uncomputed), indexed directly by `used_mask`.
//
// Real-World Applications:
//   - Counting valid slot assignments in constraint satisfaction problems
//     (CSP), such as shift scheduling, resource-to-bucket allocation, or
//     hardware register assignment with compatibility rules.
//   - Computing permanent-like counts of perfect matchings in small bipartite
//     compatibility graphs via subset DP.
//   - Combinatorial verification and test-case enumeration in formal methods.
//
// Related LeetCode Problems:
//   - #46 Permutations [done] (unconstrained permutation generation)
//   - #667 Beautiful Arrangement II [done] (constructive permutation with k distinct adjacent differences)
//   - #1947 Maximum Compatibility Score Sum [new] (bitmask DP over small permutations)
//   - #1879 Minimum XOR Sum of Two Arrays [new] (assignment problem solved via bitmask DP)
//   - #2741 Special Permutations [new] (counting permutations with adjacent divisibility via bitmask DP)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Counts all beautiful arrangements of integers `1..n`.
  //
  // Places numbers from position `n` down to `1` using bitmask memoization.
  // Time: O(n * 2^n), Space: O(2^n).
  int countArrangement(int n) {
    if (n <= 0) {
      return 0;
    }

    const std::size_t state_count = 1uz << static_cast<std::size_t>(n);
    std::vector<int> memo(state_count, -1);

    // Returns the number of valid ways to fill positions `1..pos` given that
    // the numbers marked in `used_mask` are already placed in `pos+1..n`.
    auto dfs = [&](this auto&& self, int pos, std::uint32_t used_mask) -> int {
      // Base case: at position 1, the single remaining number always divides
      // cleanly by 1, yielding exactly one valid completion.
      if (pos <= 1) {
        return 1;
      }

      if (memo[used_mask] != -1) {
        return memo[used_mask];
      }

      int total = 0;
      for (int num = 1; num <= n; ++num) {
        const std::uint32_t bit = 1u << static_cast<std::uint32_t>(num - 1);
        if ((used_mask & bit) != 0u) {
          continue;
        }
        // Check the divisibility constraint for placing `num` at `pos`.
        if (num % pos == 0 || pos % num == 0) {
          total += self(pos - 1, used_mask | bit);
        }
      }

      memo[used_mask] = total;
      return total;
    };

    return dfs(n, 0u);
  }
};

LC_PROBLEM(526, "Beautiful Arrangement", "backtracking", ctx) {
  ctx.test("base_case_n_1", [&] {
    Solution sol;
    ctx.expect_eq(sol.countArrangement(1), 1);
  });

  ctx.test("example_n_2", [&] {
    Solution sol;
    ctx.expect_eq(sol.countArrangement(2), 2);
  });

  ctx.test("small_values_3_to_7", [&] {
    Solution sol;
    ctx.expect_eq(sol.countArrangement(3), 3);
    ctx.expect_eq(sol.countArrangement(4), 8);
    ctx.expect_eq(sol.countArrangement(5), 10);
    ctx.expect_eq(sol.countArrangement(6), 36);
    ctx.expect_eq(sol.countArrangement(7), 41);
  });

  ctx.test("medium_and_max_constraint", [&] {
    Solution sol;
    ctx.expect_eq(sol.countArrangement(10), 700);
    ctx.expect_eq(sol.countArrangement(15), 24679);
  });
}

}  // namespace
