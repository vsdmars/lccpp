// LeetCode 919: Complete Binary Tree Inserter
// https://leetcode.com/problems/complete-binary-tree-inserter/
//
// Problem:
//   A complete binary tree is a binary tree in which every level except
//   possibly the last is completely filled, and all nodes in the last level
//   are as far left as possible. Design a `CBTInserter` class supporting:
//     - `CBTInserter(TreeNode* root)`: initializes with a complete binary tree.
//     - `int insert(int val)`: inserts a new node with value `val` at the next
//       leftmost vacant position keeping the tree complete, and returns the
//       parent node's value (or -1 if the tree was initially empty).
//     - `TreeNode* get_root() const`: returns the root of the tree.
//
// Examples:
//   Example 1:
//     Input:  CBTInserter([1, 2]), insert(3), insert(4), get_root()
//     Output: insert(3) -> 1, insert(4) -> 2, get_root() -> [1, 2, 3, 4]
//     Explanation:
//       - Constructor BFS visits 1 (missing right child) and 2 (missing both
//         children), initializing incomplete_parents_ = [1, 2].
//       - insert(3) inspects front parent 1, attaches 3 to 1->right, pops full node
//         1, pushes 3 to incomplete_parents_ ([2, 3]), and returns 1.
//       - insert(4) inspects front parent 2, attaches 4 to 2->left (keeping 2 at
//         front because 2->right is null), pushes 4 ([2, 3, 4]), and returns 2.
//       - get_root() returns node 1, whose level-order traversal is [1, 2, 3, 4].
//
//   Example 2:
//     Input:  CBTInserter([10, 20, 30]), insert(40), insert(50), insert(60),
//             get_root()
//     Output: insert(40) -> 20, insert(50) -> 20, insert(60) -> 30, get_root() ->
//             [10, 20, 30, 40, 50, 60]
//     Explanation:
//       - Constructor BFS skips full root 10 and enqueues incomplete nodes 20 and
//         30 into incomplete_parents_ = [20, 30].
//       - insert(40) attaches 40 as 20->left, keeps 20 at the front, pushes 40
//         ([20, 30, 40]), and returns 20.
//       - insert(50) attaches 50 as 20->right, pops full node 20, pushes 50 ([30,
//         40, 50]), and returns 20.
//       - insert(60) attaches 60 as 30->left and returns 30, producing level-order
//         [10, 20, 30, 40, 50, 60].
//
//   Example 3:
//     Input:  CBTInserter([]), get_root(), insert(3), insert(4), insert(5),
//             insert(6), get_root()
//     Output: get_root() -> nullptr, insert(3) -> -1, insert(4) -> 3, insert(5) ->
//             3, insert(6) -> 4, get_root() -> [3, 4, 5, 6]
//     Explanation:
//       - Constructor sees root_ == nullptr and leaves incomplete_parents_ empty,
//         so the initial get_root() returns nullptr.
//       - insert(3) sees root_ == nullptr, sets root_ to node 3, pushes 3 onto
//         incomplete_parents_ ([3]), and returns -1.
//       - insert(4) and insert(5) attach 4 to 3->left and 5 to 3->right, popping 3
//         and leaving incomplete_parents_ = [4, 5], each returning 3.
//       - insert(6) attaches 6 to 4->left and returns 4, leaving get_root() with
//         level-order [3, 4, 5, 6].
//
// Algorithm:
//   During construction, run a BFS level-order traversal over the initial tree
//   and enqueue every node that has at least one missing child (`left ==
//   nullptr || right == nullptr`) into a FIFO deque `incomplete_parents_`.
//
//   Because BFS visits levels left-to-right, `incomplete_parents_.front()` is
//   always the exact next parent awaiting a child:
//     - On `insert(val)`:
//       1. Allocate the new `TreeNode` inside an internal `std::deque<TreeNode>`
//          arena (ensuring pointer stability and leak-free RAII cleanup).
//       2. If the tree is empty, make the new node the root, push it into
//          `incomplete_parents_`, and return -1.
//       3. Otherwise, inspect `parent = incomplete_parents_.front()`:
//          - If `parent->left == nullptr`, attach as `parent->left` (keep
//            `parent` at the front because its right slot is still vacant).
//          - Else attach as `parent->right` and pop `parent` (now full).
//       4. Push the newly inserted node to the back of `incomplete_parents_`
//          (both its child slots are vacant) and return `parent->val`.
//   Time Complexity:  O(n) for constructor; O(1) amortized/worst-case for
//                     `insert` and `get_root`.
//   Space Complexity: O(n) for the frontier queue and newly inserted nodes.
//
// Data Structures:
//   - FIFO queue (`std::deque<TreeNode*>`) maintaining nodes with < 2 children
//     in strict level-order.
//   - Address-stable arena (`std::deque<TreeNode>`) owning all nodes created
//     by `insert()` so raw `TreeNode*` links never dangle or leak.
//
// Real-World Applications:
//   - Streaming binary heap / priority-queue implementations built on pointer
//     trees where new elements must append at the leftmost open leaf slot.
//   - Peer-to-peer broadcast / multicast overlay trees that grow incrementally
//     while preserving minimum balanced height and bounded fan-out.
//   - Merkle tree construction from append-only logs requiring deterministic
//     left-to-right leaf placement.
//
// Related LeetCode Problems:
//   - #958 Check Completeness of a Binary Tree [done] (verifying the complete binary tree invariant via BFS)
//   - #222 Count Complete Tree Nodes [done] (exploiting complete binary tree geometry)
//   - #102 Binary Tree Level Order Traversal [done] (BFS level-by-level tree traversal)
//   - #297 Serialize and Deserialize Binary Tree [done] (BFS tree construction and representation)

#include "framework/lc_framework.h"

#include <deque>
#include <queue>
#include <vector>

namespace {

// Standard binary tree node.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int v, TreeNode* l = nullptr, TreeNode* r = nullptr)
      : val(v), left(l), right(r) {}
};

// RAII arena owning initial test TreeNode allocations.
class TreeArena {
 public:
  TreeNode* make(int val, TreeNode* left = nullptr, TreeNode* right = nullptr) {
    return &nodes_.emplace_back(val, left, right);
  }

 private:
  std::deque<TreeNode> nodes_;
};

// Serializes a complete binary tree into level-order values for test checks.
std::vector<int> level_order_values(const TreeNode* root) {
  std::vector<int> values;
  if (root == nullptr) {
    return values;
  }
  std::queue<const TreeNode*> q;
  q.push(root);
  while (!q.empty()) {
    const TreeNode* curr = q.front();
    q.pop();
    values.push_back(curr->val);
    if (curr->left != nullptr) {
      q.push(curr->left);
    }
    if (curr->right != nullptr) {
      q.push(curr->right);
    }
  }
  return values;
}

// Complete Binary Tree Inserter supporting O(1) insertions.
class CBTInserter {
 public:
  // Initializes the frontier queue of incomplete parents via level-order BFS.
  // Time: O(n), Space: O(n).
  explicit CBTInserter(TreeNode* root) : root_(root) {
    if (root_ == nullptr) {
      return;
    }

    std::queue<TreeNode*> bfs;
    bfs.push(root_);

    while (!bfs.empty()) {
      TreeNode* curr = bfs.front();
      bfs.pop();

      // Any node missing a left or right child is a candidate parent for
      // upcoming insertions, in strict left-to-right level order.
      if (curr->left == nullptr || curr->right == nullptr) {
        incomplete_parents_.push_back(curr);
      }
      if (curr->left != nullptr) {
        bfs.push(curr->left);
      }
      if (curr->right != nullptr) {
        bfs.push(curr->right);
      }
    }
  }

  // Non-copyable because tree pointers reference elements inside `owned_nodes_`.
  CBTInserter(const CBTInserter&) = delete;
  CBTInserter& operator=(const CBTInserter&) = delete;
  CBTInserter(CBTInserter&&) = delete;
  CBTInserter& operator=(CBTInserter&&) = delete;
  ~CBTInserter() = default;

  // Inserts a new node with value `val` at the next complete-tree slot and
  // returns its parent's value (or -1 if the tree was initially empty).
  // Time: O(1), Space: O(1) extra per call.
  int insert(int val) {
    // std::deque::emplace_back guarantees pointer stability for existing nodes.
    TreeNode* node = &owned_nodes_.emplace_back(val);

    if (root_ == nullptr) {
      root_ = node;
      incomplete_parents_.push_back(node);
      return -1;
    }

    TreeNode* parent = incomplete_parents_.front();
    if (parent->left == nullptr) {
      parent->left = node;
    } else {
      parent->right = node;
      // Both children are now populated; remove `parent` from the frontier.
      incomplete_parents_.pop_front();
    }

    // The newly inserted leaf has two vacant child slots for future levels.
    incomplete_parents_.push_back(node);
    return parent->val;
  }

  // Returns the root pointer of the complete binary tree.
  // Time: O(1), Space: O(1).
  [[nodiscard]] TreeNode* get_root() const noexcept { return root_; }

 private:
  TreeNode* root_ = nullptr;
  std::deque<TreeNode*> incomplete_parents_;
  std::deque<TreeNode> owned_nodes_;
};

LC_PROBLEM(919, "Complete Binary Tree Inserter", "tree", ctx) {
  ctx.test("example1_insert_two_nodes", [&] {
    // Initial: [1, 2]
    TreeArena arena;
    TreeNode* root = arena.make(1, arena.make(2), nullptr);

    CBTInserter inserter(root);
    ctx.expect_eq(inserter.insert(3), 1);
    ctx.expect_eq(inserter.insert(4), 2);
    ctx.expect_eq(inserter.get_root(), root);
    ctx.expect_eq(level_order_values(inserter.get_root()),
                  std::vector<int>({1, 2, 3, 4}));
  });

  ctx.test("multi_level_growth_from_single_root", [&] {
    TreeArena arena;
    TreeNode* root = arena.make(1);

    CBTInserter inserter(root);
    ctx.expect_eq(inserter.insert(2), 1);
    ctx.expect_eq(inserter.insert(3), 1);
    ctx.expect_eq(inserter.insert(4), 2);
    ctx.expect_eq(inserter.insert(5), 2);
    ctx.expect_eq(inserter.insert(6), 3);
    ctx.expect_eq(inserter.insert(7), 3);
    ctx.expect_eq(inserter.insert(8), 4);
    ctx.expect_eq(level_order_values(inserter.get_root()),
                  std::vector<int>({1, 2, 3, 4, 5, 6, 7, 8}));
  });

  ctx.test("initial_full_tree_starts_new_level", [&] {
    // Initial: [10, 20, 30]
    TreeArena arena;
    TreeNode* root = arena.make(10, arena.make(20), arena.make(30));

    CBTInserter inserter(root);
    ctx.expect_eq(inserter.insert(40), 20);
    ctx.expect_eq(inserter.insert(50), 20);
    ctx.expect_eq(inserter.insert(60), 30);
    ctx.expect_eq(level_order_values(inserter.get_root()),
                  std::vector<int>({10, 20, 30, 40, 50, 60}));
  });

  ctx.test("empty_initial_tree_builds_from_scratch", [&] {
    CBTInserter inserter(nullptr);
    ctx.expect_eq(inserter.get_root(), nullptr);
    ctx.expect_eq(inserter.insert(3), -1);
    ctx.expect_eq(inserter.insert(4), 3);
    ctx.expect_eq(inserter.insert(5), 3);
    ctx.expect_eq(inserter.insert(6), 4);
    ctx.expect_ne(inserter.get_root(), nullptr);
    ctx.expect_eq(level_order_values(inserter.get_root()),
                  std::vector<int>({3, 4, 5, 6}));
  });
}

}  // namespace
