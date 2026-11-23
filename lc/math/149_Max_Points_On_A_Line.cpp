// LeetCode 149: Max Points On A Line
// https://leetcode.com/problems/max-points-on-a-line/
//
// Problem:
//   Given an array `points` where `points[i] = [xi, yi]` represents a point on
//   the 2D Cartesian plane, return the maximum number of points that lie on the
//   same straight line. Constraints: 1 <= points.size() <= 300,
//   -10^4 <= xi, yi <= 10^4. (Duplicate points are also handled cleanly.)
//
// Examples:
//   Example 1:
//     Input:  points = [[1, 1], [3, 2], [5, 3], [4, 1], [2, 3], [1, 4]]
//     Output: 4
//     Explanation:
//       - With n = 6 > 2, initialize best = 2 and test anchor i = 0 ([1, 1])
//         against j = 1..5, finding reduced slopes (2, 1) twice, (1, 0), (1, 2),
//         and (0, 1), which updates best to local_max + duplicates = 2 + 1 = 3.
//       - At anchor i = 1 ([3, 2]), j = 2..5 yield slope (2, 1) once and normalized
//         slope (1, -1) three times (from [4, 1], [2, 3], and [1, 4]), updating
//         best to 3 + 1 = 4.
//       - At anchor i = 2, n - i = 4 <= best (4) triggers early pruning, exiting
//         the outer loop and returning 4.
//
//   Example 2:
//     Input:  points = [[2, -5], [2, 0], [2, 7], [2, 100], [-1, 0], [5, 0]]
//     Output: 4
//     Explanation:
//       - Initialize best = 2 and inspect anchor i = 0 ([2, -5]): points [2, 0],
//         [2, 7], and [2, 100] each have dx = 0 and positive dy, normalizing to
//         canonical vertical slope (0, 1) with count 3.
//       - Points [-1, 0] and [5, 0] produce distinct slopes (3, -5) and (3, 5), so
//         anchor i = 0 finishes with local_max = 3 and duplicates = 1, updating
//         best to 4.
//       - Anchor i = 1 ([2, 0]) records vertical slope (0, 1) twice and horizontal
//         slope (1, 0) twice (keeping best = 4), and anchor i = 2 exits via early
//         pruning (6 - 2 <= 4) to return 4.
//
//   Example 3:
//     Input:  points = [[0, 0], [0, 0], [1, 1], [2, 2], [1, 0]]
//     Output: 4
//     Explanation:
//       - Initialize best = 2 and start anchor i = 0 ([0, 0]) with duplicates = 1;
//         j = 1 ([0, 0]) has dx = 0 and dy = 0, incrementing duplicates to 2.
//       - Points j = 2 ([1, 1]) and j = 3 ([2, 2]) both reduce by their GCD to
//         slope (1, 1) so slope_counts[(1, 1)] = 2, while j = 4 ([1, 0]) yields
//         slope (1, 0).
//       - Anchor i = 0 sets best = max(2, local_max + duplicates) = 2 + 2 = 4, and
//         i = 1 stops via early pruning (5 - 1 <= 4) and returns 4.
//
// Algorithm:
//   Anchor-point slope hashing with exact rational reduction.
//   Fix each point `i` as an anchor and examine every subsequent point `j > i`.
//   Two points `j` and `k` are collinear with `i` if and only if their direction
//   vectors `(dx, dy)` from `i` are scalar multiples of one another. To avoid
//   floating-point precision errors on large or nearly equal slopes, reduce
//   `(dx, dy)` by `gcd(|dx|, |dy|)` and canonicalize its sign so that `dx > 0`
//   (or `dx == 0 && dy == 1` for vertical lines, `dx == 1 && dy == 0` for
//   horizontal lines). Count frequencies of each canonical slope in a hash map
//   for anchor `i`, add the count of points identical to `i` (including `i`
//   itself), and take the global maximum across all anchors.
//   Time Complexity:  O(n^2 log M), where M is the coordinate span (for GCD).
//   Space Complexity: O(n) for the per-anchor slope frequency table.
//
// Data Structures:
//   - Slope struct (normalized `(dx, dy)` pair) with a custom hash functor:
//     represents exact reduced rational slopes without floating-point rounding.
//   - std::unordered_map<Slope, int, SlopeHash>: counts collinear points per
//     canonical direction passing through the current anchor point.
//
// Real-World Applications:
//   - Computer vision line detection (discrete Hough-transform / RANSAC voting
//     for collinear edges and lane markings in LiDAR and camera point clouds).
//   - Computational geometry degeneracy detection and collinearity testing in
//     CAD, GIS map simplification, and computational crystallography.
//   - Astronomy and particle-physics track reconstruction identifying linear
//     trajectories from noisy 2D detector hits.
//
// Related LeetCode Problems:
//   - #1232 Check If It Is a Straight Line [new] (checks if all given points are collinear in O(n))
//   - #2280 Minimum Lines to Represent a Line Chart [new] (uses exact cross-product / slope equality)
//   - #356 Line Reflection [done] (2D point symmetry using hash-based coordinate lookup)
//   - #447 Number of Boomerangs [done] (anchor-point distance hashing in O(n^2))

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <numeric>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

// Canonical reduced direction vector (dx, dy) representing an exact line slope.
struct Slope {
  std::int64_t dx = 0;
  std::int64_t dy = 0;

  [[nodiscard]] bool operator==(const Slope& other) const = default;
};

// Hash functor combining the two 64-bit slope components.
struct SlopeHash {
  [[nodiscard]] std::size_t operator()(const Slope& slope) const noexcept {
    const std::size_t h1 = std::hash<std::int64_t>{}(slope.dx);
    const std::size_t h2 = std::hash<std::int64_t>{}(slope.dy);
    return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6U) + (h1 >> 2U));
  }
};

class Solution {
 public:
  // Finds the maximum number of collinear points in `points`.
  //
  // Invariant: for each anchor `i`, we only scan `j > i` because any line
  // containing a point with index `< i` was already evaluated when that earlier
  // point served as the anchor.
  //
  // Time: O(n^2 log M), Space: O(n).
  int maxPoints(const std::vector<std::vector<int>>& points) {
    const std::size_t n = points.size();
    if (n <= 2uz) {
      return static_cast<int>(n);
    }

    int best = 2;
    std::unordered_map<Slope, int, SlopeHash> slope_counts;
    slope_counts.reserve(n);

    for (std::size_t i = 0uz; i < n; ++i) {
      // Early pruning: even if all remaining points (plus anchor i) were
      // collinear, they could not beat `best`.
      if (std::cmp_less_equal(n - i, best)) {
        break;
      }

      slope_counts.clear();
      // `duplicates` counts points coinciding with anchor `i` (including `i`).
      int duplicates = 1;
      int local_max = 0;

      for (std::size_t j = i + 1uz; j < n; ++j) {
        const std::int64_t dx = static_cast<std::int64_t>(points[j][0]) -
                                static_cast<std::int64_t>(points[i][0]);
        const std::int64_t dy = static_cast<std::int64_t>(points[j][1]) -
                                static_cast<std::int64_t>(points[i][1]);

        if (dx == 0 && dy == 0) {
          ++duplicates;
          continue;
        }

        const Slope slope = normalize_slope(dx, dy);
        local_max = std::max(local_max, ++slope_counts[slope]);
      }

      best = std::max(best, local_max + duplicates);
    }

    return best;
  }

 private:
  // Reduces (dx, dy) by gcd(|dx|, |dy|) and enforces a unique canonical sign:
  //   - vertical lines   -> (0, 1)
  //   - horizontal lines -> (1, 0)
  //   - general lines    -> dx > 0 and gcd(dx, |dy|) == 1
  static Slope normalize_slope(std::int64_t dx, std::int64_t dy) {
    // std::gcd always returns a non-negative integer; since (dx, dy) != (0, 0),
    // g >= 1 is guaranteed.
    const std::int64_t g = std::gcd(dx, dy);
    dx /= g;
    dy /= g;

    // Flip orientation so opposite rays along the same line share one key.
    if (dx < 0 || (dx == 0 && dy < 0)) {
      dx = -dx;
      dy = -dy;
    }
    return Slope{.dx = dx, .dy = dy};
  }
};

LC_PROBLEM(149, "Max Points On A Line", "math", ctx) {
  Solution sol;

  ctx.test("example_1_all_collinear", [&] {
    const std::vector<std::vector<int>> points = {{1, 1}, {2, 2}, {3, 3}};
    ctx.expect_eq(sol.maxPoints(points), 3);
  });

  ctx.test("example_2_diagonal_of_four", [&] {
    const std::vector<std::vector<int>> points = {{1, 1}, {3, 2}, {5, 3},
                                                  {4, 1}, {2, 3}, {1, 4}};
    ctx.expect_eq(sol.maxPoints(points), 4);
  });

  ctx.test("single_and_two_points", [&] {
    ctx.expect_eq(sol.maxPoints({{4, -2}}), 1);
    ctx.expect_eq(sol.maxPoints({{0, 0}, {5, -3}}), 2);
  });

  ctx.test("vertical_and_horizontal_lines", [&] {
    const std::vector<std::vector<int>> points = {
        {2, -5}, {2, 0}, {2, 7}, {2, 100}, {-1, 0}, {5, 0}};
    ctx.expect_eq(sol.maxPoints(points), 4);
  });

  ctx.test("float_precision_trap", [&] {
    // Slopes 94911150/94911151 and 94911151/94911152 collide in IEEE-754 float
    // (and nearly in double), verifying exact integer GCD normalization.
    const std::vector<std::vector<int>> points = {
        {0, 0}, {94911151, 94911150}, {94911152, 94911151}};
    ctx.expect_eq(sol.maxPoints(points), 2);
  });

  ctx.test("duplicate_points", [&] {
    const std::vector<std::vector<int>> points = {
        {0, 0}, {0, 0}, {1, 1}, {2, 2}, {1, 0}};
    ctx.expect_eq(sol.maxPoints(points), 4);
  });
}

}  // namespace
