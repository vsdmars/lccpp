// https://leetcode.com/problems/smallest-string-with-swaps/
//
// Given a string `s` and pairs of swappable indices `pairs`, you may swap the
// characters at any given pair any number of times. Return the lexicographically
// smallest string achievable.

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <ranges>
#include <string>
#include <vector>

namespace {

class Solution {
 public:
  // Swappability is transitive, so indices within each connected component can
  // be arbitrarily permuted. Collect each component via DFS, sort its indices
  // and characters in ascending order, and place characters back greedily.
  // Time: O(N log N + E), Space: O(N + E).
  std::string smallestStringWithSwaps(
      std::string s, const std::vector<std::vector<int>>& pairs) {
    const std::size_t n = s.size();
    std::vector<std::vector<std::size_t>> adj(n);
    for (const auto& p : pairs) {
      const auto u = static_cast<std::size_t>(p[0]);
      const auto v = static_cast<std::size_t>(p[1]);
      adj[u].push_back(v);
      adj[v].push_back(u);
    }

    std::vector<std::uint8_t> visited(n, 0);
    std::vector<std::size_t> comp_indices;
    std::string comp_chars;

    auto dfs = [&](this auto&& self, std::size_t u) -> void {
      visited[u] = 1;
      comp_indices.push_back(u);
      comp_chars.push_back(s[u]);
      for (const std::size_t v : adj[u]) {
        if (visited[v] == 0) {
          self(v);
        }
      }
    };

    for (std::size_t i = 0uz; i < n; ++i) {
      if (visited[i] != 0 || adj[i].empty()) {
        continue;
      }
      comp_indices.clear();
      comp_chars.clear();
      dfs(i);

      std::ranges::sort(comp_indices);
      std::ranges::sort(comp_chars);
      for (std::size_t k = 0uz; k < comp_indices.size(); ++k) {
        s[comp_indices[k]] = comp_chars[k];
      }
    }

    return s;
  }
};

LC_PROBLEM(1202, "Smallest String With Swaps", "graph", ctx) {
  Solution sol;

  ctx.test("example 1: two disjoint swap pairs", [&] {
    ctx.expect_eq(sol.smallestStringWithSwaps("dcab", {{0, 3}, {1, 2}}),
                  "bacd");
  });

  ctx.test("example 2: single connected component sorts entire string", [&] {
    ctx.expect_eq(sol.smallestStringWithSwaps("dcab", {{0, 3}, {1, 2}, {0, 2}}),
                  "abcd");
  });

  ctx.test("example 3: chain swaps", [&] {
    ctx.expect_eq(sol.smallestStringWithSwaps("cba", {{0, 1}, {1, 2}}), "abc");
  });

  ctx.test("no swap pairs leaves string unchanged", [&] {
    ctx.expect_eq(sol.smallestStringWithSwaps("dcba", {}), "dcba");
  });
}

}  // namespace
