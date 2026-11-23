// LeetCode 652: Find Duplicate Subtrees
// https://leetcode.com/problems/find-duplicate-subtrees/
//
// Problem:
//   Given the root of a binary tree, find all duplicate subtrees. Two subtrees
//   are duplicates if they have the exact same structure and identical node
//   values at every corresponding position. For each distinct kind of
//   duplicate subtree, return the root node of any one occurrence (in any
//   order). Tree size is in [1, 5000] and values are in [-200, 200].
//
// Algorithm:
//   1. Delimited post-order serialization (`findDuplicateSubtrees`):
//      Recursively serialize every subtree into an unambiguous preorder string
//      `"<val>,<left_sig>,<right_sig>"` with `"#"` representing null pointers.
//      (Using an explicit comma delimiter fixes the collision bug where `"1"` +
//      `"23"` would otherwise collide with `"12"` + `"3"`.) Count occurrences
//      in a hash map and record `node` into the result list strictly when its
//      signature count hits 2, ensuring each duplicate structure is reported
//      once.
//      Time: O(N^2) worst-case (concatenating signatures on skewed trees).
//      Space: O(N^2) worst-case for stored serialized strings.
//   2. Linear-time subtree ID interning (`findDuplicateSubtreesLinear`):
//      Instead of concatenating full strings, assign every unique structural
//      triplet `(node->val, left_id, right_id)` a compact integer ID (with `0`
//      for null). Each subtree's canonical signature is therefore a fixed-size
//      key of three integers, reducing hashing and comparison to O(1).
//      Time: O(N), Space: O(N).
//
// Data Structures:
//   - `std::unordered_map<std::string, int>`: maps serialized subtree strings
//     to occurrence counts in the string-encoding approach.
//   - `std::unordered_map<SubtreeKey, int, SubtreeKeyHash>` + `std::vector<int>`:
//     interns `(val, left_id, right_id)` triplets to unique integer IDs and
//     tracks frequency per ID in O(1) amortized time.
//
// Real-World Applications:
//   - Common Subexpression Elimination (CSE) and hash-consing in compiler AST
//     and IR optimization passes (collapsing identical expression DAGs).
//   - Content-addressed deduplication in Merkle trees, Git object stores, and
//     distributed file systems (identifying identical directory subtrees).
//   - Structural deduplication in XML/JSON/DOM diffing and binary decision
//     diagram (BDD) reduction.
//
// Related LeetCode Problems:
//   - #297 Serialize and Deserialize Binary Tree [done] (unambiguous tree encoding)
//   - #100 Same Tree [done] (structural equality check between two trees)
//   - #572 Subtree of Another Tree [done] (subtree matching inside a larger tree)
//   - #606 Construct String from Binary Tree [done] (preorder tree serialization)
//   - #1948 Delete Duplicate Folders in System [new] (hash-consing duplicate n-ary subtrees)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <optional>
#include <queue>
#include <string>
#include <unordered_map>
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

// Fixed-size structural signature `(val, left_id, right_id)` for O(N) hash-consing.
struct SubtreeKey {
  int val = 0;
  int left_id = 0;
  int right_id = 0;

  bool operator==(const SubtreeKey&) const = default;
};

// Custom hash combiner for `SubtreeKey`.
struct SubtreeKeyHash {
  std::size_t operator()(const SubtreeKey& key) const noexcept {
    // Pack and mix the three 32-bit integers using splitmix-style avalanche.
    auto h = static_cast<std::uint64_t>(static_cast<std::uint32_t>(key.val));
    h = (h * 0x9e3779b97f4a7c15ULL) ^
        static_cast<std::uint64_t>(static_cast<std::uint32_t>(key.left_id));
    h = (h * 0xbf58476d1ce4e5b9ULL) ^
        static_cast<std::uint64_t>(static_cast<std::uint32_t>(key.right_id));
    h ^= (h >> 30U);
    return static_cast<std::size_t>(h);
  }
};

class Solution {
 public:
  // Finds one root per duplicate subtree using delimited preorder serialization.
  // Time: O(N^2) worst-case, Space: O(N^2) worst-case.
  std::vector<TreeNode*> findDuplicateSubtrees(TreeNode* root) {
    std::unordered_map<std::string, int> counts;
    std::vector<TreeNode*> duplicates;

    auto encode = [&](this auto&& self, TreeNode* node) -> std::string {
      if (node == nullptr) {
        return "#";
      }

      // Comma delimiters guarantee unambiguous boundaries between multi-digit
      // and negative node values.
      std::string sig = std::to_string(node->val);
      sig.push_back(',');
      sig += self(node->left);
      sig.push_back(',');
      sig += self(node->right);

      // Push only on the second occurrence so every duplicate kind appears once.
      if (++counts[sig] == 2) {
        duplicates.push_back(node);
      }
      return sig;
    };

    (void)encode(root);
    return duplicates;
  }

  // Optimal O(N) version using bottom-up integer ID interning (hash-consing).
  // Time: O(N), Space: O(N).
  std::vector<TreeNode*> findDuplicateSubtreesLinear(TreeNode* root) {
    std::unordered_map<SubtreeKey, int, SubtreeKeyHash> key_to_id;
    // Index 0 is reserved for the null subtree; assigned IDs start at 1.
    std::vector<int> id_counts = {0};
    std::vector<TreeNode*> duplicates;

    auto intern = [&](this auto&& self, TreeNode* node) -> int {
      if (node == nullptr) {
        return 0;
      }

      const int left_id = self(node->left);
      const int right_id = self(node->right);
      const SubtreeKey key{.val = node->val, .left_id = left_id, .right_id = right_id};

      // Lookup or mint a fresh integer ID for this canonical subtree structure.
      auto [it, inserted] = key_to_id.emplace(key, static_cast<int>(id_counts.size()));
      if (inserted) {
        id_counts.push_back(0);
      }

      const int id = it->second;
      if (++id_counts[static_cast<std::size_t>(id)] == 2) {
        duplicates.push_back(node);
      }
      return id;
    };

    (void)intern(root);
    return duplicates;
  }
};

// RAII arena that owns all test nodes cleanly without manual `delete`.
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

// Serializes a binary tree into a canonical string for order-independent comparison.
std::string canonical_preorder(const TreeNode* root) {
  if (root == nullptr) {
    return "#";
  }
  return std::to_string(root->val) + "," + canonical_preorder(root->left) + "," +
         canonical_preorder(root->right);
}

// Converts a returned vector of duplicate subtree roots into sorted canonical strings.
std::vector<std::string> sorted_signatures(const std::vector<TreeNode*>& roots) {
  std::vector<std::string> sigs;
  sigs.reserve(roots.size());
  for (const TreeNode* node : roots) {
    sigs.push_back(canonical_preorder(node));
  }
  std::ranges::sort(sigs);
  return sigs;
}

LC_PROBLEM(652, "Find Duplicate Subtrees", "tree", ctx) {
  Solution sol;

  ctx.test("example_1_nested_duplicates", [&] {
    TreeArena arena;
    TreeNode* root =
        arena.build({1, 2, 3, 4, std::nullopt, 2, 4, std::nullopt, std::nullopt, 4});
    // Expected duplicate subtrees: [2, 4] -> "2,4,#,#,#" and [4] -> "4,#,#".
    std::vector<std::string> expected = {"2,4,#,#,#", "4,#,#"};
    std::ranges::sort(expected);

    ctx.expect_eq(sorted_signatures(sol.findDuplicateSubtrees(root)), expected);
    ctx.expect_eq(sorted_signatures(sol.findDuplicateSubtreesLinear(root)), expected);
  });

  ctx.test("example_2_identical_leaves", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({2, 1, 1});
    const std::vector<std::string> expected = {"1,#,#"};

    ctx.expect_eq(sorted_signatures(sol.findDuplicateSubtrees(root)), expected);
    ctx.expect_eq(sorted_signatures(sol.findDuplicateSubtreesLinear(root)), expected);
  });

  ctx.test("example_3_shared_subtrees_with_equal_values", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({2, 2, 2, 3, std::nullopt, 3, std::nullopt});
    std::vector<std::string> expected = {"2,3,#,#,#", "3,#,#"};
    std::ranges::sort(expected);

    ctx.expect_eq(sorted_signatures(sol.findDuplicateSubtrees(root)), expected);
    ctx.expect_eq(sorted_signatures(sol.findDuplicateSubtreesLinear(root)), expected);
  });

  ctx.test("delimiter_collision_regression", [&] {
    // Without delimiters, subtree (1 -> left 23) and (12 -> left 3) both
    // concatenate to "123###" and would falsely collide.
    TreeArena arena;
    TreeNode* root = arena.build({0, 1, 12, 23, std::nullopt, 3, std::nullopt});

    ctx.expect_true(sol.findDuplicateSubtrees(root).empty());
    ctx.expect_true(sol.findDuplicateSubtreesLinear(root).empty());
  });

  ctx.test("all_unique_and_empty", [&] {
    TreeArena arena;
    TreeNode* unique_tree = arena.build({1, 2, 3, 4, 5});

    ctx.expect_true(sol.findDuplicateSubtrees(unique_tree).empty());
    ctx.expect_true(sol.findDuplicateSubtreesLinear(unique_tree).empty());
    ctx.expect_true(sol.findDuplicateSubtrees(nullptr).empty());
    ctx.expect_true(sol.findDuplicateSubtreesLinear(nullptr).empty());
  });
}

}  // namespace
