// LeetCode 1347: Minimum Number Of Steps To Make Two Strings Anagram
// https://leetcode.com/problems/minimum-number-of-steps-to-make-two-strings-anagram/
//
// Problem:
//   Given two equal-length strings `s` and `t` consisting of lowercase English
//   letters (1 <= s.length <= 50000), in one step you may choose any character
//   of `t` and replace it with any other lowercase letter. Return the minimum
//   number of steps required to make `t` an anagram of `s`.
//
// Algorithm:
//   Maintain a fixed 26-element net frequency array `balance` indexed by
//   `ch - 'a'`. Increment `balance` for every character in `s` and decrement
//   `balance` for every character in `t`. After scanning both strings:
//     - Positive entries `balance[c] > 0` measure how many copies of letter `c`
//       are missing from `t` relative to `s`.
//     - Negative entries `balance[c] < 0` measure surplus characters in `t`.
//   Because `s.size() == t.size()`, the sum of all positive entries equals the
//   sum of the absolute values of all negative entries. Each replacement in `t`
//   simultaneously eliminates one surplus character and supplies one missing
//   character, so summing only the positive entries avoids double-counting and
//   yields the exact minimum number of steps.
//   Time Complexity:  O(N), where N = s.size().
//   Space Complexity: O(1) (fixed 26-integer array).
//
// Data Structures:
//   - std::array<int, 26>: cache-resident alphabet frequency delta table.
//
// Real-World Applications:
//   - Bag-of-words / multiset edit distance (L1 histogram distance) in NLP,
//     telemetry drift detection, and approximate string matching.
//   - Inventory rebalancing and resource-allocation diffing where each unit
//     reassignment moves one surplus item to one deficit slot.
//
// Related LeetCode Problems:
//   - #242 Valid Anagram [done] (checking whether two strings have zero frequency delta)
//   - #438 Find All Anagrams in a String [done] (sliding-window 26-letter frequency balance)
//   - #2186 Minimum Number of Steps to Make Two Strings Anagram II [new] (insertions allowed in both strings)
//   - #1657 Determine if Two Strings Are Close [new] (anagram equivalence up to character relabeling)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <string_view>

namespace {

constexpr std::size_t kAlphabetSize = 26uz;

class Solution {
 public:
  // Returns the minimum number of character replacements in `t` needed to make
  // `t` an anagram of `s`.
  //
  // Time Complexity:  O(N)
  // Space Complexity: O(1)
  int minSteps(std::string_view s, std::string_view t) {
    std::array<int, kAlphabetSize> balance{};

    // Compute per-letter deficit (s[i] contributes +1, t[i] contributes -1).
    for (std::size_t i = 0uz; i < s.size(); ++i) {
      ++balance[static_cast<std::size_t>(s[i] - 'a')];
      --balance[static_cast<std::size_t>(t[i] - 'a')];
    }

    // Sum only positive deficits; counting negative surpluses as well would
    // double-count every replacement.
    int steps = 0;
    for (const int delta : balance) {
      if (delta > 0) {
        steps += delta;
      }
    }
    return steps;
  }
};

LC_PROBLEM(1347, "Minimum Number Of Steps To Make Two Strings Anagram",
           "string", ctx) {
  ctx.test("example_1_build_case", [&] {
    Solution sol;
    ctx.expect_eq(sol.minSteps("bab", "aba"), 1);
  });

  ctx.test("example_2", [&] {
    Solution sol;
    ctx.expect_eq(sol.minSteps("leetcode", "practice"), 5);
  });

  ctx.test("example_3_already_anagram", [&] {
    Solution sol;
    ctx.expect_eq(sol.minSteps("anagram", "mangaar"), 0);
  });

  ctx.test("example_4_identical", [&] {
    Solution sol;
    ctx.expect_eq(sol.minSteps("xxyyzz", "xxyyzz"), 0);
  });

  ctx.test("example_5", [&] {
    Solution sol;
    ctx.expect_eq(sol.minSteps("friend", "family"), 4);
  });

  ctx.test("edge_cases", [&] {
    Solution sol;
    ctx.expect_eq(sol.minSteps("a", "a"), 0);
    ctx.expect_eq(sol.minSteps("a", "b"), 1);
    ctx.expect_eq(sol.minSteps("aaaa", "bbbb"), 4);
  });
}

}  // namespace
