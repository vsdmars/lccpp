// LeetCode 1239: Maximum Length Of A Concatenated String With Unique Characters
// https://leetcode.com/problems/maximum-length-of-a-concatenated-string-with-unique-characters/
//
// Problem:
//   Given an array of lowercase English strings `arr`, form a string `s` by
//   concatenating a subsequence of `arr` such that `s` contains no duplicate
//   characters. Return the maximum possible length of `s`.
//   Constraints: 1 <= arr.length <= 16, 1 <= arr[i].length <= 26,
//   arr[i] consists only of lowercase English letters `'a'..'z'`.
//
// Algorithm:
//   Iterative Subset Bitmask Enumeration (Bitset DP):
//   Since the alphabet has only 26 lowercase letters, any string with unique
//   characters can be encoded losslessly as a 26-bit integer mask where bit
//   `(ch - 'a')` is set iff `ch` appears in the string.
//   1. Start with a list of reachable masks initialized to `{0}` (the empty
//      concatenation).
//   2. For each word in `arr`:
//      - Compute its bitmask via `toUniqueMask()`. If the word itself contains
//        duplicate characters, discard it immediately (mask == 0).
//      - Snapshot the current size of `reachable_masks` and attempt to combine
//        `word_mask` with every existing mask `prev`.
//      - Two character sets are disjoint iff `(prev & word_mask) == 0`. When
//        disjoint, append `prev | word_mask` and update `best_len` using
//        `std::popcount(combined)`.
//   Time Complexity:  O(L + 2^n), where L is the total character count in `arr`
//                     and n = arr.length <= 16 (at most 2^16 = 65,536 masks).
//   Space Complexity: O(2^n) to store all valid reachable subset bitmasks.
//
// Data Structures:
//   - `std::uint32_t` 26-bit bitmasks: O(1) disjointness check (`&`), union
//     (`|`), and length computation (`std::popcount`), replacing Python `set`s.
//   - `std::vector<std::uint32_t>`: stores all reachable valid subset masks.
//
// Real-World Applications:
//   - Resource-conflict-free scheduling (selecting the largest subset of tasks
//     or microservices requiring mutually exclusive hardware/lock resources).
//   - Feature-flag and compiler-pass compatibility selection when combining
//     non-overlapping capability bitfields.
//   - Exact maximum-weight independent set / set-packing solvers on small
//     bit-encoded universes.
//
// Related LeetCode Problems:
//   - #78 Subsets [done] (iterative subset enumeration over an array)
//   - #318 Maximum Product of Word Lengths [new] (26-bit character bitmasks for disjointness)
//   - #1684 Count the Number of Consistent Strings [new] (lowercase alphabet bitmasking)
//   - #1125 Smallest Sufficient Team [new] (bitmask DP over skill subsets)

#include "framework/lc_framework.h"

#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the maximum length of a concatenation of a subsequence of `arr`
  // whose characters are all unique.
  // Time: O(L + 2^n), Space: O(2^n).
  int maxLength(const std::vector<std::string>& arr) {
    // Seed DP state with the empty string's bitmask (0 bits set).
    std::vector<std::uint32_t> reachable_masks = {0U};
    int best_len = 0;

    for (const std::string& word : arr) {
      const std::uint32_t word_mask = toUniqueMask(word);
      // Skip strings that are empty or already contain duplicate characters.
      if (word_mask == 0U) {
        continue;
      }

      // Snapshot the current count so `word_mask` is combined only with
      // combinations formed from earlier elements in `arr`.
      const std::size_t prev_count = reachable_masks.size();
      for (std::size_t i = 0uz; i < prev_count; ++i) {
        if ((reachable_masks[i] & word_mask) != 0U) {
          continue;
        }
        const std::uint32_t combined = reachable_masks[i] | word_mask;
        reachable_masks.push_back(combined);
        best_len = std::max(best_len, std::popcount(combined));
      }
    }

    return best_len;
  }

 private:
  // Encodes `word` as a 26-bit mask over `'a'..'z'`, or returns 0 if `word`
  // contains any repeated character (or is empty).
  static std::uint32_t toUniqueMask(std::string_view word) {
    std::uint32_t mask = 0U;
    for (const char ch : word) {
      const auto bit_index = static_cast<unsigned int>(ch - 'a');
      const std::uint32_t bit = 1U << bit_index;
      if ((mask & bit) != 0U) {
        return 0U;
      }
      mask |= bit;
    }
    return mask;
  }
};

LC_PROBLEM(1239,
           "Maximum Length Of A Concatenated String With Unique Characters",
           "array", ctx) {
  Solution sol;

  ctx.test("example_1_overlapping_pairs", [&] {
    ctx.expect_eq(sol.maxLength({"un", "iq", "ue"}), 4);
  });

  ctx.test("example_2_build_case", [&] {
    ctx.expect_eq(sol.maxLength({"cha", "r", "act", "ers"}), 6);
  });

  ctx.test("example_3_full_alphabet", [&] {
    ctx.expect_eq(sol.maxLength({"abcdefghijklmnopqrstuvwxyz"}), 26);
  });

  ctx.test("example_4_all_internal_duplicates", [&] {
    ctx.expect_eq(sol.maxLength({"aa", "bb"}), 0);
  });

  ctx.test("edge_cases_disjoint_and_duplicates_mixed", [&] {
    ctx.expect_eq(sol.maxLength({"ab", "cd", "ef", "aa", "ace"}), 6);
    ctx.expect_eq(sol.maxLength({"a", "b", "c", "d"}), 4);
  });
}

}  // namespace
