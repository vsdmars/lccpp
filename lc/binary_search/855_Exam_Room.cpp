// LeetCode 855: Exam Room
// https://leetcode.com/problems/exam-room/
//
// Problem:
//   An exam room has `n` seats in a single row numbered `0, 1, ..., n - 1`.
//   Design an `ExamRoom(int n)` class supporting:
//     - `seat()`: places a student in the seat that maximizes the distance to
//       the closest already-seated student. Ties are broken in favor of the
//       lowest seat index. If the room is empty, the student sits at seat `0`.
//     - `leave(int p)`: removes the student currently sitting in seat `p`.
//   Constraints: 1 <= n <= 10^9; at most 10^4 calls to `seat()` and `leave()`.
//
// Algorithm:
//   Maintain a sorted list `seats_` of all currently occupied seat indices.
//   - `seat()`:
//       1. If `seats_` is empty, seat the student at `0`.
//       2. Otherwise evaluate three candidate categories in increasing index
//          order, updating the best seat only on strict improvement (`>`),
//          which automatically breaks ties in favor of the lowest seat number:
//            a) Left boundary seat `0`, with distance `seats_.front()`.
//            b) Midpoint `a + (b - a) / 2` of every consecutive occupied pair
//               `(a, b)`, with distance `(b - a) / 2`.
//            c) Right boundary seat `n - 1`, with distance
//               `(n - 1) - seats_.back()`.
//       3. Insert the chosen seat into `seats_` in sorted order via
//          `std::ranges::lower_bound`.
//   - `leave(p)`:
//       Locate `p` in `seats_` using `std::ranges::lower_bound` and erase it.
//   Time Complexity: O(P) per `seat()` and O(P) per `leave()` (O(log P) search
//   + O(P) vector shift), where `P` is the number of currently seated students
//   (at most 10^4).
//   Space Complexity: O(P).
//
// Data Structures:
//   - Sorted `std::vector<int>` (`seats_`) storing occupied seat indices,
//     supporting cache-friendly linear gap scans and binary-searched
//     insertions/erasures.
//
// Real-World Applications:
//   - Physical-distancing venue/theater seat assignment and conference-room
//     scheduling.
//   - Spatially spread placement of replicas, sensors, or cache shards along a
//     1D hash ring or linear topology to maximize fault isolation.
//   - Memory/address-space guard-zone placement maximizing separation between
//     active allocations.
//
// Related LeetCode Problems:
//   - #849 Maximize Distance to Closest Person [new] (static single-seat version of Exam Room)
//   - #715 Range Module [done] (maintaining sorted dynamic intervals via binary search)
//   - #774 Minimize Max Distance To Gas Station [done] (minimax gap optimization on a 1D line)
//   - #1552 Magnetic Force Between Two Balls [done] (placing k items to maximize minimum spacing)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class ExamRoom {
 public:
  // Creates an empty exam room with seats numbered `0 .. n - 1`.
  explicit ExamRoom(int n) : n_(n) {}

  // Assigns and returns the optimal seat maximizing distance to the closest
  // occupied seat (lowest index on ties).
  // Time: O(P), Space: O(1) extra (where P = currently seated students).
  int seat() {
    int chosen = 0;

    if (!seats_.empty()) {
      // Candidate 1: left edge seat `0` (distance equals the first occupied
      // seat index; handles the case where seat 0 was vacated earlier).
      int best_dist = seats_.front();

      // Candidate 2: interior midpoints between consecutive occupied seats.
      // Strict `>` preserves lowest-index tie-breaking because pairs are
      // visited left-to-right.
      for (std::size_t i = 1uz; i < seats_.size(); ++i) {
        const int left_seat = seats_[i - 1uz];
        const int right_seat = seats_[i];
        const int dist = (right_seat - left_seat) / 2;
        if (dist > best_dist) {
          best_dist = dist;
          chosen = left_seat + dist;
        }
      }

      // Candidate 3: right edge seat `n_ - 1` (distance to the rightmost
      // occupied seat).
      const int right_edge_dist = (n_ - 1) - seats_.back();
      if (right_edge_dist > best_dist) {
        chosen = n_ - 1;
      }
    }

    // Insert `chosen` while keeping `seats_` strictly ascending.
    const auto pos = std::ranges::lower_bound(seats_, chosen);
    seats_.insert(pos, chosen);
    return chosen;
  }

  // Vacates seat `p` (guaranteed to be occupied per problem constraints).
  // Time: O(P) (O(log P) search + O(P) erase), Space: O(1).
  void leave(int p) {
    const auto pos = std::ranges::lower_bound(seats_, p);
    if (pos != seats_.end() && *pos == p) {
      seats_.erase(pos);
    }
  }

 private:
  int n_ = 0;
  std::vector<int> seats_;
};

LC_PROBLEM(855, "Exam Room", "binary_search", ctx) {
  ctx.test("example_1", [&] {
    ExamRoom room(10);
    ctx.expect_eq(room.seat(), 0);
    ctx.expect_eq(room.seat(), 9);
    ctx.expect_eq(room.seat(), 4);
    ctx.expect_eq(room.seat(), 2);
    room.leave(4);
    ctx.expect_eq(room.seat(), 5);
  });

  ctx.test("python_main_n4_sequence", [&] {
    ExamRoom room(4);
    ctx.expect_eq(room.seat(), 0);
    ctx.expect_eq(room.seat(), 3);
    ctx.expect_eq(room.seat(), 1);
    ctx.expect_eq(room.seat(), 2);
    room.leave(1);
    room.leave(3);
    // Occupied: [0, 2]. Seat 1 has distance 1; seat 3 has distance 1.
    // Tie breaks to the lower index: seat 1.
    ctx.expect_eq(room.seat(), 1);
    ctx.expect_eq(room.seat(), 3);
  });

  ctx.test("vacate_and_reclaim_boundary_seats", [&] {
    ExamRoom room(10);
    ctx.expect_eq(room.seat(), 0);
    ctx.expect_eq(room.seat(), 9);
    ctx.expect_eq(room.seat(), 4);
    room.leave(0);
    // Occupied: [4, 9]. Seat 0 has distance 4; midpoint (4, 9) has distance 2.
    ctx.expect_eq(room.seat(), 0);
    room.leave(9);
    // Occupied: [0, 4]. Seat 9 has distance 5; midpoint (0, 4) has distance 2.
    ctx.expect_eq(room.seat(), 9);
  });

  ctx.test("single_seat_and_large_n", [&] {
    ExamRoom tiny(1);
    ctx.expect_eq(tiny.seat(), 0);
    tiny.leave(0);
    ctx.expect_eq(tiny.seat(), 0);

    ExamRoom huge(1000000000);
    ctx.expect_eq(huge.seat(), 0);
    ctx.expect_eq(huge.seat(), 999999999);
    ctx.expect_eq(huge.seat(), 499999999);
  });
}

}  // namespace
