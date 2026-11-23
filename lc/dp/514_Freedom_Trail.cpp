// LeetCode 514: Freedom Trail
// https://leetcode.com/problems/freedom-trail/
//
// Problem:
//   Given a circular dial represented by string `ring` (initially aligned with
//   `ring[0]` at the 12:00 position) and a target string `key`, compute the
//   minimum number of steps needed to spell every character of `key` in order.
//   Rotating the dial one position clockwise or counterclockwise costs 1 step,
//   and pressing the center button to spell the aligned character costs 1 step.
//   Constraints: 1 <= ring.size(), key.size() <= 100; both contain lowercase
//   English letters and `key` is guaranteed to be spellable from `ring`.
//
// Examples:
//   Example 1:
//     Input:  ring = "godding", key = "gd"
//     Output: 4
//     Explanation:
//       - Index ring positions ('g' at [0, 6], 'd' at [2, 3]) and start with states
//         = {(0, 0)}.
//       - For key[0] = 'g', transition from (0, 0) to index 0 (distance 0) and
//         index 6 (wrap-around distance 1), yielding states = {(0, 0), (6, 1)}.
//       - For key[1] = 'd', take minimum transition cost to index 2 (min(0 + 2, 1 +
//         3) = 2) and index 3 (min(0 + 3, 1 + 3) = 3), yielding states = {(2, 2),
//         (3, 3)}.
//       - Take min_rotations = min(2, 3) = 2 and add key.size() = 2 button presses
//         to return 4.
//
//   Example 2:
//     Input:  ring = "iotfo", key = "fioot"
//     Output: 11
//     Explanation:
//       - Index ring positions ('f': [3], 'i': [0], 'o': [1, 4], 't': [2]) on
//         ring_len = 5 and start with states = {(0, 0)}.
//       - Spell 'f' then 'i' via unique indices 3 (cost 2) and 0 (cost 2 + 2 = 4),
//         yielding states = {(0, 4)}.
//       - Spell the first 'o' at indices 1 and 4 (both distance 1 from 0) for
//         states = {(1, 5), (4, 5)}, and stay at those indices for the second 'o'
//         at cost 5 each.
//       - Spell 't' at index 2: moving from (1, 5) costs 5 + 1 = 6 while moving
//         from (4, 5) costs 5 + 2 = 7, so min_rotations = 6.
//       - Add key.size() = 5 button presses to min_rotations = 6 and return 11.
//
//   Example 3:
//     Input:  ring = "abc", key = "aaaa"
//     Output: 4
//     Explanation:
//       - Index 'a' at [0] on ring_len = 3 and initialize states = {(0, 0)}.
//       - For each of the 4 'a' characters in key, transition from (0, 0) to index
//         0 with ring_distance(0, 0, 3) = 0, keeping states = {(0, 0)}.
//       - Add key.size() = 4 button presses to min_rotations = 0 to return 4.
//
// Algorithm:
//   Use layered dynamic programming over the characters of `key`, pre-indexing
//   all occurrences of each letter `'a'..'z'` in `ring`.
//   Maintain a sparse frontier `states` of `(ring_pos, min_rotations)` pairs
//   representing the minimum rotation steps needed to finish spelling the
//   current prefix of `key` while ending with index `ring_pos` at 12:00.
//   Initially `states = {(0, 0)}`. For each target character `ch` in `key`:
//     - For every index `to` where `ring[to] == ch`, compute:
//         cost[to] = min_{(from, prev_cost) in states}
//                      (prev_cost + ring_distance(from, to))
//       where `ring_distance(a, b) = min(|a - b|, R - |a - b|)`.
//     - Replace `states` with the new frontier.
//   The answer is `min_rotations + key.size()`, adding 1 button-press step per
//   character in `key`.
//   Time Complexity:  O(K * R^2) worst case (when all characters in `ring` are
//                     identical), where K = key.size() and R = ring.size().
//   Space Complexity: O(R) for the character-position index and current/next
//                     DP frontiers.
//
// Data Structures:
//   - Fixed-size position index (`std::array<std::vector<std::size_t>, 26>`):
//     maps each lowercase letter to all its indices on `ring` in O(1) lookup.
//   - Sparse DP state frontier (`std::vector<std::pair<std::size_t, int>>`):
//     tracks only valid ring indices matching the most recently spelled key
//     character along with their optimal rotation costs.
//
// Real-World Applications:
//   - CNC turret tool-changer and rotary carousel optimization minimizing
//     angular travel across a sequence of manufacturing operations.
//   - Automated tape/disk-carousel retrieval systems planning shortest
//     bidirectional head movements across replicated storage slots.
//   - Viterbi-style shortest-path decoding over layered state graphs with
//     cyclic transition costs.
//
// Related LeetCode Problems:
//   - #752 Open the Lock [done] (circular dial state transitions with shortest-path search)
//   - #1320 Minimum Distance to Type a Word Using Two Fingers [done] (layered DP minimizing movement distance)
//   - #1974 Minimum Time to Type Word Using Special Typewriter [new] (circular alphabet distance greedy)
//   - #1235 Maximum Profit in Job Scheduling [done] (staged DP over sequential decisions)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <string_view>
#include <utility>
#include <vector>

namespace {

constexpr std::size_t kAlphabetSize = 26uz;

class Solution {
 public:
  // Finds the minimum total steps (rotations + button presses) to spell `key`
  // on circular dial `ring` using layered dynamic programming.
  // Time: O(K * R^2) worst case, Space: O(R).
  int findRotateSteps(std::string_view ring, std::string_view key) {
    if (ring.empty() || key.empty()) {
      return 0;
    }

    const std::size_t ring_len = ring.size();

    // Precompute every dial index where each lowercase letter appears.
    std::array<std::vector<std::size_t>, kAlphabetSize> char_positions;
    for (std::size_t i = 0uz; i < ring_len; ++i) {
      const auto bucket = static_cast<std::size_t>(ring[i] - 'a');
      char_positions[bucket].push_back(i);
    }

    // `states` holds `(dial_index, min_rotation_steps)` after spelling the
    // processed prefix of `key`. Initially index 0 is aligned at 12:00 with
    // zero rotation cost.
    std::vector<std::pair<std::size_t, int>> states = {{0uz, 0}};

    for (const char target_ch : key) {
      const auto bucket = static_cast<std::size_t>(target_ch - 'a');
      const auto& targets = char_positions[bucket];

      std::vector<std::pair<std::size_t, int>> next_states;
      next_states.reserve(targets.size());

      // For each candidate occurrence of `target_ch`, choose the cheapest
      // predecessor state from the previous layer.
      for (const std::size_t to : targets) {
        int best = std::numeric_limits<int>::max();
        for (const auto& [from, prev_cost] : states) {
          const int candidate =
              prev_cost + ring_distance(from, to, ring_len);
          best = std::min(best, candidate);
        }
        next_states.emplace_back(to, best);
      }

      states = std::move(next_states);
    }

    // Add `key.size()` for pressing the center button once per spelled letter.
    int min_rotations = std::numeric_limits<int>::max();
    for (const auto& entry : states) {
      min_rotations = std::min(min_rotations, entry.second);
    }
    return min_rotations + static_cast<int>(key.size());
  }

 private:
  // Shortest circular distance (clockwise or counterclockwise) between two
  // indices on a ring of length `ring_len`.
  [[nodiscard]] static int ring_distance(std::size_t from, std::size_t to,
                                         std::size_t ring_len) noexcept {
    const std::size_t direct = (from >= to) ? (from - to) : (to - from);
    return static_cast<int>(std::min(direct, ring_len - direct));
  }
};

LC_PROBLEM(514, "Freedom Trail", "dp", ctx) {
  ctx.test("example_1_godding_gd", [&] {
    Solution sol;
    ctx.expect_eq(sol.findRotateSteps("godding", "gd"), 4);
  });

  ctx.test("example_2_godding_full", [&] {
    Solution sol;
    ctx.expect_eq(sol.findRotateSteps("godding", "godding"), 13);
  });

  ctx.test("wrap_around_counterclockwise", [&] {
    Solution sol;
    // 'a'(0) -> 'd'(3, dist 2 ccw) -> 'e'(4, dist 1 cw) => 3 rotations + 3 presses.
    ctx.expect_eq(sol.findRotateSteps("abcde", "ade"), 6);
  });

  ctx.test("repeated_same_character", [&] {
    Solution sol;
    // Already aligned at 'a'; 4 button presses with 0 rotations.
    ctx.expect_eq(sol.findRotateSteps("abc", "aaaa"), 4);
  });

  ctx.test("multiple_occurrences_non_greedy_choice", [&] {
    Solution sol;
    // Two 'o' positions at equal distance from 'i'; choosing index 1 minimizes
    // the subsequent move to 't' at index 2 (6 rotations + 5 presses = 11).
    ctx.expect_eq(sol.findRotateSteps("iotfo", "fioot"), 11);
  });
}

}  // namespace
