// https://leetcode.com/problems/add-and-search-word-data-structure-design/description/
//
// Design a data structure that supports adding lowercase words and searching
// for a word where '.' matches any single lowercase letter ('a'-'z').

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
