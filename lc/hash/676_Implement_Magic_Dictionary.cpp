// LeetCode 676: Implement Magic Dictionary
// https://leetcode.com/problems/implement-magic-dictionary/
//
// Problem:
//   Design a dictionary data structure supporting two operations:
//     - buildDict(dictionary): Initializes the dictionary from a list of unique
//       lowercase English words.
//     - search(searchWord): Returns true if and only if changing *exactly one*
//       character of searchWord into a different lowercase English letter
//       produces a word present in the dictionary.
//   Constraints: 1 <= dictionary.length <= 100, 1 <= word.length <= 100,
//   lowercase English letters only, at most 100 search calls.
//
// Examples:
//   Example 1:
//     Input:  operations = ["MagicDictionary", "buildDict", "search", "search",
//             "search", "search"], args = [[], [["hello", "leetcode"]], ["hello"],
//             ["hhllo"], ["hell"], ["leetcoded"]]
//     Output: [null, null, false, true, false, false]
//     Explanation:
//       - buildDict inserts "hello" and "leetcode" into nodes_ and sets max_len_ =
//         8.
//       - search("hello") walks h->e->l->l->o trying sibling child substitutions at
//         indices 0..4, finds no 5-letter match, and returns false after consuming
//         0 mismatches.
//       - search("hhllo") advances along 'h' at i = 0, then at i = 1 ('h') tries
//         sibling child 'e' via matchWithSubstitution and exactMatch("llo",
//         child['e']) reaches "hello"'s is_end node to return true.
//       - search("hell") checks substitutions at indices 0..3 (none terminate a
//         4-letter word because "hello" needs 'o') and returns false when the loop
//         ends.
//       - search("leetcoded") has length 9 > max_len_ (8), so search immediately
//         returns false.
//
//   Example 2:
//     Input:  operations = ["MagicDictionary", "buildDict", "search", "search",
//             "search"], args = [[], [["hello", "hallo", "leetcode", "judge"]],
//             ["hello"], ["judgee"], ["zudge"]]
//     Output: [null, null, true, false, true]
//     Explanation:
//       - buildDict inserts "hello", "hallo", "leetcode", and "judge" into the Trie
//         with max_len_ = 8.
//       - search("hello") advances to 'h' at i = 0, then at i = 1 ('e') tries
//         existing sibling child 'a' and matches "hallo" via exactMatch("llo",
//         child['a']), returning true.
//       - search("judgee") walks j->u->d->g->e up to i = 4, then at i = 5 ('e') the
//         node has no children at all, so matchWithSubstitution fails and the
//         missing exact edge returns false.
//       - search("zudge") substitutes 'z' at i = 0 with root child 'j', and
//         exactMatch("udge", child['j']) matches "judge" to return true.
//
//   Example 3:
//     Input:  operations = ["MagicDictionary", "buildDict", "search", "search",
//             "buildDict", "search", "search"], args = [[], [["a", "b"]], ["a"],
//             ["ab"], [["a"]], ["a"], ["z"]]
//     Output: [null, null, true, false, null, false, true]
//     Explanation:
//       - First buildDict({"a", "b"}) creates root children 'a' and 'b' (both
//         is_end = true) with max_len_ = 1.
//       - search("a") substitutes 'a' with sibling child 'b' at i = 0 and returns
//         true, whereas search("ab") has length 2 > max_len_ (1) and returns false.
//       - Second buildDict({"a"}) clears nodes_, inserts only "a", and sets
//         max_len_ = 1.
//       - search("a") finds no sibling child at the root and returns false with 0
//         mismatches, while search("z") tries root child 'a' at i = 0 and returns
//         true.
//
// Algorithm:
//   Insert every dictionary word into an arena-backed Trie and track the
//   maximum word length `max_len_` for O(1) rejection of overlong queries.
//   To answer `search(word)`:
//     Walk `word` character-by-character from the Trie root, maintaining the
//     invariant that every character before index `i` matched the dictionary
//     prefix exactly (0 mismatches consumed so far). At each index `i`:
//       1. For every existing child letter `alt != word[i]` at the current
//          node, spend our single allowed modification here and check whether
//          the remaining suffix `word[i + 1 ..]` matches a complete word via
//          `exactMatch`. If so, return true immediately.
//       2. Advance along the exact edge `word[i]`. If no such edge exists,
//          stop early and return false because no further single-mismatch
//          branch can exist past a broken prefix.
//     If the loop completes without returning true, only a 0-mismatch path (or
//     proper prefix) was traversed, so return false.
//   Time Complexity:
//     - buildDict: O(total characters in dictionary)
//     - search: O(26 * L^2) worst case per query (L = word.length), and much
//       faster in practice due to sparse branching and early prefix exit.
//   Space Complexity: O(total characters in dictionary * 26) for the Trie.
//
// Data Structures:
//   - Arena-allocated Trie (`std::vector<TrieNode>` where each node stores a
//     26-slot child index array `std::array<int, 26>` and an `is_end` flag):
//     provides contiguous, leak-free storage and O(1) child lookup without
//     heap-per-node fragmentation.
//
// Real-World Applications:
//   - Spell checkers and typo-tolerant autocomplete ("did you mean?" for
//     Hamming-distance-1 substitutions in IDE symbol lookup and search bars).
//   - DNA/RNA barcode demultiplexing in bioinformatics where sequencing reads
//     are matched to known index tags allowing a single base substitution.
//   - Command-line and DNS typosquatting detection engines.
//
// Related LeetCode Problems:
//   - #208 Implement Trie [done] (Prefix Tree) (foundational prefix-tree structure)
//   - #211 Design Add and Search Words Data Structure [done] (Trie search with wildcard '.')
//   - #720 Longest Word in Dictionary [done] (Trie traversal over dictionary words)
//   - #1268 Search Suggestions System [done] (prefix-based dictionary retrieval)
//   - #1554 Strings Differ by One Character [done] (detecting Hamming-distance-1 word pairs)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace {

// Trie-based MagicDictionary supporting O(1)-mismatch single-character queries.
class MagicDictionary {
 public:
  // Initializes an empty dictionary containing only the Trie root node.
  MagicDictionary() { nodes_.emplace_back(); }

  // Rebuilds the Trie from `dictionary` and records the longest word length.
  // Time: O(sum of word lengths), Space: O(sum of word lengths * 26).
  void buildDict(const std::vector<std::string>& dictionary) {
    nodes_.clear();
    nodes_.emplace_back();
    max_len_ = 0uz;

    for (const std::string& word : dictionary) {
      max_len_ = std::max(max_len_, word.size());
      int cur = 0;

      for (const char ch : word) {
        const std::size_t idx = letterIndex(ch);
        if (nodes_[static_cast<std::size_t>(cur)].children[idx] == kNoChild) {
          const auto next_id = static_cast<int>(nodes_.size());
          nodes_[static_cast<std::size_t>(cur)].children[idx] = next_id;
          nodes_.emplace_back();
        }
        cur = nodes_[static_cast<std::size_t>(cur)].children[idx];
      }

      // Mark word termination so prefixes alone do not count as complete words.
      nodes_[static_cast<std::size_t>(cur)].is_end = true;
    }
  }

  // Returns true iff modifying exactly one character in `search_word` yields a
  // word stored in the Trie.
  // Time: O(26 * L^2) worst case where L = search_word.size(), Space: O(1).
  [[nodiscard]] bool search(std::string_view search_word) const {
    // Words longer than any dictionary entry can never match by substitution.
    if (search_word.size() > max_len_) {
      return false;
    }

    int cur = 0;
    for (std::size_t i = 0uz; i < search_word.size(); ++i) {
      const std::size_t target = letterIndex(search_word[i]);
      const std::string_view remaining = search_word.substr(i + 1uz);

      // Try substituting search_word[i] with any other existing child letter.
      if (matchWithSubstitution(cur, target, remaining)) {
        return true;
      }

      // Advance along the exact character; if absent, no later single
      // substitution can repair the mismatch at position `i`.
      const int next = nodes_[static_cast<std::size_t>(cur)].children[target];
      if (next == kNoChild) {
        return false;
      }
      cur = next;
    }

    // Reached the end with 0 modifications; strictly 1 change is required.
    return false;
  }

 private:
  static constexpr std::size_t kAlphabetSize = 26uz;
  static constexpr int kNoChild = -1;

  // Single Trie node storing child indices into `nodes_` and end-of-word flag.
  struct TrieNode {
    std::array<int, kAlphabetSize> children{};
    bool is_end = false;

    TrieNode() { children.fill(kNoChild); }
  };

  // Converts a lowercase ASCII letter ('a'..'z') into a 0..25 array index.
  [[nodiscard]] static std::size_t letterIndex(char ch) noexcept {
    return static_cast<std::size_t>(static_cast<unsigned char>(ch) -
                                    static_cast<unsigned char>('a'));
  }

  // Checks if `suffix` matches an exact complete word starting from `node_idx`.
  [[nodiscard]] bool exactMatch(std::string_view suffix, int node_idx) const {
    int cur = node_idx;
    for (const char ch : suffix) {
      const int next =
          nodes_[static_cast<std::size_t>(cur)].children[letterIndex(ch)];
      if (next == kNoChild) {
        return false;
      }
      cur = next;
    }
    return nodes_[static_cast<std::size_t>(cur)].is_end;
  }

  // Tries every child of `node_idx` other than `skip_letter` as the single
  // substituted character and checks if `remaining` matches verbatim.
  [[nodiscard]] bool matchWithSubstitution(int node_idx,
                                           std::size_t skip_letter,
                                           std::string_view remaining) const {
    const auto& children = nodes_[static_cast<std::size_t>(node_idx)].children;
    for (std::size_t alt = 0uz; alt < kAlphabetSize; ++alt) {
      if (alt != skip_letter && children[alt] != kNoChild &&
          exactMatch(remaining, children[alt])) {
        return true;
      }
    }
    return false;
  }

  std::vector<TrieNode> nodes_;
  std::size_t max_len_ = 0uz;
};

LC_PROBLEM(676, "Implement Magic Dictionary", "hash", ctx) {
  ctx.test("leetcode_example_1", [&] {
    MagicDictionary dict;
    dict.buildDict({"hello", "leetcode"});
    ctx.expect_false(dict.search("hello"));
    ctx.expect_true(dict.search("hhllo"));
    ctx.expect_false(dict.search("hell"));
    ctx.expect_false(dict.search("leetcoded"));
  });

  ctx.test("overlapping_words_allow_exact_word_as_query", [&] {
    MagicDictionary dict;
    dict.buildDict({"hello", "hallo", "leetcode", "judge"});
    // "hello" can become "hallo" via 1 change (e -> a).
    ctx.expect_true(dict.search("hello"));
    ctx.expect_true(dict.search("hallo"));
    // "judgee" is longer than "judge" and matches nothing of length 6.
    ctx.expect_false(dict.search("judgee"));
    // First and last character substitutions.
    ctx.expect_true(dict.search("zudge"));
    ctx.expect_true(dict.search("judga"));
  });

  ctx.test("single_char_words", [&] {
    MagicDictionary dict;
    dict.buildDict({"a", "b"});
    ctx.expect_true(dict.search("a"));
    ctx.expect_true(dict.search("c"));
    ctx.expect_false(dict.search("ab"));

    // Rebuild dictionary with a single 1-char word: querying itself must fail.
    dict.buildDict({"a"});
    ctx.expect_false(dict.search("a"));
    ctx.expect_true(dict.search("z"));
  });
}

}  // namespace
