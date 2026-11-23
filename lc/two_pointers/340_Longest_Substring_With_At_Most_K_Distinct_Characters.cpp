// LeetCode 340: Longest Substring With At Most K Distinct Characters
// https://leetcode.com/problems/longest-substring-with-at-most-k-distinct-characters/
//
// Problem:
//   Given a string `s` and an integer `k`, return the length of the longest
//   contiguous substring of `s` that contains at most `k` distinct characters.
//
// Algorithm:
//   Sliding window tracking the rightmost occurrence index of each character
//   currently inside the window [left, right]:
//     - Expand `right` across `s` from 0 to n - 1, updating
//       `last_pos[s[right]] = right`.
//     - Whenever `last_pos.size() > k`, the window has k + 1 distinct
//       characters. To shrink the window as little as possible while restoring
//       at most k distinct characters, locate the character in `last_pos` with
//       the smallest rightmost index `min_idx`, erase it from `last_pos`, and
//       jump `left` directly to `min_idx + 1`.
//     - Update `max_len` with the current valid window size `right - left + 1`.
//   Time complexity: O(n * min(k, |Σ|)) — for byte-sized ASCII characters
//   |Σ| <= 256, so each eviction scan is bounded by a small constant, giving
//   O(n) effective time.
//   Space complexity: O(min(k, |Σ|)) for the rightmost-index map.
//
// Data Structures:
//   - std::unordered_map<char, std::size_t> (`last_pos`): stores the latest
//     index of every distinct character active in the current window; its size
//     directly equals the distinct character count.
//   - Sliding window boundaries (`left`, `right`): track the longest valid
//     contiguous substring without re-scanning characters.
//
// Real-World Applications:
//   - Bounded-cardinality streaming analytics (finding the longest time window
//     of events involving at most k distinct users, hosts, or error codes).
//   - LRU / small-cache working-set analysis: measuring the longest sequence of
//     memory references that fits without eviction in a k-entry cache.
//   - Bioinformatics sequence segmentation looking for low-complexity regions
//     spanning at most k distinct symbols.
//
// Related LeetCode Problems:
//   - #159 Longest Substring with At Most Two Distinct Characters [done] (special case k = 2)
//   - #3 Longest Substring Without Repeating Characters [done] (last-seen-index sliding window)
//   - #76 Minimum Window Substring [done] (variable-size sliding window with frequency counts)
//   - #424 Longest Repeating Character Replacement [done] (sliding window with character budget)
//   - #904 Fruit Into Baskets [new] (identical to k = 2 over an integer array)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <string_view>
#include <unordered_map>

namespace {

class Solution {
 public:
  // Returns the length of the longest substring of `s` containing at most `k`
  // distinct characters.
  //
  // Invariant: at the end of each iteration, `last_pos` holds the rightmost
  // index of every distinct character in s[left..right], and
  // last_pos.size() <= k.
  //
  // Time: O(n * min(k, |Σ|)), Space: O(min(k, |Σ|)).
  int lengthOfLongestSubstringKDistinct(std::string_view s, int k) {
    // Guard k <= 0 and empty input up front to avoid unsigned underflow on
    // `right - left + 1` when no character can remain in the window.
    if (k <= 0 || s.empty()) {
      return 0;
    }

    const auto max_distinct = static_cast<std::size_t>(k);
    // Maps each character currently in the window to its latest seen index.
    std::unordered_map<char, std::size_t> last_pos;

    std::size_t left = 0;
    std::size_t max_len = 0;

    for (std::size_t right = 0uz; right < s.size(); ++right) {
      last_pos[s[right]] = right;

      if (last_pos.size() > max_distinct) {
        // Evict the character whose last occurrence is furthest to the left;
        // jumping `left` past it eliminates that character completely while
        // preserving the largest possible valid suffix ending at `right`.
        const auto oldest = std::ranges::min_element(
            last_pos, std::ranges::less{},
            [](const auto& entry) { return entry.second; });
        left = oldest->second + 1;
        last_pos.erase(oldest);
      }

      max_len = std::max(max_len, (right - left) + 1);
    }

    return static_cast<int>(max_len);
  }
};

LC_PROBLEM(340, "Longest Substring With At Most K Distinct Characters",
           "two_pointers", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    // "ece" has length 3 with 2 distinct characters.
    ctx.expect_eq(sol.lengthOfLongestSubstringKDistinct("eceba", 2), 3);
  });

  ctx.test("example_2", [&] {
    ctx.expect_eq(sol.lengthOfLongestSubstringKDistinct("aa", 1), 2);
  });

  ctx.test("zero_k_and_empty_string", [&] {
    ctx.expect_eq(sol.lengthOfLongestSubstringKDistinct("eceba", 0), 0);
    ctx.expect_eq(sol.lengthOfLongestSubstringKDistinct("", 2), 0);
    ctx.expect_eq(sol.lengthOfLongestSubstringKDistinct("", 0), 0);
  });

  ctx.test("k_exceeds_distinct_count", [&] {
    ctx.expect_eq(sol.lengthOfLongestSubstringKDistinct("abcdef", 10), 6);
  });

  ctx.test("single_distinct_alternating", [&] {
    ctx.expect_eq(sol.lengthOfLongestSubstringKDistinct("ababab", 1), 1);
  });

  ctx.test("eviction_with_interleaved_repeats", [&] {
    // "accc" has length 4 with 2 distinct characters.
    ctx.expect_eq(sol.lengthOfLongestSubstringKDistinct("abaccc", 2), 4);
  });

  ctx.test("longer_window_after_multiple_evictions", [&] {
    // "bbccdd" has length 6 with 3 distinct characters.
    ctx.expect_eq(sol.lengthOfLongestSubstringKDistinct("aabbccdd", 3), 6);
  });
}

}  // namespace
