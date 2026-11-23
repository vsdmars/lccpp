// LeetCode 345: Reverse Vowels Of A String
// https://leetcode.com/problems/reverse-vowels-of-a-string/
//
// Problem:
//   Given a string `s`, reverse only the vowels ('a', 'e', 'i', 'o', 'u' in
//   both lowercase and uppercase; 'y' is not a vowel) in the string and return
//   the resulting string, leaving all non-vowel characters in their original
//   positions.
//
// Examples:
//   Example 1:
//     Input:  s = "hello"
//     Output: "holle"
//     Explanation:
//       - Initialize result = "hello", left = 0, and right = 4.
//       - Advance left past 'h' to index 1 ('e') while right stays on index 4
//         ('o').
//       - Swap result[1] and result[4] to form "holle", then advance inward to left
//         = 2 and right = 3.
//       - Advance left past 'l' to index 3, where left == right == 3 ends the loop
//         and returns "holle".
//
//   Example 2:
//     Input:  s = "IceCreAm"
//     Output: "AceCreIm"
//     Explanation:
//       - Initialize result = "IceCreAm", left = 0 ('I'), and right = 7 ('m').
//       - Move right past 'm' to index 6 ('A'), swap result[0] and result[6] to
//         produce "AceCreIm", and advance to left = 1, right = 5.
//       - Advance left past 'c' to index 2 ('e') with right already at index 5
//         ('e'), swap them (leaving "AceCreIm"), and advance to left = 3, right =
//         4.
//       - Advance left past 'C' to index 4, meeting right = 4 so the loop
//         terminates and returns "AceCreIm".
//
//   Example 3:
//     Input:  s = "rhythm"
//     Output: "rhythm"
//     Explanation:
//       - Initialize result = "rhythm", left = 0, and right = 5.
//       - Because 'y' is not in kVowels ("aeiouAEIOU"), is_vowel returns false for
//         every character in "rhythm".
//       - The inner left scan advances left from 0 to 5 until left < right is
//         false, performing no swaps and returning "rhythm".
//
// Algorithm:
//   Converging two pointers from both ends of a mutable copy of `s`:
//     - Initialize `left = 0` and `right = n - 1`.
//     - Advance `left` rightward while `left < right` and `result[left]` is not
//       a vowel.
//     - Advance `right` leftward while `left < right` and `result[right]` is
//       not a vowel.
//     - If `left < right`, both pointers sit on unswapped vowels: swap them and
//       move both pointers inward (`++left`, `--right`).
//   Guarding both inner scans and the swap with `left < right` prevents the
//   pointers from crossing over and accidentally double-swapping or causing
//   unsigned underflow on empty strings.
//   Time complexity: O(n) — each character is inspected at most once.
//   Space complexity: O(1) auxiliary space beyond the returned output string.
//
// Data Structures:
//   - std::string (`result`): mutable character buffer modified in-place.
//   - constexpr std::string_view (`kVowels`): constant 10-character vowel set
//     for zero-allocation membership checks.
//   - Two converging index pointers (`left`, `right`).
//
// Real-World Applications:
//   - Selective in-place reordering / obfuscation of filtered tokens inside a
//     formatted record while preserving delimiter and punctuation positions.
//   - Bidirectional text layout and masked-buffer reversal routines where fixed
//     control bytes must stay anchored while payload bytes are reversed.
//
// Related LeetCode Problems:
//   - #344 Reverse String [done] (unfiltered converging two-pointer reversal)
//   - #917 Reverse Only Letters [new] (same filtered two-pointer swap with std::isalpha)
//   - #541 Reverse String II [new] (chunked substring reversal)
//   - #125 Valid Palindrome [done] (converging two pointers skipping non-alphanumerics)
//   - #1119 Remove Vowels from a String [new] (single-pass vowel filtering)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <utility>

namespace {

constexpr std::string_view kVowels = "aeiouAEIOU";

// Returns true iff `ch` is an English ASCII vowel (excluding 'y'/'Y').
constexpr bool is_vowel(char ch) noexcept { return kVowels.contains(ch); }

class Solution {
 public:
  // Reverses only the vowels of `s` while leaving all other characters at
  // their original indices.
  //
  // Invariant: every vowel strictly outside [left, right] has already been
  // swapped into its final reversed position.
  //
  // Time: O(n), Space: O(1) auxiliary (O(n) for the returned string).
  std::string reverseVowels(std::string_view s) {
    std::string result(s);
    // Strings of length 0 or 1 have no pair of positions to swap (also avoids
    // `size() - 1` underflow on empty input).
    if (result.size() < 2) {
      return result;
    }

    std::size_t left = 0;
    std::size_t right = result.size() - 1;

    while (left < right) {
      // Advance `left` to the next unswapped vowel.
      while (left < right && !is_vowel(result[left])) {
        ++left;
      }
      // Advance `right` to the previous unswapped vowel.
      while (left < right && !is_vowel(result[right])) {
        --right;
      }
      if (left < right) {
        std::swap(result[left], result[right]);
        ++left;
        --right;
      }
    }

    return result;
  }
};

LC_PROBLEM(345, "Reverse Vowels Of A String", "two_pointers", ctx) {
  Solution sol;

  ctx.test("example_1_hello", [&] {
    ctx.expect_eq(sol.reverseVowels("hello"), "holle");
  });

  ctx.test("example_2_leetcode", [&] {
    ctx.expect_eq(sol.reverseVowels("leetcode"), "leotcede");
  });

  ctx.test("python_source_cases", [&] {
    ctx.expect_eq(sol.reverseVowels("leefcode"), "leofcede");
    ctx.expect_eq(sol.reverseVowels("ooee"), "eeoo");
  });

  ctx.test("mixed_case_vowels", [&] {
    ctx.expect_eq(sol.reverseVowels("IceCreAm"), "AceCreIm");
    ctx.expect_eq(sol.reverseVowels("aA"), "Aa");
  });

  ctx.test("empty_and_single_char", [&] {
    ctx.expect_eq(sol.reverseVowels(""), "");
    ctx.expect_eq(sol.reverseVowels("a"), "a");
    ctx.expect_eq(sol.reverseVowels("z"), "z");
  });

  ctx.test("no_vowels_and_y_excluded", [&] {
    ctx.expect_eq(sol.reverseVowels("rhythm"), "rhythm");
    ctx.expect_eq(sol.reverseVowels("bcdfgY"), "bcdfgY");
  });

  ctx.test("all_vowels", [&] {
    ctx.expect_eq(sol.reverseVowels("aeiouAEIOU"), "UOIEAuoiea");
  });
}

}  // namespace
