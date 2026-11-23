// LeetCode 1654: Minimum Jumps To Reach Home
// https://leetcode.com/problems/minimum-jumps-to-reach-home/
//
// Problem:
//   A bug starts at position 0 on the non-negative integer line and wants to
//   reach its home at position `x`. On each move, it may jump:
//     - forward by exactly `+a` positions, or
//     - backward by exactly `-b` positions, provided it does NOT jump backward
//       twice in a row and never lands on a negative coordinate.
//   The bug may never land on any coordinate listed in `forbidden`, though it
//   is allowed to jump beyond `x` and later return. Return the minimum number
//   of jumps needed to reach `x`, or `-1` if `x` is unreachable.
//   Constraints: 1 <= forbidden.length <= 1000; 1 <= a, b, forbidden[i] <= 2000;
//   0 <= x <= 2000; all forbidden positions are distinct and `x` is not forbidden.
//
// Algorithm:
//   State-space Breadth-First Search (BFS) on an unweighted implicit graph:
//     - Each node in the state graph is `(pos, last_was_backward)`, because
//       arriving at `pos` via a forward jump strictly dominates arriving via a
//       backward jump (forward arrival unlocks a subsequent backward jump,
//       whereas backward arrival does not). Tracking visited states separately
//       for both arrival directions prevents a suboptimal backward arrival from
//       blocking a necessary forward arrival at the same coordinate.
//     - Search Upper Bound Proof: Let `M = max(x, max(forbidden))`. We never
//       need to visit any coordinate greater than `M + a + b`:
//         * If `a >= b`, once `pos > x + b`, even a backward jump stays above
//           `x` and the forced next forward jump (`+a >= +b`) cannot make net
//           downward progress, so `x + b <= M + a + b` suffices.
//         * If `a < b`, suppose a shortest path reaches some `p > M + b` (at
//           most `M + b + a` on its first excursion above `M + b`) via a forward
//           jump and later takes a backward jump `-b` after `k >= 1` more
//           forward jumps. Every intermediate position `q >= p` satisfies
//           `q - b > M >= max(forbidden)`, so shifting that `-b` earlier to `p`
//           yields a valid path of the same length with strictly smaller peak
//           coordinate and a forward final arrival. By induction, an optimal
//           path always stays within `[0, M + a + b]`.
//   Time Complexity: O(U), where `U = max(x, max(forbidden)) + a + b` (at most
//   ~6000 under problem constraints), since each of the `2 * (U + 1)` states is
//   enqueued and expanded at most once.
//   Space Complexity: O(U) for the 2-state visited table and BFS queue.
//
// Data Structures:
//   - `std::vector<std::array<bool, 2>>` visited table indexed by
//     `[pos][last_was_backward]` (pre-marking forbidden coordinates in both
//     slots for O(1) collision checks without hashing).
//   - `std::queue<JumpState>` FIFO queue guaranteeing level-order BFS so the
//     first pop with `pos == x` yields the exact minimum jump count.
//
// Real-World Applications:
//   - Robotics and autonomous vehicle motion planning on discrete lattices with
//     kinematic constraints (e.g., no consecutive reverse maneuvers) and static
//     obstacles.
//   - Elevator / crane / shuttle scheduling along a 1D track with fixed step
//     increments, restricted zones, and direction-reversal rules.
//   - Protocol and state-machine reachability verification (model checking)
//     where transitions depend on the previous action and unsafe states must be
//     avoided.
//
// Related LeetCode Problems:
//   - #1306 Jump Game III [new] (1D BFS/DFS reachability with +/- arr[i] jumps)
//   - #1345 Jump Game IV [done] (unweighted BFS shortest jumps on a 1D array)
//   - #752 Open the Lock [done] (BFS shortest path avoiding forbidden deadend states)
//   - #45 Jump Game II [done] (minimum jumps to reach target position on 1D line)
//   - #1293 Shortest Path in a Grid with Obstacles Elimination [new] (BFS over augmented `(position, constraint_state)` graph)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <queue>
#include <vector>

namespace {

// Represents a single state in the BFS search graph.
struct JumpState {
  int pos = 0;
  bool last_was_backward = false;
  int jumps = 0;
};

class Solution {
 public:
  // Computes the minimum number of jumps from 0 to `x` using forward step `a`
  // and backward step `b` without two consecutive backward jumps or landing on
  // any coordinate in `forbidden`. Returns -1 if unreachable.
  //
  // Time Complexity: O(max(x, max(forbidden)) + a + b)
  // Space Complexity: O(max(x, max(forbidden)) + a + b)
  int minimumJumps(const std::vector<int>& forbidden, int a, int b, int x) {
    // Compute the proven search upper bound U = max(x, max(forbidden)) + a + b.
    // Guard against an empty `forbidden` list for general callers.
    const int max_forbidden =
        forbidden.empty() ? 0 : std::ranges::max(forbidden);
    const int upper_bound = std::max(x, max_forbidden) + a + b;
    const auto limit = static_cast<std::size_t>(upper_bound);

    // `visited[pos][0]` = reached `pos` via forward jump (or start); may jump
    // backward next.
    // `visited[pos][1]` = reached `pos` via backward jump; must jump forward
    // next.
    std::vector<std::array<bool, 2>> visited(limit + 1uz, {false, false});

    // Pre-mark all forbidden coordinates as blocked in both arrival states so
    // the BFS loop only needs a single bounds + visited check.
    for (const int f : forbidden) {
      if (f >= 0 && f <= upper_bound) {
        visited[static_cast<std::size_t>(f)] = {true, true};
      }
    }

    // Seed BFS at position 0 (0 jumps taken, eligible for a forward jump;
    // backward from 0 would be negative and rejected by the `>= 0` guard).
    std::queue<JumpState> bfs;
    visited[0][0] = true;
    bfs.push({.pos = 0, .last_was_backward = false, .jumps = 0});

    while (!bfs.empty()) {
      const JumpState cur = bfs.front();
      bfs.pop();

      // Standard unweighted BFS guarantees the first visit to `x` is minimal.
      if (cur.pos == x) {
        return cur.jumps;
      }

      // Transition 1: Forward jump (+a). Always allowed regardless of the
      // previous jump direction, and resets `last_was_backward` to false (0).
      const int next_fwd = cur.pos + a;
      if (next_fwd <= upper_bound &&
          !visited[static_cast<std::size_t>(next_fwd)][0]) {
        visited[static_cast<std::size_t>(next_fwd)][0] = true;
        bfs.push({
            .pos = next_fwd,
            .last_was_backward = false,
            .jumps = cur.jumps + 1,
        });
      }

      // Transition 2: Backward jump (-b). Only allowed if the previous jump was
      // NOT backward and the landing position stays non-negative.
      const int next_back = cur.pos - b;
      if (!cur.last_was_backward && next_back >= 0 &&
          !visited[static_cast<std::size_t>(next_back)][1]) {
        visited[static_cast<std::size_t>(next_back)][1] = true;
        bfs.push({
            .pos = next_back,
            .last_was_backward = true,
            .jumps = cur.jumps + 1,
        });
      }
    }

    return -1;
  }
};

LC_PROBLEM(1654, "Minimum Jumps To Reach Home", "tree", ctx) {
  ctx.test("example_1_forward_only", [&] {
    Solution sol;
    const std::vector<int> forbidden = {14, 4, 18, 1, 15};
    ctx.expect_eq(sol.minimumJumps(forbidden, 3, 15, 9), 3);
  });

  ctx.test("example_2_unreachable", [&] {
    Solution sol;
    const std::vector<int> forbidden = {8, 3, 16, 6, 12, 20};
    ctx.expect_eq(sol.minimumJumps(forbidden, 15, 13, 11), -1);
  });

  ctx.test("example_3_forward_then_backward", [&] {
    Solution sol;
    const std::vector<int> forbidden = {1, 6, 2, 14, 5, 17, 4};
    ctx.expect_eq(sol.minimumJumps(forbidden, 16, 9, 7), 2);
  });

  ctx.test("python_build_overshoots_max_forbidden", [&] {
    Solution sol;
    // Shortest path: 0 -> 61 -> 122 -> 183 -> 13 -> 74 -> 135 (6 jumps).
    // Position 183 exceeds both x (135) and max(forbidden) (178).
    const std::vector<int> forbidden = {128, 178, 147, 165, 63, 11,
                                        150, 20,  158, 144, 136};
    ctx.expect_eq(sol.minimumJumps(forbidden, 61, 170, 135), 6);
  });

  ctx.test("python_build_first_step_blocked", [&] {
    Solution sol;
    const std::vector<int> forbidden = {18, 13, 3, 9, 8, 14};
    ctx.expect_eq(sol.minimumJumps(forbidden, 3, 8, 6), -1);
  });

  ctx.test("already_at_home_zero", [&] {
    Solution sol;
    const std::vector<int> forbidden = {1, 2, 3};
    ctx.expect_eq(sol.minimumJumps(forbidden, 5, 3, 0), 0);
  });

  ctx.test("requires_distinct_forward_and_backward_visited_states", [&] {
    Solution sol;
    // Path 0 -> 5 -> 10 -> 7 visits 7 via a backward jump first, blocking any
    // backward jump from 7 if visited state is 1D. Path 0 -> 5 -> 2 -> 7 -> 4
    // visits 7 via a forward jump at the same depth and then jumps back to 4.
    const std::vector<int> forbidden = {12, 15};
    ctx.expect_eq(sol.minimumJumps(forbidden, 5, 3, 4), 4);
  });

  ctx.test("tight_upper_bound_f_plus_a_plus_b", [&] {
    Solution sol;
    // Must jump past f + b = 1998 + 2000 = 3998 to 5997 (= f + a + b), then
    // zigzag down (-2000, +1999) until landing on 2000.
    // Net change per (-b, +a) pair is -1; starting from 3998 -> 5997 -> 3997,
    // reaching 4000 -> 2000 is valid and finite.
    const std::vector<int> forbidden = {1998};
    const int jumps = sol.minimumJumps(forbidden, 1999, 2000, 2000);
    ctx.expect_eq(jumps, 3998);
  });
}

}  // namespace
