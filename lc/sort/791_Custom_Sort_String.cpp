// LeetCode 791: Custom Sort String
// https://leetcode.com/problems/custom-sort-string/
//
// Problem:
//   You are given two lowercase English strings `order` and `s`. All characters
//   of `order` are unique and were sorted in a custom order. Permute the
//   characters of `s` so that if character `x` appears before character `y` in
//   `order`, then `x` also appears before `y` in the returned permutation.
//   Characters of `s` that do not appear in `order` may be placed anywhere.
//   Constraints: `1 <= order.length <= 26`, `1 <= s.length <= 200`.
//
// Algorithm:
//   1. Custom Comparator Rank Sort (`customSortString`):
//      Assign every character in `order` its integer position `0 .. |order|-1`
//      in a fixed 26-entry lookup table, defaulting unranked letters to `26`
//      (so they sort after all ranked letters). Sort `s` using this rank table.
//      Time Complexity:  O(|order| + |s| log |s|).
//      Space Complexity: O(1) auxiliary space (26-element array + O(log |s|)
//                        sort stack).
//   2. Linear Counting Sort (`customSortStringCounting`):
//      Count letter frequencies in `s` using a 26-element array. First emit all
//      occurrences of each character in `order` in sequence (zeroing its count),
//      then emit all remaining unranked characters.
//      Time Complexity:  O(|order| + |s|).
//      Space Complexity: O(1) auxiliary space (excluding output string).
//
// Data Structures:
//   - Fixed-size rank/frequency table (`std::array<int, 26>`): provides O(1)
//     cache-local lookup per lowercase ASCII letter without heap allocation.
//
// Real-World Applications:
//   - Locale-specific collation (ICU / Unicode UCA) where languages define
//     custom alphabetical precedence differing from raw ASCII code-point order.
//   - Database `ORDER BY FIELD(...)` / enum-priority sorting in SQL query
//     engines and UI table sorters.
//   - Canonicalizing configuration keys, HTTP headers, or compiler pass tags
//     into a domain-defined priority order.
//
// Related LeetCode Problems:
//   - #1122 Relative Sort Array [new] (same custom-order + tail-order pattern for integers)
//   - #953 Verifying an Alien Dictionary [done] (validating lexicographic order under a custom alphabet)
//   - #269 Alien Dictionary [done] (inferring a custom character order via topological sort)
//   - #451 Sort Characters By Frequency [done] (bucket/counting-based character permutation)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <string>
#include <string_view>

namespace {

constexpr std::size_t kAlphabetSize = 26;

// Converts a lowercase ASCII letter `'a'..'z'` to an index in `[0, 25]`
// using unsigned arithmetic to avoid signed-char pitfalls.
constexpr std::size_t to_index(char ch) {
  return static_cast<std::size_t>(static_cast<unsigned char>(ch) -
                                  static_cast<unsigned char>('a'));
}

class Solution {
 public:
  // Permutes `s` to respect the relative character order in `order` via a
  // custom rank-table comparator.
  //
  // Time: O(|order| + |s| log |s|), Space: O(1) auxiliary.
  std::string customSortString(std::string_view order, std::string s) {
    // Initialize every letter's rank to `kAlphabetSize` so letters absent from
    // `order` deterministically trail all ranked letters.
    std::array<int, kAlphabetSize> rank{};
    rank.fill(static_cast<int>(kAlphabetSize));

    for (std::size_t idx = 0; idx < order.size(); ++idx) {
      rank[to_index(order[idx])] = static_cast<int>(idx);
    }

    // Sort characters of `s` in-place by their custom rank.
    std::ranges::sort(s, [&](char lhs, char rhs) {
      return rank[to_index(lhs)] < rank[to_index(rhs)];
    });

    return s;
  }

  // Linear-time counting-sort alternative: counts character frequencies in `s`,
  // emits characters in `order` order first, then appends any remaining letters.
  //
  // Time: O(|order| + |s|), Space: O(1) auxiliary (excluding output).
  std::string customSortStringCounting(std::string_view order,
                                       std::string_view s) {
    std::array<std::size_t, kAlphabetSize> counts{};
    for (const char ch : s) {
      ++counts[to_index(ch)];
    }

    std::string result;
    result.reserve(s.size());

    // Phase 1: Emit all ranked characters in the exact sequence given by `order`.
    for (const char ch : order) {
      const std::size_t idx = to_index(ch);
      result.append(counts[idx], ch);
      counts[idx] = 0;
    }

    // Phase 2: Append all remaining unranked characters.
    for (std::size_t idx = 0; idx < kAlphabetSize; ++idx) {
      if (counts[idx] != 0) {
        result.append(counts[idx],
                      static_cast<char>(static_cast<unsigned char>('a') + idx));
      }
    }

    return result;
  }
};

// Verifies that `result` is a valid permutation of `original` whose characters
// appearing in `order` occur in non-decreasing rank order.
bool is_valid_custom_sort(std::string_view order, std::string_view original,
                          std::string_view result) {
  if (original.size() != result.size()) {
    return false;
  }

  // Check multiset equality.
  std::string sorted_orig(original);
  std::string sorted_res(result);
  std::ranges::sort(sorted_orig);
  std::ranges::sort(sorted_res);
  if (sorted_orig != sorted_res) {
    return false;
  }

  // Check relative order of all characters present in `order`.
  std::array<int, kAlphabetSize> rank{};
  rank.fill(-1);
  for (std::size_t i = 0; i < order.size(); ++i) {
    rank[to_index(order[i])] = static_cast<int>(i);
  }

  int max_rank_seen = -1;
  for (const char ch : result) {
    const std::size_t idx = to_index(ch);
    if (rank[idx] != -1) {
      if (rank[idx] < max_rank_seen) {
        return false;
      }
      max_rank_seen = rank[idx];
    }
  }
  return true;
}

LC_PROBLEM(791, "Custom Sort String", "sort", ctx) {
  Solution sol;

  auto expect_both_valid = [&](std::string_view order, std::string_view s) {
    const std::string res_sort = sol.customSortString(order, std::string(s));
    const std::string res_cnt = sol.customSortStringCounting(order, s);
    ctx.expect_true(is_valid_custom_sort(order, s, res_sort),
                    "customSortString produced invalid order");
    ctx.expect_true(is_valid_custom_sort(order, s, res_cnt),
                    "customSortStringCounting produced invalid order");
  };

  ctx.test("example_1", [&] {
    expect_both_valid("cba", "abcd");
    ctx.expect_eq(sol.customSortString("cba", "abcd"), std::string("cbad"));
    ctx.expect_eq(sol.customSortStringCounting("cba", "abcd"),
                  std::string("cbad"));
  });

  ctx.test("example_2_duplicates", [&] {
    expect_both_valid("bcafg", "abcd");
    expect_both_valid("kqep", "pekeq");
  });

  ctx.test("repeated_characters_and_unranked_letters", [&] {
    expect_both_valid("zyx", "axbyczxxyyzz");
  });

  ctx.test("order_contains_absent_characters", [&] {
    expect_both_valid("abcdefgh", "ha");
    ctx.expect_eq(sol.customSortString("abcdefgh", "ha"), std::string("ah"));
    ctx.expect_eq(sol.customSortStringCounting("abcdefgh", "ha"),
                  std::string("ah"));
  });

  ctx.test("no_overlap_between_order_and_s", [&] {
    expect_both_valid("xyz", "abcabc");
  });
}

}  // namespace
