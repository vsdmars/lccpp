// https://leetcode.com/problems/friend-circles/
// (Now also titled "Number of Provinces" on LeetCode.)
//
// Given an N x N symmetric adjacency matrix `M` where `M[i][j] == 1` indicates
// students `i` and `j` are direct friends, return the total number of friend
// circles (connected components).

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
