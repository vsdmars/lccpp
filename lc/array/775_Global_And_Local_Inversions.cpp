// LeetCode 775: Global And Local Inversions
// https://leetcode.com/problems/global-and-local-inversions/
//
// Problem:
//   Given an integer array `nums` of length `n` representing a permutation of
//   all integers in `[0, n - 1]`, determine whether the number of global
//   inversions equals the number of local inversions:
//     - A global inversion is a pair `(i, j)` with `0 <= i < j < n` and
//       `nums[i] > nums[j]`.
//     - A local inversion is an index `i` with `0 <= i < n - 1` and
//       `nums[i] > nums[i + 1]`.
//   Return `true` if the two counts are equal, and `false` otherwise.
//   Constraints: 1 <= n <= 10^5; `nums` is a permutation of `0..n-1`.
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 0, 2]
//     Output: true
//     Explanation:
//       - At idx = 0, compute |nums[0] - 0| = |1 - 0| = 1 <= 1.
//       - At idx = 1, compute |nums[1] - 1| = |0 - 1| = 1 <= 1.
//       - At idx = 2, compute |nums[2] - 2| = |2 - 2| = 0 <= 1.
//       - All elements sit within distance 1 of their sorted positions, so return
//         true.
//
//   Example 2:
//     Input:  nums = [1, 2, 0]
//     Output: false
//     Explanation:
//       - At idx = 0 and idx = 1, |1 - 0| = 1 and |2 - 1| = 1 both satisfy
//         displacement <= 1.
//       - At idx = 2, |nums[2] - 2| = |0 - 2| = 2 > 1 exceeds the allowed
//         displacement of 1.
//       - Return false immediately because the displacement proves a non-local
//         inversion exists.
//
//   Example 3:
//     Input:  nums = [0, 3, 2, 1]
//     Output: false
//     Explanation:
//       - At idx = 0, compute |0 - 0| = 0 <= 1 and continue.
//       - At idx = 1, compute |nums[1] - 1| = |3 - 1| = 2 > 1.
//       - Return false immediately without scanning further.
//
// Algorithm:
//   Every local inversion is automatically a global inversion, so the two
//   counts are equal if and only if there is no non-local inversion (`i + 2 <= j`
//   with `nums[i] > nums[j]`). Because `nums` is a permutation of `0..n-1`,
//   no non-local inversion exists if and only if every element sits at most
//   one slot away from its sorted index, i.e., `|nums[i] - i| <= 1` for all `i`:
//     - Necessity: if `nums[i] >= i + 2`, then at least two elements smaller
//       than `nums[i]` must appear to the right of `i`, so at least one sits at
//       some `j >= i + 2` (forming a non-local inversion). Symmetrically, if
//       `nums[i] <= i - 2`, at least one larger value sits at some `k <= i - 2`.
//     - Sufficiency: if `|nums[k] - k| <= 1` for all `k`, then for any
//       `j >= i + 2`, `nums[i] <= i + 1 <= j - 1 <= nums[j]`, so no non-local
//       inversion can occur.
//   Time Complexity:  O(n), single pass verifying displacement at each index.
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - Direct Index-Value Comparison on the input array (`std::vector<int>`):
//     no auxiliary structures needed.
//
// Real-World Applications:
//   - Streaming and network packet reordering diagnostics: verifying whether
//     out-of-order packets suffer only adjacent swaps (bounded 1-step jitter)
//     rather than deep reordering requiring large reassembly buffers.
//   - Adaptive sorting selection: detecting nearly-sorted permutations where
//     one pass of bubble/insertion sort finishes in linear time.
//   - Permutation group / combinatorics analysis: identifying permutations
//     decomposable into disjoint adjacent transpositions.
//
// Related LeetCode Problems:
//   - #315 Count of Smaller Numbers After Self [done] (general inversion counting via Fenwick/merge sort)
//   - #493 Reverse Pairs [new] (modified global inversion counting)
//   - #769 Max Chunks To Make Sorted [new] (permutation index-displacement invariants)
//   - #287 Find the Duplicate Number [done] (pigeonhole reasoning on 0..n-1 / 1..n arrays)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdlib>
#include <vector>

namespace {

class Solution {
 public:
  // Returns true iff `nums` (a permutation of 0..n-1) contains no non-local
  // inversion, which is equivalent to `|nums[i] - i| <= 1` for every index `i`.
  // Time: O(n), Space: O(1).
  bool isIdealPermutation(const std::vector<int>& nums) {
    for (std::size_t idx = 0uz; idx < nums.size(); ++idx) {
      // Any displacement greater than 1 forces a non-local inversion by the
      // pigeonhole principle over the permutation 0..n-1.
      if (std::abs(nums[idx] - static_cast<int>(idx)) > 1) {
        return false;
      }
    }
    return true;
  }
};

LC_PROBLEM(775, "Global And Local Inversions", "array", ctx) {
  Solution sol;

  ctx.test("example_1_single_adjacent_swap", [&] {
    ctx.expect_true(sol.isIdealPermutation({1, 0, 2}));
  });

  ctx.test("example_2_non_local_inversion", [&] {
    ctx.expect_false(sol.isIdealPermutation({1, 2, 0}));
  });

  ctx.test("already_sorted_zero_inversions", [&] {
    ctx.expect_true(sol.isIdealPermutation({0, 1, 2, 3, 4}));
  });

  ctx.test("multiple_disjoint_adjacent_swaps", [&] {
    ctx.expect_true(sol.isIdealPermutation({1, 0, 3, 2, 4}));
    ctx.expect_true(sol.isIdealPermutation({0, 2, 1, 4, 3}));
  });

  ctx.test("single_and_two_elements", [&] {
    ctx.expect_true(sol.isIdealPermutation({0}));
    ctx.expect_true(sol.isIdealPermutation({1, 0}));
  });

  ctx.test("late_non_local_inversion", [&] {
    ctx.expect_false(sol.isIdealPermutation({0, 3, 2, 1}));
  });
}

}  // namespace
