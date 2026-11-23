// LeetCode 211: Design Add and Search Words Data Structure
// https://leetcode.com/problems/design-add-and-search-words-data-structure/
//
// Problem:
//   Design a `WordDictionary` class supporting two operations:
//     - addWord(word): Adds a lowercase English word ('a'-'z') to the
//       dictionary.
//     - search(word): Returns true if any stored word matches `word`, where the
//       wildcard character '.' can match any single lowercase English letter.
//   Matches must span the entire word (a stored word's prefix does not match
//   unless that prefix was also added as a word). Typical constraints: word
//   length <= 25, at most 2-3 dots per query, up to 10^4 calls.
//
// Examples:
//   Example 1:
//     Input:  operations = ["WordDictionary", "addWord", "addWord", "addWord",
//             "search", "search", "search", "search", "search", "search",
//             "search"], args = [[], ["bad"], ["dad"], ["mad"], ["pad"], ["bad"],
//             [".ad"], ["b.."], ["..."], [".."], ["...."]]
//     Output: [null, null, null, null, false, true, true, true, true, false, false]
//     Explanation:
//       - addWord inserts "bad", "dad", and "mad" into three depth-3 paths from
//         root 0, setting is_end = true on each final 'd' node.
//       - search("pad") sees nodes_[0].children['p' - 'a'] == kNoChild (false),
//         while search("bad") walks 'b'->'a'->'d' to is_end = true (true).
//       - search(".ad") branches over root 0's non-zero children and matches
//         'b'->'a'->'d' (true), and search("b..") and search("...") similarly reach
//         a depth-3 terminal node via wildcard branching (true).
//       - search("..") terminates at depth 2 where is_end is false on every node
//         (false), and search("....") hits kNoChild at depth 3 (false).
//
//   Example 2:
//     Input:  operations = ["WordDictionary", "addWord", "search", "search",
//             "search", "search", "search", "search"], args = [[], ["ru"], ["."],
//             ["r"], [".."], ["r."], [".u"], ["..."]]
//     Output: [null, null, false, false, true, true, true, false]
//     Explanation:
//       - addWord("ru") allocates node 1 ('r', is_end = false) and node 2 ('u',
//         is_end = true) beneath root 0.
//       - search(".") and search("r") both reach node 1 at pos == 1 and return
//         nodes_[1].is_end (false), showing prefixes alone do not match.
//       - search(".."), search("r."), and search(".u") all reach node 2 at pos == 2
//         and return nodes_[2].is_end (true).
//       - search("...") tries to advance from node 2 at pos == 2, finds all
//         children equal to kNoChild, and returns false.
//
//   Example 3:
//     Input:  operations = ["WordDictionary", "search", "search", "addWord",
//             "addWord", "search", "search", "search", "search", "search",
//             "search"], args = [[], ["a"], ["."], ["a"], ["ab"], ["a"], ["."],
//             ["a."], [".b"], ["b."], ["a.."]]
//     Output: [null, false, false, null, null, true, true, true, true, false,
//             false]
//     Explanation:
//       - Before any insertions, root 0 has no children, so search("a") and
//         search(".") return false.
//       - addWord("a") marks node 1 ('a') with is_end = true, and addWord("ab")
//         appends node 2 ('b', is_end = true) as a child of node 1.
//       - search("a") and search(".") finish at node 1 (true), while search("a.")
//         and search(".b") finish at node 2 (true).
//       - search("b.") fails on missing 'b' at root 0 (false), and search("a..")
//         fails at node 2 because node 2 has no children (false).
//
// Algorithm:
//   Maintain an arena-backed prefix tree (`nodes_`) with node 0 as root and
//   index 0 (`kNoChild`) as the empty-child sentinel.
//   - addWord(word): Traverse from node 0 character by character, allocating
//     missing child nodes at the back of `nodes_`, and set `is_end = true` at
//     the terminal node.
//   - search(word): Run a recursive depth-first search `dfs(node_idx, word, pos)`:
//     - Base case: when `pos == word.size()`, return `nodes_[node_idx].is_end`.
//     - Exact letter: follow `children[c - 'a']` if present and recurse on
//       `pos + 1`.
//     - Wildcard '.': use `std::ranges::any_of` to try every non-empty child
//       slot, short-circuiting as soon as any branch returns true.
//
//   Complexity:
//     - addWord: Time O(L), Space O(L) new nodes, where L is word length.
//     - search:  Time O(L) with no wildcards, O(26^D * L) with D wildcards
//       (worst-case O(26^L) when all characters are '.'); Space O(L) call stack.
//
// Data Structures:
//   - `std::vector<Node> nodes_`: Contiguous arena of trie nodes indexed by
//     `std::size_t`, keeping cache locality high during DFS branching.
//   - `Node`: Holds `std::array<std::size_t, 26> children` (0 = no child) and
//     `bool is_end` marking valid word completions.
//
// Real-World Applications:
//   - Crossword solvers, Scrabble/word-game pattern lookups, and glob matching.
//   - Simple regular-expression and wildcard filtering in log or file search.
//   - Fuzzy dictionary lookup engines and DNS wildcard record resolution.
//
// Related LeetCode Problems:
//   - #208 Implement Trie [done] (Prefix Tree) (core trie without wildcard matching)
//   - #212 Word Search II [done] (combines a trie with backtracking DFS on a 2D grid)
//   - #676 Implement Magic Dictionary [done] (trie search with exactly one mismatch)
//   - #745 Prefix and Suffix Search [new] (extended trie queries on prefixes/suffixes)
//   - #1032 Stream of Characters [done] (matches stream suffixes via a reversed trie)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <string_view>
#include <vector>

namespace {

constexpr std::size_t kAlphabetSize = 26uz;
constexpr std::size_t kNoChild = 0uz;

constexpr std::size_t char_index(char c) {
  return static_cast<std::size_t>(static_cast<unsigned char>(c)) -
         static_cast<std::size_t>(static_cast<unsigned char>('a'));
}

// Arena-backed Trie with DFS wildcard branching for '.'.
// addWord: Time O(L), Space O(L) new nodes.
// search:  Time O(L) without wildcards, worst-case O(26^L) with all '.';
//          Space O(L) recursion stack.
class WordDictionary {
 public:
  WordDictionary() : nodes_(1uz) {}

  void addWord(std::string_view word) {
    std::size_t cur = 0uz;
    for (const char c : word) {
      const std::size_t idx = char_index(c);
      if (nodes_[cur].children[idx] == kNoChild) {
        const std::size_t next_id = nodes_.size();
        nodes_[cur].children[idx] = next_id;
        nodes_.emplace_back();
      }
      cur = nodes_[cur].children[idx];
    }
    nodes_[cur].is_end = true;
  }

  [[nodiscard]] bool search(std::string_view word) const {
    return dfs(0uz, word, 0uz);
  }

 private:
  struct Node {
    std::array<std::size_t, kAlphabetSize> children{};
    bool is_end = false;
  };

  [[nodiscard]] bool dfs(std::size_t node_idx, std::string_view word,
                         std::size_t pos) const {
    if (pos == word.size()) {
      return nodes_[node_idx].is_end;
    }
    const char c = word[pos];
    if (c == '.') {
      return std::ranges::any_of(
          nodes_[node_idx].children, [this, word, pos](std::size_t child) {
            return child != kNoChild && dfs(child, word, pos + 1uz);
          });
    }
    const std::size_t next = nodes_[node_idx].children[char_index(c)];
    return next != kNoChild && dfs(next, word, pos + 1uz);
  }

  std::vector<Node> nodes_;
};

LC_PROBLEM(211, "Add And Search Word Data Structure Design", "trie", ctx) {
  ctx.test("leetcode_example", [&] {
    WordDictionary dict;
    dict.addWord("bad");
    dict.addWord("dad");
    dict.addWord("mad");

    ctx.expect_false(dict.search("pad"));
    ctx.expect_true(dict.search("bad"));
    ctx.expect_true(dict.search(".ad"));
    ctx.expect_true(dict.search("b.."));
    ctx.expect_true(dict.search("..."));
    ctx.expect_false(dict.search(".."));
    ctx.expect_false(dict.search("...."));
  });

  ctx.test("whole_word_not_prefix", [&] {
    WordDictionary dict;
    dict.addWord("ru");
    ctx.expect_false(dict.search("."));
    ctx.expect_false(dict.search("r"));
    ctx.expect_true(dict.search(".."));
    ctx.expect_true(dict.search("r."));
    ctx.expect_true(dict.search(".u"));
    ctx.expect_false(dict.search("..."));
  });

  ctx.test("empty_and_overlapping_prefixes", [&] {
    WordDictionary dict;
    ctx.expect_false(dict.search("a"));
    ctx.expect_false(dict.search("."));

    dict.addWord("a");
    dict.addWord("ab");
    ctx.expect_true(dict.search("a"));
    ctx.expect_true(dict.search("."));
    ctx.expect_true(dict.search("a."));
    ctx.expect_true(dict.search(".b"));
    ctx.expect_false(dict.search("b."));
    ctx.expect_false(dict.search("a.."));
  });
}

}  // namespace
