// LeetCode 428: Serialize And Deserialize N Ary Tree
// https://leetcode.com/problems/serialize-and-deserialize-n-ary-tree/
//
// Problem:
//   Design a stateless algorithm to serialize an N-ary tree (a rooted tree
//   where each node has an arbitrary list of ordered children) into a string,
//   and deserialize that string back into the exact original N-ary tree
//   structure and values.
//
// Examples:
//   Example 1:
//     Input:  root = [1, null, 3, 2, 4, null, 5, 6]
//     Output: serialize(root) = "1 3 5 # 6 # # 2 # 4 # #", deserialize(encoded) =
//             [1, null, 3, 2, 4, null, 5, 6]
//     Explanation:
//       - serialize runs preorder DFS from root 1, emitting "1", recursing into
//         child 3 -> 5 -> "#" -> 6 -> "#" and closing 3 with "#", then emitting "2
//         # 4 #" and a final "#" for root 1.
//       - deserialize splits "1 3 5 # 6 # # 2 # 4 # #" into 12 string_view tokens,
//         parses root Node(1) at idx = 0, and advances idx to 1.
//       - dfs(1) parses child 3 at idx = 1, recursively attaches leaves 5 (at idx =
//         2, closed at 3) and 6 (at idx = 4, closed at 5), and consumes 3's closing
//         "#" at idx = 6.
//       - Continuing from idx = 7, dfs(1) parses child 2 (closed at 8) and child 4
//         (closed at 10), then advances idx past root 1's closing "#" at idx = 11
//         to return the reconstructed tree.
//
//   Example 2:
//     Input:  root = [99]
//     Output: serialize(root) = "99 #", deserialize("99 #") = [99]
//     Explanation:
//       - serialize visits node 99, appends "99", skips the empty children loop,
//         and appends the closing sentinel "#" to return "99 #".
//       - deserialize splits "99 #" into ["99", "#"], constructs root Node(99) at
//         idx = 0, and increments idx to 1.
//       - dfs(root) sees tokens[1] == "#", skips child parsing, advances idx past
//         the sentinel, and returns the single-node tree [99].
//
//   Example 3:
//     Input:  root = [] (nullptr)
//     Output: serialize(nullptr) = "", deserialize("") = [] (nullptr)
//     Explanation:
//       - serialize checks root == nullptr at entry and immediately returns the
//         empty string "".
//       - deserialize calls split_tokens("") which returns an empty token vector,
//         causing it to return nullptr directly.
//
// Algorithm:
//   Use Preorder DFS with an explicit end-of-children sentinel `"#"`:
//     - Serialize:
//       * If `root == nullptr`, return the empty string `""`.
//       * Visit `node`: emit `node->val`, recursively serialize each child in
//         order, and then emit `"#"` to terminate `node`'s child list.
//       * Separate all tokens with a single space.
//       * For example, `[1 [3[5 6] 2 4]]` encodes as `"1 3 5 # 6 # # 2 # 4 # #"`.
//     - Deserialize:
//       * If `data` is empty, return `nullptr`.
//       * Split `data` into non-empty `std::string_view` tokens (zero copies).
//       * Maintain a forward cursor `idx`. Parse the root integer, then
//         recursively read child subtrees for `parent` until the matching `"#"`
//         sentinel is reached, consuming the `"#"` before returning.
//   Because every node's subtree is unambiguously delimited by its closing
//   `"#"` marker, the encoding is bijective without needing to store child
//   counts.
//   Time Complexity: O(N), where N is the number of nodes (each node produces
//   exactly two tokens: its value and its closing sentinel).
//   Space Complexity: O(N) for the serialized string, token view vector, and
//   O(H) recursion depth.
//
// Data Structures:
//   - N-ary tree (`Node` owning children via `std::vector<std::unique_ptr<Node>>`
//     for leak-free RAII ownership).
//   - Token slice array (`std::vector<std::string_view>`) parsed directly from
//     the serialized buffer using `std::from_chars`.
//
// Real-World Applications:
//   - AST / DOM / JSON / XML wire serialization and checkpointing over RPCs or
//     disk storage.
//   - File-system directory snapshot formats and hierarchical configuration
//     export/import tools.
//   - Game-engine scene-graph persistence and undo/redo tree snapshots.
//
// Related LeetCode Problems:
//   - #297 Serialize and Deserialize Binary Tree [done] (preorder + null sentinel codec for binary trees)
//   - #449 Serialize and Deserialize BST [done] (compact preorder-only BST codec using value bounds)
//   - #431 Encode N-ary Tree to Binary Tree [new] (left-child / right-sibling encoding of N-ary trees)
//   - #589 N-ary Tree Preorder Traversal [new] (preorder traversal of N-ary trees)
//   - #590 N-ary Tree Postorder Traversal [new] (postorder traversal of N-ary trees)

#include "framework/lc_framework.h"

#include <charconv>
#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

namespace {

// N-ary tree node with RAII ownership of its children.
struct Node {
  int val = 0;
  std::vector<std::unique_ptr<Node>> children;

  Node() = default;
  explicit Node(int v) : val(v) {}
};

class Codec {
 public:
  // Encodes an N-ary tree to a space-delimited preorder string with `#`
  // end-of-children sentinels.
  // Time: O(N), Space: O(N).
  [[nodiscard]] std::string serialize(const Node* root) const {
    if (root == nullptr) {
      return "";
    }

    std::string out;
    auto append_token = [&](std::string_view token) {
      if (!out.empty()) {
        out.push_back(' ');
      }
      out.append(token);
    };

    auto dfs = [&](this auto&& self, const Node* node) -> void {
      append_token(std::to_string(node->val));
      for (const auto& child : node->children) {
        self(child.get());
      }
      // Sentinel marks the end of `node`'s children list.
      append_token("#");
    };

    dfs(root);
    return out;
  }

  // Decodes a string produced by `serialize` back into an N-ary tree.
  // Time: O(N), Space: O(N).
  [[nodiscard]] std::unique_ptr<Node> deserialize(std::string_view data) const {
    const std::vector<std::string_view> tokens = split_tokens(data);
    if (tokens.empty()) {
      return nullptr;
    }

    std::size_t idx = 0uz;
    auto root = std::make_unique<Node>(parse_int(tokens[idx]));
    ++idx;

    // Consume all child subtrees belonging to `parent` up to its matching `#`.
    auto dfs = [&](this auto&& self, Node* parent) -> void {
      while (idx < tokens.size() && tokens[idx] != "#") {
        auto child = std::make_unique<Node>(parse_int(tokens[idx]));
        ++idx;
        self(child.get());
        parent->children.push_back(std::move(child));
      }
      // Advance past the closing `#` sentinel for `parent`.
      if (idx < tokens.size()) {
        ++idx;
      }
    };

    dfs(root.get());
    return root;
  }

 private:
  // Splits `data` on ASCII spaces into non-empty views without heap copying.
  static std::vector<std::string_view> split_tokens(std::string_view data) {
    std::vector<std::string_view> tokens;
    std::size_t pos = 0uz;
    while (pos < data.size()) {
      while (pos < data.size() && data[pos] == ' ') {
        ++pos;
      }
      if (pos >= data.size()) {
        break;
      }
      const std::size_t end = data.find(' ', pos);
      if (end == std::string_view::npos) {
        tokens.push_back(data.substr(pos));
        break;
      }
      tokens.push_back(data.substr(pos, end - pos));
      pos = end + 1uz;
    }
    return tokens;
  }

  // Parses a base-10 signed integer token using `std::from_chars`.
  static int parse_int(std::string_view token) {
    int value = 0;
    const auto [ptr, ec] =
        std::from_chars(token.data(), token.data() + token.size(), value);
    if (ec != std::errc{} || ptr != token.data() + token.size()) {
      return 0;
    }
    return value;
  }
};

// Recursively checks structural and value equality between two N-ary trees.
bool trees_equal(const Node* a, const Node* b) {
  if (a == nullptr || b == nullptr) {
    return a == b;
  }
  if (a->val != b->val || a->children.size() != b->children.size()) {
    return false;
  }
  for (std::size_t i = 0uz; i < a->children.size(); ++i) {
    if (!trees_equal(a->children[i].get(), b->children[i].get())) {
      return false;
    }
  }
  return true;
}

LC_PROBLEM(428, "Serialize And Deserialize N Ary Tree", "tree", ctx) {
  ctx.test("example_3_ary_tree_round_trip", [&] {
    // [1 [3 [5 6] 2 4]]
    auto root = std::make_unique<Node>(1);
    auto n3 = std::make_unique<Node>(3);
    n3->children.push_back(std::make_unique<Node>(5));
    n3->children.push_back(std::make_unique<Node>(6));
    root->children.push_back(std::move(n3));
    root->children.push_back(std::make_unique<Node>(2));
    root->children.push_back(std::make_unique<Node>(4));

    const Codec codec{};
    const std::string encoded = codec.serialize(root.get());
    ctx.expect_eq(encoded, std::string("1 3 5 # 6 # # 2 # 4 # #"));

    const auto decoded = codec.deserialize(encoded);
    ctx.expect_true(trees_equal(root.get(), decoded.get()));
  });

  ctx.test("empty_and_single_node", [&] {
    const Codec codec{};
    ctx.expect_eq(codec.serialize(nullptr), std::string(""));
    ctx.expect_true(codec.deserialize("") == nullptr);

    auto single = std::make_unique<Node>(99);
    const std::string encoded = codec.serialize(single.get());
    ctx.expect_eq(encoded, std::string("99 #"));
    const auto decoded = codec.deserialize(encoded);
    ctx.expect_true(trees_equal(single.get(), decoded.get()));
  });

  ctx.test("deep_chain_and_negative_values", [&] {
    // Chain: -10 -> 0 -> 42 -> (-7, -8, 100)
    auto root = std::make_unique<Node>(-10);
    auto n0 = std::make_unique<Node>(0);
    auto n42 = std::make_unique<Node>(42);
    n42->children.push_back(std::make_unique<Node>(-7));
    n42->children.push_back(std::make_unique<Node>(-8));
    n42->children.push_back(std::make_unique<Node>(100));
    n0->children.push_back(std::move(n42));
    root->children.push_back(std::move(n0));

    const Codec codec{};
    const std::string encoded = codec.serialize(root.get());
    const auto decoded = codec.deserialize(encoded);
    ctx.expect_true(trees_equal(root.get(), decoded.get()));
    // Re-serializing the decoded tree must yield the identical encoding.
    ctx.expect_eq(codec.serialize(decoded.get()), encoded);
  });
}

}  // namespace
