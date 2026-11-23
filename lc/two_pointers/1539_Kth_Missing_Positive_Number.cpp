// LeetCode 1539: Kth Missing Positive Number
// https://leetcode.com/problems/kth-missing-positive-number/
//
// Problem:
//   Given an array `arr` of positive integers sorted in strictly increasing
//   order and a positive integer `k`, return the `k`-th positive integer that
//   is missing from `arr`.
//   Constraints:
//     - 1 <= arr.length <= 1000
//     - 1 <= arr[i] <= 1000
//     - 1 <= k <= 1000
//     - arr[i] < arr[j] for 0 <= i < j < arr.length
//
// Examples:
//   Example 1:
//     Input:  arr = [2, 3, 4, 7, 11], k = 5
//     Output: 9
//     Explanation:
//       - Initialize left = 0 and right = 5; at mid = 2 (arr[2] = 4),
//         missing_before_mid = 4 - (2 + 1) = 1 < 5, so advance left = 3.
//       - At mid = 4 (arr[4] = 11), missing_before_mid = 11 - (4 + 1) = 6 >= 5, so
//         narrow right = 4.
//       - At mid = 3 (arr[3] = 7), missing_before_mid = 7 - (3 + 1) = 3 < 5, so set
//         left = 4; now left == right == 4, and returning left + k = 4 + 5 gives 9.
//
//   Example 2:
//     Input:  arr = [1, 2, 3, 4], k = 2
//     Output: 6
//     Explanation:
//       - Initialize left = 0 and right = 4; at mid = 2 (arr[2] = 3),
//         missing_before_mid = 3 - 3 = 0 < 2, so set left = 3.
//       - At mid = 3 (arr[3] = 4), missing_before_mid = 4 - 4 = 0 < 2, so set left
//         = 4.
//       - Now left == right == 4, indicating all 4 elements of arr precede the 2nd
//         missing positive integer, and returning left + k = 4 + 2 yields 6.
//
//   Example 3:
//     Input:  arr = [5, 6, 7], k = 4
//     Output: 4
//     Explanation:
//       - Initialize left = 0 and right = 3; at mid = 1 (arr[1] = 6),
//         missing_before_mid = 6 - 2 = 4 >= 4, so narrow right = 1.
//       - At mid = 0 (arr[0] = 5), missing_before_mid = 5 - 1 = 4 >= 4, so narrow
//         right = 0.
//       - With left == right == 0, no elements of arr are smaller than the answer,
//         so return left + k = 0 + 4 = 4.
//
// Algorithm:
//   Primary (Binary Search):
//     Because `arr` is strictly increasing and 1-based positive integers start
//     at 1, the exact number of positive integers missing strictly before
//     `arr[i]` is:
//       missing(i) = arr[i] - (i + 1).
//     Since `arr` is strictly increasing, `missing(i)` is monotonically
//     non-decreasing with `i`. Binary search for the smallest index `left` in
//     [0, n] such that `missing(left) >= k` (treating missing(n) = +infinity).
//     At termination, exactly `left` elements of `arr` are smaller than the
//     answer, and `k` positive integers are missing up to the answer, so the
//     answer is simply `left + k`.
//     Time Complexity:  O(log n), where n = arr.size().
//     Space Complexity: O(1).
//
//   Alternative (Two-Pointer Linear Scan):
//     Walk candidate positive integers 1, 2, 3, ... while tracking an index
//     pointer into `arr`. Whenever `candidate == arr[idx]`, advance `idx`;
//     otherwise decrement `k`. When `k` reaches 0, `candidate` is the answer.
//     Time Complexity:  O(n + k).
//     Space Complexity: O(1).
//
// Data Structures:
//   - Plain index pointers (`left`, `right`, `mid`) over the sorted
//     `std::vector<int>` with O(1) auxiliary storage.
//
// Real-World Applications:
//   - Distributed sequence-number gap detection (finding the k-th dropped
//     packet or missing log sequence ID in a sorted acknowledgment buffer).
//   - Database primary-key / ticket ID allocation over sorted tables of
//     already-occupied identifiers.
//   - Sparse array indexing and calendar slot availability lookup (finding the
//     k-th free day given a sorted list of booked days).
//
// Related LeetCode Problems:
//   - #1060 Missing Element in Sorted Array [done] (k-th missing number starting from arr[0] via binary search)
//   - #41 First Missing Positive [done] (find the 1st missing positive in an unsorted array)
//   - #268 Missing Number [done] (find the single missing number in [0, n])
//   - #448 Find All Numbers Disappeared in an Array [done] (enumerate all missing integers in [1, n])
//   - #2195 Append K Integers With Minimal Sum [new] (sum of the first k missing positive integers)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the k-th missing positive integer using binary search on the
  // monotonic prefix-missing count `arr[i] - (i + 1)`.
  //
  // Invariant:
  //   - For every index < `left`, missing(index) < k.
  //   - For every index >= `right`, missing(index) >= k.
  // Time: O(log n), Space: O(1).
  int findKthPositive(const std::vector<int>& arr, int k) {
    if (k <= 0) {
      return 0;
    }

    std::size_t left = 0uz;
    std::size_t right = arr.size();

    while (left < right) {
      const std::size_t mid = left + ((right - left) / 2uz);
      // Number of positive integers in [1, arr[mid]] not present in arr[0..mid].
      const int missing_before_mid = arr[mid] - static_cast<int>(mid + 1uz);

      if (missing_before_mid < k) {
        // The k-th missing integer lies strictly after arr[mid].
        left = mid + 1uz;
      } else {
        // At least k integers are missing before arr[mid]; search left half.
        right = mid;
      }
    }

    // Exactly `left` elements of `arr` precede the k-th missing positive
    // integer, so its value is `left + k`.
    return static_cast<int>(left) + k;
  }

  // Two-pointer linear simulation: advances a candidate counter alongside an
  // index into `arr`, counting down `k` whenever a candidate is absent.
  // Time: O(n + k), Space: O(1).
  int findKthPositiveTwoPointers(const std::vector<int>& arr, int k) {
    if (k <= 0) {
      return 0;
    }

    std::size_t idx = 0uz;
    int candidate = 0;

    while (k > 0) {
      ++candidate;
      if (idx < arr.size() && arr[idx] == candidate) {
        ++idx;
      } else {
        --k;
      }
    }

    return candidate;
  }
};

LC_PROBLEM(1539, "Kth Missing Positive Number", "two_pointers", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    const std::vector<int> arr = {2, 3, 4, 7, 11};
    ctx.expect_eq(sol.findKthPositive(arr, 5), 9);
    ctx.expect_eq(sol.findKthPositiveTwoPointers(arr, 5), 9);
  });

  ctx.test("example_2_past_end", [&] {
    const std::vector<int> arr = {1, 2, 3, 4};
    ctx.expect_eq(sol.findKthPositive(arr, 2), 6);
    ctx.expect_eq(sol.findKthPositiveTwoPointers(arr, 2), 6);
  });

  ctx.test("missing_before_first_element", [&] {
    const std::vector<int> arr = {5, 6, 7};
    ctx.expect_eq(sol.findKthPositive(arr, 1), 1);
    ctx.expect_eq(sol.findKthPositive(arr, 4), 4);
    ctx.expect_eq(sol.findKthPositiveTwoPointers(arr, 4), 4);
  });

  ctx.test("single_element_boundaries", [&] {
    ctx.expect_eq(sol.findKthPositive({1}, 1), 2);
    ctx.expect_eq(sol.findKthPositive({2}, 1), 1);
    ctx.expect_eq(sol.findKthPositive({2}, 2), 3);
    ctx.expect_eq(sol.findKthPositiveTwoPointers({2}, 2), 3);
  });

  ctx.test("empty_array", [&] {
    ctx.expect_eq(sol.findKthPositive({}, 3), 3);
    ctx.expect_eq(sol.findKthPositiveTwoPointers({}, 3), 3);
  });
}

}  // namespace
