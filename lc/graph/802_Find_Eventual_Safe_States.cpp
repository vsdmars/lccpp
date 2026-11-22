// https://leetcode.com/problems/find-eventual-safe-states/description/
//
// In a directed graph, a node is *eventually safe* if every possible walk
// starting from it leads to a terminal node (a node with no outgoing edges),
// i.e. no path from it can ever enter a cycle. Return all eventually safe nodes
// in ascending order.

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Classifies every node as safe or unsafe via memoized 3-state DFS.
  // A node is safe iff every outgoing neighbor is safe (and none is currently
  // on the active recursion stack).
  // Time: O(V + E), Space: O(V).
  std::vector<int> eventualSafeNodes(
      const std::vector<std::vector<int>>& graph) {
    const std::size_t n = graph.size();
    enum class State : std::uint8_t { kUnvisited, kVisiting, kSafe, kUnsafe };
    std::vector<State> state(n, State::kUnvisited);

    auto is_safe = [&](this auto&& self, std::size_t u) -> bool {
      if (state[u] == State::kVisiting || state[u] == State::kUnsafe) {
        return false;
      }
      if (state[u] == State::kSafe) {
        return true;
      }

      state[u] = State::kVisiting;
      for (const int v : graph[u]) {
        if (!self(static_cast<std::size_t>(v))) {
          state[u] = State::kUnsafe;
          return false;
        }
      }
      state[u] = State::kSafe;
      return true;
    };

    std::vector<int> safe_nodes;
    safe_nodes.reserve(n);
    for (std::size_t i = 0uz; i < n; ++i) {
      if (is_safe(i)) {
        safe_nodes.push_back(static_cast<int>(i));
      }
    }
    return safe_nodes;
  }
};

LC_PROBLEM(802, "Find Eventual Safe States", "graph", ctx) {
  Solution sol;

  ctx.test("example 1: mixed safe and cyclic nodes", [&] {
    ctx.expect_eq(
        sol.eventualSafeNodes({{1, 2}, {2, 3}, {5}, {0}, {5}, {}, {}}),
        std::vector<int>{2, 4, 5, 6});
  });

  ctx.test("self-loop at node 0 poisons ancestors", [&] {
    ctx.expect_eq(sol.eventualSafeNodes({{0}, {2, 3, 4}, {3, 4}, {0, 4}, {}}),
                  std::vector<int>{4});
  });

  ctx.test("all terminal nodes are safe", [&] {
    ctx.expect_eq(sol.eventualSafeNodes({{}, {}, {}}),
                  std::vector<int>{0, 1, 2});
  });
}

}  // namespace
