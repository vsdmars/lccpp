// LeetCode 649: Dota2 Senate
// https://leetcode.com/problems/dota2-senate/
//
// Problem:
//   A string `senate` of length `n` represents senators from two rival parties
//   — Radiant ('R') and Dire ('D') — voting in cyclic rounds from index 0 to
//   n - 1. On each active senator's turn, they may either permanently ban one
//   opposing senator's voting right or declare victory if no opposing senators
//   remain. Assuming every senator plays optimally, predict which party will
//   win ("Radiant" or "Dire").
//
// Examples:
//   Example 1:
//     Input:  senate = "RD"
//     Output: "Radiant"
//     Explanation:
//       - Initialize turn queues with n = 2: radiant = [0] and dire = [1].
//       - Pop r_turn = 0 and d_turn = 1; since 0 < 1, Radiant acts first, bans Dire
//         index 1, and re-enqueues 0 + 2 = 2 into radiant.
//       - dire is now empty so the loop terminates and returns "Radiant".
//
//   Example 2:
//     Input:  senate = "RDD"
//     Output: "Dire"
//     Explanation:
//       - Initialize turn queues with n = 3: radiant = [0] and dire = [1, 2].
//       - Round 1 pops r_turn = 0 and d_turn = 1; since 0 < 1, Radiant bans Dire
//         index 1 and pushes 0 + 3 = 3 into radiant, leaving dire = [2].
//       - Round 2 pops r_turn = 3 and d_turn = 2; since 2 < 3, Dire bans Radiant
//         index 3 and pushes 2 + 3 = 5 into dire.
//       - radiant is now empty so the loop ends and returns "Dire".
//
//   Example 3:
//     Input:  senate = "DDRRR"
//     Output: "Dire"
//     Explanation:
//       - Initialize turn queues with n = 5: radiant = [2, 3, 4] and dire = [0, 1].
//       - First two matchups compare (r=2, d=0) and (r=3, d=1); Dire acts earlier
//         in both, banning 2 and 3 while pushing 5 and 6 to dire = [5, 6].
//       - Third matchup pops r_turn = 4 and d_turn = 5; Radiant acts earlier, bans
//         5, and pushes 4 + 5 = 9 into radiant = [9].
//       - Fourth matchup pops r_turn = 9 and d_turn = 6; Dire acts earlier, bans 9,
//         and pushes 6 + 5 = 11, leaving radiant empty so "Dire" wins.
//
// Algorithm:
//   Greedy strategy: whenever a senator acts, it is strictly optimal to ban
//   the *earliest* active opposing senator who will act next (delaying or
//   preventing that opponent from banning an ally). Model this with two FIFO
//   queues storing the turn indices of active Radiant and Dire senators:
//     1. Enqueue every 'R' index into `radiant` and every 'D' index into
//        `dire`.
//     2. While both queues are non-empty, pop the front index `r` and `d` from
//        each queue. The smaller index acts earlier, bans the larger index
//        permanently, and re-enters its own queue at `index + n` to act in the
//        next round.
//     3. Each iteration eliminates exactly one senator, so the loop runs at
//        most n - 1 times until one party's queue is empty.
//   Time Complexity:  O(N), where N = senate.size() (each senator is popped at
//                     most twice across all rounds).
//   Space Complexity: O(N) for the two index queues.
//
// Data Structures:
//   - Two std::queue<std::size_t> queues (`radiant`, `dire`): Maintain active
//     senators per party in strictly increasing cyclic turn order with O(1)
//     push/pop.
//
// Real-World Applications:
//   - Round-robin process/packet scheduling with preemption or cancellation
//     between competing priority classes.
//   - Turn-based multi-agent game AI and resource-elimination simulations
//     (Josephus-style cyclic elimination protocols).
//   - Quorum and consensus voting simulations in distributed fault-tolerance
//     modeling.
//
// Related LeetCode Problems:
//   - #1823 Find the Winner of the Circular Game [new] (cyclic elimination)
//   - #622 Design Circular Queue [done] (cyclic FIFO turn buffer)
//   - #950 Reveal Cards In Increasing Order [new] (queue simulation of rounds)
//   - #1700 Number of Students Unable to Eat Lunch [new] (cyclic queue voting)

#include "framework/lc_framework.h"

#include <cstddef>
#include <queue>
#include <string>
#include <string_view>

namespace {

class Solution {
 public:
  // Simulates optimal greedy round-based bans using two turn-order queues and
  // returns the winning party ("Radiant" or "Dire").
  // Time: O(N). Space: O(N).
  std::string predictPartyVictory(std::string_view senate) {
    const std::size_t n = senate.size();
    std::queue<std::size_t> radiant;
    std::queue<std::size_t> dire;

    // Seed each party's FIFO queue with its senators' initial turn positions.
    for (std::size_t i = 0uz; i < n; ++i) {
      if (senate[i] == 'R') {
        radiant.push(i);
      } else {
        dire.push(i);
      }
    }

    // In each matchup, the senator with the smaller turn index acts first,
    // eliminates the opponent's earliest upcoming senator, and advances to the
    // next voting round at `index + n`.
    while (!radiant.empty() && !dire.empty()) {
      const std::size_t r_turn = radiant.front();
      radiant.pop();
      const std::size_t d_turn = dire.front();
      dire.pop();

      if (r_turn < d_turn) {
        radiant.push(r_turn + n);
      } else {
        dire.push(d_turn + n);
      }
    }

    return radiant.empty() ? "Dire" : "Radiant";
  }
};

LC_PROBLEM(649, "Dota2 Senate", "design", ctx) {
  ctx.test("leetcode_example_1", [&] {
    Solution sol;
    ctx.expect_eq(sol.predictPartyVictory("RD"), std::string("Radiant"));
  });

  ctx.test("leetcode_example_2", [&] {
    Solution sol;
    ctx.expect_eq(sol.predictPartyVictory("RDD"), std::string("Dire"));
  });

  ctx.test("single_senator", [&] {
    Solution sol;
    ctx.expect_eq(sol.predictPartyVictory("R"), std::string("Radiant"));
    ctx.expect_eq(sol.predictPartyVictory("D"), std::string("Dire"));
  });

  ctx.test("multi_round_wraparound_bans", [&] {
    Solution sol;
    ctx.expect_eq(sol.predictPartyVictory("DDRRR"), std::string("Dire"));
    ctx.expect_eq(sol.predictPartyVictory("DDRRRR"), std::string("Radiant"));
    ctx.expect_eq(sol.predictPartyVictory("RRDDD"), std::string("Radiant"));
    ctx.expect_eq(sol.predictPartyVictory("DRRD"), std::string("Dire"));
  });
}

}  // namespace
