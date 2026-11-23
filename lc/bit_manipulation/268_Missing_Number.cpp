// LeetCode 268: Missing Number
// https://leetcode.com/problems/missing-number/
//
// Problem:
//   Given an array `nums` containing `n` distinct integers taken from the
//   inclusive range `[0, n]`, find and return the single number in `[0, n]`
//   that is missing from the array. The solution must run in O(n) linear time
//   and O(1) constant auxiliary space.
//
// Algorithm:
//   1. XOR Index-Value Cancellation (`missingNumber`):
//      Every number present in `nums` appears once as a value and once as an
//      index in `{0, 1, ..., n}` (with `n` seeded upfront). XORing `n`, every
//      index `i`, and every `nums[i]` cancels all present numbers (`x ^ x = 0`),
//      leaving only the missing number without any risk of integer overflow.
//      Time: O(n), Space: O(1).
//   2. Gauss Arithmetic Sum (`missingNumberGauss`, ported from Python
//      `youshouldbesmart`):
//      Computes `n * (n + 1) / 2 - sum(nums)` using 64-bit signed integers so
//      large `n` cannot overflow 32-bit multiplication or summation.
//      Time: O(n), Space: O(1).
//
// Data Structures:
//   - Single scalar integer accumulator (XOR register or 64-bit running sum).
//
// Real-World Applications:
//   - Detecting a dropped packet sequence number in a contiguous sliding
//     window of TCP/UDP frames without allocating a bitset.
//   - Verifying contiguous shard, partition, or chunk IDs during distributed
//     file reassembly and ETL batch completeness audits.
//   - Database primary-key gap verification across dense ID ranges.
//
// Related LeetCode Problems:
//   - #136 Single Number [done] (same XOR self-cancellation principle over paired elements)
//   - #260 Single Number III [done] (extends XOR cancellation to two missing/unique items)
//   - #41 First Missing Positive [done] (generalizes missing-number recovery to arbitrary arrays via cyclic placement)
//   - #287 Find the Duplicate Number [done] (complementary pigeonhole problem on `[1, n]`)
//   - #448 Find All Numbers Disappeared in an Array [done] (finds multiple missing values in `[1, n]`)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the single missing integer in `[0, nums.size()]` via XOR cancellation.
  // Invariant: after processing index `i`, `missing` equals
  // `n ^ (0 ^ 1 ^ ... ^ i) ^ (nums[0] ^ ... ^ nums[i])`.
  // Unlike arithmetic summation, XOR never overflows regardless of `n`.
  // Time: O(n), Space: O(1).
  int missingNumber(const std::vector<int>& nums) {
    // Seed with `n` because loop indices cover `0 .. n - 1`.
    int missing = static_cast<int>(nums.size());
    for (std::size_t i = 0uz; i < nums.size(); ++i) {
      missing ^= static_cast<int>(i) ^ nums[i];
    }
    return missing;
  }

  // Secondary approach (ported from Python `youshouldbesmart`):
  // Subtracts the actual element sum from Gauss's closed-form expected sum
  // `n * (n + 1) / 2`, widened to int64_t to prevent overflow.
  // Time: O(n), Space: O(1).
  int missingNumberGauss(const std::vector<int>& nums) {
    const auto n = static_cast<std::int64_t>(nums.size());
    const std::int64_t expected_sum = (n * (n + 1)) / 2;

    std::int64_t actual_sum = 0;
    for (int num : nums) {
      actual_sum += static_cast<std::int64_t>(num);
    }
    return static_cast<int>(expected_sum - actual_sum);
  }
};

LC_PROBLEM(268, "Missing Number", "bit_manipulation", ctx) {
  Solution sol;

  ctx.test("example_1_small", [&] {
    const std::vector<int> nums = {3, 0, 1};
    ctx.expect_eq(sol.missingNumber(nums), 2);
    ctx.expect_eq(sol.missingNumberGauss(nums), 2);
  });

  ctx.test("example_2_nine_elements", [&] {
    const std::vector<int> nums = {9, 6, 4, 2, 3, 5, 7, 0, 1};
    ctx.expect_eq(sol.missingNumber(nums), 8);
    ctx.expect_eq(sol.missingNumberGauss(nums), 8);
  });

  ctx.test("missing_endpoints_zero_and_n", [&] {
    const std::vector<int> missing_n = {0, 1};
    ctx.expect_eq(sol.missingNumber(missing_n), 2);
    ctx.expect_eq(sol.missingNumberGauss(missing_n), 2);

    const std::vector<int> missing_zero = {1, 2, 3};
    ctx.expect_eq(sol.missingNumber(missing_zero), 0);
    ctx.expect_eq(sol.missingNumberGauss(missing_zero), 0);
  });

  ctx.test("single_and_empty_arrays", [&] {
    ctx.expect_eq(sol.missingNumber({0}), 1);
    ctx.expect_eq(sol.missingNumberGauss({0}), 1);

    ctx.expect_eq(sol.missingNumber({1}), 0);
    ctx.expect_eq(sol.missingNumberGauss({1}), 0);

    // Empty array (n = 0) has 0 as its sole missing element in [0, 0].
    ctx.expect_eq(sol.missingNumber({}), 0);
    ctx.expect_eq(sol.missingNumberGauss({}), 0);
  });
}

}  // namespace
