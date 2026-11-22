// https://leetcode.com/problems/word-search-ii/description/
//
// Given an m x n board of lowercase letters and a list of dictionary words,
// return all words that can be formed by sequentially adjacent (horizontally
// or vertically) cells without reusing the same cell within a single word.

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace {

constexpr std::size_t kAlphabetSize = 26uz;
constexpr std::size_t kNoChild = 0uz;
constexpr char kVisited = '#';
constexpr std::array<std::pair<int, int>, 4> kDirs{
    {{0, 1}, {0, -1}, {1, 0}, {-1, 0}}};

constexpr std::size_t char_index(char c) {
  return static_cast<std::size_t>(static_cast<unsigned char>(c)) -
         static_cast<std::size_t>(static_cast<unsigned char>('a'));
}

// Trie + Backtracking DFS.
// Build a trie from `words`, then DFS from every cell on `board` following
// matching trie edges. Once a word is matched, clear its marker on the trie
// node to prevent duplicates while continuing DFS for longer words sharing the
// same prefix.
// Time: O(W * L + M * N * 4 * 3^(L - 1)), Space: O(W * L).
class Solution {
 public:
  std::vector<std::string> findWords(std::vector<std::vector<char>>& board,
                                     const std::vector<std::string>& words) {
    if (board.empty() || board[0].empty() || words.empty()) {
      return {};
    }

    std::vector<TrieNode> nodes = build_trie(words);
    std::vector<std::string> result;

    const std::size_t rows = board.size();
    const std::size_t cols = board[0].size();
    for (std::size_t r = 0uz; r < rows; ++r) {
      for (std::size_t c = 0uz; c < cols; ++c) {
        const std::size_t next = nodes[0].children[char_index(board[r][c])];
        if (next != kNoChild) {
          dfs(board, words, nodes, next, r, c, result);
        }
      }
    }
    return result;
  }

 private:
  struct TrieNode {
    std::array<std::size_t, kAlphabetSize> children{};
    std::optional<std::size_t> word_idx;
  };

  static std::vector<TrieNode> build_trie(
      const std::vector<std::string>& words) {
    std::vector<TrieNode> nodes(1uz);
    for (std::size_t wi = 0uz; wi < words.size(); ++wi) {
      std::size_t cur = 0uz;
      for (const char c : words[wi]) {
        const std::size_t idx = char_index(c);
        if (nodes[cur].children[idx] == kNoChild) {
          const std::size_t next_id = nodes.size();
          nodes[cur].children[idx] = next_id;
          nodes.emplace_back();
        }
        cur = nodes[cur].children[idx];
      }
      nodes[cur].word_idx = wi;
    }
    return nodes;
  }

  static void dfs(std::vector<std::vector<char>>& board,
                  const std::vector<std::string>& words,
                  std::vector<TrieNode>& nodes, std::size_t node_idx,
                  std::size_t r, std::size_t c,
                  std::vector<std::string>& result) {
    if (nodes[node_idx].word_idx.has_value()) {
      result.push_back(words[*nodes[node_idx].word_idx]);
      // Clear marker to avoid duplicates, but keep exploring longer words.
      nodes[node_idx].word_idx.reset();
    }

    const std::size_t rows = board.size();
    const std::size_t cols = board[0].size();
    const char saved = board[r][c];
    board[r][c] = kVisited;

    for (const auto [dr, dc] : kDirs) {
      const int nr_i = static_cast<int>(r) + dr;
      const int nc_i = static_cast<int>(c) + dc;
      if (nr_i < 0 || nc_i < 0) {
        continue;
      }
      const auto nr = static_cast<std::size_t>(nr_i);
      const auto nc = static_cast<std::size_t>(nc_i);
      if (nr >= rows || nc >= cols || board[nr][nc] == kVisited) {
        continue;
      }
      const std::size_t next =
          nodes[node_idx].children[char_index(board[nr][nc])];
      if (next != kNoChild) {
        dfs(board, words, nodes, next, nr, nc, result);
      }
    }

    board[r][c] = saved;
  }
};

LC_PROBLEM(212, "Word Search II", "trie", ctx) {
  ctx.test("leetcode_example", [&] {
    std::vector<std::vector<char>> board{
        {'o', 'a', 'a', 'n'},
        {'e', 't', 'a', 'e'},
        {'i', 'h', 'k', 'r'},
        {'i', 'f', 'l', 'v'},
    };
    const std::vector<std::string> words{"oath", "pea", "eat", "rain"};
    auto actual = Solution().findWords(board, words);
    std::ranges::sort(actual);
    ctx.expect_eq(actual, std::vector<std::string>{"eat", "oath"});
  });

  ctx.test("python_build_case", [&] {
    std::vector<std::vector<char>> board{
        {'o', 'a', 'b', 'n'},
        {'o', 't', 'a', 'e'},
        {'a', 'h', 'k', 'r'},
        {'a', 'f', 'l', 'v'},
    };
    const std::vector<std::string> words{"oath", "eat"};
    auto actual = Solution().findWords(board, words);
    std::ranges::sort(actual);
    ctx.expect_eq(actual, std::vector<std::string>{"eat", "oath"});
  });

  ctx.test("prefix_and_no_cell_reuse", [&] {
    std::vector<std::vector<char>> board{
        {'o', 'a'},
        {'h', 't'},
    };
    const std::vector<std::string> words{"o", "oa", "oat", "oath", "oato"};
    auto actual = Solution().findWords(board, words);
    std::ranges::sort(actual);
    ctx.expect_eq(actual, std::vector<std::string>{"o", "oa", "oat", "oath"});
    // Ensure board is cleanly restored after backtracking.
    ctx.expect_eq(board[0][0], 'o');
    ctx.expect_eq(board[1][1], 't');
  });

  ctx.test("single_cell_and_empty", [&] {
    std::vector<std::vector<char>> single{{'a'}};
    ctx.expect_eq(Solution().findWords(single, {"a", "aa"}),
                  std::vector<std::string>{"a"});

    std::vector<std::vector<char>> empty_board;
    ctx.expect_true(Solution().findWords(empty_board, {"a"}).empty());
  });
}

}  // namespace
