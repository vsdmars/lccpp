// LeetCode 1884: Egg Drop With 2 Eggs And N Floors
// https://leetcode.com/problems/egg-drop-with-2-eggs-and-n-floors/
//
// Problem:
//   You are given 2 identical eggs and a building with `n` floors labeled `1`
//   through `n`. There exists an unknown threshold floor `f` (`0 <= f <= n`)
//   such that any egg dropped from a floor strictly above `f` breaks, whereas
//   any egg dropped at or below `f` survives and can be reused. Each drop
//   counts as one move. Return the minimum number of moves required in the
//   worst case to determine `f` with certainty.
//   Constraints: 1 <= n <= 1000.
//
// Algorithm:
//   Minimax Dynamic Programming (top-down memoized DFS):
//   1. Let `dfs(floors)` be the minimum worst-case number of moves needed with
//      2 eggs to determine the threshold across `floors` contiguous untested
//      floors.
//   2. Base cases: `dfs(0) = 0` (no floors to test) and `dfs(1) = 1` (one drop
//      determines whether `f` is `0` or `1`).
//   3. Transition: suppose we drop the first egg from floor `drop`
//      (`1 <= drop < floors`). Two mutually exclusive outcomes occur:
//      - The egg breaks: only 1 egg remains, so we must linearly test floors
//        `1, 2, ..., drop - 1` from bottom to top, taking at most `drop - 1`
//        additional moves.
//      - The egg survives: both eggs remain intact, leaving an independent
//        identical subproblem of `floors - drop` remaining floors above, taking
//        `dfs(floors - drop)` additional moves.
//      To guarantee certainty in the worst case, we minimize the maximum over
//      both outcomes:
//        dfs(floors) = min_{1 <= drop < floors} (1 + max(drop - 1, dfs(floors - drop))).
//   4. Mathematical equivalence (also provided in `twoEggDropMath`): with `m`
//      total moves, 2 eggs can cover at most `1 + 2 + ... + m = m * (m + 1) / 2`
//      floors, so the answer is the smallest `m` with `m * (m + 1) / 2 >= n`.
//
//   Complexity:
//   - Top-down DP (`twoEggDrop`):
//     - Time:  O(n^2) — `n` states, each scanning up to `n` candidate drop floors.
//     - Space: O(n) — 1D memoization table and recursion stack.
//   - Triangular-number closed form (`twoEggDropMath`):
//     - Time:  O(sqrt(n)), Space: O(1).
//
// Data Structures:
//   - `std::vector<int> memo`: 1D DP cache indexed by remaining floor count
//     `[0..n]`, initialized to `-1` for uncomputed states.
//
// Real-World Applications:
//   - Hardware destruct-testing and QA stress testing (e.g., finding maximum
//     voltage, pressure, or load tolerance using a limited budget of prototypes).
//   - Network bandwidth / MTU probing and canary-rollout threshold discovery
//     where overshooting a limit incurs a costly failure recovery.
//   - Optimal fault-isolation strategies in systems with asymmetric probe costs.
//
// Related LeetCode Problems:
//   - #887 Super Egg Drop [new] (generalization to k eggs and n floors via DP + binary search)
//   - #375 Guess Number Higher or Lower II [done] (minimax DP minimizing worst-case search cost)
//   - #278 First Bad Version [done] (finding a monotonic threshold with unlimited non-destructive probes)
//   - #1130 Minimum Cost Tree From Leaf Values [new] (optimal split DP minimizing worst/total cost)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the minimum worst-case number of drops to find the critical floor
  // among `n` floors using 2 identical eggs via top-down memoized minimax DP.
  //
  // Invariant: `memo[k]` stores the exact minimum worst-case moves for `k`
  // remaining floors with 2 eggs, or `-1` if not yet computed.
  //
  // Time: O(n^2), Space: O(n).
  int twoEggDrop(int n) {
    if (n <= 0) {
      return 0;
    }

    std::vector<int> memo(static_cast<std::size_t>(n) + 1, -1);
    memo[0] = 0;
    memo[1] = 1;

    auto dfs = [&](this auto&& self, int floors) -> int {
      const auto idx = static_cast<std::size_t>(floors);
      if (memo[idx] != -1) {
        return memo[idx];
      }

      // Dropping at floor `floors` itself would take `1 + (floors - 1) = floors`
      // moves in the worst case, serving as a safe upper bound.
      int best = floors;
      for (int drop = 1; drop < floors; ++drop) {
        // Break branch: 1 egg left -> linear scan of `drop - 1` floors below.
        // Survive branch: 2 eggs left -> independent subproblem of `floors - drop`
        // floors above.
        const int worst_case = 1 + std::max(drop - 1, self(floors - drop));
        best = std::min(best, worst_case);
      }

      memo[idx] = best;
      return best;
    };

    return dfs(n);
  }

  // Secondary O(sqrt(n)) approach using the triangular-number capacity bound:
  // with `moves` drops and 2 eggs, the maximum distinguishable floor count is
  // `moves * (moves + 1) / 2`.
  //
  // Time: O(sqrt(n)), Space: O(1).
  int twoEggDropMath(int n) {
    int moves = 0;
    int max_floors = 0;
    while (max_floors < n) {
      ++moves;
      max_floors += moves;
    }
    return moves;
  }
};

LC_PROBLEM(1884, "Egg Drop With 2 Eggs And N Floors", "dp", ctx) {
  Solution sol;

  ctx.test("single_floor", [&] {
    // Matches Python build(): n = 1 requires exactly 1 drop.
    ctx.expect_eq(sol.twoEggDrop(1), 1);
    ctx.expect_eq(sol.twoEggDropMath(1), 1);
  });

  ctx.test("example_1", [&] {
    ctx.expect_eq(sol.twoEggDrop(2), 2);
    ctx.expect_eq(sol.twoEggDropMath(2), 2);
  });

  ctx.test("example_2", [&] {
    // Classic 100-floor 2-egg puzzle: 14 * 15 / 2 = 105 >= 100.
    ctx.expect_eq(sol.twoEggDrop(100), 14);
    ctx.expect_eq(sol.twoEggDropMath(100), 14);
  });

  ctx.test("triangular_number_boundaries", [&] {
    // At triangular numbers T(k) = k*(k+1)/2 the answer is k; at T(k)+1 it jumps
    // to k+1.
    ctx.expect_eq(sol.twoEggDrop(3), 2);
    ctx.expect_eq(sol.twoEggDrop(4), 3);
    ctx.expect_eq(sol.twoEggDrop(6), 3);
    ctx.expect_eq(sol.twoEggDrop(7), 4);
    ctx.expect_eq(sol.twoEggDrop(91), 13);
    ctx.expect_eq(sol.twoEggDrop(92), 14);
    ctx.expect_eq(sol.twoEggDrop(105), 14);
    ctx.expect_eq(sol.twoEggDrop(106), 15);
  });

  ctx.test("max_constraint_1000", [&] {
    // 44 * 45 / 2 = 990 < 1000 <= 1035 = 45 * 46 / 2.
    ctx.expect_eq(sol.twoEggDrop(1000), 45);
    ctx.expect_eq(sol.twoEggDropMath(1000), 45);
  });

  ctx.test("dp_matches_math_across_range", [&] {
    for (int n = 1; n <= 200; ++n) {
      ctx.expect_eq(sol.twoEggDrop(n), sol.twoEggDropMath(n));
    }
  });
}

}  // namespace
