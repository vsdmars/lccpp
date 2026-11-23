// LeetCode 1743: Restore The Array From Adjacent Pairs
// https://leetcode.com/problems/restore-the-array-from-adjacent-pairs/
//
// Problem:
//   An integer array `nums` of length n (2 <= n <= 10^5) contains n unique
//   elements in [-10^5, 10^5]. You are given `adjacentPairs` of length n - 1,
//   where each pair `[u, v]` indicates that `u` and `v` are adjacent in `nums`
//   (in either order). Reconstruct and return any valid original array `nums`.
//
// Examples:
//   Example 1:
//     Input:  adjacentPairs = [[2, 1], [3, 4], [3, 2]]
//     Output: [1, 2, 3, 4]
//     Explanation:
//       - Building the adjacency map produces {1: [2], 2: [1, 3], 3: [4, 2], 4:
//         [3]}.
//       - Scanning adj for degree-1 vertices finds 1 and 4 and picks the smaller
//         endpoint start = 1.
//       - result starts with [1, 2] where prev = 1 and curr = adj[1][0] = 2.
//       - Looping while result.size() < 4 picks next = 3 from adj[2] = [1, 3]
//         (since 1 == prev) and then next = 4 from adj[3] = [4, 2], returning [1,
//         2, 3, 4].
//
//   Example 2:
//     Input:  adjacentPairs = [[4, -2], [1, 4], [-3, 1]]
//     Output: [-3, 1, 4, -2]
//     Explanation:
//       - Building adj produces {-3: [1], 1: [4, -3], 4: [-2, 1], -2: [4]}.
//       - Between degree-1 endpoints -3 and -2, std::min selects start = -3, and
//         result begins with [-3, 1].
//       - At curr = 1 (prev = -3), adj[1] = [4, -3] yields next = 4; at curr = 4
//         (prev = 1), adj[4] = [-2, 1] yields next = -2, returning [-3, 1, 4, -2].
//
//   Example 3:
//     Input:  adjacentPairs = [[100000, -100000]]
//     Output: [-100000, 100000]
//     Explanation:
//       - Building adj creates two degree-1 nodes: 100000 -> [-100000] and -100000
//         -> [100000].
//       - Selecting the smaller endpoint sets start = -100000 and appends its
//         neighbor curr = 100000 so result = [-100000, 100000].
//       - Because result.size() already equals n = 2, the while loop does not
//         execute and [-100000, 100000] is returned.
//
// Algorithm:
//   Model the adjacent pairs as an undirected graph. Because all n values are
//   unique and pairs form a single simple path of n vertices, the graph is a
//   path graph: exactly two vertices (the two ends of `nums`) have degree 1,
//   and all n - 2 interior vertices have degree 2.
//   1. Build an adjacency list mapping each value to its 1 or 2 neighbors.
//   2. Locate the degree-1 endpoints and deterministically start from the
//      smaller endpoint.
//   3. Walk the path iteratively from `start`: at every step, pick the unique
//      neighbor that differs from `prev` (avoiding both recursion-stack
//      overflow for n = 10^5 and extra visited-set lookups).
//
//   Time Complexity:  O(n) expected time to build the graph and traverse it.
//   Space Complexity: O(n) for the adjacency map and output array.
//
// Data Structures:
//   - `std::unordered_map<int, std::vector<int>>` adjacency list storing up to
//     two neighbors per distinct array value.
//   - `std::vector<int>` storing the reconstructed sequence in traversal order.
//
// Real-World Applications:
//   - Bioinformatics / genome assembly: reconstructing a linear DNA contig
//     from unordered overlapping read-pair adjacencies (simplified Eulerian /
//     Hamiltonian path reconstruction).
//   - Network topology discovery: recovering the linear order of hops or
//     daisy-chained switches from unordered neighbor-discovery (LLDP) reports.
//   - Distributed log / event reconstruction: ordering a doubly-linked chain
//     of records from unordered predecessor/successor edge pairs.
//
// Related LeetCode Problems:
//   - #332 Reconstruct Itinerary [done] (directed path reconstruction via Eulerian DFS)
//   - #2097 Valid Arrangement of Pairs [new] (directed pair chaining via Hierholzer's algorithm)
//   - #261 Graph Valid Tree [done] (undirected graph degree/connectivity analysis)
//   - #797 All Paths From Source to Target [done] (graph path traversal via DFS)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <set>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Reconstructs a valid original array from its unordered adjacent pairs.
  //
  // Complexity: O(n) expected time, O(n) space.
  std::vector<int> restoreArray(
      const std::vector<std::vector<int>>& adjacentPairs) {
    if (adjacentPairs.empty()) {
      return {};
    }

    const std::size_t n = adjacentPairs.size() + 1uz;

    // Build undirected adjacency list; every vertex has degree 1 (endpoints) or
    // degree 2 (interior elements).
    std::unordered_map<int, std::vector<int>> adj;
    adj.reserve(n);
    for (const auto& pair : adjacentPairs) {
      adj[pair[0]].push_back(pair[1]);
      adj[pair[1]].push_back(pair[0]);
    }

    // Choose the smaller degree-1 endpoint so the output is deterministic.
    int start = std::numeric_limits<int>::max();
    for (const auto& [node, neighbors] : adj) {
      if (neighbors.size() == 1uz) {
        start = std::min(start, node);
      }
    }

    // Iteratively follow the unique path from `start` without recursion so
    // inputs up to n = 10^5 never risk stack overflow.
    std::vector<int> result;
    result.reserve(n);
    result.push_back(start);

    int prev = start;
    int curr = adj[start][0];
    result.push_back(curr);

    while (result.size() < n) {
      const auto& neighbors = adj[curr];
      // Every interior node has exactly two neighbors: the node we just came
      // from (`prev`) and the next unvisited node along the chain.
      const int next = (neighbors[0] == prev) ? neighbors[1] : neighbors[0];
      prev = curr;
      curr = next;
      result.push_back(curr);
    }

    return result;
  }
};

// Validates that `restored` has length `pairs.size() + 1` and induces the exact
// same multiset of unordered adjacent pairs as `pairs`.
[[nodiscard]] bool is_valid_restoration(
    const std::vector<std::vector<int>>& pairs,
    const std::vector<int>& restored) {
  if (restored.size() != pairs.size() + 1uz) {
    return false;
  }

  auto normalize = [](int a, int b) -> std::pair<int, int> {
    return a < b ? std::pair{a, b} : std::pair{b, a};
  };

  std::set<std::pair<int, int>> expected_edges;
  for (const auto& p : pairs) {
    expected_edges.insert(normalize(p[0], p[1]));
  }

  std::set<std::pair<int, int>> actual_edges;
  for (std::size_t i = 0uz; i + 1uz < restored.size(); ++i) {
    actual_edges.insert(normalize(restored[i], restored[i + 1uz]));
  }
  return actual_edges == expected_edges;
}

LC_PROBLEM(1743, "Restore The Array From Adjacent Pairs", "dfs", ctx) {
  Solution sol;

  ctx.test("example_1_ascending_chain", [&] {
    const std::vector<std::vector<int>> pairs = {{2, 1}, {3, 4}, {3, 2}};
    const auto restored = sol.restoreArray(pairs);
    ctx.expect_eq(restored, std::vector<int>{1, 2, 3, 4});
    ctx.expect_true(is_valid_restoration(pairs, restored));
  });

  ctx.test("example_2_negative_values", [&] {
    const std::vector<std::vector<int>> pairs = {{4, -2}, {1, 4}, {-3, 1}};
    const auto restored = sol.restoreArray(pairs);
    ctx.expect_true(is_valid_restoration(pairs, restored));
    ctx.expect_eq(restored, std::vector<int>{-3, 1, 4, -2});
  });

  ctx.test("example_3_single_pair_extremes", [&] {
    const std::vector<std::vector<int>> pairs = {{100000, -100000}};
    const auto restored = sol.restoreArray(pairs);
    ctx.expect_true(is_valid_restoration(pairs, restored));
    ctx.expect_eq(restored, std::vector<int>{-100000, 100000});
  });

  ctx.test("longer_shuffled_chain", [&] {
    const std::vector<std::vector<int>> pairs = {
        {5, 3}, {1, 4}, {3, 2}, {4, 5}, {2, 0}};
    const auto restored = sol.restoreArray(pairs);
    ctx.expect_true(is_valid_restoration(pairs, restored));
    ctx.expect_eq(restored, std::vector<int>{0, 2, 3, 5, 4, 1});
  });

  ctx.test("empty_input", [&] {
    ctx.expect_eq(sol.restoreArray({}), std::vector<int>{});
  });
}

}  // namespace
