// LeetCode 1: Two Sum
// https://leetcode.com/problems/two-sum/
//
// Problem:
//   Given an integer array `nums` and an integer `target`, return the two
//   distinct indices `i` and `j` such that `nums[i] + nums[j] == target`.
//   Each input has exactly one valid solution, the same element cannot be used
//   twice, and the answer may be returned in any order.
//
// Algorithm:
//   Single-pass complement lookup using a hash map. As we scan `nums` from
//   left to right, we compute the required complement `need = target - nums[i]`
//   and check whether `need` has already been seen at an earlier index. If so,
//   we immediately return `{seen[need], i}`; otherwise we record
//   `seen[nums[i]] = i`. Checking before inserting prevents pairing an element
//   with itself while naturally handling duplicate values (such as `[3, 3]`).
//   Time: O(N) average, Space: O(N).
//
// Data Structures:
//   - `std::unordered_map<int, int> seen`: maps each previously visited array
//     value to its index for O(1) average-time complement lookup.
//   - `std::vector<int>`: stores the input array and the two-element result.
//
// Real-World Applications:
//   - Financial reconciliation and payment matching (pairing transactions or
//     invoices that sum to a target settlement amount).
//   - Hash-join probing in relational databases, where one relation is indexed
//     in memory and streamed rows probe for matching join keys in O(1) time.
//   - Inventory or resource pairing systems matching complementary capacities.
//
// Related LeetCode Problems:
//   - #167 Two Sum II [done] - Input Array Is Sorted (two-pointer approach in O(1)
//     extra space when input is sorted)
//   - #15 3Sum [done] (extends pair lookup to unique triplets summing to zero)
//   - #18 4Sum [done] (generalizes target sum search to k-tuples with deduplication)
//   - #560 Subarray Sum Equals K [done] (uses the same hash-map complement technique
//     over running prefix sums)
//   - #170 Two Sum III [done] - Data structure design (streaming API supporting add
//     and pair-sum lookup queries)

#include "framework/lc_framework.h"

#include <cstddef>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  std::vector<int> twoSum(const std::vector<int>& nums, int target) {
    std::unordered_map<int, int> seen;
    for (std::size_t i = 0uz; i < nums.size(); ++i) {
      const int need = target - nums[i];
      if (auto it = seen.find(need); it != seen.end()) {
        return {it->second, static_cast<int>(i)};
      }
      seen.emplace(nums[i], static_cast<int>(i));
    }
    return {};
  }
};

LC_PROBLEM(1, "Two Sum", "array", ctx) {
  Solution sol;

  ctx.test("example 1: [2,7,11,15], target=9", [&] {
    ctx.expect_eq(sol.twoSum({2, 7, 11, 15}, 9), std::vector<int>{0, 1});
  });

  ctx.test("example 2: [3,2,4], target=6", [&] {
    ctx.expect_eq(sol.twoSum({3, 2, 4}, 6), std::vector<int>{1, 2});
  });

  ctx.test("example 3: duplicate values [3,3], target=6", [&] {
    ctx.expect_eq(sol.twoSum({3, 3}, 6), std::vector<int>{0, 1});
  });
}

}  // namespace
