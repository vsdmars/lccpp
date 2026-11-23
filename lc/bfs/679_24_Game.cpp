// LeetCode 679: 24 Game
// https://leetcode.com/problems/24-game/
//
// Problem:
//   Given an integer array `cards` of 4 numbers, each in [1, 9], determine
//   whether they can be arranged into a valid arithmetic expression using the
//   binary operators +, -, *, / (real floating-point division) and arbitrary
//   parentheses to evaluate to 24. Unary minus and digit concatenation are
//   not permitted.
//
// Examples:
//   Example 1:
//     Input:  cards = [4, 1, 8, 7]
//     Output: true
//     Explanation:
//       - Convert cards to doubles and start BFS level 0 with a single 4-element
//         state {4.0, 1.0, 8.0, 7.0}.
//       - In level 1 (size 3), expand_state pairs 4.0 and 8.0 via 8.0 - 4.0 = 4.0,
//         producing {1.0, 7.0, 4.0} among the 36 successor states.
//       - In level 2 (size 2), pairing 1.0 and 7.0 via 7.0 - 1.0 = 6.0 produces
//         state {4.0, 6.0}.
//       - In level 3 (size 1), combine_pair evaluates 4.0 * 6.0 = 24.0, yielding
//         {24.0}, which satisfies |24.0 - 24.0| < 1e-6 and returns true.
//
//   Example 2:
//     Input:  cards = [3, 3, 8, 8]
//     Output: true
//     Explanation:
//       - Start BFS level 0 with {3.0, 3.0, 8.0, 8.0}.
//       - In level 1 (size 3), pairing 3.0 and 8.0 with real division 8.0 / 3.0
//         creates state {3.0, 8.0, 2.666666...}.
//       - In level 2 (size 2), combining 3.0 and 8.0 / 3.0 via 3.0 - (8.0 / 3.0)
//         produces {8.0, 0.333333...} (1/3).
//       - In level 3 (size 1), since |1/3| > 1e-6, combine_pair computes 8.0 /
//         (1/3) = 24.0, which lies within 1e-6 of 24.0 and returns true.
//
//   Example 3:
//     Input:  cards = [1, 2, 1, 2]
//     Output: false
//     Explanation:
//       - Start BFS level 0 with {1.0, 2.0, 1.0, 2.0} and expand all unordered
//         pairs across three BFS reductions (4 -> 3 -> 2 -> 1).
//       - Guard division at each step so neither a nor b is used as a divisor when
//         its absolute value is <= 1e-6 (e.g., after 1.0 - 1.0 or 2.0 - 2.0).
//       - Check every final 1-element state in level 3 (maximum achievable value is
//         9.0 from (1 + 2) * (1 + 2)); none satisfy |val - 24.0| < 1e-6, so return
//         false.
//
// Algorithm:
//   Perform level-by-level Breadth-First Search (BFS) over multisets of
//   remaining numbers, reducing the multiset cardinality by 1 at each level
//   (4 -> 3 -> 2 -> 1):
//     1. Start with a single state containing the 4 cards converted to double.
//     2. At each level, for every state and every unordered pair of indices
//        (i < j), remove `state[i]` and `state[j]` and combine them with all
//        valid binary operations:
//          a + b, a * b, a - b, b - a,
//          a / b (when |b| > eps), and b / a (when |a| > eps).
//        Note that addition and multiplication are commutative, so only one
//        order is needed for them, whereas subtraction and division require
//        both operand orders.
//     3. Each resulting value appended to the remaining elements forms a state
//        in the next BFS level.
//     4. When states shrink to a single number, check whether any value lies
//        within `kEpsilon = 1e-6` of `24.0` to account for IEEE-754 rounding
//        (e.g., 8 / (3 - 8 / 3) = 24).
//   Time Complexity:  O(1) for the fixed 4-card input (at most
//                     C(4,2)*6 * C(3,2)*6 * C(2,2)*6 = 3,888 leaf evaluations;
//                     in general O(6^(n-1) * (n!)^2 / 2^(n-1))).
//   Space Complexity: O(1) for fixed 4 cards (at most 648 intermediate states
//                     stored at the widest BFS level of size 2).
//
// Data Structures:
//   - std::vector<std::vector<double>>: Current and next BFS frontier levels,
//     where each state is a small vector of remaining floating-point operands.
//   - std::vector<double>: Holds the up-to-6 candidate results of combining a
//     pair of operands.
//
// Real-World Applications:
//   - Symbolic expression synthesis and superoptimization (searching small
//     arithmetic/bitwise DAGs that compute a target constant or function).
//   - Automated theorem proving and constraint solvers over arithmetic
//     expression trees.
//   - Game-solving engines for arithmetic puzzle games (Countdown numbers
//     round, 24-point trainers).
//
// Related LeetCode Problems:
//   - #282 Expression Add Operators [done] (backtracking over operators on a fixed digit sequence)
//   - #241 Different Ways to Add Parentheses [done] (evaluating all parenthesizations of an expression)
//   - #224 Basic Calculator [done] (evaluating parenthesized arithmetic expressions)
//   - #494 Target Sum [done] (choosing +/- signs to hit a target sum)
//   - #773 Sliding Puzzle [done] (state-space BFS over small combinatorial configurations)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

constexpr double kTarget = 24.0;
constexpr double kEpsilon = 1e-6;

class Solution {
 public:
  // Returns true if the cards can be combined via binary +, -, *, / and
  // parentheses to produce 24 within floating-point tolerance.
  // Time: O(1) for 4 cards, Space: O(1) for 4 cards.
  bool judgePoint24(const std::vector<int>& cards) {
    if (cards.empty()) {
      return false;
    }

    // Convert integer cards to double for exact real-division semantics.
    std::vector<double> initial;
    initial.reserve(cards.size());
    for (int card : cards) {
      initial.push_back(static_cast<double>(card));
    }

    std::vector<std::vector<double>> level;
    level.push_back(std::move(initial));

    // Each BFS layer reduces the number of remaining operands by 1 until
    // every state in `level` holds a single final expression value.
    while (!level.empty() && level.front().size() > 1uz) {
      std::vector<std::vector<double>> next_level;
      for (const auto& state : level) {
        expand_state(state, next_level);
      }
      level = std::move(next_level);
    }

    return std::ranges::any_of(level, [](const std::vector<double>& state) {
      return !state.empty() && std::fabs(state[0] - kTarget) < kEpsilon;
    });
  }

 private:
  // Computes all distinct valid binary operation results for operands (a, b).
  static std::vector<double> combine_pair(double a, double b) {
    std::vector<double> results;
    results.reserve(6uz);
    results.push_back(a + b);
    results.push_back(a * b);
    results.push_back(a - b);
    results.push_back(b - a);
    // Guard against division by zero (including near-zero floating-point
    // cancellation like 1.0 - 1.0 or 3.0 - 9.0/3.0).
    if (std::fabs(b) > kEpsilon) {
      results.push_back(a / b);
    }
    if (std::fabs(a) > kEpsilon) {
      results.push_back(b / a);
    }
    return results;
  }

  // Expands one state of size k into all successor states of size k - 1 by
  // pairing every unordered index pair (i < j).
  static void expand_state(const std::vector<double>& state,
                           std::vector<std::vector<double>>& next_level) {
    const std::size_t n = state.size();
    for (std::size_t j = 1uz; j < n; ++j) {
      for (std::size_t i = 0uz; i < j; ++i) {
        // Build the reduced multiset excluding indices i and j.
        std::vector<double> base;
        base.reserve(n - 1uz);
        for (std::size_t k = 0uz; k < n; ++k) {
          if (k != i && k != j) {
            base.push_back(state[k]);
          }
        }

        for (double val : combine_pair(state[i], state[j])) {
          std::vector<double> next_state = base;
          next_state.push_back(val);
          next_level.push_back(std::move(next_state));
        }
      }
    }
  }
};

LC_PROBLEM(679, "24 Game", "bfs", ctx) {
  ctx.test("example_1_solvable", [&] {
    Solution sol;
    // (8 - 4) * (7 - 1) = 24
    ctx.expect_true(sol.judgePoint24({4, 1, 8, 7}));
  });

  ctx.test("example_2_unsolvable", [&] {
    Solution sol;
    ctx.expect_false(sol.judgePoint24({1, 2, 1, 2}));
  });

  ctx.test("fractional_division_3_3_8_8", [&] {
    Solution sol;
    // 8 / (3 - 8 / 3) = 24 (requires real division and epsilon comparison)
    ctx.expect_true(sol.judgePoint24({3, 3, 8, 8}));
  });

  ctx.test("fractional_division_1_3_4_6", [&] {
    Solution sol;
    // 6 / (1 - 3 / 4) = 24
    ctx.expect_true(sol.judgePoint24({1, 3, 4, 6}));
  });

  ctx.test("all_ones_unsolvable_and_division_by_zero_safe", [&] {
    Solution sol;
    ctx.expect_false(sol.judgePoint24({1, 1, 1, 1}));
  });

  ctx.test("simple_product_1_1_1_8", [&] {
    Solution sol;
    // (1 + 1 + 1) * 8 = 24
    ctx.expect_true(sol.judgePoint24({1, 1, 1, 8}));
  });

  ctx.test("all_nines_unsolvable", [&] {
    Solution sol;
    ctx.expect_false(sol.judgePoint24({9, 9, 9, 9}));
  });
}

}  // namespace
