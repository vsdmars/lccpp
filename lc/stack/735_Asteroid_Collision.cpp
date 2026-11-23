// LeetCode 735: Asteroid Collision
// https://leetcode.com/problems/asteroid-collision/
//
// Problem:
//   Given an array `asteroids` of non-zero integers representing asteroids in a
//   row, where `abs(asteroids[i])` is the asteroid's size and its sign is its
//   direction (positive = moving right, negative = moving left), all moving at
//   the same speed, return the state of the asteroids after all collisions.
//   When two asteroids meet, the smaller one explodes; if both have the same
//   size, both explode. Asteroids moving in the same direction (or away from
//   each other) never collide.
//
// Algorithm:
//   Sweep left-to-right while maintaining a stack of surviving asteroids that
//   are already in a collision-free prefix state. A new asteroid can collide
//   with the top of the stack if and only if the stack top moves right (`> 0`)
//   and the incoming asteroid moves left (`< 0`):
//     - While the incoming asteroid is still alive and collides with `stack.back()`:
//       * If `stack.back() < -asteroid`, the stack top explodes (pop it) and
//         the incoming asteroid continues colliding leftward.
//       * If `stack.back() == -asteroid`, both explode (pop and mark dead).
//       * If `stack.back() > -asteroid`, only the incoming asteroid explodes
//         (mark dead).
//     - If the incoming asteroid survives all collisions, push it onto `stack`.
//   Time Complexity:  O(n), as every asteroid is pushed onto the stack at most
//                     once and popped at most once.
//   Space Complexity: O(n) for the surviving-asteroid stack.
//
// Data Structures:
//   - Collision stack (`std::vector<int>`): maintains the resolved prefix of
//     surviving asteroids in left-to-right order.
//
// Real-World Applications:
//   - 1D particle / packet collision simulation and traffic shockwave modeling.
//   - Simplifying opposing directional moves or opposing vector deltas in
//     command-stream compression (e.g., cancelling opposite undo/move actions).
//   - Stack-based algebraic reduction in free groups (cancelling adjacent
//     inverse elements).
//
// Related LeetCode Problems:
//   - #2751 Robot Collisions [new] (2D-sorted multi-property collision stack)
//   - #2211 Count Collisions on a Road [new] (directional collision counting)
//   - #20 Valid Parentheses [done] (stack cancellation of matching opposites)
//   - #1047 Remove All Adjacent Duplicates In String [done] (adjacent stack reduction)
//   - #739 Daily Temperatures [done] (monotonic stack sweep)

#include "framework/lc_framework.h"

#include <vector>

namespace {

class Solution {
 public:
  // Simulates all pairwise asteroid collisions from left to right and returns
  // the surviving asteroids in order.
  // Time: O(n), Space: O(n).
  std::vector<int> asteroidCollision(const std::vector<int>& asteroids) {
    // Invariant: `stack` contains all surviving asteroids from the processed
    // prefix; no right-moving asteroid precedes a left-moving asteroid in it.
    std::vector<int> stack;
    stack.reserve(asteroids.size());

    for (int asteroid : asteroids) {
      bool alive = true;

      // Only a right-moving stack top meeting a left-moving incoming asteroid
      // can collide.
      while (alive && !stack.empty() && asteroid < 0 && stack.back() > 0) {
        const int incoming_size = -asteroid;
        if (stack.back() < incoming_size) {
          // Right-moving top is smaller and explodes; incoming keeps moving.
          stack.pop_back();
        } else if (stack.back() == incoming_size) {
          // Equal sizes: mutual annihilation.
          stack.pop_back();
          alive = false;
        } else {
          // Right-moving top is larger; incoming explodes.
          alive = false;
        }
      }

      if (alive) {
        stack.push_back(asteroid);
      }
    }

    return stack;
  }
};

LC_PROBLEM(735, "Asteroid Collision", "stack", ctx) {
  ctx.test("example_1_right_survives", [&] {
    Solution sol;
    ctx.expect_eq(sol.asteroidCollision({5, 10, -5}),
                  std::vector<int>{5, 10});
  });

  ctx.test("example_2_mutual_annihilation", [&] {
    Solution sol;
    ctx.expect_eq(sol.asteroidCollision({8, -8}), std::vector<int>{});
  });

  ctx.test("example_3_chain_collision", [&] {
    Solution sol;
    ctx.expect_eq(sol.asteroidCollision({10, 2, -5}), std::vector<int>{10});
  });

  ctx.test("example_4_diverging_no_collision", [&] {
    Solution sol;
    ctx.expect_eq(sol.asteroidCollision({-2, -1, 1, 2}),
                  std::vector<int>{-2, -1, 1, 2});
  });

  ctx.test("chain_destroys_all_right_movers", [&] {
    Solution sol;
    ctx.expect_eq(sol.asteroidCollision({1, 2, 3, -10}), std::vector<int>{-10});
  });

  ctx.test("chain_ends_in_equal_cancellation", [&] {
    Solution sol;
    ctx.expect_eq(sol.asteroidCollision({1, 5, 2, -5}), std::vector<int>{1});
  });
}

}  // namespace
