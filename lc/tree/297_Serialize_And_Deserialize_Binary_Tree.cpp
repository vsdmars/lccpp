// LeetCode 297: Serialize And Deserialize Binary Tree
// https://leetcode.com/problems/serialize-and-deserialize-binary-tree/
//
// Problem:
//   Design a stateless codec (`serialize` and `deserialize`) that encodes an
//   arbitrary binary tree into a string representation and decodes that string
//   back into an exact copy of the original binary tree (preserving every node
//   value, including negative integers, and exact parent-child topology).
//
// Algorithm:
//   Preorder DFS traversal with explicit null sentinels (`"#"`) separated by
//   single spaces:
//     - `serialize(root)`: Emit `node->val` followed recursively by the encoded
//       left subtree and encoded right subtree. Whenever `node == nullptr`,
//       emit the sentinel `"#"`. Because every non-null node has exactly two
//       child slots (each terminated by `#` sentinels), the preorder token
//       stream uniquely determines the binary tree structure without needing an
//       auxiliary inorder sequence.
//     - `deserialize(data)`: Scan whitespace-separated tokens left-to-right
//       using an index cursor `pos`. If the next token is `"#"` (or missing),
//       return `nullptr`; otherwise parse the integer with `std::from_chars`,
//       recursively decode the left subtree, then the right subtree, and attach
//       them to the newly constructed node.
//   Time Complexity:  O(n) for both `serialize` and `deserialize` (emits and
//                     consumes exactly 2n + 1 tokens).
//   Space Complexity: O(n) for the serialized string and O(h) recursion stack
//                     (O(n) worst case for a skewed tree).
//
// Data Structures:
//   - `std::string` output buffer for serialized preorder tokens.
//   - Cursor index (`std::size_t`) scanning `std::string_view` in zero-copy
//     fashion during deserialization, paired with `std::unique_ptr` custom
//     deleter for leak-free RAII ownership.
//
// Real-World Applications:
//   - Persisting and transmitting abstract syntax trees (ASTs), decision-tree
//     ML models, and scene graphs across RPC / disk checkpoints.
//   - Caching hierarchical UI component trees or query execution plans in
//     key-value stores (e.g., Redis, Memcached).
//   - Snapshot replication of tree-structured state machines in distributed
//     databases.
//
// Related LeetCode Problems:
//   - #449 Serialize and Deserialize BST [done] (omits null sentinels using BST value bounds)
//   - #428 Serialize and Deserialize N-ary Tree [done] (encodes variable child counts)
//   - #105 Construct Binary Tree from Preorder and Inorder Traversal [done] (rebuilding trees without null sentinels)
//   - #222 Count Complete Tree Nodes [done] (exploiting structural invariants of binary trees)

#include "framework/lc_framework.h"

#include <charconv>
#include <cstddef>
#include <deque>
#include <memory>
#include <string>
#include <string_view>
#include <system_error>

namespace {

struct TreeNode {
  int val;
  TreeNode* left;
  TreeNode* right;

  explicit TreeNode(int v, TreeNode* l = nullptr, TreeNode* r = nullptr)
      : val(v), left(l), right(r) {}
};

// Custom RAII deleter for recursively freeing dynamically allocated trees
// returned by `Codec::deserialize`.
struct TreeDeleter {
  void operator()(TreeNode* node) const noexcept {
    if (node == nullptr) {
      return;
    }
    (*this)(node->left);
    (*this)(node->right);
    delete node;
  }
};

using TreePtr = std::unique_ptr<TreeNode, TreeDeleter>;

// Stateless binary tree serializer/deserializer using space-delimited preorder
// traversal with `#` null sentinels.
class Codec {
 public:
  // Encodes a binary tree to a single space-delimited preorder string.
  // Time: O(n), Space: O(n).
  [[nodiscard]] std::string serialize(const TreeNode* root) const {
    std::string out;

    auto encode = [&](this auto&& self, const TreeNode* node) -> void {
      if (!out.empty()) {
        out.push_back(' ');
      }
      if (node == nullptr) {
        out.push_back('#');
        return;
      }
      out += std::to_string(node->val);
      self(node->left);
      self(node->right);
    };

    encode(root);
    return out;
  }

  // Decodes a serialized preorder string back into a newly allocated tree.
  // Caller owns the returned tree (wrap in `TreePtr` for automatic cleanup).
  // Time: O(n), Space: O(h) recursion stack (excluding reconstructed nodes).
  [[nodiscard]] TreeNode* deserialize(std::string_view data) const {
    std::size_t pos = 0uz;
    return decode_node(data, pos).release();
  }

 private:
  // Extracts the next whitespace-delimited token from `data` starting at `pos`.
  static std::string_view next_token(std::string_view data, std::size_t& pos) {
    while (pos < data.size() && data[pos] == ' ') {
      ++pos;
    }
    const std::size_t start = pos;
    while (pos < data.size() && data[pos] != ' ') {
      ++pos;
    }
    return data.substr(start, pos - start);
  }

  // Recursively consumes tokens in preorder and builds an exception-safe
  // `TreePtr` subtree.
  static TreePtr decode_node(std::string_view data, std::size_t& pos) {
    const std::string_view token = next_token(data, pos);
    if (token.empty() || token == "#") {
      return nullptr;
    }

    int val = 0;
    const auto [ptr, ec] =
        std::from_chars(token.data(), token.data() + token.size(), val);
    if (ec != std::errc{} || ptr != token.data() + token.size()) {
      return nullptr;
    }

    // Decode children into RAII handles before attaching so no subtree can
    // leak even if an allocation fails.
    TreePtr node(new TreeNode(val));
    TreePtr left = decode_node(data, pos);
    TreePtr right = decode_node(data, pos);
    node->left = left.release();
    node->right = right.release();
    return node;
  }
};

// Recursively verifies exact structural and value equality of two binary trees.
bool trees_equal(const TreeNode* a, const TreeNode* b) {
  if (a == nullptr && b == nullptr) {
    return true;
  }
  if (a == nullptr || b == nullptr) {
    return false;
  }
  return a->val == b->val && trees_equal(a->left, b->left) &&
         trees_equal(a->right, b->right);
}

LC_PROBLEM(297, "Serialize And Deserialize Binary Tree", "tree", ctx) {
  const Codec codec;

  ctx.test("leetcode_example", [&] {
    //     1
    //    / \
    //   2   3
    //      / \
    //     4   5
    std::deque<TreeNode> arena;
    auto make = [&](int v) -> TreeNode* { return &arena.emplace_back(v); };
    TreeNode* root = make(1);
    root->left = make(2);
    root->right = make(3);
    root->right->left = make(4);
    root->right->right = make(5);

    const std::string encoded = codec.serialize(root);
    ctx.expect_eq(encoded, std::string("1 2 # # 3 4 # # 5 # #"));

    const TreePtr decoded(codec.deserialize(encoded));
    ctx.expect_true(trees_equal(root, decoded.get()));
    ctx.expect_eq(codec.serialize(decoded.get()), encoded);
  });

  ctx.test("source_build_tree", [&] {
    //       1
    //      / \
    //     2   3
    //    /   / \
    //   7   4   5
    std::deque<TreeNode> arena;
    auto make = [&](int v) -> TreeNode* { return &arena.emplace_back(v); };
    TreeNode* root = make(1);
    root->left = make(2);
    root->left->left = make(7);
    root->right = make(3);
    root->right->left = make(4);
    root->right->right = make(5);

    const std::string encoded = codec.serialize(root);
    ctx.expect_eq(encoded, std::string("1 2 7 # # # 3 4 # # 5 # #"));

    const TreePtr decoded(codec.deserialize(encoded));
    ctx.expect_true(trees_equal(root, decoded.get()));
    ctx.expect_eq(codec.serialize(decoded.get()), encoded);
  });

  ctx.test("empty_and_single_node", [&] {
    const std::string empty_enc = codec.serialize(nullptr);
    ctx.expect_eq(empty_enc, std::string("#"));
    const TreePtr empty_dec(codec.deserialize(empty_enc));
    ctx.expect_eq(empty_dec.get(), nullptr);

    TreeNode single(-1000);
    const std::string single_enc = codec.serialize(&single);
    ctx.expect_eq(single_enc, std::string("-1000 # #"));
    const TreePtr single_dec(codec.deserialize(single_enc));
    ctx.expect_true(trees_equal(&single, single_dec.get()));
  });

  ctx.test("skewed_and_negative_values", [&] {
    std::deque<TreeNode> arena;
    auto make = [&](int v) -> TreeNode* { return &arena.emplace_back(v); };
    TreeNode* root = make(-5);
    root->right = make(0);
    root->right->left = make(42);
    root->right->left->right = make(-999);

    const std::string encoded = codec.serialize(root);
    const TreePtr decoded(codec.deserialize(encoded));
    ctx.expect_true(trees_equal(root, decoded.get()));
    ctx.expect_eq(codec.serialize(decoded.get()), encoded);
  });
}

}  // namespace
