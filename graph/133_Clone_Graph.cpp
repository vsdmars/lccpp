// https://leetcode.com/problems/clone-graph/description/
//
// Given a reference of a node in a connected undirected graph, return a deep
// copy (clone) of the graph. Each node contains a value (`int`) and a list of
// its neighbors (`std::vector<Node*>`).

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
