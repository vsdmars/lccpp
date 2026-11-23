// LeetCode 1062: Longest Repeating Substring
// https://leetcode.com/problems/longest-repeating-substring/
//
// Problem:
//   Given a string `s` of lowercase English letters (1 <= s.length <= 1500),
//   return the length of the longest substring that appears at least twice in
//   `s` (occurrences may overlap). Return 0 if no repeating substring exists.
//
// Examples:
//   Example 1:
//     Input:  s = "abbaba"
//     Output: 2
//     Explanation:
//       - Initialize max_len = 0 and sweep offset = 1 across s[0..4] vs s[1..5],
//         where only s[1] == s[2] ('b') matches for same_cnt = 1 and max_len = 1.
//       - At offset = 2, compare s[0..3] against s[2..5]: s[2] == s[4] ('b') sets
//         same_cnt = 1 and s[3] == s[5] ('a') increments same_cnt = 2, updating
//         max_len = 2 for substring "ba".
//       - At offset = 3 (n - offset = 3 > 2), s[0] == s[3] ('a') and s[1] == s[4]
//         ('b') again reach same_cnt = 2 ("ab") before s[2] != s[5] resets same_cnt
//         = 0.
//       - At offset = 4, remaining diagonal length n - offset = 2 is no longer
//         greater than max_len = 2, so the loop prunes early and returns 2.
//
//   Example 2:
//     Input:  s = "aaaaa"
//     Output: 4
//     Explanation:
//       - Start at offset = 1 (limit = 4) and compare s[i] with s[i + 1] for i =
//         0..3.
//       - Every pair compares 'a' == 'a', incrementing same_cnt through 1, 2, 3,
//         and 4 and updating max_len = 4 for the overlapping substring "aaaa" at
//         [0..3] and [1..4].
//       - Before offset = 2 starts, n - offset = 3 <= max_len = 4, so the outer
//         loop terminates immediately via early pruning and returns 4.
//
//   Example 3:
//     Input:  s = "abcd"
//     Output: 0
//     Explanation:
//       - At offset = 1, compare adjacent pairs ('a','b'), ('b','c'), and
//         ('c','d'); all mismatch, keeping same_cnt = 0 and max_len = 0.
//       - At offset = 2, compare ('a','c') and ('b','d'); both mismatch, keeping
//         max_len = 0.
//       - At offset = 3, compare ('a','d'); it mismatches, ending the sweep with
//         max_len = 0.
//
// Algorithm:
//   1. Diagonal Shifted Comparison (primary, matching the Python solution):
//      In the classic Longest Common Substring DP table where
//      `dp[i][j]` is the length of the common suffix ending at `s[i]` and
//      `s[j]` (`i < j`), each state depends only on `dp[i - 1][j - 1]`.
//      Sweeping along each diagonal `offset = j - i` from `1` to `n - 1`
//      compares `s[i]` against `s[i + offset]` and maintains a single scalar
//      run-length counter `same_cnt`. Early pruning stops as soon as the
//      remaining diagonal length `n - offset` cannot exceed `max_len`.
//      Time Complexity:  O(N^2) worst case, with early pruning on long repeats.
//      Space Complexity: O(1).
//   2. Binary Search + Double-Hashed Rabin-Karp (secondary O(N log N) method):
//      Monotonicity holds: if a repeating substring of length `L` exists, then
//      one exists for every length `< L`. Binary search on `L` in `[1, N - 1]`
//      using a 64-bit composite double rolling hash (two distinct prime moduli
//      packed into one `uint64_t`) to detect duplicate length-`L` windows in
//      O(N) expected time per probe.
//      Time Complexity:  O(N log N) expected.
//      Space Complexity: O(N).
//
// Data Structures:
//   - Scalar run-length counter (O(1) space) for the diagonal sweep.
//   - std::unordered_set<std::uint64_t>: stores packed double rolling hashes
//     of seen substrings of length `L` during the binary-search check.
//
// Real-World Applications:
//   - Data deduplication and LZ77/LZSS dictionary compression finding longest
//     repeated byte sequences in sliding windows.
//   - Bioinformatics repeat discovery in DNA/protein sequences.
//   - Code-clone detection and plagiarism scanning in source repositories.
//
// Related LeetCode Problems:
//   - #1044 Longest Duplicate Substring [done] (harder variant requiring the substring itself, N <= 3e4)
//   - #718 Maximum Length of Repeated Subarray [new] (longest common subarray between two sequences)
//   - #187 Repeated DNA Sequences [new] (fixed-length rolling hash repeat detection)
//   - #1316 Distinct Echo Substrings [new] (rolling-hash repeated contiguous halves)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <unordered_set>

namespace {

constexpr std::uint64_t kHashBase1 = 131;
constexpr std::uint64_t kHashMod1 = 1'000'000'007;
constexpr std::uint64_t kHashBase2 = 137;
constexpr std::uint64_t kHashMod2 = 1'000'000'009;

class Solution {
 public:
  // Finds the length of the longest repeating substring via space-optimized
  // diagonal DP (comparing `s` against shifted copies of itself).
  //
  // Time Complexity:  O(N^2)
  // Space Complexity: O(1)
  int longestRepeatingSubstring(std::string_view s) {
    const std::size_t n = s.size();
    std::size_t max_len = 0uz;

    // `offset` is the non-zero distance between two candidate start positions.
    // A diagonal of shift `offset` has length `n - offset`, so once
    // `n - offset <= max_len` no longer repeat can be discovered.
    for (std::size_t offset = 1uz; offset < n && (n - offset) > max_len;
         ++offset) {
      std::size_t same_cnt = 0uz;
      const std::size_t limit = n - offset;
      for (std::size_t i = 0uz; i < limit; ++i) {
        if (s[i] == s[i + offset]) {
          ++same_cnt;
          max_len = std::max(max_len, same_cnt);
        } else {
          same_cnt = 0uz;
        }
      }
    }

    return static_cast<int>(max_len);
  }

  // Alternative O(N log N) solution using binary search on answer length
  // paired with a double-modulus Rabin-Karp rolling hash.
  //
  // Time Complexity:  O(N log N) expected
  // Space Complexity: O(N)
  int longestRepeatingSubstringRollingHash(std::string_view s) {
    const std::size_t n = s.size();
    if (n < 2uz) {
      return 0;
    }

    std::size_t lo = 1uz;
    std::size_t hi = n - 1uz;
    std::size_t best = 0uz;

    // Binary search the maximum length `mid` for which a duplicate exists.
    while (lo <= hi) {
      const std::size_t mid = lo + ((hi - lo) / 2uz);
      if (has_duplicate_of_length(s, mid)) {
        best = mid;
        lo = mid + 1uz;
      } else {
        hi = mid - 1uz;
      }
    }

    return static_cast<int>(best);
  }

 private:
  // Packs two 30-bit modular hashes into a single 64-bit hash key.
  static std::uint64_t pack_hashes(std::uint64_t h1, std::uint64_t h2) {
    return (h1 << 32U) | h2;
  }

  // Returns true if `s` contains at least two occurrences of some substring of
  // exact length `len`, using a double-hashed Rabin-Karp sliding window.
  static bool has_duplicate_of_length(std::string_view s, std::size_t len) {
    const std::size_t n = s.size();
    std::uint64_t h1 = 0;
    std::uint64_t h2 = 0;
    std::uint64_t pow1 = 1;
    std::uint64_t pow2 = 1;

    // Compute hashes for the initial window s[0..len-1] and base^len mod M.
    for (std::size_t i = 0uz; i < len; ++i) {
      const auto val = static_cast<std::uint64_t>(s[i] - 'a') + 1;
      h1 = ((h1 * kHashBase1) + val) % kHashMod1;
      h2 = ((h2 * kHashBase2) + val) % kHashMod2;
      pow1 = (pow1 * kHashBase1) % kHashMod1;
      pow2 = (pow2 * kHashBase2) % kHashMod2;
    }

    std::unordered_set<std::uint64_t> seen;
    seen.reserve((n - len) + 1uz);
    seen.insert(pack_hashes(h1, h2));

    // Roll the window one character at a time across s[len..n-1].
    for (std::size_t i = len; i < n; ++i) {
      const auto add_val = static_cast<std::uint64_t>(s[i] - 'a') + 1;
      const auto rem_val = static_cast<std::uint64_t>(s[i - len] - 'a') + 1;

      h1 = ((h1 * kHashBase1) + add_val) % kHashMod1;
      h1 = (h1 + kHashMod1 - ((rem_val * pow1) % kHashMod1)) % kHashMod1;

      h2 = ((h2 * kHashBase2) + add_val) % kHashMod2;
      h2 = (h2 + kHashMod2 - ((rem_val * pow2) % kHashMod2)) % kHashMod2;

      if (!seen.insert(pack_hashes(h1, h2)).second) {
        return true;
      }
    }

    return false;
  }
};

LC_PROBLEM(1062, "Longest Repeating Substring", "string", ctx) {
  auto verify_both = [&](std::string_view input, int expected) {
    Solution sol;
    ctx.expect_eq(sol.longestRepeatingSubstring(input), expected);
    ctx.expect_eq(sol.longestRepeatingSubstringRollingHash(input), expected);
  };

  ctx.test("example_1_no_repeat", [&] { verify_both("abcd", 0); });

  ctx.test("example_2_length_two", [&] { verify_both("abbaba", 2); });

  ctx.test("example_3_triple_occurrence",
           [&] { verify_both("aabcaabdaab", 3); });

  ctx.test("example_4_all_same_overlapping", [&] { verify_both("aaaaa", 4); });

  ctx.test("edge_cases", [&] {
    verify_both("", 0);
    verify_both("a", 0);
    verify_both("zz", 1);
    verify_both("abcxyzabc", 3);
    verify_both("abababa", 5);
  });
}

}  // namespace
