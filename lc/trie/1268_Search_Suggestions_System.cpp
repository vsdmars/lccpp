// https://leetcode.com/problems/search-suggestions-system/description/
//
// Given an array of strings `products` and a string `searchWord`, design a
// system that suggests at most three product names from `products` after each
// character of `searchWord` is typed. Suggested products must share a common
// prefix with `searchWord`, and if more than three match, return the three
// lexicographically smallest products.

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
