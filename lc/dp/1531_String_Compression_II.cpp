// LeetCode 1531: String Compression II
// https://leetcode.com/problems/string-compression-ii/
//
// Problem:
//   Run-length encoding (RLE) compresses consecutive identical characters of
//   length >= 2 into `<char><count>` (e.g., `"aabccc"` -> `"a2bc3"`, omitting
//   `'1'` for singletons). Given a lowercase English string `s` (length
//   1..100) and an integer `k` (`0 <= k <= s.length`), return the minimum
//   possible length of the RLE string after deleting at most `k` characters
//   from `s`.
//
// Algorithm:
//   Bottom-up 2D dynamic programming over suffix index and remaining deletion
//   budget. Rather than tracking `(idx, prev_char, prev_cnt, rem_k)` as a 4D
//   state, we group an entire kept run of `s[i]` in a single transition:
//   Let `dp[i][j]` be the minimum RLE length for suffix `s[i..n-1]` using at
//   most `j` deletions, with base case `dp[n][j] = 0`.
//   For each `i` from `n - 1` down to `0` and each budget `j` from `0` to `k`:
//     1. Delete `s[i]` (when `j > 0`): `dp[i][j] = dp[i + 1][j - 1]`.
//     2. Keep `s[i]` as the start of a contiguous run of `s[i]` ending at some
//        `end >= i`: scan `end` from `i` to `n - 1`, counting `same` matches of
//        `s[i]` and `diff` intervening non-`s[i]` characters that must be
//        deleted to merge those matches. While `diff <= j`, relax:
//          `dp[i][j] = min(dp[i][j], encodedLength(same) + dp[end + 1][j - diff])`
//        Deleting an interior `s[i]` is never needed because deleting an outer
//        `s[i]` achieves the same run length without increasing `diff`.
//   Time Complexity:  O(n^2 * k), where `n = s.size()`.
//   Space Complexity: O(n * k) for the 2D DP table.
//
// Data Structures:
//   - 2D DP table (`std::vector<std::vector<int>>` of size `(n + 1) x (k + 1)`)
//     storing minimum compressed suffix lengths for each deletion budget.
//
// Real-World Applications:
//   - Lossy run-length / delta compression for telemetry, bitmap graphics, and
//     time-series streams where a bounded noise-dropping budget yields drastic
//     storage savings.
//   - Optimal packet/segment coalescing in network log compaction and columnar
//     database RLE block encoding.
//
// Related LeetCode Problems:
//   - #443 String Compression [done] (in-place run-length encoding without deletions)
//   - #38 Count and Say [done] (iterative run-length encoding transformation)
//   - #664 Strange Printer [new] (interval DP merging identical-character runs)
//   - #1320 Minimum Distance To Type A Word Using Two Fingers [done] (DP state-space reduction)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <string>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the minimum run-length encoded length of `s` after deleting at
  // most `k` characters using bottom-up 2D run-grouping DP.
  // Time: O(n^2 * k), Space: O(n * k).
  int getLengthOfOptimalCompression(std::string_view s, int k) {
    const std::size_t n = s.size();
    const auto budget = static_cast<std::size_t>(k);
    if (budget >= n) {
      return 0;
    }

    // `dp[i][j]` is the minimum compressed length of `s[i..n)` with at most `j`
    // deletions. Empty suffix `s[n..n)` always compresses to length 0.
    std::vector<std::vector<int>> dp(n + 1uz,
                                     std::vector<int>(budget + 1uz, kInf));
    for (std::size_t j = 0uz; j <= budget; ++j) {
      dp[n][j] = 0;
    }

    for (std::size_t pos = n; pos > 0uz; --pos) {
      const std::size_t i = pos - 1uz;
      for (std::size_t j = 0uz; j <= budget; ++j) {
        // Option 1: delete `s[i]` if deletion budget remains.
        int best = (j > 0uz) ? dp[i + 1uz][j - 1uz] : kInf;

        // Option 2: keep `s[i]` and greedily merge any prefix of future
        // occurrences of `s[i]` by deleting the intervening mismatched chars.
        best = std::min(best, bestKeepRun(s, i, j, dp));
        dp[i][j] = best;
      }
    }

    return dp[0uz][budget];
  }

 private:
  static constexpr int kInf = std::numeric_limits<int>::max() / 2;

  // Returns the encoded length of a run of `count` identical characters
  // (`1 <= count <= 100`): `"a"` -> 1, `"a2".."a9"` -> 2, `"a10".."a99"` -> 3,
  // `"a100"` -> 4.
  static int encodedLength(int count) {
    if (count <= 1) {
      return count;
    }
    if (count < 10) {
      return 2;
    }
    if (count < 100) {
      return 3;
    }
    return 4;
  }

  // Evaluates all runs starting at `start` composed of character `s[start]`,
  // deleting intervening different characters within budget `budget`.
  static int bestKeepRun(std::string_view s, std::size_t start,
                         std::size_t budget,
                         const std::vector<std::vector<int>>& dp) {
    const std::size_t n = s.size();
    int best = kInf;
    int same = 0;
    std::size_t diff = 0uz;

    for (std::size_t end = start; end < n; ++end) {
      if (s[end] == s[start]) {
        ++same;
      } else {
        ++diff;
        if (diff > budget) {
          // Exceeded remaining deletion budget; no longer possible to extend
          // this run of `s[start]`.
          break;
        }
      }
      best = std::min(best, encodedLength(same) + dp[end + 1uz][budget - diff]);
    }

    return best;
  }
};

LC_PROBLEM(1531, "String Compression II", "dp", ctx) {
  Solution sol;

  ctx.test("example_1_python_build", [&] {
    // Delete 'b' and 'd' -> "a3c3" (length 4).
    ctx.expect_eq(sol.getLengthOfOptimalCompression("aaabcccd", 2), 4);
  });

  ctx.test("example_2_merge_runs_across_deletion", [&] {
    // Delete both 'b's -> "a4" (length 2).
    ctx.expect_eq(sol.getLengthOfOptimalCompression("aabbaa", 2), 2);
  });

  ctx.test("example_3_no_deletions_two_digit_count", [&] {
    // "a11" -> length 3.
    ctx.expect_eq(sol.getLengthOfOptimalCompression("aaaaaaaaaaa", 0), 3);
  });

  ctx.test("digit_boundary_reductions", [&] {
    // 10 'a's -> "a10" (len 3); deleting 1 gives "a9" (len 2).
    ctx.expect_eq(sol.getLengthOfOptimalCompression("aaaaaaaaaa", 0), 3);
    ctx.expect_eq(sol.getLengthOfOptimalCompression("aaaaaaaaaa", 1), 2);
    // 100 'a's -> "a100" (len 4); deleting 1 gives "a99" (len 3).
    const std::string hundred_a(100uz, 'a');
    ctx.expect_eq(sol.getLengthOfOptimalCompression(hundred_a, 0), 4);
    ctx.expect_eq(sol.getLengthOfOptimalCompression(hundred_a, 1), 3);
  });

  ctx.test("edge_cases_delete_all_and_single_char", [&] {
    ctx.expect_eq(sol.getLengthOfOptimalCompression("a", 0), 1);
    ctx.expect_eq(sol.getLengthOfOptimalCompression("a", 1), 0);
    ctx.expect_eq(sol.getLengthOfOptimalCompression("abcdef", 6), 0);
    // Delete 'b' and 1 'a' from "aaaaabaaaaa" -> "a9" (length 2).
    ctx.expect_eq(sol.getLengthOfOptimalCompression("aaaaabaaaaa", 2), 2);
  });
}

}  // namespace
