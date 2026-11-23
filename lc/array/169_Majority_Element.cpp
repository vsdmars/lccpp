// LeetCode 169: Majority Element
// https://leetcode.com/problems/majority-element/
//
// Problem:
//   Given an integer array `nums` of size `n`, return the majority element,
//   defined as the element that appears strictly more than `floor(n / 2)`
//   times. You may assume that a majority element always exists in `nums`.
//
// Algorithm:
//   1. Hash-Map Frequency Count (`majorityElement`):
//      Count occurrences of each integer in an `std::unordered_map` and return
//      immediately as soon as any value's frequency exceeds `n / 2`.
//      Time Complexity:  O(n) average.
//      Space Complexity: O(n) in the worst case for distinct elements.
//   2. Boyer-Moore Majority Vote (`majorityElementBoyerMoore`):
//      Maintain a running `candidate` and net vote `balance`. Whenever
//      `balance == 0`, adopt the current element as the new `candidate`; then
//      increment `balance` if `num == candidate`, else decrement it. Every
//      non-majority element can cancel at most one occurrence of the majority
//      element, so because the majority element appears > n / 2 times, it is
//      guaranteed to survive as `candidate` at the end of the pass.
//      Time Complexity:  O(n).
//      Space Complexity: O(1).
//
// Data Structures:
//   - std::unordered_map<int, std::size_t>: tracks element frequencies in the
//     hash-map approach.
//   - Scalar state (`candidate`, `balance`): tracks the surviving consensus
//     candidate in O(1) space for Boyer-Moore.
//
// Real-World Applications:
//   - Quorum and fault-tolerant consensus voting in distributed systems
//     (e.g., Triple Modular Redundancy and Byzantine/replicated state reads).
//   - Streaming heavy-hitter detection in network traffic monitoring and
//     telemetry pipelines (Misra-Gries / Boyer-Moore generalization).
//   - Ensemble model prediction aggregation (majority-vote classification).
//
// Related LeetCode Problems:
//   - #229 Majority Element II [new] (Boyer-Moore extended to > n / 3 threshold)
//   - #1150 Check If a Number Is Majority Element in a Sorted Array [new] (O(log n) binary search)
//   - #2780 Minimum Index of a Valid Split [new] (prefix/suffix dominant element split)
//   - #136 Single Number [done] (O(1)-space linear cancellation trick)

#include "framework/lc_framework.h"

#include <cstddef>
#include <limits>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the majority element by counting frequencies in a hash map and
  // short-circuiting as soon as a value's count exceeds n / 2.
  // Time: O(n) average, Space: O(n).
  int majorityElement(const std::vector<int>& nums) {
    const std::size_t threshold = nums.size() / 2;
    std::unordered_map<int, std::size_t> counts;

    for (const int num : nums) {
      if (++counts[num] > threshold) {
        return num;
      }
    }

    // Unreachable when a majority element exists (problem guarantee).
    return 0;
  }

  // Finds the majority element in O(1) space using Boyer-Moore Majority Vote.
  // Invariant: if the prefix already processed is split whenever `balance`
  // hits 0, each discarded prefix has at most half of its elements equal to
  // the true majority, so the true majority remains a majority in the suffix.
  // Time: O(n), Space: O(1).
  int majorityElementBoyerMoore(const std::vector<int>& nums) {
    int candidate = 0;
    int balance = 0;

    for (const int num : nums) {
      if (balance == 0) {
        candidate = num;
      }
      balance += (num == candidate) ? 1 : -1;
    }

    return candidate;
  }
};

LC_PROBLEM(169, "Majority Element", "array", ctx) {
  Solution sol;

  ctx.test("example_1_length_three", [&] {
    const std::vector<int> nums{3, 2, 3};
    ctx.expect_eq(sol.majorityElement(nums), 3);
    ctx.expect_eq(sol.majorityElementBoyerMoore(nums), 3);
  });

  ctx.test("example_2_interleaved", [&] {
    const std::vector<int> nums{2, 2, 1, 1, 1, 2, 2};
    ctx.expect_eq(sol.majorityElement(nums), 2);
    ctx.expect_eq(sol.majorityElementBoyerMoore(nums), 2);
  });

  ctx.test("single_element", [&] {
    const std::vector<int> nums{42};
    ctx.expect_eq(sol.majorityElement(nums), 42);
    ctx.expect_eq(sol.majorityElementBoyerMoore(nums), 42);
  });

  ctx.test("all_identical_negative", [&] {
    const std::vector<int> nums{-5, -5, -5, -5};
    ctx.expect_eq(sol.majorityElement(nums), -5);
    ctx.expect_eq(sol.majorityElementBoyerMoore(nums), -5);
  });

  ctx.test("majority_clustered_at_tail", [&] {
    const std::vector<int> nums{1, 2, 3, 4, 4, 4, 4};
    ctx.expect_eq(sol.majorityElement(nums), 4);
    ctx.expect_eq(sol.majorityElementBoyerMoore(nums), 4);
  });

  ctx.test("extreme_int32_majority", [&] {
    constexpr int kMin = std::numeric_limits<int>::min();
    const std::vector<int> nums{kMin, 1, kMin};
    ctx.expect_eq(sol.majorityElement(nums), kMin);
    ctx.expect_eq(sol.majorityElementBoyerMoore(nums), kMin);
  });
}

}  // namespace
