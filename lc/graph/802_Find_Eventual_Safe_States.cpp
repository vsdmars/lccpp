// LeetCode 802: Find Eventual Safe States
// https://leetcode.com/problems/find-eventual-safe-states/
//
// Problem:
//   Given a directed graph of `n` nodes labeled `0..n-1` as an adjacency list
//   `graph`, a node is a *terminal node* if it has no outgoing edges, and a
//   node is *eventually safe* if every possible directed walk starting from it
//   leads to a terminal node (equivalently, no path from it can ever enter a
//   cycle). Return all eventually safe nodes sorted in ascending order.
//
// Algorithm:
//   Memoized DFS with 4-state node classification (`kUnvisited`, `kVisiting`,
//   `kSafe`, `kUnsafe`), refining classic 3-color cycle detection. On entering
//   an unvisited node `u`, mark it `kVisiting` (currently on the active call
//   stack) and recursively test every outgoing neighbor `v`. If any neighbor
//   is `kVisiting` (back-edge closing a cycle) or `kUnsafe`, mark `u` as
//   `kUnsafe` and return `false`. Only if every outgoing neighbor is confirmed
//   `kSafe` is `u` marked `kSafe` and `true` returned. Iterating `i = 0..n-1`
//   collects all safe nodes directly in ascending order without sorting.
//   Time: O(V + E), Space: O(V) for state memoization and recursion stack.
//
// Data Structures:
//   - `const std::vector<std::vector<int>>& graph`: Input directed adjacency
//     list.
//   - `std::vector<State> state`: Per-node memoization table storing
//     `kUnvisited`, `kVisiting`, `kSafe`, or `kUnsafe` (`std::uint8_t` enum).
//   - `std::vector<int> safe_nodes`: Output buffer collecting safe node IDs in
//     ascending index order.
//
// Real-World Applications:
//   - Static analysis and termination checking: identifying program states or
//     CFG basic blocks guaranteed to terminate rather than enter infinite loops.
//   - Distributed workflow and state-machine verification: auditing transitions
//     to guarantee every execution reaches a valid terminal state without
//     livelock.
//   - Spreadsheet and dependency-graph validation: isolating cells unaffected
//     by circular reference cycles.
//
// Related LeetCode Problems:
//   - #207 Course Schedule (3-color DFS cycle detection in a directed graph)
//   - #210 Course Schedule II (topological sorting of acyclic directed graphs)
//   - #2127 Maximum Employees to Be Invited to a Meeting (analyzing directed
//     cycles and acyclic branches feeding into them)
//   - #2360 Longest Cycle in a Graph (detecting and measuring cycles in a
//     directed graph)

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
