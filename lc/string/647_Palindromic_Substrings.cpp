// LeetCode 647: Palindromic Substrings
// https://leetcode.com/problems/palindromic-substrings/
//
// Problem:
//   Given a string `s`, return the total number of palindromic substrings in
//   `s`. Substrings with different start or end indices are counted as
//   distinct even if they contain identical characters.
//   Constraints: 1 <= s.size() <= 1000, lowercase English letters.
//
// Algorithm:
//   1. Expand-Around-Center (`countSubstrings`, primary):
//      Every palindrome expands symmetrically from one of `2 * N - 1` possible
//      centers: `N` single-character centers (odd-length palindromes) and
//      `N - 1` adjacent character gaps (even-length palindromes). For each
//      center `pivot` in `[0, 2 * N - 1)`, start with `left = pivot / 2` and
//      `right = left + (pivot % 2)`, expanding outward while `s[left] ==
//      s[right]` and counting every valid expansion step.
//      Time Complexity: O(N^2), Space Complexity: O(1).
//   2. Manacher's Algorithm (`countSubstringsManacher`, linear-time variant):
//      Insert `#` separators between characters so odd and even palindromes
//      share a uniform odd-length representation `t` of length `2 * N + 1`.
//      Track the rightmost palindrome `[center - r, center + r]` to initialize
//      each new center's radius from its mirror `2 * center - i` in O(1)
//      amortized time. Each center `i` with radius `radius[i]` contributes
//      `(radius[i] + 1) / 2` palindromic substrings in `s`.
//      Time Complexity: O(N), Space Complexity: O(N).
//
// Data Structures:
//   - Two index pointers (`left`, `right`) for O(1)-space center expansion.
//   - Transformed `std::string` and `std::vector<std::size_t>` radius table
//     for the O(N) Manacher's variant.
//
// Real-World Applications:
//   - Bioinformatics: detecting inverted repeats and palindromic restriction
//     enzyme recognition sites in DNA/RNA sequences.
//   - Data compression and stringology: palindrome trees (Eertree) and
//     symmetry analysis in text indexing.
//   - Signal processing and structural pattern matching where mirror-symmetric
//     subsequences signify folding or resonance motifs.
//
// Related LeetCode Problems:
//   - #5 Longest Palindromic Substring [done] (same center-expansion / Manacher technique tracking max length)
//   - #516 Longest Palindromic Subsequence [done] (non-contiguous palindromic DP on subintervals)
//   - #131 Palindrome Partitioning [done] (backtracking partitions into palindromic substrings)
//   - #680 Valid Palindrome II [done] (two-pointer palindrome check allowing at most one deletion)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Counts all palindromic substrings via expand-around-center across all
  // 2 * N - 1 odd and even centers.
  // Time: O(N^2), Space: O(1).
  int countSubstrings(std::string_view s) {
    if (s.empty()) {
      return 0;
    }

    const std::size_t num_centers = (2uz * s.size()) - 1uz;
    int total = 0;
    for (std::size_t pivot = 0uz; pivot < num_centers; ++pivot) {
      // Even pivot -> single-char center (left == right);
      // Odd pivot  -> between-char center (right == left + 1).
      const std::size_t left = pivot / 2uz;
      const std::size_t right = left + (pivot % 2uz);
      total += expand_around_center(s, left, right);
    }
    return total;
  }

  // Counts all palindromic substrings in linear time using Manacher's
  // algorithm on a separator-interleaved view of `s`.
  // Time: O(N), Space: O(N).
  int countSubstringsManacher(std::string_view s) {
    if (s.empty()) {
      return 0;
    }

    // Build "#c0#c1#...#cn-1#"; real characters sit at odd indices and
    // separators at even indices, so symmetry comparisons always pair same
    // parity indices (even with even, odd with odd).
    std::string transformed;
    transformed.reserve((2uz * s.size()) + 1uz);
    transformed.push_back('#');
    for (const char c : s) {
      transformed.push_back(c);
      transformed.push_back('#');
    }

    const std::size_t m = transformed.size();
    std::vector<std::size_t> radius(m, 0uz);
    std::size_t center = 0uz;
    std::size_t right_bound = 0uz;
    int total = 0;

    for (std::size_t i = 0uz; i < m; ++i) {
      // Reuse the mirror palindrome radius when `i` lies inside the rightmost
      // known palindrome [2*center - right_bound, right_bound].
      if (i < right_bound) {
        const std::size_t mirror = (2uz * center) - i;
        radius[i] = std::min(right_bound - i, radius[mirror]);
      }

      // Attempt to expand beyond the inherited radius.
      while (i > radius[i] && i + radius[i] + 1uz < m &&
             transformed[i + radius[i] + 1uz] ==
                 transformed[i - radius[i] - 1uz]) {
        ++radius[i];
      }

      // Update rightmost palindrome boundary if this expansion reached further.
      if (i + radius[i] > right_bound) {
        center = i;
        right_bound = i + radius[i];
      }

      // A transformed palindrome of radius r corresponds to ceil(r / 2)
      // palindromic substrings centered at this position in the original `s`.
      total += static_cast<int>((radius[i] + 1uz) / 2uz);
    }

    return total;
  }

 private:
  // Expands symmetrically outward from `[left, right]` and counts every valid
  // palindromic substring encountered.
  static int expand_around_center(std::string_view s, std::size_t left,
                                  std::size_t right) {
    int count = 0;
    while (right < s.size() && s[left] == s[right]) {
      ++count;
      // Guard before decrementing unsigned `left` past index 0.
      if (left == 0uz) {
        break;
      }
      --left;
      ++right;
    }
    return count;
  }
};

LC_PROBLEM(647, "Palindromic Substrings", "string", ctx) {
  ctx.test("example_1_all_distinct", [&] {
    Solution sol;
    ctx.expect_eq(sol.countSubstrings("abc"), 3);
    ctx.expect_eq(sol.countSubstringsManacher("abc"), 3);
  });

  ctx.test("example_2_all_equal", [&] {
    Solution sol;
    ctx.expect_eq(sol.countSubstrings("aaa"), 6);
    ctx.expect_eq(sol.countSubstringsManacher("aaa"), 6);
  });

  ctx.test("python_build_input", [&] {
    Solution sol;
    ctx.expect_eq(sol.countSubstrings("laksjdkalshfa"), 13);
    ctx.expect_eq(sol.countSubstringsManacher("laksjdkalshfa"), 13);
  });

  ctx.test("even_and_odd_mixed_palindromes", [&] {
    Solution sol;
    ctx.expect_eq(sol.countSubstrings("abba"), 6);
    ctx.expect_eq(sol.countSubstringsManacher("abba"), 6);
    ctx.expect_eq(sol.countSubstrings("aba"), 4);
    ctx.expect_eq(sol.countSubstringsManacher("aba"), 4);
    ctx.expect_eq(sol.countSubstrings("fdsklf"), 6);
    ctx.expect_eq(sol.countSubstringsManacher("fdsklf"), 6);
  });

  ctx.test("empty_and_single_char", [&] {
    Solution sol;
    ctx.expect_eq(sol.countSubstrings(""), 0);
    ctx.expect_eq(sol.countSubstringsManacher(""), 0);
    ctx.expect_eq(sol.countSubstrings("z"), 1);
    ctx.expect_eq(sol.countSubstringsManacher("z"), 1);
  });
}

}  // namespace
