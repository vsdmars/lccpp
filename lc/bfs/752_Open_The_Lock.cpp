// LeetCode 752: Open The Lock
// https://leetcode.com/problems/open-the-lock/
//
// Problem:
//   A combination lock has 4 circular digit wheels ('0'-'9') initially set to
//   "0000". Each move turns a single wheel one slot forward or backward,
//   wrapping around between '9' and '0'. Given a list of forbidden `deadends`
//   (states that permanently freeze the lock) and a `target` 4-digit code,
//   return the minimum number of turns required to reach `target`, or -1 if it
//   is impossible.
//
// Algorithm:
//   Model the 10,000 possible 4-digit combinations as vertices 0..9999 in an
//   unweighted undirected graph where each vertex has 8 neighbors (turning
//   each of the 4 wheels +1 or -1 modulo 10). Run Breadth-First Search (BFS):
//     1. Mark all `deadends` in a fixed-size boolean array `blocked_or_visited`
//        of length 10,000.
//     2. If "0000" or `target` is a deadend, return -1; if `target == "0000"`,
//        return 0.
//     3. Push state 0 ("0000") into a FIFO queue and expand level by level.
//     4. For each popped code, modify each of its 4 base-10 digits up (+1 mod
//        10) and down (+9 mod 10) using O(1) integer arithmetic (avoiding
//        string allocations). The first time `target` is generated, return the
//        current turn count.
//   Time Complexity:  O(D + W * 10^W) where D = deadends.size() and W = 4
//                     wheels (at most 10,000 states * 8 neighbors = 80,000
//                     transitions).
//   Space Complexity: O(10^W) = O(10,000) for the fixed visited array and BFS
//                     queue.
//
// Data Structures:
//   - std::array<bool, 10000>: Direct-addressed visited & deadend lookup table
//     with O(1) access and zero heap allocations.
//   - std::queue<int>: FIFO queue storing encoded 4-digit lock states.
//   - std::array<int, 4>: Place values {1000, 100, 10, 1} for digit extraction.
//
// Real-World Applications:
//   - Shortest-path state-space exploration in hardware/protocol verification
//     with forbidden fault states.
//   - Safe configuration transition planning in distributed systems (finding
//     the fewest single-parameter changes between configs while avoiding
//     unsafe intermediate combinations).
//   - Robotic joint-space discrete motion planning with collision avoidance.
//
// Related LeetCode Problems:
//   - #127 Word Ladder [done] (shortest single-character transformation BFS)
//   - #433 Minimum Genetic Mutation [new] (BFS over 8-char DNA strings with valid bank)
//   - #773 Sliding Puzzle [done] (minimum-move state-space BFS on a 2x3 board)
//   - #815 Bus Routes [done] (minimum-transfer BFS on transit networks)
//   - #909 Snakes and Ladders [new] (minimum-move BFS on a numbered board)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <queue>
#include <string>
#include <string_view>
#include <vector>

namespace {

constexpr std::size_t kStateSpace = 10000uz;
constexpr std::array<int, 4> kPlaceValues = {1000, 100, 10, 1};

class Solution {
 public:
  // Returns the minimum number of single-wheel turns from "0000" to `target`
  // without entering any code in `deadends`, or -1 if unreachable.
  // Time: O(D + 10^4), Space: O(10^4).
  int openLock(const std::vector<std::string>& deadends,
               std::string_view target) {
    // Treating deadends as pre-visited states prevents the BFS from ever
    // entering or expanding them.
    std::array<bool, kStateSpace> visited{};
    for (const std::string& code : deadends) {
      visited[static_cast<std::size_t>(encode(code))] = true;
    }

    const int target_code = encode(target);
    if (visited[0] || visited[static_cast<std::size_t>(target_code)]) {
      return -1;
    }
    if (target_code == 0) {
      return 0;
    }

    std::queue<int> frontier;
    visited[0] = true;
    frontier.push(0);

    int turns = 0;
    while (!frontier.empty()) {
      ++turns;
      const std::size_t level_size = frontier.size();
      for (std::size_t i = 0uz; i < level_size; ++i) {
        const int current = frontier.front();
        frontier.pop();

        if (expand_neighbors(current, target_code, visited, frontier)) {
          return turns;
        }
      }
    }

    return -1;
  }

 private:
  // Encodes a 4-digit decimal string (e.g. "0202") into an integer in [0, 9999].
  static int encode(std::string_view code) {
    int value = 0;
    for (char ch : code) {
      value = (value * 10) + (ch - '0');
    }
    return value;
  }

  // Generates all 8 one-turn neighbors of `current`, enqueuing unvisited ones.
  // Returns true immediately if `target_code` is reached.
  static bool expand_neighbors(int current, int target_code,
                               std::array<bool, kStateSpace>& visited,
                               std::queue<int>& frontier) {
    for (int place : kPlaceValues) {
      const int digit = (current / place) % 10;
      const int without_digit = current - (digit * place);

      // +1 wraps 9 -> 0; +9 (equivalent to -1 mod 10) wraps 0 -> 9.
      const std::array<int, 2> candidates = {
          without_digit + (((digit + 1) % 10) * place),
          without_digit + (((digit + 9) % 10) * place),
      };

      for (int next : candidates) {
        if (next == target_code) {
          return true;
        }
        const auto idx = static_cast<std::size_t>(next);
        if (!visited[idx]) {
          visited[idx] = true;
          frontier.push(next);
        }
      }
    }
    return false;
  }
};

LC_PROBLEM(752, "Open The Lock", "bfs", ctx) {
  ctx.test("example_1_six_turns", [&] {
    Solution sol;
    ctx.expect_eq(
        sol.openLock({"0201", "0101", "0102", "1212", "2002"}, "0202"), 6);
  });

  ctx.test("example_2_wrap_around_reverse", [&] {
    Solution sol;
    ctx.expect_eq(sol.openLock({"8888"}, "0009"), 1);
  });

  ctx.test("example_3_target_completely_surrounded", [&] {
    Solution sol;
    ctx.expect_eq(
        sol.openLock(
            {"8887", "8889", "8878", "8898", "8788", "8988", "7888", "9888"},
            "8888"),
        -1);
  });

  ctx.test("example_4_start_is_deadend", [&] {
    Solution sol;
    ctx.expect_eq(sol.openLock({"0000"}, "8888"), -1);
  });

  ctx.test("target_is_start", [&] {
    Solution sol;
    ctx.expect_eq(sol.openLock({"0001", "0009"}, "0000"), 0);
    ctx.expect_eq(sol.openLock({"0000"}, "0000"), -1);
  });

  ctx.test("max_distance_without_deadends", [&] {
    Solution sol;
    // Every wheel needs 5 turns (0 -> 5), for 20 total turns.
    ctx.expect_eq(sol.openLock({}, "5555"), 20);
  });
}

}  // namespace
