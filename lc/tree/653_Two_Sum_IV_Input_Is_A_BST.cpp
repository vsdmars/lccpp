// LeetCode 653: Two Sum IV Input Is A BST
// https://leetcode.com/problems/two-sum-iv-input-is-a-bst/
//
// Problem:
//   Given the root of a Binary Search Tree (BST) and an integer target `k`,
//   return `true` if there exist two distinct nodes in the BST whose values
//   sum to `k`, and `false` otherwise. Node count is in [1, 10^4], node values
//   are in [-10^4, 10^4], and `k` is in [-10^5, 10^5].
//
// Examples:
//   Example 1:
//     Input:  root = [5, 3, 6, 2, 4, null, 7], k = 9
//     Output: true
//     Explanation:
//       - In-order DFS in findTarget starts with an empty seen set and reaches the
//         leftmost node 2, where 9 - 2 = 7 is not in seen, so it inserts 2 (seen =
//         {2}).
//       - Visiting 3 checks 9 - 3 = 6 (absent, seen = {2, 3}), and visiting its
//         right child 4 checks 9 - 4 = 5 (absent, seen = {2, 3, 4}).
//       - Returning to root 5, seen.contains(9 - 5) finds 4 already in seen, so
//         findTarget immediately returns true without traversing the right subtree.
//
//   Example 2:
//     Input:  root = [5, 3, 6, 2, 4, null, 7], k = 28
//     Output: false
//     Explanation:
//       - findTargetTwoPointers initializes left_it at minimum node 2 (stack [5, 3,
//         2]) and right_it at maximum node 7 (stack [5, 6, 7]).
//       - Because 2 + 7 = 9 < 28, left_it advances sequentially through 3 (sum 10),
//         4 (sum 11), 5 (sum 12), and 6 (sum 13), each time staying below 28.
//       - Advancing left_it from 6 pushes its right child 7, making left_it.peek()
//         == right_it.peek() at node 7 so the loop exits and returns false.
//
//   Example 3:
//     Input:  root = [2], k = 4
//     Output: false
//     Explanation:
//       - In findTarget, the in-order DFS visits node 2 with an empty seen set and
//         checks seen.contains(4 - 2) before inserting 2.
//       - Because 2 is not yet in seen, it inserts 2 and recurses into the null
//         right child, preventing node 2 from being paired with itself.
//       - In findTargetTwoPointers, both left_it and right_it peek at the same node
//         2, so left_it.peek() != right_it.peek() is immediately false and returns
//         false.
//
// Algorithm:
//   1. DFS with complement hash set (`findTarget`):
//      Perform an in-order DFS while storing visited node values in a hash set.
//      At each node, check whether `k - node->val` has already been seen; if
//      so, immediately return `true` without visiting the remaining subtree.
//      Because we check before inserting `node->val`, a single node can never
//      be paired with itself.
//      Time: O(N), Space: O(N) for the hash set plus O(H) recursion stack.
//   2. Controlled two-pointer BST iterators (`findTargetTwoPointers`):
//      Exploit the BST ordering property directly by running two lazy in-order
//      iterators simultaneously—one ascending from the minimum element and one
//      descending from the maximum element. Compare their sum to `k` and
//      advance the appropriate pointer until they match or meet at the same
//      node.
//      Time: O(N), Space: O(H) where H is the height of the BST.
//
// Data Structures:
//   - `std::unordered_set<std::int64_t>`: O(1) average complement lookup in the
//     hash-set DFS solution.
//   - Two explicit iterator stacks (`std::vector<const TreeNode*>`): store only
//     the root-to-current path (depth H) for O(H)-space two-pointer scanning.
//
// Real-World Applications:
//   - Order-book matching and financial trade pairing across sorted price trees
//     (finding complementary bid/ask amounts summing to a target notional).
//   - Database index range-join / complement queries executed directly over
//     in-memory B-tree / red-black tree indices without materializing a flat
//     sorted array.
//   - Budget or capacity allocation checks over hierarchically indexed resource
//     pools.
//
// Related LeetCode Problems:
//   - #1 Two Sum [done] (classic unsorted array complement lookup with hash map)
//   - #167 Two Sum II [done] - Input Array Is Sorted (two-pointer scan on sorted order)
//   - #173 Binary Search Tree Iterator [done] (lazy O(H)-space BST stack iterator)
//   - #1214 Two Sum BSTs [new] (finding a target pair across two separate BSTs)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <deque>
#include <optional>
#include <queue>
#include <unordered_set>
#include <vector>

namespace {

// Standard LeetCode binary tree node definition.
struct TreeNode {
  int val = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;

  explicit TreeNode(int v, TreeNode* l = nullptr, TreeNode* r = nullptr)
      : val(v), left(l), right(r) {}
};

// Lazy O(H)-space BST iterator supporting both ascending (in-order) and
// descending (reverse in-order) traversal.
class BstIterator {
 public:
  BstIterator(const TreeNode* root, bool reverse) : reverse_(reverse) {
    push_branch(root);
  }

  // Returns the current smallest (or largest if `reverse_`) unconsumed node.
  [[nodiscard]] const TreeNode* peek() const { return stack_.back(); }

  // Advances to the next node in sorted (or reverse-sorted) order.
  void advance() {
    const TreeNode* node = stack_.back();
    stack_.pop_back();
    push_branch(reverse_ ? node->left : node->right);
  }

 private:
  // Descends all the way down the left (or right if reversed) spine.
  void push_branch(const TreeNode* node) {
    while (node != nullptr) {
      stack_.push_back(node);
      node = reverse_ ? node->right : node->left;
    }
  }

  bool reverse_ = false;
  std::vector<const TreeNode*> stack_;
};

class Solution {
 public:
  // Checks whether two distinct nodes sum to `k` using DFS + hash set.
  // Time: O(N), Space: O(N).
  bool findTarget(const TreeNode* root, int k) {
    if (root == nullptr) {
      return false;
    }

    // Use 64-bit integers so `k - node->val` never overflows signed 32-bit int.
    std::unordered_set<std::int64_t> seen;
    const auto target = static_cast<std::int64_t>(k);

    auto dfs = [&](this auto&& self, const TreeNode* node) -> bool {
      if (node == nullptr) {
        return false;
      }

      if (self(node->left)) {
        return true;
      }

      // Check complement before inserting `node->val` to ensure distinct nodes.
      const auto val = static_cast<std::int64_t>(node->val);
      if (seen.contains(target - val)) {
        return true;
      }
      seen.insert(val);

      return self(node->right);
    };

    return dfs(root);
  }

  // Space-optimal BST solution using converging ascending/descending iterators.
  // Time: O(N), Space: O(H) where H is the height of the BST.
  bool findTargetTwoPointers(const TreeNode* root, int k) {
    if (root == nullptr) {
      return false;
    }

    BstIterator left_it(root, /*reverse=*/false);
    BstIterator right_it(root, /*reverse=*/true);
    const auto target = static_cast<std::int64_t>(k);

    // Comparing node pointers ensures two distinct nodes and stops the moment
    // the two iterators converge at the same node.
    while (left_it.peek() != right_it.peek()) {
      const std::int64_t sum = static_cast<std::int64_t>(left_it.peek()->val) +
                               static_cast<std::int64_t>(right_it.peek()->val);
      if (sum == target) {
        return true;
      }
      if (sum < target) {
        left_it.advance();
      } else {
        right_it.advance();
      }
    }

    return false;
  }
};

// RAII arena that owns all test nodes with zero manual `delete`.
class TreeArena {
 public:
  TreeNode* build(const std::vector<std::optional<int>>& vals) {
    if (vals.empty() || !vals[0].has_value()) {
      return nullptr;
    }

    TreeNode* root = create(*vals[0]);
    std::queue<TreeNode*> q;
    q.push(root);

    std::size_t idx = 1;
    while (!q.empty() && idx < vals.size()) {
      TreeNode* curr = q.front();
      q.pop();

      if (vals[idx].has_value()) {
        curr->left = create(*vals[idx]);
        q.push(curr->left);
      }
      ++idx;

      if (idx < vals.size() && vals[idx].has_value()) {
        curr->right = create(*vals[idx]);
        q.push(curr->right);
      }
      ++idx;
    }
    return root;
  }

 private:
  TreeNode* create(int val) {
    nodes_.emplace_back(val);
    return &nodes_.back();
  }

  std::deque<TreeNode> nodes_;
};

LC_PROBLEM(653, "Two Sum IV Input Is A BST", "tree", ctx) {
  Solution sol;

  ctx.test("example_1_found", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({5, 3, 6, 2, 4, std::nullopt, 7});
    ctx.expect_true(sol.findTarget(root, 9));
    ctx.expect_true(sol.findTargetTwoPointers(root, 9));
  });

  ctx.test("example_2_not_found", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({5, 3, 6, 2, 4, std::nullopt, 7});
    ctx.expect_false(sol.findTarget(root, 28));
    ctx.expect_false(sol.findTargetTwoPointers(root, 28));
  });

  ctx.test("python_build_case", [&] {
    // BST from the original Python `build()`: [9, 3, 20, 2, null, 15, 22], k=12.
    TreeArena arena;
    TreeNode* root = arena.build({9, 3, 20, 2, std::nullopt, 15, 22});
    ctx.expect_true(sol.findTarget(root, 12));
    ctx.expect_true(sol.findTargetTwoPointers(root, 12));
  });

  ctx.test("cannot_reuse_same_node", [&] {
    TreeArena arena;
    TreeNode* single = arena.build({2});
    ctx.expect_false(sol.findTarget(single, 4));
    ctx.expect_false(sol.findTargetTwoPointers(single, 4));

    // With 1 and 3 present, 1 + 3 == 4 is valid without reusing 2.
    TreeNode* triple = arena.build({2, 1, 3});
    ctx.expect_true(sol.findTarget(triple, 4));
    ctx.expect_true(sol.findTargetTwoPointers(triple, 4));
    ctx.expect_false(sol.findTarget(triple, 6));
    ctx.expect_false(sol.findTargetTwoPointers(triple, 6));
  });

  ctx.test("negative_values_and_empty_tree", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({-5, -10, 0, -15, -7});
    ctx.expect_true(sol.findTarget(root, -15));
    ctx.expect_true(sol.findTargetTwoPointers(root, -15));
    ctx.expect_false(sol.findTarget(root, -30));
    ctx.expect_false(sol.findTargetTwoPointers(root, -30));

    ctx.expect_false(sol.findTarget(nullptr, 0));
    ctx.expect_false(sol.findTargetTwoPointers(nullptr, 0));
  });
}

}  // namespace
