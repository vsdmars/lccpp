// LeetCode 2076: Process Restricted Friend Requests
// https://leetcode.com/problems/process-restricted-friend-requests/
//
// Problem:
//   There are `n` people labeled `0` to `n - 1`. You are given a list of
//   `restrictions` where `restrictions[i] = [xi, yi]` means person `xi` and
//   person `yi` cannot become friends directly or indirectly through mutual
//   friends. You are also given an ordered list of friend `requests` where
//   `requests[j] = [uj, vj]`. Process requests sequentially: a request succeeds
//   (and permanently merges `uj` and `vj`'s friend groups) iff doing so does
//   not cause any restricted pair to end up in the same group; otherwise it is
//   rejected without changing state. Requests between people who are already
//   friends always succeed. Return a boolean array indicating each outcome.
//   Typical constraints: 2 <= n <= 1000; restrictions, requests <= 1000.
//
// Algorithm:
//   Disjoint Set Union (Union-Find) with Pre-Merge Conflict Validation:
//   1. Maintain a single `DisjointSet` representing the current connected
//      components of accepted friendships (initialized with each person alone).
//   2. For each request `[u, v]`:
//      - Find `root_u = dsu.find(u)` and `root_v = dsu.find(v)`.
//      - If `root_u == root_v`, `u` and `v` are already direct or indirect
//        friends; record `true` immediately without modifying the DSU.
//      - Otherwise, call `can_merge(root_u, root_v, dsu, restrictions)` to scan
//        every restriction `[x, y]`. Because the DSU invariant guarantees no
//        prior merge violated any restriction, merging `root_u` and `root_v`
//        violates `[x, y]` iff one of `find(x), find(y)` equals `root_u` and
//        the other equals `root_v`.
//      - If no restriction conflicts, commit `dsu.unite_roots(root_u, root_v)`
//        and record `true`; otherwise leave the DSU untouched and record
//        `false`.
//
//   Complexity:
//     - Time: O(Q * R * alpha(n)), where Q = requests.size(),
//       R = restrictions.size(), and alpha is the inverse Ackermann function.
//     - Space: O(n) auxiliary space for the DSU (excluding the O(Q) output).
//
// Data Structures:
//   - `DisjointSet`: Union-Find with two-pass iterative path compression in
//     `find` (`std::vector<std::size_t> parent_`) and union by rank
//     (`std::vector<int> rank_`) in `unite_roots`.
//   - `std::vector<bool> result`: Pre-reserved output recording whether each
//     request was approved or rejected.
//
// Real-World Applications:
//   - Enforcing Separation of Duties (SoD) and conflict-of-interest policies in
//     Role-Based Access Control (RBAC) when granting permissions or team joins.
//   - Social-network block lists and moderation rules across group merges.
//   - Incremental entity resolution / record linkage with "cannot-link"
//     constraints in constrained clustering and data deduplication.
//
// Related LeetCode Problems:
//   - #990 Satisfiability of Equality Equations (DSU unions + inequality check)
//   - #684 Redundant Connection (incremental DSU connectivity testing)
//   - #1697 Checking Existence of Edge Length Limited Paths (DSU query checks)
//   - #1202 Smallest String With Swaps (groups elements into DSU components)
//   - #721 Accounts Merge (transitive grouping of users with Disjoint Set Union)

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
