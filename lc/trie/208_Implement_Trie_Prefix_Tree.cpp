// https://leetcode.com/problems/implement-trie-prefix-tree/description/
//
// Implement a trie (prefix tree) supporting `insert`, `search`, and
// `startsWith` operations over lowercase English letters ('a'-'z').

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <optional>
#include <string_view>
#include <vector>

namespace {

constexpr std::size_t kAlphabetSize = 26uz;
constexpr std::size_t kNoChild = 0uz;

constexpr std::size_t char_index(char c) {
  return static_cast<std::size_t>(static_cast<unsigned char>(c)) -
         static_cast<std::size_t>(static_cast<unsigned char>('a'));
}

// Flat arena-backed prefix tree: node 0 is always the root, so index 0 doubles
// as the empty-child sentinel (`kNoChild`).
// Time: O(L) per operation where L is word/prefix length.
// Space: O(N * 26) where N is the number of created trie nodes.
class Trie {
 public:
  Trie() : nodes_(1uz) {}

  void insert(std::string_view word) {
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
    const auto node = find_node(word);
    return node.has_value() && nodes_[*node].is_end;
  }

  [[nodiscard]] bool startsWith(std::string_view prefix) const {
    return find_node(prefix).has_value();
  }

 private:
  struct Node {
    std::array<std::size_t, kAlphabetSize> children{};
    bool is_end = false;
  };

  [[nodiscard]] std::optional<std::size_t> find_node(
      std::string_view text) const {
    std::size_t cur = 0uz;
    for (const char c : text) {
      const std::size_t next = nodes_[cur].children[char_index(c)];
      if (next == kNoChild) {
        return std::nullopt;
      }
      cur = next;
    }
    return cur;
  }

  std::vector<Node> nodes_;
};

LC_PROBLEM(208, "Implement Trie Prefix Tree", "trie", ctx) {
  ctx.test("leetcode_example", [&] {
    Trie trie;
    trie.insert("apple");
    ctx.expect_true(trie.search("apple"));
    ctx.expect_false(trie.search("app"));
    ctx.expect_true(trie.startsWith("app"));
    trie.insert("app");
    ctx.expect_true(trie.search("app"));
  });

  ctx.test("python_build_words", [&] {
    Trie trie;
    trie.insert("hello");
    trie.insert("world");
    ctx.expect_false(trie.search("a"));
    ctx.expect_false(trie.startsWith("a"));
    ctx.expect_true(trie.search("hello"));
    ctx.expect_true(trie.startsWith("hel"));
    ctx.expect_false(trie.search("hel"));
    ctx.expect_true(trie.search("world"));
    ctx.expect_false(trie.startsWith("worlds"));
  });

  ctx.test("single_char_and_duplicate_insert", [&] {
    Trie trie;
    ctx.expect_false(trie.search("a"));
    ctx.expect_false(trie.startsWith("a"));

    trie.insert("a");
    trie.insert("a");
    ctx.expect_true(trie.search("a"));
    ctx.expect_true(trie.startsWith("a"));
    ctx.expect_false(trie.search("ab"));
    ctx.expect_false(trie.startsWith("ab"));
  });
}

}  // namespace
