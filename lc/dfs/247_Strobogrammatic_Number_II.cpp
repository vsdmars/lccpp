// LeetCode 247: Strobogrammatic Number II
// https://leetcode.com/problems/strobogrammatic-number-ii/
//
// Problem:
//   Given an integer `n`, return all strobogrammatic numbers of length `n` in
//   any order. A strobogrammatic number is a string of digits that looks
//   identical when rotated 180 degrees (upside down). Valid single center
//   digits are {'0', '1', '8'}, and valid mirrored outer pairs are
//   {'00', '11', '69', '88', '96'}. Multi-digit numbers (n > 1) must not have
//   leading zeros.
//
// Examples:
//   Example 1:
//     Input:  n = 3
//     Output: ["101", "111", "181", "609", "619", "689", "808", "818", "888",
//             "906", "916", "986"]
//     Explanation:
//       - dfs(3) sees odd len = 3 and recurses on len - 1 = 2 to generate all valid
//         length-2 outer shells.
//       - Base case dfs(2) returns kOuterPairs = ["11", "69", "88", "96"],
//         excluding "00" so multi-digit numbers never start with '0'.
//       - insertAtMidpoint splits each length-2 shell at half = 1 into left and
//         right substrings of length 1.
//       - Inserting every center from kOddCenters = ["0", "1", "8"] between left
//         and right yields the 4 * 3 = 12 sorted strings from "101" through "986".
//
//   Example 2:
//     Input:  n = 2
//     Output: ["11", "69", "88", "96"]
//     Explanation:
//       - findStrobogrammatic(2) passes the n <= 0 guard and invokes dfs(2).
//       - dfs(2) hits the len == 2 base case and returns kOuterPairs = ["11", "69",
//         "88", "96"], omitting "00" to prevent leading zeros.
//
//   Example 3:
//     Input:  n = 1
//     Output: ["0", "1", "8"]
//     Explanation:
//       - findStrobogrammatic(1) invokes dfs(1), which hits the len == 1 base case.
//       - It returns kOddCenters = ["0", "1", "8"] directly because '0' is valid as
//         a standalone single-digit strobogrammatic number.
//
// Algorithm:
//   Recursive DFS building strings by inserting valid central digits/pairs into
//   the exact midpoint of shorter valid strobogrammatic strings:
//   - Base case n == 1: {"0", "1", "8"}.
//   - Base case n == 2: {"11", "69", "88", "96"} ("00" is excluded so no
//     multi-digit number starts with '0').
//   - For odd n > 1: recursively generate all valid strings of length n - 1
//     (which are even-length and have no leading zero), then insert each single
//     self-symmetric digit from {"0", "1", "8"} at the midpoint.
//   - For even n > 2: recursively generate all valid strings of length n - 2
//     (which already exclude leading zeros), then insert each mirrored pair
//     from {"00", "11", "69", "88", "96"} at the midpoint.
//   Time Complexity: O(n * 5^(floor(n / 2))), proportional to total output size.
//   Space Complexity: O(n * 5^(floor(n / 2))) for storing intermediate and
//     final strings, plus O(n) recursion stack depth.
//
// Data Structures:
//   - std::array<std::string_view, ...>: compile-time tables of valid center
//     digits and mirrored digit pairs.
//   - std::vector<std::string>: stores generated strobogrammatic numbers at
//     each recursive level.
//
// Real-World Applications:
//   - Designing ambigram / rotational-symmetry typography, security watermarks,
//     and tamper-evident serial codes on physical labels.
//   - Combinatorial enumeration of palindromic or complement-symmetric DNA/RNA
//     motifs (reverse-complement sequences in bioinformatics).
//   - Generating symmetric test vectors for optical character recognition (OCR)
//     and seven-segment display validation.
//
// Related LeetCode Problems:
//   - #246 Strobogrammatic Number [done] (two-pointer validation of a single string)
//   - #248 Strobogrammatic Number III [new] (counting strobogrammatic numbers in a range)
//   - #1056 Confusing Number [new] (checking if 180-degree rotation yields a different number)
//   - #1088 Confusing Number II [done] (DFS counting all confusing numbers up to N)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

// Self-symmetric digits valid in the exact middle of an odd-length number.
constexpr std::array<std::string_view, 3> kOddCenters = {"0", "1", "8"};

// Mirrored digit pairs valid strictly inside a multi-digit number.
constexpr std::array<std::string_view, 5> kEvenCenters = {"00", "11", "69",
                                                          "88", "96"};

// Non-zero-leading mirrored pairs for the base case n == 2.
constexpr std::array<std::string_view, 4> kOuterPairs = {"11", "69", "88",
                                                         "96"};

class Solution {
 public:
  // Generates all strobogrammatic numbers of exact length `n`.
  //
  // Invariant: Every string returned by `dfs(k)` has length `k`, is invariant
  // under 180-degree rotation, and has no leading '0' whenever k > 1.
  //
  // Time: O(n * 5^(floor(n / 2))); Space: O(n * 5^(floor(n / 2))).
  std::vector<std::string> findStrobogrammatic(int n) {
    if (n <= 0) {
      return {};
    }

    auto dfs = [](this auto&& self, int len) -> std::vector<std::string> {
      // Base case: single-digit strobogrammatic numbers (0 is allowed alone).
      if (len == 1) {
        return {kOddCenters.begin(), kOddCenters.end()};
      }
      // Base case: length-2 numbers exclude "00" to prevent leading zeros.
      if (len == 2) {
        return {kOuterPairs.begin(), kOuterPairs.end()};
      }

      // Odd lengths insert a 1-digit center into an (len - 1) outer shell;
      // even lengths insert a 2-digit pair (including "00") into an (len - 2)
      // outer shell.
      if ((len % 2) != 0) {
        const std::vector<std::string> outers = self(len - 1);
        return insertAtMidpoint(outers, kOddCenters);
      }

      const std::vector<std::string> outers = self(len - 2);
      return insertAtMidpoint(outers, kEvenCenters);
    };

    return dfs(n);
  }

 private:
  // Inserts every string in `centers` at the midpoint of every string in
  // `outers`, producing `outers.size() * centers.size()` results.
  template <std::size_t K>
  static std::vector<std::string> insertAtMidpoint(
      const std::vector<std::string>& outers,
      const std::array<std::string_view, K>& centers) {
    std::vector<std::string> result;
    result.reserve(outers.size() * centers.size());

    for (const std::string& outer : outers) {
      const std::string_view view(outer);
      const std::size_t half = view.size() / 2uz;
      const std::string_view left = view.substr(0uz, half);
      const std::string_view right = view.substr(half);

      for (const std::string_view mid : centers) {
        std::string combined;
        combined.reserve(view.size() + mid.size());
        combined.append(left);
        combined.append(mid);
        combined.append(right);
        result.push_back(std::move(combined));
      }
    }

    return result;
  }
};

// Verifies that `s` is a valid strobogrammatic number of length `n`.
bool isValidStrobogrammatic(std::string_view s, std::size_t n) {
  if (s.size() != n) {
    return false;
  }
  if (n == 0uz) {
    return true;
  }
  if (n > 1uz && s[0] == '0') {
    return false;
  }

  auto rotate = [](char ch) -> char {
    switch (ch) {
      case '0':
      case '1':
      case '8':
        return ch;
      case '6':
        return '9';
      case '9':
        return '6';
      default:
        return '?';
    }
  };

  for (std::size_t i = 0uz; i <= s.size() / 2uz; ++i) {
    if (rotate(s[i]) != s[s.size() - 1uz - i]) {
      return false;
    }
  }
  return true;
}

LC_PROBLEM(247, "Strobogrammatic Number II", "dfs", ctx) {
  ctx.test("n_equals_1", [&] {
    Solution sol;
    std::vector<std::string> actual = sol.findStrobogrammatic(1);
    std::ranges::sort(actual);
    ctx.expect_eq(actual, std::vector<std::string>{"0", "1", "8"});
  });

  ctx.test("n_equals_2", [&] {
    Solution sol;
    std::vector<std::string> actual = sol.findStrobogrammatic(2);
    std::ranges::sort(actual);
    ctx.expect_eq(actual, std::vector<std::string>{"11", "69", "88", "96"});
  });

  ctx.test("n_equals_3", [&] {
    Solution sol;
    std::vector<std::string> actual = sol.findStrobogrammatic(3);
    std::ranges::sort(actual);
    const std::vector<std::string> expected = {
        "101", "111", "181", "609", "619", "689",
        "808", "818", "888", "906", "916", "986"};
    ctx.expect_eq(actual, expected);
  });

  ctx.test("n_equals_4_count_and_invariants", [&] {
    Solution sol;
    std::vector<std::string> actual = sol.findStrobogrammatic(4);
    std::ranges::sort(actual);
    // 4 outer pairs * 5 inner pairs = 20 unique strings.
    ctx.expect_eq(actual.size(), 20uz);
    ctx.expect_true(std::ranges::adjacent_find(actual) == actual.end());
    ctx.expect_true(std::ranges::all_of(
        actual, [](const std::string& s) { return isValidStrobogrammatic(s, 4uz); }));
    ctx.expect_true(std::ranges::binary_search(actual, "1001"));
    ctx.expect_true(std::ranges::binary_search(actual, "6009"));
    ctx.expect_true(std::ranges::binary_search(actual, "9696"));
  });

  ctx.test("n_equals_5_count_and_invariants", [&] {
    Solution sol;
    std::vector<std::string> actual = sol.findStrobogrammatic(5);
    std::ranges::sort(actual);
    // 4 * 5 * 3 = 60 unique strings.
    ctx.expect_eq(actual.size(), 60uz);
    ctx.expect_true(std::ranges::adjacent_find(actual) == actual.end());
    ctx.expect_true(std::ranges::all_of(
        actual, [](const std::string& s) { return isValidStrobogrammatic(s, 5uz); }));
  });
}

}  // namespace
