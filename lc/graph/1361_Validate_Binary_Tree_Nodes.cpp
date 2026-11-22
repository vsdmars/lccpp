// https://leetcode.com/problems/validate-binary-tree-nodes/
//
// You have `n` nodes numbered 0..n-1 with children given by `leftChild[i]` and
// `rightChild[i]` (`-1` means no child). Return true if and only if all `n`
// nodes form exactly one valid binary tree.
//
// A directed graph with at most 2 outgoing edges per node is a single valid
// binary tree iff:
//   1. Every node has in-degree <= 1, and exactly one node (the root) has
//      in-degree 0;
//   2. Traversing from the root visits every node exactly once (no cycles and
//      no disconnected components).

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
