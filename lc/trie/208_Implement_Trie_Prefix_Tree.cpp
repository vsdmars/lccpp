// LeetCode 208: Implement Trie (Prefix Tree)
// https://leetcode.com/problems/implement-trie-prefix-tree/
//
// Problem:
//   Implement a Trie (prefix tree) supporting three operations over lowercase
//   English strings ('a'-'z'):
//     - insert(word): Inserts the string `word` into the trie.
//     - search(word): Returns true if `word` was previously inserted as a
//       complete word, and false otherwise.
//     - startsWith(prefix): Returns true if any inserted word begins with
//       `prefix`, and false otherwise.
//   Typical constraints: 1 <= word.length, prefix.length <= 2000; up to
//   3 * 10^4 total calls across all three methods.
//
// Examples:
//   Example 1:
//     Input:  operations = ["Trie", "insert", "search", "search", "startsWith",
//             "insert", "search"], args = [[], ["apple"], ["apple"], ["app"],
//             ["app"], ["app"], ["app"]]
//     Output: [null, null, true, false, true, null, true]
//     Explanation:
//       - Trie() allocates root node 0 in nodes_, and insert("apple") creates nodes
//         1 through 5 along 'a'->'p'->'p'->'l'->'e' with nodes_[5].is_end = true.
//       - search("apple") follows nodes 0 -> 1 -> 2 -> 3 -> 4 -> 5 via find_node
//         and returns true because nodes_[5].is_end is true.
//       - search("app") reaches node 3 where is_end is false and returns false,
//         whereas startsWith("app") returns true because find_node("app") succeeds.
//       - insert("app") traverses existing nodes 0 -> 1 -> 2 -> 3 and sets
//         nodes_[3].is_end = true, making the final search("app") return true.
//
//   Example 2:
//     Input:  operations = ["Trie", "insert", "insert", "search", "startsWith",
//             "search", "startsWith", "search", "search", "startsWith"], args =
//             [[], ["hello"], ["world"], ["a"], ["a"], ["hello"], ["hel"], ["hel"],
//             ["world"], ["worlds"]]
//     Output: [null, null, null, false, false, true, true, false, true, false]
//     Explanation:
//       - Inserting "hello" and "world" builds two separate branches under root 0
//         at children['h' - 'a'] and children['w' - 'a'], marking the final 'o' and
//         'd' nodes with is_end = true.
//       - search("a") and startsWith("a") find nodes_[0].children[0] == kNoChild
//         and return false.
//       - search("hello") reaches the terminal 'o' node and returns true, while
//         startsWith("hel") succeeds at the first 'l' node (true) and search("hel")
//         returns false because is_end is false there.
//       - search("world") returns true at the terminal 'd' node, whereas
//         startsWith("worlds") hits kNoChild on 's' from that node and returns
//         false.
//
//   Example 3:
//     Input:  operations = ["Trie", "search", "startsWith", "insert", "insert",
//             "search", "startsWith", "search", "startsWith"], args = [[], ["a"],
//             ["a"], ["a"], ["a"], ["a"], ["a"], ["ab"], ["ab"]]
//     Output: [null, false, false, null, null, true, true, false, false]
//     Explanation:
//       - Initially root 0 has all child slots set to kNoChild, so search("a") and
//         startsWith("a") return false.
//       - The first insert("a") allocates node 1 at nodes_[0].children[0] with
//         is_end = true, and the second insert("a") traverses the same slot
//         idempotently.
//       - search("a") and startsWith("a") both locate node 1 and return true.
//       - search("ab") and startsWith("ab") reach node 1 on 'a' and then encounter
//         nodes_[1].children[1] == kNoChild for 'b', returning false.
//
// Algorithm:
//   Store the trie in a contiguous flat arena (`std::vector<Node>`) where node
//   index 0 is always the root. Because no valid edge ever points back to the
//   root, index 0 (`kNoChild`) doubles as the missing-child sentinel.
//   - insert(word): Walk from root (0), mapping each character `c` to `c - 'a'`.
//     Whenever a child slot is `kNoChild`, append a new default-initialized
//     Node to `nodes_` and record its index. Mark the final node `is_end = true`.
//   - find_node(text): Follow character transitions from node 0, returning
//     `std::nullopt` if any transition hits `kNoChild`, or the final node index.
//   - search(word): Succeeds iff `find_node(word)` returns a node with `is_end`.
//   - startsWith(prefix): Succeeds iff `find_node(prefix)` returns any node.
//
//   Complexity:
//     - Time: O(L) per operation, where L is the length of `word` or `prefix`.
//     - Space: O(N * 26) where N is the total number of allocated trie nodes
//       (at most the sum of inserted word lengths plus one root).
//
// Data Structures:
//   - `std::vector<Node> nodes_`: Contiguous arena storing all trie nodes by
//     integer index, avoiding per-node heap allocations and pointer chasing.
//   - `Node`: Contains a fixed `std::array<std::size_t, 26> children` table of
//     child indices (0 meaning absent) and a `bool is_end` word-terminal flag.
//   - `std::optional<std::size_t>`: Return type of `find_node` to distinguish a
//     failed lookup from a valid node index cleanly.
//
// Real-World Applications:
//   - Prefix-based autocomplete and command-line tab completion.
//   - Spell-checker dictionaries and T9 / mobile predictive keyboards.
//   - Longest-prefix IP routing tables (CIDR / radix tries) and URL routers.
//
// Related LeetCode Problems:
//   - #211 Design Add and Search Words Data Structure [done] (adds '.' wildcard DFS)
//   - #1804 Implement Trie II [new] (Prefix Tree) (adds word/prefix counts & erase)
//   - #648 Replace Words [new] (finds shortest dictionary root prefix for each word)
//   - #677 Map Sum Pairs [new] (stores weights in a trie to sum values by prefix)
//   - #720 Longest Word in Dictionary [done] (uses trie traversal to validate prefixes)

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
