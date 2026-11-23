// LeetCode 165: Compare Version Numbers
// https://leetcode.com/problems/compare-version-numbers/
//
// Problem:
//   Given two version strings `version1` and `version2` containing dot-separated
//   non-negative integer revisions (e.g., "1.0.1" and "1.00"), compare them
//   left-to-right by numeric revision value, ignoring leading zeros and
//   treating missing trailing revisions as 0. Return -1 if `version1 < version2`,
//   1 if `version1 > version2`, and 0 if they are equal.
//
// Algorithm:
//   Stream both version strings simultaneously with two cursors (`i` and `j`)
//   without splitting or allocating arrays:
//   1. At each revision level, extract the next dot-delimited digit span from
//      each string (or an empty span if that string is already exhausted).
//   2. Strip all leading '0' characters from each span so that numeric zero
//      maps canonically to the empty view `""`.
//   3. Compare the two canonical digit views first by length (fewer digits
//      means smaller non-negative integer) and then lexicographically. This
//      avoids integer overflow even for arbitrarily long revision numbers.
//   4. Return immediately on the first non-equal revision; return 0 once both
//      strings are completely consumed.
//   Time Complexity:  O(M + N), where M = |version1| and N = |version2|.
//   Space Complexity: O(1) auxiliary space (zero heap allocations).
//
// Data Structures:
//   - Two `std::size_t` cursor positions and non-owning `std::string_view`
//     digit slices for zero-copy streaming comparison.
//
// Real-World Applications:
//   - Package managers and dependency resolvers (npm, Cargo, pip, dpkg, Homebrew)
//     ordering semantic and multi-segment software release versions.
//   - Operating system and firmware update checkers determining upgrade eligibility.
//   - Protocol capability negotiation and API deprecation gating.
//
// Related LeetCode Problems:
//   - #65 Valid Number [done] (structured numeric string parsing)
//   - #161 One Edit Distance [done] (single-pass zero-copy string comparison)
//   - #415 Add Strings [done] (arbitrary-length decimal digit manipulation)
//   - #937 Reorder Data in Log Files [new] (custom multi-key string comparator)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string_view>

namespace {

class Solution {
 public:
  // Compares two dot-separated version strings numerically revision by revision.
  // Returns -1 if version1 < version2, 1 if version1 > version2, else 0.
  // Time: O(|version1| + |version2|), Space: O(1).
  int compareVersion(std::string_view version1, std::string_view version2) {
    std::size_t i = 0uz;
    std::size_t j = 0uz;

    // Loop invariant: all revisions before positions `i` and `j` are equal.
    while (i < version1.size() || j < version2.size()) {
      const std::string_view rev1 = nextCanonicalRevision(version1, i);
      const std::string_view rev2 = nextCanonicalRevision(version2, j);

      // Compare canonical (leading-zero-stripped) unsigned decimal strings:
      // longer string is strictly larger; equal length compares lexicographically.
      if (rev1.size() != rev2.size()) {
        return rev1.size() < rev2.size() ? -1 : 1;
      }
      if (rev1 != rev2) {
        return rev1 < rev2 ? -1 : 1;
      }
    }

    return 0;
  }

 private:
  // Extracts the next revision starting at `pos`, advances `pos` past the
  // trailing '.' (if present), and returns the digit view stripped of leading
  // '0's (so numeric 0 or an absent trailing revision yields "").
  static std::string_view nextCanonicalRevision(std::string_view version,
                                                std::size_t& pos) {
    if (pos >= version.size()) {
      return "";
    }

    const std::size_t start = pos;
    while (pos < version.size() && version[pos] != '.') {
      ++pos;
    }
    const std::size_t end = pos;

    // Advance past the '.' separator for the next call.
    if (pos < version.size() && version[pos] == '.') {
      ++pos;
    }

    // Trim leading zeros so "000" -> "" and "0012" -> "12".
    std::size_t first_nonzero = start;
    while (first_nonzero < end && version[first_nonzero] == '0') {
      ++first_nonzero;
    }

    return version.substr(first_nonzero, end - first_nonzero);
  }
};

LC_PROBLEM(165, "Compare Version Numbers", "string", ctx) {
  Solution sol;

  ctx.test("leetcode examples", [&] {
    ctx.expect_eq(sol.compareVersion("0.1", "1.1"), -1);
    ctx.expect_eq(sol.compareVersion("1.0.1", "1"), 1);
    ctx.expect_eq(sol.compareVersion("7.5.2.4", "7.5.3"), -1);
  });

  ctx.test("source build cases and leading or trailing zeros", [&] {
    ctx.expect_eq(sol.compareVersion("7.5.2", "7.5.3"), -1);
    ctx.expect_eq(sol.compareVersion("01", "1"), 0);
    ctx.expect_eq(sol.compareVersion("1.0.000.00", "1"), 0);
    ctx.expect_eq(sol.compareVersion("0.1", "0.0.1"), 1);
    ctx.expect_eq(sol.compareVersion("1.0", "1.10"), -1);
  });

  ctx.test("edge cases and huge revisions", [&] {
    ctx.expect_eq(sol.compareVersion("0.0.0", "0"), 0);
    ctx.expect_eq(sol.compareVersion("1", "1.0.0.1"), -1);
    ctx.expect_eq(sol.compareVersion("1.999999999999999999999",
                                     "1.999999999999999999998"),
                  1);
  });
}

}  // namespace
