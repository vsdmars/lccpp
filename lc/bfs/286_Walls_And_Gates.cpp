// LeetCode 286: Walls And Gates
// https://leetcode.com/problems/walls-and-gates/
//
// Problem:
//   You are given an `m x n` 2D grid `rooms` initialized with three possible
//   values:
//     - `-1`: a wall or impassable obstacle.
//     - `0`: a gate.
//     - `INF` (`2^31 - 1 = 2147483647`): an empty room.
//   Modify `rooms` in place so every empty room holds the 4-directional
//   shortest distance to its nearest gate. Rooms unreachable from any gate
//   must remain `INF`.
//
// Examples:
//   Example 1:
//     Input:  rooms = [[2147483647, -1, 0, 2147483647], [2147483647, 2147483647,
//             2147483647, -1], [2147483647, -1, 2147483647, -1], [0, -1,
//             2147483647, 2147483647]]
//     Output: [[3, -1, 0, 1], [2, 2, 1, -1], [1, -1, 2, -1], [0, -1, 3, 4]]
//     Explanation:
//       - Scan rooms row-major and seed the BFS frontier queue with both gates (0,
//         2) and (3, 0) at distance 0.
//       - Pop (0, 2) with next_dist = 1 to update empty neighbors (0, 3) = 1 and
//         (1, 2) = 1 and enqueue them, then pop (3, 0) to update (2, 0) = 1 and
//         enqueue it.
//       - Pop the distance-1 cells: (1, 2) sets (1, 1) = 2 and (2, 2) = 2, while
//         (2, 0) sets (1, 0) = 2, enqueueing all three.
//       - Expand distance-2 cells: (2, 2) sets (3, 2) = 3 and (1, 0) sets (0, 0) =
//         3, then (3, 2) sets (3, 3) = 4 while walls (-1) and already-filled cells
//         are skipped.
//
//   Example 2:
//     Input:  rooms = [[0, 2147483647, 2147483647, 2147483647, 0]]
//     Output: [[0, 1, 2, 1, 0]]
//     Explanation:
//       - Seed frontier with both gates (0, 0) and (0, 4) simultaneously at
//         distance 0.
//       - Pop (0, 0) with next_dist = 1 to set rooms[0][1] = 1 and enqueue (0, 1),
//         then pop (0, 4) to set rooms[0][3] = 1 and enqueue (0, 3).
//       - Pop (0, 1) with next_dist = 2 to set rooms[0][2] = 2 and enqueue (0, 2);
//         when (0, 3) and (0, 2) are popped, no kEmpty neighbors remain so (0, 2)
//         keeps its shortest distance 2.
//
//   Example 3:
//     Input:  rooms = [[0, -1, 2147483647], [2147483647, -1, 2147483647]]
//     Output: [[0, -1, 2147483647], [1, -1, 2147483647]]
//     Explanation:
//       - Seed frontier with the single gate (0, 0).
//       - Pop (0, 0) with next_dist = 1: neighbor (0, 1) is a wall (-1) and
//         skipped, while empty neighbor (1, 0) becomes 1 and is enqueued.
//       - Pop (1, 0) with next_dist = 2: its right neighbor (1, 1) is a wall (-1)
//         and (0, 0) is not kEmpty, so frontier empties and unreachable cells (0,
//         2) and (1, 2) remain 2147483647.
//
// Algorithm:
//   Multi-source Breadth-First Search (reverse flood-fill from all gates):
//   Instead of launching an O(m * n) BFS from each empty room, enqueue every
//   gate `(r, c)` simultaneously at distance 0. Repeatedly pop `(r, c)` and
//   inspect its 4 orthogonal neighbors `(nr, nc)`. Whenever a neighbor is
//   in-bounds and still equals `kEmpty` (`INF`), set
//   `rooms[nr][nc] = rooms[r][c] + 1` and push `(nr, nc)`. Because the FIFO
//   queue expands in non-decreasing distance order, the first wave to touch an
//   empty room is guaranteed to deliver its minimum distance to any gate, and
//   overwriting `kEmpty` doubles as the visited marker.
//   Time Complexity:  O(m * n) — every cell is enqueued and relaxed at most
//                     once.
//   Space Complexity: O(m * n) in the worst case for the BFS queue.
//
// Data Structures:
//   - `std::queue<std::pair<int, int>>`: Multi-source BFS frontier storing grid
//     coordinates in non-decreasing distance order.
//   - `std::array<std::pair<int, int>, 4>`: Constant 4-directional offset
//     table (right, left, down, up).
//
// Real-World Applications:
//   - Emergency evacuation routing and fire-exit distance-field precomputation
//     in building floor plans and crowd simulations.
//   - Signed distance fields (SDF) and navigation influence maps in robotics
//     and game AI pathfinding (Dijkstra maps).
//   - Cellular tower / CDN edge-node coverage latency maps over discrete grids.
//
// Related LeetCode Problems:
//   - #994 Rotting Oranges [done] (identical multi-source grid BFS wave)
//   - #542 01 Matrix [done] (distance to nearest 0 in a binary grid)
//   - #130 Surrounded Regions [done] (boundary-seeded grid flood fill)
//   - #317 Shortest Distance From All Buildings [done] (sum of BFS distances)
//   - #417 Pacific Atlantic Water Flow [done] (reverse traversal from borders)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

namespace {

constexpr int kEmpty = std::numeric_limits<int>::max();
constexpr int kGate = 0;
constexpr int kWall = -1;

constexpr std::array<std::pair<int, int>, 4> kDirs = {
    {{0, 1}, {0, -1}, {1, 0}, {-1, 0}},
};

class Solution {
 public:
  // Fills each empty room (`kEmpty`) in `rooms` with its shortest 4-directional
  // distance to any gate (`0`) via multi-source BFS.
  // Invariant: Cells leave the queue in monotonically non-decreasing distance
  // order, so updating a cell only when `rooms[nr][nc] == kEmpty` assigns the
  // exact shortest distance on first visit.
  // Time: O(m * n), Space: O(m * n).
  void wallsAndGates(std::vector<std::vector<int>>& rooms) {
    if (rooms.empty() || rooms[0].empty()) {
      return;
    }

    const int rows = static_cast<int>(rooms.size());
    const int cols = static_cast<int>(rooms[0].size());

    // Seed the BFS queue with every gate simultaneously (distance 0 layer).
    std::queue<std::pair<int, int>> frontier;
    for (int r = 0; r < rows; ++r) {
      for (int c = 0; c < cols; ++c) {
        if (rooms[static_cast<std::size_t>(r)][static_cast<std::size_t>(c)] ==
            kGate) {
          frontier.emplace(r, c);
        }
      }
    }

    // Expand outward one step at a time across all gates in lockstep.
    while (!frontier.empty()) {
      const auto [r, c] = frontier.front();
      frontier.pop();
      const int next_dist =
          rooms[static_cast<std::size_t>(r)][static_cast<std::size_t>(c)] + 1;

      for (const auto& [dr, dc] : kDirs) {
        const int nr = r + dr;
        const int nc = c + dc;
        if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) {
          continue;
        }
        int& cell =
            rooms[static_cast<std::size_t>(nr)][static_cast<std::size_t>(nc)];
        if (cell != kEmpty) {
          continue;
        }
        cell = next_dist;
        frontier.emplace(nr, nc);
      }
    }
  }
};

LC_PROBLEM(286, "Walls And Gates", "bfs", ctx) {
  Solution sol;

  ctx.test("example_1_standard_4x4", [&] {
    std::vector<std::vector<int>> rooms = {
        {kEmpty, kWall, kGate, kEmpty},
        {kEmpty, kEmpty, kEmpty, kWall},
        {kEmpty, kWall, kEmpty, kWall},
        {kGate, kWall, kEmpty, kEmpty},
    };
    sol.wallsAndGates(rooms);

    const std::vector<std::vector<int>> expected = {
        {3, kWall, 0, 1},
        {2, 2, 1, kWall},
        {1, kWall, 2, kWall},
        {0, kWall, 3, 4},
    };
    ctx.expect_eq(rooms, expected);
  });

  ctx.test("unreachable_room_stays_inf", [&] {
    std::vector<std::vector<int>> rooms = {
        {kGate, kWall, kEmpty},
        {kEmpty, kWall, kEmpty},
    };
    sol.wallsAndGates(rooms);

    const std::vector<std::vector<int>> expected = {
        {0, kWall, kEmpty},
        {1, kWall, kEmpty},
    };
    ctx.expect_eq(rooms, expected);
  });

  ctx.test("no_gates_grid_unchanged", [&] {
    std::vector<std::vector<int>> rooms = {
        {kEmpty, kWall},
        {kEmpty, kEmpty},
    };
    const auto expected = rooms;
    sol.wallsAndGates(rooms);
    ctx.expect_eq(rooms, expected);
  });

  ctx.test("single_cell_and_empty_grid", [&] {
    std::vector<std::vector<int>> empty_grid;
    sol.wallsAndGates(empty_grid);
    ctx.expect_true(empty_grid.empty());

    std::vector<std::vector<int>> single_wall = {{kWall}};
    sol.wallsAndGates(single_wall);
    ctx.expect_eq(single_wall, std::vector<std::vector<int>>{{kWall}});

    std::vector<std::vector<int>> single_gate = {{kGate}};
    sol.wallsAndGates(single_gate);
    ctx.expect_eq(single_gate, std::vector<std::vector<int>>{{kGate}});
  });

  ctx.test("multiple_gates_shortest_wins", [&] {
    std::vector<std::vector<int>> rooms = {
        {kGate, kEmpty, kEmpty, kEmpty, kGate},
    };
    sol.wallsAndGates(rooms);
    const std::vector<std::vector<int>> expected = {{0, 1, 2, 1, 0}};
    ctx.expect_eq(rooms, expected);
  });
}

}  // namespace
