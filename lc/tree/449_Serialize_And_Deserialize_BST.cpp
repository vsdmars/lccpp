// LeetCode 449: Serialize And Deserialize BST
// https://leetcode.com/problems/serialize-and-deserialize-bst/
//
// Problem:
//   Design a stateless `Codec` to serialize a Binary Search Tree (BST) into as
//   compact a string as possible, and deserialize that string back into the
//   exact original BST structure.
//
// Algorithm:
//   Unlike a general binary tree, a BST's in-order traversal is uniquely
//   determined by sorting its keys, so preorder traversal alone (with NO null
//   sentinels) uniquely determines the entire tree:
//     - Serialize:
//       * Perform a preorder DFS (`root`, `left`, `right`) and append each
//         node's integer value separated by spaces. An empty tree encodes as
//         `""`.
//     - Deserialize:
//       * Parse the space-separated integers into a `preorder` vector and
//         maintain a single forward index `idx`.
//       * Recursively build the subtree whose values lie strictly inside the
//         open interval `(lower, upper)` (using 64-bit `std::int64_t` bounds
//         so `INT_MIN` and `INT_MAX` nodes are handled without edge-case
//         overflow):
//           - If `idx == preorder.size()` or `preorder[idx]` is outside
//             `(lower, upper)`, the current subtree is empty (`nullptr`).
//           - Otherwise consume `val = preorder[idx++]`, create the node,
//             recursively build `left` with `(lower, val)`, and `right` with
//             `(val, upper)`.
//   Time Complexity: O(N) for both serialization and deserialization (each
//   node is emitted and consumed exactly once).
//   Space Complexity: O(N) for the serialized string / integer buffer and O(H)
//   for the recursion stack.
//
// Data Structures:
//   - Binary Search Tree (`TreeNode` owning children via
//     `std::unique_ptr<TreeNode>` for leak-free RAII ownership).
//   - Flat preorder integer buffer (`std::vector<int>`) parsed via
//     `std::from_chars`.
//
// Real-World Applications:
//   - Compact on-disk persistence and replication of ordered index trees
//     without wasting space on explicit null child markers.
//   - Fast wire-protocol snapshots of routing tables, interval trees, or
//     sorted symbol tables across distributed nodes.
//   - Compiler / query-planner serialization of ordered lookup structures.
//
// Related LeetCode Problems:
//   - #297 Serialize and Deserialize Binary Tree [done] (general binary tree codec requiring null markers)
//   - #428 Serialize and Deserialize N-ary Tree [done] (N-ary tree codec with end-of-children sentinels)
//   - #1008 Construct Binary Search Tree from Preorder Traversal [done] (linear-time bounded deserialization)
//   - #105 Construct Binary Tree from Preorder and Inorder Traversal [done] (reconstructing from two traversals)
//   - #98 Validate Binary Search Tree [done] (same `(lower, upper)` range invariant across BST subtrees)

#include "framework/lc_framework.h"

#include <charconv>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace {

// Binary Search Tree node with RAII child ownership.
struct TreeNode {
  int val = 0;
  std::unique_ptr<TreeNode> left;
  std::unique_ptr<TreeNode> right;

  TreeNode() = default;
  explicit TreeNode(int v) : val(v) {}
};

class Codec {
 public:
  // Encodes a BST compactly using preorder values with no null markers.
  // Time: O(N), Space: O(N).
  [[nodiscard]] std::string serialize(const TreeNode* root) const {
    if (root == nullptr) {
      return "";
    }

    std::string out;
    auto dfs = [&](this auto&& self, const TreeNode* node) -> void {
      if (node == nullptr) {
        return;
      }
      if (!out.empty()) {
        out.push_back(' ');
      }
      out.append(std::to_string(node->val));
      self(node->left.get());
      self(node->right.get());
    };

    dfs(root);
    return out;
  }

  // Reconstructs the exact BST from its preorder encoding using BST value
  // bounds in O(N) time.
  // Time: O(N), Space: O(N).
  [[nodiscard]] std::unique_ptr<TreeNode> deserialize(
      std::string_view data) const {
    const std::vector<int> preorder = parse_values(data);
    if (preorder.empty()) {
      return nullptr;
    }

    std::size_t idx = 0uz;

    // Consumes the longest prefix starting at `idx` belonging to `(lower,
    // upper)`. Using 64-bit bounds supports every 32-bit `int` including
    // INT_MIN and INT_MAX.
    auto build = [&](this auto&& self, std::int64_t lower,
                     std::int64_t upper) -> std::unique_ptr<TreeNode> {
      if (idx >= preorder.size()) {
        return nullptr;
      }
      const auto val = static_cast<std::int64_t>(preorder[idx]);
      if (val <= lower || val >= upper) {
        return nullptr;
      }

      ++idx;
      auto node = std::make_unique<TreeNode>(static_cast<int>(val));
      node->left = self(lower, val);
      node->right = self(val, upper);
      return node;
    };

    return build(std::numeric_limits<std::int64_t>::min(),
                 std::numeric_limits<std::int64_t>::max());
  }

 private:
  // Parses space-delimited base-10 integers into a vector.
  static std::vector<int> parse_values(std::string_view data) {
    std::vector<int> values;
    std::size_t pos = 0uz;
    while (pos < data.size()) {
      while (pos < data.size() && data[pos] == ' ') {
        ++pos;
      }
      if (pos >= data.size()) {
        break;
      }
      const std::size_t end = data.find(' ', pos);
      const std::string_view token =
          end == std::string_view::npos ? data.substr(pos)
                                        : data.substr(pos, end - pos);
      int val = 0;
      const auto [ptr, ec] =
          std::from_chars(token.data(), token.data() + token.size(), val);
      if (ec == std::errc{} && ptr == token.data() + token.size()) {
        values.push_back(val);
      }
      if (end == std::string_view::npos) {
        break;
      }
      pos = end + 1uz;
    }
    return values;
  }
};

// Recursively verifies structural and value equality between two BSTs.
bool trees_equal(const TreeNode* a, const TreeNode* b) {
  if (a == nullptr || b == nullptr) {
    return a == b;
  }
  return a->val == b->val && trees_equal(a->left.get(), b->left.get()) &&
         trees_equal(a->right.get(), b->right.get());
}

// Helper to insert a key into a BST for concise test construction.
void bst_insert(std::unique_ptr<TreeNode>& root, int val) {
  if (root == nullptr) {
    root = std::make_unique<TreeNode>(val);
    return;
  }
  if (val < root->val) {
    bst_insert(root->left, val);
  } else {
    bst_insert(root->right, val);
  }
}

LC_PROBLEM(449, "Serialize And Deserialize BST", "tree", ctx) {
  ctx.test("example_bst_from_python_build", [&] {
    // 5 -> (2 -> (1, 3), 6)
    std::unique_ptr<TreeNode> root;
    for (const int v : {5, 2, 1, 3, 6}) {
      bst_insert(root, v);
    }

    const Codec codec{};
    const std::string encoded = codec.serialize(root.get());
    ctx.expect_eq(encoded, std::string("5 2 1 3 6"));

    const auto decoded = codec.deserialize(encoded);
    ctx.expect_true(trees_equal(root.get(), decoded.get()));
  });

  ctx.test("empty_and_single_node", [&] {
    const Codec codec{};
    ctx.expect_eq(codec.serialize(nullptr), std::string(""));
    ctx.expect_true(codec.deserialize("") == nullptr);

    auto single = std::make_unique<TreeNode>(7);
    const std::string encoded = codec.serialize(single.get());
    ctx.expect_eq(encoded, std::string("7"));
    ctx.expect_true(trees_equal(single.get(), codec.deserialize(encoded).get()));
  });

  ctx.test("skewed_left_and_right_bsts", [&] {
    const Codec codec{};

    std::unique_ptr<TreeNode> left_skew;
    for (const int v : {5, 4, 3, 2, 1}) {
      bst_insert(left_skew, v);
    }
    const std::string left_enc = codec.serialize(left_skew.get());
    ctx.expect_eq(left_enc, std::string("5 4 3 2 1"));
    ctx.expect_true(
        trees_equal(left_skew.get(), codec.deserialize(left_enc).get()));

    std::unique_ptr<TreeNode> right_skew;
    for (const int v : {1, 2, 3, 4, 5}) {
      bst_insert(right_skew, v);
    }
    const std::string right_enc = codec.serialize(right_skew.get());
    ctx.expect_eq(right_enc, std::string("1 2 3 4 5"));
    ctx.expect_true(
        trees_equal(right_skew.get(), codec.deserialize(right_enc).get()));
  });

  ctx.test("negative_and_int_extreme_values", [&] {
    std::unique_ptr<TreeNode> root;
    for (const int v : {0, std::numeric_limits<int>::min(), -100,
                        std::numeric_limits<int>::max(), 100}) {
      bst_insert(root, v);
    }

    const Codec codec{};
    const std::string encoded = codec.serialize(root.get());
    const auto decoded = codec.deserialize(encoded);
    ctx.expect_true(trees_equal(root.get(), decoded.get()));
    ctx.expect_eq(codec.serialize(decoded.get()), encoded);
  });
}

}  // namespace
