// LeetCode 1386: Cinema Seat Allocation
// https://leetcode.com/problems/cinema-seat-allocation/
//
// Problem:
//   A cinema has `n` rows numbered `1..n`, each with 10 seats labeled `1..10`
//   and aisles between seats 3-4 and 7-8: `[1 2 3] | [4 5 6 7] | [8 9 10]`.
//   Given `reservedSeats` where each entry `[row, seat]` marks an already
//   occupied seat, return the maximum number of four-person groups that can be
//   seated. Each group must occupy 4 contiguous seats in the same row, and may
//   only cross an aisle if split evenly (2 seats on each side).
//   Constraints: 1 <= n <= 10^9, 1 <= reservedSeats.length <= min(10*n, 10^4).
//
// Algorithm:
//   Observe that there are only three valid 4-seat blocks per row:
//     - Left block:   seats {2, 3, 4, 5}
//     - Middle block: seats {4, 5, 6, 7}
//     - Right block:  seats {6, 7, 8, 9}
//   Seats 1 and 10 never affect any valid block. Every row with no reservations
//   in seats 2..9 fits 2 groups (Left + Right), giving a baseline of `2 * n`.
//   Because `n` can be up to 10^9, iterating over all rows would TLE; instead,
//   process only rows that appear in `reservedSeats` with a seat in 2..9.
//   For each such row, record a 3-bit bitmask of blocked regions:
//     - Any reservation in 2..9 blocks at least Left or Right (note that every
//       seat in Middle {4, 5, 6, 7} also belongs to Left or Right), so an
//       affected row can hold at most 1 group instead of 2.
//     - If all 3 bits are set, the row holds 0 groups (subtract 2 from `2 * n`).
//     - Otherwise at least one valid block remains, so the row holds 1 group
//       (subtract 1 from `2 * n`).
//   Time Complexity:  O(m) average, where m = reservedSeats.size().
//   Space Complexity: O(min(n, m)) for the map of affected rows.
//
// Data Structures:
//   - Hash map (`std::unordered_map<int, std::uint32_t>`): maps each affected
//     row index to a 3-bit bitmask representing which of the Left, Middle, and
//     Right 4-seat blocks are obstructed.
//
// Real-World Applications:
//   - Ticketing and reservation engines (airlines, cinemas, stadiums): fast
//     sparse availability calculations across huge venue inventories without
//     materializing empty rows.
//   - Sparse memory/slot allocators: tracking blocked contiguous alignment
//     windows using compact bitmasks for non-empty pages only.
//   - Resource scheduling: counting packable fixed-size contiguous time blocks
//     around sparse pre-booked reservations.
//
// Related LeetCode Problems:
//   - #2286 Booking Concert Tickets in Groups [new] (group seat allocation across cinema rows)
//   - #849 Maximize Distance to Closest Person [new] (1D seating placement optimization)
//   - #855 Exam Room [done] (dynamic seat assignment with spacing constraints)
//   - #605 Can Place Flowers [done] (greedy non-overlapping slot placement in a 1D row)

#include "framework/lc_framework.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace {

constexpr std::uint32_t kLeftBlock = 1U << 0U;    // Seats 2, 3, 4, 5
constexpr std::uint32_t kMiddleBlock = 1U << 1U;  // Seats 4, 5, 6, 7
constexpr std::uint32_t kRightBlock = 1U << 2U;   // Seats 6, 7, 8, 9
constexpr std::uint32_t kAllBlocks = kLeftBlock | kMiddleBlock | kRightBlock;

class Solution {
 public:
  // Returns the maximum number of 4-person families that can be seated across
  // `n` rows given `reservedSeats`.
  // Time: O(m) average, Space: O(min(n, m)), where m = reservedSeats.size().
  int maxNumberOfFamilies(int n,
                          const std::vector<std::vector<int>>& reservedSeats) {
    // Only rows with at least one reservation in seats 2..9 need tracking;
    // untouched rows (or rows with only seats 1/10 reserved) always fit 2.
    std::unordered_map<int, std::uint32_t> blocked_masks;
    blocked_masks.reserve(reservedSeats.size());

    for (const auto& entry : reservedSeats) {
      const int row = entry[0];
      const int seat = entry[1];

      // Seats 4..5 overlap Left & Middle; seats 6..7 overlap Middle & Right.
      // Seats 1 and 10 affect no valid block and are ignored so they do not
      // insert an entry into `blocked_masks`.
      std::uint32_t mask = 0;
      if (seat >= 2 && seat <= 5) {
        mask |= kLeftBlock;
      }
      if (seat >= 4 && seat <= 7) {
        mask |= kMiddleBlock;
      }
      if (seat >= 6 && seat <= 9) {
        mask |= kRightBlock;
      }

      if (mask != 0) {
        blocked_masks[row] |= mask;
      }
    }

    // Start from the unreserved upper bound of 2 families per row (fits in
    // signed 32-bit int since n <= 10^9 => 2 * n <= 2 * 10^9 < INT_MAX).
    int max_families = 2 * n;
    for (const auto& entry : blocked_masks) {
      // Every entry in `blocked_masks` blocks at least Left or Right, reducing
      // that row's capacity from 2 down to either 0 (all 3 blocks blocked) or
      // 1 (at least one block still open).
      if (entry.second == kAllBlocks) {
        max_families -= 2;
      } else {
        max_families -= 1;
      }
    }

    return max_families;
  }
};

LC_PROBLEM(1386, "Cinema Seat Allocation", "math", ctx) {
  Solution sol;

  ctx.test("example_1_mixed_rows", [&] {
    const std::vector<std::vector<int>> reserved = {
        {1, 2}, {1, 3}, {1, 8}, {2, 6}, {3, 1}, {3, 10}};
    ctx.expect_eq(sol.maxNumberOfFamilies(3, reserved), 4);
  });

  ctx.test("example_2_two_rows", [&] {
    const std::vector<std::vector<int>> reserved = {{2, 1}, {1, 8}, {2, 6}};
    ctx.expect_eq(sol.maxNumberOfFamilies(2, reserved), 2);
  });

  ctx.test("example_3_overlapping_middle_blocks", [&] {
    const std::vector<std::vector<int>> reserved = {
        {4, 3}, {1, 4}, {4, 6}, {1, 7}};
    ctx.expect_eq(sol.maxNumberOfFamilies(4, reserved), 4);
  });

  ctx.test("only_middle_available_and_fully_blocked", [&] {
    // Seats 2 and 9 block Left and Right, leaving Middle {4, 5, 6, 7} open.
    ctx.expect_eq(sol.maxNumberOfFamilies(1, {{1, 2}, {1, 9}}), 1);
    // Seats 4 and 6 block all three blocks -> 0 families.
    ctx.expect_eq(sol.maxNumberOfFamilies(1, {{1, 4}, {1, 6}}), 0);
  });

  ctx.test("sparse_billion_rows_edge_seats_ignored", [&] {
    // Seats 1 and 10 do not obstruct any 4-seat block; handles n = 10^9 in O(m).
    const std::vector<std::vector<int>> reserved = {{500000000, 1},
                                                    {500000000, 10}};
    ctx.expect_eq(sol.maxNumberOfFamilies(1000000000, reserved), 2000000000);
    ctx.expect_eq(sol.maxNumberOfFamilies(5, {}), 10);
  });
}

}  // namespace
