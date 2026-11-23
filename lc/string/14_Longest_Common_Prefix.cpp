// LeetCode 14: Longest Common Prefix
// https://leetcode.com/problems/longest-common-prefix/
//
// Problem:
//   Given an array of strings `strs`, find and return the longest common prefix
//   shared by every string in `strs`. If no common prefix exists, return `""`.
//
//   Constraints:
//   - 1 <= strs.length <= 200
//   - 0 <= strs[i].length <= 200
//   - strs[i] consists of only lowercase English letters (if non-empty)
//
// Algorithm:
//   Vertical column-by-column scanning (the direct O(1)-extra-space equivalent
//   of Python's `zip(*strs)`):
//   1. If `strs` is empty, return `""`.
//   2. For each column index `col` from `0` to `strs[0].size() - 1`, take
//      `expected = strs[0][col]` and compare it against `strs[row][col]` for
//      every remaining string `row` in `[1, strs.size() - 1]`.
//   3. As soon as any string terminates (`col == strs[row].size()`) or differs
//      (`strs[row][col] != expected`), the prefix `[0, col)` is maximal—return
//      `strs[0].substr(0, col)` immediately.
//   4. If the outer loop completes without a mismatch, all of `strs[0]` is a
//      prefix of every string in `strs`.
//
//   Time Complexity:  O(S), where S is the sum of lengths of all strings (in
//                     practice O(n * L_min) where L_min is the shortest string
//                     or first mismatch column, exiting early on divergence).
//   Space Complexity: O(1) auxiliary space (excluding the returned substring).
//
// Data Structures:
//   - Input `std::vector<std::string>` accessed by `(row, col)` indices without
//     building auxiliary tries or hash sets.
//
// Real-World Applications:
//   - Shell and IDE tab-completion computing the shared stem across candidate
//     commands or symbols.
//   - Filesystem path canonicalization and computing the lowest common base
//     directory of a batch of file paths.
//   - Prefix compression in B-trees, SSTables (RocksDB/LevelDB), and IP/URL
//     radix routing tables.
//
// Related LeetCode Problems:
//   - #208 Implement Trie [done] (Prefix Tree) (prefix-indexed string structure)
//   - #1233 Remove Sub-Folders from the Filesystem [done] (path prefix filtering)
//   - #2416 Sum of Prefix Scores of Strings [new] (prefix frequency aggregation)
//   - #3043 Find the Length of the Longest Common Prefix [new] (LCP across two arrays)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the longest common prefix of all strings in `strs` via vertical scan.
  // Invariant: before checking column `col`, all strings agree on `[0, col)`.
  // Time: O(S), Space: O(1) auxiliary.
  std::string longestCommonPrefix(const std::vector<std::string>& strs) {
    if (strs.empty()) {
      return {};
    }

    const std::string& first = strs[0];
    for (std::size_t col = 0uz; col < first.size(); ++col) {
      const char expected = first[col];
      for (std::size_t row = 1uz; row < strs.size(); ++row) {
        // Stop immediately when any string runs out of characters or diverges.
        if (col == strs[row].size() || strs[row][col] != expected) {
          return first.substr(0uz, col);
        }
      }
    }

    // Every character of `first` matched across all strings.
    return first;
  }
};

LC_PROBLEM(14, "Longest Common Prefix", "string", ctx) {
  Solution sol;

  ctx.test("example1_shared_prefix", [&] {
    ctx.expect_eq(sol.longestCommonPrefix({"flower", "flow", "flight"}), "fl");
  });

  ctx.test("example2_no_common_prefix", [&] {
    ctx.expect_eq(sol.longestCommonPrefix({"dog", "racecar", "car"}), "");
  });

  ctx.test("shorter_string_bounds_prefix", [&] {
    ctx.expect_eq(sol.longestCommonPrefix({"abab", "aba", "aba"}), "aba");
    ctx.expect_eq(sol.longestCommonPrefix({"a", "ab", "abc"}), "a");
  });

  ctx.test("edge_cases", [&] {
    ctx.expect_eq(sol.longestCommonPrefix({}), "");
    ctx.expect_eq(sol.longestCommonPrefix({"alone"}), "alone");
    ctx.expect_eq(sol.longestCommonPrefix({"abc", "", "ab"}), "");
    ctx.expect_eq(sol.longestCommonPrefix({"same", "same", "same"}), "same");
  });
}

}  // namespace
