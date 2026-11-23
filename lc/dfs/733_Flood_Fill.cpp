// LeetCode 733: Flood Fill
// https://leetcode.com/problems/flood-fill/
//
// Problem:
//   Given an `m x n` integer grid `image` representing pixel colors, a
//   starting coordinate `(sr, sc)`, and a replacement `color`, recolor the
//   starting pixel and every pixel 4-directionally connected to it that shares
//   the starting pixel's original color. Return the modified image.
//   Constraints: 1 <= m, n <= 50; 0 <= image[i][j], color < 2^16;
//   0 <= sr < m; 0 <= sc < n.
//
// Algorithm:
//   Record `original_color = image[sr][sc]`. If `original_color == color`,
//   return `image` immediately because no pixel changes and recursing would
//   loop infinitely. Otherwise run a 4-directional recursive DFS starting at
//   `(sr, sc)`: recolor each matching in-bounds pixel to `color` immediately
//   upon visiting (which simultaneously serves as the visited marker), then
//   recurse on its four cardinal neighbors.
//   Time Complexity: O(m * n), since each pixel is visited and recolored at
//     most once.
//   Space Complexity: O(m * n) in the worst case for the DFS call stack when
//     all pixels belong to a single snake-shaped region.
//
// Data Structures:
//   - std::vector<std::vector<int>> `image`: 2D grid mutated in place; setting
//     a cell to `color` acts as an implicit visited set.
//   - std::array<std::pair<int, int>, 4> `kDirs`: static 4-directional offsets.
//
// Real-World Applications:
//   - Raster graphics editors (Photoshop/GIMP "paint bucket" fill tool).
//   - Connected-component labeling in computer vision and medical image
//     segmentation.
//   - Board game territory evaluation (e.g., Go liberties / Minesweeper
//     blank-cell reveal).
//
// Related LeetCode Problems:
//   - #200 Number of Islands [done] (4-directional DFS/BFS connected components on a grid)
//   - #695 Max Area of Island [done] (computing size of each 4-connected grid component)
//   - #130 Surrounded Regions [done] (border-seeded flood fill to mark escaped regions)
//   - #1034 Coloring A Border [done] (flood fill that only recolors component boundary cells)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Flood-fills the 4-connected region starting at `(sr, sc)` with `color`.
  // Time: O(m * n); Space: O(m * n) worst-case recursion depth.
  std::vector<std::vector<int>> floodFill(std::vector<std::vector<int>> image,
                                          int sr, int sc, int color) {
    if (image.empty() || image[0].empty()) {
      return image;
    }

    const int rows = static_cast<int>(image.size());
    const int cols = static_cast<int>(image[0].size());
    const int original_color =
        image[static_cast<std::size_t>(sr)][static_cast<std::size_t>(sc)];

    // Guard against infinite recursion when the target color already matches.
    if (original_color == color) {
      return image;
    }

    static constexpr std::array<std::pair<int, int>, 4> kDirs{
        {{0, 1}, {0, -1}, {1, 0}, {-1, 0}}};

    // Recursively paint every 4-connected pixel matching `original_color`.
    auto dfs = [&](this auto&& self, int r, int c) -> void {
      if (r < 0 || r >= rows || c < 0 || c >= cols) {
        return;
      }
      auto& pixel =
          image[static_cast<std::size_t>(r)][static_cast<std::size_t>(c)];
      if (pixel != original_color) {
        return;
      }

      // Updating the pixel before recursing prevents revisiting this cell.
      pixel = color;
      for (const auto& [dr, dc] : kDirs) {
        self(r + dr, c + dc);
      }
    };

    dfs(sr, sc);
    return image;
  }
};

LC_PROBLEM(733, "Flood Fill", "dfs", ctx) {
  ctx.test("example_1_center_region", [&] {
    Solution sol;
    const std::vector<std::vector<int>> image = {
        {1, 1, 1},
        {1, 1, 0},
        {1, 0, 1},
    };
    const std::vector<std::vector<int>> expected = {
        {2, 2, 2},
        {2, 2, 0},
        {2, 0, 1},
    };
    ctx.expect_eq(sol.floodFill(image, 1, 1, 2), expected);
  });

  ctx.test("example_2_same_color_noop", [&] {
    Solution sol;
    const std::vector<std::vector<int>> image = {
        {0, 0, 0},
        {0, 1, 1},
    };
    ctx.expect_eq(sol.floodFill(image, 1, 1, 1), image);
  });

  ctx.test("all_zeros_same_color", [&] {
    Solution sol;
    const std::vector<std::vector<int>> image = {
        {0, 0, 0},
        {0, 0, 0},
    };
    ctx.expect_eq(sol.floodFill(image, 0, 0, 0), image);
  });

  ctx.test("single_pixel_image", [&] {
    Solution sol;
    const std::vector<std::vector<int>> image = {{5}};
    const std::vector<std::vector<int>> expected = {{9}};
    ctx.expect_eq(sol.floodFill(image, 0, 0, 9), expected);
  });

  ctx.test("diagonals_not_connected", [&] {
    Solution sol;
    const std::vector<std::vector<int>> image = {
        {1, 0, 1},
        {0, 1, 0},
        {1, 0, 1},
    };
    const std::vector<std::vector<int>> expected = {
        {1, 0, 1},
        {0, 3, 0},
        {1, 0, 1},
    };
    ctx.expect_eq(sol.floodFill(image, 1, 1, 3), expected);
  });
}

}  // namespace
