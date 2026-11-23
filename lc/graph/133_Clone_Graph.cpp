// LeetCode 133: Clone Graph
// https://leetcode.com/problems/clone-graph/
//
// Problem:
//   Given a pointer `node` to a node in a connected undirected graph (or
//   `nullptr` for an empty graph), return a deep copy (clone) of the entire
//   graph. Each `Node` holds an integer `val` and a `std::vector<Node*>` of
//   neighbor pointers. Every cloned node must be newly allocated with matching
//   values and neighbor topology, sharing no pointers with the original graph.
//   Constraints: 0 <= number of nodes <= 100, unique node values (1..100 in
//   standard inputs; the implementation also handles self-cycles).
//
// Examples:
//   Example 1:
//     Input:  adjList = [[2, 4], [1, 3], [2, 4], [1, 3]] (starting at node 1)
//     Output: [[2, 4], [1, 3], [2, 4], [1, 3]] (deep copy with newly allocated
//             Node* instances)
//     Explanation:
//       - `dfs(1)` allocates clone `1'`, records `cloned[1] = 1'` before recursing,
//         and visits neighbors `2` and `4`.
//       - Recursing on `2` allocates `2'` and stores `cloned[2] = 2'`; its neighbor
//         `1` hits `cloned[1]` and returns `1'`, then DFS recurses on `3`.
//       - Recursing on `3` allocates `3'` and stores `cloned[3] = 3'`; neighbor `2`
//         hits `cloned[2]`, and neighbor `4` allocates `4'` and stores `cloned[4] =
//         4'`.
//       - At `4`, neighbors `1` and `3` hit `cloned[1]` and `cloned[3]`, completing
//         `4'->neighbors = {1', 3'}` and unwinding so every cloned node mirrors the
//         original topology with no shared pointers.
//
//   Example 2:
//     Input:  adjList = [[1, 2], [2], [2]] (3-node graph 0->[1,2], 1->[2], 2->[2]
//             starting at node 0)
//     Output: [[1, 2], [2], [2]] (deep copy preserving shared edge to 2 and
//             self-loop on 2)
//     Explanation:
//       - `dfs(0)` allocates clone `0'`, stores `cloned[0] = 0'`, and recurses on
//         neighbor `1`.
//       - `dfs(1)` allocates `1'`, stores `cloned[1] = 1'`, and recurses on
//         neighbor `2`.
//       - `dfs(2)` allocates `2'` and stores `cloned[2] = 2'` before visiting
//         neighbors, so the self-edge `2 -> 2` hits `cloned[2]` immediately and
//         appends `2'` to `2'->neighbors`.
//       - Unwinding sets `1'->neighbors = {2'}` and returns to `0'`, where the
//         second neighbor `2` hits `cloned[2]` so `0'->neighbors = {1', 2'}` shares
//         the same `2'`.
//
//   Example 3:
//     Input:  node = nullptr
//     Output: nullptr
//     Explanation:
//       - `cloneGraph(nullptr)` (and `review(nullptr)`) checks `if (node ==
//         nullptr)` at entry.
//       - Because the input pointer is null, it immediately returns `nullptr`
//         without allocating any `Node` or hash map entries.
//
// Algorithm:
//   Memoized Depth-First Search (DFS) graph cloning. Both `cloneGraph` (keyed
//   by original `const Node*` address) and `review` (keyed by unique `int` node
//   value) allocate the cloned `Node` and register it in a hash map BEFORE
//   recursing on its neighbors. This pre-order registration invariant breaks
//   cycles (including self-loops) and ensures every original node is cloned
//   exactly once; subsequent encounters return the cached clone pointer in O(1)
//   average time, faithfully reconstructing shared edges.
//   Time Complexity: O(V + E), where V is the number of nodes and E is the
//     number of edges, visiting each node and neighbor edge once.
//   Space Complexity: O(V) for the clone hash map and DFS recursion stack
//     (excluding the O(V + E) cloned graph output).
//
// Data Structures:
//   - `Node` (`int val`, `std::vector<Node*> neighbors`): Heap-allocated
//     adjacency-list graph node matching LeetCode's C++ interface.
//   - `std::unordered_map<const Node*, Node*> cloned` (`cloneGraph`) and
//     `std::unordered_map<int, Node*> dp` (`review`): Hash maps associating
//     original nodes/values with their cloned `Node*` to prevent infinite
//     recursion on cycles and preserve graph topology.
//   - `GraphOwner` (test helper using `std::unordered_set<Node*>` and iterative
//     `std::vector<Node*>` stack): RAII guard freeing all reachable nodes once.
//
// Real-World Applications:
//   - Deep cloning and serialization of cyclic in-memory object graphs (e.g.,
//     VM heap snapshots, object pickling, or DOM/scene-graph duplication).
//   - Compiler IR and ML computation-graph cloning (duplicating control-flow or
//     tensor graphs prior to destructive optimization passes).
//   - Copy-on-write state isolation in graph databases and simulation engines.
//
// Related LeetCode Problems:
//   - #138 Copy List with Random Pointer [done] (deep-cloning a cyclic pointer graph
//     via an original-to-clone hash map)
//   - #1485 Clone Binary Tree With Random Pointer [done] (memoized deep copy of a
//     tree augmented with arbitrary cross/cycle pointers)
//   - #1490 Clone N-ary Tree [new] (recursive deep cloning of an acyclic N-ary tree)
//   - #297 Serialize and Deserialize Binary Tree [done] (traversing and reconstructing
//     pointer-based node structures)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdio>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace {

// Definition for a Node (matches LeetCode C++ signature).
class Node {
 public:
  int val = 0;
  std::vector<Node*> neighbors;

  Node() = default;
  explicit Node(int v) : val(v) {}
  Node(int v, std::vector<Node*> nbrs) : val(v), neighbors(std::move(nbrs)) {}
};

class Solution {
 public:
  // DFS clone caching visited nodes by value/pointer.
  Node* cloneGraph(Node* node) {
    if (node == nullptr) {
      return nullptr;
    }
    std::unordered_map<const Node*, Node*> cloned;
    auto dfs = [&](this auto&& self, const Node* cur) -> Node* {
      if (auto it = cloned.find(cur); it != cloned.end()) {
        return it->second;
      }
      auto* copy = new Node(cur->val);
      cloned.emplace(cur, copy);
      copy->neighbors.reserve(cur->neighbors.size());
      for (const Node* nbr : cur->neighbors) {
        copy->neighbors.push_back(self(nbr));
      }
      return copy;
    };
    return dfs(node);
  }

  // Alternative review implementation (mirrors review() in 133_Clone_Graph.py).
  Node* review(Node* node) {
    if (node == nullptr) {
      return nullptr;
    }
    std::unordered_map<int, Node*> dp;
    auto dfs = [&](this auto&& self, const Node* cur) -> Node* {
      if (auto it = dp.find(cur->val); it != dp.end()) {
        return it->second;
      }
      auto* copy = new Node(cur->val, {});
      dp.emplace(cur->val, copy);
      for (const Node* nbr : cur->neighbors) {
        copy->neighbors.push_back(self(nbr));
      }
      return copy;
    };
    return dfs(node);
  }
};

// RAII guard that frees all reachable nodes in a raw-pointer graph on scope exit.
struct GraphOwner {
  Node* root = nullptr;

  explicit GraphOwner(Node* r) : root(r) {}
  ~GraphOwner() {
    if (root == nullptr) {
      return;
    }
    try {
      std::unordered_set<Node*> visited;
      std::vector<Node*> owned;
      std::vector<Node*> stack;
      stack.push_back(root);
      visited.insert(root);
      while (!stack.empty()) {
        Node* cur = stack.back();
        stack.pop_back();
        owned.push_back(cur);
        for (Node* nbr : cur->neighbors) {
          if (nbr != nullptr && visited.insert(nbr).second) {
            stack.push_back(nbr);
          }
        }
      }
      for (Node* n : owned) {
        delete n;
      }
    } catch (...) {
      std::fputs("GraphOwner cleanup failed\n", stderr);
    }
  }

  GraphOwner(const GraphOwner&) = delete;
  GraphOwner& operator=(const GraphOwner&) = delete;
};

// Mirrors build() from 133_Clone_Graph.py: 3 nodes (0, 1, 2) with self-cycle on 2.
GraphOwner build_py_sample_graph() {
  auto* n0 = new Node(0);
  auto* n1 = new Node(1);
  auto* n2 = new Node(2);

  n0->neighbors.push_back(n1);
  n0->neighbors.push_back(n2);
  n1->neighbors.push_back(n2);
  n2->neighbors.push_back(n2);
  return GraphOwner(n0);
}

// Standard LeetCode Example 1: 4-node undirected cycle [[2,4],[1,3],[2,4],[1,3]].
GraphOwner build_leetcode_example1() {
  auto* n1 = new Node(1);
  auto* n2 = new Node(2);
  auto* n3 = new Node(3);
  auto* n4 = new Node(4);

  n1->neighbors = {n2, n4};
  n2->neighbors = {n1, n3};
  n3->neighbors = {n2, n4};
  n4->neighbors = {n1, n3};
  return GraphOwner(n1);
}

// Verifies `cloned` is a structurally identical deep copy of `orig` (no shared
// node pointers anywhere in the reachable graph).
bool is_deep_clone(const Node* orig, const Node* cloned) {
  if (orig == nullptr || cloned == nullptr) {
    return orig == cloned;
  }
  std::unordered_map<const Node*, const Node*> mapping;
  auto check = [&](this auto&& self, const Node* a, const Node* b) -> bool {
    if (a == nullptr || b == nullptr) {
      return a == b;
    }
    if (a == b || a->val != b->val || a->neighbors.size() != b->neighbors.size()) {
      return false;
    }
    if (auto it = mapping.find(a); it != mapping.end()) {
      return it->second == b;
    }
    mapping.emplace(a, b);
    for (std::size_t i = 0uz; i < a->neighbors.size(); ++i) {
      if (!self(a->neighbors[i], b->neighbors[i])) {
        return false;
      }
    }
    return true;
  };
  return check(orig, cloned);
}

LC_PROBLEM(133, "Clone Graph", "graph", ctx) {
  Solution sol;

  ctx.test("cloneGraph: sample graph with self-cycle", [&] {
    auto orig = build_py_sample_graph();
    GraphOwner copy(sol.cloneGraph(orig.root));
    ctx.expect_true(is_deep_clone(orig.root, copy.root),
                    "cloned graph must be a deep copy");
  });

  ctx.test("review: sample graph with self-cycle", [&] {
    auto orig = build_py_sample_graph();
    GraphOwner copy(sol.review(orig.root));
    ctx.expect_true(is_deep_clone(orig.root, copy.root),
                    "review() must produce a deep copy");
  });

  ctx.test("cloneGraph & review: LeetCode 4-node cycle", [&] {
    auto orig = build_leetcode_example1();
    GraphOwner c1(sol.cloneGraph(orig.root));
    GraphOwner c2(sol.review(orig.root));
    ctx.expect_true(is_deep_clone(orig.root, c1.root));
    ctx.expect_true(is_deep_clone(orig.root, c2.root));
  });

  ctx.test("nullptr input", [&] {
    ctx.expect_eq(sol.cloneGraph(nullptr), nullptr);
    ctx.expect_eq(sol.review(nullptr), nullptr);
  });
}

}  // namespace
