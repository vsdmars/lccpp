// LeetCode 616: Add Bold Tag In String
// https://leetcode.com/problems/add-bold-tag-in-string/
//
// Problem:
//   Given a string `s` and an array of dictionary strings `words`, wrap every
//   substring of `s` that matches any word in `words` with `<b>` and `</b>`
//   tags. If two bolded substrings overlap or are directly adjacent
//   (consecutive), merge them under a single pair of `<b>...</b>` tags.
//   Constraints: 1 <= s.size() <= 1000, 0 <= words.size() <= 100,
//   1 <= words[i].size() <= 1000. (Equivalent to LeetCode #758.)
//
// Examples:
//   Example 1:
//     Input:  s = "abcxyz123", words = ["abc", "123"]
//     Output: "<b>abc</b>xyz<b>123</b>"
//     Explanation:
//       - Allocate diff of size 10 filled with zeros; finding "abc" at index 0 sets
//         diff[0] = 1, diff[3] = -1, and finding "123" at index 6 sets diff[6] = 1,
//         diff[9] = -1.
//       - Sweeping i = 0..2 keeps active = 1 > 0, so "<b>" opens at i = 0 and "abc"
//         is appended.
//       - At i = 3, active becomes 0 (1 + (-1)), closing "</b>" before appending
//         "xyz" across i = 3..5.
//       - At i = 6, active rises to 1, opening "<b>" before "123", and after i = 8
//         finishes with in_bold == true, the trailing "</b>" is appended to return
//         "<b>abc</b>xyz<b>123</b>".
//
//   Example 2:
//     Input:  s = "aaabbcc", words = ["aaa", "aab", "bc"]
//     Output: "<b>aaabbc</b>c"
//     Explanation:
//       - Matching each word marks intervals [0, 3) for "aaa", [1, 4) for "aab",
//         and [4, 6) for "bc", yielding diff = [1, 1, 0, -1, 0, 0, -1, 0].
//       - At index 4, the -1 from "aab" ending and +1 from "bc" starting cancel at
//         diff[4] == 0, keeping the prefix sum active = [1, 2, 2, 1, 1, 1, 0]
//         positive throughout i = 0..5.
//       - Emit "<b>" at i = 0, append "aaabbc" while active > 0, and close "</b>"
//         at i = 6 when active drops to 0 before appending the final 'c' to produce
//         "<b>aaabbc</b>c".
//
//   Example 3:
//     Input:  s = "baaaab", words = ["aa"]
//     Output: "b<b>aaaa</b>b"
//     Explanation:
//       - Advancing pos = s.find("aa", pos + 1) by 1 finds three overlapping
//         matches at indices 1, 2, and 3, producing diff = [0, 1, 1, 0, -1, -1, 0].
//       - Taking the running prefix sum across i = 0..5 yields active = [0, 1, 2,
//         2, 1, 0].
//       - Index 0 ('b') is emitted unbolded, "<b>" wraps indices 1..4 ("aaaa"), and
//         "</b>" closes at i = 5 before appending 'b', returning "b<b>aaaa</b>b".
//
// Algorithm:
//   Interval coverage via a difference array (sweep-line / prefix-sum mask):
//   1. Allocate a difference array `diff` of size `n + 1` initialized to 0.
//   2. For each non-empty `word` in `words`, search for every occurrence of
//      `word` in `s` using `s.find(word, pos)` advancing by 1 on each match
//      (to catch overlapping occurrences such as `"aa"` in `"aaa"`). For each
//      match at `[pos, pos + word.size())`, increment `diff[pos]` and decrement
//      `diff[pos + word.size()]` in O(1) time.
//   3. Sweep left-to-right across `i = 0 .. n - 1`, maintaining the running
//      prefix sum `active += diff[i]`. Character `s[i]` is bold iff
//      `active > 0`.
//   4. Emit `<b>` when transitioning from non-bold to bold, append `s[i]`, and
//      emit `</b>` when transitioning from bold back to non-bold (or at the
//      end of `s`). Because overlapping and consecutive intervals keep
//      `active > 0` across their boundary (`-1` and `+1` cancel at the join
//      index), merging happens automatically.
//   Time Complexity: O(M * N * L) worst-case substring search where M is
//   `words.size()`, N is `s.size()`, and L is average word length; marking
//   each match is O(1) via the difference array instead of O(L).
//   Space Complexity: O(N) for the difference array and output string.
//
// Data Structures:
//   - `std::vector<int>` difference array of length `N + 1`: records interval
//     start (`+1`) and end (`-1`) events in O(1) per match, whose prefix sum
//     yields exact per-character bold coverage.
//   - `std::string`: output buffer assembling the tagged HTML string.
//
// Real-World Applications:
//   - Search engine result snippet highlighting (wrapping overlapping query
//     keyword hits in `<b>` / `<mark>` tags without producing malformed nested
//     HTML).
//   - IDE syntax and regex match highlighting where multiple token rules can
//     annotate overlapping spans of source text.
//   - Named-entity recognition (NER) UI renderers merging contiguous entity
//     spans in document viewers.
//
// Related LeetCode Problems:
//   - #758 Bold Words in String [new] (identical problem with smaller word-length constraints)
//   - #56 Merge Intervals [done] (merging overlapping intervals in sorted order)
//   - #57 Insert Interval [done] (inserting and coalescing a new interval into existing spans)
//   - #833 Find And Replace in String [new] (applying non-overlapping substring annotations/replacements)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Wraps all occurrences of `words` in `s` with `<b>...</b>`, merging
  // overlapping and adjacent bold regions.
  // Time: O(N * sum(|word|)), Space: O(N).
  std::string addBoldTag(std::string_view s,
                         const std::vector<std::string>& words) {
    const std::size_t n = s.size();
    if (n == 0uz || words.empty()) {
      return std::string(s);
    }

    // Difference array where diff[l] += 1 and diff[r] -= 1 marks [l, r) bold.
    std::vector<int> diff(n + 1uz, 0);
    for (const std::string& word : words) {
      if (word.empty() || word.size() > n) {
        continue;
      }
      // Advance by 1 after each hit so overlapping matches are also marked.
      std::size_t pos = s.find(word, 0uz);
      while (pos != std::string_view::npos) {
        ++diff[pos];
        --diff[pos + word.size()];
        pos = s.find(word, pos + 1uz);
      }
    }

    // Sweep the difference array to emit merged <b>...</b> spans in one pass.
    std::string result;
    result.reserve(n);

    int active = 0;
    bool in_bold = false;
    for (std::size_t i = 0uz; i < n; ++i) {
      active += diff[i];
      const bool should_bold = active > 0;

      if (should_bold && !in_bold) {
        result += "<b>";
        in_bold = true;
      } else if (!should_bold && in_bold) {
        result += "</b>";
        in_bold = false;
      }
      result.push_back(s[i]);
    }

    // Close any bold span extending to the end of the string.
    if (in_bold) {
      result += "</b>";
    }

    return result;
  }
};

LC_PROBLEM(616, "Add Bold Tag In String", "string", ctx) {
  ctx.test("example_1_disjoint_tags", [&] {
    Solution sol;
    const std::vector<std::string> words = {"abc", "123"};
    ctx.expect_eq(sol.addBoldTag("abcxyz123", words),
                  "<b>abc</b>xyz<b>123</b>");
  });

  ctx.test("example_2_overlapping_and_adjacent", [&] {
    Solution sol;
    const std::vector<std::string> words = {"aaa", "aab", "bc"};
    ctx.expect_eq(sol.addBoldTag("aaabbcc", words), "<b>aaabbc</b>c");
  });

  ctx.test("python_build_prefix_unbolded", [&] {
    Solution sol;
    const std::vector<std::string> words = {"abc", "123"};
    ctx.expect_eq(sol.addBoldTag("cccabcxyz123", words),
                  "ccc<b>abc</b>xyz<b>123</b>");
  });

  ctx.test("consecutive_spans_merge", [&] {
    Solution sol;
    const std::vector<std::string> words = {"abc", "123"};
    ctx.expect_eq(sol.addBoldTag("abc123", words), "<b>abc123</b>");
  });

  ctx.test("no_matches_and_empty_inputs", [&] {
    Solution sol;
    const std::vector<std::string> words = {"xyz"};
    ctx.expect_eq(sol.addBoldTag("abcdef", words), "abcdef");
    ctx.expect_eq(sol.addBoldTag("abcdef", {}), "abcdef");
    ctx.expect_eq(sol.addBoldTag("", words), "");
  });

  ctx.test("repeated_overlapping_occurrences", [&] {
    Solution sol;
    const std::vector<std::string> words = {"aa"};
    ctx.expect_eq(sol.addBoldTag("baaaab", words), "b<b>aaaa</b>b");
  });
}

}  // namespace
