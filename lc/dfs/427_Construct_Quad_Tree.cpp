// LeetCode 427: Construct Quad Tree
// https://leetcode.com/problems/construct-quad-tree/
//
// Problem:
//   Given an n x n binary matrix `grid` (where n is a power of 2 and each cell
//   is 0 or 1), construct and return the root of a Quad-Tree representing
//   `grid`. Each Quad-Tree node covers a square sub-region: if all cells in the
//   region have the same value, the node is a leaf (`is_leaf = true`, `val` set
//   to that boolean value, and all four child pointers null). Otherwise, the
//   node is an internal node (`is_leaf = false`, arbitrary `val`) whose four
//   children recursively represent the top-left, top-right, bottom-left, and
//   bottom-right quadrants of size (n / 2) x (n / 2).
//
// Algorithm:
//   Top-down divide-and-conquer DFS accelerated with a 2D prefix-sum table:
//   1. Precompute an (n + 1) x (n + 1) 2D prefix sum over `grid` in O(n^2) time
//      so the sum of any sub-square `[r, r + size) x [c, c + size)` can be
//      queried in O(1) time.
//   2. Recursively build the subtree for `(r, c, size)`:
//      - If the sub-square sum is 0, return a leaf node `(val = false, is_leaf = true)`.
//      - If the sub-square sum equals `size * size`, return a leaf node
//        `(val = true, is_leaf = true)`.
//      - Otherwise, split into four sub-squares of side `half = size / 2` and
//        return an internal node owning the four recursively built quadrants.
//   Using coordinate bounds + 2D prefix sums eliminates both sub-grid copying
//   and repeated cell scans, reducing total work from O(n^2 log n) to optimal
//   O(n^2).
//   Time Complexity: O(n^2) — O(n^2) prefix-sum build plus O(1) work per
//     created node (at most (4 / 3) * n^2 nodes in a full quadtree).
//   Space Complexity: O(n^2) for the prefix-sum table and resulting Quad-Tree,
//     plus O(log n) recursion stack depth.
//
// Data Structures:
//   - Quad-Tree (`Node` owning four `std::unique_ptr<Node>` quadrant children
//     for automatic RAII memory safety with zero leaks).
//   - 2D prefix-sum matrix (`std::vector<std::vector<int>>`) for O(1) region
//     uniformity tests.
//
// Real-World Applications:
//   - Spatial indexing in GIS, mapping engines, and 2D game physics (broad-
//     phase collision detection and viewport culling).
//   - Lossless and lossy image compression / level-of-detail (LOD) terrain
//     rendering where uniform regions collapse into single tiles.
//   - Sparse matrix and mesh refinement (adaptive finite-element grids in
//     scientific computing).
//
// Related LeetCode Problems:
//   - #558 Logical OR of Two Binary Grids Represented as Quad-Trees [new] (recursive quadtree merge)
//   - #304 Range Sum Query 2D [done] - Immutable (2D prefix sums for O(1) submatrix queries)
//   - #105 Construct Binary Tree from Preorder and Inorder Traversal [done] (divide-and-conquer tree construction)
//   - #108 Convert Sorted Array to Binary Search Tree [done] (recursive balanced tree construction)

#include "framework/lc_framework.h"

#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

namespace {

// Quad-Tree node using RAII `std::unique_ptr` ownership for its four quadrants.
struct Node {
  bool val = false;
  bool is_leaf = false;
  std::unique_ptr<Node> top_left;
  std::unique_ptr<Node> top_right;
  std::unique_ptr<Node> bottom_left;
  std::unique_ptr<Node> bottom_right;

  Node(bool v, bool leaf) : val(v), is_leaf(leaf) {}

  Node(bool v, bool leaf, std::unique_ptr<Node> tl, std::unique_ptr<Node> tr,
       std::unique_ptr<Node> bl, std::unique_ptr<Node> br)
      : val(v),
        is_leaf(leaf),
        top_left(std::move(tl)),
        top_right(std::move(tr)),
        bottom_left(std::move(bl)),
        bottom_right(std::move(br)) {}
};

class Solution {
 public:
  // Constructs the minimal Quad-Tree representing the n x n binary `grid`.
  //
  // Invariant: Every returned subtree represents `[r, r + size) x [c, c + size)`
  // and is collapsed into a single leaf if and only if all cells in that square
  // share the same value.
  //
  // Time: O(n^2); Space: O(n^2).
  std::unique_ptr<Node> construct(const std::vector<std::vector<int>>& grid) {
    if (grid.empty()) {
      return nullptr;
    }

    const std::size_t n = grid.size();
    const std::vector<std::vector<int>> prefix = buildPrefixSum(grid);
    return buildNode(prefix, 0uz, 0uz, n);
  }

 private:
  // Builds an (n + 1) x (n + 1) 2D prefix-sum table where `prefix[r][c]` equals
  // the sum of all cells in `grid[0..r-1][0..c-1]`.
  static std::vector<std::vector<int>> buildPrefixSum(
      const std::vector<std::vector<int>>& grid) {
    const std::size_t n = grid.size();
    std::vector<std::vector<int>> prefix(n + 1uz,
                                         std::vector<int>(n + 1uz, 0));

    for (std::size_t r = 0uz; r < n; ++r) {
      for (std::size_t c = 0uz; c < n; ++c) {
        prefix[r + 1uz][c + 1uz] =
            grid[r][c] + prefix[r][c + 1uz] + prefix[r + 1uz][c] - prefix[r][c];
      }
    }
    return prefix;
  }

  // Returns the sum of the square region `[r, r + size) x [c, c + size)` in
  // O(1) via inclusion-exclusion on `prefix`.
  static int regionSum(const std::vector<std::vector<int>>& prefix,
                       std::size_t r, std::size_t c, std::size_t size) {
    return prefix[r + size][c + size] - prefix[r][c + size] -
           prefix[r + size][c] + prefix[r][c];
  }

  // Recursively constructs the Quad-Tree node for `[r, r + size) x [c, c + size)`.
  static std::unique_ptr<Node> buildNode(
      const std::vector<std::vector<int>>& prefix, std::size_t r, std::size_t c,
      std::size_t size) {
    const int sum = regionSum(prefix, r, c, size);
    const int area = static_cast<int>(size * size);

    // Uniform region -> leaf node (all 0s or all 1s).
    if (sum == 0 || sum == area) {
      return std::make_unique<Node>(sum == area, true);
    }

    // Mixed region -> subdivide into four equal quadrants of side `size / 2`.
    const std::size_t half = size / 2uz;
    return std::make_unique<Node>(
        true, false, buildNode(prefix, r, c, half),
        buildNode(prefix, r, c + half, half),
        buildNode(prefix, r + half, c, half),
        buildNode(prefix, r + half, c + half, half));
  }
};

// Recursively expands a Quad-Tree back into its n x n binary grid to verify
// exact lossless encoding in unit tests.
void expandQuadTree(const Node* node, std::size_t r, std::size_t c,
                    std::size_t size, std::vector<std::vector<int>>& out) {
  if (node == nullptr) {
    return;
  }
  if (node->is_leaf) {
    const int cell = node->val ? 1 : 0;
    for (std::size_t i = r; i < r + size; ++i) {
      for (std::size_t j = c; j < c + size; ++j) {
        out[i][j] = cell;
      }
    }
    return;
  }

  const std::size_t half = size / 2uz;
  expandQuadTree(node->top_left.get(), r, c, half, out);
  expandQuadTree(node->top_right.get(), r, c + half, half, out);
  expandQuadTree(node->bottom_left.get(), r + half, c, half, out);
  expandQuadTree(node->bottom_right.get(), r + half, c + half, half, out);
}

// Verifies structural minimality: leaves have null children, and no internal
// node has four identical leaf children that should have been merged.
bool isMinimalQuadTree(const Node* node) {
  if (node == nullptr) {
    return true;
  }
  if (node->is_leaf) {
    return node->top_left == nullptr && node->top_right == nullptr &&
           node->bottom_left == nullptr && node->bottom_right == nullptr;
  }
  if (node->top_left == nullptr || node->top_right == nullptr ||
      node->bottom_left == nullptr || node->bottom_right == nullptr) {
    return false;
  }

  const bool all_leaves =
      node->top_left->is_leaf && node->top_right->is_leaf &&
      node->bottom_left->is_leaf && node->bottom_right->is_leaf;
  const bool same_val =
      (node->top_left->val == node->top_right->val) &&
      (node->top_left->val == node->bottom_left->val) &&
      (node->top_left->val == node->bottom_right->val);
  if (all_leaves && same_val) {
    return false;
  }

  return isMinimalQuadTree(node->top_left.get()) &&
         isMinimalQuadTree(node->top_right.get()) &&
         isMinimalQuadTree(node->bottom_left.get()) &&
         isMinimalQuadTree(node->bottom_right.get());
}

LC_PROBLEM(427, "Construct Quad Tree", "dfs", ctx) {
  ctx.test("example_8x8_grid", [&] {
    const std::vector<std::vector<int>> grid = {
        {1, 1, 1, 1, 0, 0, 0, 0}, {1, 1, 1, 1, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 0, 0, 0, 0}, {1, 1, 1, 1, 0, 0, 0, 0},
        {1, 1, 1, 1, 0, 0, 0, 0}, {1, 1, 1, 1, 0, 0, 0, 0}};

    Solution sol;
    const std::unique_ptr<Node> root = sol.construct(grid);
    ctx.expect_true(root != nullptr);
    if (root == nullptr) {
      return;
    }
    ctx.expect_false(root->is_leaf);
    ctx.expect_true(isMinimalQuadTree(root.get()));

    // Check top-level quadrant structure directly.
    ctx.expect_true(root->top_left != nullptr && root->top_left->is_leaf &&
                    root->top_left->val);
    ctx.expect_true(root->top_right != nullptr && !root->top_right->is_leaf);
    ctx.expect_true(root->bottom_left != nullptr &&
                    root->bottom_left->is_leaf && root->bottom_left->val);
    ctx.expect_true(root->bottom_right != nullptr &&
                    root->bottom_right->is_leaf && !root->bottom_right->val);

    // Check subdivided top-right quadrant leaves.
    if (const Node* tr = root->top_right.get(); tr != nullptr) {
      ctx.expect_true(tr->top_left != nullptr && tr->top_left->is_leaf &&
                      !tr->top_left->val);
      ctx.expect_true(tr->top_right != nullptr && tr->top_right->is_leaf &&
                      !tr->top_right->val);
      ctx.expect_true(tr->bottom_left != nullptr && tr->bottom_left->is_leaf &&
                      tr->bottom_left->val);
      ctx.expect_true(tr->bottom_right != nullptr &&
                      tr->bottom_right->is_leaf && tr->bottom_right->val);
    }

    // Verify exact round-trip reconstruction.
    std::vector<std::vector<int>> rebuilt(8uz, std::vector<int>(8uz, -1));
    expandQuadTree(root.get(), 0uz, 0uz, 8uz, rebuilt);
    ctx.expect_eq(rebuilt, grid);
  });

  ctx.test("uniform_grid_single_leaf", [&] {
    const std::vector<std::vector<int>> grid = {{1, 1}, {1, 1}};
    Solution sol;
    const std::unique_ptr<Node> root = sol.construct(grid);
    ctx.expect_true(root != nullptr);
    if (root == nullptr) {
      return;
    }
    ctx.expect_true(root->is_leaf);
    ctx.expect_true(root->val);
    ctx.expect_true(isMinimalQuadTree(root.get()));
  });

  ctx.test("single_cell_zero", [&] {
    Solution sol;
    const std::unique_ptr<Node> root = sol.construct({{0}});
    ctx.expect_true(root != nullptr);
    if (root == nullptr) {
      return;
    }
    ctx.expect_true(root->is_leaf);
    ctx.expect_false(root->val);
    ctx.expect_true(isMinimalQuadTree(root.get()));
  });

  ctx.test("checkerboard_2x2", [&] {
    const std::vector<std::vector<int>> grid = {{0, 1}, {1, 0}};
    Solution sol;
    const std::unique_ptr<Node> root = sol.construct(grid);
    ctx.expect_true(root != nullptr);
    if (root == nullptr) {
      return;
    }
    ctx.expect_false(root->is_leaf);
    ctx.expect_true(isMinimalQuadTree(root.get()));

    std::vector<std::vector<int>> rebuilt(2uz, std::vector<int>(2uz, -1));
    expandQuadTree(root.get(), 0uz, 0uz, 2uz, rebuilt);
    ctx.expect_eq(rebuilt, grid);
  });
}

}  // namespace
