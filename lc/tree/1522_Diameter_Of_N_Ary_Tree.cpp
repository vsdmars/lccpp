// LeetCode 1522: Diameter Of N Ary Tree
// https://leetcode.com/problems/diameter-of-n-ary-tree/
//
// Problem:
//   Given the root of an N-ary tree, compute the length of its diameter: the
//   number of edges on the longest path between any two nodes in the tree.
//   The longest path may or may not pass through the root.
//   Constraints: depth <= 1000; total number of nodes in [1, 10^4].
//
// Algorithm:
//   Use post-order DFS returning the maximum downward height (in edges) from
//   the current node to any leaf in its subtree (a leaf returns 0). While
//   iterating over a node's children, track the two largest downward branch
//   lengths `top1 >= top2`, where each child contributes `dfs(child) + 1`
//   edges. The longest path whose highest node (LCA) is the current node has
//   length `top1 + top2`; update the global `best_diameter` with this value
//   and return `top1` to the parent. Maintaining `top1` and `top2` directly in
//   two integers matches the top-2 min-heap selection in O(1) extra space per
//   recursion frame.
//   Time Complexity:  O(N), visiting every node once.
//   Space Complexity: O(H) for the DFS recursion stack, where H <= 1000 is the
//                     tree depth.
//
// Data Structures:
//   - N-ary Node struct with `std::vector<Node*> children`.
//   - Two scalar trackers (`top1`, `top2`) per DFS frame for O(1) top-2 child
//     depth tracking.
//
// Real-World Applications:
//   - Measuring worst-case communication hops across hierarchical organizational
//     or directory trees.
//   - Computing maximum dependency chain / critical span in multi-way ASTs and
//     hierarchical routing trees.
//   - Analyzing spread radius in phylogenetic and taxonomic trees.
//
// Related LeetCode Problems:
//   - #543 Diameter of Binary Tree [done] (binary-tree counterpart)
//   - #1245 Tree Diameter [done] (unrooted undirected tree given as edge list)
//   - #559 Maximum Depth of N-ary Tree [done] (single deepest downward path)
//   - #429 N-ary Tree Level Order Traversal [new] (N-ary tree traversal fundamentals)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <deque>
#include <optional>
#include <queue>
#include <vector>

namespace {

// Standard LeetCode N-ary tree node.
struct Node {
  int val = 0;
  std::vector<Node*> children;

  Node() = default;
  explicit Node(int x) : val(x) {}
};

// RAII arena owning test N-ary Node instances via pointer-stable std::deque.
class NaryTreeArena {
 public:
  [[nodiscard]] Node* make(int val) {
    nodes_.emplace_back(val);
    return &nodes_.back();
  }

  // Builds an N-ary tree from LeetCode's level-order serialization where
  // std::nullopt delimits each parent's group of children.
  [[nodiscard]] Node* build(const std::vector<std::optional<int>>& vals) {
    if (vals.empty() || !vals[0].has_value()) {
      return nullptr;
    }
    Node* root = make(*vals[0]);
    std::queue<Node*> q;
    q.push(root);

    // Skip index 0 (root) and index 1 (the null separator following root).
    std::size_t idx = 2uz;
    while (!q.empty() && idx < vals.size()) {
      auto* parent = q.front();
      q.pop();

      while (idx < vals.size() && vals[idx].has_value()) {
        Node* child = make(*vals[idx]);
        parent->children.push_back(child);
        q.push(child);
        ++idx;
      }
      // Advance past the std::nullopt group delimiter.
      ++idx;
    }
    return root;
  }

 private:
  std::deque<Node> nodes_;
};

class Solution {
 public:
  // Computes the diameter (longest path in edges) of the N-ary tree rooted at
  // `root` using bottom-up post-order DFS. Time: O(N), Space: O(H).
  int diameter(const Node* root) {
    if (root == nullptr) {
      return 0;
    }

    int best_diameter = 0;

    // Returns the longest downward path length (in edges) from `node` to any
    // leaf in its subtree (0 for a leaf).
    auto dfs = [&](this auto&& self, const Node* node) -> int {
      int top1 = 0;
      int top2 = 0;

      for (const auto* child : node->children) {
        if (child == nullptr) {
          continue;
        }
        // Each edge from `node` to `child` adds 1 to the child's max depth.
        const int depth = self(child) + 1;
        if (depth > top1) {
          top2 = top1;
          top1 = depth;
        } else if (depth > top2) {
          top2 = depth;
        }
      }

      // The longest path turning at `node` links its two deepest child paths.
      best_diameter = std::max(best_diameter, top1 + top2);
      return top1;
    };

    dfs(root);
    return best_diameter;
  }
};

LC_PROBLEM(1522, "Diameter Of N Ary Tree", "tree", ctx) {
  ctx.test("example_1", [&] {
    NaryTreeArena arena;
    const Node* root =
        arena.build({1, std::nullopt, 3, 2, 4, std::nullopt, 5, 6});
    Solution sol;
    ctx.expect_eq(sol.diameter(root), 3);
  });

  ctx.test("example_2_and_source_build", [&] {
    // Matches Example 2 and Python build():
    //   1 -> 2 -> {3 -> 5, 4 -> 6}, diameter = 5-3-2-4-6 = 4 edges.
    NaryTreeArena arena;
    const Node* root = arena.build({1, std::nullopt, 2, std::nullopt, 3, 4,
                                    std::nullopt, 5, std::nullopt, 6});
    Solution sol;
    ctx.expect_eq(sol.diameter(root), 4);
  });

  ctx.test("example_3_deep_multiway_tree", [&] {
    NaryTreeArena arena;
    const Node* root = arena.build(
        {1,  std::nullopt, 2,  3,  4,  5,  std::nullopt, std::nullopt, 6,
         7,  std::nullopt, 8,  std::nullopt, 9,  10, std::nullopt, std::nullopt,
         11, std::nullopt, 12, std::nullopt, 13, std::nullopt, std::nullopt,
         14});
    Solution sol;
    ctx.expect_eq(sol.diameter(root), 7);
  });

  ctx.test("single_node_and_empty", [&] {
    NaryTreeArena arena;
    const Node* single = arena.make(1);
    Solution sol;
    ctx.expect_eq(sol.diameter(single), 0);
    ctx.expect_eq(sol.diameter(nullptr), 0);
  });

  ctx.test("wide_star_tree", [&] {
    NaryTreeArena arena;
    const Node* root = arena.build({1, std::nullopt, 2, 3, 4, 5, 6});
    Solution sol;
    ctx.expect_eq(sol.diameter(root), 2);
  });
}

}  // namespace
