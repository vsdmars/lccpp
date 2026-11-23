// LeetCode 383: Ransom Note
// https://leetcode.com/problems/ransom-note/
//
// Problem:
//   Given two lowercase English strings `ransom_note` and `magazine`, return
//   `true` if `ransom_note` can be constructed by using characters from
//   `magazine`, where each character in `magazine` may be used at most once;
//   otherwise return `false`.
//
// Algorithm:
//   First apply an O(1) length guard: if `ransom_note.size() > magazine.size()`,
//   construction is impossible. Next, count the frequency of each lowercase
//   letter (`'a'`..`'z'`) in `magazine` inside a fixed-size 26-slot array. Then
//   iterate over `ransom_note`, decrementing the corresponding letter's count
//   and returning `false` immediately if any required letter's remaining supply
//   is already zero. If every character is satisfied, return `true`.
//   Time Complexity:  O(m + n), where m = |magazine| and n = |ransom_note|.
//   Space Complexity: O(1) (fixed 26-element frequency table).
//
// Data Structures:
//   - `std::array<int, 26>`: stack-allocated frequency histogram acting as a
//     zero-allocation multiset over the lowercase ASCII alphabet.
//   - `std::string_view`: zero-copy read-only views of both input strings.
//
// Real-World Applications:
//   - Inventory / bill-of-materials feasibility checks (verifying whether a
//     warehouse stock multiset covers all parts in an order).
//   - Resource-quota and token-budget validation before admitting batch jobs.
//   - Word-game tile validation (Scrabble / Boggle rack availability checks)
//     and multiset containment queries in search indexing.
//
// Related LeetCode Problems:
//   - #242 Valid Anagram [done] (checks exact multiset equality rather than multiset subset)
//   - #49 Group Anagrams [done] (buckets strings by their 26-letter frequency signature)
//   - #76 Minimum Window Substring [done] (sliding-window multiset covering check)
//   - #387 First Unique Character in a String [done] (26-slot character frequency histogram)
//   - #1160 Find Words That Can Be Formed by Characters [new] (multi-word variant of magazine-character construction)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <string_view>

namespace {

class Solution {
 public:
  // Returns true iff every character in `ransom_note` appears at least as many
  // times in `magazine` (both strings contain lowercase English letters only).
  // Time: O(m + n), Space: O(1).
  bool canConstruct(std::string_view ransom_note, std::string_view magazine) {
    // Pigeonhole guard: cannot build a longer note from a shorter magazine.
    if (ransom_note.size() > magazine.size()) {
      return false;
    }

    // Tally available character supply from `magazine`.
    std::array<int, kAlphabetSize> available{};
    for (const char ch : magazine) {
      ++available[static_cast<std::size_t>(ch - 'a')];
    }

    // Consume required characters for `ransom_note`; fail fast on deficit.
    for (const char ch : ransom_note) {
      const auto idx = static_cast<std::size_t>(ch - 'a');
      if (available[idx] == 0) {
        return false;
      }
      --available[idx];
    }

    return true;
  }

 private:
  static constexpr std::size_t kAlphabetSize = 26uz;
};

LC_PROBLEM(383, "Ransom Note", "string", ctx) {
  Solution sol;

  ctx.test("example_1_single_mismatch",
           [&] { ctx.expect_false(sol.canConstruct("a", "b")); });

  ctx.test("example_2_insufficient_duplicates",
           [&] { ctx.expect_false(sol.canConstruct("aa", "ab")); });

  ctx.test("example_3_sufficient_duplicates",
           [&] { ctx.expect_true(sol.canConstruct("aa", "aab")); });

  ctx.test("build_case_aaa_from_aab",
           [&] { ctx.expect_false(sol.canConstruct("aaa", "aab")); });

  ctx.test("empty_and_exact_match_edges", [&] {
    ctx.expect_true(sol.canConstruct("", ""));
    ctx.expect_true(sol.canConstruct("", "abc"));
    ctx.expect_false(sol.canConstruct("a", ""));
    ctx.expect_true(sol.canConstruct("listen", "silent"));
  });
}

}  // namespace
