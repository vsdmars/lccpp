// https://leetcode.com/problems/longest-word-in-dictionary/
//
// Given an array of lowercase English strings `words`, return the longest word
// that can be built one character at a time by other words in `words`. Break
// ties by returning the lexicographically smallest word; return "" if none.

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
