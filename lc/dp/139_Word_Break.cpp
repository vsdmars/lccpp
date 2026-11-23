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
// Examples:
//   Example 1:
//     Input:  s = "leetcode", wordDict = ["leet", "code"]
//     Output: true
//     Explanation:
//       - Insert "leet" and "code" into dict with max_word_len = 4, and initialize
//         dp[0] = true and dp[1..8] = false.
//       - For r = 1..3, no prefix s[0..r-1] matches dict, while at r = 4 (len = 4,
//         l = 0), dp[0] is true and s.substr(0, 4) = "leet" is in dict, setting
//         dp[4] = true.
//       - For r = 5..7, only dp[0] and dp[4] are true among prior positions, and no
//         candidate slice within max_word_len = 4 matches dict, leaving dp[5..7] =
//         false.
//       - At r = 8 with len = 4 (l = 4), dp[4] is true and s.substr(4, 4) = "code"
//         is in dict, setting dp[8] = true and returning true.
//
//   Example 2:
//     Input:  s = "catsandog", wordDict = ["cats", "dog", "sand", "and", "cat"]
//     Output: false
//     Explanation:
//       - Populate dict with the five words (max_word_len = 4), and initialize
//         dp[0] = true and dp[1..9] = false.
//       - At r = 3 and r = 4 with l = 0, s.substr(0, 3) = "cat" and s.substr(0, 4)
//         = "cats" are in dict, setting dp[3] = true and dp[4] = true.
//       - At r = 7, l = 3 checks s.substr(3, 4) = "sand" with dp[3] = true (and l =
//         4 would also match "and"), setting dp[7] = true while dp[5], dp[6], and
//         dp[8] stay false.
//       - At r = 9, checking len = 1..4 probes l = 8, 7, 6, 5; only dp[7] is true
//         and s.substr(7, 2) = "og" is absent from dict, so dp[9] remains false and
//         wordBreak returns false.
//
//   Example 3:
//     Input:  s = "aaaaaaa", wordDict = ["aaaa", "aaa"]
//     Output: true
//     Explanation:
//       - Build dict = {"aaaa", "aaa"} with max_word_len = 4, and initialize dp[0]
//         = true and dp[1..7] = false.
//       - dp[1] and dp[2] stay false, while l = 0 matches "aaa" at r = 3 (dp[3] =
//         true) and "aaaa" at r = 4 (dp[4] = true).
//       - dp[5] stays false because no split of length 1..4 reaches a true prefix,
//         and at r = 6 with l = 3, dp[3] = true and s.substr(3, 3) = "aaa" set
//         dp[6] = true.
//       - At r = 7, len = 3 (l = 4) finds dp[4] = true and s.substr(4, 3) = "aaa"
//         in dict, setting dp[7] = true and returning true.
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
