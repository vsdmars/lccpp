// LeetCode 1929: Concatenation Of Array
// https://leetcode.com/problems/concatenation-of-array/
//
// Problem:
//   Given an integer array `nums` of length `n`, construct and return an array
//   `ans` of length `2n` such that `ans[i] == nums[i]` and
//   `ans[i + n] == nums[i]` for every `0 <= i < n`. That is, `ans` is the
//   concatenation of two copies of `nums`.
//   Constraints: 1 <= n <= 1000; 1 <= nums[i] <= 1000.
//
// Algorithm:
//   Single-Allocation Double Copy:
//   Pre-allocate an output vector with exact capacity `2 * nums.size()` via
//   `reserve()`, then append the contiguous range `[nums.begin(), nums.end())`
//   twice. Pre-reserving avoids intermediate reallocations and copies while
//   keeping the input immutable (unlike self-inserting into `nums` without
//   reserving, which would invalidate iterators in C++).
//   Time Complexity:  O(n)
//   Space Complexity: O(n) for the returned array of size 2n (O(1) extra
//                     working space).
//
// Data Structures:
//   - `std::vector<int>` pre-reserved to `2 * n` elements for contiguous
//     bulk-copy construction.
//
// Real-World Applications:
//   - Circular-array linearization: doubling an array so any wrap-around
//     window or rotation can be scanned as a contiguous subarray without
//     modulo indexing (used in string rotation checks and circular buffers).
//   - Audio/signal buffer tiling and periodic boundary padding in DSP and
//     image processing pipelines.
//
// Related LeetCode Problems:
//   - #1920 Build Array from Permutation [new] (basic index-mapped array construction)
//   - #1470 Shuffle the Array [new] (rearranging two halves of a 2n array)
//   - #796 Rotate String [new] (classic use of `s + s` concatenation to check cyclic shifts)
//   - #503 Next Greater Element II [done] (circular array traversal conceptually modeled by doubling)

#include "framework/lc_framework.h"

#include <vector>

namespace {

class Solution {
 public:
  // Returns a new vector consisting of `nums` concatenated with itself.
  //
  // Time: O(n), Space: O(n) for the output vector of length 2n.
  std::vector<int> getConcatenation(const std::vector<int>& nums) {
    std::vector<int> ans;
    // Reserve the exact final size upfront to guarantee a single allocation.
    ans.reserve(nums.size() * 2uz);

    // Append two contiguous copies of `nums`.
    ans.insert(ans.end(), nums.begin(), nums.end());
    ans.insert(ans.end(), nums.begin(), nums.end());
    return ans;
  }
};

LC_PROBLEM(1929, "Concatenation Of Array", "array", ctx) {
  ctx.test("example_1_three_elements", [&] {
    Solution sol;
    ctx.expect_eq(sol.getConcatenation({1, 2, 1}),
                  std::vector<int>({1, 2, 1, 1, 2, 1}));
  });

  ctx.test("example_2_four_elements", [&] {
    Solution sol;
    ctx.expect_eq(sol.getConcatenation({1, 3, 2, 1}),
                  std::vector<int>({1, 3, 2, 1, 1, 3, 2, 1}));
  });

  ctx.test("single_element", [&] {
    Solution sol;
    ctx.expect_eq(sol.getConcatenation({7}), std::vector<int>({7, 7}));
  });

  ctx.test("empty_input_edge_case", [&] {
    Solution sol;
    ctx.expect_eq(sol.getConcatenation({}), std::vector<int>({}));
  });
}

}  // namespace
