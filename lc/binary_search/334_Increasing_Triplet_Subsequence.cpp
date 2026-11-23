// LeetCode 334: Increasing Triplet Subsequence
// https://leetcode.com/problems/increasing-triplet-subsequence/
//
// Problem:
//   Given an integer array nums, return true if there exists a triple of
//   indices (i, j, k) such that 0 <= i < j < k < n and
//   nums[i] < nums[j] < nums[k]; otherwise return false.
//   Required complexity: O(n) time and O(1) auxiliary space.
//
// Examples:
//   Example 1:
//     Input:  nums = [2, 1, 5, 0, 4, 6]
//     Output: true
//     Explanation:
//       - Initialize first = INT_MAX and second = INT_MAX.
//       - Process 2 (<= first) to set first = 2, then 1 (<= first) to lower first =
//         1.
//       - Process 5 (> first, <= second) to set second = 5.
//       - Process 0 (<= first) to lower first = 0, then 4 (> first, <= second) to
//         tighten second = 4.
//       - Process 6 (> second = 4), confirming a valid increasing triplet such as
//         (1, 4, 6) and returning true.
//
//   Example 2:
//     Input:  nums = [20, 100, 10, 120]
//     Output: true
//     Explanation:
//       - Initialize first = INT_MAX and second = INT_MAX, then process 20 to set
//         first = 20.
//       - Process 100 (> 20, <= INT_MAX) to set second = 100, locking in the
//         invariant that an earlier value 20 < 100 exists.
//       - Process 10 (<= 20) to lower first = 10 while leaving second = 100
//         unchanged.
//       - Process 120 (> second = 100), completing the increasing triplet (20, 100,
//         120) and returning true.
//
//   Example 3:
//     Input:  nums = [6, 7, 1, 2]
//     Output: false
//     Explanation:
//       - Initialize first = INT_MAX and second = INT_MAX, then process 6 to set
//         first = 6 and 7 to set second = 7.
//       - Process 1 (<= 6) to lower first = 1 while keeping second = 7.
//       - Process 2 (> 1, <= 7) to tighten second = 2.
//       - Reach the end of nums without encountering any element greater than
//         second, returning false.
//
// Algorithm:
//   Two equivalent O(n) time, O(1) space approaches are provided:
//   1. Greedy Two-Threshold Scan (primary):
//      Maintain `first` (smallest element seen so far) and `second` (smallest
//      second element of any increasing pair seen so far):
//        - If `num <= first`, update `first = num`.
//        - Else if `num <= second`, update `second = num`.
//        - Else `num > second`, which guarantees some earlier value < `second`
//          existed before `second` (even if `first` was later lowered after
//          `second` was set), so a valid increasing triplet exists.
//      Using `<=` cleanly handles duplicate values and INT_MAX without needing
//      sentinel overflows.
//   2. Patience Sort / LIS Truncated at Length 3 (mirrors the Python source):
//      Apply LeetCode #300's patience-sorting `tails` array with
//      `std::ranges::lower_bound`, returning true immediately once
//      `tails.size() == 3`. Because `tails` never grows beyond 3 elements,
//      both the binary search and storage are strictly O(1).
//   Time: O(n), Space: O(1).
//
// Data Structures:
//   - Two scalar integer thresholds (`first`, `second`) in the greedy scan.
//   - A tiny bounded `std::vector<int>` (max size 3) in the patience-sort
//     variant.
//
// Real-World Applications:
//   - Streaming anomaly / trend detection in financial tick streams (detecting
//     a sustained three-step upward breakout in a single O(1)-memory pass).
//   - Real-time telemetry alerting for monotonically escalating latency or
//     temperature thresholds without buffering historical windows.
//
// Related LeetCode Problems:
//   - #300 Longest Increasing Subsequence [done] (generalizes length-3 check to arbitrary maximum length)
//   - #456 132 Pattern [new] (three-element subsequence with ordering nums[i] < nums[k] < nums[j])
//   - #1995 Count Special Quadruplets [new] (ordered subsequence condition on tuples)
//   - #2179 Count Good Triplets in an Array [new] (counting increasing triplets across two permutations)

#include "framework/lc_framework.h"

#include <algorithm>
#include <limits>
#include <vector>

namespace {

class Solution {
 public:
  // Checks in a single O(n) pass and O(1) space whether any indices i < j < k
  // satisfy nums[i] < nums[j] < nums[k].
  //
  // Invariant: Whenever `second < INT_MAX`, there exists an index p < q with
  // nums[p] < nums[q] == second (even if `first` was updated after q).
  // Time: O(n), Space: O(1).
  bool increasingTriplet(const std::vector<int>& nums) {
    int first = std::numeric_limits<int>::max();
    int second = std::numeric_limits<int>::max();

    for (const int num : nums) {
      if (num <= first) {
        // Strictly smaller (or equal) candidate for the first element of a
        // future triplet.
        first = num;
      } else if (num <= second) {
        // Found a pair (first < num) with a tighter second threshold.
        second = num;
      } else {
        // num > second implies some earlier element < second < num exists.
        return true;
      }
    }

    return false;
  }

  // Patience-sorting approach mirroring the Python source (specialized #300).
  // Since `tails` holds at most 3 elements before returning, time is O(n) and
  // space is O(1).
  bool increasingTripletPatience(const std::vector<int>& nums) {
    std::vector<int> tails;
    tails.reserve(3uz);

    for (const int num : nums) {
      const auto it = std::ranges::lower_bound(tails, num);
      if (it == tails.end()) {
        tails.push_back(num);
        if (tails.size() == 3uz) {
          return true;
        }
      } else {
        *it = num;
      }
    }

    return false;
  }
};

LC_PROBLEM(334, "Increasing Triplet Subsequence", "binary_search", ctx) {
  ctx.test("docstring_and_build_examples", [&] {
    Solution sol;
    const std::vector<int> ex1 = {1, 2, 3, 4, 5};
    const std::vector<int> ex2 = {5, 4, 3, 2, 1};
    const std::vector<int> ex3 = {100, 1000, 1, 2, 3};
    const std::vector<int> ex4 = {6, 7, 1, 2};
    const std::vector<int> ex5 = {1, 1, 1, 1};
    const std::vector<int> ex6 = {2, 1, 5, 0, 4, 6};

    ctx.expect_true(sol.increasingTriplet(ex1));
    ctx.expect_false(sol.increasingTriplet(ex2));
    ctx.expect_true(sol.increasingTriplet(ex3));
    ctx.expect_false(sol.increasingTriplet(ex4));
    ctx.expect_false(sol.increasingTriplet(ex5));
    ctx.expect_true(sol.increasingTriplet(ex6));

    ctx.expect_true(sol.increasingTripletPatience(ex1));
    ctx.expect_false(sol.increasingTripletPatience(ex2));
    ctx.expect_true(sol.increasingTripletPatience(ex3));
    ctx.expect_false(sol.increasingTripletPatience(ex4));
    ctx.expect_false(sol.increasingTripletPatience(ex5));
    ctx.expect_true(sol.increasingTripletPatience(ex6));
  });

  ctx.test("short_and_extreme_int_boundaries", [&] {
    Solution sol;
    constexpr int kMinInt = std::numeric_limits<int>::min();
    constexpr int kMaxInt = std::numeric_limits<int>::max();

    ctx.expect_false(sol.increasingTriplet({}));
    ctx.expect_false(sol.increasingTriplet({1, 2}));
    ctx.expect_true(sol.increasingTriplet({1, 2, kMaxInt}));
    ctx.expect_true(sol.increasingTriplet({kMinInt, 0, kMaxInt}));
    ctx.expect_false(sol.increasingTriplet({kMaxInt, kMaxInt, kMaxInt}));
    ctx.expect_false(sol.increasingTriplet({kMaxInt - 1, kMaxInt, kMaxInt}));

    // Verify `first` lowered after `second` still detects a valid triplet:
    // pair (20, 100) exists, then 10 lowers `first`, then 120 completes (20, 100, 120).
    ctx.expect_true(sol.increasingTriplet({20, 100, 10, 120}));
    ctx.expect_true(sol.increasingTripletPatience({20, 100, 10, 120}));
  });
}

}  // namespace
