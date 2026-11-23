// LeetCode 655: Print Binary Tree
// https://leetcode.com/problems/print-binary-tree/
//
// Problem:
//   Given the root of a binary tree, format it into a 2D string matrix `res`
//   of size `m x n` according to these layout rules:
//   - `m` equals the node-height `H` of the tree (equivalently `height + 1`
//     when height is measured in edges, with `H` in [1, 10]).
//   - `n = 2^H - 1` (always odd).
//   - Place the root in the exact horizontal center of row 0: `res[0][(n-1)/2]`.
//   - For any node placed at `(r, c)` spanning columns `[lhs, rhs]`, place its
//     left child at the midpoint of `[lhs, c - 1]` on row `r + 1`, and its
//     right child at the midpoint of `[c + 1, rhs]` on row `r + 1`.
//   - Every unoccupied cell contains the empty string `""`.
//
// Examples:
//   Example 1:
//     Input:  root = [1, 2]
//     Output: [["", "1", ""], ["2", "", ""]]
//     Explanation:
//       - get_height returns rows = 2, so cols = (1 << 2) - 1 = 3 and a 2 x 3 grid
//         of "" is allocated.
//       - fill(1, row=0, lhs=0, rhs=2) computes mid = 0 + (2 - 0) / 2 = 1 and
//         writes "1" at grid[0][1].
//       - Recursing on non-null left child 2 with (row=1, lhs=0, rhs=0) computes
//         mid = 0 and writes "2" at grid[1][0].
//
//   Example 2:
//     Input:  root = [1, 2, 3, null, 4]
//     Output: [["", "", "", "1", "", "", ""], ["", "2", "", "", "", "3", ""], ["",
//             "", "4", "", "", "", ""]]
//     Explanation:
//       - get_height returns rows = 3, giving cols = (1 << 3) - 1 = 7 columns
//         initialized to "".
//       - Root 1 spans [0, 6] to place "1" at grid[0][3], splitting into left child
//         2 on [0, 2] (grid[1][1] = "2") and right child 3 on [4, 6] (grid[1][5] =
//         "3").
//       - Node 2 skips its null left child and places right child 4 on [mid + 1,
//         rhs] = [2, 2] at grid[2][2].
//
//   Example 3:
//     Input:  root = [-1, null, -2]
//     Output: [["", "-1", ""], ["", "", "-2"]]
//     Explanation:
//       - get_height returns rows = 2, yielding a 2 x 3 grid of "" strings.
//       - Root -1 bisects [0, 2] to mid = 1 and places "-1" at grid[0][1].
//       - Its null left child is skipped while right child -2 bisects [2, 2] to
//         place "-2" at grid[1][2].
//
// Algorithm:
//   Two-pass recursive DFS:
//   1. Compute tree height `H` (number of levels) via post-order DFS.
//   2. Allocate an `H x (2^H - 1)` grid pre-filled with `""`.
//   3. Populate the grid via divide-and-conquer pre-order DFS tracking the
//      inclusive column interval `[lhs, rhs]` owned by each subtree:
//      - Place `std::to_string(node->val)` at `mid = lhs + ((rhs - lhs) / 2)`.
//      - Recurse into `node->left` on `(row + 1, lhs, mid - 1)` (only when
//        `node->left != nullptr`, which also guarantees `mid > lhs` so unsigned
//        subtraction never underflows) and into `node->right` on
//        `(row + 1, mid + 1, rhs)`.
//   Time: O(H * 2^H) dominated by constructing and returning the output matrix.
//   Space: O(H * 2^H) for the output matrix plus O(H) recursion stack.
//
// Data Structures:
//   - `std::vector<std::vector<std::string>>`: 2D output matrix indexed by
//     `(row, col)` representing the formatted ASCII-style tree layout.
//   - Call stack (depth H): tracks current row and `[lhs, rhs]` column bounds.
//
// Real-World Applications:
//   - Pretty-printing parse trees, ASTs, and query execution plans in CLI
//     debuggers, database `EXPLAIN` visualizers, and educational tools.
//   - Hierarchical tree layout engines (Reingold-Tilford / layered graph
//     drawing) that assign 2D grid coordinates to nodes before rendering.
//   - Visualizing binary decision trees or tournament brackets in dashboards.
//
// Related LeetCode Problems:
//   - #104 Maximum Depth of Binary Tree [done] (pass-1 height calculation)
//   - #662 Maximum Width of Binary Tree [done] (binary-heap-style positional coordinates)
//   - #297 Serialize and Deserialize Binary Tree [done] (compact text representation of trees)
//   - #987 Vertical Order Traversal of a Binary Tree [done] (2D `(row, col)` tree coordinate mapping)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <deque>
#include <optional>
#include <queue>
#include <string>
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

class Solution {
 public:
  // Formats `root` into an `H x (2^H - 1)` 2D string grid.
  // Time: O(H * 2^H), Space: O(H * 2^H) output + O(H) stack.
  std::vector<std::vector<std::string>> printTree(const TreeNode* root) {
    if (root == nullptr) {
      return {};
    }

    // Pass 1: determine the total number of rows (node-height H).
    const std::size_t rows = get_height(root);
    // Width of a perfect binary tree of height H is 2^H - 1 (always odd).
    const std::size_t cols = (1uz << rows) - 1uz;

    std::vector<std::vector<std::string>> grid(
        rows, std::vector<std::string>(cols, ""));

    // Pass 2: recursively bisect the column interval [lhs, rhs] to place nodes.
    auto fill = [&](this auto&& self, const TreeNode* node, std::size_t row,
                    std::size_t lhs, std::size_t rhs) -> void {
      const std::size_t mid = lhs + ((rhs - lhs) / 2uz);
      grid[row][mid] = std::to_string(node->val);

      // Guarding on non-null children ensures `row + 1 < rows`, `mid > lhs`,
      // and `mid < rhs`, preventing any unsigned underflow on `mid - 1`.
      if (node->left != nullptr) {
        self(node->left, row + 1uz, lhs, mid - 1uz);
      }
      if (node->right != nullptr) {
        self(node->right, row + 1uz, mid + 1uz, rhs);
      }
    };

    fill(root, 0uz, 0uz, cols - 1uz);
    return grid;
  }

 private:
  // Returns the number of levels in the tree (0 for empty, 1 for a leaf).
  static std::size_t get_height(const TreeNode* node) {
    if (node == nullptr) {
      return 0uz;
    }
    return 1uz + std::max(get_height(node->left), get_height(node->right));
  }
};

// RAII arena that owns all test tree nodes cleanly.
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

LC_PROBLEM(655, "Print Binary Tree", "tree", ctx) {
  Solution sol;

  ctx.test("example_1_height_2", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({1, 2});
    const std::vector<std::vector<std::string>> expected = {
        {"", "1", ""},
        {"2", "", ""},
    };
    ctx.expect_eq(sol.printTree(root), expected);
  });

  ctx.test("example_2_height_3", [&] {
    TreeArena arena;
    TreeNode* root = arena.build({1, 2, 3, std::nullopt, 4});
    const std::vector<std::vector<std::string>> expected = {
        {"", "", "", "1", "", "", ""},
        {"", "2", "", "", "", "3", ""},
        {"", "", "4", "", "", "", ""},
    };
    ctx.expect_eq(sol.printTree(root), expected);
  });

  ctx.test("example_3_height_4_skewed_left", [&] {
    TreeArena arena;
    TreeNode* root = arena.build(
        {1, 2, 5, 3, std::nullopt, std::nullopt, std::nullopt, 4});
    const std::vector<std::vector<std::string>> expected = {
        {"", "", "", "", "", "", "", "1", "", "", "", "", "", "", ""},
        {"", "", "", "2", "", "", "", "", "", "", "", "5", "", "", ""},
        {"", "3", "", "", "", "", "", "", "", "", "", "", "", "", ""},
        {"4", "", "", "", "", "", "", "", "", "", "", "", "", "", ""},
    };
    ctx.expect_eq(sol.printTree(root), expected);
  });

  ctx.test("single_node_and_negative_values", [&] {
    TreeArena arena;
    TreeNode* single = arena.build({42});
    ctx.expect_eq(sol.printTree(single),
                  std::vector<std::vector<std::string>>{{"42"}});

    TreeNode* neg = arena.build({-1, std::nullopt, -2});
    const std::vector<std::vector<std::string>> expected = {
        {"", "-1", ""},
        {"", "", "-2"},
    };
    ctx.expect_eq(sol.printTree(neg), expected);
  });

  ctx.test("empty_tree", [&] {
    ctx.expect_true(sol.printTree(nullptr).empty());
  });
}

}  // namespace
