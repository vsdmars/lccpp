// LeetCode 773: Sliding Puzzle
// https://leetcode.com/problems/sliding-puzzle/
//
// Problem:
//   On a 2 x 3 board containing tiles 1..5 and an empty square 0, a move swaps
//   0 with any 4-directionally adjacent tile. The board is solved when it
//   matches [[1, 2, 3], [4, 5, 0]]. Given an initial permutation of [0..5],
//   return the minimum number of moves required to solve the board, or -1 if
//   the board is unsolvable.
//
// Algorithm:
//   Serialize the 2 x 3 grid row-by-row into a 6-character string (target
//   "123450") and run level-order Breadth-First Search (BFS) over the state
//   graph:
//     1. Precompute the exact 4-directional neighbor indices in the flattened
//        6-character layout for each position 0..5.
//     2. Start with the serialized initial board in a FIFO queue and a
//        `visited` hash set.
//     3. Expand level by level, tracking `moves`. When a dequeued state equals
//        "123450", return `moves`.
//     4. Locate '0' in the current state and swap it with each adjacent index
//        to generate neighbor states; enqueue any unseen state.
//     5. If the queue empties without reaching "123450" (the initial state lies
//        in the disconnected odd-parity permutation component), return -1.
//   Time Complexity:  O(V * L) where V <= 6! = 720 (at most 360 reachable
//                     states) and L = 6 is the board size — effectively O(1).
//   Space Complexity: O(V * L) for the visited set and BFS queue.
//
// Data Structures:
//   - std::string: Compact 6-byte representation of a 2 x 3 board state.
//   - std::array<NeighborList, 6>: Compile-time adjacency table mapping each
//     flattened index (0..5) to its 2 or 3 adjacent board indices.
//   - std::queue<std::string>: FIFO queue for shortest-path BFS.
//   - std::unordered_set<std::string>: Deduplicates visited board states.
//
// Real-World Applications:
//   - Automated planning and puzzle solvers (8-puzzle, 15-puzzle, Rubik's cube
//     BFS/IDA* state-space search).
//   - Automated storage and retrieval systems (AS/RS) and robotic parking
//     garages that slide pallets/cars using a single empty buffer slot.
//   - Verification of reachability classes in permutation-group reconfiguration
//     problems.
//
// Related LeetCode Problems:
//   - #752 Open The Lock [done] (minimum-move BFS over compact discrete states)
//   - #127 Word Ladder [done] (shortest-path BFS over string state transitions)
//   - #854 K-Similar Strings [new] (minimum swaps between string permutations via BFS)
//   - #815 Bus Routes [done] (level-order BFS for minimum transfers)
//   - #1263 Minimum Moves to Move a Box to Their Target Location [done] (2D state-space shortest-path BFS)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <queue>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

namespace {

constexpr std::string_view kTarget = "123450";

struct NeighborList {
  std::array<std::size_t, 3> indices{};
  std::size_t count = 0uz;
};

// Flattened 2x3 index layout:
//   0 1 2
//   3 4 5
// Each entry lists the 4-directionally adjacent indices for that cell.
constexpr std::array<NeighborList, 6> kNeighbors = {{
    {.indices = {1uz, 3uz, 0uz}, .count = 2uz},
    {.indices = {0uz, 2uz, 4uz}, .count = 3uz},
    {.indices = {1uz, 5uz, 0uz}, .count = 2uz},
    {.indices = {0uz, 4uz, 0uz}, .count = 2uz},
    {.indices = {1uz, 3uz, 5uz}, .count = 3uz},
    {.indices = {2uz, 4uz, 0uz}, .count = 2uz},
}};

class Solution {
 public:
  // Returns the fewest tile slides needed to transform `board` into
  // [[1, 2, 3], [4, 5, 0]], or -1 if unsolvable.
  // Time: O(6! * 6), Space: O(6! * 6).
  int slidingPuzzle(const std::vector<std::vector<int>>& board) {
    std::string start;
    start.reserve(6uz);
    for (const auto& row : board) {
      for (int tile : row) {
        start.push_back(static_cast<char>('0' + tile));
      }
    }

    if (start == kTarget) {
      return 0;
    }

    std::unordered_set<std::string> visited;
    std::queue<std::string> frontier;
    visited.insert(start);
    frontier.push(std::move(start));

    int moves = 0;
    while (!frontier.empty()) {
      ++moves;
      const std::size_t level_size = frontier.size();
      for (std::size_t i = 0uz; i < level_size; ++i) {
        std::string current = std::move(frontier.front());
        frontier.pop();

        if (expand_state(current, visited, frontier)) {
          return moves;
        }
      }
    }

    return -1;
  }

 private:
  // Generates all valid slides from `current`, returning true immediately if
  // `kTarget` is produced.
  static bool expand_state(const std::string& current,
                           std::unordered_set<std::string>& visited,
                           std::queue<std::string>& frontier) {
    const std::size_t zero_pos = current.find('0');
    const NeighborList& adj = kNeighbors[zero_pos];

    for (std::size_t k = 0uz; k < adj.count; ++k) {
      std::string next = current;
      std::swap(next[zero_pos], next[adj.indices[k]]);
      if (next == kTarget) {
        return true;
      }
      if (visited.insert(next).second) {
        frontier.push(std::move(next));
      }
    }
    return false;
  }
};

LC_PROBLEM(773, "Sliding Puzzle", "bfs", ctx) {
  ctx.test("already_solved", [&] {
    Solution sol;
    ctx.expect_eq(sol.slidingPuzzle({{1, 2, 3}, {4, 5, 0}}), 0);
  });

  ctx.test("example_1_one_move", [&] {
    Solution sol;
    ctx.expect_eq(sol.slidingPuzzle({{1, 2, 3}, {4, 0, 5}}), 1);
  });

  ctx.test("example_2_unsolvable_odd_parity", [&] {
    Solution sol;
    ctx.expect_eq(sol.slidingPuzzle({{1, 2, 3}, {5, 4, 0}}), -1);
  });

  ctx.test("example_3_five_moves", [&] {
    Solution sol;
    ctx.expect_eq(sol.slidingPuzzle({{4, 1, 2}, {5, 0, 3}}), 5);
  });

  ctx.test("example_4_fourteen_moves", [&] {
    Solution sol;
    ctx.expect_eq(sol.slidingPuzzle({{3, 2, 4}, {1, 5, 0}}), 14);
  });
}

}  // namespace
