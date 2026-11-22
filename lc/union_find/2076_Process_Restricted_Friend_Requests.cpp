// https://leetcode.com/problems/process-restricted-friend-requests/
//
// Given n people labeled 0..n-1, a list of restriction pairs [xi, yi] that
// cannot become direct or indirect friends, and an ordered list of friend
// requests [uj, vj], process each request in order. A request succeeds if
// merging uj's and vj's friend groups does not connect any restricted pair;
// otherwise it is rejected. Already-connected pairs always succeed.

#include "framework/lc_framework.h"

#include <cstddef>
#include <utility>
#include <vector>

namespace {

// Disjoint Set Union (Union-Find) with iterative path compression and union by
// rank.
class DisjointSet {
 public:
  explicit DisjointSet(std::size_t size) : parent_(size), rank_(size, 0) {
    for (std::size_t i = 0uz; i < size; ++i) {
      parent_[i] = i;
    }
  }

  std::size_t find(std::size_t node) {
    std::size_t root = node;
    while (parent_[root] != root) {
      root = parent_[root];
    }
    while (parent_[node] != root) {
      const std::size_t next = parent_[node];
      parent_[node] = root;
      node = next;
    }
    return root;
  }

  void unite_roots(std::size_t root_a, std::size_t root_b) {
    if (root_a == root_b) {
      return;
    }
    if (rank_[root_a] < rank_[root_b]) {
      std::swap(root_a, root_b);
    }
    parent_[root_b] = root_a;
    if (rank_[root_a] == rank_[root_b]) {
      ++rank_[root_a];
    }
  }

 private:
  std::vector<std::size_t> parent_;
  std::vector<int> rank_;
};

class Solution {
 public:
  // Maintain a single DSU of accepted friendships. Before merging the components
  // of u and v, verify that no restricted pair [x, y] has its endpoints split
  // across those exact two components (which would unite x and y).
  //
  // Time:  O(Q * R * alpha(n)), where Q = requests.size(), R = restrictions.size()
  // Space: O(n) auxiliary space for the DSU (excluding the output array)
  std::vector<bool> friendRequests(
      int n, const std::vector<std::vector<int>>& restrictions,
      const std::vector<std::vector<int>>& requests) {
    DisjointSet dsu(static_cast<std::size_t>(n));
    std::vector<bool> result;
    result.reserve(requests.size());

    for (const auto& request : requests) {
      const auto root_u = dsu.find(static_cast<std::size_t>(request[0]));
      const auto root_v = dsu.find(static_cast<std::size_t>(request[1]));

      // Already direct or indirect friends; always succeeds without mutating.
      if (root_u == root_v) {
        result.push_back(true);
        continue;
      }

      const bool allowed = can_merge(root_u, root_v, dsu, restrictions);
      if (allowed) {
        dsu.unite_roots(root_u, root_v);
      }
      result.push_back(allowed);
    }

    return result;
  }

 private:
  static bool can_merge(std::size_t root_u, std::size_t root_v,
                        DisjointSet& dsu,
                        const std::vector<std::vector<int>>& restrictions) {
    for (const auto& restriction : restrictions) {
      const auto root_x = dsu.find(static_cast<std::size_t>(restriction[0]));
      const auto root_y = dsu.find(static_cast<std::size_t>(restriction[1]));
      const bool conflicts = (root_x == root_u && root_y == root_v) ||
                             (root_x == root_v && root_y == root_u);
      if (conflicts) {
        return false;
      }
    }
    return true;
  }
};

LC_PROBLEM(2076, "Process Restricted Friend Requests", "union_find", ctx) {
  Solution sol;

  ctx.test("example_1", [&] {
    ctx.expect_eq(sol.friendRequests(3, {{0, 1}}, {{0, 2}, {2, 1}}),
                  std::vector<bool>{true, false});
  });

  ctx.test("example_2", [&] {
    ctx.expect_eq(sol.friendRequests(3, {{0, 1}}, {{1, 2}, {0, 2}}),
                  std::vector<bool>{true, false});
  });

  ctx.test("example_3", [&] {
    ctx.expect_eq(
        sol.friendRequests(5, {{0, 1}, {1, 2}, {2, 3}},
                           {{0, 4}, {1, 2}, {3, 1}, {3, 4}}),
        std::vector<bool>{true, false, true, false});
  });

  ctx.test("python_build_case", [&] {
    ctx.expect_eq(
        sol.friendRequests(7, {{0, 6}, {6, 2}},
                           {{0, 2}, {2, 3}, {0, 2}, {6, 4}, {6, 4}}),
        std::vector<bool>{true, true, true, true, true});
  });

  ctx.test("no_restrictions_all_succeed", [&] {
    ctx.expect_eq(
        sol.friendRequests(4, {}, {{0, 1}, {1, 2}, {2, 3}, {0, 3}}),
        std::vector<bool>{true, true, true, true});
  });

  ctx.test("rejected_request_does_not_mutate_state", [&] {
    // 0-1 restricted; 0-2 and 1-3 succeed; 2-3 would connect 0 and 1 so it
    // must fail without merging; 0-4 and 3-4 then test the clean state (0-4
    // succeeds, after which 3-4 would connect 0 and 1 and must fail).
    ctx.expect_eq(
        sol.friendRequests(5, {{0, 1}},
                           {{0, 2}, {1, 3}, {2, 3}, {0, 4}, {3, 4}, {2, 4}}),
        std::vector<bool>{true, true, false, true, false, true});
  });

  ctx.test("already_indirect_friends_succeed", [&] {
    ctx.expect_eq(
        sol.friendRequests(4, {{0, 3}}, {{0, 1}, {1, 2}, {0, 2}, {2, 3}}),
        std::vector<bool>{true, true, true, false});
  });
}

}  // namespace
