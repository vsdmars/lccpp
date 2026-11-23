// LeetCode 1485: Clone Binary Tree With Random Pointer
// https://leetcode.com/problems/clone-binary-tree-with-random-pointer/
//
// Problem:
//   Given the root of a binary tree where every node has an integer value,
//   left and right child pointers, and an additional `random` pointer that may
//   point to any node in the tree (including itself or an ancestor) or nullptr,
//   return an independent deep copy of the entire structure as a `NodeCopy`
//   tree preserving all left, right, and random relationships.
//
// Algorithm:
//   Use depth-first traversal with a memoization hash map mapping each
//   original `const Node*` to its newly allocated `NodeCopy*`. When visiting a
//   node:
//     1. If `node == nullptr`, return `nullptr`.
//     2. If `node` already exists in the map, return the cached `NodeCopy*`
//        immediately (this prevents infinite recursion on cycles induced by
//        `random` pointers and guarantees a 1-to-1 bijection).
//     3. Allocate `copy = new NodeCopy(node->val)`, record `clones[node] = copy`
//        BEFORE recursing, and then recursively populate `copy->left`,
//        `copy->right`, and `copy->random`.
//   Time Complexity:  O(N), where N is the number of nodes (each node and its
//                     three outgoing edges are processed once).
//   Space Complexity: O(N) for the original-to-clone hash map and recursion
//                     stack.
//
// Data Structures:
//   - std::unordered_map<const Node*, NodeCopy*>: bijection cache ensuring
//     shared/cyclic random references resolve to the exact cloned node.
//
// Real-World Applications:
//   - Deep-copying ASTs / IR graphs in compilers where nodes hold cross-tree
//     symbol-table or def-use back-pointers.
//   - Object-graph serialization and snapshotting (e.g., Java/Python deepcopy,
//     ORM entity detachment) where arbitrary references can form cycles.
//   - Cloning scene graphs or document trees with cross-references/hyperlinks.
//
// Related LeetCode Problems:
//   - #138 Copy List with Random Pointer [done] (linked-list equivalent with random links)
//   - #133 Clone Graph [done] (general cyclic graph deep copy with DFS/BFS memoization)
//   - #1490 Clone N-ary Tree [new] (deep copy of an N-ary tree without random pointers)
//   - #226 Invert Binary Tree [done] (recursive structural tree transformation)

#include "framework/lc_framework.h"

#include <deque>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

// Input binary tree node with an arbitrary random pointer.
struct Node {
  int val = 0;
  Node* left = nullptr;
  Node* right = nullptr;
  Node* random = nullptr;

  Node() = default;
  explicit Node(int x) : val(x) {}
  Node(int x, Node* l, Node* r, Node* rand)
      : val(x), left(l), right(r), random(rand) {}
};

// Output cloned binary tree node mirroring Node's fields.
struct NodeCopy {
  int val = 0;
  NodeCopy* left = nullptr;
  NodeCopy* right = nullptr;
  NodeCopy* random = nullptr;

  NodeCopy() = default;
  explicit NodeCopy(int x) : val(x) {}
  NodeCopy(int x, NodeCopy* l, NodeCopy* r, NodeCopy* rand)
      : val(x), left(l), right(r), random(rand) {}
};

// RAII owner for heap-allocated NodeCopy graphs returned by Solution, freeing
// every reachable cloned node (even in the presence of random-pointer cycles).
class ClonedTreeGuard {
 public:
  explicit ClonedTreeGuard(NodeCopy* root) : root_(root) {
    if (root_ == nullptr) {
      return;
    }
    // Collect ownership of all reachable nodes upfront in deterministic DFS
    // order so destruction is noexcept and avoids iterating an unordered_set.
    std::unordered_set<NodeCopy*> visited;
    std::vector<NodeCopy*> stack{root_};
    while (!stack.empty()) {
      auto* cur = stack.back();
      stack.pop_back();
      if (cur == nullptr || !visited.insert(cur).second) {
        continue;
      }
      owned_.emplace_back(cur);
      stack.push_back(cur->left);
      stack.push_back(cur->right);
      stack.push_back(cur->random);
    }
  }
  ClonedTreeGuard(const ClonedTreeGuard&) = delete;
  ClonedTreeGuard& operator=(const ClonedTreeGuard&) = delete;
  ClonedTreeGuard(ClonedTreeGuard&&) = delete;
  ClonedTreeGuard& operator=(ClonedTreeGuard&&) = delete;
  ~ClonedTreeGuard() = default;

  [[nodiscard]] NodeCopy* get() const { return root_; }

 private:
  NodeCopy* root_ = nullptr;
  std::vector<std::unique_ptr<NodeCopy>> owned_;
};

// Verifies that `copy` is a structurally identical, value-preserving,
// bijective deep copy of `orig` across left, right, and random links.
[[nodiscard]] bool verify_clone(const Node* orig, const NodeCopy* copy) {
  std::unordered_map<const Node*, const NodeCopy*> forward;
  std::unordered_set<const NodeCopy*> seen_copies;

  auto dfs = [&](this auto&& self, const Node* a, const NodeCopy* b) -> bool {
    if (a == nullptr && b == nullptr) {
      return true;
    }
    if (a == nullptr || b == nullptr || a->val != b->val) {
      return false;
    }
    if (const auto it = forward.find(a); it != forward.end()) {
      return it->second == b;
    }
    if (!seen_copies.insert(b).second) {
      return false;
    }
    forward.emplace(a, b);
    return self(a->left, b->left) && self(a->right, b->right) &&
           self(a->random, b->random);
  };

  return dfs(orig, copy);
}

class Solution {
 public:
  // Deep-copies a binary tree with arbitrary random pointers using memoized
  // DFS. Inserts each clone into the lookup map before recursing so cycles
  // created by random pointers terminate cleanly. Time: O(N), Space: O(N).
  NodeCopy* copyRandomBinaryTree(const Node* root) {
    std::unordered_map<const Node*, NodeCopy*> clones;

    auto dfs = [&](this auto&& self, const Node* node) -> NodeCopy* {
      if (node == nullptr) {
        return nullptr;
      }

      // Return the already-created clone if this node was reached earlier
      // (either via a tree edge or a random back/cross pointer).
      if (const auto it = clones.find(node); it != clones.end()) {
        return it->second;
      }

      // Register the clone BEFORE recursing so self-loops and random cycles
      // resolve to `copy` instead of recursing infinitely.
      auto* copy = new NodeCopy(node->val);
      clones.emplace(node, copy);

      copy->left = self(node->left);
      copy->right = self(node->right);
      copy->random = self(node->random);
      return copy;
    };

    return dfs(root);
  }
};

LC_PROBLEM(1485, "Clone Binary Tree With Random Pointer", "tree", ctx) {
  ctx.test("example_1_and_source_build", [&] {
    // Matches LeetCode Example 1 and Python build():
    //   1 -> right=4 -> left=7; 4.random=7; 7.random=1.
    std::deque<Node> arena;
    Node& n1 = arena.emplace_back(1);
    Node& n4 = arena.emplace_back(4);
    Node& n7 = arena.emplace_back(7);
    n1.right = &n4;
    n4.left = &n7;
    n4.random = &n7;
    n7.random = &n1;

    Solution sol;
    ClonedTreeGuard guard(sol.copyRandomBinaryTree(&n1));
    ctx.expect_true(verify_clone(&n1, guard.get()));
    const auto* cloned = guard.get();
    ctx.expect_true(cloned != nullptr);
    if (cloned != nullptr && cloned->right != nullptr &&
        cloned->right->left != nullptr && cloned->right->random != nullptr) {
      ctx.expect_eq(cloned->val, 1);
      ctx.expect_eq(cloned->right->random->val, 7);
      ctx.expect_eq(cloned->right->left->random, cloned);
    }
  });

  ctx.test("empty_tree", [&] {
    Solution sol;
    ClonedTreeGuard guard(sol.copyRandomBinaryTree(nullptr));
    ctx.expect_eq(guard.get(), nullptr);
  });

  ctx.test("single_node_self_random_cycle", [&] {
    std::deque<Node> arena;
    Node& only = arena.emplace_back(42);
    only.random = &only;

    Solution sol;
    ClonedTreeGuard guard(sol.copyRandomBinaryTree(&only));
    ctx.expect_true(verify_clone(&only, guard.get()));
    const auto* cloned = guard.get();
    ctx.expect_true(cloned != nullptr);
    if (cloned != nullptr) {
      ctx.expect_eq(cloned->random, cloned);
    }
  });

  ctx.test("mutual_random_cycle_and_cross_pointers", [&] {
    std::deque<Node> arena;
    Node& n1 = arena.emplace_back(1);
    Node& n2 = arena.emplace_back(2);
    Node& n3 = arena.emplace_back(3);
    Node& n4 = arena.emplace_back(4);
    Node& n5 = arena.emplace_back(5);

    n1.left = &n2;
    n1.right = &n3;
    n2.left = &n4;
    n3.right = &n5;

    // Create a self-loop, a 2-cycle between siblings, and a cross-subtree link.
    n1.random = &n1;
    n2.random = &n3;
    n3.random = &n2;
    n4.random = &n5;
    n5.random = &n1;

    Solution sol;
    ClonedTreeGuard guard(sol.copyRandomBinaryTree(&n1));
    ctx.expect_true(verify_clone(&n1, guard.get()));
  });

  ctx.test("no_random_pointers", [&] {
    std::deque<Node> arena;
    Node& n1 = arena.emplace_back(10);
    Node& n2 = arena.emplace_back(20);
    Node& n3 = arena.emplace_back(30);
    n1.left = &n2;
    n1.right = &n3;

    Solution sol;
    ClonedTreeGuard guard(sol.copyRandomBinaryTree(&n1));
    ctx.expect_true(verify_clone(&n1, guard.get()));
  });
}

}  // namespace
