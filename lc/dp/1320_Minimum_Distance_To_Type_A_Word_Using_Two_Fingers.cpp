// LeetCode 1320: Minimum Distance To Type A Word Using Two Fingers
// https://leetcode.com/problems/minimum-distance-to-type-a-word-using-two-fingers/
//
// Problem:
//   Uppercase English letters `'A'..'Z'` are laid out on a 6-column grid where
//   letter index `c` (`0..25`) sits at `(c / 6, c % 6)`. Given a string `word`
//   (`2 <= word.length <= 300`), return the minimum total Manhattan distance
//   needed to type `word` using two fingers. The initial placement of both
//   fingers is free (distance 0).
//
// Examples:
//   Example 1:
//     Input:  word = "CAKE"
//     Output: 3
//     Explanation:
//       - Start with finger 1 free on word[0] = 'C' at (0, 2) and finger 2 hovering
//         at kHover = 26, calling dfs(1, 26).
//       - At idx = 1 ('A' at (0, 0)), move finger 1 from 'C' to 'A' at Manhattan
//         cost |0 - 0| + |2 - 0| = 2 and call dfs(2, 26).
//       - At idx = 2 ('K' at (1, 4)), place the hovering finger (other = 26) on 'K'
//         at cost 0, leaving 'A' as the idle finger in dfs(3, 'A').
//       - At idx = 3 ('E' at (0, 4)), move the active finger from 'K' (1, 4) to 'E'
//         (0, 4) at cost |1 - 0| + |4 - 4| = 1, giving minimum total distance 2 + 0
//         + 1 = 3.
//
//   Example 2:
//     Input:  word = "HAPPY"
//     Output: 6
//     Explanation:
//       - Start with finger 1 free on word[0] = 'H' at (1, 1) and finger 2 hovering
//         (other = 26), calling dfs(1, 26).
//       - At idx = 1 ('A' at (0, 0)), moving finger 1 from 'H' to 'A' costs |1 - 0|
//         + |1 - 0| = 2 and reaches dfs(2, 26).
//       - At idx = 2 ('P' at (2, 3)), placing the hovering finger on 'P' costs 0
//         and transitions to dfs(3, 'A').
//       - At idx = 3 ('P' at (2, 3)), keeping the active finger on 'P' costs 0 and
//         transitions to dfs(4, 'A').
//       - At idx = 4 ('Y' at (4, 0)), moving the idle finger from 'A' (0, 0) to 'Y'
//         (4, 0) costs |0 - 4| + |0 - 0| = 4, yielding minimum total distance 2 + 0
//         + 0 + 4 = 6.
//
//   Example 3:
//     Input:  word = "ABABAB"
//     Output: 0
//     Explanation:
//       - Start with finger 1 free on word[0] = 'A' at (0, 0) and finger 2 hovering
//         (other = 26).
//       - At idx = 1 ('B' at (0, 1)), placing the hovering finger on 'B' costs
//         dist(26, 'B') = 0 and enters dfs(2, 'A').
//       - For idx = 2..5 ('A', 'B', 'A', 'B'), each target character already
//         matches the idle finger position, so moving the idle finger to curr costs
//         0 at every step and returns 0.
//
// Algorithm:
//   Top-down memoized DFS with state compression. Right before typing
//   `word[idx]` (`idx >= 1`), one finger is guaranteed to rest on the previous
//   letter `word[idx - 1]`, so the full configuration is completely specified
//   by `(idx, other)`, where `other` in `0..26` is the position of the second
//   finger (`26` denotes "not yet placed / hovering", which moves anywhere at
//   cost 0).
//   At each step `idx`, let `prev = word[idx - 1]` and `curr = word[idx]`:
//     1. Move the finger on `prev` to `curr`:
//          `cost = dist(prev, curr) + dfs(idx + 1, other)`
//     2. Move the `other` finger to `curr` (leaving the first finger on `prev`):
//          `cost = dist(other, curr) + dfs(idx + 1, prev)`
//   Starting with the first finger placed for free on `word[0]` and the second
//   finger hovering (`dfs(1, 26)`), we take the minimum of both choices and
//   memoize every `(idx, other)` state.
//   Time Complexity:  O(27 * n) = O(n), where `n = word.size()`.
//   Space Complexity: O(27 * n) = O(n) for the flat memoization table and stack.
//
// Data Structures:
//   - Flat 2D memoization vector (`std::vector<int>` of size `n * 27`) indexed
//     by `(idx * kFingerStates) + other`, initialized to `-1`.
//
// Real-World Applications:
//   - Two-server / k-server online and offline scheduling (minimizing total
//     head/arm movement for dual-head disk drives, SMT pick-and-place robots, or
//     dual-extruder 3D printers).
//   - Ergonomic virtual-keyboard layout optimization for two-thumb mobile
//     typing.
//
// Related LeetCode Problems:
//   - #1165 Single-Row Keyboard [new] (1D single-finger typing distance)
//   - #1974 Minimum Time to Type Word Using Special Typewriter [new] (circular keyboard distance)
//   - #1478 Allocate Mailboxes [new] (minimum Manhattan-distance placement DP)
//   - #1531 String Compression II [done] (multi-dimensional sequence DP with state compression)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the minimum total Manhattan distance to type `word` with two
  // fingers using state-compressed top-down memoized DFS.
  // Time: O(n), Space: O(n).
  int minimumDistance(std::string_view word) {
    const std::size_t n = word.size();
    if (n <= 2uz) {
      // Initial placement of both fingers is free, so <= 2 letters cost 0.
      return 0;
    }

    static constexpr int kUnvisited = -1;
    std::vector<int> memo(n * kFingerStates, kUnvisited);

    auto dfs = [&](this auto&& self, std::size_t idx, std::size_t other) -> int {
      if (idx == n) {
        return 0;
      }

      const std::size_t state = (idx * kFingerStates) + other;
      if (memo[state] != kUnvisited) {
        return memo[state];
      }

      const std::size_t prev = charToKey(word[idx - 1uz]);
      const std::size_t curr = charToKey(word[idx]);

      // Option 1: move the finger currently on `prev` to `curr`; `other` stays.
      const int move_active = dist(prev, curr) + self(idx + 1uz, other);
      // Option 2: move the `other` finger to `curr`; `prev` becomes the idle
      // finger for the next character.
      const int move_other = dist(other, curr) + self(idx + 1uz, prev);

      memo[state] = std::min(move_active, move_other);
      return memo[state];
    };

    // Place finger 1 on `word[0]` for free while finger 2 hovers (`kHover`).
    return dfs(1uz, kHover);
  }

 private:
  static constexpr std::size_t kCols = 6uz;
  static constexpr std::size_t kHover = 26uz;
  static constexpr std::size_t kFingerStates = 27uz;

  // Maps an uppercase ASCII letter `'A'..'Z'` to `0..25`.
  static std::size_t charToKey(char ch) {
    return static_cast<std::size_t>(static_cast<unsigned char>(ch) -
                                    static_cast<unsigned char>('A'));
  }

  // Returns Manhattan distance between two keys on the 6-column keyboard, or 0
  // if the source finger has not been placed yet (`kHover`).
  static int dist(std::size_t from, std::size_t to) {
    if (from == kHover) {
      return 0;
    }
    const int from_row = static_cast<int>(from / kCols);
    const int from_col = static_cast<int>(from % kCols);
    const int to_row = static_cast<int>(to / kCols);
    const int to_col = static_cast<int>(to % kCols);
    return std::abs(from_row - to_row) + std::abs(from_col - to_col);
  }
};

LC_PROBLEM(1320, "Minimum Distance To Type A Word Using Two Fingers", "dp",
           ctx) {
  Solution sol;

  ctx.test("example_1_cake", [&] {
    ctx.expect_eq(sol.minimumDistance("CAKE"), 3);
  });

  ctx.test("example_2_happy", [&] {
    ctx.expect_eq(sol.minimumDistance("HAPPY"), 6);
  });

  ctx.test("example_3_new", [&] {
    ctx.expect_eq(sol.minimumDistance("NEW"), 3);
  });

  ctx.test("example_4_year", [&] {
    ctx.expect_eq(sol.minimumDistance("YEAR"), 7);
  });

  ctx.test("edge_cases_short_and_repeated_words", [&] {
    ctx.expect_eq(sol.minimumDistance("AZ"), 0);
    ctx.expect_eq(sol.minimumDistance("AAAA"), 0);
    ctx.expect_eq(sol.minimumDistance("ABABAB"), 0);
  });
}

}  // namespace
