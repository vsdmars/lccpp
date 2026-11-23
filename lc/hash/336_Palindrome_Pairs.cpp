// LeetCode 336: Palindrome Pairs
// https://leetcode.com/problems/palindrome-pairs/
//
// Problem:
//   Given a 0-indexed list of unique strings `words`, find all pairs of
//   distinct indices `(i, j)` (`i != j`) such that the concatenation
//   `words[i] + words[j]` forms a palindrome. Strings in `words` may include
//   the empty string `""`.
//
// Examples:
//   Example 1:
//     Input:  words = ["bat", "tab", "cat"]
//     Output: [[0, 1], [1, 0]]
//     Explanation:
//       - Build the reversed-word map rev_to_idx = {"tab": 0, "bat": 1, "tac": 2}.
//       - For words[0] = "bat" at cut = 0, prefix = "" is a palindrome and suffix =
//         "bat" matches rev_to_idx["bat"] = 1, so Case 1 emits [1, 0] ("tabbat").
//       - For words[1] = "tab" at cut = 0, prefix = "" is a palindrome and suffix =
//         "tab" matches rev_to_idx["tab"] = 0, so Case 1 emits [0, 1] ("battab").
//       - All splits of "cat" miss in rev_to_idx and cut == len is skipped in Case
//         2, leaving [[0, 1], [1, 0]] after sorting.
//
//   Example 2:
//     Input:  words = ["abcd", "dcba", "lls", "s", "sssll"]
//     Output: [[0, 1], [1, 0], [2, 4], [3, 2]]
//     Explanation:
//       - Build rev_to_idx = {"dcba": 0, "abcd": 1, "sll": 2, "s": 3, "llsss": 4}.
//       - At cut = 0, words[0] = "abcd" matches rev_to_idx["abcd"] = 1 to emit [1,
//         0], and words[1] = "dcba" matches rev_to_idx["dcba"] = 0 to emit [0, 1].
//       - For words[2] = "lls" at cut = 2, prefix = "ll" is a palindrome and suffix
//         = "s" matches rev_to_idx["s"] = 3, emitting [3, 2] ("slls").
//       - For words[4] = "sssll" at cut = 2, prefix = "ss" is a palindrome and
//         suffix = "sll" matches rev_to_idx["sll"] = 2, emitting [2, 4]
//         ("llssssll") for a sorted result of [[0, 1], [1, 0], [2, 4], [3, 2]].
//
//   Example 3:
//     Input:  words = ["a", ""]
//     Output: [[0, 1], [1, 0]]
//     Explanation:
//       - Build rev_to_idx = {"a": 0, "": 1}.
//       - For words[0] = "a" at cut = 0, Case 1 skips "a" because it maps to
//         self-index 0, while Case 2 sees palindrome suffix = "a" and matches
//         prefix = "" to rev_to_idx[""] = 1, emitting [0, 1].
//       - For words[0] = "a" at cut = 1, Case 1 sees palindrome prefix = "a" and
//         matches suffix = "" to rev_to_idx[""] = 1, emitting [1, 0], while Case 2
//         skips cut == len.
//       - For words[1] = "" at cut = 0, suffix = "" only matches self-index 1, so
//         no additional pairs are added and sorting yields [[0, 1], [1, 0]].
//
// Algorithm:
//   Pre-index the reverse of every word (`rev(words[idx]) -> idx`) in a hash
//   map with transparent `std::string_view` lookup so inner-loop queries
//   allocate zero memory.
//   For each word `w = words[k]` of length `L`, split `w` at every cut point
//   `cut` in `[0, L]` into `prefix = w[0..cut)` and `suffix = w[cut..L)`:
//     1. If `prefix` is a palindrome, then `rev(suffix) + w` is a palindrome
//        (`rev(suffix) + prefix + suffix`). Look up `suffix` in `rev_to_idx`;
//        if found at `other != k`, emit `{other, k}`.
//     2. If `cut != L` and `suffix` is a palindrome, then `w + rev(prefix)` is
//        a palindrome (`prefix + suffix + rev(prefix)`). Look up `prefix` in
//        `rev_to_idx`; if found at `other != k`, emit `{k, other}`.
//   Excluding `cut == L` from Case 2 prevents duplicate pairs when two words
//   are exact reverses of one another (already handled by Case 1 at `cut == 0`
//   when visiting the partner word).
//   Time Complexity: O(N * L^2), where N = words.size() and L = max word length.
//   Space Complexity: O(N * L) for the reversed-word hash map.
//
// Data Structures:
//   - `std::unordered_map<std::string, int, TransparentStringHash,
//     std::equal_to<>>`: stores reversed words for O(L) zero-allocation lookup
//     using `std::string_view` slices.
//
// Real-World Applications:
//   - Bioinformatics DNA/RNA primer and restriction-site design, where joining
//     two short oligo fragments must avoid or create palindromic hairpins.
//   - String-matching indices and dictionary join engines performing symmetric
//     prefix/suffix lookups over large vocabularies.
//   - Word-puzzle solvers and cryptographic/coding-theory symmetry checks.
//
// Related LeetCode Problems:
//   - #5 Longest Palindromic Substring [done] (palindrome detection and expansion)
//   - #125 Valid Palindrome [done] (two-pointer palindrome verification)
//   - #214 Shortest Palindrome [done] (finding the longest palindromic prefix)
//   - #208 Implement Trie [done] (Prefix Tree) (alternative trie-based approach to palindrome pairs)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

// Heterogeneous hasher allowing zero-allocation std::string_view lookups in a
// std::unordered_map keyed by std::string.
struct TransparentStringHash {
  using is_transparent = void;

  [[nodiscard]] std::size_t operator()(std::string_view text) const noexcept {
    return std::hash<std::string_view>{}(text);
  }
};

using ReversedWordMap = std::unordered_map<std::string, int,
                                           TransparentStringHash,
                                           std::equal_to<>>;

class Solution {
 public:
  // Finds all distinct index pairs `(i, j)` such that `words[i] + words[j]`
  // is a palindrome.
  // Time: O(N * L^2), Space: O(N * L).
  std::vector<std::vector<int>> palindromePairs(
      const std::vector<std::string>& words) {
    // Store each reversed word -> original index so checking whether
    // `rev(slice)` exists in `words` becomes a direct lookup of `slice`.
    ReversedWordMap rev_to_idx;
    rev_to_idx.reserve(words.size());
    for (std::size_t i = 0uz; i < words.size(); ++i) {
      std::string rev = words[i];
      std::ranges::reverse(rev);
      rev_to_idx.emplace(std::move(rev), static_cast<int>(i));
    }

    std::vector<std::vector<int>> pairs;
    for (std::size_t i = 0uz; i < words.size(); ++i) {
      collectPairsForWord(words[i], static_cast<int>(i), rev_to_idx, pairs);
    }
    return pairs;
  }

 private:
  // Returns true iff `text` reads the same forwards and backwards.
  [[nodiscard]] static bool isPalindrome(std::string_view text) noexcept {
    std::size_t left = 0uz;
    std::size_t right = text.size();
    while (left < right) {
      --right;
      if (text[left] != text[right]) {
        return false;
      }
      ++left;
    }
    return true;
  }

  // Evaluates every prefix/suffix split of `word` and appends all valid
  // palindrome pairs involving `word_idx`.
  static void collectPairsForWord(std::string_view word, int word_idx,
                                  const ReversedWordMap& rev_to_idx,
                                  std::vector<std::vector<int>>& pairs) {
    const std::size_t len = word.size();
    for (std::size_t cut = 0uz; cut <= len; ++cut) {
      const std::string_view prefix = word.substr(0uz, cut);
      const std::string_view suffix = word.substr(cut);

      // Case 1: `prefix` is the palindrome center; prepend `rev(suffix)`.
      if (isPalindrome(prefix)) {
        if (const auto it = rev_to_idx.find(suffix);
            it != rev_to_idx.end() && it->second != word_idx) {
          pairs.push_back({it->second, word_idx});
        }
      }

      // Case 2: `suffix` is the palindrome center; append `rev(prefix)`.
      // Skip `cut == len` (empty suffix) to avoid duplicating exact-reverse
      // pairs already emitted by Case 1 at `cut == 0` on the partner word.
      if (cut != len && isPalindrome(suffix)) {
        if (const auto it = rev_to_idx.find(prefix);
            it != rev_to_idx.end() && it->second != word_idx) {
          pairs.push_back({word_idx, it->second});
        }
      }
    }
  }
};

LC_PROBLEM(336, "Palindrome Pairs", "hash", ctx) {
  auto sorted_pairs = [](std::vector<std::vector<int>> res) {
    std::ranges::sort(res);
    return res;
  };

  ctx.test("example_1", [&] {
    Solution sol;
    const std::vector<std::vector<int>> expected = {{0, 1}, {1, 0}};
    ctx.expect_eq(sorted_pairs(sol.palindromePairs({"bat", "tab", "cat"})),
                  expected);
  });

  ctx.test("example_2", [&] {
    Solution sol;
    const std::vector<std::vector<int>> expected = {
        {0, 1}, {1, 0}, {2, 4}, {3, 2}};
    ctx.expect_eq(sorted_pairs(sol.palindromePairs(
                      {"abcd", "dcba", "lls", "s", "sssll"})),
                  expected);
  });

  ctx.test("empty_string_with_palindromes_and_non_palindromes", [&] {
    Solution sol;
    ctx.expect_eq(sorted_pairs(sol.palindromePairs({"a", ""})),
                  std::vector<std::vector<int>>{{0, 1}, {1, 0}});
    ctx.expect_eq(sorted_pairs(sol.palindromePairs({"", "ab"})),
                  std::vector<std::vector<int>>{});
  });

  ctx.test("overlapping_prefix_and_suffix_palindromes", [&] {
    Solution sol;
    const std::vector<std::vector<int>> expected = {{0, 1}, {1, 0}};
    ctx.expect_eq(sorted_pairs(sol.palindromePairs({"a", "aa"})), expected);
  });

  ctx.test("no_pairs", [&] {
    Solution sol;
    ctx.expect_eq(sol.palindromePairs({"abc", "def", "ghi"}),
                  std::vector<std::vector<int>>{});
  });
}

}  // namespace
