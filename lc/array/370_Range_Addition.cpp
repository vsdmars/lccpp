// LeetCode 370: Range Addition
// https://leetcode.com/problems/range-addition/
//
// Problem:
//   You are given an integer `length` representing an array initially filled
//   with `length` zeros, and a list of `k` range update operations
//   `updates[j] = [start_idx, end_idx, inc]`. Each operation adds `inc` to
//   every element in the inclusive subarray `[start_idx, end_idx]`. Return the
//   final array after applying all `k` updates.
//
// Algorithm:
//   Difference Array + Prefix Sum Reconstruction:
//   Instead of updating every element in `[start_idx, end_idx]` (which would
//   take O(k * n) time), record only the slope changes (boundary deltas) in the
//   output array `diff`:
//   1. For each `[start, end, inc]`:
//      - `diff[start] += inc` (the increment takes effect starting at `start`).
//      - If `end + 1 < length`, `diff[end + 1] -= inc` (the increment stops
//        immediately after `end`).
//   2. Compute the running prefix sum of `diff` in-place from left to right.
//      At each index `i`, the running sum equals the total `inc` of all
//      intervals covering `i`.
//   Time Complexity:  O(k + n) — O(1) per update plus one O(n) prefix pass.
//   Space Complexity: O(1) auxiliary space beyond the O(n) returned array.
//
// Data Structures:
//   - `std::vector<int>` (`result`): acts first as the 1D difference array of
//     interval boundary deltas, then becomes the reconstructed output array in
//     the prefix-sum sweep.
//
// Real-World Applications:
//   - Resource reservation and capacity planning (computing concurrent hotel
//     bookings, flight seat allocations, or cloud VM reservations across time
//     intervals in linear time).
//   - Network bandwidth / traffic burst aggregation over fixed time buckets.
//   - Financial ledger and payroll batch adjustments applied across contiguous
//     accounting periods.
//
// Related LeetCode Problems:
//   - #1109 Corporate Flight Bookings [new] (identical difference-array formulation with 1-indexed ranges)
//   - #1094 Car Pooling [done] (sweep-line / difference array for overlapping capacity intervals)
//   - #253 Meeting Rooms II [done] (interval boundary events for peak overlap)
//   - #2536 Increment Submatrices by One [new] (2D difference-array generalization)
//   - #598 Range Addition II [new] (2D prefix-rectangle intersection variant)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Applies all inclusive range increments `[start, end, inc]` to an initial
  // zero-filled array of size `length` and returns the resulting array.
  //
  // Invariant:
  //   - After Pass 1: `result[i]` stores the net change entering index `i`
  //     relative to index `i - 1`.
  //   - During Pass 2: `running_sum` equals the sum of `inc` for all updates
  //     whose interval `[start, end]` contains the current index.
  //
  // Time Complexity:  O(k + n), where k = updates.size() and n = length
  // Space Complexity: O(1) extra space (excluding the returned array)
  std::vector<int> getModifiedArray(
      int length, const std::vector<std::vector<int>>& updates) {
    if (length <= 0) {
      return {};
    }

    const auto n = static_cast<std::size_t>(length);
    std::vector<int> result(n, 0);

    // Pass 1: Record O(1) boundary deltas in the difference array.
    for (const auto& update : updates) {
      const auto start = static_cast<std::size_t>(update[0]);
      const auto end = static_cast<std::size_t>(update[1]);
      const int inc = update[2];

      result[start] += inc;
      if (end + 1uz < n) {
        result[end + 1uz] -= inc;
      }
    }

    // Pass 2: Reconstruct actual values via an in-place prefix-sum sweep.
    int running_sum = 0;
    for (int& val : result) {
      running_sum += val;
      val = running_sum;
    }

    return result;
  }
};

LC_PROBLEM(370, "Range Addition", "array", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    const std::vector<std::vector<int>> updates = {
        {1, 3, 2},
        {2, 4, 3},
        {0, 2, -2},
    };
    ctx.expect_eq(sol.getModifiedArray(5, updates),
                  std::vector<int>{-2, 0, 3, 5, 3});
  });

  ctx.test("example_2_larger_range", [&] {
    const std::vector<std::vector<int>> updates = {
        {2, 4, 6},
        {5, 6, 8},
        {1, 9, -4},
    };
    ctx.expect_eq(sol.getModifiedArray(10, updates),
                  std::vector<int>{0, -4, 2, 2, 2, 4, 4, -4, -4, -4});
  });

  ctx.test("no_updates", [&] {
    ctx.expect_eq(sol.getModifiedArray(4, {}), std::vector<int>{0, 0, 0, 0});
  });

  ctx.test("single_element_and_canceling_updates", [&] {
    const std::vector<std::vector<int>> updates = {
        {0, 0, 5},
        {0, 0, -5},
        {0, 0, 3},
    };
    ctx.expect_eq(sol.getModifiedArray(1, updates), std::vector<int>{3});
  });

  ctx.test("overlapping_full_range_updates", [&] {
    const std::vector<std::vector<int>> updates = {
        {0, 3, 1},
        {0, 3, 2},
        {1, 2, -3},
    };
    ctx.expect_eq(sol.getModifiedArray(4, updates),
                  std::vector<int>{3, 0, 0, 3});
  });
}

}  // namespace
