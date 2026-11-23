// LeetCode 1263: Minimum Moves To Move A Box To Their Target Location
// https://leetcode.com/problems/minimum-moves-to-move-a-box-to-their-target-location/
//
// Problem:
//   Given an m x n warehouse grid (1 <= m, n <= 20) consisting of walls '#',
//   open floor '.', one player 'S', one box 'B', and one target 'T', return
//   the minimum number of pushes needed to move the box to the target cell,
//   or -1 if it is impossible. The player moves in 4 cardinal directions on
//   non-wall cells, cannot walk through the box, and pushes the box one cell
//   forward by standing on the opposite adjacent cell.
//
// Algorithm:
//   Nested BFS (state-space BFS with an inner reachability BFS):
//   - Outer BFS explores states (box_pos, player_pos) level by level where
//     each transition corresponds to exactly one box push, ensuring the first
//     time the box reaches the target yields the minimum push count.
//   - For each of the 4 cardinal push directions d, pushing the box from b to
//     b + d requires both b + d (new box cell) and b - d (required player
//     standing cell) to be in-bounds non-wall cells, and the player must be
//     able to walk from their current position to b - d without crossing b.
//   - An inner BFS checks player reachability while treating the current box
//     cell as an impassable obstacle.
//   - After pushing the box from b to b + d, the player steps into b. Since
//     the pair (new_box_cell, push_direction) uniquely determines both the
//     new box cell and the player's post-push cell b, marking
//     visited[new_box_id * 4 + dir] prevents revisiting identical push states.
//   - Time Complexity: O((m * n)^2) in the worst case (at most 4 * m * n
//     distinct push transitions, each running an O(m * n) reachability BFS).
//   - Space Complexity: O(m * n) for the visited state table and BFS queues.
//
// Data Structures:
//   - Fixed 2D offset array (`std::array<std::pair<int, int>, 4>`) for
//     4-directional movement.
//   - Outer FIFO queue (`std::queue<PushState>`) for minimum-push BFS.
//   - Inner FIFO queue (`std::queue<Pos>`) and flat byte array
//     (`std::vector<std::uint8_t>`) for player walkability checks.
//   - Flat visited byte array indexed by (box_cell * 4 + dir) to deduplicate
//     outer BFS transitions in O(1) time.
//
// Real-World Applications:
//   - Automated warehouse robotics (AGV/forklift path planning with movable
//     pallets and narrow aisles).
//   - Sokoban-style motion planning and puzzle verification in game AI.
//   - Robotics manipulation planning where an agent repositions obstacles to
//     reach a goal configuration.
//
// Related LeetCode Problems:
//   - #1293 Shortest Path in a Grid with Obstacles Elimination [new] (state-space BFS on a grid with extra state dimensions)
//   - #864 Shortest Path to Get All Keys [new] (multi-dimensional grid BFS with state deduplication)
//   - #773 Sliding Puzzle [done] (minimum-move BFS over board configurations)
//   - #1091 Shortest Path in Binary Matrix [done] (unweighted shortest path via grid BFS)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <queue>
#include <utility>
#include <vector>

namespace {

constexpr std::array<std::pair<int, int>, 4> kDirs = {{
    {0, 1},
    {0, -1},
    {1, 0},
    {-1, 0},
}};

struct Pos {
  int row = -1;
  int col = -1;

  bool operator==(const Pos& other) const = default;
};

struct PushState {
  int pushes = 0;
  Pos player;
  Pos box;
};

class Solution {
 public:
  // Computes the minimum number of pushes to move 'B' onto 'T', or -1 if
  // unreachable.
  // Time: O((m * n)^2) worst case. Space: O(m * n).
  int minPushBox(const std::vector<std::vector<char>>& grid) {
    if (grid.empty() || grid[0].empty()) {
      return -1;
    }

    const int rows = static_cast<int>(grid.size());
    const int cols = static_cast<int>(grid[0].size());
    const std::size_t total_cells = grid.size() * grid[0].size();

    // Locate initial box, player, and target coordinates.
    const auto [box_start, player_start, target] = find_positions(grid);
    if (box_start.row == -1 || player_start.row == -1 || target.row == -1) {
      return -1;
    }

    // Track visited (new_box_cell, push_direction) pairs. Each valid push in
    // direction `dir` lands the box at `next_box` and places the player on
    // the box's former cell, so this key uniquely captures the post-push state.
    std::vector<std::uint8_t> visited(total_cells * 4uz, 0);
    std::queue<PushState> queue;
    queue.push(PushState{.pushes = 0, .player = player_start, .box = box_start});

    while (!queue.empty()) {
      const PushState curr = queue.front();
      queue.pop();

      if (curr.box == target) {
        return curr.pushes;
      }

      // Attempt to push the box in each of the 4 cardinal directions.
      for (std::size_t dir_idx = 0uz; dir_idx < kDirs.size(); ++dir_idx) {
        const auto [dr, dc] = kDirs[dir_idx];
        const Pos next_box{.row = curr.box.row + dr, .col = curr.box.col + dc};
        const Pos push_from{.row = curr.box.row - dr, .col = curr.box.col - dc};

        if (!is_open(grid, rows, cols, next_box) ||
            !is_open(grid, rows, cols, push_from)) {
          continue;
        }

        const std::size_t state_key =
            (cell_id(next_box, grid[0].size()) * 4uz) + dir_idx;
        if (visited[state_key] != 0) {
          continue;
        }

        // Verify the player can walk to `push_from` without passing through
        // the box's current cell.
        if (!can_reach(grid, rows, cols, curr.player, push_from, curr.box)) {
          continue;
        }

        visited[state_key] = 1;
        // After pushing, the player moves into the box's previous cell.
        queue.push(PushState{
            .pushes = curr.pushes + 1,
            .player = curr.box,
            .box = next_box,
        });
      }
    }

    return -1;
  }

 private:
  // Scans the grid once to find the box 'B', player 'S', and target 'T'.
  static std::array<Pos, 3> find_positions(
      const std::vector<std::vector<char>>& grid) {
    Pos box;
    Pos player;
    Pos target;

    for (std::size_t r = 0uz; r < grid.size(); ++r) {
      for (std::size_t c = 0uz; c < grid[r].size(); ++c) {
        const char ch = grid[r][c];
        const Pos pos{.row = static_cast<int>(r), .col = static_cast<int>(c)};
        if (ch == 'B') {
          box = pos;
        } else if (ch == 'S') {
          player = pos;
        } else if (ch == 'T') {
          target = pos;
        }
      }
    }

    return {box, player, target};
  }

  // Flattens a valid in-bounds coordinate into a row-major cell index.
  static std::size_t cell_id(const Pos& pos, std::size_t cols) {
    return (static_cast<std::size_t>(pos.row) * cols) +
           static_cast<std::size_t>(pos.col);
  }

  // Returns true if `pos` is within grid bounds and is not a wall '#'.
  static bool is_open(const std::vector<std::vector<char>>& grid, int rows,
                      int cols, const Pos& pos) {
    if (pos.row < 0 || pos.row >= rows || pos.col < 0 || pos.col >= cols) {
      return false;
    }
    return grid[static_cast<std::size_t>(pos.row)]
               [static_cast<std::size_t>(pos.col)] != '#';
  }

  // Inner BFS: returns true if the player can walk from `start` to `goal`
  // without stepping onto walls or the current `box` position.
  static bool can_reach(const std::vector<std::vector<char>>& grid, int rows,
                        int cols, const Pos& start, const Pos& goal,
                        const Pos& box) {
    if (start == goal) {
      return true;
    }

    const auto ucols = static_cast<std::size_t>(cols);
    std::vector<std::uint8_t> seen(static_cast<std::size_t>(rows) * ucols, 0);
    std::queue<Pos> queue;
    seen[cell_id(start, ucols)] = 1;
    queue.push(start);

    while (!queue.empty()) {
      const Pos curr = queue.front();
      queue.pop();

      for (const auto& [dr, dc] : kDirs) {
        const Pos next{.row = curr.row + dr, .col = curr.col + dc};
        if (next == box || !is_open(grid, rows, cols, next)) {
          continue;
        }

        const std::size_t id = cell_id(next, ucols);
        if (seen[id] != 0) {
          continue;
        }
        if (next == goal) {
          return true;
        }

        seen[id] = 1;
        queue.push(next);
      }
    }

    return false;
  }
};

LC_PROBLEM(1263, "Minimum Moves To Move A Box To Their Target Location", "bfs",
           ctx) {
  ctx.test("example_1_three_pushes", [&] {
    const std::vector<std::vector<char>> grid = {
        {'#', '#', '#', '#', '#', '#'}, {'#', 'T', '#', '#', '#', '#'},
        {'#', '.', '.', 'B', '.', '#'}, {'#', '.', '#', '#', '.', '#'},
        {'#', '.', '.', '.', 'S', '#'}, {'#', '#', '#', '#', '#', '#'},
    };
    Solution sol;
    ctx.expect_eq(sol.minPushBox(grid), 3);
  });

  ctx.test("example_2_blocked_corridor", [&] {
    const std::vector<std::vector<char>> grid = {
        {'#', '#', '#', '#', '#', '#'}, {'#', 'T', '#', '#', '#', '#'},
        {'#', '.', '.', 'B', '.', '#'}, {'#', '#', '#', '#', '.', '#'},
        {'#', '.', '.', '.', 'S', '#'}, {'#', '#', '#', '#', '#', '#'},
    };
    Solution sol;
    ctx.expect_eq(sol.minPushBox(grid), -1);
  });

  ctx.test("example_3_five_pushes", [&] {
    const std::vector<std::vector<char>> grid = {
        {'#', '#', '#', '#', '#', '#'}, {'#', 'T', '.', '.', '#', '#'},
        {'#', '.', '#', 'B', '.', '#'}, {'#', '.', '.', '.', '.', '#'},
        {'#', '.', '.', '.', 'S', '#'}, {'#', '#', '#', '#', '#', '#'},
    };
    Solution sol;
    ctx.expect_eq(sol.minPushBox(grid), 5);
  });

  ctx.test("example_4_player_trapped_behind_wall", [&] {
    const std::vector<std::vector<char>> grid = {
        {'#', '#', '#', '#', '#', '#', '#'},
        {'#', 'S', '#', '.', 'B', 'T', '#'},
        {'#', '#', '#', '#', '#', '#', '#'},
    };
    Solution sol;
    ctx.expect_eq(sol.minPushBox(grid), -1);
  });

  ctx.test("single_push_straight_line", [&] {
    const std::vector<std::vector<char>> grid = {
        {'S', 'B', 'T'},
    };
    Solution sol;
    ctx.expect_eq(sol.minPushBox(grid), 1);
  });

  ctx.test("box_trapped_in_corner_unreachable", [&] {
    const std::vector<std::vector<char>> grid = {
        {'B', '.', 'T'},
        {'.', '.', 'S'},
    };
    Solution sol;
    ctx.expect_eq(sol.minPushBox(grid), -1);
  });
}

}  // namespace
