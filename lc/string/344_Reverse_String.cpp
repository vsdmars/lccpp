// LeetCode 344: Reverse String
// https://leetcode.com/problems/reverse-string/
//
// Problem:
//   Given a string represented as a mutable array of characters `s`, reverse
//   the characters in place using O(1) extra memory.
//
// Algorithm:
//   Use two opposite-direction pointers, `left` initialized to `0` and `right`
//   initialized to `s.size() - 1`. While `left < right`, swap `s[left]` with
//   `s[right]`, increment `left`, and decrement `right`. When the pointers meet
//   or cross, every symmetric pair `(i, n - 1 - i)` has been swapped exactly
//   once and the array is reversed in place.
//   Time Complexity:  O(n)
//   Space Complexity: O(1)
//
// Data Structures:
//   - `std::vector<char>`: in-place mutable character buffer.
//   - Two `std::size_t` indices (`left`, `right`) tracking the unreversed
//     interior range `[left, right]`.
//
// Real-World Applications:
//   - In-place array rotation via three reversals (Bentley's reversal algorithm,
//     used in `std::rotate` and text-editor gap/piece buffers).
//   - Endianness byte-swapping and network-to-host byte-order conversion on
//     fixed-width buffers.
//   - Reversing digit buffers during integer-to-ASCII (`itoa`) formatting and
//     DNA reverse-complement computation.
//
// Related LeetCode Problems:
//   - #541 Reverse String II [new] (reverses the first k characters of every 2k block)
//   - #345 Reverse Vowels of a String [done] (conditional two-pointer swap restricted to vowels)
//   - #151 Reverse Words in a String [done] (in-place whole-string + per-word reversal)
//   - #189 Rotate Array [done] (uses three in-place segment reversals for O(1) space rotation)
//   - #125 Valid Palindrome [done] (symmetric two-pointer scan from both ends)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string_view>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Reverses `s` in place using symmetric two-pointer swaps.
  // Invariant: before each iteration, `s[0..left-1]` and `s[right+1..n-1]` are
  // already in their final reversed positions.
  // Time: O(n), Space: O(1).
  void reverseString(std::vector<char>& s) {
    if (s.size() < 2uz) {
      return;
    }

    std::size_t left = 0uz;
    std::size_t right = s.size() - 1uz;
    while (left < right) {
      std::swap(s[left], s[right]);
      ++left;
      --right;
    }
  }
};

// Converts a string literal to `std::vector<char>` for concise test setup.
std::vector<char> to_chars(std::string_view text) {
  return {text.begin(), text.end()};
}

LC_PROBLEM(344, "Reverse String", "string", ctx) {
  Solution sol;

  ctx.test("example_1_hello", [&] {
    std::vector<char> s = to_chars("hello");
    sol.reverseString(s);
    ctx.expect_eq(s, to_chars("olleh"));
  });

  ctx.test("example_2_panama", [&] {
    std::vector<char> s = to_chars("A man, a plan, a canal: Panama");
    sol.reverseString(s);
    ctx.expect_eq(s, to_chars("amanaP :lanac a ,nalp a ,nam A"));
  });

  ctx.test("even_length_hannah", [&] {
    std::vector<char> s = to_chars("Hannah");
    sol.reverseString(s);
    ctx.expect_eq(s, to_chars("hannaH"));
  });

  ctx.test("empty_and_single_char", [&] {
    std::vector<char> empty;
    sol.reverseString(empty);
    ctx.expect_eq(empty, std::vector<char>{});

    std::vector<char> single{'x'};
    sol.reverseString(single);
    ctx.expect_eq(single, std::vector<char>{'x'});
  });

  ctx.test("double_reverse_is_identity", [&] {
    const std::vector<char> original = to_chars("abcdefg123");
    std::vector<char> s = original;
    sol.reverseString(s);
    sol.reverseString(s);
    ctx.expect_eq(s, original);
  });
}

}  // namespace
