// LeetCode 823: Binary Trees With Factors
// https://leetcode.com/problems/binary-trees-with-factors/
//
// Problem:
//   Given an array `arr` of unique integers where every `arr[i] > 1`, count
//   how many binary trees can be constructed using elements of `arr` (each
//   value may be reused arbitrarily many times) such that every non-leaf
//   node's value equals the product of its left and right children's values.
//   Return the total number of valid binary trees modulo 10^9 + 7.
//   Constraints: 1 <= arr.length <= 1000, 2 <= arr[i] <= 10^9, all unique.
//
// Examples:
//   Example 1:
//     Input:  arr = [2, 4, 5, 10]
//     Output: 7
//     Explanation:
//       - Sort arr to [2, 4, 5, 10] and compute dp[2] = 1 (the single-node tree
//         [2]).
//       - For root = 4, divisor left = 2 has complementary factor right = 4 / 2 = 2
//         in dp, giving dp[4] = 1 + dp[2] * dp[2] = 2.
//       - For root = 5, no smaller element divides 5, so dp[5] = 1.
//       - For root = 10, pairs (left = 2, right = 5) and (left = 5, right = 2)
//         yield dp[10] = 1 + (1 * 1) + (1 * 1) = 3, and total = 1 + 2 + 1 + 3 = 7.
//
//   Example 2:
//     Input:  arr = [18, 3, 6, 2]
//     Output: 12
//     Explanation:
//       - Sort arr ascending to [2, 3, 6, 18] so every factor is solved before its
//         multiples.
//       - Set dp[2] = 1 and dp[3] = 1, then for root = 6 combine ordered pairs (2,
//         3) and (3, 2) to get dp[6] = 1 + 1 * 1 + 1 * 1 = 3.
//       - For root = 18, left = 2 has right = 9 (absent from dp), while (3, 6) and
//         (6, 3) each add 1 * 3 = 3, giving dp[18] = 1 + 3 + 3 = 7 and total = 1 +
//         1 + 3 + 7 = 12.
//
//   Example 3:
//     Input:  arr = [2, 3, 5, 7, 11]
//     Output: 5
//     Explanation:
//       - Keep the already-sorted array [2, 3, 5, 7, 11], where every element is
//         prime and root % left != 0 holds for all j < i.
//       - Assign dp[root] = 1 for each of the 5 values and sum them to return total
//         = 5.
//
// Algorithm:
//   Because every value is strictly greater than 1, any child in a valid tree
//   must have a strictly smaller value than its parent (`left * right = root`
//   implies `left < root` and `right < root`). This induces a DAG ordering:
//     1. Sort `arr` in ascending order so all potential factors of `root` are
//        solved before `root` itself.
//     2. Define `dp[root]` = number of valid binary trees rooted at `root`
//        (modulo 10^9 + 7). Initialize `dp[root] = 1` for the single-node tree.
//     3. For each smaller candidate `left = arr[j]` (`0 <= j < i`):
//          If `root % left == 0`, let `right = root / left`. If `right` exists
//          in `dp`, add `(dp[left] * dp[right]) % kMod` to `dp[root]`.
//        Note that iterating `left` across all smaller values naturally counts
//        ordered child pairs `(left, right)` and `(right, left)` separately
//        when `left != right`, and once when `left == right`.
//     4. Sum `dp[root]` across all `root` modulo 10^9 + 7.
//   Time Complexity: O(N^2) where N = arr.size() (sorting O(N log N) + nested
//   divisor scan with O(1) average hash-map lookups).
//   Space Complexity: O(N) for the sorted copy and DP hash map.
//
// Data Structures:
//   - Sorted array (`std::vector<int>`): establishes bottom-up DP topological
//     order over factor relationships.
//   - Hash map (`std::unordered_map<int, std::int64_t>`): maps each node value
//     to the number of valid subtrees rooted at that value for O(1) lookup of
//     the complementary factor `root / left`.
//
// Real-World Applications:
//   - Combinatorial counting of expression trees and factorization trees in
//     symbolic algebra systems and query-plan enumeration (join-tree counts).
//   - Dynamic programming over divisor lattices in number-theoretic
//     cryptography and integer factorization analysis.
//   - Counting valid hierarchical decompositions of multiplicative resource
//     budgets or signal-processing FFT factorization schedules.
//
// Related LeetCode Problems:
//   - #96 Unique Binary Search Trees [done] (Catalan-style bottom-up tree counting DP)
//   - #368 Largest Divisible Subset [done] (sorting + divisor-based DP over an array)
//   - #894 All Possible Full Binary Trees [new] (recursive/DP enumeration of full binary trees)
//   - #139 Word Break [done] (bottom-up DP decomposing targets into valid subparts)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Counts the total number of factored binary trees modulo 10^9 + 7.
  // Time: O(N^2), Space: O(N).
  int numFactoredBinaryTrees(const std::vector<int>& arr) {
    // Sort ascending so every valid child factor (< root) is computed first.
    std::vector<int> sorted = arr;
    std::ranges::sort(sorted);

    // dp[v] stores the number of valid binary trees rooted at value `v` (mod).
    std::unordered_map<int, std::int64_t> dp;
    dp.reserve(sorted.size());

    std::int64_t total = 0;

    for (std::size_t i = 0uz; i < sorted.size(); ++i) {
      const int root = sorted[i];
      // Every value forms at least one trivial single-node tree by itself.
      std::int64_t ways = 1;

      for (std::size_t j = 0uz; j < i; ++j) {
        const int left = sorted[j];
        if (root % left != 0) {
          continue;
        }

        // Check if the complementary right child value exists in the array.
        const int right = root / left;
        const auto right_it = dp.find(right);
        if (right_it != dp.end()) {
          const std::int64_t left_ways = dp[left];
          // Both factors are < kMod (~10^9), so their product fits in int64_t.
          const std::int64_t pair_ways = (left_ways * right_it->second) % kMod;
          ways = (ways + pair_ways) % kMod;
        }
      }

      dp[root] = ways;
      total = (total + ways) % kMod;
    }

    return static_cast<int>(total);
  }

 private:
  static constexpr std::int64_t kMod = 1'000'000'007;
};

LC_PROBLEM(823, "Binary Trees With Factors", "hash", ctx) {
  Solution sol;

  ctx.test("leetcode_example_1_from_build", [&] {
    // Valid trees: [2], [4], [4, 2, 2].
    ctx.expect_eq(sol.numFactoredBinaryTrees({2, 4}), 3);
  });

  ctx.test("leetcode_example_2", [&] {
    // Valid trees: [2], [4], [5], [10], [4,2,2], [10,2,5], [10,5,2].
    ctx.expect_eq(sol.numFactoredBinaryTrees({2, 4, 5, 10}), 7);
  });

  ctx.test("unsorted_input", [&] {
    ctx.expect_eq(sol.numFactoredBinaryTrees({10, 5, 4, 2}), 7);
    // 2->1, 3->1, 6->3, 18->1 + 2*(1*3) = 7 => total 12.
    ctx.expect_eq(sol.numFactoredBinaryTrees({18, 3, 6, 2}), 12);
  });

  ctx.test("powers_of_two_chain", [&] {
    // dp[2]=1, dp[4]=2, dp[8]=5, dp[16]=15 => total 23.
    ctx.expect_eq(sol.numFactoredBinaryTrees({2, 4, 8, 16}), 23);
    // dp[32]=1 + 2*(1*15) + 2*(2*5) = 51 => total 74.
    ctx.expect_eq(sol.numFactoredBinaryTrees({2, 4, 8, 16, 32}), 74);
  });

  ctx.test("single_and_all_primes", [&] {
    ctx.expect_eq(sol.numFactoredBinaryTrees({2}), 1);
    ctx.expect_eq(sol.numFactoredBinaryTrees({2, 3, 5, 7, 11}), 5);
  });
}

}  // namespace
