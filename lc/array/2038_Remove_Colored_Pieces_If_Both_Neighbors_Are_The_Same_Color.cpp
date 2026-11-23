// LeetCode 2038: Remove Colored Pieces If Both Neighbors Are The Same Color
// https://leetcode.com/problems/remove-colored-pieces-if-both-neighbors-are-the-same-color/
//
// Problem:
//   There are `n` colored pieces arranged in a line, given as a string
//   `colors` of characters `'A'` and `'B'`. Alice and Bob take turns removing
//   pieces, with Alice moving first:
//   - Alice may remove an `'A'` piece only if both its immediate left and
//     right neighbors are also `'A'`.
//   - Bob may remove a `'B'` piece only if both its immediate left and right
//     neighbors are also `'B'`.
//   - Neither player can remove pieces on the boundary or of the opponent's
//     color. The first player who cannot make a move loses.
//   Return `true` if Alice wins and `false` if Bob wins under optimal play.
//   Constraints: 1 <= colors.size() <= 10^5; colors[i] is `'A'` or `'B'`.
//
// Examples:
//   Example 1:
//     Input:  colors = "AAABABB"
//     Output: true
//     Explanation:
//       - Verify colors.size() >= 3 (7 >= 3) and initialize alice_moves = 0 and
//         bob_moves = 0.
//       - Scan interior indices i = 1..5: only i = 1 satisfies colors[0] ==
//         colors[1] == colors[2] == 'A', incrementing alice_moves to 1 while
//         bob_moves stays 0.
//       - Compare alice_moves > bob_moves (1 > 0) and return true.
//
//   Example 2:
//     Input:  colors = "ABBBBBBBAAA"
//     Output: false
//     Explanation:
//       - Verify colors.size() >= 3 (11 >= 3) and initialize alice_moves = 0 and
//         bob_moves = 0.
//       - At interior indices i = 2..6 inside the 7-character 'B' run, colors[i -
//         1] == colors[i] == colors[i + 1] == 'B' holds five times, raising
//         bob_moves to 5.
//       - At interior index i = 9 inside the trailing "AAA" run, colors[8] ==
//         colors[9] == colors[10] == 'A' increments alice_moves to 1.
//       - Evaluate alice_moves > bob_moves (1 > 5) and return false.
//
//   Example 3:
//     Input:  colors = "AAABBB"
//     Output: false
//     Explanation:
//       - Verify colors.size() >= 3 (6 >= 3) and initialize alice_moves = 0 and
//         bob_moves = 0.
//       - At interior index i = 1, colors[0..2] == "AAA" increments alice_moves to
//         1, while indices i = 2 ("AAB") and i = 3 ("ABB") do not qualify.
//       - At interior index i = 4, colors[3..5] == "BBB" increments bob_moves to 1.
//       - Evaluate alice_moves > bob_moves (1 > 1) and return false because Alice
//         runs out of moves first.
//
// Algorithm:
//   Game-Theoretic Move Counting (Independent Runs):
//   A crucial structural invariant makes simulation unnecessary: removing an
//   interior `'A'` from a contiguous run of `k >= 3` `'A'`s reduces its length
//   to `k - 1 >= 2`. Because at least two `'A'`s always remain at the ends of
//   the run, Alice's moves can never bring two `'B'` runs together, and
//   symmetrically Bob's moves can never affect Alice's available moves.
//   Therefore, the two players' total move budgets are completely independent:
//   each contiguous run of length `k >= 3` contributes `k - 2` moves, which
//   equals the count of interior positions `i` (`1 <= i <= n - 2`) where
//   `colors[i - 1] == colors[i] == colors[i + 1]`.
//   Because Alice moves first, she wins if and only if
//   `alice_moves > bob_moves`.
//   Time Complexity:  O(n)
//   Space Complexity: O(1)
//
// Data Structures:
//   - Non-owning `std::string_view` over the input sequence and two integer
//     counters (`alice_moves`, `bob_moves`).
//
// Real-World Applications:
//   - Combinatorial game theory analysis for decoupled impartial/partizan
//     resource-depletion games (where sub-games do not interact).
//   - Run-length and interior-point filtering in 1D signal denoising,
//     morphological erosion, and contiguous-block compaction algorithms.
//
// Related LeetCode Problems:
//   - #292 Nim Game [new] (closed-form mathematical invariant replacing game simulation)
//   - #877 Stone Game [new] (two-player optimal-play turn-based game on a line)
//   - #1561 Maximum Number of Coins You Can Get [new] (greedy turn-based array game)
//   - #1446 Consecutive Characters [new] (single-pass contiguous run analysis on a string)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string_view>

namespace {

class Solution {
 public:
  // Determines whether Alice wins by counting each player's independent pool
  // of removable interior triples ("AAA" for Alice, "BBB" for Bob).
  //
  // Invariant: Every index `i` with `colors[i - 1] == colors[i] == colors[i + 1]`
  // corresponds bijectively to one legal move for that color, and removing any
  // interior piece leaves the endpoints intact so the opponent's moves never
  // change.
  //
  // Time: O(n), Space: O(1).
  bool winnerOfGame(std::string_view colors) {
    // Fewer than 3 pieces means neither player can form a removable triple,
    // so Alice has no opening move and immediately loses.
    if (colors.size() < 3uz) {
      return false;
    }

    int alice_moves = 0;
    int bob_moves = 0;

    // Scan every interior position `i` in [1, n - 2].
    for (std::size_t i = 1uz; i + 1uz < colors.size(); ++i) {
      if (colors[i - 1uz] == colors[i] && colors[i] == colors[i + 1uz]) {
        if (colors[i] == 'A') {
          ++alice_moves;
        } else {
          ++bob_moves;
        }
      }
    }

    // Alice goes first, so she needs strictly more moves than Bob to win.
    return alice_moves > bob_moves;
  }
};

LC_PROBLEM(2038, "Remove Colored Pieces If Both Neighbors Are The Same Color",
           "array", ctx) {
  ctx.test("example_1_alice_has_one_move_bob_zero", [&] {
    Solution sol;
    ctx.expect_true(sol.winnerOfGame("AAABABB"));
  });

  ctx.test("example_2_no_valid_moves", [&] {
    Solution sol;
    ctx.expect_false(sol.winnerOfGame("AA"));
  });

  ctx.test("example_3_bob_has_more_moves", [&] {
    Solution sol;
    ctx.expect_false(sol.winnerOfGame("ABBBBBBBAAA"));
  });

  ctx.test("tie_goes_to_bob", [&] {
    Solution sol;
    // Alice has 1 move ("AAA"), Bob has 1 move ("BBB"); Alice runs out first.
    ctx.expect_false(sol.winnerOfGame("AAABBB"));
  });

  ctx.test("multiple_runs_alice_wins_by_one", [&] {
    Solution sol;
    // Alice: (4 - 2) + (3 - 2) = 3 moves; Bob: (4 - 2) = 2 moves.
    ctx.expect_true(sol.winnerOfGame("AAAABBBBAAA"));
  });

  ctx.test("single_character_edge_case", [&] {
    Solution sol;
    ctx.expect_false(sol.winnerOfGame("A"));
    ctx.expect_false(sol.winnerOfGame("B"));
  });
}

}  // namespace
