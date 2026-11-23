// LeetCode 642: Design Search Autocomplete System
// https://leetcode.com/problems/design-search-autocomplete-system/
//
// Problem:
//   Design a search autocomplete system initialized with historical `sentences`
//   and their corresponding frequency (`times`) counts. Characters consist of
//   lowercase English letters ('a'-'z') and spaces (' '). Each call to
//   `input(char c)` represents the user typing one character:
//     - If `c == '#'`, the current typed sentence ends: save it to history
//       (incrementing its hotness count by 1), clear the active input state,
//       and return an empty list.
//     - Otherwise, append `c` to the current prefix and return up to the top 3
//       historical sentences starting with that prefix, ranked by hotness
//       descending and then ASCII lexicographical order ascending.
//
// Algorithm:
//   Arena-Backed Trie with Stateful Incremental Cursor + Subtree Top-K Sort:
//   - Map `'a'-'z'` to indices `0..25` and `' '` to index `26` (`kSpaceIndex`).
//   - In the constructor, insert each `(sentence, times[i])` into the trie via
//     `add_sentence`, storing the full `sentence` string and cumulative `hot`
//     count at its terminal node.
//   - Maintain an incremental cursor `cur_node_` (starting at root 0) and the
//     accumulated buffer `current_input_`.
//   - On `input('#')`, record `current_input_` with hotness `+1`, clear
//     `current_input_`, reset `cur_node_` to root 0, and return `{}`.
//   - On a regular character `c`, advance `cur_node_` by one edge in O(1). If
//     no edge exists, set `cur_node_ = std::nullopt` so subsequent characters
//     in the same query return `{}` immediately without traversing the trie.
//   - Otherwise, DFS (`collect`) the subtree rooted at `*cur_node_` to gather
//     all nodes with `hot > 0`, and use `std::ranges::partial_sort` to select
//     the top `min(3, M)` candidates by `(hot desc, sentence asc)`.
//
//   Complexity:
//     - Constructor: Time O(S), Space O(S * 27), where S is the total number of
//       characters across all initial sentences.
//     - input(c): Time O(T + M log K) for subtree size T, M matching sentences,
//       and K = 3 (O(L) on '#' to insert the finished sentence of length L).
//     - Space: O(S_total * 27) across all recorded sentences plus O(M) per query.
//
// Data Structures:
//   - `std::vector<Node> nodes_`: Flat trie arena where each `Node` holds a
//     27-slot `children` array, terminal `sentence`, and cumulative `hot` count.
//   - `std::optional<std::size_t> cur_node_`: Incremental trie cursor avoiding
//     re-walking the prefix on each keystroke (nullopt on dead-end prefix).
//   - `std::string current_input_`: Accumulates characters of the in-progress
//     query until `'#'` commits it.
//   - `std::vector<Candidate>`: Lightweight `(hot, std::string_view)` list used
//     with `std::ranges::partial_sort` to rank top-3 matches without copying
//     non-winning strings.
//
// Real-World Applications:
//   - Search engine query typeahead (Google, Bing, e-commerce search bars).
//   - IDE symbol/command autocomplete ranked by usage frequency.
//   - Address, contact, and ticker-symbol lookup widgets with live telemetry.
//
// Related LeetCode Problems:
//   - #208 Implement Trie (Prefix Tree) (base prefix tree without ranking)
//   - #1268 Search Suggestions System (top 3 lexicographical prefix suggestions)
//   - #677 Map Sum Pairs (aggregates numeric scores over a trie prefix subtree)
//   - #1804 Implement Trie II (Prefix Tree) (tracks per-word frequencies in trie)
//   - #211 Design Add and Search Words Data Structure (custom trie query design)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace {

constexpr std::size_t kAlphabetSize = 27uz;  // 'a'-'z' plus ' '
constexpr std::size_t kSpaceIndex = 26uz;
constexpr std::size_t kNoChild = 0uz;
constexpr std::size_t kTopK = 3uz;

constexpr std::size_t char_index(char c) {
  if (c == ' ') {
    return kSpaceIndex;
  }
  return static_cast<std::size_t>(static_cast<unsigned char>(c)) -
         static_cast<std::size_t>(static_cast<unsigned char>('a'));
}

// Trie-based AutocompleteSystem tracking an incremental cursor (`cur_node_`).
// Constructor: Time O(sum of sentence lengths), Space O(sum of lengths * 27).
// input(c):    Time O(subtree size + M log M) where M is matching sentences.
class AutocompleteSystem {
 public:
  AutocompleteSystem(const std::vector<std::string>& sentences,
                     const std::vector<int>& times)
      : nodes_(1uz), cur_node_(0uz) {
    for (std::size_t i = 0uz; i < sentences.size(); ++i) {
      add_sentence(sentences[i], times[i]);
    }
  }

  std::vector<std::string> input(char c) {
    if (c == '#') {
      if (!current_input_.empty()) {
        add_sentence(current_input_, 1);
      }
      current_input_.clear();
      cur_node_ = 0uz;
      return {};
    }

    current_input_.push_back(c);
    if (!cur_node_.has_value()) {
      return {};
    }

    const std::size_t next = nodes_[*cur_node_].children[char_index(c)];
    if (next == kNoChild) {
      cur_node_ = std::nullopt;
      return {};
    }
    cur_node_ = next;

    std::vector<Candidate> candidates;
    collect(next, candidates);

    const std::size_t take = std::min(kTopK, candidates.size());
    std::ranges::partial_sort(
        candidates, candidates.begin() + static_cast<std::ptrdiff_t>(take),
        [](const Candidate& a, const Candidate& b) {
          if (a.hot != b.hot) {
            return a.hot > b.hot;
          }
          return a.sentence < b.sentence;
        });

    std::vector<std::string> top;
    top.reserve(take);
    for (std::size_t i = 0uz; i < take; ++i) {
      top.emplace_back(candidates[i].sentence);
    }
    return top;
  }

 private:
  struct Node {
    std::array<std::size_t, kAlphabetSize> children{};
    std::string sentence;
    int hot = 0;
  };

  struct Candidate {
    int hot = 0;
    std::string_view sentence;
  };

  void add_sentence(std::string_view sentence, int hot) {
    std::size_t cur = 0uz;
    for (const char c : sentence) {
      const std::size_t idx = char_index(c);
      if (nodes_[cur].children[idx] == kNoChild) {
        const std::size_t next_id = nodes_.size();
        nodes_[cur].children[idx] = next_id;
        nodes_.emplace_back();
      }
      cur = nodes_[cur].children[idx];
    }
    if (nodes_[cur].sentence.empty()) {
      nodes_[cur].sentence = std::string(sentence);
    }
    nodes_[cur].hot += hot;
  }

  void collect(std::size_t node_idx, std::vector<Candidate>& out) const {
    const Node& node = nodes_[node_idx];
    if (node.hot > 0) {
      out.push_back(Candidate{.hot = node.hot, .sentence = node.sentence});
    }
    for (const std::size_t child : node.children) {
      if (child != kNoChild) {
        collect(child, out);
      }
    }
  }

  std::vector<Node> nodes_;
  std::optional<std::size_t> cur_node_;
  std::string current_input_;
};

LC_PROBLEM(642, "Design Search Autocomplete System", "trie", ctx) {
  ctx.test("leetcode_example_and_hotness_updates", [&] {
    AutocompleteSystem sys(
        {"i love you", "island", "ironman", "i love leetcode"}, {5, 3, 2, 2});

    // First query: "i a#"
    ctx.expect_eq(
        sys.input('i'),
        std::vector<std::string>{"i love you", "island", "i love leetcode"});
    ctx.expect_eq(sys.input(' '),
                  std::vector<std::string>{"i love you", "i love leetcode"});
    ctx.expect_true(sys.input('a').empty());
    ctx.expect_true(sys.input('#').empty());

    // Second query: "i a#" is now recorded with hotness 1.
    ctx.expect_eq(
        sys.input('i'),
        std::vector<std::string>{"i love you", "island", "i love leetcode"});
    ctx.expect_eq(
        sys.input(' '),
        std::vector<std::string>{"i love you", "i love leetcode", "i a"});
    ctx.expect_eq(sys.input('a'), std::vector<std::string>{"i a"});
    ctx.expect_true(sys.input('#').empty());

    // Third query: "i a" now has hotness 2 and sorts before "i love leetcode".
    ctx.expect_eq(sys.input('i'),
                  std::vector<std::string>{"i love you", "island", "i a"});
    ctx.expect_eq(
        sys.input(' '),
        std::vector<std::string>{"i love you", "i a", "i love leetcode"});
    ctx.expect_true(sys.input('#').empty());
  });

  ctx.test("prefix_sentence_does_not_hide_longer_children", [&] {
    AutocompleteSystem sys({"a", "ab", "abc", "abd"}, {1, 2, 3, 2});
    ctx.expect_eq(sys.input('a'), std::vector<std::string>{"abc", "ab", "abd"});
    ctx.expect_eq(sys.input('b'), std::vector<std::string>{"abc", "ab", "abd"});
    ctx.expect_eq(sys.input('c'), std::vector<std::string>{"abc"});
    ctx.expect_true(sys.input('#').empty());
  });

  ctx.test("empty_initial_history", [&] {
    AutocompleteSystem sys({}, {});
    ctx.expect_true(sys.input('h').empty());
    ctx.expect_true(sys.input('i').empty());
    ctx.expect_true(sys.input('#').empty());
    ctx.expect_eq(sys.input('h'), std::vector<std::string>{"hi"});
    ctx.expect_true(sys.input('#').empty());
  });
}

}  // namespace
