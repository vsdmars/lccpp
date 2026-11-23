// LeetCode 1319: Number of Operations to Make Network Connected
// https://leetcode.com/problems/number-of-operations-to-make-network-connected/
//
// Problem:
//   Given `n` computers labeled `0..n-1` and an array of undirected ethernet
//   cables `connections[i] = [a, b]`, you may unplug any existing cable and
//   reconnect it between any pair of disconnected computers. Return the minimum
//   number of cable relocations needed to connect all `n` computers into one
//   network, or `-1` if impossible. Constraints: 1 <= n <= 10^5,
//   1 <= connections.length <= min(n * (n - 1) / 2, 10^5), with no self-loops
//   or duplicate cables.
//
// Examples:
//   Example 1:
//     Input:  n = 4, connections = [[0, 1], [0, 2], [1, 2]]
//     Output: 1
//     Explanation:
//       - Since connections.size() is 3 >= n - 1 = 3, enough cables exist to
//         connect all 4 computers.
//       - DFS from node 0 marks {0, 1, 2} as visited and counts component 1.
//       - Node 3 remains unvisited and isolated, so DFS from node 3 increments
//         components to 2, and the solver returns components - 1 = 1.
//
//   Example 2:
//     Input:  n = 6, connections = [[0, 1], [0, 2], [0, 3], [1, 2], [1, 3]]
//     Output: 2
//     Explanation:
//       - Since connections.size() is 5 >= n - 1 = 5, rewiring redundant cables can
//         unify the network.
//       - DFS from node 0 traverses all edges among {0, 1, 2, 3}, counting
//         component 1.
//       - Isolated nodes 4 and 5 each trigger their own DFS pass, raising
//         components to 3 and returning components - 1 = 2.
//
//   Example 3:
//     Input:  n = 6, connections = [[0, 1], [0, 2], [0, 3], [1, 2]]
//     Output: -1
//     Explanation:
//       - With n = 6, spanning all computers requires at least n - 1 = 5 cables.
//       - Because connections.size() is 4 < 5, the early sufficiency check
//         immediately returns -1.
//
// Algorithm:
//   Edge-sufficiency check followed by connected-component counting via DFS.
//   Key invariant: spanning `n` vertices requires at least `n - 1` edges, so
//   fewer than `n - 1` cables immediately returns `-1`. When at least `n - 1`
//   cables exist and the graph splits into `C` connected components, those
//   components span `n - C` tree edges and therefore hold at least `C - 1`
//   redundant cycle edges that can be relocated to bridge all `C` components in
//   exactly `C - 1` operations. The solution builds an undirected adjacency
//   list, runs DFS from each unvisited vertex to count `components`, and
//   returns `components - 1`.
//   Time Complexity: O(V + E), where V = n and E = connections.size().
//   Space Complexity: O(V + E) for the adjacency list, visited flags, and DFS
//     recursion stack.
//
// Data Structures:
//   - `std::vector<std::vector<int>> adj`: Undirected adjacency list storing
//     cable neighbors for each computer `0..n-1`.
//   - `std::vector<std::uint8_t> visited`: Compact byte-flag array tracking
//     already-explored vertices across DFS traversals.
//   - Recursive call stack (`dfs` lambda): Explores all vertices belonging to
//     the current connected component.
//
// Real-World Applications:
//   - Data center and telecom topology recovery: computing the minimum link
//     rewirings needed to heal partitioned clusters into one connected fabric.
//   - Power grid and utility restoration planning when re-routing redundant
//     transmission lines to reconnect isolated substations.
//   - Distributed mesh-network maintenance to unify split-brain partitions with
//     minimal edge reassignments.
//
// Related LeetCode Problems:
//   - #547 Number of Provinces [done] (counting connected components in a graph)
//   - #323 Number of Connected Components in an Undirected Graph [done] (direct
//     connected-component counting via DFS or Union-Find)
//   - #684 Redundant Connection [done] (identifying redundant cycle edges in a graph)
//   - #261 Graph Valid Tree [done] (checking n - 1 edges and single-component status)
//   - #2316 Count Unreachable Pairs of Nodes in an Undirected Graph [new] (analyzing
//     component structure in disconnected undirected graphs)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Verifies edge sufficiency and counts connected components via DFS.
  // Time: O(V + E), Space: O(V + E).
  int makeConnected(int n, const std::vector<std::vector<int>>& connections) {
    if (n <= 1) {
      return 0;
    }
    if (connections.size() < static_cast<std::size_t>(n - 1)) {
      return -1;
    }

    const auto count = static_cast<std::size_t>(n);
    std::vector<std::vector<int>> adj(count);
    for (const auto& edge : connections) {
      adj[static_cast<std::size_t>(edge[0])].push_back(edge[1]);
      adj[static_cast<std::size_t>(edge[1])].push_back(edge[0]);
    }

    std::vector<std::uint8_t> visited(count, 0);
    auto dfs = [&](this auto&& self, int u) -> void {
      visited[static_cast<std::size_t>(u)] = 1;
      for (const int v : adj[static_cast<std::size_t>(u)]) {
        if (visited[static_cast<std::size_t>(v)] == 0) {
          self(v);
        }
      }
    };

    int components = 0;
    for (int node = 0; node < n; ++node) {
      if (visited[static_cast<std::size_t>(node)] == 0) {
        dfs(node);
        ++components;
      }
    }
    return components - 1;
  }
};

LC_PROBLEM(1319, "Number of Operations to Make Network Connected", "graph",
           ctx) {
  Solution sol;

  ctx.test("example 1: 4 computers, 1 move", [&] {
    ctx.expect_eq(sol.makeConnected(4, {{0, 1}, {0, 2}, {1, 2}}), 1);
  });

  ctx.test("example 2: 6 computers, 2 moves", [&] {
    ctx.expect_eq(
        sol.makeConnected(6, {{0, 1}, {0, 2}, {0, 3}, {1, 2}, {1, 3}}), 2);
  });

  ctx.test("example 3: not enough cables", [&] {
    ctx.expect_eq(sol.makeConnected(6, {{0, 1}, {0, 2}, {0, 3}, {1, 2}}), -1);
  });

  ctx.test("python sample: 5 computers with 1 isolated node", [&] {
    ctx.expect_eq(sol.makeConnected(5, {{0, 1}, {0, 2}, {0, 3}, {1, 2}}), 1);
  });
}

}  // namespace
