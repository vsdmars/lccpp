// LeetCode 784: Letter Case Permutation
// https://leetcode.com/problems/letter-case-permutation/
//
// Problem:
//   Given an alphanumeric string `s` (1 <= s.size() <= 12), we may transform
//   every English letter independently into either lowercase or uppercase while
//   leaving digits unchanged. Return a list of all possible strings that can be
//   created (in any order).
//
// Examples:
//   Example 1:
//     Input:  s = "a1b2"
//     Output: ["a1b2", "a1B2", "A1b2", "A1B2"]
//     Explanation:
//       - Count `letter_count = 2` (`'a'`, `'b'`), reserve `2^2 = 4` slots in
//         `result`, and initialize `current = "a1b2"`.
//       - At `idx = 0` (`'a'`), `dfs` first sets `current[0] = 'a'` and recurses
//         past digit `'1'` at `idx = 1` to `idx = 2`.
//       - At `idx = 2` (`'b'`), `dfs` branches on `'b'` and `'B'`, each skipping
//         digit `'2'` at `idx = 3` to reach `idx = 4` and append `"a1b2"` and
//         `"a1B2"`.
//       - Backtracking to `idx = 0`, `dfs` sets `current[0] = 'A'` and repeats the
//         two choices at `idx = 2`, appending `"A1b2"` and `"A1B2"`.
//
//   Example 2:
//     Input:  s = "3z4"
//     Output: ["3z4", "3Z4"]
//     Explanation:
//       - Count `letter_count = 1` (`'z'`), reserve `2` slots in `result`, and
//         start `dfs(0)` with `current = "3z4"`.
//       - At `idx = 0`, `std::isalpha('3')` is `0`, so `dfs` keeps `'3'` unchanged
//         and recurses directly to `idx = 1`.
//       - At `idx = 1`, `dfs` sets `current[1]` to `'z'` and then `'Z'`, passing
//         digit `'4'` at `idx = 2` in each branch to push `"3z4"` and `"3Z4"` at
//         `idx = 3`.
//
//   Example 3:
//     Input:  s = "12345"
//     Output: ["12345"]
//     Explanation:
//       - Scan `s` to find `letter_count = 0` and reserve `2^0 = 1` slot in
//         `result`.
//       - Recurse through `idx = 0..4` without branching because `std::isalpha`
//         returns `0` for every digit, then push `"12345"` at `idx = 5`.
//
// Algorithm:
//   In-place DFS backtracking over character positions `0..s.size()-1`.
//   - If `s[idx]` is not an ASCII letter (e.g., a digit), there is only one
//     choice: keep it as-is and recurse to `idx + 1`.
//   - If `s[idx]` is an ASCII letter, branch into two recursive calls: first
//     setting `current[idx]` to lowercase, then to uppercase.
//   - When `idx == current.size()`, append the completed string `current` to
//     `result`.
//   This generates the exact Cartesian product of per-position character
//   choices while mutating a single working buffer instead of copying prefixes
//   at every internal node.
//
//   Time Complexity:  O(n * 2^L), where `n = s.size()` and `L <= n` is the
//                     number of alphabetic characters in `s` (2^L output
//                     strings, each of length `n`).
//   Space Complexity: O(n) auxiliary space for the working string and recursion
//                     stack (excluding the O(n * 2^L) output vector).
//
// Data Structures:
//   - std::string (`current`): Mutable in-place character buffer representing
//     the active DFS path.
//   - std::vector<std::string> (`result`): Pre-reserved output container
//     collecting every generated permutation.
//
// Real-World Applications:
//   - Generating case-variant test inputs and fuzzing corpora for
//     case-sensitive parsers, file systems, and HTTP/DNS handlers.
//   - Expanding small configuration or template matrices (Cartesian product of
//     binary feature flags) for combinatorial test suites.
//   - Password/token normalization testing and security auditing of
//     case-insensitive vs. case-sensitive authentication flows.
//
// Related LeetCode Problems:
//   - #17 Letter Combinations of a Phone Number [done] (Cartesian product over per-digit letter sets)
//   - #78 Subsets [done] (binary include/exclude backtracking of size 2^n)
//   - #46 Permutations [done] (order-based backtracking over characters/elements)
//   - #1087 Brace Expansion [new] (Cartesian product over explicit character option groups)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Generates all case-variant strings of `s` by branching lowercase/uppercase
  // on each alphabetic character and leaving digits fixed.
  //
  // Time: O(n * 2^L), Space: O(n) auxiliary (L = number of letters in `s`).
  std::vector<std::string> letterCasePermutation(std::string_view s) {
    // Count letters to pre-reserve the exact output size (2^L) when small.
    std::size_t letter_count = 0;
    for (const char ch : s) {
      if (std::isalpha(static_cast<unsigned char>(ch)) != 0) {
        ++letter_count;
      }
    }

    std::vector<std::string> result;
    if (letter_count <= 20uz) {
      result.reserve(1uz << letter_count);
    }

    std::string current(s);

    // Recursively assigns character choices from `idx` to the end of `current`.
    auto dfs = [&](this auto&& self, std::size_t idx) -> void {
      if (idx == current.size()) {
        result.push_back(current);
        return;
      }

      // Cast through unsigned char before <cctype> calls to avoid UB on
      // negative signed char values.
      const auto uch = static_cast<unsigned char>(current[idx]);
      if (std::isalpha(uch) == 0) {
        self(idx + 1);
        return;
      }

      current[idx] = static_cast<char>(std::tolower(uch));
      self(idx + 1);

      current[idx] = static_cast<char>(std::toupper(uch));
      self(idx + 1);
    };

    dfs(0uz);
    return result;
  }
};

std::vector<std::string> sorted_copy(std::vector<std::string> values) {
  std::ranges::sort(values);
  return values;
}

LC_PROBLEM(784, "Letter Case Permutation", "backtracking", ctx) {
  ctx.test("example_mixed_letters_and_digits", [&] {
    Solution sol;
    const std::vector<std::string> expected = {"a1b2", "a1B2", "A1b2", "A1B2"};
    ctx.expect_eq(sorted_copy(sol.letterCasePermutation("a1b2")),
                  sorted_copy(expected));
  });

  ctx.test("example_single_letter", [&] {
    Solution sol;
    const std::vector<std::string> expected = {"3z4", "3Z4"};
    ctx.expect_eq(sorted_copy(sol.letterCasePermutation("3z4")),
                  sorted_copy(expected));
  });

  ctx.test("example_digits_only", [&] {
    Solution sol;
    const std::vector<std::string> expected = {"12345"};
    ctx.expect_eq(sol.letterCasePermutation("12345"), expected);
  });

  ctx.test("uppercase_input_normalized", [&] {
    Solution sol;
    const std::vector<std::string> expected = {"c", "C"};
    ctx.expect_eq(sorted_copy(sol.letterCasePermutation("C")),
                  sorted_copy(expected));
  });

  ctx.test("all_letters_three_chars", [&] {
    Solution sol;
    const auto actual = sol.letterCasePermutation("AbC");
    ctx.expect_eq(actual.size(), 8uz);
    const std::vector<std::string> expected = {
        "abc", "abC", "aBc", "aBC", "Abc", "AbC", "ABc", "ABC",
    };
    ctx.expect_eq(sorted_copy(actual), sorted_copy(expected));
  });

  ctx.test("empty_string", [&] {
    Solution sol;
    const std::vector<std::string> expected = {""};
    ctx.expect_eq(sol.letterCasePermutation(""), expected);
  });
}

}  // namespace
