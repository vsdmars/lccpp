// LeetCode 448: Find All Numbers Disappeared In An Array
// https://leetcode.com/problems/find-all-numbers-disappeared-in-an-array/
//
// Problem:
//   Given an integer array `nums` of length `n` where every element satisfies
//   `1 <= nums[i] <= n`, some elements appear twice (or more) and others appear
//   once. Return all integers in the inclusive range `[1, n]` that do not
//   appear in `nums`. The algorithm should run in O(n) time and O(1) extra
//   space (excluding the returned list).
//
// Algorithm:
//   Use the input array itself as an in-place presence map via sign bits:
//     1. Marking pass: For each element `raw` in `nums`, recover its original
//        positive value `val = std::abs(raw)` and target index `slot = val - 1`.
//        If `nums[slot] > 0`, negate it (`nums[slot] = -nums[slot]`) to record
//        that `val` is present. (Only negating when positive ensures duplicate
//        occurrences never flip the sign back.)
//     2. Collection pass: Scan indices `0..n-1`. Any index `i` where
//        `nums[i] > 0` was never targeted, so `i + 1` is missing from `nums`.
//
//   Time Complexity:  O(n) — two linear passes over `nums`.
//   Space Complexity: O(1) auxiliary space (output vector excluded).
//
// Data Structures:
//   - Input `std::vector<int>` repurposed as an in-place sign-marked presence
//     table mapping value `v` to index `v - 1`.
//   - Output `std::vector<int>` storing all missing values in ascending order.
//
// Real-World Applications:
//   - Detecting missing sequence numbers or dropped frames in bounded-window
//     network protocols and video streams without extra heap allocations.
//   - Auditing dense contiguous record IDs (e.g., invoice numbers, log chunks,
//     or storage block indices) to identify gaps after batch ingestion.
//   - Verifying completeness of shard responses in distributed map-reduce jobs.
//
// Related LeetCode Problems:
//   - #442 Find All Duplicates In An Array [done] (same in-place sign-marking trick to report repeated elements)
//   - #41 First Missing Positive [done] (finds the smallest missing positive integer in O(n) time and O(1) space)
//   - #268 Missing Number [done] (finds the single missing integer in [0, n])
//   - #645 Set Mismatch [new] (finds one duplicate and one missing integer in [1, n])

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdlib>
#include <vector>

namespace {

class Solution {
 public:
  // Returns all numbers in `[1, n]` missing from `nums` in ascending order.
  //
  // Invariant: After the first pass, `nums[i] < 0` if and only if the value
  // `i + 1` appeared at least once in the original array.
  //
  // Time: O(n), Space: O(1) auxiliary (excluding result).
  std::vector<int> findDisappearedNumbers(std::vector<int>& nums) {
    // Pass 1: Mark every present value `v` by ensuring `nums[v - 1]` is negative.
    for (const int raw : nums) {
      const auto slot = static_cast<std::size_t>(std::abs(raw) - 1);
      if (nums[slot] > 0) {
        nums[slot] = -nums[slot];
      }
    }

    // Pass 2: Any slot that remained positive corresponds to a missing value.
    std::vector<int> missing;
    for (std::size_t i = 0uz; i < nums.size(); ++i) {
      if (nums[i] > 0) {
        missing.push_back(static_cast<int>(i + 1uz));
      }
    }

    return missing;
  }
};

LC_PROBLEM(448, "Find All Numbers Disappeared In An Array", "array", ctx) {
  ctx.test("example_1_standard", [&] {
    Solution sol;
    std::vector<int> nums = {4, 3, 2, 7, 8, 2, 3, 1};
    ctx.expect_eq(sol.findDisappearedNumbers(nums), std::vector<int>{5, 6});
  });

  ctx.test("example_2_duplicate_ones", [&] {
    Solution sol;
    std::vector<int> nums = {1, 1};
    ctx.expect_eq(sol.findDisappearedNumbers(nums), std::vector<int>{2});
  });

  ctx.test("all_present_permutation", [&] {
    Solution sol;
    std::vector<int> nums = {3, 1, 2, 4};
    ctx.expect_eq(sol.findDisappearedNumbers(nums), std::vector<int>{});
  });

  ctx.test("all_same_element", [&] {
    Solution sol;
    std::vector<int> nums = {2, 2, 2, 2};
    ctx.expect_eq(sol.findDisappearedNumbers(nums), std::vector<int>{1, 3, 4});
  });

  ctx.test("single_element_present", [&] {
    Solution sol;
    std::vector<int> nums = {1};
    ctx.expect_eq(sol.findDisappearedNumbers(nums), std::vector<int>{});
  });

  ctx.test("empty_array", [&] {
    Solution sol;
    std::vector<int> nums = {};
    ctx.expect_eq(sol.findDisappearedNumbers(nums), std::vector<int>{});
  });
}

}  // namespace
