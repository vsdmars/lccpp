// LeetCode 321: Create Maximum Number
// https://leetcode.com/problems/create-maximum-number/
//
// Problem:
//   Given two integer arrays nums1 (length m) and nums2 (length n) whose
//   elements are decimal digits in [0, 9], construct the lexicographically
//   largest array of length k (0 <= k <= m + n) using digits from nums1 and
//   nums2 such that the relative order of digits taken from the same array is
//   preserved.
//
// Algorithm:
//   Enumerate every feasible split (i, k - i), where i digits are selected from
//   nums1 and k - i digits from nums2, bounded by max(0, k - n) <= i <= min(k,
//   m). For each split:
//     1. Extract the lexicographically largest subsequence of the required
//        length from each array using a greedy monotonic decreasing stack with
//        a fixed discard budget (drop = nums.size() - count). Whenever the
//        current digit exceeds the stack top and drop > 0, pop the smaller
//        digit to surface a larger digit earlier.
//     2. Greedily merge the two subsequences into the lexicographically largest
//        interleaving: at each step, compare the remaining suffixes of both
//        subsequences lexicographically and take the leading digit from the
//        larger suffix (ties must look ahead so a larger future digit is
//        exposed sooner).
//     3. Keep the lexicographically largest merged result across all splits.
//
//   Time Complexity:  O(k * (m + n + k^2)) across at most k + 1 splits
//                     (O(m + n) stack extraction + O(k^2) suffix-comparing
//                     merge per split).
//   Space Complexity: O(k) for the two subsequences, merged candidate, and
//                     best answer vector.
//
// Data Structures:
//   - std::vector<int> used as a monotonic decreasing stack to compute each
//     array's optimal fixed-length subsequence in linear time.
//   - std::vector<int> for merged candidates and lexicographical comparison.
//
// Real-World Applications:
//   - Multi-stream priority merging where two ordered event queues must be
//     combined into a fixed-capacity highest-priority schedule without
//     reordering items within either queue.
//   - Financial order-book aggregation and greedy bandwidth allocation across
//     two ordered packet streams under a fixed transmission budget.
//
// Related LeetCode Problems:
//   - #402 Remove K Digits [done] (monotonic stack minimizing a single-array subsequence)
//   - #316 Remove Duplicate Letters [done] (greedy monotonic stack preserving relative order)
//   - #1081 Smallest Subsequence of Distinct Characters [new] (same monotonic stack invariant as #316)
//   - #1673 Find the Most Competitive Subsequence [new] (single-array min-subsequence counterpart)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the lexicographically largest digit sequence of length k formed
  // from nums1 and nums2 while preserving intra-array relative order.
  // Time: O(k * (m + n + k^2)), Space: O(k).
  std::vector<int> maxNumber(const std::vector<int>& nums1,
                             const std::vector<int>& nums2, int k) {
    if (k <= 0) {
      return {};
    }

    const auto target = static_cast<std::size_t>(k);
    const std::size_t m = nums1.size();
    const std::size_t n = nums2.size();

    // Valid range for the number of digits taken from nums1:
    // we need i <= m and (target - i) <= n, so target - n <= i <= m.
    const std::size_t min_take = target > n ? (target - n) : 0uz;
    const std::size_t max_take = std::min(target, m);

    std::vector<int> best;
    for (std::size_t take1 = min_take; take1 <= max_take; ++take1) {
      const std::size_t take2 = target - take1;
      const std::vector<int> sub1 = max_subsequence(nums1, take1);
      const std::vector<int> sub2 = max_subsequence(nums2, take2);
      std::vector<int> candidate = merge_subsequences(sub1, sub2);

      // Update the best result using standard lexicographical vector ordering.
      if (candidate > best) {
        best = std::move(candidate);
      }
    }

    return best;
  }

 private:
  // Extracts the lexicographically largest subsequence of exact length `count`
  // from `nums` using a greedy monotonic decreasing stack.
  // Invariant: `stack` holds the best prefix built so far, and `drop` is the
  // remaining number of elements we are still allowed to discard.
  // Time: O(nums.size()), Space: O(count).
  static std::vector<int> max_subsequence(const std::vector<int>& nums,
                                          std::size_t count) {
    if (count == 0uz) {
      return {};
    }
    if (count == nums.size()) {
      return nums;
    }

    std::size_t drop = nums.size() - count;
    std::vector<int> stack;
    stack.reserve(nums.size());

    for (const int digit : nums) {
      // Pop smaller preceding digits while our discard budget allows it so
      // larger digits move as far left as possible.
      while (drop > 0uz && !stack.empty() && stack.back() < digit) {
        stack.pop_back();
        --drop;
      }
      stack.push_back(digit);
    }

    // If `nums` was already non-increasing, trim excess trailing digits.
    stack.resize(count);
    return stack;
  }

  // Returns true iff suffix a[ia..] is lexicographically greater than or equal
  // to suffix b[ib..] (with longer suffixes winning when one is a prefix of
  // the other).
  // Time: O(min(a.size() - ia, b.size() - ib)), Space: O(1).
  static bool greater_suffix(const std::vector<int>& a, std::size_t ia,
                             const std::vector<int>& b, std::size_t ib) {
    while (ia < a.size() && ib < b.size() && a[ia] == b[ib]) {
      ++ia;
      ++ib;
    }
    // If b is exhausted, a is >= b; otherwise compare the first differing
    // digits (if a is exhausted while b is not, b wins).
    return ib == b.size() || (ia < a.size() && a[ia] > b[ib]);
  }

  // Greedily merges two digit sequences into the lexicographically largest
  // interleaved sequence of length a.size() + b.size().
  // Time: O((a.size() + b.size())^2), Space: O(a.size() + b.size()).
  static std::vector<int> merge_subsequences(const std::vector<int>& a,
                                             const std::vector<int>& b) {
    std::vector<int> merged;
    merged.reserve(a.size() + b.size());

    std::size_t ia = 0uz;
    std::size_t ib = 0uz;
    while (ia < a.size() || ib < b.size()) {
      // Choosing from the lexicographically larger remaining suffix breaks
      // ties by looking ahead so the larger subsequent digit surfaces sooner.
      if (greater_suffix(a, ia, b, ib)) {
        merged.push_back(a[ia]);
        ++ia;
      } else {
        merged.push_back(b[ib]);
        ++ib;
      }
    }

    return merged;
  }
};

LC_PROBLEM(321, "Create Maximum Number", "greedy", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    const std::vector<int> nums1 = {3, 4, 6, 5};
    const std::vector<int> nums2 = {9, 1, 2, 5, 8, 3};
    const std::vector<int> expected = {9, 8, 6, 5, 3};
    ctx.expect_eq(sol.maxNumber(nums1, nums2, 5), expected);
  });

  ctx.test("example_2_tie_breaking_lookahead", [&] {
    Solution sol;
    const std::vector<int> nums1 = {6, 7};
    const std::vector<int> nums2 = {6, 0, 4};
    const std::vector<int> expected = {6, 7, 6, 0, 4};
    ctx.expect_eq(sol.maxNumber(nums1, nums2, 5), expected);
  });

  ctx.test("example_3", [&] {
    Solution sol;
    const std::vector<int> nums1 = {3, 9};
    const std::vector<int> nums2 = {8, 9};
    const std::vector<int> expected = {9, 8, 9};
    ctx.expect_eq(sol.maxNumber(nums1, nums2, 3), expected);
  });

  ctx.test("python_build_input", [&] {
    Solution sol;
    const std::vector<int> nums1 = {8, 6, 9};
    const std::vector<int> nums2 = {1, 7, 5};
    const std::vector<int> expected = {9, 7, 5};
    ctx.expect_eq(sol.maxNumber(nums1, nums2, 3), expected);
  });

  ctx.test("prefix_tie_longer_suffix_wins", [&] {
    Solution sol;
    const std::vector<int> nums1 = {6, 7};
    const std::vector<int> nums2 = {6};
    const std::vector<int> expected = {6, 7, 6};
    ctx.expect_eq(sol.maxNumber(nums1, nums2, 3), expected);
  });

  ctx.test("one_empty_array", [&] {
    Solution sol;
    const std::vector<int> nums1 = {};
    const std::vector<int> nums2 = {5, 4, 3};
    const std::vector<int> expected = {5, 4};
    ctx.expect_eq(sol.maxNumber(nums1, nums2, 2), expected);
  });

  ctx.test("zero_length", [&] {
    Solution sol;
    const std::vector<int> nums1 = {1, 2};
    const std::vector<int> nums2 = {3, 4};
    const std::vector<int> expected = {};
    ctx.expect_eq(sol.maxNumber(nums1, nums2, 0), expected);
  });
}

}  // namespace
