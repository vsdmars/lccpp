// LeetCode 844: Backspace String Compare
// https://leetcode.com/problems/backspace-string-compare/
//
// Problem:
//   Given two strings `s` and `t` consisting of lowercase English letters and
//   `#` characters (where `#` represents a backspace keystroke), return `true`
//   if both strings produce the exact same final text when typed into empty
//   text editors, and `false` otherwise. Backspacing an empty buffer leaves
//   the buffer empty.
//
// Examples:
//   Example 1:
//     Input:  s = "ab#c", t = "ad#c"
//     Output: true
//     Explanation:
//       - build_typed("ab#c") pushes 'a' and 'b', pops 'b' on '#', and pushes 'c'
//         to produce "ac".
//       - build_typed("ad#c") pushes 'a' and 'd', pops 'd' on '#', and pushes 'c'
//         to produce "ac".
//       - Comparing the two reconstructed strings ("ac" == "ac") returns true,
//         matching backspaceCompareO1Space which scans backward and compares 'c'
//         then 'a'.
//
//   Example 2:
//     Input:  s = "a##c", t = "#a#c"
//     Output: true
//     Explanation:
//       - build_typed("a##c") pushes 'a', pops 'a' on the first '#', ignores the
//         second '#' on an empty buffer, and pushes 'c' to produce "c".
//       - build_typed("#a#c") ignores the leading '#' on an empty buffer, pushes
//         'a', pops 'a' on the second '#', and pushes 'c' to produce "c".
//       - Both buffers equal "c" (and next_valid_index finds only index 3 'c' in
//         both strings before returning -1), so both methods return true.
//
//   Example 3:
//     Input:  s = "bxj##tw", t = "bxj###tw"
//     Output: false
//     Explanation:
//       - build_typed("bxj##tw") pushes "bxj", pops 'j' and 'x' for the two '#'
//         keystrokes, and appends "tw" to produce "btw".
//       - build_typed("bxj###tw") pushes "bxj", pops 'j', 'x', and 'b' for the
//         three '#' keystrokes, and appends "tw" to produce "tw".
//       - Comparing "btw" against "tw" returns false (and in
//         backspaceCompareO1Space, after matching 'w' and 't', s lands on 'b' at
//         index 0 while t reaches j = -1).
//
// Algorithm:
//   1) Stack Simulation (`backspaceCompare`):
//      Simulate typing each string left-to-right into a character stack
//      (`std::string`). Push regular characters; on `#`, pop the top character
//      if the stack is non-empty. Compare the two resulting strings for
//      equality.
//      Time Complexity: O(N + M), where N = s.size() and M = t.size().
//      Space Complexity: O(N + M) for the two reconstructed strings.
//   2) Reverse Two-Pointer Follow-Up (`backspaceCompareO1Space`):
//      Scan both strings right-to-left because a `#` only deletes characters
//      to its left. For each string, count pending `#` skips to locate the
//      next surviving character; compare surviving characters pair by pair.
//      Time Complexity: O(N + M).
//      Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - `std::string` used as a character stack for left-to-right simulation.
//   - Two signed index cursors (`std::ptrdiff_t`) and skip counters for the
//     O(1)-space reverse scan.
//
// Real-World Applications:
//   - Terminal emulators and line-editing buffers (TTY canonical mode /
//     readline) normalizing raw keystroke streams with erase characters.
//   - Input-event replay and equivalence checking in UI automated testing.
//   - Log normalization and diffing of raw command histories containing
//     control/backspace sequences.
//
// Related LeetCode Problems:
//   - #1047 Remove All Adjacent Duplicates In String [done] (character stack)
//   - #2390 Removing Stars From a String [new] (single-string backspace stack)
//   - #1598 Crawler Log Folder [new] (directory-stack simulation with "..")
//   - #71 Simplify Path [done] (path normalization stack with parent pops)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>

namespace {

class Solution {
 public:
  // Compares the final editor contents of `s` and `t` using stack simulation.
  // Time: O(N + M), Space: O(N + M).
  bool backspaceCompare(std::string_view s, std::string_view t) {
    return build_typed(s) == build_typed(t);
  }

  // Follow-up: compares `s` and `t` in a single reverse pass with O(1) extra
  // space by resolving backspaces from right to left.
  // Time: O(N + M), Space: O(1).
  bool backspaceCompareO1Space(std::string_view s, std::string_view t) {
    auto i = static_cast<std::ptrdiff_t>(s.size()) - 1;
    auto j = static_cast<std::ptrdiff_t>(t.size()) - 1;

    while (i >= 0 || j >= 0) {
      i = next_valid_index(s, i);
      j = next_valid_index(t, j);

      // Both strings exhausted all surviving characters simultaneously.
      if (i < 0 && j < 0) {
        return true;
      }
      // Exactly one string still has a surviving character.
      if (i < 0 || j < 0) {
        return false;
      }
      if (s[static_cast<std::size_t>(i)] != t[static_cast<std::size_t>(j)]) {
        return false;
      }

      --i;
      --j;
    }

    return true;
  }

 private:
  // Replays keystrokes into a string buffer acting as a character stack.
  static std::string build_typed(std::string_view raw) {
    std::string typed;
    typed.reserve(raw.size());
    for (const char ch : raw) {
      if (ch == '#') {
        if (!typed.empty()) {
          typed.pop_back();
        }
      } else {
        typed.push_back(ch);
      }
    }
    return typed;
  }

  // Walks `idx` leftward past any '#' characters and the characters they
  // delete, returning the index of the next surviving character or -1 if none.
  static std::ptrdiff_t next_valid_index(std::string_view str,
                                         std::ptrdiff_t idx) {
    int skips = 0;
    while (idx >= 0) {
      if (str[static_cast<std::size_t>(idx)] == '#') {
        ++skips;
        --idx;
      } else if (skips > 0) {
        --skips;
        --idx;
      } else {
        break;
      }
    }
    return idx;
  }
};

LC_PROBLEM(844, "Backspace String Compare", "stack", ctx) {
  Solution sol;

  auto check_both = [&](std::string_view s, std::string_view t, bool expected) {
    ctx.expect_eq(sol.backspaceCompare(s, t), expected);
    ctx.expect_eq(sol.backspaceCompareO1Space(s, t), expected);
  };

  ctx.test("examples from prompt", [&] {
    check_both("ab#c", "ad#c", true);
    check_both("ab##", "c#d#", true);
    check_both("a##c", "#a#c", true);
    check_both("a#c", "b", false);
  });

  ctx.test("excess backspaces and empty results", [&] {
    check_both("####", "", true);
    check_both("###a", "a", true);
    check_both("a###b", "b", true);
    check_both("bxj##tw", "bxo#j##tw", true);
    check_both("bxj##tw", "bxj###tw", false);
  });

  ctx.test("no backspaces and length mismatch", [&] {
    check_both("abc", "abc", true);
    check_both("abc", "ab", false);
    check_both("y#fo##f", "y#f#o##f", true);
  });
}

}  // namespace
