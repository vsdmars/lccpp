// LeetCode 246: Strobogrammatic Number
// https://leetcode.com/problems/strobogrammatic-number/
//
// Problem:
//   Given a string `num` representing an integer, return true if `num` is a
//   strobogrammatic number—that is, it looks identical when rotated 180 degrees
//   (viewed upside down)—and false otherwise.
//   Constraints: 1 <= num.length <= 50; `num` consists only of digits and has
//   no leading zeros except for "0" itself.
//
// Algorithm:
//   Rotating a string by 180 degrees reverses character order and maps each
//   digit to its upside-down counterpart. Only five digits remain valid digits
//   after a 180-degree rotation:
//     '0' <-> '0', '1' <-> '1', '6' <-> '9', '8' <-> '8', '9' <-> '6'.
//   Use two pointers (`left` starting at 0, `right` starting at `n - 1`) moving
//   inward while `left <= right`:
//   - Look up `expected = kRotationTable[num[left]]`.
//   - If `expected == '\0'` (non-rotatable digit such as 2, 3, 4, 5, 7) or
//     `num[right] != expected`, return false.
//   - Including `left == right` automatically enforces that an odd-length
//     string's middle digit must map to itself ('0', '1', or '8').
//   Time:  O(n)
//   Space: O(1)
//
// Data Structures:
//   - constexpr std::array<char, 128> kRotationTable: Compile-time direct-
//     addressed hash table mapping each ASCII digit to its 180-degree rotated
//     counterpart (or '\0' if invalid).
//
// Real-World Applications:
//   - Optical inspection and barcode/label verification for orientation-
//     ambiguous markings on PCBs, shipping labels, and seven-segment displays.
//   - Validating symmetric/palindromic encodings (e.g., reverse-complement
//     matching in DNA bioinformatics, where A<->T and C<->G play the exact same
//     role as 6<->9).
//
// Related LeetCode Problems:
//   - #247 Strobogrammatic Number II [done] (generating all strobogrammatic numbers of length n)
//   - #248 Strobogrammatic Number III [new] (counting strobogrammatic numbers in a range)
//   - #125 Valid Palindrome [done] (same inward two-pointer symmetry check)
//   - #266 Palindrome Permutation [done] (character symmetry via frequency parity)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <string_view>

namespace {

constexpr std::size_t kAsciiSize = 128uz;

// Builds the compile-time 180-degree digit rotation lookup table.
// Entries remain '\0' for characters that do not form a valid digit upside down.
constexpr std::array<char, kAsciiSize> makeRotationTable() noexcept {
  std::array<char, kAsciiSize> table{};
  table[static_cast<unsigned char>('0')] = '0';
  table[static_cast<unsigned char>('1')] = '1';
  table[static_cast<unsigned char>('6')] = '9';
  table[static_cast<unsigned char>('8')] = '8';
  table[static_cast<unsigned char>('9')] = '6';
  return table;
}

constexpr std::array<char, kAsciiSize> kRotationTable = makeRotationTable();

class Solution {
 public:
  // Returns true iff `num` reads the same when rotated 180 degrees.
  // Invariant: all outer pairs (0, n-1), ..., (left-1, right+1) are valid
  // 180-degree mirrors of one another.
  // Time: O(n), Space: O(1).
  bool isStrobogrammatic(std::string_view num) {
    if (num.empty()) {
      return true;
    }

    std::size_t left = 0uz;
    std::size_t right = num.size() - 1uz;

    while (left <= right) {
      const auto left_byte = static_cast<unsigned char>(num[left]);
      if (left_byte >= kAsciiSize) {
        return false;
      }

      // When left == right (odd length), this also verifies the middle digit
      // maps to itself ('0', '1', or '8').
      const char expected = kRotationTable[left_byte];
      if (expected == '\0' || num[right] != expected) {
        return false;
      }

      // Guard against unsigned underflow when right == 0 on length-1 inputs.
      if (left == right) {
        break;
      }
      ++left;
      --right;
    }

    return true;
  }
};

LC_PROBLEM(246, "Strobogrammatic Number", "hash", ctx) {
  Solution sol;

  ctx.test("docstring_examples", [&] {
    ctx.expect_true(sol.isStrobogrammatic("69"));
    ctx.expect_true(sol.isStrobogrammatic("88"));
    ctx.expect_true(sol.isStrobogrammatic("818"));
    ctx.expect_true(sol.isStrobogrammatic("6699"));
    ctx.expect_true(sol.isStrobogrammatic("66199"));
  });

  ctx.test("non_strobogrammatic", [&] {
    ctx.expect_false(sol.isStrobogrammatic("25"));
    ctx.expect_false(sol.isStrobogrammatic("962"));
    ctx.expect_false(sol.isStrobogrammatic("6"));
    ctx.expect_false(sol.isStrobogrammatic("9"));
    ctx.expect_false(sol.isStrobogrammatic("66699"));
  });

  ctx.test("single_digits_and_edge_cases", [&] {
    ctx.expect_true(sol.isStrobogrammatic("0"));
    ctx.expect_true(sol.isStrobogrammatic("1"));
    ctx.expect_true(sol.isStrobogrammatic("8"));
    ctx.expect_true(sol.isStrobogrammatic("609"));
    ctx.expect_true(sol.isStrobogrammatic(""));
  });
}

}  // namespace
