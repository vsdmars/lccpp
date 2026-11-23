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
// Examples:
//   Example 1:
//     Input:  graph = [[1, 2], [2, 3], [5], [0], [5], [], []]
//     Output: [2, 4, 5, 6]
//     Explanation:
//       - Start DFS at node 0 (kVisiting) -> 1 -> 2 -> 5; node 5 has no outgoing
//         edges so state[5] and then state[2] become kSafe.
//       - From node 1, recurse to node 3, whose outgoing edge hits node 0 in state
//         kVisiting; mark state[3], state[1], and state[0] as kUnsafe.
//       - In the outer loop i = 0..6, i = 2 hits memoized state[2] == kSafe and
//         appends 2, while i = 3 is skipped as kUnsafe.
//       - Node 4 visits memoized safe node 5 and becomes kSafe, and terminal nodes
//         5 and 6 confirm kSafe, yielding [2, 4, 5, 6].
//
//   Example 2:
//     Input:  graph = [[0], [2, 3, 4], [3, 4], [0, 4], []]
//     Output: [4]
//     Explanation:
//       - At i = 0, mark state[0] = kVisiting and follow self-loop edge 0 -> 0,
//         immediately hitting kVisiting and marking state[0] = kUnsafe.
//       - At i = 1, traverse 1 -> 2 -> 3 -> 0, where memoized kUnsafe at node 0
//         propagates false back to mark nodes 3, 2, and 1 as kUnsafe.
//       - Iterations i = 2 and i = 3 return false immediately from memoized kUnsafe
//         state.
//       - At i = 4, node 4 has no outgoing edges, transitions to kSafe, and returns
//         [4].
//
//   Example 3:
//     Input:  graph = [[], [], []]
//     Output: [0, 1, 2]
//     Explanation:
//       - At i = 0, mark state[0] = kVisiting, find no outgoing neighbors in
//         graph[0], set state[0] = kSafe, and push 0 to safe_nodes.
//       - Repeat for i = 1 and i = 2, marking both kSafe in ascending index order
//         and returning [0, 1, 2].
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
//   - #207 Course Schedule [done] (3-color DFS cycle detection in a directed graph)
//   - #210 Course Schedule II [done] (topological sorting of acyclic directed graphs)
//   - #2127 Maximum Employees to Be Invited to a Meeting [new] (analyzing directed
//     cycles and acyclic branches feeding into them)
//   - #2360 Longest Cycle in a Graph [new] (detecting and measuring cycles in a
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
