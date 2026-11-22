// https://leetcode.com/problems/number-of-islands-ii/
//
// Starting from an m x n grid of water, perform a sequence of `addLand(r, c)`
// operations. After each operation, return the current number of 4-directionally
// connected islands.

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

constexpr std::array<std::pair<int, int>, 4> kDirs = {
    {{0, 1}, {0, -1}, {1, 0}, {-1, 0}}};

class Solution {
 public:
  // Dynamic connectivity using Union-Find with path compression & union by rank.
  // Time: O(k * alpha(m * n)), Space: O(m * n) where k = positions.size().
  std::vector<int> numIslands2(int m, int n,
                               const std::vector<std::vector<int>>& positions) {
    const auto total = static_cast<std::size_t>(m) * static_cast<std::size_t>(n);
    // `parent[id] == -1` indicates water; otherwise it stores the DSU parent.
    std::vector<int> parent(total, -1);
    std::vector<int> rank(total, 0);

    auto find = [&](this auto&& self, int x) -> int {
      const auto idx = static_cast<std::size_t>(x);
      if (parent[idx] != x) {
        parent[idx] = self(parent[idx]);
      }
      return parent[idx];
    };

    auto unite = [&](int a, int b) -> bool {
      int ra = find(a);
      int rb = find(b);
      if (ra == rb) {
        return false;
      }
      if (rank[static_cast<std::size_t>(ra)] <
          rank[static_cast<std::size_t>(rb)]) {
        std::swap(ra, rb);
      }
      parent[static_cast<std::size_t>(rb)] = ra;
      if (rank[static_cast<std::size_t>(ra)] ==
          rank[static_cast<std::size_t>(rb)]) {
        ++rank[static_cast<std::size_t>(ra)];
      }
      return true;
    };

    std::vector<int> result;
    result.reserve(positions.size());
    int islands = 0;

    for (const auto& pos : positions) {
      const int r = pos[0];
      const int c = pos[1];
      const int id = (r * n) + c;

      // Ignore duplicate addLand operations on an existing land cell.
      if (parent[static_cast<std::size_t>(id)] != -1) {
        result.push_back(islands);
        continue;
      }

      parent[static_cast<std::size_t>(id)] = id;
      ++islands;

      for (const auto& [dr, dc] : kDirs) {
        const int nr = r + dr;
        const int nc = c + dc;
        if (nr < 0 || nr >= m || nc < 0 || nc >= n) {
          continue;
        }
        const int nid = (nr * n) + nc;
        if (parent[static_cast<std::size_t>(nid)] != -1 && unite(id, nid)) {
          --islands;
        }
      }

      result.push_back(islands);
    }

    return result;
  }
};

LC_PROBLEM(305, "Number of Islands II", "graph", ctx) {
  Solution sol;

  ctx.test("example 1: standard 3x3 grid", [&] {
    ctx.expect_eq(sol.numIslands2(3, 3, {{0, 0}, {0, 1}, {1, 2}, {2, 1}}),
                  std::vector<int>{1, 1, 2, 3});
  });

  ctx.test("duplicate land position", [&] {
    ctx.expect_eq(sol.numIslands2(3, 3, {{0, 0}, {0, 1}, {1, 2}, {1, 2}}),
                  std::vector<int>{1, 1, 2, 2});
  });

  ctx.test("bridge merges three separate islands into one", [&] {
    ctx.expect_eq(sol.numIslands2(3, 3, {{0, 0}, {0, 2}, {1, 1}, {0, 1}}),
                  std::vector<int>{1, 2, 3, 1});
  });
}

}  // namespace
