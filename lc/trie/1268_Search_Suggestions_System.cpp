// LeetCode 1268: Search Suggestions System
// https://leetcode.com/problems/search-suggestions-system/
//
// Problem:
//   Given an array of unique lowercase English strings `products`
//   (1 <= products.length <= 1000, sum of lengths <= 2 * 10^4) and a string
//   `searchWord` (1 <= searchWord.length <= 1000), return a list of suggestion
//   lists after each character of `searchWord` is typed. Each list must
//   contain up to 3 products sharing the current typed prefix; if more than 3
//   match, return the 3 lexicographically smallest products.
//
// Examples:
//   Example 1:
//     Input:  products = ["mobile", "mouse", "moneypot", "monitor", "mousepad"],
//             searchWord = "mouse"
//     Output: [["mobile", "moneypot", "monitor"], ["mobile", "moneypot",
//             "monitor"], ["mouse", "mousepad"], ["mouse", "mousepad"], ["mouse",
//             "mousepad"]]
//     Explanation:
//       - Both approaches sort products lexicographically to ["mobile", "moneypot",
//         "monitor", "mouse", "mousepad"].
//       - For prefixes "m" and "mo", lower_bound starts at "mobile" and collects
//         the first 3 matching strings ["mobile", "moneypot", "monitor"].
//       - At prefix "mou", lower_bound advances past "monitor" to "mouse" and
//         collects up to 3 matches before products.end(), yielding ["mouse",
//         "mousepad"].
//       - For prefixes "mous" and "mouse", lower_bound stays on "mouse" and both
//         entries still start with the prefix, repeating ["mouse", "mousepad"].
//
//   Example 2:
//     Input:  products = ["bags", "baggage", "banner", "box", "cloths"], searchWord
//             = "bags"
//     Output: [["baggage", "bags", "banner"], ["baggage", "bags", "banner"],
//             ["baggage", "bags"], ["bags"]]
//     Explanation:
//       - Sorting products gives ["baggage", "bags", "banner", "box", "cloths"],
//         and in the trie each visited node caches at most 3 strings in this sorted
//         order.
//       - For prefixes "b" and "ba", the first 3 sorted entries ["baggage", "bags",
//         "banner"] match and hit the kMaxSuggestions = 3 cap before "box".
//       - At prefix "bag", "banner" no longer matches so only ["baggage", "bags"]
//         are returned.
//       - At prefix "bags", lower_bound advances past "baggage" to "bags" (and the
//         trie node for 's' caches only "bags"), returning ["bags"].
//
//   Example 3:
//     Input:  products = ["havana"], searchWord = "tatiana"
//     Output: [[], [], [], [], [], [], []]
//     Explanation:
//       - Sorting leaves products = ["havana"], so the trie only populates the 'h'
//         branch from root 0.
//       - On the first character 't' (prefix "t"), lower_bound advances to
//         products.end() and the trie finds children['t' - 'a'] == 0, setting
//         matched = false.
//       - Every subsequent character of "tatiana" produces an empty vector,
//         returning 7 empty lists.
//
// Algorithm:
//   Implements two approaches:
//     1. `suggestedProducts` (Sort + Narrowing Binary Search): Sorts `products`
//        lexicographically so all strings sharing any prefix form a contiguous
//        range. As each character of `searchWord` extends `prefix`, calls
//        `std::ranges::lower_bound` from the previous iterator `it` (since
//        growing prefixes only advance the lower bound) and collects up to 3
//        consecutive entries matching `starts_with(prefix)`.
//        - Time: O(N * log(N) * L + M * (log(N) + L)), Space: O(1) auxiliary
//          (excluding the sorted copy of `products` and output).
//     2. `suggestedProductsTrie` (Sort + Cached-Suggestions Trie): Sorts
//        `products` first, then inserts each into a flat-pool trie. Because
//        insertions occur in sorted order, appending `product` to each visited
//        node while `suggestions.size() < 3` pre-caches the exact top-3
//        matches at every prefix. Querying `searchWord` steps one edge per
//        character and copies the cached list (or empty lists after a miss).
//        - Time: O(N * log(N) * L + sum(|products[i]|) + M),
//          Space: O(sum(|products[i]|)) for trie nodes and cached suggestions.
//
// Data Structures:
//   - `std::vector<std::string>` and `std::string_view`: Sorted product catalog
//     and prefix views for monotonic binary search and `starts_with` checks.
//   - `std::vector<TrieNode>` (flat arena-allocated Trie): Contiguous node pool
//     where index 0 is the root (and absent-child sentinel); each `TrieNode`
//     stores `std::array<int, 26> children` and `std::vector<std::string>
//     suggestions` capped at 3 entries.
//
// Real-World Applications:
//   - E-commerce search bars, browser omniboxes, and IDE command palettes
//     providing instant type-ahead autocomplete suggestions.
//   - Contact lookup, address autocomplete, and mobile keyboard completion
//     with precomputed top-k prefix caches.
//   - CLI tab completion and API explorer endpoint suggestion engines.
//
// Related LeetCode Problems:
//   - #208 Implement Trie [done] (Prefix Tree) (core prefix tree for prefix lookups)
//   - #642 Design Search Autocomplete System [done] (stateful top-3 autocomplete
//     ranked by historical search frequency)
//   - #211 Design Add and Search Words Data Structure [done] (trie traversal with
//     wildcard character lookup)
//   - #720 Longest Word in Dictionary [done] (lexicographically ordered trie /
//     sorted-string prefix analysis)
//   - #14 Longest Common Prefix [done] (prefix comparison across sorted strings)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

constexpr std::size_t kAlphabetSize = 26uz;
constexpr std::size_t kMaxSuggestions = 3uz;

std::size_t char_to_index(char ch) {
  return static_cast<std::size_t>(static_cast<unsigned char>(ch) -
                                  static_cast<unsigned char>('a'));
}

class Solution {
 public:
  // Approach 1: Sort + narrowing binary search (`lower_bound`).
  // After sorting lexicographically, all products sharing a given prefix form a
  // contiguous range. Because the prefix only grows, the lower-bound iterator
  // moves monotonically forward.
  // Time: O(N * log(N) * L + M * (log(N) + L))
  // Space: O(1) auxiliary (beyond sorting the input copy and output).
  std::vector<std::vector<std::string>> suggestedProducts(
      std::vector<std::string> products, std::string_view search_word) {
    std::ranges::sort(products);
    std::vector<std::vector<std::string>> result;
    result.reserve(search_word.size());

    std::string prefix;
    prefix.reserve(search_word.size());
    auto it = products.begin();

    for (char ch : search_word) {
      prefix.push_back(ch);
      it = std::ranges::lower_bound(it, products.end(), prefix);

      std::vector<std::string> matches;
      for (auto cur = it;
           cur != products.end() && matches.size() < kMaxSuggestions; ++cur) {
        if (!cur->starts_with(prefix)) {
          break;
        }
        matches.push_back(*cur);
      }
      result.push_back(std::move(matches));
    }
    return result;
  }

  // Approach 2: Sort + Trie caching up to 3 lexicographically smallest products
  // at every prefix node. Uses a flat vector pool for cache locality and RAII.
  // Time: O(N * log(N) * L + sum(|products[i]|) + M)
  // Space: O(sum(|products[i]|)) for trie nodes and cached suggestions.
  std::vector<std::vector<std::string>> suggestedProductsTrie(
      std::vector<std::string> products, std::string_view search_word) {
    struct TrieNode {
      // Index 0 denotes an absent child because root is at index 0.
      std::array<int, kAlphabetSize> children{};
      std::vector<std::string> suggestions;
    };

    std::ranges::sort(products);
    std::vector<TrieNode> nodes(1uz);

    for (const std::string& product : products) {
      std::size_t cur = 0uz;
      for (char ch : product) {
        const std::size_t idx = char_to_index(ch);
        int next = nodes[cur].children[idx];
        if (next == 0) {
          next = static_cast<int>(nodes.size());
          nodes[cur].children[idx] = next;
          nodes.emplace_back();
        }
        cur = static_cast<std::size_t>(next);
        if (nodes[cur].suggestions.size() < kMaxSuggestions) {
          nodes[cur].suggestions.push_back(product);
        }
      }
    }

    std::vector<std::vector<std::string>> result;
    result.reserve(search_word.size());
    std::size_t cur = 0uz;
    bool matched = true;

    for (char ch : search_word) {
      if (matched) {
        const int next = nodes[cur].children[char_to_index(ch)];
        if (next == 0) {
          matched = false;
        } else {
          cur = static_cast<std::size_t>(next);
        }
      }
      if (matched) {
        result.push_back(nodes[cur].suggestions);
      } else {
        result.emplace_back();
      }
    }
    return result;
  }
};

LC_PROBLEM(1268, "Search Suggestions System", "trie", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    const std::vector<std::string> products = {
        "mobile", "mouse", "moneypot", "monitor", "mousepad"};
    const std::vector<std::vector<std::string>> expected = {
        {"mobile", "moneypot", "monitor"},
        {"mobile", "moneypot", "monitor"},
        {"mouse", "mousepad"},
        {"mouse", "mousepad"},
        {"mouse", "mousepad"},
    };
    ctx.expect_eq(sol.suggestedProducts(products, "mouse"), expected);
    ctx.expect_eq(sol.suggestedProductsTrie(products, "mouse"), expected);
  });

  ctx.test("no_match", [&] {
    Solution sol;
    const std::vector<std::string> products = {"havana"};
    const std::vector<std::vector<std::string>> expected(7uz);
    ctx.expect_eq(sol.suggestedProducts(products, "tatiana"), expected);
    ctx.expect_eq(sol.suggestedProductsTrie(products, "tatiana"), expected);
  });

  ctx.test("exact_and_prefix_match", [&] {
    Solution sol;
    const std::vector<std::string> products = {"mouse", "mousepad"};
    const std::vector<std::vector<std::string>> expected(
        5uz, {"mouse", "mousepad"});
    ctx.expect_eq(sol.suggestedProducts(products, "mouse"), expected);
    ctx.expect_eq(sol.suggestedProductsTrie(products, "mouse"), expected);
  });

  ctx.test("diverging_prefixes", [&] {
    Solution sol;
    const std::vector<std::string> products = {"bags", "baggage", "banner",
                                               "box", "cloths"};
    const std::vector<std::vector<std::string>> expected = {
        {"baggage", "bags", "banner"},
        {"baggage", "bags", "banner"},
        {"baggage", "bags"},
        {"bags"},
    };
    ctx.expect_eq(sol.suggestedProducts(products, "bags"), expected);
    ctx.expect_eq(sol.suggestedProductsTrie(products, "bags"), expected);
  });

  ctx.test("single_product_full_match", [&] {
    Solution sol;
    const std::vector<std::string> products = {"havana"};
    const std::vector<std::vector<std::string>> expected(6uz, {"havana"});
    ctx.expect_eq(sol.suggestedProducts(products, "havana"), expected);
    ctx.expect_eq(sol.suggestedProductsTrie(products, "havana"), expected);
  });
}

}  // namespace
