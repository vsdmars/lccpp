// LeetCode 720: Longest Word in Dictionary
// https://leetcode.com/problems/longest-word-in-dictionary/
//
// Problem:
//   Given an array of lowercase English strings `words`, find and return the
//   longest word in `words` that can be built one character at a time from
//   other words in `words` (i.e., every non-empty prefix of the word is also
//   present in `words`). If multiple valid words share the maximum length,
//   return the lexicographically smallest one; return `""` if none exists.
//   Typical constraints: 1 <= words.length <= 1000, 1 <= words[i].length <= 30.
//
// Algorithm:
//   Trie Construction + Lexicographical Prefix-Gated DFS:
//   1. Insert every word from `words` into an arena-backed trie (`nodes`),
//      setting `is_end = true` on each word's terminal node.
//   2. Run a recursive DFS starting from root (0) with a mutable path string
//      `current` and best answer `best`:
//      - At each visited node, if `current.size() > best.size()`, update
//        `best = current`.
//      - Iterate child slots `idx = 0..25` (strictly `'a'` through `'z'`).
//      - Only descend into `child` if `child != kNoChild && nodes[child].is_end`.
//        Push the character onto `current`, recurse, and pop on backtrack.
//   Key invariants:
//   - Gating every step on `nodes[child].is_end` guarantees that every prefix
//     along `current` is a dictionary word, pruning invalid subtrees immediately.
//   - Visiting children in `'a'..'z'` order and updating `best` only on strict
//     length increases (`>`) ensures ties automatically keep the
//     lexicographically smallest word without extra string comparisons.
//
//   Complexity:
//     - Time: O(S * 26) = O(S), where S is the sum of all word lengths.
//     - Space: O(S * 26) for the trie nodes plus O(L_max) for DFS stack and
//       the path buffers.
//
// Data Structures:
//   - `std::vector<Node> nodes`: Contiguous trie arena where each `Node` holds
//     `std::array<std::size_t, 26> children` (0 = absent) and `bool is_end`.
//   - `std::string current` & `std::string best`: Reusable path buffer for
//     backtracking DFS and the longest valid word found so far.
//
// Real-World Applications:
//   - Hierarchical namespace / taxonomy validation (ensuring every parent path
//     or package prefix is registered before allowing nested entries).
//   - File-system and URL-route completeness checks where every ancestor
//     directory or route segment must exist.
//   - Word-chain puzzle engines and incremental dictionary compression.
//
// Related LeetCode Problems:
//   - #208 Implement Trie (Prefix Tree) (foundational trie data structure)
//   - #1858 Longest Word With All Prefixes (same prefix-chain requirement)
//   - #648 Replace Words (traverses trie prefixes to find dictionary roots)
//   - #524 Longest Word in Dictionary through Deleting (longest/lex-smallest)
//   - #820 Short Encoding of Words (uses a trie to collapse prefix/suffix words)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <string>
#include <vector>

namespace {

constexpr std::size_t kAlphabetSize = 26uz;
constexpr std::size_t kNoChild = 0uz;

constexpr std::size_t char_index(char c) {
  return static_cast<std::size_t>(static_cast<unsigned char>(c)) -
         static_cast<std::size_t>(static_cast<unsigned char>('a'));
}

constexpr char index_to_char(std::size_t idx) {
  return static_cast<char>(static_cast<unsigned char>('a') +
                           static_cast<unsigned char>(idx));
}

// Trie + Lexicographical DFS.
// Insert all words into a trie, then DFS starting from the root while only
// descending into children marked `is_end == true`. Because children are
// visited in 'a'..'z' order, updating `best` only on strict length increases
// automatically keeps the lexicographically smallest answer on ties.
// Time: O(sum of word lengths), Space: O(sum of word lengths * 26).
class Solution {
 public:
  std::string longestWord(const std::vector<std::string>& words) {
    std::vector<Node> nodes(1uz);
    for (const std::string& word : words) {
      std::size_t cur = 0uz;
      for (const char c : word) {
        const std::size_t idx = char_index(c);
        if (nodes[cur].children[idx] == kNoChild) {
          const std::size_t next_id = nodes.size();
          nodes[cur].children[idx] = next_id;
          nodes.emplace_back();
        }
        cur = nodes[cur].children[idx];
      }
      nodes[cur].is_end = true;
    }

    std::string current;
    std::string best;
    dfs(nodes, 0uz, current, best);
    return best;
  }

 private:
  struct Node {
    std::array<std::size_t, kAlphabetSize> children{};
    bool is_end = false;
  };

  static void dfs(const std::vector<Node>& nodes, std::size_t node_idx,
                  std::string& current, std::string& best) {
    if (current.size() > best.size()) {
      best = current;
    }
    for (std::size_t idx = 0uz; idx < kAlphabetSize; ++idx) {
      const std::size_t child = nodes[node_idx].children[idx];
      if (child != kNoChild && nodes[child].is_end) {
        current.push_back(index_to_char(idx));
        dfs(nodes, child, current, best);
        current.pop_back();
      }
    }
  }
};

LC_PROBLEM(720, "Longest Word In Dictionary", "trie", ctx) {
  ctx.test("example_1", [&] {
    ctx.expect_eq(Solution().longestWord({"w", "wo", "wor", "worl", "world"}),
                  "world");
  });

  ctx.test("example_2_lexicographical_tie", [&] {
    ctx.expect_eq(
        Solution().longestWord(
            {"a", "banana", "app", "appl", "ap", "apply", "apple"}),
        "apple");
  });

  ctx.test("missing_prefix_and_no_valid_word", [&] {
    ctx.expect_eq(Solution().longestWord({"ab", "abc", "cd"}), "");
    ctx.expect_eq(Solution().longestWord({"a", "ab", "abcd"}), "ab");
  });

  ctx.test("single_chars_and_branch_tie", [&] {
    ctx.expect_eq(Solution().longestWord({"z", "b", "a"}), "a");
    ctx.expect_eq(
        Solution().longestWord({"b", "br", "bre", "a", "ap", "app"}), "app");
  });
}

}  // namespace
