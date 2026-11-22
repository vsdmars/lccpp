// https://leetcode.com/problems/string-transforms-into-another-string/
//
// Given two lowercase strings `str1` and `str2` of equal length, determine
// whether `str1` can be transformed into `str2` using zero or more bulk
// character replacements (replacing every occurrence of one letter with
// another).
//
// Graph interpretation:
// Each letter in `str1` has at most one outgoing edge to a target letter in
// `str2` (a functional graph). A valid transformation exists iff:
//   1. `str1 == str2`, OR
//   2. Every letter in `str1` maps consistently to a single letter in `str2`
//      (no 1-to-many conflict), AND
//   3. `str2` uses strictly fewer than 26 distinct letters so at least one
//      unused letter is available as a temporary swap variable to break cycles
//      (e.g., `a -> b -> a`).

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <string_view>

namespace {

constexpr std::size_t kAlphabetSize = 26uz;

class Solution {
 public:
  // Checks functional-graph consistency and availability of a spare temp char.
  // Time: O(N), Space: O(1) (fixed 26-element arrays).
  bool canConvert(std::string_view str1, std::string_view str2) {
    if (str1.size() != str2.size()) {
      return false;
    }
    if (str1 == str2) {
      return true;
    }

    std::array<char, kAlphabetSize> mapping{};
    std::array<bool, kAlphabetSize> used_in_str2{};
    std::size_t distinct_in_str2 = 0uz;

    for (std::size_t i = 0uz; i < str1.size(); ++i) {
      const auto src = static_cast<std::size_t>(str1[i] - 'a');
      const char dst = str2[i];

      if (mapping[src] == '\0') {
        mapping[src] = dst;
      } else if (mapping[src] != dst) {
        return false;  // Same source character needs two different targets.
      }

      const auto dst_idx = static_cast<std::size_t>(dst - 'a');
      if (!used_in_str2[dst_idx]) {
        used_in_str2[dst_idx] = true;
        ++distinct_in_str2;
      }
    }

    return distinct_in_str2 < kAlphabetSize;
  }
};

LC_PROBLEM(1153, "String Transforms Into Another String", "graph", ctx) {
  Solution sol;

  ctx.test("example 1: chain conversion aabcc -> ccdee", [&] {
    ctx.expect_true(sol.canConvert("aabcc", "ccdee"));
  });

  ctx.test("example 2: conflicting mapping leetcode -> codeleet", [&] {
    ctx.expect_false(sol.canConvert("leetcode", "codeleet"));
  });

  ctx.test("26-letter cyclic shift has no unused temp char", [&] {
    ctx.expect_false(sol.canConvert("abcdefghijklmnopqrstuvwxyz",
                                    "bcdefghijklmnopqrstuvwxyza"));
  });

  ctx.test("identical 26-letter strings require zero conversions", [&] {
    ctx.expect_true(sol.canConvert("abcdefghijklmnopqrstuvwxyz",
                                   "abcdefghijklmnopqrstuvwxyz"));
  });

  ctx.test("small swap cycle and one-to-many rejection", [&] {
    ctx.expect_true(sol.canConvert("ab", "ba"));
    ctx.expect_true(sol.canConvert("ltg", "egt"));
    ctx.expect_false(sol.canConvert("aa", "bc"));
  });
}

}  // namespace
