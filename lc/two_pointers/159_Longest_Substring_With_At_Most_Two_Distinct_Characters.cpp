// LeetCode 159: Longest Substring With At Most Two Distinct Characters
// https://leetcode.com/problems/longest-substring-with-at-most-two-distinct-characters/
//
// Problem:
//   Given a string `s`, return the length of the longest contiguous substring
//   that contains at most two distinct characters.
//
// Examples:
//   Example 1:
//     Input:  s = "eceba"
//     Output: 3
//     Explanation:
//       - Indices right = 0..2 ("ece") fill last_pos = {'e': 2, 'c': 1} with left =
//         0, setting max_len = 3.
//       - At right = 3 ('b'), last_pos = {'e': 2, 'c': 1, 'b': 3} exceeds 2
//         distinct characters, so oldest entry 'c': 1 is erased and left jumps to 1
//         + 1 = 2 (window "eb", length 2).
//       - At right = 4 ('a'), last_pos = {'e': 2, 'b': 3, 'a': 4} evicts oldest
//         entry 'e': 2, setting left = 2 + 1 = 3 (window "ba", length 2) and
//         returning max_len = 3.
//
//   Example 2:
//     Input:  s = "ccaabbb"
//     Output: 5
//     Explanation:
//       - Indices right = 0..3 ("ccaa") populate last_pos = {'c': 1, 'a': 3} with
//         left = 0, updating max_len to 4.
//       - At right = 4 ('b'), last_pos = {'c': 1, 'a': 3, 'b': 4} exceeds 2
//         distinct characters, so oldest entry 'c': 1 is erased and left jumps to 1
//         + 1 = 2 (window "aab", length 3).
//       - Indices right = 5..6 ('b', 'b') update last_pos to {'a': 3, 'b': 6} while
//         left remains 2, expanding window "aabbb" to max_len = 6 - 2 + 1 = 5.
//
//   Example 3:
//     Input:  s = "aAbBcC"
//     Output: 2
//     Explanation:
//       - Indices right = 0..1 ("aA") record last_pos = {'a': 0, 'A': 1} with left
//         = 0 and max_len = 2 because uppercase and lowercase letters are distinct
//         keys.
//       - At right = 2 ('b'), last_pos = {'a': 0, 'A': 1, 'b': 2} evicts 'a': 0 and
//         advances left to 1, keeping window "Ab" at length 2.
//       - Each subsequent character ('B', 'c', 'C') evicts the oldest entry ('A':
//         1, 'b': 2, 'B': 3) in turn, leaving max_len = 2.
//
// Algorithm:
//   Variable-size sliding window `[left, right]` paired with a small hash map
//   `last_pos` storing the rightmost index at which each active character in
//   the window appears.
//   As `right` sweeps from `0` to `s.size() - 1`:
//     1. Record `last_pos[s[right]] = right`.
//     2. If `last_pos.size() > 2`, the window contains 3 distinct characters.
//        To shrink the window by the minimal amount needed to drop one
//        distinct character completely, find the entry `(ch, pos)` in
//        `last_pos` with the smallest rightmost index `pos`, erase `ch`, and
//        jump `left` directly to `pos + 1`.
//     3. Update `max_len = max(max_len, right - left + 1)`.
//   Because `last_pos` holds at most 3 entries at any moment, finding the
//   minimum entry is O(1).
//   Time Complexity:  O(n) single pass, where n = s.size().
//   Space Complexity: O(1) — the map holds at most 3 character entries.
//
// Data Structures:
//   `std::unordered_map<char, std::size_t>` bounded to at most 3 entries
//   (mapping active window characters to their rightmost seen index) plus two
//   `std::size_t` sliding-window cursors (`left`, `right`).
//
// Real-World Applications:
//   - Network / telemetry stream anomaly detection identifying longest periods
//     where traffic originates from at most K distinct sources or states.
//   - Genomic sequence analysis locating low-complexity / two-nucleotide-rich
//     regions (e.g., AT-rich or GC-rich islands) in DNA strands.
//   - Cache-locality profiling over memory access traces to find maximal
//     intervals fitting inside a small K-way associativity set.
//
// Related LeetCode Problems:
//   - #340 Longest Substring with At Most K Distinct Characters [done] (generalization from 2 to arbitrary K)
//   - #3 Longest Substring Without Repeating Characters [done] (sliding window tracking rightmost index per character)
//   - #904 Fruit Into Baskets [new] (exact reformulation of #159 on integer arrays)
//   - #76 Minimum Window Substring [done] (variable sliding window minimizing length over character counts)
//   - #424 Longest Repeating Character Replacement [done] (sliding window tracking dominant frequency)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string_view>
#include <unordered_map>

namespace {

class Solution {
 public:
  // Returns the length of the longest substring of `s` containing at most two
  // distinct characters.
  //
  // Invariant: after each loop step, `s[left .. right]` has at most
  // `kMaxDistinct` (2) distinct characters, and `last_pos` maps every distinct
  // character in `s[left .. right]` to its rightmost index in that window.
  //
  // Complexity: O(n) time, O(1) extra space (at most 3 map entries).
  [[nodiscard]] int lengthOfLongestSubstringTwoDistinct(std::string_view s) {
    static constexpr std::size_t kMaxDistinct = 2uz;

    std::unordered_map<char, std::size_t> last_pos;
    std::size_t left = 0uz;
    std::size_t max_len = 0uz;

    for (std::size_t right = 0uz; right < s.size(); ++right) {
      last_pos[s[right]] = right;

      // Once a third distinct character enters, evict whichever character's
      // final occurrence is farthest left; advancing `left` past it removes
      // that character completely in O(1) without stepping one index at a time.
      if (last_pos.size() > kMaxDistinct) {
        const auto oldest = std::ranges::min_element(
            last_pos, {}, [](const auto& entry) { return entry.second; });
        left = oldest->second + 1uz;
        last_pos.erase(oldest);
      }

      max_len = std::max(max_len, right - left + 1uz);
    }

    return static_cast<int>(max_len);
  }
};

LC_PROBLEM(159, "Longest Substring With At Most Two Distinct Characters",
           "two_pointers", ctx) {
  Solution sol;

  ctx.test("leetcode_example_1_eceba", [&] {
    ctx.expect_eq(sol.lengthOfLongestSubstringTwoDistinct("eceba"), 3);
  });

  ctx.test("leetcode_example_2_ccaabbb", [&] {
    ctx.expect_eq(sol.lengthOfLongestSubstringTwoDistinct("ccaabbb"), 5);
  });

  ctx.test("python_build_aabac", [&] {
    ctx.expect_eq(sol.lengthOfLongestSubstringTwoDistinct("aabac"), 4);
  });

  ctx.test("empty_single_and_uniform", [&] {
    ctx.expect_eq(sol.lengthOfLongestSubstringTwoDistinct(""), 0);
    ctx.expect_eq(sol.lengthOfLongestSubstringTwoDistinct("a"), 1);
    ctx.expect_eq(sol.lengthOfLongestSubstringTwoDistinct("zzzzz"), 5);
  });

  ctx.test("all_distinct_and_case_sensitive", [&] {
    ctx.expect_eq(sol.lengthOfLongestSubstringTwoDistinct("abcdef"), 2);
    ctx.expect_eq(sol.lengthOfLongestSubstringTwoDistinct("aAbBcC"), 2);
    ctx.expect_eq(sol.lengthOfLongestSubstringTwoDistinct("abaccc"), 4);
  });
}

}  // namespace
