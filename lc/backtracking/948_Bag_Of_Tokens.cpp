// LeetCode 948: Bag Of Tokens
// https://leetcode.com/problems/bag-of-tokens/
//
// Problem:
//   You start with an initial `power`, an initial `score` of 0, and a bag of
//   `tokens` where `tokens[i]` is the power value of the i-th token. Each token
//   may be played at most once in any order (and tokens may be left unplayed):
//     - Face-up:   If `power >= tokens[i]`, spend `tokens[i]` power to gain +1
//                  score.
//     - Face-down: If `score >= 1`, spend 1 score to gain `tokens[i]` power.
//   Return the maximum score achievable after playing any number of tokens.
//
// Examples:
//   Example 1:
//     Input:  tokens = [100, 200, 300, 400], power = 200
//     Output: 2
//     Explanation:
//       - Sort `tokens` to `[100, 200, 300, 400]` and initialize `left = 0`, `right
//         = 4`, `score = 0`, and `max_score = 0`.
//       - Since `power (200) >= tokens[0] (100)`, play `100` face-up to update
//         `power = 100`, `left = 1`, `score = 1`, and `max_score = 1`.
//       - Because `power (100) < tokens[1] (200)` and `score = 1 > 0`, play
//         `tokens[3] = 400` face-down (`right = 3`), raising `power` to `500` and
//         dropping `score` to `0`.
//       - Play `tokens[1] = 200` and `tokens[2] = 300` face-up (`power: 500 -> 300
//         -> 0`, `left = 3`, `score = 2`), updating `max_score = 2` before exiting
//         at `left == right`.
//
//   Example 2:
//     Input:  tokens = [26, 71, 55, 81], power = 54
//     Output: 1
//     Explanation:
//       - Sort `tokens` to `[26, 55, 71, 81]` with `left = 0`, `right = 4`, `score
//         = 0`, and `max_score = 0`.
//       - Play `tokens[0] = 26` face-up (`power = 28`, `left = 1`, `score = 1`,
//         `max_score = 1`), then sell `tokens[3] = 81` face-down (`right = 3`,
//         `power = 109`, `score = 0`).
//       - Play `tokens[1] = 55` face-up (`power = 54`, `left = 2`, `score = 1`,
//         `max_score = 1`), then sell `tokens[2] = 71` face-down (`right = 2`,
//         `power = 125`, `score = 0`), ending with `max_score = 1`.
//
//   Example 3:
//     Input:  tokens = [100], power = 50
//     Output: 0
//     Explanation:
//       - Sort `[100]` and enter the loop with `left = 0`, `right = 1`, `power =
//         50`, and `score = 0`.
//       - Since `power (50) < tokens[0] (100)` and `score == 0`, neither play is
//         legal, so the loop hits `break` and returns `max_score = 0`.
//
// Algorithm:
//   Greedy two-pointer strategy on the sorted token array.
//   - Sort `tokens` in ascending order.
//   - Buying score (face-up) always costs the same reward (+1 score), so we
//     should always buy the cheapest remaining token (`tokens[left]`) to retain
//     as much power as possible.
//   - Selling score for power (face-down) always costs 1 score, so whenever we
//     cannot afford `tokens[left]` and have `score > 0`, we should sell the
//     most expensive remaining token (`tokens[right - 1]`) to maximize gained
//     power.
//   - Because selling a token temporarily reduces `score` (and might not unlock
//     enough power to buy more than one token), we track `max_score` after
//     every face-up play.
//   - Stop as soon as neither a face-up nor a face-down play is legal, or when
//     no tokens remain (`left == right`).
//
//   Time Complexity:  O(n log n) dominated by sorting `tokens`; the two-pointer
//                     sweep is O(n).
//   Space Complexity: O(1) extra space beyond the in-place sort (O(log n) sort
//                     stack).
//
// Data Structures:
//   - Sorted std::vector<int> (`tokens`): Enables O(1) access to both the
//     minimum-cost token (left end) and maximum-refund token (right end).
//   - Two indices (`left`, `right`) forming a half-open active interval
//     `[left, right)` to prevent unsigned underflow when `tokens` is empty.
//
// Real-World Applications:
//   - Arbitrage and capital allocation: buying low-cost yield units and
//     liquidating highest-value positions when liquidity is constrained.
//   - Cloud resource and spot-instance trading: exchanging spare capacity
//     credits for compute units to maximize completed jobs under a budget.
//   - Game economy optimization and energy/resource scheduling in autonomous
//     systems.
//
// Related LeetCode Problems:
//   - #881 Boats to Save People [done] (greedy two pointers on a sorted array pairing extremes)
//   - #455 Assign Cookies [new] (greedy matching after sorting)
//   - #55 Jump Game [done] (greedy reachability tracking)
//   - #134 Gas Station [done] (greedy resource balance along a sequence)
//   - #1675 Minimize Deviation in Array [new] (greedy exchange of extreme values)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the maximum achievable score given `tokens` and initial `power`.
  //
  // Invariant: Unplayed tokens always occupy the sorted half-open range
  // `tokens[left..right-1]`. Every face-up play consumes the cheapest token
  // (`left`), and every face-down play consumes the richest token (`right-1`).
  // Time: O(n log n), Space: O(1) auxiliary.
  int bagOfTokensScore(std::vector<int> tokens, int power) {
    std::ranges::sort(tokens);

    std::size_t left = 0;
    std::size_t right = tokens.size();  // Half-open active interval [left, right)
    int score = 0;
    int max_score = 0;

    while (left < right) {
      if (power >= tokens[left]) {
        // Play the cheapest available token face-up to gain 1 score.
        power -= tokens[left];
        ++left;
        ++score;
        max_score = std::max(max_score, score);
      } else if (score > 0) {
        // Cannot afford the cheapest token: sacrifice 1 score on the highest
        // value token to maximize replenished power.
        --right;
        power += tokens[right];
        --score;
      } else {
        // Neither face-up nor face-down play is possible.
        break;
      }
    }

    return max_score;
  }
};

LC_PROBLEM(948, "Bag Of Tokens", "backtracking", ctx) {
  ctx.test("example_1_insufficient_power", [&] {
    Solution sol;
    ctx.expect_eq(sol.bagOfTokensScore({100}, 50), 0);
  });

  ctx.test("example_2_python_build_case", [&] {
    Solution sol;
    ctx.expect_eq(sol.bagOfTokensScore({100, 200}, 150), 1);
  });

  ctx.test("example_3_buy_sell_buy_buy", [&] {
    Solution sol;
    ctx.expect_eq(sol.bagOfTokensScore({100, 200, 300, 400}, 200), 2);
  });

  ctx.test("unsorted_input_and_multiple_trades", [&] {
    Solution sol;
    ctx.expect_eq(sol.bagOfTokensScore({400, 100, 300, 200}, 200), 2);
    ctx.expect_eq(sol.bagOfTokensScore({26, 71, 55, 81}, 54), 1);
  });

  ctx.test("empty_and_zero_cost_tokens", [&] {
    Solution sol;
    ctx.expect_eq(sol.bagOfTokensScore({}, 100), 0);
    ctx.expect_eq(sol.bagOfTokensScore({0, 0, 0}, 0), 3);
  });

  ctx.test("buy_all_tokens_without_selling", [&] {
    Solution sol;
    ctx.expect_eq(sol.bagOfTokensScore({10, 20, 30}, 100), 3);
  });
}

}  // namespace
