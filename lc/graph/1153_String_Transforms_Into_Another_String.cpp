// LeetCode 1153: String Transforms Into Another String
// https://leetcode.com/problems/string-transforms-into-another-string/
//
// Problem:
//   Given two lowercase English strings `str1` and `str2` of equal length,
//   determine whether `str1` can be transformed into `str2` using zero or more
//   bulk conversions, where one conversion replaces every occurrence of a
//   chosen character in `str1` with another lowercase English character.
//
// Algorithm:
//   Model the required character substitutions as a directed functional graph
//   (each source letter has outdegree at most 1). If `str1 == str2`, zero
//   operations suffice so return `true`. Otherwise, scan both strings in one
//   pass to verify two necessary and sufficient conditions:
//   1. Functional consistency: every occurrence of a letter `c` in `str1` must
//      map to the exact same target letter in `str2` (a 1-to-many split is
//      impossible because identical characters always move together).
//   2. Spare temporary character: `str2` must use strictly fewer than 26
//      distinct letters. Any directed cycle (such as `a -> b -> a`) requires
//      at least one unused letter as a temporary register (`a -> z`, `b -> a`,
//      `z -> b`). If `str2` uses all 26 letters and `str1 != str2`, the mapping
//      must be a non-identity bijection with at least one cycle and zero spare
//      characters, making conversion impossible.
//   Time: O(N), Space: O(1) (fixed 26-element arrays).
//
// Data Structures:
//   - `std::array<char, 26> mapping`: stores the unique target letter assigned
//     to each lowercase source letter (`'\0'` if unseen).
//   - `std::array<bool, 26> used_in_str2`: fixed-size bitset tracking which
//     alphabet letters appear in `str2` to count `distinct_in_str2`.
//
// Real-World Applications:
//   - Compiler register allocation (parallel-copy resolution): resolving
//     cyclic register moves (`r1 <- r2, r2 <- r1`) requires a scratch register
//     or swap instruction when all registers are live.
//   - Database schema/enum migration and bulk identifier renaming where
//     cycles between old and new names need an intermediate placeholder.
//   - Batch file renaming and symbol-table substitution engines.
//
// Related LeetCode Problems:
//   - #205 Isomorphic Strings (checks consistent one-to-one character mapping
//     between two strings)
//   - #290 Word Pattern (verifies consistent bijection between pattern
//     characters and space-separated words)
//   - #890 Find and Replace Pattern (filters words matching a consistent
//     character bijection)
//   - #854 K-Similar Strings (transforms one string into another by resolving
//     character permutation cycles with minimum swaps)
//   - #777 Swap Adjacent in LR String (decides string transformation
//     reachability via structural invariants)

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
