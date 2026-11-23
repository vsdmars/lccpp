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
// Examples:
//   Example 1:
//     Input:  isConnected = [[1, 1, 0], [1, 1, 0], [0, 0, 1]]
//     Output: 2
//     Explanation:
//       - Initialize visited = [0, 0, 0] and circles = 0 for n = 3.
//       - At i = 0 (visited[0] == 0), dfs(0) sets visited[0] = 1 and follows
//         isConnected[0][1] == 1 to call dfs(1), which sets visited[1] = 1 and
//         increments circles to 1.
//       - Skip i = 1 because visited[1] is already 1.
//       - At i = 2 (visited[2] == 0), dfs(2) marks visited[2] = 1 with no unvisited
//         neighbors and increments circles to 2.
//
//   Example 2:
//     Input:  isConnected = [[1, 1, 0], [1, 1, 1], [0, 1, 1]]
//     Output: 1
//     Explanation:
//       - Initialize visited = [0, 0, 0] and circles = 0 for n = 3.
//       - At i = 0, dfs(0) marks visited[0] = 1 and visits neighbor 1
//         (isConnected[0][1] == 1).
//       - dfs(1) marks visited[1] = 1 and visits neighbor 2 (isConnected[1][2] ==
//         1), which marks visited[2] = 1 and increments circles to 1.
//       - Skip i = 1 and i = 2 because visited is now [1, 1, 1], returning 1.
//
//   Example 3:
//     Input:  isConnected = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]
//     Output: 3
//     Explanation:
//       - Initialize visited = [0, 0, 0] and circles = 0 for n = 3.
//       - For each i in 0, 1, 2, dfs(i) finds no off-diagonal 1s, marks only
//         visited[i] = 1, and increments circles.
//       - Return circles = 3 after all three isolated nodes are processed.
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
