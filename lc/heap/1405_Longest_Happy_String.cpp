// LeetCode 1405: Longest Happy String
// https://leetcode.com/problems/longest-happy-string/
//
// Problem:
//   A string `s` is called "happy" if it contains only `'a'`, `'b'`, and `'c'`,
//   does not contain `"aaa"`, `"bbb"`, or `"ccc"` as a substring, and uses at
//   most `a` copies of `'a'`, `b` copies of `'b'`, and `c` copies of `'c'`
//   (0 <= a, b, c <= 100). Return any longest possible happy string (or `""`
//   if none exists).
//
// Examples:
//   Example 1:
//     Input:  a = 1, b = 1, c = 7
//     Output: "ccbccacc"
//     Explanation:
//       - Initialize max_heap with {(7, 'c'), (1, 'b'), (1, 'a')}, ordered by count
//         descending then character descending.
//       - Pop (7, 'c') then (6, 'c') in the first two steps to append 'c' twice,
//         producing result = "cc" with (5, 'c') in max_heap.
//       - Pop (5, 'c') on step 3, detect the trailing "cc", pop second-largest (1,
//         'b') to append 'b' (result = "ccb"), and re-push (5, 'c').
//       - Pop (5, 'c') and (4, 'c') to append two more 'c's (result = "ccbcc"),
//         then pop (3, 'c') and use (1, 'a') as separator to reach "ccbcca".
//       - Pop (3, 'c') and (2, 'c') to append "cc" (result = "ccbccacc"), then pop
//         (1, 'c'), see trailing "cc" with an empty heap, and break to return
//         "ccbccacc".
//
//   Example 2:
//     Input:  a = 7, b = 1, c = 0
//     Output: "aabaa"
//     Explanation:
//       - Push positive counts into max_heap as {(7, 'a'), (1, 'b')}, omitting 'c'
//         since c = 0.
//       - Pop (7, 'a') and (6, 'a') over the first two iterations to build result =
//         "aa" and push (5, 'a') back.
//       - Pop (5, 'a'), detect the trailing "aa", pop (1, 'b') to append a
//         separator (result = "aab"), drop 'b' at count 0, and re-push (5, 'a').
//       - Pop (5, 'a') and (4, 'a') to append 'a' twice (result = "aabaa"), then
//         pop (3, 'a'), find max_heap empty for a separator, and return "aabaa".
//
//   Example 3:
//     Input:  a = 0, b = 0, c = 5
//     Output: "cc"
//     Explanation:
//       - Initialize max_heap with only {(5, 'c')} since a = 0 and b = 0.
//       - Pop (5, 'c') and (4, 'c') across the first two iterations to append 'c'
//         twice, yielding result = "cc" with (3, 'c') left on max_heap.
//       - Pop (3, 'c'), detect that the last two characters are already 'c' while
//         max_heap is empty, and break immediately to return "cc".
//
// Algorithm:
//   Greedy Construction with a Max-Heap:
//   1. Push every `(count, ch)` with `count > 0` into a max-heap ordered by
//      remaining count descending.
//   2. Repeatedly pop the most abundant character `(count1, ch1)`:
//      - If the last two characters of `result` are both `ch1`, appending
//        `ch1` would create three consecutive identical letters.
//        * If the heap is now empty, no separator character exists, so stop
//          and return `result`.
//        * Otherwise, pop the second most abundant `(count2, ch2)`, append one
//          `ch2` to break the run, decrement `count2`, re-push `(count2, ch2)`
//          if still positive, and re-push `(count1, ch1)` unchanged.
//      - Otherwise, append `ch1`, decrement `count1`, and re-push if positive.
//   Invariant: Greedily consuming the highest-remaining-count letter whenever
//   allowed (and spending only a single copy of the second-highest letter to
//   break a run of two) minimizes imbalance and preserves the maximum number
//   of separator characters needed to accommodate the dominant letter.
//   Time Complexity:  O((a + b + c) * log 3) = O(a + b + c).
//   Space Complexity: O(a + b + c) for the output string; O(1) auxiliary space
//                     for the 3-element max-heap.
//
// Data Structures:
//   - Max-heap (`std::priority_queue<LetterCount>`): tracks up to 3 active
//     character budgets so the most and second-most abundant letters are
//     always accessible in O(1) / O(log 3) time.
//   - `std::string`: stores the constructed happy string with O(1) amortized
//     back insertion and tail inspection.
//
// Real-World Applications:
//   - Smooth weighted round-robin load balancing and packet pacing (avoiding
//     bursty consecutive transmissions from a single dominant flow).
//   - Ad/content feed interleaving (enforcing diversity rules such as "never
//     show more than two consecutive items from the same category").
//   - Task cooldown scheduling in distributed workers to prevent resource
//     hotspots.
//
// Related LeetCode Problems:
//   - #767 Reorganize String [done] (no two adjacent identical characters via max-heap)
//   - #358 Rearrange String k Distance Apart [done] (generalized k-spacing with max-heap + queue)
//   - #621 Task Scheduler [done] (greedy cooldown scheduling using frequency counts/heap)
//   - #984 String Without AAA or BBB [new] (2-character special case of Longest Happy String)
//   - #1054 Distant Barcodes [new] (adjacent-distinct rearrangement with max-heap)

#include "framework/lc_framework.h"

#include <algorithm>
#include <compare>
#include <cstddef>
#include <queue>
#include <string>
#include <string_view>
#include <utility>

namespace {

// Tracks remaining quota for one character; max-heap orders by `count` first.
struct LetterCount {
  int count = 0;
  char ch = '\0';

  auto operator<=>(const LetterCount&) const = default;
};

class Solution {
 public:
  // Constructs a longest happy string using at most `a` 'a's, `b` 'b's, and
  // `c` 'c's without containing "aaa", "bbb", or "ccc".
  //
  // Time Complexity:  O(a + b + c)
  // Space Complexity: O(a + b + c) for output, O(1) auxiliary heap space
  [[nodiscard]] std::string longestDiverseString(int a, int b, int c) {
    std::priority_queue<LetterCount> max_heap;
    if (a > 0) {
      max_heap.push(LetterCount{.count = a, .ch = 'a'});
    }
    if (b > 0) {
      max_heap.push(LetterCount{.count = b, .ch = 'b'});
    }
    if (c > 0) {
      max_heap.push(LetterCount{.count = c, .ch = 'c'});
    }

    std::string result;
    result.reserve(static_cast<std::size_t>(a) + static_cast<std::size_t>(b) +
                   static_cast<std::size_t>(c));

    while (!max_heap.empty()) {
      LetterCount first = max_heap.top();
      max_heap.pop();

      const std::size_t len = result.size();
      const bool would_triple =
          len >= 2uz && result[len - 1uz] == first.ch &&
          result[len - 2uz] == first.ch;

      if (would_triple) {
        // Cannot append `first.ch` a third time in a row; need a separator.
        if (max_heap.empty()) {
          break;
        }

        LetterCount second = max_heap.top();
        max_heap.pop();

        result.push_back(second.ch);
        --second.count;
        if (second.count > 0) {
          max_heap.push(second);
        }

        // Put `first` back untouched so it remains eligible on the next step.
        max_heap.push(first);
        continue;
      }

      result.push_back(first.ch);
      --first.count;
      if (first.count > 0) {
        max_heap.push(first);
      }
    }

    return result;
  }
};

// Verifies all happy-string constraints and exact optimal length for (a, b, c).
[[nodiscard]] bool is_optimal_happy_string(std::string_view s, int a, int b,
                                           int c) {
  int used_a = 0;
  int used_b = 0;
  int used_c = 0;

  for (std::size_t i = 0uz; i < s.size(); ++i) {
    if (s[i] == 'a') {
      ++used_a;
    } else if (s[i] == 'b') {
      ++used_b;
    } else if (s[i] == 'c') {
      ++used_c;
    } else {
      return false;
    }

    if (i >= 2uz && s[i] == s[i - 1uz] && s[i] == s[i - 2uz]) {
      return false;
    }
  }

  if (used_a > a || used_b > b || used_c > c) {
    return false;
  }

  // Exact theoretical maximum length: at most 2 copies of the dominant letter
  // can appear per separator slot (other_sum + 1 slots).
  const int total = a + b + c;
  const int dominant = std::max({a, b, c});
  const int other_sum = total - dominant;
  const int max_len = other_sum + std::min(dominant, 2 * (other_sum + 1));
  return std::cmp_equal(s.size(), max_len);
}

LC_PROBLEM(1405, "Longest Happy String", "heap", ctx) {
  ctx.test("example_1_multiple_valid_answers", [&] {
    Solution sol;
    const std::string res = sol.longestDiverseString(1, 1, 7);
    ctx.expect_true(is_optimal_happy_string(res, 1, 1, 7));
  });

  ctx.test("example_2_unique_answer", [&] {
    Solution sol;
    const std::string res = sol.longestDiverseString(7, 1, 0);
    ctx.expect_eq(res, std::string("aabaa"));
    ctx.expect_true(is_optimal_happy_string(res, 7, 1, 0));
  });

  ctx.test("balanced_counts_uses_all_letters", [&] {
    Solution sol;
    const std::string res = sol.longestDiverseString(2, 2, 1);
    ctx.expect_true(is_optimal_happy_string(res, 2, 2, 1));
  });

  ctx.test("single_letter_capped_at_two", [&] {
    Solution sol;
    ctx.expect_eq(sol.longestDiverseString(0, 0, 5), std::string("cc"));
    ctx.expect_eq(sol.longestDiverseString(1, 0, 0), std::string("a"));
  });

  ctx.test("all_zeros_returns_empty", [&] {
    Solution sol;
    ctx.expect_eq(sol.longestDiverseString(0, 0, 0), std::string(""));
  });
}

}  // namespace
