// LeetCode 1861: Rotating The Box
// https://leetcode.com/problems/rotating-the-box/
//
// Problem:
//   You are given an m x n character grid `box` representing a side-view of a
//   box where each cell is:
//     - '#' : a movable stone
//     - '*' : a stationary obstacle
//     - '.' : an empty cell
//   The box is rotated 90 degrees clockwise, causing stones to fall straight
//   down under gravity until they rest on an obstacle, another stone, or the
//   bottom of the box. Obstacles never move. Return the resulting n x m grid.
//   Constraints:
//     - 1 <= m, n <= 500
//     - box[r][c] is '#', '*', or '.'
//
// Examples:
//   Example 1:
//     Input:  box = [['#', '.', '#']]
//     Output: [['.'], ['#'], ['#']]
//     Explanation:
//       - Allocate a 3 x 1 rotated grid filled with '.' and map row r = 0 into
//         dest_col = (1 - 1) - 0 = 0 with write_row = 3.
//       - Scan right-to-left: col = 2 ('#') decrements write_row to 2 and sets
//         rotated[2][0] = '#'; col = 1 ('.') leaves rotated[1][0] empty.
//       - At col = 0 ('#'), write_row decrements to 1 and sets rotated[1][0] = '#',
//         producing [['.'], ['#'], ['#']].
//
//   Example 2:
//     Input:  box = [['#', '.', '*', '.'], ['#', '#', '*', '.']]
//     Output: [['#', '.'], ['#', '#'], ['*', '*'], ['.', '.']]
//     Explanation:
//       - Allocate a 4 x 2 rotated grid filled with '.'; row 0 maps to dest_col = 1
//         and row 1 maps to dest_col = 0.
//       - In row 0 (dest_col = 1), '*' at col = 2 writes '*' at rotated[2][1] and
//         resets write_row = 2, so '#' at col = 0 lands at rotated[1][1].
//       - In row 1 (dest_col = 0), '*' at col = 2 writes '*' at rotated[2][0] and
//         resets write_row = 2, packing the stones from col = 1 and col = 0 into
//         rotated[1][0] and rotated[0][0].
//
//   Example 3:
//     Input:  box = [['#', '#', '*', '.', '*', '.'], ['#', '#', '#', '*', '.',
//             '.'], ['#', '#', '#', '.', '#', '.']]
//     Output: [['.', '#', '#'], ['.', '#', '#'], ['#', '#', '*'], ['#', '*', '.'],
//             ['#', '.', '*'], ['#', '.', '.']]
//     Explanation:
//       - Allocate a 6 x 3 rotated grid filled with '.'; rows 0, 1, 2 write to
//         columns 2, 1, 0 respectively.
//       - In row 0 (dest_col = 2), obstacles at col = 4 and col = 2 place '*' at
//         rotated[4][2] and rotated[2][2], leaving stones from col = 1 and col = 0
//         packed at rotated[1][2] and rotated[0][2].
//       - In row 1 (dest_col = 1), the obstacle at col = 3 places '*' at
//         rotated[3][1] and resets write_row = 3, packing the three stones at col =
//         2, 1, 0 into rows 2, 1, 0.
//       - In row 2 (dest_col = 0), no obstacles are present so write_row starts at
//         6 and packs the four stones at col = 4, 2, 1, 0 into rows 5, 4, 3, 2 of
//         column 0.
//
// Algorithm:
//   Rotating 90 degrees clockwise maps original cell (r, c) to rotated cell
//   (c, m - 1 - r). Under this rotation, "falling down" in each column of the
//   rotated grid is identical to stones sliding rightward within each row of
//   the original grid until blocked by an obstacle or the right wall.
//
//   Initialize an n x m result grid filled with '.'. For each original row `r`
//   (which becomes column `dest_col = m - 1 - r` in the rotated grid), scan
//   columns from right to left using a two-pointer pattern:
//     - Maintain `write_row` as the exclusive upper bound (1 past the lowest
//       free landing row) in `rotated[*][dest_col]`, starting at `n`.
//     - On seeing a stone '#', decrement `write_row` and place '#' at
//       `rotated[write_row][dest_col]`.
//     - On seeing an obstacle '*', place '*' directly at
//       `rotated[col][dest_col]` and reset `write_row = col` so subsequent
//       stones to the left land directly above this obstacle.
//     - Empty cells '.' require no action because `rotated` is pre-filled.
//   Time Complexity:  O(m * n), visiting each cell once.
//   Space Complexity: O(1) auxiliary space (excluding the n x m output grid).
//
// Data Structures:
//   - 2D character matrix `std::vector<std::vector<char>>` for the rotated
//     output grid.
//   - Two `std::size_t` pointers (`col` reader, `write_row` landing boundary)
//     per row to simulate gravity in a single pass.
//
// Real-World Applications:
//   - 2D game physics and puzzle engines (Tetris/Match-3/falling-sand gravity
//     compaction inside segmented columns).
//   - Image rotation combined with sparse run compaction on raster grids.
//   - Memory defragmentation within fixed-barrier segments (moving movable
//     blocks tightly against pinned objects).
//
// Related LeetCode Problems:
//   - #48 Rotate Image [done] (in-place 90-degree clockwise matrix rotation)
//   - #283 Move Zeroes [done] (two-pointer stable compaction of elements)
//   - #723 Candy Crush [done] (grid gravity drop compaction)
//   - #1706 Where Will the Ball Fall [new] (2D grid simulation with obstacles)
//   - #1886 Determine Whether Matrix Can Be Obtained By Rotation [new] (matrix rotation coordinate mapping)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

constexpr char kStone = '#';
constexpr char kObstacle = '*';
constexpr char kEmpty = '.';

class Solution {
 public:
  // Simulates 90-degree clockwise rotation and stone gravity in a single pass
  // per row directly into the n x m output matrix.
  //
  // Invariant: While scanning row `r` right-to-left at index `col`, `write_row`
  // satisfies `col < write_row <= cols`, and every cell in
  // `rotated[(col + 1)..(write_row - 1)][dest_col]` is empty.
  // Time: O(m * n), Space: O(1) auxiliary (output is n x m).
  std::vector<std::vector<char>> rotateTheBox(
      const std::vector<std::vector<char>>& box) {
    if (box.empty() || box[0].empty()) {
      return {};
    }

    const std::size_t rows = box.size();
    const std::size_t cols = box[0].size();
    std::vector<std::vector<char>> rotated(cols,
                                           std::vector<char>(rows, kEmpty));

    for (std::size_t r = 0uz; r < rows; ++r) {
      // Original row `r` rotates 90 degrees clockwise into column `dest_col`.
      const std::size_t dest_col = (rows - 1uz) - r;
      // Exclusive upper bound for the next stone's landing row in `dest_col`.
      std::size_t write_row = cols;

      // Scan right-to-left (bottom-to-top in the rotated column) so stones
      // pack tightly against the bottom wall or the nearest obstacle below.
      for (std::size_t col = cols; col-- > 0uz;) {
        const char cell = box[r][col];
        if (cell == kObstacle) {
          rotated[col][dest_col] = kObstacle;
          // Any stones further left cannot fall past this obstacle.
          write_row = col;
        } else if (cell == kStone) {
          --write_row;
          rotated[write_row][dest_col] = kStone;
        }
      }
    }

    return rotated;
  }
};

LC_PROBLEM(1861, "Rotating The Box", "two_pointers", ctx) {
  Solution sol;

  ctx.test("example_1_single_row", [&] {
    const std::vector<std::vector<char>> box = {{'#', '.', '#'}};
    const std::vector<std::vector<char>> expected = {
        {'.'},
        {'#'},
        {'#'},
    };
    ctx.expect_eq(sol.rotateTheBox(box), expected);
  });

  ctx.test("example_2_with_obstacles", [&] {
    const std::vector<std::vector<char>> box = {
        {'#', '.', '*', '.'},
        {'#', '#', '*', '.'},
    };
    const std::vector<std::vector<char>> expected = {
        {'#', '.'},
        {'#', '#'},
        {'*', '*'},
        {'.', '.'},
    };
    ctx.expect_eq(sol.rotateTheBox(box), expected);
  });

  ctx.test("example_3_multiple_segments", [&] {
    const std::vector<std::vector<char>> box = {
        {'#', '#', '*', '.', '*', '.'},
        {'#', '#', '#', '*', '.', '.'},
        {'#', '#', '#', '.', '#', '.'},
    };
    const std::vector<std::vector<char>> expected = {
        {'.', '#', '#'},
        {'.', '#', '#'},
        {'#', '#', '*'},
        {'#', '*', '.'},
        {'#', '.', '*'},
        {'#', '.', '.'},
    };
    ctx.expect_eq(sol.rotateTheBox(box), expected);
  });

  ctx.test("all_empty_and_single_obstacle", [&] {
    const std::vector<std::vector<char>> box = {
        {'.', '.'},
        {'.', '*'},
    };
    const std::vector<std::vector<char>> expected = {
        {'.', '.'},
        {'*', '.'},
    };
    ctx.expect_eq(sol.rotateTheBox(box), expected);
  });

  ctx.test("single_cell", [&] {
    ctx.expect_eq(sol.rotateTheBox({{'#'}}),
                  std::vector<std::vector<char>>{{'#'}});
    ctx.expect_eq(sol.rotateTheBox({{'*'}}),
                  std::vector<std::vector<char>>{{'*'}});
  });
}

}  // namespace
