// LeetCode 490: The Maze
// https://leetcode.com/problems/the-maze/
//
// Problem:
//   Given an m x n binary grid `maze` (0 = empty space, 1 = wall, with
//   implicit walls surrounding the outer border), a `start` cell [row, col],
//   and a `destination` cell [row, col], determine whether a ball placed at
//   `start` can come to a stop at `destination`. On each move, the ball rolls
//   in one of the four cardinal directions (up, down, left, right) and cannot
//   change direction or stop until it hits a wall or the maze boundary.
//
// Examples:
//   Example 1:
//     Input:  maze = [[0, 0, 1, 0, 0], [0, 0, 0, 0, 0], [0, 0, 0, 1, 0], [1, 1, 0,
//             1, 1], [0, 0, 0, 0, 0]], start = [0, 4], destination = [4, 4]
//     Output: true
//     Explanation:
//       - Mark start cell (0, 4) visited and enqueue it into frontier.
//       - Pop (0, 4): rolling left hits the wall at (0, 2) to stop at (0, 3) and
//         rolling down hits the wall at (3, 4) to stop at (2, 4), enqueuing both.
//       - Pop (0, 3) and roll down to stop at (1, 3); popping (1, 3) rolls left to
//         (1, 0), which rolls down to (2, 0), and (2, 0) rolls right to (2, 2).
//       - Pop (2, 2) and roll down to stop at the bottom border cell (4, 2).
//       - Pop (4, 2) and roll right to the right border at (4, 4), which matches
//         destination and returns true immediately.
//
//   Example 2:
//     Input:  maze = [[0, 0, 1, 0, 0], [0, 0, 0, 0, 0], [0, 0, 0, 1, 0], [1, 1, 0,
//             1, 1], [0, 0, 0, 0, 0]], start = [0, 4], destination = [3, 2]
//     Output: false
//     Explanation:
//       - Start BFS from (0, 4), marking visited stopping cells and expanding
//         through (0, 3), (2, 4), (1, 3), (1, 0), (2, 0), and (2, 2).
//       - When rolling down from (2, 2) or up from (4, 2) along column 2,
//         roll_until_wall passes straight through (3, 2) because (3, 2) is open
//         vertically, stopping only at (4, 2) or (1, 2).
//       - Because walls at (3, 1) and (3, 3) also prevent entering (3, 2)
//         horizontally, every reachable stopping position is visited without
//         stopped == (3, 2) and hasPath returns false.
//
//   Example 3:
//     Input:  maze = [[0, 0, 0], [0, 0, 0], [0, 0, 0]], start = [0, 0], destination
//             = [1, 1]
//     Output: false
//     Explanation:
//       - Mark (0, 0) visited and enqueue it; rolling right and down reaches
//         corners (0, 2) and (2, 0).
//       - From (0, 2) and (2, 0), every unobstructed roll travels until hitting the
//         outer grid boundary, discovering only the remaining corner (2, 2).
//       - Center cell (1, 1) is never a stopping position, so frontier empties once
//         all four corners are visited and hasPath returns false.
//
// Algorithm:
//   Model the maze as an implicit directed graph whose vertices are valid
//   stopping positions and whose edges connect each stopping position to the
//   four cells reached by rolling straight until hitting a wall. Run Breadth-
//   First Search (BFS) starting from `start`:
//     1. Dequeue the current stopping cell (r, c).
//     2. For each of the 4 cardinal directions, roll step-by-step until the next
//        cell is out of bounds or a wall, yielding stopping cell (nr, nc).
//     3. If (nr, nc) equals `destination`, return true immediately.
//     4. Otherwise, if (nr, nc) has not yet been visited as a stopping point,
//        mark it visited and enqueue it.
//   Notice that rolling past `destination` without stopping does not count, so
//   we check `destination` only after a roll finishes against a wall.
//   Time Complexity:  O(m * n * (m + n)) in the worst case (at most m * n
//                     unique stopping states, each scanning up to m + n cells
//                     across the 4 directions).
//   Space Complexity: O(m * n) for the visited table and BFS queue.
//
// Data Structures:
//   - std::queue<Cell>: FIFO frontier of newly discovered stopping cells.
//   - std::vector<std::uint8_t>: Flat m * n visited array indexing (r, c) as
//     (r * cols) + c to prevent revisiting stopping states and infinite loops.
//   - std::array<std::pair<int, int>, 4>: Fixed cardinal direction deltas.
//
// Real-World Applications:
//   - Robotics motion planning for low-friction or ballistic agents that glide
//     until encountering an obstacle or bumper sensor.
//   - Puzzle game level verification and reachability solvers (e.g., ice-tile
//     sliding puzzles in video games).
//   - Automated warehouse AGV corridor reachability analysis where vehicles
//     commit to straight aisles between junctions.
//
// Related LeetCode Problems:
//   - #505 The Maze II [done] (shortest rolling distance using Dijkstra)
//   - #499 The Maze III [new] (shortest rolling path to a hole with lexicographic tie-breaking)
//   - #542 01 Matrix [done] (multi-source grid BFS for nearest-cell distance)
//   - #1091 Shortest Path in Binary Matrix [done] (8-directional single-step grid BFS)
//   - #1293 Shortest Path in a Grid with Obstacles Elimination [new] (state-augmented grid BFS)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <queue>
#include <utility>
#include <vector>

namespace {

// Four cardinal rolling directions: right, left, down, up.
constexpr std::array<std::pair<int, int>, 4> kDirs = {
    {{0, 1}, {0, -1}, {1, 0}, {-1, 0}},
};

struct Cell {
  int row = 0;
  int col = 0;

  bool operator==(const Cell&) const = default;
};

class Solution {
 public:
  // Determines whether the rolling ball can start at `start` and come to rest
  // at `destination`.
  // Time: O(m * n * (m + n)), Space: O(m * n).
  bool hasPath(const std::vector<std::vector<int>>& maze,
               const std::vector<int>& start,
               const std::vector<int>& destination) {
    if (maze.empty() || maze[0].empty()) {
      return false;
    }

    const Cell start_cell{.row = start[0], .col = start[1]};
    const Cell dest_cell{.row = destination[0], .col = destination[1]};
    if (start_cell == dest_cell) {
      return true;
    }

    const std::size_t rows = maze.size();
    const std::size_t cols = maze[0].size();

    // Track cells where the ball has already stopped; passing through a cell
    // mid-roll does not mark it visited because the ball could later stop there
    // from a perpendicular direction.
    std::vector<std::uint8_t> visited(rows * cols, 0);
    std::queue<Cell> frontier;

    visited[cell_index(start_cell, cols)] = 1;
    frontier.push(start_cell);

    while (!frontier.empty()) {
      const Cell current = frontier.front();
      frontier.pop();

      for (const auto& [dr, dc] : kDirs) {
        const Cell stopped = roll_until_wall(maze, current, dr, dc);
        if (stopped == dest_cell) {
          return true;
        }

        const std::size_t idx = cell_index(stopped, cols);
        if (visited[idx] == 0) {
          visited[idx] = 1;
          frontier.push(stopped);
        }
      }
    }

    return false;
  }

 private:
  // Converts 2D coordinates into a flat row-major index without signed/unsigned
  // implicit widening.
  static std::size_t cell_index(Cell cell, std::size_t cols) {
    return (static_cast<std::size_t>(cell.row) * cols) +
           static_cast<std::size_t>(cell.col);
  }

  // Rolls the ball from `origin` along (dr, dc) until the next cell would be
  // out of bounds or a wall (1), returning the final resting cell.
  static Cell roll_until_wall(const std::vector<std::vector<int>>& maze,
                              Cell origin, int dr, int dc) {
    const int rows = static_cast<int>(maze.size());
    const int cols = static_cast<int>(maze[0].size());

    int row = origin.row;
    int col = origin.col;
    while (true) {
      const int next_row = row + dr;
      const int next_col = col + dc;
      if (next_row < 0 || next_row >= rows || next_col < 0 ||
          next_col >= cols) {
        break;
      }
      if (maze[static_cast<std::size_t>(next_row)]
              [static_cast<std::size_t>(next_col)] == 1) {
        break;
      }
      row = next_row;
      col = next_col;
    }

    return Cell{.row = row, .col = col};
  }
};

LC_PROBLEM(490, "The Maze", "bfs", ctx) {
  const std::vector<std::vector<int>> kClassicMaze = {
      {0, 0, 1, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 1, 0},
      {1, 1, 0, 1, 1}, {0, 0, 0, 0, 0},
  };

  ctx.test("example_1_reachable_destination", [&] {
    Solution sol;
    ctx.expect_true(sol.hasPath(kClassicMaze, {0, 4}, {4, 4}));
  });

  ctx.test("example_2_cannot_stop_at_destination", [&] {
    Solution sol;
    ctx.expect_false(sol.hasPath(kClassicMaze, {0, 4}, {3, 2}));
  });

  ctx.test("python_build_case_stop_below_wall", [&] {
    Solution sol;
    ctx.expect_true(sol.hasPath(kClassicMaze, {0, 4}, {1, 2}));
  });

  ctx.test("start_equals_destination", [&] {
    Solution sol;
    ctx.expect_true(sol.hasPath({{0}}, {0, 0}, {0, 0}));
  });

  ctx.test("open_center_cannot_stop", [&] {
    Solution sol;
    const std::vector<std::vector<int>> open_maze = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
    };
    // Every roll passes straight through (1, 1) to the outer border.
    ctx.expect_false(sol.hasPath(open_maze, {0, 0}, {1, 1}));
    // Border corners are always reachable stops.
    ctx.expect_true(sol.hasPath(open_maze, {0, 0}, {2, 2}));
  });

  ctx.test("destination_isolated_by_walls", [&] {
    Solution sol;
    const std::vector<std::vector<int>> blocked = {
        {0, 0, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 1, 0},
    };
    ctx.expect_false(sol.hasPath(blocked, {0, 0}, {2, 3}));
  });
}

}  // namespace
