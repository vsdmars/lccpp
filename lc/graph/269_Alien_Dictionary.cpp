// https://leetcode.com/problems/alien-dictionary/description/
//
// Given a list of non-empty lowercase words sorted lexicographically according
// to an unknown alien alphabet, derive a valid ordering of all distinct letters
// present in `words`. Return "" if no valid ordering exists.

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <queue>
#include <string>
#include <unordered_set>
#include <vector>

namespace {

constexpr std::size_t kAlphabetSize = 26uz;

class Solution {
 public:
  // Derives character precedence edges from adjacent word pairs and computes a
  // topological ordering via Kahn's BFS algorithm.
  // Time: O(C) where C is the total number of characters across all words.
  // Space: O(1) (bounded by 26 letters and at most 26^2 edges).
  std::string alienOrder(const std::vector<std::string>& words) {
    AlphabetGraph graph;
    if (!build_graph(words, graph)) {
      return "";
    }
    return topo_sort(graph);
  }

 private:
  struct AlphabetGraph {
    std::array<bool, kAlphabetSize> present{};
    std::array<std::array<bool, kAlphabetSize>, kAlphabetSize> adj{};
    std::array<int, kAlphabetSize> indegree{};
    std::size_t unique_chars = 0uz;
  };

  // Extracts all present letters and directed ordering constraints from
  // adjacent word pairs. Returns false if an invalid prefix ordering is found.
  static bool build_graph(const std::vector<std::string>& words,
                          AlphabetGraph& graph) {
    for (const auto& word : words) {
      for (const char c : word) {
        const auto idx = static_cast<std::size_t>(c - 'a');
        if (!graph.present[idx]) {
          graph.present[idx] = true;
          ++graph.unique_chars;
        }
      }
    }

    for (std::size_t i = 0uz; i + 1uz < words.size(); ++i) {
      if (!add_ordering_edge(words[i], words[i + 1uz], graph)) {
        return false;
      }
    }
    return true;
  }

  // Compares two adjacent words and records the first differing character edge.
  static bool add_ordering_edge(const std::string& w1, const std::string& w2,
                                AlphabetGraph& graph) {
    const std::size_t min_len = std::min(w1.size(), w2.size());
    for (std::size_t j = 0uz; j < min_len; ++j) {
      if (w1[j] != w2[j]) {
        const auto u = static_cast<std::size_t>(w1[j] - 'a');
        const auto v = static_cast<std::size_t>(w2[j] - 'a');
        if (!graph.adj[u][v]) {
          graph.adj[u][v] = true;
          ++graph.indegree[v];
        }
        return true;
      }
    }
    // A longer word cannot precede its own prefix in lexicographical order.
    return w1.size() <= w2.size();
  }

  // Kahn's BFS topological sort over the 26-letter graph.
  static std::string topo_sort(AlphabetGraph& graph) {
    std::queue<std::size_t> q;
    for (std::size_t c = 0uz; c < kAlphabetSize; ++c) {
      if (graph.present[c] && graph.indegree[c] == 0) {
        q.push(c);
      }
    }

    std::string order;
    order.reserve(graph.unique_chars);
    while (!q.empty()) {
      const std::size_t u = q.front();
      q.pop();
      order.push_back(static_cast<char>('a' + static_cast<int>(u)));

      for (std::size_t v = 0uz; v < kAlphabetSize; ++v) {
        if (graph.adj[u][v] && --graph.indegree[v] == 0) {
          q.push(v);
        }
      }
    }

    // If not every unique letter was emitted, a cycle prevents a valid order.
    return order.size() == graph.unique_chars ? order : "";
  }
};

// Verifies `order` contains the exact set of letters in `words` and sorts all
// adjacent word pairs lexicographically.
bool is_valid_alien_order(const std::vector<std::string>& words,
                          const std::string& order) {
  std::unordered_set<char> expected_chars;
  for (const auto& w : words) {
    expected_chars.insert(w.begin(), w.end());
  }
  if (order.size() != expected_chars.size()) {
    return false;
  }

  std::array<int, kAlphabetSize> rank{};
  rank.fill(-1);
  for (std::size_t i = 0uz; i < order.size(); ++i) {
    const auto idx = static_cast<std::size_t>(order[i] - 'a');
    if (rank[idx] != -1 || !expected_chars.contains(order[i])) {
      return false;
    }
    rank[idx] = static_cast<int>(i);
  }

  for (std::size_t i = 0uz; i + 1uz < words.size(); ++i) {
    const std::string& a = words[i];
    const std::string& b = words[i + 1uz];
    const std::size_t len = std::min(a.size(), b.size());
    bool diff = false;
    for (std::size_t j = 0uz; j < len; ++j) {
      if (a[j] != b[j]) {
        if (rank[static_cast<std::size_t>(a[j] - 'a')] >=
            rank[static_cast<std::size_t>(b[j] - 'a')]) {
          return false;
        }
        diff = true;
        break;
      }
    }
    if (!diff && a.size() > b.size()) {
      return false;
    }
  }
  return true;
}

LC_PROBLEM(269, "Alien Dictionary", "graph", ctx) {
  Solution sol;

  ctx.test("example 1: unique order wertf", [&] {
    ctx.expect_eq(sol.alienOrder({"wrt", "wrf", "er", "ett", "rftt"}), "wertf");
  });

  ctx.test("example 2: two single-char words", [&] {
    ctx.expect_eq(sol.alienOrder({"z", "x"}), "zx");
  });

  ctx.test("example 3: cycle returns empty", [&] {
    ctx.expect_eq(sol.alienOrder({"z", "x", "z"}), "");
  });

  ctx.test("valid prefix pair from python sample", [&] {
    const std::vector<std::string> words = {"wrt", "wrtkj"};
    const std::string order = sol.alienOrder(words);
    ctx.expect_true(is_valid_alien_order(words, order),
                    "order must contain all letters and preserve word order");
  });

  ctx.test("invalid prefix ordering returns empty", [&] {
    ctx.expect_eq(sol.alienOrder({"abc", "ab"}), "");
  });
}

}  // namespace
