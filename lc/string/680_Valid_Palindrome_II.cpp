// LeetCode 680: Valid Palindrome II
// https://leetcode.com/problems/valid-palindrome-ii/
//
// Problem:
//   Given a non-empty string `s`, return `true` if `s` can become a palindrome
//   after deleting at most one character from it, and `false` otherwise.
//   Constraints: 1 <= s.size() <= 10^5, lowercase English letters.
//
// Examples:
//   Example 1:
//     Input:  s = "abca"
//     Output: true
//     Explanation:
//       - Initialize left = 0 ('a') and right = 3 ('a'); since s[0] == s[3],
//         advance inward to left = 1 and right = 2.
//       - Detect mismatch s[1] = 'b' != s[2] = 'c', so check whether s[2..2] or
//         s[1..1] is a strict palindrome.
//       - Call is_palindrome(s, 2, 2), which exits while (left < right) immediately
//         and returns true.
//
//   Example 2:
//     Input:  s = "abc"
//     Output: false
//     Explanation:
//       - Initialize left = 0 ('a') and right = 2 ('c'), which immediately mismatch
//         (s[0] != s[2]).
//       - Check is_palindrome(s, 1, 2) for "bc", which finds 'b' != 'c' and returns
//         false.
//       - Check is_palindrome(s, 0, 1) for "ab", which finds 'a' != 'b' and returns
//         false, so return false.
//
//   Example 3:
//     Input:  s = "ab"
//     Output: true
//     Explanation:
//       - Initialize left = 0 ('a') and right = 1 ('b'), which mismatch at the
//         first comparison.
//       - Check is_palindrome(s, 1, 1); because left < right is false, the helper
//         returns true and validPalindrome returns true.
//
// Algorithm:
//   Greedy two-pointer scan with one-shot branch on first mismatch:
//   1. Place `left = 0` and `right = s.size() - 1` and advance both inward
//      while `s[left] == s[right]`.
//   2. If the pointers meet or cross without mismatch, `s` is already a
//      palindrome (0 deletions needed) -> return `true`.
//   3. At the first mismatch `s[left] != s[right]`, any valid solution using
//      at most one deletion MUST delete either `s[left]` or `s[right]`
//      (deleting any interior character would leave `s[left]` and `s[right]`
//      still mismatched). Therefore, test whether either remaining closed
//      subrange `[left + 1, right]` or `[left, right - 1]` is strictly a
//      palindrome using the same O(1)-space two-pointer helper.
//   Time Complexity: O(N), where N = s.size() (each character is examined at
//   most twice across the outer scan and at most two inner checks).
//   Space Complexity: O(1) (unlike slice-and-reverse implementations that
//   allocate O(N) temporary copies).
//
// Data Structures:
//   - Two index pointers (`left`, `right`) operating directly on a
//     `std::string_view` with zero heap allocation.
//
// Real-World Applications:
//   - Fuzzy string matching and DNA motif alignment tolerating single-base
//     indel sequencing errors in palindromic recognition sites.
//   - Typo-tolerant verification of symmetric identifiers, checksums, or
//     mirror-encoded barcodes.
//   - Lightweight edit-distance-1 symmetry filters in search/indexing pipelines.
//
// Related LeetCode Problems:
//   - #125 Valid Palindrome [done] (standard two-pointer palindrome check with alphanumeric filtering)
//   - #647 Palindromic Substrings [done] (counting all palindromic substrings via center expansion)
//   - #1216 Valid Palindrome III [done] (generalization allowing at most k deletions via DP)
//   - #2330 Valid Palindrome IV [new] (palindrome validity allowing at most two character replacements)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string_view>

namespace {

class Solution {
 public:
  // Returns true iff `s` can be turned into a palindrome by deleting at most
  // one character.
  // Time: O(N), Space: O(1).
  bool validPalindrome(std::string_view s) {
    if (s.size() <= 1uz) {
      return true;
    }

    std::size_t left = 0uz;
    std::size_t right = s.size() - 1uz;

    while (left < right) {
      if (s[left] != s[right]) {
        // One of the two mismatched endpoints must be deleted; the remaining
        // substring must then be a strict palindrome with zero further edits.
        return is_palindrome(s, left + 1uz, right) ||
               is_palindrome(s, left, right - 1uz);
      }
      ++left;
      --right;
    }

    return true;
  }

 private:
  // Checks whether the inclusive slice `s[left..right]` is a strict palindrome.
  static bool is_palindrome(std::string_view s, std::size_t left,
                            std::size_t right) {
    while (left < right) {
      if (s[left] != s[right]) {
        return false;
      }
      ++left;
      --right;
    }
    return true;
  }
};

LC_PROBLEM(680, "Valid Palindrome II", "string", ctx) {
  ctx.test("example_1_already_palindrome", [&] {
    Solution sol;
    ctx.expect_true(sol.validPalindrome("aba"));
  });

  ctx.test("example_2_delete_one_right", [&] {
    Solution sol;
    ctx.expect_true(sol.validPalindrome("abca"));
  });

  ctx.test("python_build_delete_one_left", [&] {
    Solution sol;
    ctx.expect_true(sol.validPalindrome("acba"));
  });

  ctx.test("example_3_impossible", [&] {
    Solution sol;
    ctx.expect_false(sol.validPalindrome("abc"));
  });

  ctx.test("tricky_branch_choice", [&] {
    Solution sol;
    // Both s[left+1] and s[right-1] match initially; only one branch succeeds.
    ctx.expect_true(sol.validPalindrome("aguokepatgbnvfqmgmlcupuufxoohdfpgjd"
                                        "mysgvhmvffcnqxjjxqncffvmhvgsymdjgpfdh"
                                        "ooxfuupuculmgmqfvnbgtapekouga"));
    ctx.expect_true(sol.validPalindrome("ebcbbececabbacecbbcbe"));
    ctx.expect_false(sol.validPalindrome("abcdefg"));
  });

  ctx.test("minimal_lengths", [&] {
    Solution sol;
    ctx.expect_true(sol.validPalindrome("a"));
    ctx.expect_true(sol.validPalindrome("ab"));
    ctx.expect_true(sol.validPalindrome(""));
  });
}

}  // namespace
