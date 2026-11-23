// LeetCode 212: Word Search II
// https://leetcode.com/problems/word-search-ii/
//
// Problem:
//   Given an m x n grid `board` of lowercase English letters and a list of
//   dictionary strings `words`, return all distinct words from `words` that can
//   be constructed on the board. Each word must follow sequentially adjacent
//   cells (up, down, left, right), and the same board cell cannot be reused
//   more than once within a single word. Typical constraints: 1 <= m, n <= 12,
//   1 <= words.length <= 3 * 10^4, 1 <= words[i].length <= 10.
//
// Examples:
//   Example 1:
//     Input:  board =
//             [['o','a','a','n'],['e','t','a','e'],['i','h','k','r'],['i','f','l','v']],
//             words = ["oath", "pea", "eat", "rain"]
//     Output: ["eat", "oath"]
//     Explanation:
//       - build_trie indexes all four words into the trie arena, assigning word_idx
//         0..3 on their terminal nodes.
//       - Scanning the board finds 'o' at (0, 0), so DFS follows 'o' -> 'a' at (0,
//         1) -> 't' at (1, 1) -> 'h' at (2, 1), records "oath", and resets its
//         word_idx so it cannot be duplicated.
//       - DFS from 'e' at (1, 3) follows 'e' -> 'a' at (1, 2) -> 't' at (1, 1),
//         records "eat", and resets its word_idx; starting cells for "pea" (none)
//         and "rain" ('r' at (2, 3) has no 'a' neighbor) prune immediately.
//       - Sorting the collected matches yields ["eat", "oath"].
//
//   Example 2:
//     Input:  board = [['o','a'],['h','t']], words = ["o", "oa", "oat", "oath",
//             "oato"]
//     Output: ["o", "oa", "oat", "oath"]
//     Explanation:
//       - build_trie builds a single prefix chain for "o", "oa", "oat", "oath", and
//         "oato" with terminal word_idx markers at depths 1 through 4 (and
//         branching to 'o' after "oat").
//       - Starting at (0, 0) = 'o', DFS records "o", marks (0, 0) as '#', and
//         continues along (0, 1) = 'a' -> (1, 1) = 't' -> (1, 0) = 'h', collecting
//         "oa", "oat", and "oath" without stopping at shorter words.
//       - When checking neighbors of (1, 1) = 't' for "oato", cell (0, 0) is marked
//         '#' (kVisited), preventing cell reuse; backtracking restores every '#'
//         cell back to its original letter.
//
//   Example 3:
//     Input:  board = [['a']], words = ["a", "aa"]
//     Output: ["a"]
//     Explanation:
//       - build_trie inserts "a" (depth 1) and "aa" (depth 2) into the trie.
//       - DFS starts at (0, 0) = 'a', immediately matches word_idx 0 ("a"), and
//         marks (0, 0) as '#'.
//       - All four neighbor offsets from (0, 0) are out of bounds on the 1x1 board,
//         so "aa" cannot be extended and only ["a"] is returned.
//
// Algorithm:
//   Trie-Guided Backtracking DFS:
//   1. Build a single arena-backed trie containing every word in `words`,
//      storing the word's index (`word_idx`) on its terminal node.
//   2. For every cell `(r, c)` on `board`, check whether the root has a child
//      edge matching `board[r][c]`. If so, launch backtracking DFS from that
//      cell and child trie node.
//   3. During `dfs`, if the current trie node holds a `word_idx`, append
//      `words[*word_idx]` to `result` and immediately `reset()` `word_idx` so
//      the same word is never reported twice, while still continuing deeper to
//      discover longer words sharing the same prefix.
//   4. Mark `board[r][c]` in place with `kVisited` ('#') before exploring its
//      4 orthogonal neighbors whose letters exist in the current trie node's
//      children, then restore the original letter on backtrack.
//   Key invariant: A path on the board is pruned the instant its prefix ceases
//   to exist in the trie, searching all dictionary words simultaneously.
//
//   Complexity:
//     - Time: O(W * L + M * N * 4 * 3^(L - 1)), where W = words.size(),
//       L = max word length, and M x N are the board dimensions (at most 3
//       unvisited neighbor directions after the first step).
//     - Space: O(W * L * 26) for the trie arena plus O(L) recursion depth.
//
// Data Structures:
//   - `std::vector<TrieNode> nodes`: Flat arena-backed prefix tree indexing all
//     candidate words; each `TrieNode` has `std::array<std::size_t, 26>
//     children` and `std::optional<std::size_t> word_idx`.
//   - `board` (mutated in place): Temporarily marked with `'#'` (`kVisited`) to
//     track the active DFS path without allocating a separate visited matrix.
//   - `kDirs`: Fixed 4-element array of orthogonal `(dr, dc)` offsets.
//
// Real-World Applications:
//   - Automated solvers for Boggle, Wordament, and similar word-grid puzzles.
//   - Multi-pattern spatial/2D symbol scanning in OCR and image-grid analysis.
//   - Dictionary-pruned path search in bioinformatics sequence-motif matching.
//
// Related LeetCode Problems:
//   - #79 Word Search [done] (single-word 2D grid backtracking without a trie)
//   - #208 Implement Trie [done] (Prefix Tree) (foundational trie indexing structure)
//   - #211 Design Add and Search Words Data Structure [done] (DFS over trie branches)
//   - #472 Concatenated Words [new] (matches multiple dictionary words via trie/DFS)
//   - #1032 Stream of Characters [done] (multi-word matching using a trie)

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
