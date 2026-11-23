// LeetCode 442: Find All Duplicates In An Array
// https://leetcode.com/problems/find-all-duplicates-in-an-array/
//
// Problem:
//   Given an integer array `nums` of length `n` where every element satisfies
//   `1 <= nums[i] <= n` and each integer appears at most twice, return an
//   array of all integers that appear twice. The algorithm must run in O(n)
//   time and use only O(1) extra space (excluding the returned list).
//
// Algorithm:
//   Exploit the value range `[1, n]` to use the input array itself as an
//   in-place boolean visited bitset encoded in the sign bit of each element:
//     - For each element `num` in `nums`, recover its original positive value
//       `val = std::abs(num)` and map it to slot `idx = val - 1`.
//     - If `nums[idx] < 0`, slot `idx` was already flipped on an earlier
//       encounter, meaning `val` appears twice; append `val` to `duplicates`.
//     - Otherwise, negate `nums[idx]` to record the first visit of `val`.
//
//   Because every element appears at most twice, no slot is flipped back to
//   positive after becoming negative, and taking `std::abs` preserves the
//   original magnitude needed when visiting `nums[idx]` later in the scan.
//
//   Time Complexity:  O(n) — single linear pass over `nums`.
//   Space Complexity: O(1) auxiliary space (output vector excluded).
//
// Data Structures:
//   - Input `std::vector<int>` repurposed as an in-place sign-marked hash
//     table mapping value `v` to index `v - 1`.
//   - Output `std::vector<int>` collecting every duplicate value found.
//
// Real-World Applications:
//   - Memory-constrained embedded telemetry deduplication when IDs belong to
//     a dense bounded range `[1, N]`.
//   - Integrity verification of dense sequence numbers (e.g., packet IDs,
//     contiguous database primary keys, or batch job indices) without
//     allocating secondary hash tables.
//   - In-place permutation validation in numerical linear algebra libraries.
//
// Related LeetCode Problems:
//   - #448 Find All Numbers Disappeared In An Array [done] (same in-place sign-marking trick to detect absent values)
//   - #287 Find the Duplicate Number [done] (bounded range [1, n] duplicate detection when input cannot be modified)
//   - #41 First Missing Positive [done] (in-place index-mapping on [1, n] for arbitrary integers)
//   - #645 Set Mismatch [new] (finds both the duplicated and missing number in [1, n])
//   - #268 Missing Number [done] (finds a single missing value in [0, n])

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdlib>
#include <vector>

namespace {

class Solution {
 public:
  // Finds all integers in `[1, n]` that appear twice in `nums` using in-place
  // sign marking.
  //
  // Invariant: Before processing position `i`, `nums[v - 1] < 0` if and only if
  // value `v` has appeared at least once in `nums[0..i-1]`.
  //
  // Time: O(n), Space: O(1) auxiliary (excluding result).
  std::vector<int> findDuplicates(std::vector<int>& nums) {
    std::vector<int> duplicates;

    for (const int raw : nums) {
      // Recover the original magnitude in case this slot was already negated.
      const int val = std::abs(raw);
      const auto slot = static_cast<std::size_t>(val - 1);

      if (nums[slot] < 0) {
        // Negative sign proves `val` was seen once before; record duplicate.
        duplicates.push_back(val);
      } else {
        // Flip sign to mark the first occurrence of `val`.
        nums[slot] = -nums[slot];
      }
    }

    return duplicates;
  }
};

LC_PROBLEM(442, "Find All Duplicates In An Array", "array", ctx) {
  ctx.test("example_1_standard", [&] {
    Solution sol;
    std::vector<int> nums = {4, 3, 2, 7, 8, 2, 3, 1};
    ctx.expect_eq(sol.findDuplicates(nums), std::vector<int>{2, 3});
  });

  ctx.test("example_2_small_duplicate", [&] {
    Solution sol;
    std::vector<int> nums = {1, 1, 2};
    ctx.expect_eq(sol.findDuplicates(nums), std::vector<int>{1});
  });

  ctx.test("example_3_single_element", [&] {
    Solution sol;
    std::vector<int> nums = {1};
    ctx.expect_eq(sol.findDuplicates(nums), std::vector<int>{});
  });

  ctx.test("no_duplicates", [&] {
    Solution sol;
    std::vector<int> nums = {1, 2, 3, 4, 5};
    ctx.expect_eq(sol.findDuplicates(nums), std::vector<int>{});
  });

  ctx.test("all_pairs_duplicated", [&] {
    Solution sol;
    std::vector<int> nums = {2, 2, 1, 1};
    ctx.expect_eq(sol.findDuplicates(nums), std::vector<int>{2, 1});
  });

  ctx.test("empty_array", [&] {
    Solution sol;
    std::vector<int> nums = {};
    ctx.expect_eq(sol.findDuplicates(nums), std::vector<int>{});
  });
}

}  // namespace
