// LeetCode 266: Palindrome Permutation
// https://leetcode.com/problems/palindrome-permutation/
//
// Problem:
//   Given a string `s`, return true if any permutation of `s` can form a
//   palindrome, and false otherwise.
//   Constraints: 1 <= s.length <= 5000; `s` consists of ASCII characters
//   (lowercase English letters on LeetCode, handled for full ASCII here).
//
// Examples:
//   Example 1:
//     Input:  s = "code"
//     Output: false
//     Explanation:
//       - Initialize the 256-bit `odd_parity` bitset to all zeros.
//       - Flip the bits for `'c'`, `'o'`, `'d'`, and `'e'`, leaving four distinct
//         bits set to `1`.
//       - Evaluate `odd_parity.count() <= 1`: since `4 > 1`, return `false`.
//
//   Example 2:
//     Input:  s = "carerac"
//     Output: true
//     Explanation:
//       - Start with `odd_parity` all zeros and flip bits left-to-right for `'c'`,
//         `'a'`, `'r'`, `'e'`, `'r'`, `'a'`, and `'c'`.
//       - Each of `'c'`, `'a'`, and `'r'` is flipped twice and cancels back to `0`,
//         leaving only the bit for `'e'` set to `1`.
//       - Since `odd_parity.count() == 1 <= 1`, return `true`.
//
//   Example 3:
//     Input:  s = "aabbcc"
//     Output: true
//     Explanation:
//       - Scan `"aabbcc"`, flipping the bit for each character in `odd_parity`.
//       - Because `'a'`, `'b'`, and `'c'` each appear twice, every flipped bit
//         toggles twice and returns to `0`.
//       - Observe `odd_parity.count() == 0 <= 1` and return `true`.
//
// Algorithm:
//   In any palindrome, every character appearing on the left half must be
//   paired with an identical character on the right half, leaving at most one
//   unpaired center character (when the string length is odd). Therefore, a
//   permutation of `s` can form a palindrome if and only if at most one
//   distinct character has an odd frequency.
//
//   Track frequency parity for all 256 ASCII byte values using a fixed-size
//   `std::bitset<256> odd_parity`: flip bit `static_cast<unsigned char>(ch)`
//   for each character in `s`, then return `odd_parity.count() <= 1`.
//   Time:  O(n)
//   Space: O(1) (32-byte fixed bitset)
//
// Data Structures:
//   - std::bitset<256> odd_parity: Compact bit-vector hash table tracking
//     whether each ASCII character currently has an odd count.
//
// Real-World Applications:
//   - Fast necessary-condition prefiltering before expensive palindrome /
//     symmetric-sequence construction or backtracking search.
//   - Parity-bit checks and XOR cancellation patterns in error-detection
//     codes and streaming odd-occurrence detection.
//   - Bioinformatics: screening k-mer multisets for palindromic feasibility.
//
// Related LeetCode Problems:
//   - #267 Palindrome Permutation II [new] (generating all unique palindromic permutations)
//   - #409 Longest Palindrome [done] (maximum palindrome length buildable from character frequencies)
//   - #242 Valid Anagram [done] (character-frequency counting over strings)
//   - #246 Strobogrammatic Number [done] (two-pointer rotational symmetry check)

#include "framework/lc_framework.h"

#include <bitset>
#include <cstddef>
#include <string_view>

namespace {

constexpr std::size_t kAsciiSize = 256uz;

class Solution {
 public:
  // Returns true iff the characters of `s` can be rearranged into a palindrome.
  // Invariant: after processing a prefix of `s`, bit `b` of `odd_parity` is 1
  // iff byte `b` has appeared an odd number of times in that prefix.
  // Time: O(n), Space: O(1).
  bool canPermutePalindrome(std::string_view s) {
    std::bitset<kAsciiSize> odd_parity;

    for (char ch : s) {
      odd_parity.flip(static_cast<unsigned char>(ch));
    }

    // At most one character may have an odd frequency (placed at the center).
    return odd_parity.count() <= 1uz;
  }
};

LC_PROBLEM(266, "Palindrome Permutation", "hash", ctx) {
  Solution sol;

  ctx.test("docstring_examples", [&] {
    ctx.expect_false(sol.canPermutePalindrome("code"));
    ctx.expect_true(sol.canPermutePalindrome("aab"));
    ctx.expect_true(sol.canPermutePalindrome("carerac"));
  });

  ctx.test("source_build_and_edge_cases", [&] {
    ctx.expect_false(sol.canPermutePalindrome("cd"));
    ctx.expect_true(sol.canPermutePalindrome(""));
    ctx.expect_true(sol.canPermutePalindrome("a"));
    ctx.expect_true(sol.canPermutePalindrome("aa"));
    ctx.expect_true(sol.canPermutePalindrome("aabbcc"));
    ctx.expect_false(sol.canPermutePalindrome("aabbcd"));
  });
}

}  // namespace
