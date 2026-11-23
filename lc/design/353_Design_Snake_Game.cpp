// LeetCode 353: Design Snake Game
// https://leetcode.com/problems/design-snake-game/
//
// Problem:
//   Design a Snake game played on a `height x width` grid. The snake starts at
//   `(0, 0)` with length 1 and score 0. Food items appear sequentially at
//   positions listed in `food` (the next item appears only after the current
//   one is eaten). Each `move(direction)` ("U", "D", "L", "R") advances the
//   head by one cell:
//     - If the head hits a wall or collides with the snake's own body (note:
//       moving into the cell vacated simultaneously by the tail is legal),
//       the game ends and `move()` returns -1.
//     - If the head lands on the active food cell, the snake grows by 1 (tail
//       stays put) and score increases by 1.
//     - Otherwise the tail pops off and the score is unchanged.
//   Return the updated score after each move, or -1 if the game is over.
//
// Algorithm:
//   Maintain the snake's ordered body segments in a double-ended queue (head
//   at front, tail at back) paired with a hash set of currently occupied cells
//   encoded as `row * width + col`. On each `move()`:
//     1. Compute the next head coordinate and immediately fail on wall impact.
//     2. Check whether the next head matches the current food item.
//     3. If NOT eating food, remove the tail from both the deque and the
//        occupied set BEFORE checking self-collision (tail-chasing is legal).
//     4. Check whether the next head is in the occupied set; if so, game over.
//     5. Push the new head onto the front of the deque and insert it into the
//        occupied set; return the current score.
//   Time Complexity: O(1) amortized per `move()`.
//   Space Complexity: O(min(width * height, food.size()) + food.size()) for
//     the snake body, occupancy set, and food schedule.
//
// Data Structures:
//   - `std::deque<std::pair<int, int>>`: O(1) push-front (new head) and
//     pop-back (advancing tail).
//   - `std::unordered_set<std::int64_t>`: O(1) average self-collision lookup
//     keyed by linearized 2D cell coordinates.
//   - `std::vector<std::vector<int>>` + index cursor: O(1) sequential food
//     consumption without mutating or reversing the input food list.
//
// Real-World Applications:
//   - Real-time 2D grid game engines and collision detection loops.
//   - Sliding-window spatial occupancy trackers (e.g., robotic path-following
//     trail buffers and warehouse AGV reserved-cell queues).
//   - Circular trail buffers in graphics and telemetry visualizers.
//
// Related LeetCode Problems:
//   - #348 Design Tic Tac Toe [done] (stateful grid game design)
//   - #622 Design Circular Queue [done] (fixed-capacity FIFO ring buffer)
//   - #874 Walking Robot Simulation [new] (2D grid movement with obstacle set)
//   - #1041 Robot Bounded In Circle [new] (directional state machine on a grid)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <deque>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

namespace {

// Stateful O(1)-per-move Snake game simulation.
class SnakeGame {
 public:
  // Initializes a `height x width` board with the snake at (0, 0) and the
  // given ordered sequence of food coordinates `[row, col]`.
  // Time: O(1) (moves `food`), Space: O(food.size()).
  SnakeGame(int width, int height, std::vector<std::vector<int>> food)
      : width_(width), height_(height), food_(std::move(food)) {
    body_.emplace_front(0, 0);
    occupied_.insert(encode(0, 0));
  }

  // Advances the snake one step in `direction` ("U", "D", "L", or "R").
  // Returns the updated score, or -1 if the move causes wall/self collision
  // (or if the game has already ended).
  // Time: O(1) average, Space: O(1) per move.
  int move(std::string_view direction) {
    if (game_over_) {
      return -1;
    }

    auto [head_r, head_c] = body_.front();
    if (direction == "U") {
      --head_r;
    } else if (direction == "D") {
      ++head_r;
    } else if (direction == "L") {
      --head_c;
    } else if (direction == "R") {
      ++head_c;
    } else {
      game_over_ = true;
      return -1;
    }

    // 1. Wall collision check.
    if (head_r < 0 || head_r >= height_ || head_c < 0 || head_c >= width_) {
      game_over_ = true;
      return -1;
    }

    // 2. Determine whether the snake eats the current active food item.
    const bool eats_food = food_idx_ < food_.size() &&
                           food_[food_idx_][0] == head_r &&
                           food_[food_idx_][1] == head_c;

    if (eats_food) {
      ++food_idx_;
      ++score_;
    } else {
      // Vacate the tail BEFORE self-collision check so moving into the tail's
      // previous cell is allowed.
      const auto [tail_r, tail_c] = body_.back();
      body_.pop_back();
      occupied_.erase(encode(tail_r, tail_c));
    }

    // 3. Self-collision check against all remaining body segments.
    const std::int64_t next_key = encode(head_r, head_c);
    if (occupied_.contains(next_key)) {
      game_over_ = true;
      return -1;
    }

    // 4. Commit the new head position.
    body_.emplace_front(head_r, head_c);
    occupied_.insert(next_key);
    return score_;
  }

 private:
  // Linearizes (row, col) into a unique 64-bit integer key.
  [[nodiscard]] std::int64_t encode(int row, int col) const noexcept {
    return (static_cast<std::int64_t>(row) * width_) + col;
  }

  int width_ = 0;
  int height_ = 0;
  std::vector<std::vector<int>> food_;
  std::size_t food_idx_ = 0;
  std::deque<std::pair<int, int>> body_;
  std::unordered_set<std::int64_t> occupied_;
  int score_ = 0;
  bool game_over_ = false;
};

LC_PROBLEM(353, "Design Snake Game", "design", ctx) {
  ctx.test("leetcode example (python build)", [&] {
    SnakeGame game(3, 2, {{1, 2}, {0, 1}});
    ctx.expect_eq(game.move("R"), 0);
    ctx.expect_eq(game.move("D"), 0);
    ctx.expect_eq(game.move("R"), 1);
    ctx.expect_eq(game.move("U"), 1);
    ctx.expect_eq(game.move("L"), 2);
    ctx.expect_eq(game.move("U"), -1);
    // Subsequent moves after game over remain -1.
    ctx.expect_eq(game.move("D"), -1);
  });

  ctx.test("chasing own vacating tail is legal on 2x2 grid", [&] {
    // Grow snake to length 4 covering all cells of a 2x2 board:
    // (0,0) -> (0,1) [eat] -> (1,1) [eat] -> (1,0) [eat], then step into
    // (0,0) which is simultaneously vacated by the tail.
    SnakeGame game(2, 2, {{0, 1}, {1, 1}, {1, 0}});
    ctx.expect_eq(game.move("R"), 1);
    ctx.expect_eq(game.move("D"), 2);
    ctx.expect_eq(game.move("L"), 3);
    ctx.expect_eq(game.move("U"), 3);
    ctx.expect_eq(game.move("R"), 3);
  });

  ctx.test("self-collision ends the game", [&] {
    // Grow snake to length 5 on a 3x3 board and turn into its own neck/body.
    SnakeGame game(3, 3, {{0, 1}, {0, 2}, {1, 2}, {1, 1}});
    ctx.expect_eq(game.move("R"), 1);  // (0,1)
    ctx.expect_eq(game.move("R"), 2);  // (0,2)
    ctx.expect_eq(game.move("D"), 3);  // (1,2)
    ctx.expect_eq(game.move("L"), 4);  // (1,1) - body: (1,1),(1,2),(0,2),(0,1),(0,0)
    ctx.expect_eq(game.move("U"), -1);  // (0,1) is still occupied after (0,0) vacates
  });

  ctx.test("empty food list and immediate boundary hit", [&] {
    SnakeGame game(2, 2, {});
    ctx.expect_eq(game.move("R"), 0);
    ctx.expect_eq(game.move("L"), 0);
    ctx.expect_eq(game.move("L"), -1);
  });
}

}  // namespace
