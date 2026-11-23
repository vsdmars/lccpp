// LeetCode 559: Maximum Depth Of N Ary Tree
// https://leetcode.com/problems/maximum-depth-of-n-ary-tree/
//
// Problem:
//   Given the root of an n-ary tree (where each node stores a value and a list
//   of zero or more child pointers), return its maximum depth — the number of
//   nodes along the longest downward path from the root node to any leaf node.
//   An empty tree (`nullptr`) has depth 0.
//
// Examples:
//   Example 1:
//     Input:  root = [1, null, 3, 2, 4, null, 5, 6]
//     Output: 3
//     Explanation:
//       - maxDepth(1) initializes max_child_depth = 0 and iterates over children
//         [3, 2, 4].
//       - For node 3, recursive calls on leaves 5 and 6 each have empty children
//         and return 1, so maxDepth(3) returns 1 + max(1, 1) = 2.
//       - Leaf nodes 2 and 4 each have no children, returning maxDepth(2) = 1 and
//         maxDepth(4) = 1.
//       - At root 1, max_child_depth becomes max(2, 1, 1) = 2, so maxDepth returns
//         1 + 2 = 3.
//
//   Example 2:
//     Input:  root = [1, null, 2, 3, 4, 5, null, null, 6, 7, null, 8, null, 9, 10,
//             null, null, 11, null, 12, null, 13, null, null, 14]
//     Output: 5
//     Explanation:
//       - maxDepth(1) evaluates children [2, 3, 4, 5], with leaf 2 returning 1
//         immediately.
//       - Under node 3, leaf 6 returns 1 while the chain 7 -> 11 -> 14 unwinds as
//         maxDepth(14) = 1, maxDepth(11) = 2, and maxDepth(7) = 3, giving
//         maxDepth(3) = 1 + max(1, 3) = 4.
//       - Subtree 4 -> 8 -> 12 returns 3, and subtree 5 -> {9 -> 13, 10} returns 1
//         + max(2, 1) = 3.
//       - At root 1, max_child_depth is max(1, 4, 3, 3) = 4, yielding final depth 1
//         + 4 = 5.
//
//   Example 3:
//     Input:  root = [1, null, 2, null, 3, null, 4]
//     Output: 4
//     Explanation:
//       - maxDepth(1) follows the single-child chain 1 -> 2 -> 3 -> 4 down to leaf
//         4.
//       - Because node 4 has empty children, max_child_depth stays 0 and
//         maxDepth(4) returns 1.
//       - Each ancestor adds 1 on return: maxDepth(3) = 2, maxDepth(2) = 3, and
//         maxDepth(1) = 4.
//
// Algorithm:
//   Use postorder recursive DFS: the depth of a null node is 0, and the depth
//   of any non-null node is `1 + max(maxDepth(child) for child in children)`
//   (evaluating to `1` when the node is a leaf with no children). Because each
//   edge in the tree is traversed once, the algorithm visits every node
//   exactly once.
//   Time Complexity:  O(N), where N is the total number of nodes in the tree.
//   Space Complexity: O(H) for the recursion stack, where H is the tree height
//                     (O(N) worst case for a skewed chain, O(log_k N) for a
//                     balanced k-ary tree).
//
// Data Structures:
//   - N-ary tree (`Node` with `std::vector<Node*> children`): traversed via
//     recursive DFS.
//   - Call stack: maintains the active root-to-node path up to depth H.
//
// Real-World Applications:
//   - Filesystem & directory tree inspection: computing maximum nesting depth
//     of directories to enforce path-length or traversal-depth limits.
//   - DOM / XML / JSON schema validation: measuring document nesting depth to
//     guard parsers against stack-overflow denial-of-service payloads.
//   - Organizational & taxonomy hierarchies: determining the longest chain of
//     command or category inheritance depth.
//
// Related LeetCode Problems:
//   - #104 Maximum Depth of Binary Tree [done] (binary-tree special case)
//   - #111 Minimum Depth of Binary Tree [done] (shortest root-to-leaf path)
//   - #429 N-ary Tree Level Order Traversal [new] (BFS over n-ary trees)
//   - #589 N-ary Tree Preorder Traversal [new] (preorder DFS on n-ary trees)
//   - #1522 Diameter of N-Ary Tree [done] (longest path between any two nodes)

#include "framework/lc_framework.h"

#include <algorithm>
#include <deque>
#include <utility>
#include <vector>

namespace {

// Standard n-ary tree node used by LeetCode n-ary tree problems.
struct Node {
  int val = 0;
  std::vector<Node*> children;

  Node() = default;
  explicit Node(int v) : val(v) {}
  Node(int v, std::vector<Node*> ch) : val(v), children(std::move(ch)) {}
};

// Pointer-stable RAII arena for allocating n-ary tree nodes in unit tests.
class NaryArena {
 public:
  Node* make(int val, std::vector<Node*> children = {}) {
    nodes_.emplace_back(val, std::move(children));
    return &nodes_.back();
  }

 private:
  std::deque<Node> nodes_;
};

class Solution {
 public:
  // Returns the number of nodes on the longest root-to-leaf path in `root`.
  // Time: O(N) | Space: O(H) recursion stack.
  int maxDepth(const Node* root) {
    if (root == nullptr) {
      return 0;
    }

    // Recurse on all children; if `root` is a leaf, `max_child_depth` stays 0
    // and this node contributes depth 1.
    int max_child_depth = 0;
    for (const Node* child : root->children) {
      max_child_depth = std::max(max_child_depth, maxDepth(child));
    }
    return 1 + max_child_depth;
  }
};

LC_PROBLEM(559, "Maximum Depth Of N Ary Tree", "dfs", ctx) {
  ctx.test("example_1_from_python_build", [&] {
    NaryArena arena;
    // [1, null, 3, 2, 4, null, 5, 6]
    Node* n5 = arena.make(5);
    Node* n6 = arena.make(6);
    Node* n3 = arena.make(3, {n5, n6});
    Node* n2 = arena.make(2);
    Node* n4 = arena.make(4);
    Node* root = arena.make(1, {n3, n2, n4});

    Solution sol;
    ctx.expect_eq(sol.maxDepth(root), 3);
  });

  ctx.test("example_2_deeper_nary_tree", [&] {
    NaryArena arena;
    // [1,null,2,3,4,5,null,null,6,7,null,8,null,9,10,null,null,11,null,12,null,13,null,null,14]
    Node* n14 = arena.make(14);
    Node* n11 = arena.make(11, {n14});
    Node* n7 = arena.make(7, {n11});
    Node* n6 = arena.make(6);
    Node* n3 = arena.make(3, {n6, n7});

    Node* n12 = arena.make(12);
    Node* n8 = arena.make(8, {n12});
    Node* n4 = arena.make(4, {n8});

    Node* n13 = arena.make(13);
    Node* n9 = arena.make(9, {n13});
    Node* n10 = arena.make(10);
    Node* n5 = arena.make(5, {n9, n10});

    Node* n2 = arena.make(2);
    Node* root = arena.make(1, {n2, n3, n4, n5});

    Solution sol;
    ctx.expect_eq(sol.maxDepth(root), 5);
  });

  ctx.test("empty_and_single_node", [&] {
    NaryArena arena;
    Solution sol;
    ctx.expect_eq(sol.maxDepth(nullptr), 0);

    const Node* single = arena.make(42);
    ctx.expect_eq(sol.maxDepth(single), 1);
  });

  ctx.test("skewed_chain_and_wide_tree", [&] {
    NaryArena arena;
    Node* leaf = arena.make(4);
    Node* mid2 = arena.make(3, {leaf});
    Node* mid1 = arena.make(2, {mid2});
    Node* chain_root = arena.make(1, {mid1});

    std::vector<Node*> wide_children;
    wide_children.reserve(10uz);
    for (int i = 0; i < 10; ++i) {
      wide_children.push_back(arena.make(i));
    }
    const Node* wide_root = arena.make(100, std::move(wide_children));

    Solution sol;
    ctx.expect_eq(sol.maxDepth(chain_root), 4);
    ctx.expect_eq(sol.maxDepth(wide_root), 2);
  });
}

}  // namespace
