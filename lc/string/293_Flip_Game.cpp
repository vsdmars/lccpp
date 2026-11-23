// LeetCode 293: Flip Game
// https://leetcode.com/problems/flip-game/
//
// Problem:
//   You are playing Flip Game with a string `current_state` consisting only of
//   `'+'` and `'-'`. In one valid move, a player flips two consecutive `"++"`
//   characters into `"--"`. Return all possible states of the string after one
//   valid move; if no valid move exists, return an empty list.
//
// Examples:
//   Example 1:
//     Input:  current_state = "++++"
//     Output: ["--++", "+--+", "++--"]
//     Explanation:
//       - Verify current_state.size() == 4 >= 2 and scan adjacent pairs (i, i + 1)
//         for i in {0, 1, 2}.
//       - At i = 0, indices 0 and 1 are both '+', so copy "++++" and flip indices 0
//         and 1 to '-' to append "--++".
//       - At i = 1, flip indices 1 and 2 to append "+--+", and at i = 2, flip
//         indices 2 and 3 to append "++--".
//       - Return moves = ["--++", "+--+", "++--"].
//
//   Example 2:
//     Input:  current_state = "++-+++"
//     Output: ["---+++", "++---+", "++-+--"]
//     Explanation:
//       - At i = 0, current_state[0..1] is "++", so flipping indices 0 and 1
//         appends "---+++".
//       - At i = 1 ("+-") and i = 2 ("-+"), the pair is not "++", so both indices
//         are skipped.
//       - At i = 3 and i = 4, current_state[3..4] and current_state[4..5] are "++",
//         appending "++---+" and "++-+--".
//       - Return moves = ["---+++", "++---+", "++-+--"].
//
//   Example 3:
//     Input:  current_state = "+-+-+"
//     Output: []
//     Explanation:
//       - Verify current_state.size() == 5 >= 2 and inspect adjacent pairs (i, i +
//         1) for i in {0, 1, 2, 3}.
//       - Each pair is either "+-" or "-+", so the condition current_state[i] ==
//         '+' && current_state[i + 1] == '+' is never true.
//       - Return the empty vector [].
//
// Algorithm:
//   Perform a single linear pass across adjacent index pairs `(i, i + 1)` for
//   `0 <= i < n - 1`. Whenever `current_state[i] == '+'` and
//   `current_state[i + 1] == '+'`, copy `current_state`, overwrite positions
//   `i` and `i + 1` with `'-'`, and append the resulting string to the output
//   vector. Guard `n < 2` up front so unsigned index arithmetic cannot
//   underflow.
//   Time Complexity:  O(n^2) in the worst case (up to n - 1 output strings,
//                     each of length n).
//   Space Complexity: O(1) auxiliary space (O(n^2) including the returned list).
//
// Data Structures:
//   - `std::string_view`: read-only view over the input board state.
//   - `std::vector<std::string>`: stores all reachable one-move successor states.
//
// Real-World Applications:
//   - Successor-state generation in combinatorial game solvers, minimax/alpha-
//     beta search trees, and Sprague-Grundy nimber computation.
//   - Local neighborhood enumeration (1-step bit/pair flip moves) in simulated
//     annealing, tabu search, and SAT/CSP local search heuristics.
//   - State-space exploration in formal verification and model checking.
//
// Related LeetCode Problems:
//   - #294 Flip Game II [new] (determines whether the starting player has a forced win via memoized DFS / Grundy numbers)
//   - #292 Nim Game [new] (classic impartial two-player game theory)
//   - #464 Can I Win [new] (bitmask minimax memoization over finite game states)
//   - #486 Predict the Winner [done] (two-player optimal play DP on arrays)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Generates every board state reachable by flipping one consecutive "++"
  // pair in `current_state` into "--".
  // Time: O(n^2) worst case, Space: O(1) auxiliary (O(n^2) output).
  std::vector<std::string> generatePossibleNextMoves(
      std::string_view current_state) {
    std::vector<std::string> moves;
    if (current_state.size() < 2uz) {
      return moves;
    }

    // Scan each adjacent pair (i, i + 1); mutate a single fresh copy per hit
    // rather than concatenating three separate substrings.
    for (std::size_t i = 0uz; i + 1uz < current_state.size(); ++i) {
      if (current_state[i] == '+' && current_state[i + 1uz] == '+') {
        std::string next_state(current_state);
        next_state[i] = '-';
        next_state[i + 1uz] = '-';
        moves.push_back(std::move(next_state));
      }
    }

    return moves;
  }
};

LC_PROBLEM(293, "Flip Game", "string", ctx) {
  Solution sol;

  ctx.test("example_four_pluses", [&] {
    const std::vector<std::string> expected{"--++", "+--+", "++--"};
    ctx.expect_eq(sol.generatePossibleNextMoves("++++"), expected);
  });

  ctx.test("no_valid_moves", [&] {
    const std::vector<std::string> empty{};
    ctx.expect_eq(sol.generatePossibleNextMoves(""), empty);
    ctx.expect_eq(sol.generatePossibleNextMoves("+"), empty);
    ctx.expect_eq(sol.generatePossibleNextMoves("-"), empty);
    ctx.expect_eq(sol.generatePossibleNextMoves("+-+-+"), empty);
    ctx.expect_eq(sol.generatePossibleNextMoves("----"), empty);
  });

  ctx.test("disjoint_plus_runs", [&] {
    const std::vector<std::string> expected{"---+++", "++---+", "++-+--"};
    ctx.expect_eq(sol.generatePossibleNextMoves("++-+++"), expected);
  });

  ctx.test("exact_pair", [&] {
    const std::vector<std::string> expected{"--"};
    ctx.expect_eq(sol.generatePossibleNextMoves("++"), expected);
  });
}

}  // namespace
