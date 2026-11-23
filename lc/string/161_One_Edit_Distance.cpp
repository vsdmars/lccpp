// LeetCode 161: One Edit Distance
// https://leetcode.com/problems/one-edit-distance/
//
// Problem:
//   Given two strings `s` and `t`, return true if and only if they are exactly
//   one edit distance apart. An edit is defined as inserting one character,
//   deleting one character, or replacing one character in `s` to obtain `t`.
//   Identical strings are 0 edits apart and therefore return false.
//
// Algorithm:
//   Normalize the inputs so that `s.size() <= t.size()` by swapping their
//   `std::string_view` handles if necessary.
//   1. If `t.size() - s.size() > 1`, at least two insertions/deletions are
//      required, so return false immediately.
//   2. Scan from index 0 to `s.size() - 1` looking for the first mismatch
//      `s[i] != t[i]`:
//      - If `s.size() == t.size()`, the only valid single edit is replacing
//        `s[i]` with `t[i]`, which succeeds iff `s.substr(i + 1) == t.substr(i + 1)`.
//      - If `s.size() + 1 == t.size()`, the only valid single edit is inserting
//        `t[i]` before `s[i]`, which succeeds iff `s.substr(i) == t.substr(i + 1)`.
//   3. If no mismatch occurs across all of `s`, then `s` is a prefix of `t`,
//      so they are one edit apart iff `t.size() == s.size() + 1` (one trailing
//      insertion, excluding the 0-edit identical case).
//   Time Complexity:  O(min(|s|, |t|)) single linear comparison pass.
//   Space Complexity: O(1) using non-owning `std::string_view` slices.
//
// Data Structures:
//   - `std::string_view` for O(1) zero-allocation suffix slicing and comparison.
//
// Real-World Applications:
//   - Spell-checkers and search-bar typo tolerance ("did you mean?") for
//     single-keystroke errors (omissions, transpositions/substitutions, extras).
//   - DNA/RNA sequencing primer matching allowing a single SNP or 1-bp indel.
//   - CLI flag suggestion engines (e.g., git/clang typo diagnostics).
//
// Related LeetCode Problems:
//   - #72 Edit Distance [done] (general DP Levenshtein distance for arbitrary k)
//   - #583 Delete Operation for Two Strings [done] (delete-only edit distance)
//   - #712 Minimum ASCII Delete Sum for Two Strings [new] (weighted edit DP)
//   - #165 Compare Version Numbers [done] (single-pass linear string comparison)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string_view>
#include <utility>

namespace {

class Solution {
 public:
  // Determines whether `s` and `t` are separated by exactly one single-char
  // insertion, deletion, or replacement.
  // Time: O(min(|s|, |t|)), Space: O(1).
  bool isOneEditDistance(std::string_view s, std::string_view t) {
    // Enforce the invariant `s.size() <= t.size()` to cut casework in half.
    if (s.size() > t.size()) {
      std::swap(s, t);
    }

    // A length gap of 2 or more requires at least 2 edits.
    if (t.size() - s.size() > 1uz) {
      return false;
    }

    for (std::size_t i = 0uz; i < s.size(); ++i) {
      if (s[i] != t[i]) {
        // Equal length: only a single replacement at index `i` can reconcile.
        if (s.size() == t.size()) {
          return s.substr(i + 1uz) == t.substr(i + 1uz);
        }
        // `t` is 1 char longer: only inserting `t[i]` into `s` can reconcile.
        return s.substr(i) == t.substr(i + 1uz);
      }
    }

    // All characters of `s` matched `t`'s prefix; valid iff `t` has exactly
    // one extra trailing character (rejects identical strings).
    return s.size() + 1uz == t.size();
  }
};

LC_PROBLEM(161, "One Edit Distance", "string", ctx) {
  Solution sol;

  ctx.test("single insertion and deletion", [&] {
    ctx.expect_true(sol.isOneEditDistance("", "a"));
    ctx.expect_true(sol.isOneEditDistance("a", ""));
    ctx.expect_true(sol.isOneEditDistance("sds", "sxds"));
    ctx.expect_true(sol.isOneEditDistance("ab", "acb"));
    ctx.expect_true(sol.isOneEditDistance("abc", "ab"));
  });

  ctx.test("single replacement", [&] {
    ctx.expect_true(sol.isOneEditDistance("cat", "cut"));
    ctx.expect_true(sol.isOneEditDistance("a", "b"));
  });

  ctx.test("zero edits and multiple edits", [&] {
    ctx.expect_false(sol.isOneEditDistance("", ""));
    ctx.expect_false(sol.isOneEditDistance("abc", "abc"));
    ctx.expect_false(sol.isOneEditDistance("cab", "ad"));
    ctx.expect_false(sol.isOneEditDistance("1203", "12134"));
    ctx.expect_false(sol.isOneEditDistance("abc", "abdef"));
  });
}

}  // namespace
