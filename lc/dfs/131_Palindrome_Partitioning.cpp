// LeetCode 131: Palindrome Partitioning
// https://leetcode.com/problems/palindrome-partitioning/
//
// Problem:
//   Given a string `s` of lowercase English letters (1 <= s.length <= 16),
//   partition `s` into non-empty substrings such that every substring in the
//   partition is a palindrome. Return all possible valid palindrome
//   partitionings of `s`.
//
// Examples:
//   Example 1:
//     Input:  s = "aab"
//     Output: [["a", "a", "b"], ["aa", "b"]]
//     Explanation:
//       - Starting at start = 0, dfs takes palindromic prefix "a" (end = 1), then
//         "a" at start = 1 (end = 2), then "b" at start = 2 (end = 3), reaching
//         start == 3 and recording ["a", "a", "b"].
//       - Backtracking to start = 1 tests end = 3 ("ab"), which fails isPalindrome
//         and is skipped.
//       - Backtracking to start = 0 tests end = 2 ("aa"), which passes
//         isPalindrome, recurses to start = 2, takes "b", and records ["aa", "b"].
//       - Trying end = 3 ("aab") at start = 0 fails isPalindrome, so partition
//         returns [["a", "a", "b"], ["aa", "b"]].
//
//   Example 2:
//     Input:  s = "efe"
//     Output: [["e", "f", "e"], ["efe"]]
//     Explanation:
//       - From start = 0, dfs takes "e" (end = 1), recurses through "f" (start = 1,
//         end = 2) and "e" (start = 2, end = 3), and records ["e", "f", "e"], while
//         "fe" at start = 1 fails isPalindrome.
//       - Backtracking to start = 0 skips non-palindrome "ef" (end = 2) and accepts
//         "efe" (end = 3), reaching start == 3 and recording ["efe"].
//
//   Example 3:
//     Input:  s = "a"
//     Output: [["a"]]
//     Explanation:
//       - At start = 0, dfs extracts prefix "a" (end = 1), confirms
//         isPalindrome("a") is true, pushes "a" onto current, and recurses to start
//         = 1.
//       - Because start == s.size() (1 == 1), ["a"] is appended to result and
//         returned.
//
// Algorithm:
//   Depth-First Search (DFS) backtracking over split indices.
//   At recursion index `start`, iterate over every candidate prefix
//   `s[start..end)` (with `end` from `start + 1` to `s.size()`). Whenever that
//   prefix is a palindrome (checked in-place via two pointers on a zero-copy
//   `std::string_view`), push it onto the active `path`, recurse from `end`,
//   and pop it on backtrack. When `start == s.size()`, every character has
//   been covered by valid palindromic pieces, so record a copy of `path`.
//   Time Complexity:  O(N * 2^N) in the worst case (e.g., "aaaa...a" has
//                     2^(N-1) valid partitions, each of total length N).
//   Space Complexity: O(N) auxiliary space for the recursion stack and active
//                     partition path (excluding the O(N * 2^N) output).
//
// Data Structures:
//   - Active partition stack (`std::vector<std::string>`): mutated via
//     push/pop during backtracking to avoid copying the prefix at every node.
//   - Non-owning substring slice (`std::string_view`): zero-allocation
//     palindrome checking prior to committing a branch.
//   - Result list (`std::vector<std::vector<std::string>>`).
//
// Real-World Applications:
//   - Bioinformatics: enumerating palindromic restriction-enzyme recognition
//     motifs and hairpin-forming decompositions in DNA/RNA sequences.
//   - Natural language processing and tokenization: lattice enumeration of all
//     valid segmentations matching a predicate (e.g., CJK word segmentation).
//   - Combinatorial test-case generation for string parsers and compression
//     engines.
//
// Related LeetCode Problems:
//   - #132 Palindrome Partitioning II [done] (minimum cuts using DP in O(N^2))
//   - #93 Restore IP Addresses [done] (constrained backtracking string splits)
//   - #139 Word Break [done] (DP/DFS string segmentation)
//   - #140 Word Break II [done] (enumerate all valid dictionary segmentations)
//   - #647 Palindromic Substrings [done] (counting palindromic substrings)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Returns all partitions of `s` where every substring is a palindrome.
  // Time: O(N * 2^N), Space: O(N) auxiliary stack + path buffer.
  std::vector<std::vector<std::string>> partition(std::string_view s) {
    std::vector<std::vector<std::string>> result;
    std::vector<std::string> current;
    current.reserve(s.size());
    dfs(s, 0uz, current, result);
    return result;
  }

 private:
  // Returns true if `sv` reads identically forward and backward.
  static bool isPalindrome(std::string_view sv) {
    std::size_t left = 0uz;
    std::size_t right = sv.size();
    while (left < right) {
      --right;
      if (sv[left] != sv[right]) {
        return false;
      }
      ++left;
    }
    return true;
  }

  // Explores all palindromic splits of suffix `s[start..]`.
  static void dfs(std::string_view s, std::size_t start,
                  std::vector<std::string>& current,
                  std::vector<std::vector<std::string>>& result) {
    // Base case: entire string partitioned into valid palindromes.
    if (start == s.size()) {
      result.push_back(current);
      return;
    }

    // Try every palindromic prefix starting at `start` and recurse on suffix.
    for (std::size_t end = start + 1uz; end <= s.size(); ++end) {
      const std::string_view prefix = s.substr(start, end - start);
      if (isPalindrome(prefix)) {
        current.emplace_back(prefix);
        dfs(s, end, current, result);
        current.pop_back();
      }
    }
  }
};

LC_PROBLEM(131, "Palindrome Partitioning", "dfs", ctx) {
  Solution sol;

  ctx.test("example_1_aab", [&] {
    const std::vector<std::vector<std::string>> expected = {
        {"a", "a", "b"},
        {"aa", "b"},
    };
    ctx.expect_eq(sol.partition("aab"), expected);
  });

  ctx.test("example_2_single_char", [&] {
    const std::vector<std::vector<std::string>> expected = {{"a"}};
    ctx.expect_eq(sol.partition("a"), expected);
  });

  ctx.test("whole_string_palindrome_efe", [&] {
    const std::vector<std::vector<std::string>> expected = {
        {"e", "f", "e"},
        {"efe"},
    };
    ctx.expect_eq(sol.partition("efe"), expected);
  });

  ctx.test("all_same_characters_aaa", [&] {
    const std::vector<std::vector<std::string>> expected = {
        {"a", "a", "a"},
        {"a", "aa"},
        {"aa", "a"},
        {"aaa"},
    };
    ctx.expect_eq(sol.partition("aaa"), expected);
  });

  ctx.test("no_multi_char_palindromes_abc", [&] {
    const std::vector<std::vector<std::string>> expected = {{"a", "b", "c"}};
    ctx.expect_eq(sol.partition("abc"), expected);
  });
}

}  // namespace
