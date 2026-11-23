// LeetCode 767: Reorganize String
// https://leetcode.com/problems/reorganize-string/
//
// Problem:
//   Given a lowercase English string `s`, rearrange its characters so that no
//   two adjacent characters are identical. Return any valid rearrangement if
//   one exists, or an empty string "" if it is impossible.
//
// Examples:
//   Example 1:
//     Input:  s = "aab"
//     Output: "aba"
//     Explanation:
//       - Count character frequencies to get freq['a'] = 2 and freq['b'] = 1, with
//         max_idx pointing to 'a'.
//       - Check the Pigeonhole bound: max frequency 2 <= (3 + 1) / 2 = 2, so
//         reorganization is feasible.
//       - Place both 'a' characters at even indices 0 and 2, setting result = "a_a"
//         and advancing write_pos to 4.
//       - Because write_pos (4) >= n (3), wrap write_pos to 1 and place 'b' at
//         index 1 to return "aba".
//
//   Example 2:
//     Input:  s = "aaab"
//     Output: ""
//     Explanation:
//       - Tally character frequencies to find freq['a'] = 3 and freq['b'] = 1, with
//         max_idx pointing to 'a'.
//       - Check the Pigeonhole bound: max frequency 3 > (4 + 1) / 2 = 2, so no
//         non-adjacent placement exists and the function returns "".
//
//   Example 3:
//     Input:  s = "cbbaaaa"
//     Output: "ababaca"
//     Explanation:
//       - Tally frequencies to get freq['a'] = 4, freq['b'] = 2, and freq['c'] = 1,
//         with max_idx pointing to 'a' and 4 <= (7 + 1) / 2 = 4.
//       - Place all four 'a' characters at even indices 0, 2, 4, and 6, advancing
//         write_pos to 8.
//       - When placing 'b', wrap write_pos (8 >= 7) to odd index 1 and write both
//         'b' characters at indices 1 and 3, advancing write_pos to 5.
//       - Write the single 'c' at index 5 to complete the valid arrangement
//         "ababaca".
//
// Algorithm:
//   Count the frequency of each lowercase letter ('a'-'z') and identify the
//   most frequent character `max_char` with frequency `max_cnt`.
//   1. Feasibility check (Pigeonhole Principle): a string of length n has
//      ceil(n / 2) = (n + 1) / 2 even indices (0, 2, 4, ...). If
//      `max_cnt > (n + 1) / 2`, two copies of `max_char` must inevitably be
//      adjacent, so return "".
//   2. Interleaved even-then-odd placement: place all `max_cnt` copies of
//      `max_char` at even indices 0, 2, 4, ... first. Then iterate through the
//      remaining letters in any order, continuing to fill any remaining even
//      indices and wrapping once to odd indices 1, 3, 5, ... when the write
//      cursor reaches or exceeds n.
//      Invariant: because `max_char` occupies only even slots and every other
//      character has frequency <= n / 2 (or cannot straddle the wrap point into
//      an index adjacent to its own first even slot), no two identical
//      characters can ever land at adjacent positions.
//   Time Complexity:  O(n), where n = |s| (counting + one placement pass over
//                     the fixed 26-letter alphabet).
//   Space Complexity: O(1) auxiliary space (26-element frequency array),
//                     excluding the O(n) output string.
//
// Data Structures:
//   - std::array<int, 26>: fixed-size frequency table for lowercase letters.
//   - std::string: output buffer filled directly at even, then odd, positions.
//
// Real-World Applications:
//   - CPU and task schedulers spacing out identical resource-heavy jobs so no
//     two back-to-back executions contend for the same lock or thermal zone.
//   - Playlist and feed-ranking interleavers preventing consecutive items from
//     the same artist, advertiser, or content creator.
//   - Manufacturing and print-head scheduling avoiding consecutive passes over
//     adjacent nozzles or curing zones.
//
// Related LeetCode Problems:
//   - #358 Rearrange String k Distance Apart [done] (generalization from distance 2 to k)
//   - #621 Task Scheduler [done] (minimum schedule length with cooldown constraint)
//   - #1054 Distant Barcodes [new] (identical adjacency constraint on integer arrays)
//   - #1405 Longest Happy String [done] (greedy character placement with run-length cap)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <string>
#include <string_view>

namespace {

class Solution {
 public:
  // Rearranges `s` so no two adjacent characters match, or returns "" if no
  // valid arrangement exists.
  // Time: O(n), Space: O(1) auxiliary (O(n) output).
  std::string reorganizeString(std::string_view s) {
    const std::size_t n = s.size();
    if (n <= 1uz) {
      return std::string(s);
    }

    // Step 1: Tally frequencies and locate the most frequent character.
    std::array<std::size_t, kAlphabetSize> freq{};
    std::size_t max_idx = 0uz;
    for (const char ch : s) {
      const auto idx = static_cast<std::size_t>(ch - 'a');
      ++freq[idx];
      if (freq[idx] > freq[max_idx]) {
        max_idx = idx;
      }
    }

    // By the Pigeonhole Principle, no character may appear more than
    // ceil(n / 2) times without forcing at least one adjacent duplicate.
    if (freq[max_idx] > ((n + 1uz) / 2uz)) {
      return "";
    }

    std::string result(n, '\0');
    std::size_t write_pos = 0uz;

    // Helper lambda to write all remaining occurrences of letter `letter_idx`
    // into alternating slots (even positions first, wrapping once to odd).
    auto place_all = [&](std::size_t letter_idx) {
      const char ch = static_cast<char>('a' + static_cast<char>(letter_idx));
      while (freq[letter_idx] > 0uz) {
        if (write_pos >= n) {
          write_pos = 1uz;
        }
        result[write_pos] = ch;
        write_pos += 2uz;
        --freq[letter_idx];
      }
    };

    // Step 2: Place the most frequent character first so that when its count
    // equals ceil(n / 2), it fits strictly into even indices without wrapping.
    place_all(max_idx);

    // Step 3: Fill all remaining characters into the remaining even slots and
    // then the odd slots.
    for (std::size_t i = 0uz; i < kAlphabetSize; ++i) {
      if (freq[i] > 0uz) {
        place_all(i);
      }
    }

    return result;
  }

 private:
  static constexpr std::size_t kAlphabetSize = 26;
};

// Verifies that `output` is a valid reorganization of `input`: exact anagram
// with no two adjacent characters equal.
bool is_valid_reorganization(std::string_view input, std::string_view output) {
  if (input.size() != output.size()) {
    return false;
  }
  for (std::size_t i = 1uz; i < output.size(); ++i) {
    if (output[i] == output[i - 1uz]) {
      return false;
    }
  }
  std::string sorted_in(input);
  std::string sorted_out(output);
  std::ranges::sort(sorted_in);
  std::ranges::sort(sorted_out);
  return sorted_in == sorted_out;
}

LC_PROBLEM(767, "Reorganize String", "string", ctx) {
  Solution sol;

  ctx.test("example_1_possible", [&] {
    const std::string out = sol.reorganizeString("aab");
    ctx.expect_true(is_valid_reorganization("aab", out));
  });

  ctx.test("example_2_impossible", [&] {
    ctx.expect_eq(sol.reorganizeString("aaab"), "");
  });

  ctx.test("source_build_case_cbbaaaa", [&] {
    const std::string out = sol.reorganizeString("cbbaaaa");
    ctx.expect_true(is_valid_reorganization("cbbaaaa", out));
  });

  ctx.test("even_length_tied_max_counts", [&] {
    const std::string out = sol.reorganizeString("aabbcc");
    ctx.expect_true(is_valid_reorganization("aabbcc", out));
  });

  ctx.test("odd_length_boundary_wrap", [&] {
    const std::string out = sol.reorganizeString("abbcc");
    ctx.expect_true(is_valid_reorganization("abbcc", out));
  });

  ctx.test("single_character", [&] {
    ctx.expect_eq(sol.reorganizeString("z"), "z");
  });

  ctx.test("two_identical_impossible", [&] {
    ctx.expect_eq(sol.reorganizeString("aa"), "");
  });
}

}  // namespace
