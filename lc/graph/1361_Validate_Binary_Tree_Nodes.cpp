// LeetCode 1361: Validate Binary Tree Nodes
// https://leetcode.com/problems/validate-binary-tree-nodes/
//
// Problem:
//   You are given `n` nodes labeled `0..n-1` with child pointers given by
//   `leftChild[i]` and `rightChild[i]` (`-1` means no child). Return `true` if
//   and only if all `n` nodes form exactly one valid binary tree; otherwise
//   return `false`. Constraints: 1 <= n <= 10^4,
//   leftChild.length == rightChild.length == n, and
//   -1 <= leftChild[i], rightChild[i] <= n - 1.
//
// Examples:
//   Example 1:
//     Input:  n = 4, leftChild = [1, -1, 3, -1], rightChild = [2, -1, -1, -1]
//     Output: true
//     Explanation:
//       - find_unique_root computes indegree = [0, 1, 1, 1] and identifies node 0
//         as the unique root.
//       - BFS starts at node 0 (visited_count = 1), enqueues children 1 and 2, and
//         then pops node 2 to enqueue child 3.
//       - All nodes are visited once without revisits, so visited_count == 4
//         matches n and returns true.
//
//   Example 2:
//     Input:  n = 4, leftChild = [1, -1, 3, -1], rightChild = [2, 3, -1, -1]
//     Output: false
//     Explanation:
//       - find_unique_root scans parent links and increments indegree[1] = 1,
//         indegree[2] = 1, and indegree[3] = 1 from nodes 0 and 1.
//       - At node 2, leftChild[2] = 3 increments indegree[3] to 2 > 1, so
//         find_unique_root returns -1 and validation returns false.
//
//   Example 3:
//     Input:  n = 4, leftChild = [1, 0, 3, -1], rightChild = [-1, -1, -1, -1]
//     Output: false
//     Explanation:
//       - find_unique_root computes indegree = [1, 1, 0, 1] and finds node 2 as the
//         unique node with indegree 0.
//       - BFS starting from root 2 visits only nodes 2 and 3, leaving the {0, 1}
//         cycle unreachable.
//       - When the queue empties, visited_count is 2 != 4, so
//         validateBinaryTreeNodes returns false.
//
// Algorithm:
//   Two-phase validation combining in-degree inspection with Breadth-First
//   Search (BFS). A directed graph where every vertex has out-degree <= 2 forms
//   a single valid binary tree if and only if: (1) every node has in-degree <=
//   1 (no shared child / multiple parents) and exactly one node has in-degree 0
//   (the unique root), and (2) traversing from that root visits all `n` nodes
//   exactly once (no directed cycles and no disconnected forest components).
//   `find_unique_root` validates condition (1) in a single pass, then BFS from
//   `root` checks for revisits and verifies `visited_count == n`.
//   Time Complexity: O(n), inspecting each node and at most 2n child links a
//     constant number of times.
//   Space Complexity: O(n) for the in-degree array, visited array, and BFS
//     queue.
//
// Data Structures:
//   - `std::vector<int> indegree`: Counts incoming parent edges per node to
//     reject nodes with > 1 parent and locate the unique root.
//   - `std::queue<int> q`: FIFO frontier queue driving BFS traversal from the
//     root.
//   - `std::vector<std::uint8_t> visited`: Compact byte-flag array detecting
//     cycles or shared descendants and verifying full reachability.
//
// Real-World Applications:
//   - Validating deserialized ASTs, DOM trees, or flat index-table tree formats
//     before running recursive traversals that would loop on hidden cycles.
//   - Integrity verification of hierarchical directory trees, org charts, or
//     scene graphs stored as parent-child rows in relational databases.
//   - Sanity-checking binary decision trees or expression trees built from
//     untrusted user configurations.
//
// Related LeetCode Problems:
//   - #261 Graph Valid Tree [done] (validating that an undirected graph is a single
//     connected acyclic tree)
//   - #685 Redundant Connection II [done] (rooted directed tree anomalies involving a
//     node with two parents or a directed cycle)
//   - #2196 Create Binary Tree From Descriptions [new] (building a binary tree from
//     child links and finding the unique zero-in-degree root)
//   - #207 Course Schedule [done] (directed cycle detection via in-degrees and BFS)
//   - #98 Validate Binary Search Tree [done] (validating structural/ordering rules on
//     a binary tree)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <queue>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the unique root via in-degrees, then runs BFS to verify there are no
  // cycles, shared children, or disconnected subtrees.
  // Time: O(n), Space: O(n).
  bool validateBinaryTreeNodes(int n, const std::vector<int>& leftChild,
                               const std::vector<int>& rightChild) {
    if (n <= 0) {
      return false;
    }

    const int root = find_unique_root(n, leftChild, rightChild);
    if (root == -1) {
      return false;
    }

    const auto count = static_cast<std::size_t>(n);
    std::queue<int> q;
    std::vector<std::uint8_t> visited(count, 0);
    q.push(root);
    visited[static_cast<std::size_t>(root)] = 1;
    std::size_t visited_count = 1uz;

    while (!q.empty()) {
      const auto u = static_cast<std::size_t>(q.front());
      q.pop();

      for (const int child : {leftChild[u], rightChild[u]}) {
        if (child == -1) {
          continue;
        }
        const auto uc = static_cast<std::size_t>(child);
        if (visited[uc] != 0) {
          return false;  // Cycle or shared descendant detected.
        }
        visited[uc] = 1;
        ++visited_count;
        q.push(child);
      }
    }

    return visited_count == count;
  }

 private:
  // Computes in-degrees and returns the unique root (in-degree 0), or -1 if
  // any node has > 1 parent or there is not exactly one root.
  static int find_unique_root(int n, const std::vector<int>& left_child,
                              const std::vector<int>& right_child) {
    const auto count = static_cast<std::size_t>(n);
    std::vector<int> indegree(count, 0);

    for (std::size_t i = 0uz; i < count; ++i) {
      for (const int child : {left_child[i], right_child[i]}) {
        if (child != -1 && ++indegree[static_cast<std::size_t>(child)] > 1) {
          return -1;
        }
      }
    }

    int root = -1;
    for (int i = 0; i < n; ++i) {
      if (indegree[static_cast<std::size_t>(i)] == 0) {
        if (root != -1) {
          return -1;  // Multiple roots imply a forest.
        }
        root = i;
      }
    }
    return root;
  }
};

LC_PROBLEM(1361, "Validate Binary Tree Nodes", "graph", ctx) {
  Solution sol;

  ctx.test("example 1: valid 4-node binary tree", [&] {
    ctx.expect_true(
        sol.validateBinaryTreeNodes(4, {1, -1, 3, -1}, {2, -1, -1, -1}));
  });

  ctx.test("example 2: node 3 has two parents", [&] {
    ctx.expect_false(
        sol.validateBinaryTreeNodes(4, {1, -1, 3, -1}, {2, 3, -1, -1}));
  });

  ctx.test("example 3: 2-node cycle", [&] {
    ctx.expect_false(sol.validateBinaryTreeNodes(2, {1, 0}, {-1, -1}));
  });

  ctx.test("example 4: disconnected forest of two trees", [&] {
    ctx.expect_false(sol.validateBinaryTreeNodes(6, {1, -1, -1, 4, -1, -1},
                                                 {2, -1, -1, 5, -1, -1}));
  });

  ctx.test("python sample: cycle plus separate component", [&] {
    ctx.expect_false(
        sol.validateBinaryTreeNodes(4, {1, 0, 3, -1}, {-1, -1, -1, -1}));
  });
}

}  // namespace
