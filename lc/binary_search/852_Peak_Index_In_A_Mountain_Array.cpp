// LeetCode 852: Peak Index In A Mountain Array
// https://leetcode.com/problems/peak-index-in-a-mountain-array/
//
// Problem:
//   An array `arr` is a mountain if `arr.length >= 3` and there exists a unique
//   index `i` with `0 < i < arr.length - 1` such that:
//     `arr[0] < arr[1] < ... < arr[i - 1] < arr[i] > arr[i + 1] > ... > arr[n - 1]`.
//   Given a guaranteed mountain array `arr`, return the peak index `i`.
//   Constraints: 3 <= arr.length <= 10^5, 0 <= arr[i] <= 10^6.
//
// Examples:
//   Example 1:
//     Input:  arr = [0, 1, 0]
//     Output: 1
//     Explanation:
//       - Initialize closed bounds lhs = 0 and rhs = 2 since arr.size() is 3.
//       - On iteration 1, mid = 0 + (2 - 0) / 2 = 1; arr[1] = 1 < arr[2] = 0 is
//         false, so set rhs = 1.
//       - On iteration 2, mid = 0 + (1 - 0) / 2 = 0; arr[0] = 0 < arr[1] = 1 is
//         true, so set lhs = 1.
//       - Loop terminates with lhs == rhs == 1 and returns peak index 1.
//
//   Example 2:
//     Input:  arr = [1, 3, 5, 7, 9, 12, 4]
//     Output: 5
//     Explanation:
//       - Initialize closed bounds lhs = 0 and rhs = 6.
//       - On iteration 1, mid = 3 has arr[3] = 7 < arr[4] = 9, so move left bound
//         to lhs = 4.
//       - On iteration 2, mid = 5 has arr[5] = 12 < arr[6] = 4 false, so tighten
//         right bound to rhs = 5.
//       - On iteration 3, mid = 4 has arr[4] = 9 < arr[5] = 12, advancing lhs to 5
//         so lhs == rhs == 5 is returned.
//
//   Example 3:
//     Input:  arr = [1, 100, 90, 80, 70, 60, 50, 10]
//     Output: 1
//     Explanation:
//       - Initialize closed bounds lhs = 0 and rhs = 7.
//       - On iteration 1, mid = 3 has arr[3] = 80 < arr[4] = 70 false, shrinking
//         the interval to [0, 3].
//       - On iteration 2, mid = 1 has arr[1] = 100 < arr[2] = 90 false, shrinking
//         the interval to [0, 1].
//       - On iteration 3, mid = 0 has arr[0] = 1 < arr[1] = 100 true, setting lhs =
//         1 and returning 1.
//
// Algorithm:
//   The adjacent slope predicate `arr[j] > arr[j + 1]` is false along the
//   strictly ascending prefix `[0, peak - 1]` and true along the descending
//   suffix `[peak, n - 2]`. Binary-search for the first index where the slope
//   turns downward over `[lhs, rhs]` initialized to `[0, n - 1]`:
//     - Compute `mid = lhs + ((rhs - lhs) / 2)`. Because `lhs < rhs`, `mid + 1`
//       is always a valid index.
//     - If `arr[mid] < arr[mid + 1]`, we are still on the ascending side, so
//       the peak lies strictly to the right: `lhs = mid + 1`.
//     - Otherwise `arr[mid] > arr[mid + 1]`, so `mid` is at or past the peak:
//       `rhs = mid`.
//   When `lhs == rhs`, `lhs` is the unique peak index.
//   Time Complexity: O(log n).
//   Space Complexity: O(1).
//   (The Python source's O(n) linear scan is also included for comparison.)
//
// Data Structures:
//   - Read-only `std::vector<int>` accessed with two `std::size_t` search
//     bounds (`lhs`, `rhs`).
//
// Real-World Applications:
//   - Autofocus lens positioning in cameras (locating the sharpness peak of a
//     unimodal focus-contrast curve).
//   - Finding the maximum-throughput operating point (knee/peak) of a unimodal
//     system benchmark or RF signal-strength sweep.
//   - Discrete unimodal function optimization in numerical computing.
//
// Related LeetCode Problems:
//   - #162 Find Peak Element [done] (finding any local peak in a general array)
//   - #704 Binary Search [done] (standard monotonic binary search)
//   - #1095 Find in Mountain Array [new] (find peak first, then binary search both slopes)
//   - #153 Find Minimum in Rotated Sorted Array [done] (binary search using adjacent/boundary comparisons)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the unique peak index in a mountain array using O(log n) binary
  // search on the adjacent slope.
  // Time: O(log n), Space: O(1).
  int peakIndexInMountainArray(const std::vector<int>& arr) {
    if (arr.size() < 3uz) {
      return -1;
    }

    // Closed search interval [lhs, rhs] always contains the unique peak index.
    std::size_t lhs = 0uz;
    std::size_t rhs = arr.size() - 1uz;

    while (lhs < rhs) {
      // Since lhs < rhs, mid < rhs <= arr.size() - 1, so mid + 1 is in bounds.
      const std::size_t mid = lhs + ((rhs - lhs) / 2uz);
      if (arr[mid] < arr[mid + 1uz]) {
        // Ascending slope: peak must lie strictly to the right of `mid`.
        lhs = mid + 1uz;
      } else {
        // Descending slope (or at the peak): peak lies in `[lhs, mid]`.
        rhs = mid;
      }
    }

    return static_cast<int>(lhs);
  }

  // Linear-scan baseline from the Python source: returns the first index where
  // the sequence stops ascending.
  // Time: O(n), Space: O(1).
  int peakIndexLinear(const std::vector<int>& arr) {
    for (std::size_t i = 1uz; i < arr.size(); ++i) {
      if (arr[i] < arr[i - 1uz]) {
        return static_cast<int>(i - 1uz);
      }
    }
    return -1;
  }
};

LC_PROBLEM(852, "Peak Index In A Mountain Array", "binary_search", ctx) {
  auto verify_both = [&](const std::vector<int>& arr, int expected) {
    Solution sol;
    ctx.expect_eq(sol.peakIndexInMountainArray(arr), expected);
    ctx.expect_eq(sol.peakIndexLinear(arr), expected);
  };

  ctx.test("example_1_minimal_mountain", [&] {
    verify_both({0, 1, 0}, 1);
  });

  ctx.test("example_2_short_asymmetric", [&] {
    verify_both({0, 2, 1, 0}, 1);
  });

  ctx.test("example_3_steep_drop", [&] {
    verify_both({0, 10, 5, 2}, 1);
  });

  ctx.test("peak_near_right_end", [&] {
    verify_both({1, 3, 5, 7, 9, 12, 4}, 5);
  });

  ctx.test("peak_near_left_end", [&] {
    verify_both({1, 100, 90, 80, 70, 60, 50, 10}, 1);
  });

  ctx.test("symmetric_long_mountain", [&] {
    verify_both({0, 2, 4, 8, 16, 32, 16, 8, 4, 2, 0}, 5);
  });
}

}  // namespace
