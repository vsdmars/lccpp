// LeetCode 941: Valid Mountain Array
// https://leetcode.com/problems/valid-mountain-array/
//
// Problem:
//   Given an integer array `arr`, return true if and only if it is a valid
//   mountain array. An array is a valid mountain array when:
//     1. arr.length >= 3.
//     2. There exists an index i with 0 < i < arr.length - 1 such that:
//        arr[0] < arr[1] < ... < arr[i - 1] < arr[i] and
//        arr[i] > arr[i + 1] > ... > arr[arr.length - 1].
//   Constraints: 1 <= arr.length <= 10^4, 0 <= arr[i] <= 10^4.
//
// Examples:
//   Example 1:
//     Input:  arr = [0, 3, 2, 1]
//     Output: true
//     Explanation:
//       - Check `n = 4 >= 3` and start the cursor at `i = 0`.
//       - In phase 1, `arr[0] = 0 < arr[1] = 3` advances `i` to `1`, and `arr[1] =
//         3 < arr[2] = 2` is false so the climb stops at peak `i = 1`.
//       - Verify `i = 1` is strictly inside `(0, 3)` (`i != 0 && i != n - 1`).
//       - In phase 2, `arr[1] = 3 > arr[2] = 2` and `arr[2] = 2 > arr[3] = 1`
//         advance `i` to `2` and then `3`.
//       - Because `i == n - 1` (`3 == 3`), return `true`.
//
//   Example 2:
//     Input:  arr = [0, 2, 3, 3, 5, 2, 1, 0]
//     Output: false
//     Explanation:
//       - Check `n = 8 >= 3` and start at `i = 0`.
//       - In phase 1, `arr[0] = 0 < arr[1] = 2 < arr[2] = 3` advances `i` to `2`,
//         where the plateau `arr[2] == arr[3] == 3` halts the upward walk.
//       - Confirm `i = 2` is neither `0` nor `n - 1 = 7`, then begin phase 2.
//       - Because `arr[2] > arr[3]` (`3 > 3`) is false, `i` remains `2`, so `i ==
//         7` evaluates to `false`.
//
//   Example 3:
//     Input:  arr = [0, 2, 3, 1, 2, 0]
//     Output: false
//     Explanation:
//       - Check `n = 6 >= 3` and walk upward from `i = 0` across `0 < 2 < 3` to
//         peak `i = 2`.
//       - Verify `i = 2` is neither `0` nor `5`, then start phase 2.
//       - Advance `i` from `2` to `3` since `arr[2] = 3 > arr[3] = 1`, then stop
//         because `arr[3] = 1 < arr[4] = 2` breaks strict decrease.
//       - Since `i = 3 != n - 1 = 5`, return `false`.
//
// Algorithm:
//   Single-Pass Two-Phase Walk:
//   1. If `arr.size() < 3`, no interior peak can exist, so return false.
//   2. Walk pointer `i` forward while the sequence is strictly increasing
//      (`arr[i] < arr[i + 1]`).
//   3. Verify that `i` moved past index 0 and stopped before the last index
//      (`i != 0 && i != n - 1`), ensuring both a non-empty climb and a
//      non-empty descent exist.
//   4. Continue walking `i` forward while the sequence is strictly decreasing
//      (`arr[i] > arr[i + 1]`).
//   5. The array is a valid mountain iff `i` reaches the final index `n - 1`.
//      Any plateau (`arr[i] == arr[i + 1]`) or second climb stops `i` early.
//   Time Complexity:  O(n) — each index is visited at most once.
//   Space Complexity: O(1) — only a single index variable is maintained.
//
// Data Structures:
//   - Single cursor index (`std::size_t`) scanning the input `std::vector<int>`
//     in place with no auxiliary containers.
//
// Real-World Applications:
//   - Signal processing and unimodal waveform validation (confirming a sensor
//     pulse or chromatographic peak has a single clean rise and fall).
//   - Numerical optimization preconditions (verifying unimodality before
//     applying golden-section or ternary search on discrete curves).
//   - Audio envelope and physical motion profile checks (acceleration phase
//     followed by deceleration phase without jitter or flat plateaus).
//
// Related LeetCode Problems:
//   - #852 Peak Index in a Mountain Array [done] (binary search for the peak)
//   - #845 Longest Mountain in Array [done] (longest valid mountain subarray)
//   - #896 Monotonic Array [done] (single-direction monotonic scan)
//   - #1095 Find in Mountain Array [new] (binary search on ascending/descending halves)
//   - #1671 Minimum Number of Removals to Make Mountain Array [new] (LIS + LDS)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Determines whether `arr` strictly increases to a single interior peak and
  // then strictly decreases to the end.
  // Time: O(n), Space: O(1).
  bool validMountainArray(const std::vector<int>& arr) {
    const std::size_t n = arr.size();
    // A mountain needs at least one element on each side of the peak.
    if (n < 3uz) {
      return false;
    }

    std::size_t i = 0uz;

    // Phase 1: walk strictly upward to the peak.
    while ((i + 1uz) < n && arr[i] < arr[i + 1uz]) {
      ++i;
    }

    // The peak cannot be the first element (no climb) or the last (no descent).
    if (i == 0uz || i == (n - 1uz)) {
      return false;
    }

    // Phase 2: walk strictly downward from the peak.
    while ((i + 1uz) < n && arr[i] > arr[i + 1uz]) {
      ++i;
    }

    // Valid iff the strict descent consumed every remaining element.
    return i == (n - 1uz);
  }
};

LC_PROBLEM(941, "Valid Mountain Array", "array", ctx) {
  Solution sol;

  ctx.test("example_valid_mountain", [&] {
    ctx.expect_true(sol.validMountainArray({0, 3, 2, 1}));
  });

  ctx.test("too_short", [&] {
    ctx.expect_false(sol.validMountainArray({}));
    ctx.expect_false(sol.validMountainArray({1}));
    ctx.expect_false(sol.validMountainArray({2, 1}));
  });

  ctx.test("plateau_not_allowed", [&] {
    ctx.expect_false(sol.validMountainArray({3, 5, 5}));
    ctx.expect_false(sol.validMountainArray({0, 2, 3, 3, 5, 2, 1, 0}));
  });

  ctx.test("strictly_monotone_invalid", [&] {
    ctx.expect_false(sol.validMountainArray({1, 2, 3, 4}));
    ctx.expect_false(sol.validMountainArray({4, 3, 2, 1}));
  });

  ctx.test("double_peak_invalid", [&] {
    ctx.expect_false(sol.validMountainArray({0, 2, 3, 1, 2, 0}));
  });

  ctx.test("minimal_valid_mountain", [&] {
    ctx.expect_true(sol.validMountainArray({1, 3, 2}));
  });
}

}  // namespace
