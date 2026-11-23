// LeetCode 505: The Maze II
// https://leetcode.com/problems/the-maze-ii/
//
// Problem:
//   Given an m x n binary grid `maze` (0 = empty space, 1 = wall, surrounded
//   by implicit border walls), a `start` cell [row, col], and a `destination`
//   cell [row, col], find the shortest distance for a rolling ball to come to
//   a stop at `destination`. The ball rolls in one of the four cardinal
//   directions until it hits a wall, and distance is the total number of empty
//   cells traveled from `start` (excluded) to `destination` (included). Return
//   -1 if the ball cannot stop at `destination`.
//
// Algorithm:
//   Because each roll traverses a variable number of cells (edge weight >= 1),
//   unweighted BFS minimizes the number of turns rather than total distance.
//   Use Dijkstra's shortest-path algorithm over stopping cells:
//     1. Maintain a distance table `dist` initialized to +infinity, with
//        `dist[start] = 0`, and push {0, start} into a min-heap.
//     2. Pop the state with the smallest accumulated distance `d`. If `d` is
//        larger than `dist[cell]`, discard it as an outdated heap entry.
//     3. If `cell == destination`, return `d` immediately (all edge weights
//        are non-negative, so the first extraction of `destination` is optimal).
//     4. For each of the 4 directions, roll until hitting a wall, counting the
//        number of steps taken. If `d + steps < dist[next]`, relax `dist[next]`
//        and push `{d + steps, next}` onto the min-heap.
//   Time Complexity:  O(m * n * (m + n) + m * n * log(m * n)) because each of
//                     the m * n stopping cells is settled at most once, rolling
//                     up to m + n cells across 4 directions and triggering at
//                     most 4 * m * n heap relaxations.
//   Space Complexity: O(m * n) for the distance table and priority queue.
//
// Data Structures:
//   - std::priority_queue<State, std::vector<State>, std::greater<>>: Min-heap
//     ordered by cumulative rolled distance for Dijkstra's extraction step.
//   - std::vector<int>: Flat m * n table storing the shortest known distance
//     to stop at each cell.
//   - std::array<std::pair<int, int>, 4>: Cardinal direction offsets.
//
// Real-World Applications:
//   - Minimum-distance trajectory planning for autonomous vehicles or drones
//     constrained to straight-line segments between waypoints/obstacles.
//   - Network routing and weighted state-space shortest-path solvers (e.g.,
//     OSPF link-state routing using Dijkstra's algorithm).
//   - Game AI pathfinding in turn-restricted or momentum-based environments.
//
// Related LeetCode Problems:
//   - #490 The Maze [done] (unweighted reachability variant using BFS)
//   - #499 The Maze III [new] (shortest rolling distance with lexicographic path output)
//   - #743 Network Delay Time [new] (canonical Dijkstra shortest-path on weighted graphs)
//   - #787 Cheapest Flights Within K Stops [done] (constrained weighted shortest path)
//   - #1631 Path With Minimum Effort [new] (Dijkstra over a 2D grid with custom edge weights)

#include "framework/lc_framework.h"

#include <array>
#include <compare>
#include <cstddef>
#include <functional>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

namespace {

constexpr std::array<std::pair<int, int>, 4> kDirs = {
    {{0, 1}, {0, -1}, {1, 0}, {-1, 0}},
};

struct Cell {
  int row = 0;
  int col = 0;

  auto operator<=>(const Cell&) const = default;
};

struct State {
  int dist = 0;
  Cell cell;

  auto operator<=>(const State&) const = default;
};

struct RollResult {
  Cell stopped;
  int steps = 0;
};

class Solution {
 public:
  // Computes the minimum number of empty spaces traveled to stop at
  // `destination`, or -1 if unreachable.
  // Time: O(m * n * (m + n) + m * n * log(m * n)), Space: O(m * n).
  int shortestDistance(const std::vector<std::vector<int>>& maze,
                       const std::vector<int>& start,
                       const std::vector<int>& destination) {
    if (maze.empty() || maze[0].empty()) {
      return -1;
    }

    const Cell start_cell{.row = start[0], .col = start[1]};
    const Cell dest_cell{.row = destination[0], .col = destination[1]};
    if (start_cell == dest_cell) {
      return 0;
    }

    const std::size_t rows = maze.size();
    const std::size_t cols = maze[0].size();
    const int kInf = std::numeric_limits<int>::max();

    std::vector<int> dist(rows * cols, kInf);
    std::priority_queue<State, std::vector<State>, std::greater<>> min_heap;

    dist[cell_index(start_cell, cols)] = 0;
    min_heap.push(State{.dist = 0, .cell = start_cell});

    while (!min_heap.empty()) {
      const State current = min_heap.top();
      min_heap.pop();

      // Skip stale priority-queue entries superseded by a shorter path.
      if (current.dist > dist[cell_index(current.cell, cols)]) {
        continue;
      }

      // First extraction of the destination is guaranteed to be optimal.
      if (current.cell == dest_cell) {
        return current.dist;
      }

      for (const auto& [dr, dc] : kDirs) {
        const RollResult roll = roll_until_wall(maze, current.cell, dr, dc);
        if (roll.steps == 0) {
          continue;
        }

        const int next_dist = current.dist + roll.steps;
        const std::size_t next_idx = cell_index(roll.stopped, cols);
        if (next_dist < dist[next_idx]) {
          dist[next_idx] = next_dist;
          min_heap.push(State{.dist = next_dist, .cell = roll.stopped});
        }
      }
    }

    return -1;
  }

 private:
  static std::size_t cell_index(Cell cell, std::size_t cols) {
    return (static_cast<std::size_t>(cell.row) * cols) +
           static_cast<std::size_t>(cell.col);
  }

  // Rolls from `origin` along (dr, dc) until hitting a wall or border,
  // returning both the resting cell and the number of spaces rolled.
  static RollResult roll_until_wall(const std::vector<std::vector<int>>& maze,
                                    Cell origin, int dr, int dc) {
    const int rows = static_cast<int>(maze.size());
    const int cols = static_cast<int>(maze[0].size());

    int row = origin.row;
    int col = origin.col;
    int steps = 0;
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
      ++steps;
    }

    return RollResult{.stopped = Cell{.row = row, .col = col}, .steps = steps};
  }
};

LC_PROBLEM(505, "The Maze II", "bfs", ctx) {
  const std::vector<std::vector<int>> kClassicMaze = {
      {0, 0, 1, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 1, 0},
      {1, 1, 0, 1, 1}, {0, 0, 0, 0, 0},
  };

  ctx.test("example_1_shortest_path_12", [&] {
    Solution sol;
    ctx.expect_eq(sol.shortestDistance(kClassicMaze, {0, 4}, {4, 4}), 12);
  });

  ctx.test("example_2_unreachable_stop", [&] {
    Solution sol;
    ctx.expect_eq(sol.shortestDistance(kClassicMaze, {0, 4}, {3, 2}), -1);
  });

  ctx.test("classic_maze_stop_at_1_2", [&] {
    Solution sol;
    // (0,4)->(0,3)[1]->(1,3)[1]->(1,0)[3]->(2,0)[1]->(2,2)[2]->(1,2)[1] = 9
    ctx.expect_eq(sol.shortestDistance(kClassicMaze, {0, 4}, {1, 2}), 9);
  });

  ctx.test("more_turns_shorter_distance_beats_unweighted_bfs", [&] {
    Solution sol;
    // Right detour: (0,1)->(0,5)[4]->(2,5)[2]->(2,2)[3] = 9 (3 rolls).
    // Left detour:  (0,1)->(0,0)[1]->(3,0)[3]->(3,2)[2]->(2,2)[1] = 7 (4 rolls).
    const std::vector<std::vector<int>> maze = {
        {0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0},
        {0, 1, 0, 0, 0, 0},
        {0, 0, 0, 1, 1, 1},
    };
    ctx.expect_eq(sol.shortestDistance(maze, {0, 1}, {2, 2}), 7);
  });

  ctx.test("start_equals_destination", [&] {
    Solution sol;
    ctx.expect_eq(sol.shortestDistance(kClassicMaze, {0, 4}, {0, 4}), 0);
  });

  ctx.test("single_corridor", [&] {
    Solution sol;
    const std::vector<std::vector<int>> corridor = {{0, 0, 0, 0}};
    ctx.expect_eq(sol.shortestDistance(corridor, {0, 0}, {0, 3}), 3);
    ctx.expect_eq(sol.shortestDistance(corridor, {0, 0}, {0, 2}), -1);
  });
}

}  // namespace
