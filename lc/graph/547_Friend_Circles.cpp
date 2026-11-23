// LeetCode 547: Friend Circles (Number of Provinces)
// https://leetcode.com/problems/number-of-provinces/
//
// Problem:
//   Given an `n x n` symmetric binary adjacency matrix `isConnected` where
//   `isConnected[i][j] == 1` means node `i` and node `j` are directly
//   connected (and `0` otherwise), return the total number of connected
//   components (friend circles / provinces). Friendship is reflexive and
//   symmetric, and groups are closed under transitivity.
//
// Algorithm:
//   Depth-First Search (DFS) connected-component counting directly over the
//   adjacency matrix. Iterate over each node `i` from `0` to `n - 1`; whenever
//   `i` is unvisited, launch a recursive DFS that marks every transitively
//   reachable neighbor `v` (where `isConnected[u][v] == 1`) as visited and
//   increment the component counter. Each node is visited once, and its full
//   matrix row of length `n` is scanned once.
//   Time: O(N^2), Space: O(N) for the visited array and DFS recursion stack.
//
// Data Structures:
//   - `const std::vector<std::vector<int>>& isConnected`: Input `N x N`
//     undirected adjacency matrix.
//   - `std::vector<std::uint8_t> visited`: Flat boolean-like array tracking
//     which nodes have already been assigned to a component.
//   - Implicit call stack: Drives the recursive DFS lambda traversal.
//
// Real-World Applications:
//   - Social network community detection: partitioning users into isolated
//     social clusters from pairwise connection graphs.
//   - Network topology auditing: counting disconnected subnetworks or isolated
//     server islands in a cluster connectivity matrix.
//   - Fraud and entity clustering: grouping accounts or devices linked by
//     shared attributes into connected risk clusters.
//
// Related LeetCode Problems:
//   - #200 Number of Islands [done] (counting connected components in a 2D grid)
//   - #323 Number of Connected Components in an Undirected Graph [done] (component
//     counting from an edge list)
//   - #695 Max Area of Island [done] (measuring the largest connected component)
//   - #721 Accounts Merge [done] (grouping records into components via shared emails)
//   - #1319 Number of Operations to Make Network Connected [done] (using component
//     counts to find the minimum bridging edges)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Counts connected components in the adjacency matrix using DFS.
  // Time: O(N^2), Space: O(N).
  int findCircleNum(const std::vector<std::vector<int>>& isConnected) {
    const std::size_t n = isConnected.size();
    std::vector<std::uint8_t> visited(n, 0);

    auto dfs = [&](this auto&& self, std::size_t u) -> void {
      visited[u] = 1;
      for (std::size_t v = 0uz; v < n; ++v) {
        if (isConnected[u][v] == 1 && visited[v] == 0) {
          self(v);
        }
      }
    };

    int circles = 0;
    for (std::size_t i = 0uz; i < n; ++i) {
      if (visited[i] == 0) {
        dfs(i);
        ++circles;
      }
    }
    return circles;
  }
};

LC_PROBLEM(547, "Friend Circles", "graph", ctx) {
  Solution sol;

  ctx.test("example 1: two friend circles", [&] {
    const std::vector<std::vector<int>> matrix = {
        {1, 1, 0},
        {1, 1, 0},
        {0, 0, 1},
    };
    ctx.expect_eq(sol.findCircleNum(matrix), 2);
  });

  ctx.test("example 2: one transitive friend circle", [&] {
    const std::vector<std::vector<int>> matrix = {
        {1, 1, 0},
        {1, 1, 1},
        {0, 1, 1},
    };
    ctx.expect_eq(sol.findCircleNum(matrix), 1);
  });

  ctx.test("all isolated students", [&] {
    const std::vector<std::vector<int>> matrix = {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1},
    };
    ctx.expect_eq(sol.findCircleNum(matrix), 3);
  });
}

}  // namespace
