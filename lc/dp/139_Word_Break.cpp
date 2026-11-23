// LeetCode 139: Word Break
// https://leetcode.com/problems/word-break/
//
// Problem:
//   Given a string s and a dictionary of strings wordDict, return true if s
//   can be segmented into a space-separated sequence of one or more dictionary
//   words. The same dictionary word may be reused multiple times.
//   Constraints: 1 <= s.length <= 300; 1 <= wordDict.length <= 1000;
//   1 <= wordDict[i].length <= 20; lowercase English letters only.
//
// Algorithm:
//   Bottom-up 1D Dynamic Programming over string prefix lengths:
//   - Insert all dictionary words into an unordered_set<std::string_view> for
//     O(1) average lookup and track `max_word_len` = longest word length.
//   - Define dp[r] = true iff the prefix s[0..r-1] can be segmented into valid
//     dictionary words.
//   - Base case: dp[0] = true (empty prefix is trivially segmented).
//   - For each end index r from 1 to n, check candidate last-word lengths
//     `len` from 1 up to min(r, max_word_len):
//       * Let l = r - len. If dp[l] is true and s.substr(l, len) is in the
//         dictionary set, set dp[r] = true and break early.
//   - Return dp[n].
//   - Bounding `len <= max_word_len` avoids futile lookups for substrings
//     longer than any dictionary entry.
//   - Time Complexity: O(n * L^2 + M), where n = |s|, L = max word length in
//     wordDict (hashing each substring of length up to L), and M = total
//     characters across wordDict.
//   - Space Complexity: O(n + W) for the DP table and view-based hash set
//     (where W = wordDict.size(), with zero string copies).
//
// Data Structures:
//   - 1D boolean DP table (std::vector<bool>) of size n + 1.
//   - Hash set of string views (std::unordered_set<std::string_view>) providing
//     zero-copy O(1) expected dictionary membership queries.
//
// Real-World Applications:
//   - Tokenization and word segmentation for unspaced languages (Chinese,
//     Japanese, Thai) and compound-word splitting (German, URL slugs, domain
//     names, hashtags).
//   - Search-engine query segmentation and spell-checker compound recovery.
//   - Lexical analysis in compilers and protocol parsers matching greedy or
//     valid token decompositions.
//
// Related LeetCode Problems:
//   - #132 Palindrome Partitioning II [done] (same 1D prefix-segmentation DP structure)
//   - #140 Word Break II [done] (returns all valid segmentations via memoized DFS)
//   - #472 Concatenated Words [new] (applies Word Break DP to every word in a dictionary)
//   - #2707 Extra Characters in a String [new] (minimizes leftover characters using the same prefix DP)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace {

class Solution {
 public:
  // Determines whether `s` can be segmented into a sequence of words from
  // `word_dict` using bottom-up 1D prefix DP.
  //
  // Invariant: after iteration `r`, dp[r] is true iff prefix s[0..r-1] admits
  // at least one valid segmentation into dictionary words.
  // Time: O(n * L^2 + M), Space: O(n + W).
  bool wordBreak(std::string_view s,
                 const std::vector<std::string>& word_dict) {
    if (word_dict.empty()) {
      return false;
    }

    // Build a zero-copy view set for fast O(1) lookups and record the maximum
    // word length to prune impossible splits.
    std::unordered_set<std::string_view> dict;
    dict.reserve(word_dict.size());
    std::size_t max_word_len = 0uz;
    for (const std::string& word : word_dict) {
      dict.insert(word);
      max_word_len = std::max(max_word_len, word.size());
    }

    const std::size_t n = s.size();
    // dp[r] = true iff s[0..r-1] can be segmented using `dict`.
    std::vector<bool> dp(n + 1, false);
    dp[0] = true;  // Base case: empty prefix requires zero words.

    for (std::size_t r = 1uz; r <= n; ++r) {
      const std::size_t max_len = std::min(r, max_word_len);
      for (std::size_t len = 1uz; len <= max_len; ++len) {
        const std::size_t l = r - len;
        // Check cheap boolean dp[l] before hashing the substring slice.
        if (dp[l] && dict.contains(s.substr(l, len))) {
          dp[r] = true;
          break;
        }
      }
    }

    return dp[n];
  }
};

LC_PROBLEM(139, "Word Break", "dp", ctx) {
  Solution sol;

  ctx.test("example_1_leetcode", [&] {
    ctx.expect_true(sol.wordBreak("leetcode", {"leet", "code"}));
  });

  ctx.test("example_2_word_reuse", [&] {
    ctx.expect_true(sol.wordBreak("applepenapple", {"apple", "pen"}));
  });

  ctx.test("example_3_unsegmentable", [&] {
    ctx.expect_false(
        sol.wordBreak("catsandog", {"cats", "dog", "sand", "and", "cat"}));
  });

  ctx.test("python_build_and_boundary_cases", [&] {
    ctx.expect_true(sol.wordBreak("lcoa", {"l", "c", "oa"}));
    ctx.expect_false(sol.wordBreak("leetX", {"leet", "code"}));
  });

  ctx.test("overlapping_prefix_choices", [&] {
    // Requires choosing "aaaa" + "aaa" rather than greedy "aaa" + "aaa" + "a".
    ctx.expect_true(sol.wordBreak("aaaaaaa", {"aaaa", "aaa"}));
    ctx.expect_false(sol.wordBreak("aaaaaaa", {"aaaa", "aa"}));
  });

  ctx.test("empty_dictionary", [&] {
    ctx.expect_false(sol.wordBreak("a", {}));
  });
}

}  // namespace
