// LeetCode 489: Robot Room Cleaner
// https://leetcode.com/problems/robot-room-cleaner/
//
// Problem:
//   A robot cleaner operates in an unknown 2D grid room where `1` represents
//   an open cell and `0` represents a blocked cell (with walls surrounding the
//   grid). The robot starts on an open cell facing up, knowing neither the
//   room layout nor its absolute coordinates. Using only four API operations —
//   `move()`, `turnLeft()`, `turnRight()`, and `clean()` — design an algorithm
//   to clean every reachable cell in the room.
//
// Algorithm:
//   Perform blindfolded DFS with explicit physical backtracking in a relative
//   coordinate frame centered at the starting cell `(0, 0)` with initial
//   heading `dir = 0` (Up). The four clockwise headings `kDirs` are Up, Right,
//   Down, Left. At each newly visited cell `(r, c, dir)`: mark `(r, c)`
//   visited, call `robot.clean()`, and iterate through all 4 clockwise
//   directions starting from `dir`. For each candidate neighbor `(nr, nc)` in
//   direction `next_dir`, if `(nr, nc)` is unvisited and `robot.move()`
//   succeeds, recurse into `(nr, nc, next_dir)`, and upon return execute a
//   180-degree turn, step back to `(r, c)`, and turn 180 degrees again to
//   restore both position and orientation. Always turn right at the end of
//   each loop iteration so after 4 iterations the robot faces its entry
//   heading `dir` again.
//   Time Complexity:  O(N - M), where N is the number of cells and M is the
//                     number of obstacles; each accessible cell is visited once
//                     and probed from at most 4 directions.
//   Space Complexity: O(N - M) for the visited coordinate set and DFS stack.
//
// Data Structures:
//   - Hash set (`std::unordered_set<std::uint64_t>`): stores bit-packed
//     relative `(row, col)` coordinates of already-visited open cells.
//   - Fixed direction array (`std::array<std::pair<int, int>, 4>`): encodes
//     clockwise 90-degree turns matching `Robot::turnRight()`.
//
// Real-World Applications:
//   - Autonomous floor-care & warehouse robotics (SLAM coverage planning):
//     guaranteeing complete exploration of an unmapped environment using only
//     local odometry and bump sensors.
//   - Automated rover & drone inspection: systematic cave, tunnel, or pipe
//     mapping when GPS and prior blueprints are unavailable.
//   - Graph exploration by mobile agents in distributed sensor networks.
//
// Related LeetCode Problems:
//   - #200 Number of Islands [done] (grid DFS connected-component traversal)
//   - #695 Max Area of Island [done] (flood-fill DFS over 4-connected cells)
//   - #79 Word Search [done] (DFS with state restoration on backtrack)
//   - #1778 Shortest Path in a Hidden Grid [new] (interactive blindfolded grid DFS+BFS)
//   - #1810 Minimum Path Cost in a Hidden Grid [new] (interactive hidden grid search)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <unordered_set>
#include <utility>
#include <vector>

namespace {

// Clockwise direction deltas matching 90-degree right turns:
// 0 = Up (-1, 0), 1 = Right (0, 1), 2 = Down (1, 0), 3 = Left (0, -1).
constexpr std::array<std::pair<int, int>, 4> kDirs = {
    {{-1, 0}, {0, 1}, {1, 0}, {0, -1}}};

// Packs signed relative `(row, col)` into an unsigned 64-bit key without UB
// even when relative coordinates are negative.
std::uint64_t encodeCell(int r, int c) {
  const auto ur = static_cast<std::uint64_t>(static_cast<std::uint32_t>(r));
  const auto uc = static_cast<std::uint64_t>(static_cast<std::uint32_t>(c));
  return (ur << 32U) | uc;
}

// Mock interactive Robot matching the LeetCode judge interface, plus read-only
// test accessors to verify full-room coverage and final pose.
class Robot {
 public:
  Robot(std::vector<std::vector<int>> room, int start_row, int start_col)
      : room_(std::move(room)), row_(start_row), col_(start_col) {}

  // Moves one cell forward if open and in bounds; otherwise stays put.
  bool move() {
    const auto [dr, dc] = kDirs[static_cast<std::size_t>(dir_)];
    const int nr = row_ + dr;
    const int nc = col_ + dc;
    if (nr < 0 || std::cmp_greater_equal(nr, room_.size()) || nc < 0 ||
        std::cmp_greater_equal(nc, room_[0].size()) ||
        room_[static_cast<std::size_t>(nr)][static_cast<std::size_t>(nc)] ==
            0) {
      return false;
    }
    row_ = nr;
    col_ = nc;
    return true;
  }

  void turnLeft() { dir_ = (dir_ + 3) % 4; }

  void turnRight() { dir_ = (dir_ + 1) % 4; }

  void clean() { cleaned_.insert(encodeCell(row_, col_)); }

  // Test helpers (not called by Solution).
  [[nodiscard]] bool allAccessibleCellsClean() const {
    for (std::size_t r = 0uz; r < room_.size(); ++r) {
      for (std::size_t c = 0uz; c < room_[r].size(); ++c) {
        if (room_[r][c] == 1 &&
            !cleaned_.contains(
                encodeCell(static_cast<int>(r), static_cast<int>(c)))) {
          return false;
        }
      }
    }
    return true;
  }

  [[nodiscard]] std::size_t cleanedCount() const { return cleaned_.size(); }
  [[nodiscard]] int row() const { return row_; }
  [[nodiscard]] int col() const { return col_; }
  [[nodiscard]] int dir() const { return dir_; }

 private:
  std::vector<std::vector<int>> room_;
  int row_ = 0;
  int col_ = 0;
  int dir_ = 0;  // Starts facing Up.
  std::unordered_set<std::uint64_t> cleaned_;
};

class Solution {
 public:
  // Cleans every reachable open cell in the room using only the Robot API.
  // Time: O(N - M) | Space: O(N - M), where N = grid cells, M = obstacles.
  void cleanRoom(Robot& robot) {
    std::unordered_set<std::uint64_t> visited;

    // Invariant: entering `dfs(r, c, dir)` places the robot at relative `(r, c)`
    // facing `dir`, and returning leaves the robot at `(r, c)` still facing `dir`.
    auto dfs = [&](this auto&& self, int r, int c, int dir) -> void {
      visited.insert(encodeCell(r, c));
      robot.clean();

      // Probe all 4 directions clockwise, turning right after each attempt so
      // 4 right turns restore the robot's original orientation `dir`.
      for (int step = 0; step < 4; ++step) {
        const int next_dir = (dir + step) % 4;
        const auto [dr, dc] = kDirs[static_cast<std::size_t>(next_dir)];
        const int nr = r + dr;
        const int nc = c + dc;

        if (!visited.contains(encodeCell(nr, nc)) && robot.move()) {
          self(nr, nc, next_dir);
          stepBack(robot);
        }

        robot.turnRight();
      }
    };

    dfs(0, 0, 0);
  }

 private:
  // Reverses one step into the previous cell and restores the facing direction.
  static void stepBack(Robot& robot) {
    robot.turnRight();
    robot.turnRight();
    robot.move();
    robot.turnRight();
    robot.turnRight();
  }
};

LC_PROBLEM(489, "Robot Room Cleaner", "dfs", ctx) {
  ctx.test("example_1_from_python", [&] {
    const std::vector<std::vector<int>> room = {
        {1, 1, 1, 1, 1, 0, 1, 1},
        {1, 1, 1, 1, 1, 0, 1, 1},
        {1, 0, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 1, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1},
    };
    Robot robot(room, 1, 3);
    Solution sol;
    sol.cleanRoom(robot);

    ctx.expect_true(robot.allAccessibleCellsClean());
    ctx.expect_eq(robot.cleanedCount(), 30uz);
    // Clean backtracking returns the robot to its starting cell & heading.
    ctx.expect_eq(robot.row(), 1);
    ctx.expect_eq(robot.col(), 3);
    ctx.expect_eq(robot.dir(), 0);
  });

  ctx.test("single_cell_room", [&] {
    Robot robot({{1}}, 0, 0);
    Solution sol;
    sol.cleanRoom(robot);

    ctx.expect_true(robot.allAccessibleCellsClean());
    ctx.expect_eq(robot.cleanedCount(), 1uz);
  });

  ctx.test("corridor_starting_in_middle", [&] {
    Robot robot({{1, 1, 1, 1, 1}}, 0, 2);
    Solution sol;
    sol.cleanRoom(robot);

    ctx.expect_true(robot.allAccessibleCellsClean());
    ctx.expect_eq(robot.cleanedCount(), 5uz);
    ctx.expect_eq(robot.row(), 0);
    ctx.expect_eq(robot.col(), 2);
  });

  ctx.test("ring_around_central_obstacle", [&] {
    const std::vector<std::vector<int>> room = {
        {1, 1, 1},
        {1, 0, 1},
        {1, 1, 1},
    };
    Robot robot(room, 2, 2);
    Solution sol;
    sol.cleanRoom(robot);

    ctx.expect_true(robot.allAccessibleCellsClean());
    ctx.expect_eq(robot.cleanedCount(), 8uz);
  });
}

}  // namespace
