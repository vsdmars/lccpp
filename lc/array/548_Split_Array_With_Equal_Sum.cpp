// LeetCode 548: Split Array With Equal Sum
// https://leetcode.com/problems/split-array-with-equal-sum/
//
// Problem:
//   Given an integer array `nums` of length `n`, determine whether there exist
//   three cut indices `(i, j, k)` satisfying:
//     1. `0 < i`, `i + 1 < j`, and `j + 1 < k < n - 1` (each of the four
//        resulting subarrays is non-empty, so `n >= 7`).
//     2. The sums of the four exclusive-of-cuts subarrays
//        `nums[0..i-1]`, `nums[i+1..j-1]`, `nums[j+1..k-1]`, and
//        `nums[k+1..n-1]` are all equal.
//
// Examples:
//   Example 1:
//     Input:  nums = [1, 2, 1, 2, 1, 2, 1]
//     Output: true
//     Explanation:
//       - With n = 7, build prefix = [0, 1, 3, 4, 6, 7, 9, 10] and iterate middle
//         cut j over [3, 3].
//       - At j = 3, collectLeftEqualSums tests i = 1: first = prefix[1] - prefix[0]
//         = 1 and second = prefix[3] - prefix[2] = 1, inserting 1 into left_sums.
//       - hasMatchingRightSplit tests k = 5: third = prefix[5] - prefix[4] = 1 and
//         fourth = prefix[7] - prefix[6] = 1, which matches left_sums = {1} and
//         returns true.
//
//   Example 2:
//     Input:  nums = [1, -1, 1, -1, 1, -1, 1]
//     Output: true
//     Explanation:
//       - Build prefix = [0, 1, 0, 1, 0, 1, 0, 1] for n = 7 and evaluate the sole
//         middle cut j = 3.
//       - For i = 1, first = prefix[1] - prefix[0] = 1 equals second = prefix[3] -
//         prefix[2] = 1, so left_sums becomes {1}.
//       - For k = 5, third = prefix[5] - prefix[4] = 1 equals fourth = prefix[7] -
//         prefix[6] = 1 and exists in left_sums, returning true.
//
//   Example 3:
//     Input:  nums = [1, 2, 1, 2, 1, 2, 2]
//     Output: false
//     Explanation:
//       - Build prefix = [0, 1, 3, 4, 6, 7, 9, 11] for n = 7 and evaluate the only
//         valid middle cut j = 3.
//       - collectLeftEqualSums checks i = 1 and finds first = 1 equals second = 4 -
//         3 = 1, inserting 1 into left_sums.
//       - hasMatchingRightSplit checks k = 5 and gets third = 7 - 6 = 1 and fourth
//         = 11 - 9 = 2; because third != fourth and no other j exists, return
//         false.
//
// Algorithm:
//   Meet-in-the-middle decomposition anchored on the middle cut `j` combined
//   with a precomputed prefix-sum array:
//     1. If `n < 7`, return `false` immediately.
//     2. Build `prefix` of length `n + 1` using `std::int64_t` (avoiding
//        32-bit overflow) so any inclusive subarray sum `nums[l..r]` is
//        computed in O(1) as `prefix[r + 1] - prefix[l]`.
//     3. Iterate over every valid middle cut `j` in `[3, n - 4]`:
//        - Scan all left cuts `i` in `[1, j - 2]`. Whenever the left two
//          quarters have equal sums, insert that shared sum into a hash set
//          `left_sums`.
//        - If `left_sums` is non-empty, scan all right cuts `k` in
//          `[j + 2, n - 2]`. Whenever the right two quarters have equal sums
//          AND that sum exists in `left_sums`, all four quarters match;
//          return `true`.
//     4. If no middle cut `j` yields a match, return `false`.
//
//   Fixing `j` decouples the choice of `i` and `k`, reducing naive O(n^3)
//   triplet enumeration to O(n^2) time.
//
//   Time Complexity:  O(n^2) — O(n) choices of `j`, each scanning O(n) left
//                     and right cuts with O(1) average hash-set operations.
//   Space Complexity: O(n) — prefix-sum array plus the per-`j` hash set.
//
// Data Structures:
//   - `std::vector<std::int64_t>` (`prefix`): prefix sums for O(1) range-sum
//     queries without overflow.
//   - `std::unordered_set<std::int64_t>` (`left_sums`): stores achievable
//     equal quarter-sums on the left side of the current middle cut `j`.
//
// Real-World Applications:
//   - Balanced multi-way workload partitioning (e.g., splitting sequential
//     pipeline stages or time-series chunks with separator checkpoints).
//   - Meet-in-the-middle query planning and multi-join optimization in
//     databases where anchoring a middle pivot decouples independent subplans.
//   - Financial ledger reconciliation searching for equal-sum accounting
//     periods separated by adjustment entries.
//
// Related LeetCode Problems:
//   - #1013 Partition Array Into Three Parts With Equal Sum [new] (3-way contiguous split without excluded pivot elements)
//   - #416 Partition Equal Subset Sum [done] (subset-sum 2-way equal partition via DP)
//   - #724 Find Pivot Index [done] (single excluded pivot balancing left and right sums)
//   - #560 Subarray Sum Equals K [done] (prefix-sum + hash table lookup for subarray sums)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <unordered_set>
#include <vector>

namespace {

class Solution {
 public:
  // Returns true iff `nums` can be split at three non-adjacent interior
  // indices `(i, j, k)` into four non-empty subarrays of equal sum.
  //
  // Time: O(n^2), Space: O(n).
  bool splitArray(const std::vector<int>& nums) {
    const std::size_t n = nums.size();
    // Minimum valid layout is [x, i, x, j, x, k, x], requiring 7 elements.
    if (n < 7uz) {
      return false;
    }

    // Build 1-indexed prefix sums so range_sum(l, r) = prefix[r + 1] - prefix[l].
    std::vector<std::int64_t> prefix(n + 1uz, 0);
    for (std::size_t idx = 0uz; idx < n; ++idx) {
      prefix[idx + 1uz] = prefix[idx] + static_cast<std::int64_t>(nums[idx]);
    }

    // Anchor the middle cut `j` to decouple the left cut `i` and right cut `k`.
    for (std::size_t j = 3uz; (j + 3uz) < n; ++j) {
      const std::unordered_set<std::int64_t> left_sums =
          collectLeftEqualSums(prefix, j);
      if (left_sums.empty()) {
        continue;
      }

      if (hasMatchingRightSplit(prefix, j, left_sums)) {
        return true;
      }
    }

    return false;
  }

 private:
  // Collects all values `s` such that some left cut `i` in `[1, j - 2]` makes
  // `sum(0..i-1) == sum(i+1..j-1) == s`.
  static std::unordered_set<std::int64_t> collectLeftEqualSums(
      const std::vector<std::int64_t>& prefix, std::size_t j) {
    std::unordered_set<std::int64_t> left_sums;
    for (std::size_t i = 1uz; (i + 1uz) < j; ++i) {
      const std::int64_t first = prefix[i] - prefix[0uz];
      const std::int64_t second = prefix[j] - prefix[i + 1uz];
      if (first == second) {
        left_sums.insert(first);
      }
    }
    return left_sums;
  }

  // Returns true if some right cut `k` in `[j + 2, n - 2]` produces equal
  // third and fourth quarter sums whose value also appears in `left_sums`.
  static bool hasMatchingRightSplit(
      const std::vector<std::int64_t>& prefix, std::size_t j,
      const std::unordered_set<std::int64_t>& left_sums) {
    const std::size_t n = prefix.size() - 1uz;
    for (std::size_t k = j + 2uz; (k + 1uz) < n; ++k) {
      const std::int64_t third = prefix[k] - prefix[j + 1uz];
      const std::int64_t fourth = prefix[n] - prefix[k + 1uz];
      if (third == fourth && left_sums.contains(third)) {
        return true;
      }
    }
    return false;
  }
};

LC_PROBLEM(548, "Split Array With Equal Sum", "array", ctx) {
  ctx.test("example_1_min_length_valid", [&] {
    Solution sol;
    ctx.expect_true(sol.splitArray({1, 2, 1, 2, 1, 2, 1}));
  });

  ctx.test("python_build_longer_valid", [&] {
    Solution sol;
    ctx.expect_true(sol.splitArray({1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1}));
  });

  ctx.test("too_short_array", [&] {
    Solution sol;
    ctx.expect_false(sol.splitArray({1, 1, 1, 1, 1, 1}));
    ctx.expect_false(sol.splitArray({}));
  });

  ctx.test("all_zeros_valid", [&] {
    Solution sol;
    ctx.expect_true(sol.splitArray({0, 0, 0, 0, 0, 0, 0}));
  });

  ctx.test("negative_numbers_valid", [&] {
    Solution sol;
    ctx.expect_true(sol.splitArray({1, -1, 1, -1, 1, -1, 1}));
  });

  ctx.test("no_valid_split", [&] {
    Solution sol;
    ctx.expect_false(sol.splitArray({1, 2, 1, 2, 1, 2, 2}));
    ctx.expect_false(sol.splitArray({1, 2, 3, 4, 5, 6, 7}));
  });
}

}  // namespace
