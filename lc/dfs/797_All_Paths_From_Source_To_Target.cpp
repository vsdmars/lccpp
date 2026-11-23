// LeetCode 797: All Paths From Source To Target
// https://leetcode.com/problems/all-paths-from-source-to-target/
//
// Problem:
//   Given a directed acyclic graph (DAG) of `n` nodes labeled `0` through
//   `n - 1` represented as an adjacency list `graph`, return all possible
//   directed paths from node `0` to node `n - 1` in any order.
//   Constraints: 2 <= n <= 15, 0 <= graph[i][j] < n, no self-loops,
//   unique outgoing neighbors per node, and the graph is guaranteed to be a
//   DAG.
//
// Algorithm:
//   Because the input graph is a DAG, no path can revisit a node (no cycles),
//   so a visited set is unnecessary. Run backtracking DFS starting from node
//   `0` with `path = {0}`:
//     - If `node == n - 1`, snapshot `path` into `result` and return.
//     - Otherwise, for each outgoing neighbor `next` in `graph[node]`, push
//       `next` onto `path`, recurse, and pop `next` on backtrack.
//   Mutating a single `path` vector in place avoids copying the current prefix
//   at every intermediate edge.
//   Time Complexity: O(n * 2^n) in the worst case (a complete DAG has
//     `2^(n - 2)` paths from `0` to `n - 1`, each of length up to `n`).
//   Space Complexity: O(n) auxiliary space for the current path and recursion
//     stack (excluding the returned output list).
//
// Data Structures:
//   - const std::vector<std::vector<int>>& `graph`: input DAG adjacency list.
//   - std::vector<int> `path`: active DFS stack representing the current root-
//     to-node path.
//   - std::vector<std::vector<int>> `result`: stores all completed paths.
//
// Real-World Applications:
//   - Static program analysis and symbolic execution: enumerating all control-
//     flow paths between entry and exit basic blocks in acyclic CFGs.
//   - Build systems and workflow engines (e.g., Bazel, Airflow): tracing all
//     dependency chains from a root target to a final artifact.
//   - Network routing and supply-chain lineage tracing across directed stages.
//
// Related LeetCode Problems:
//   - #207 Course Schedule [done] (cycle detection / topological ordering on directed graphs)
//   - #210 Course Schedule II [done] (topological sort of a DAG)
//   - #1059 All Paths from Source Lead to Destination [new] (verifying all paths terminate at target)
//   - #257 Binary Tree Paths [done] (enumerating all root-to-leaf paths via backtracking DFS)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Enumerates all directed paths from node `0` to node `n - 1` in a DAG.
  // Time: O(n * 2^n); Space: O(n) auxiliary stack space.
  std::vector<std::vector<int>> allPathsSourceTarget(
      const std::vector<std::vector<int>>& graph) {
    std::vector<std::vector<int>> result;
    if (graph.empty()) {
      return result;
    }

    const int target = static_cast<int>(graph.size()) - 1;
    std::vector<int> path;
    path.reserve(graph.size());
    path.push_back(0);

    // Backtracking DFS: invariant is that `path` holds a valid directed path
    // starting at 0 and ending at `node`.
    auto dfs = [&](this auto&& self, int node) -> void {
      if (node == target) {
        result.push_back(path);
        return;
      }

      for (const int next : graph[static_cast<std::size_t>(node)]) {
        path.push_back(next);
        self(next);
        path.pop_back();
      }
    };

    dfs(0);
    return result;
  }
};

LC_PROBLEM(797, "All Paths From Source To Target", "dfs", ctx) {
  ctx.test("example_1_diamond_dag", [&] {
    Solution sol;
    const std::vector<std::vector<int>> graph = {{1, 2}, {3}, {3}, {}};
    auto actual = sol.allPathsSourceTarget(graph);
    std::vector<std::vector<int>> expected = {{0, 1, 3}, {0, 2, 3}};
    std::ranges::sort(actual);
    std::ranges::sort(expected);
    ctx.expect_eq(actual, expected);
  });

  ctx.test("example_2_five_node_dag", [&] {
    Solution sol;
    const std::vector<std::vector<int>> graph = {
        {4, 3, 1}, {3, 2, 4}, {3}, {4}, {}};
    auto actual = sol.allPathsSourceTarget(graph);
    std::vector<std::vector<int>> expected = {
        {0, 4}, {0, 3, 4}, {0, 1, 3, 4}, {0, 1, 2, 3, 4}, {0, 1, 4}};
    std::ranges::sort(actual);
    std::ranges::sort(expected);
    ctx.expect_eq(actual, expected);
  });

  ctx.test("minimal_two_node_graph", [&] {
    Solution sol;
    const std::vector<std::vector<int>> graph = {{1}, {}};
    const std::vector<std::vector<int>> expected = {{0, 1}};
    ctx.expect_eq(sol.allPathsSourceTarget(graph), expected);
  });

  ctx.test("unreachable_target", [&] {
    Solution sol;
    const std::vector<std::vector<int>> graph = {{1}, {}, {3}, {}};
    const std::vector<std::vector<int>> expected = {};
    ctx.expect_eq(sol.allPathsSourceTarget(graph), expected);
  });

  ctx.test("complete_four_node_dag", [&] {
    Solution sol;
    const std::vector<std::vector<int>> graph = {{1, 2, 3}, {2, 3}, {3}, {}};
    auto actual = sol.allPathsSourceTarget(graph);
    std::vector<std::vector<int>> expected = {
        {0, 3}, {0, 1, 3}, {0, 2, 3}, {0, 1, 2, 3}};
    std::ranges::sort(actual);
    std::ranges::sort(expected);
    ctx.expect_eq(actual, expected);
  });
}

}  // namespace
