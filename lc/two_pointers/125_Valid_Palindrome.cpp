// LeetCode 125: Valid Palindrome
// https://leetcode.com/problems/valid-palindrome/
//
// Problem:
//   Given a string `s`, determine whether it is a palindrome after converting
//   all uppercase ASCII letters to lowercase and removing all non-alphanumeric
//   characters. An empty string (or one with no alphanumeric characters) is
//   considered a valid palindrome.
//
// Examples:
//   Example 1:
//     Input:  s = "A man, a plan, a canal: Panama"
//     Output: true
//     Explanation:
//       - Initialize left = 0 ('A') and right = 29 ('a'); their lowercase forms
//         both equal 'a', so move inward to left = 1 and right = 28.
//       - Skip the space at left = 1 to reach 'm' at index 2 and match it with 'm'
//         at index 28, continuing inward while skipping spaces, commas, and ':'
//         across matching pairs ('a', 'n', 'a', 'p', 'l', 'a', 'n', 'a', 'c', 'a',
//         'n').
//       - The pointers meet at the central 'a' (left = 15, right = 15), exiting the
//         left < right loop and returning true.
//
//   Example 2:
//     Input:  s = "race a car"
//     Output: false
//     Explanation:
//       - Start with left = 0 and right = 9, matching 'r' (0, 9), 'a' (1, 8), and
//         'c' (2, 7) as both pointers step inward.
//       - At left = 3 ('e') and right = 6 (' '), the inner loop skips the space so
//         right moves to 5 ('a').
//       - Comparing to_lower(s[3]) ('e') against to_lower(s[5]) ('a') finds a
//         mismatch and immediately returns false.
//
//   Example 3:
//     Input:  s = "0P"
//     Output: false
//     Explanation:
//       - Initialize left = 0 ('0') and right = 1 ('P'), both of which satisfy
//         is_alnum.
//       - Compute to_lower('0') = '0' and to_lower('P') = 'p' using
//         unsigned-char-safe std::tolower instead of bitmasking.
//       - Since '0' != 'p', return false immediately.
//
// Algorithm:
//   Converging two pointers (`left` starting at 0, `right` starting at
//   `s.size() - 1`) without allocating a filtered copy of the string.
//   In each iteration:
//     1. Advance `left` rightward past non-alphanumeric characters while
//        `left < right`.
//     2. Retreat `right` leftward past non-alphanumeric characters while
//        `left < right`.
//     3. If `left >= right`, all alphanumeric pairs matched; stop.
//     4. Compare lowercase forms of `s[left]` and `s[right]`; if they differ,
//        return `false`. Otherwise move both pointers inward.
//   Characters are cast to `unsigned char` before calling `<cctype>` helpers
//   to avoid undefined behavior on negative `char` values.
//   Time Complexity:  O(n), where n = s.size(); each character is visited at
//                     most once.
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   Read-only `std::string_view` traversed in-place with two `std::size_t`
//   boundary indices.
//
// Real-World Applications:
//   - Zero-allocation canonical string normalization and comparison in parsers,
//     HTTP header/URL validators, and lexical analyzers.
//   - Bioinformatics sequence validation checking palindromic restriction
//     enzyme recognition sites in DNA/RNA sequences while ignoring annotations.
//   - Input validation and fuzzy token matching in embedded or high-throughput
//     services where heap allocations must be avoided.
//
// Related LeetCode Problems:
//   - #680 Valid Palindrome II [done] (converging two pointers allowing at most one deletion)
//   - #9 Palindrome Number [done] (palindrome check on integer digits without string allocation)
//   - #234 Palindrome Linked List [done] (palindrome check via midpoint reversal on a linked list)
//   - #5 Longest Palindromic Substring [done] (expand-around-center palindrome detection)
//   - #2002 Maximum Product of the Length of Two Palindromic Subsequences [new] (palindromic subsequence search)

#include "framework/lc_framework.h"

#include <cctype>
#include <cstddef>
#include <string_view>

namespace {

class Solution {
 public:
  // Checks whether `s` reads the same forward and backward when considering
  // only ASCII alphanumeric characters and ignoring letter case.
  //
  // Complexity: O(n) time, O(1) extra space.
  [[nodiscard]] bool isPalindrome(std::string_view s) {
    if (s.empty()) {
      return true;
    }

    std::size_t left = 0uz;
    std::size_t right = s.size() - 1uz;

    while (left < right) {
      // Skip non-alphanumeric characters from both ends toward the center.
      while (left < right && !is_alnum(s[left])) {
        ++left;
      }
      while (left < right && !is_alnum(s[right])) {
        --right;
      }

      // If pointers met or crossed while skipping punctuation, all remaining
      // alphanumeric characters have been matched.
      if (left >= right) {
        break;
      }

      if (to_lower(s[left]) != to_lower(s[right])) {
        return false;
      }

      // Since `left < right`, `right >= 1` is guaranteed so `--right` cannot
      // underflow.
      ++left;
      --right;
    }

    return true;
  }

 private:
  // Safe wrapper around `std::isalnum` that casts to `unsigned char` to avoid
  // UB on negative `char` values.
  [[nodiscard]] static bool is_alnum(char ch) {
    return std::isalnum(static_cast<unsigned char>(ch)) != 0;
  }

  // Safe wrapper around `std::tolower` that casts to `unsigned char`.
  [[nodiscard]] static char to_lower(char ch) {
    return static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
  }
};

LC_PROBLEM(125, "Valid Palindrome", "two_pointers", ctx) {
  Solution sol;

  ctx.test("leetcode_example_1_panama", [&] {
    ctx.expect_true(sol.isPalindrome("A man, a plan, a canal: Panama"));
  });

  ctx.test("leetcode_example_2_race_a_car", [&] {
    ctx.expect_false(sol.isPalindrome("race a car"));
  });

  ctx.test("empty_and_punctuation_only", [&] {
    ctx.expect_true(sol.isPalindrome(""));
    ctx.expect_true(sol.isPalindrome(" "));
    ctx.expect_true(sol.isPalindrome(".,!@#$%^&*()"));
  });

  ctx.test("single_alnum_with_punctuation", [&] {
    ctx.expect_true(sol.isPalindrome("a"));
    ctx.expect_true(sol.isPalindrome("a."));
    ctx.expect_true(sol.isPalindrome(".a"));
    ctx.expect_true(sol.isPalindrome("...Z..."));
  });

  ctx.test("digits_and_ascii_pitfall", [&] {
    // '0' (48) and 'P' (80) differ by 32 in ASCII, catching naive bitmask case
    // folding bugs.
    ctx.expect_false(sol.isPalindrome("0P"));
    ctx.expect_true(sol.isPalindrome("1a2b2a1"));
    ctx.expect_false(sol.isPalindrome("1a2"));
  });
}

}  // namespace
