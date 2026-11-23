// LeetCode 2068: Check Whether Two Strings Are Almost Equivalent
// https://leetcode.com/problems/check-whether-two-strings-are-almost-equivalent/
//
// Problem:
//   Two strings `word1` and `word2` of equal length `n` (consisting only of
//   lowercase English letters `'a'`-`'z'`) are considered *almost equivalent*
//   if the absolute difference between the frequencies of each letter `'a'`
//   through `'z'` in `word1` and `word2` is at most 3. Return `true` if they
//   are almost equivalent, and `false` otherwise.
//   Constraints: n == word1.length == word2.length, 1 <= n <= 100.
//
// Examples:
//   Example 1:
//     Input:  word1 = "aaaa", word2 = "bccb"
//     Output: false
//     Explanation:
//       - Initialize the 26-element array freq_diff to zeros and increment buckets
//         for word1 = "aaaa", setting freq_diff[0] ('a') = 4.
//       - Decrement buckets for word2 = "bccb", setting freq_diff[1] ('b') = -2 and
//         freq_diff[2] ('c') = -2 while freq_diff[0] remains 4.
//       - Evaluate std::abs(diff) <= 3 across freq_diff; since std::abs(4) > 3 at
//         index 0, std::ranges::all_of returns false.
//
//   Example 2:
//     Input:  word1 = "abcdeef", word2 = "abaaacc"
//     Output: true
//     Explanation:
//       - Add 1 per character of word1 = "abcdeef", setting buckets a:1, b:1, c:1,
//         d:1, e:2, f:1.
//       - Subtract 1 per character of word2 = "abaaacc" (which has 4 'a's, 1 'b',
//         and 2 'c's), leaving nonzero diffs a:-3, c:-1, d:1, e:2, f:1.
//       - Every bucket in freq_diff has std::abs(diff) <= 3 (maximum magnitude is 3
//         for 'a'), so std::ranges::all_of returns true.
//
//   Example 3:
//     Input:  word1 = "z", word2 = "z"
//     Output: true
//     Explanation:
//       - Increment freq_diff[25] ('z') to 1 while scanning word1 = "z", then
//         decrement freq_diff[25] back to 0 while scanning word2 = "z".
//       - All 26 entries in freq_diff equal 0 <= 3, so std::ranges::all_of returns
//         true.
//
// Algorithm:
//   Maintain a single signed frequency-difference histogram of size 26. Add 1
//   for each character in `word1` and subtract 1 for each character in
//   `word2`. Finally verify that every bucket satisfies `|diff| <= 3`.
//   Time Complexity:  O(n + sigma), where n = word1.size() and sigma = 26.
//   Space Complexity: O(sigma) = O(1) fixed-size stack array.
//
// Data Structures:
//   - `std::array<int, 26>`: fixed-size alphabet histogram holding the net
//     frequency difference `count(word1, c) - count(word2, c)` for each letter.
//
// Real-World Applications:
//   - Approximate anagram / token-distribution similarity checks in search
//     query normalization and spell-check pre-filtering.
//   - L-infinity (Chebyshev) histogram distance gating in data-drift and
//     text-fingerprinting monitors.
//   - Bioinformatic k-mer / base-composition tolerance checks over fixed
//     alphabets.
//
// Related LeetCode Problems:
//   - #242 Valid Anagram [done] (exact frequency equivalence where max allowed diff is 0)
//   - #383 Ransom Note [done] (fixed-alphabet frequency counting)
//   - #1347 Minimum Number of Steps to Make Two Strings Anagram [done] (summing positive frequency diffs)
//   - #1657 Determine if Two Strings Are Close [new] (comparing character frequency multisets)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdlib>
#include <string_view>

namespace {

class Solution {
 public:
  // Checks whether every lowercase letter's frequency differs by at most 3
  // between `word1` and `word2`.
  // Time: O(n + 26), Space: O(1).
  bool checkAlmostEquivalent(std::string_view word1, std::string_view word2) {
    // Net frequency difference: positive means more frequent in word1,
    // negative means more frequent in word2.
    std::array<int, kAlphabetSize> freq_diff{};

    for (const char ch : word1) {
      ++freq_diff[letter_index(ch)];
    }
    for (const char ch : word2) {
      --freq_diff[letter_index(ch)];
    }

    // Both strings are almost equivalent iff the L-infinity norm of the
    // frequency difference vector does not exceed 3.
    return std::ranges::all_of(
        freq_diff, [](int diff) { return std::abs(diff) <= kMaxAllowedDiff; });
  }

 private:
  static constexpr std::size_t kAlphabetSize = 26uz;
  static constexpr int kMaxAllowedDiff = 3;

  // Converts a lowercase ASCII letter 'a'..'z' to its 0..25 bucket index
  // without signed-char narrowing issues.
  static std::size_t letter_index(char ch) {
    return static_cast<std::size_t>(static_cast<unsigned char>(ch) -
                                    static_cast<unsigned char>('a'));
  }
};

LC_PROBLEM(2068, "Check Whether Two Strings Are Almost Equivalent", "math",
           ctx) {
  ctx.test("example_1_build_exceeds_threshold", [&] {
    Solution sol;
    // 'a' appears 4 times in word1 and 0 times in word2 -> diff is 4 > 3.
    ctx.expect_false(sol.checkAlmostEquivalent("aaaa", "bccb"));
  });

  ctx.test("example_2_exact_threshold_three", [&] {
    Solution sol;
    ctx.expect_true(sol.checkAlmostEquivalent("abcdeef", "abaaacc"));
  });

  ctx.test("example_3_balanced_counts", [&] {
    Solution sol;
    ctx.expect_true(sol.checkAlmostEquivalent("cccddabba", "babababab"));
  });

  ctx.test("boundary_three_vs_four", [&] {
    Solution sol;
    ctx.expect_true(sol.checkAlmostEquivalent("aaa", "bbb"));
    ctx.expect_false(sol.checkAlmostEquivalent("aaaa", "bbbb"));
  });

  ctx.test("identical_single_letter", [&] {
    Solution sol;
    ctx.expect_true(sol.checkAlmostEquivalent("z", "z"));
  });
}

}  // namespace
