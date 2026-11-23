// LeetCode 756: Pyramid Transition Matrix
// https://leetcode.com/problems/pyramid-transition-matrix/
//
// Problem:
//   You are stacking colored blocks (single uppercase letters) to form a
//   pyramid starting from a given `bottom` row string. A block `C` may be
//   placed directly on top of an adjacent left block `A` and right block `B`
//   only if the 3-letter string `"ABC"` appears in `allowed`. Return true if
//   the pyramid can be built all the way to a single apex block, and false
//   otherwise.
//   Constraints: 2 <= bottom.size() <= 6, 0 <= allowed.size() <= 216,
//   allowed[i].size() == 3.
//
// Examples:
//   Example 1:
//     Input:  bottom = "XYZ", allowed = ["XYD", "YZE", "DEA", "FFF"]
//     Output: true
//     Explanation:
//       - pyramidTransition builds transitions with ('X','Y') -> "D", ('Y','Z') ->
//         "E", ('D','E') -> "A", and ('F','F') -> "F".
//       - can_build_pyramid("XYZ") runs build_next_row across pos = 0 ("XY" -> 'D')
//         and pos = 1 ("YZ" -> 'E'), producing next_row = "DE".
//       - can_build_pyramid("DE") maps ('D','E') -> 'A' to form next_row = "A".
//       - can_build_pyramid("A") reaches the base case row.size() == 1uz and
//         unwinds true.
//
//   Example 2:
//     Input:  bottom = "XXYX", allowed = ["XXX", "XXY", "XYX", "XYY", "YXZ"]
//     Output: false
//     Explanation:
//       - pyramidTransition populates transitions with ('X','X') -> "XY", ('X','Y')
//         -> "XY", and ('Y','X') -> "Z".
//       - From "XXYX", build_next_row combines pairs ("XX", "XY", "YX") to test
//         every candidate next row: "XXZ", "XYZ", "YXZ", and "YYZ".
//       - Each candidate ends in "XZ" or "YZ", which has no entry in transitions,
//         so each 3-letter row fails and is memoized in failed_rows.
//       - After all branches return false, "XXYX" is inserted into failed_rows and
//         pyramidTransition returns false.
//
//   Example 3:
//     Input:  bottom = "ABC", allowed = ["ABD", "ABE", "BCF", "EFG"]
//     Output: true
//     Explanation:
//       - pyramidTransition maps ('A','B') -> "DE", ('B','C') -> "F", and ('E','F')
//         -> "G".
//       - Starting from "ABC", build_next_row first picks 'D' for "AB" and 'F' for
//         "BC", calling can_build_pyramid("DF") which finds no transition for
//         ('D','F') and records "DF" in failed_rows.
//       - Backtracking at pos = 0 pops 'D', selects 'E' for "AB", and pairs it with
//         'F' from "BC" to build next_row = "EF".
//       - can_build_pyramid("EF") maps ('E','F') -> 'G' to reach apex "G" (size 1)
//         and returns true.
//
// Algorithm:
//   Preprocess `allowed` into a 2D transition lookup table `transitions[a][b]`
//   storing all valid top characters `c` that can sit on `(a, b)`.
//   Perform memoized DFS (`can_build_pyramid`) row by row:
//     - Base case: a row of length 1 is the apex; return true.
//     - Memoization: if `row` is already recorded in `failed_rows`, return
//       false immediately (any row that succeeds unwinds immediately to true,
//       so only dead-end rows ever need to be cached).
//     - Row construction (`build_next_row`): recursively assemble candidate
//       next rows of length `row.size() - 1` one adjacent pair at a time using
//       backtracking (`push_back`/`pop_back`). Whenever a full next row is
//       completed, recurse into `can_build_pyramid(next_row)`. Short-circuit
//       as soon as any branch reaches the apex; if none succeed, memoize `row`
//       in `failed_rows`.
//   Building candidate rows incrementally via backtracking avoids eagerly
//   materializing the entire Cartesian product when an early pair has no
//   allowed transition or an early branch already succeeds.
//   Time Complexity: O(A^N) worst-case number of distinct row states explored,
//     where `A` is the alphabet size (<= 6 in LeetCode, <= 26 general) and
//     `N = bottom.size()`.
//   Space Complexity: O(A^N) for the memoized dead-end row set plus O(N^2)
//     for the recursion stack.
//
// Data Structures:
//   - std::array<std::array<std::string, 26>, 26> `transitions`: O(1) lookup
//     from base pair `(A, B)` to all allowed top characters `C`.
//   - std::unordered_set<std::string> `failed_rows`: memoization table of row
//     strings proven unable to reach the apex.
//
// Real-World Applications:
//   - Bottom-up parsing (CYK / chart parsing) for context-free grammars where
//     adjacent non-terminals reduce to parent symbols via production rules.
//   - Cellular automata reachability analysis (determining whether a starting
//     configuration can evolve into a target state under local rewrite rules).
//   - Constraint satisfaction and rule-based synthesis in EDA / logic layout.
//
// Related LeetCode Problems:
//   - #139 Word Break [done] (memoized DFS / top-down DP over string states)
//   - #140 Word Break II [done] (backtracking with memoization over string prefixes)
//   - #464 Can I Win [new] (memoized state-space DFS with dead-end caching)
//   - #679 24 Game [done] (recursive reduction of a sequence to a single target)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace {

class Solution {
 public:
  // Returns true if a valid pyramid can be stacked from `bottom` to a single
  // top block using only triples in `allowed`.
  // Time: O(A^N) memoized states; Space: O(A^N).
  bool pyramidTransition(std::string_view bottom,
                         const std::vector<std::string>& allowed) {
    if (bottom.empty()) {
      return false;
    }

    // Build an O(1) lookup table mapping base pair (left, right) -> valid tops.
    TransitionTable transitions{};
    for (const std::string& triple : allowed) {
      if (triple.size() != 3uz) {
        continue;
      }
      const auto left = to_index(triple[0]);
      const auto right = to_index(triple[1]);
      transitions[left][right].push_back(triple[2]);
    }

    std::unordered_set<std::string> failed_rows;
    return can_build_pyramid(std::string(bottom), transitions, failed_rows);
  }

 private:
  static constexpr std::size_t kAlphabetSize = 26uz;
  using TransitionTable =
      std::array<std::array<std::string, kAlphabetSize>, kAlphabetSize>;

  static std::size_t to_index(char ch) {
    return static_cast<std::size_t>(ch - 'A');
  }

  // Returns true if `row` can be reduced all the way to a 1-block apex.
  static bool can_build_pyramid(const std::string& row,
                                const TransitionTable& transitions,
                                std::unordered_set<std::string>& failed_rows) {
    if (row.size() == 1uz) {
      return true;
    }
    if (failed_rows.contains(row)) {
      return false;
    }

    std::string next_row;
    next_row.reserve(row.size() - 1uz);
    if (build_next_row(row, 0uz, next_row, transitions, failed_rows)) {
      return true;
    }

    // Cache dead-end rows so identical subproblems are never re-explored.
    failed_rows.insert(row);
    return false;
  }

  // Backtracks across adjacent pairs `(row[pos], row[pos + 1])` to construct
  // every valid next row one block at a time and test it immediately.
  static bool build_next_row(const std::string& row, std::size_t pos,
                             std::string& next_row,
                             const TransitionTable& transitions,
                             std::unordered_set<std::string>& failed_rows) {
    if ((pos + 1uz) == row.size()) {
      return can_build_pyramid(next_row, transitions, failed_rows);
    }

    const std::string& candidates =
        transitions[to_index(row[pos])][to_index(row[pos + 1uz])];
    for (const char top : candidates) {
      next_row.push_back(top);
      const bool ok =
          build_next_row(row, pos + 1uz, next_row, transitions, failed_rows);
      next_row.pop_back();
      if (ok) {
        return true;
      }
    }
    return false;
  }
};

LC_PROBLEM(756, "Pyramid Transition Matrix", "dfs", ctx) {
  ctx.test("example_1_xyz_valid", [&] {
    Solution sol;
    ctx.expect_true(
        sol.pyramidTransition("XYZ", {"XYD", "YZE", "DEA", "FFF"}));
  });

  ctx.test("example_2_xxyx_invalid", [&] {
    Solution sol;
    ctx.expect_false(
        sol.pyramidTransition("XXYX", {"XXX", "XXY", "XYX", "XYY", "YXZ"}));
  });

  ctx.test("leetcode_bcd_valid", [&] {
    Solution sol;
    ctx.expect_true(
        sol.pyramidTransition("BCD", {"BCC", "CDE", "CEA", "FFF"}));
  });

  ctx.test("leetcode_aaaa_invalid", [&] {
    Solution sol;
    ctx.expect_false(
        sol.pyramidTransition("AAAA", {"AAB", "AAC", "BCD", "BBE", "DEF"}));
  });

  ctx.test("single_block_already_apex", [&] {
    Solution sol;
    ctx.expect_true(sol.pyramidTransition("A", {}));
  });

  ctx.test("backtracking_across_multiple_candidates", [&] {
    Solution sol;
    // AB can map to D or E, BC maps to F; DF has no transition, EF maps to G.
    ctx.expect_true(
        sol.pyramidTransition("ABC", {"ABD", "ABE", "BCF", "EFG"}));
  });
}

}  // namespace
