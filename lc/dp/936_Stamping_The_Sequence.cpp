// LeetCode 936: Stamping The Sequence
// https://leetcode.com/problems/stamping-the-sequence/
//
// Problem:
//   Starting from a string of `target.size()` '?' characters, you may stamp
//   `stamp` onto any in-bounds substring of length `stamp.size()`, overwriting
//   its characters. Return a sequence of at most `10 * target.size()` leftmost
//   stamp indices that transforms the initial string into `target`, or an empty
//   vector if `target` cannot be formed.
//
// Algorithm:
//   Top-down DFS with memoization over `(sidx, tidx)` pairs, where `sidx` is
//   the current position in `stamp` and `tidx` is the current position in
//   `target`, paired with a zero-copy backtracking `std::deque<int>` storing
//   the stamp order:
//     - Base case (`tidx == target.size()`): succeeds iff `sidx == stamp.size()`
//       (the final stamp ends cleanly at the end of `target`).
//     - Stamp completed (`sidx == stamp.size()`): an earlier stamp placed
//       underneath can become visible starting at any offset `idx` in
//       `[0, stamp.size())`, corresponding to position `pos = tidx - idx`. We
//       prepend `pos` to the front of `moves` (stamped before the current
//       stamp) and recurse to `(idx, tidx)`.
//     - Character match (`stamp[sidx] == target[tidx]`): either continue
//       matching the current stamp at `(sidx + 1, tidx + 1)`, or overlay a new
//       later stamp starting at `tidx + 1` (appending `tidx + 1` to the back of
//       `moves`) and recurse to `(0, tidx + 1)`.
//   Failed `(sidx, tidx)` states are memoized in a flat boolean table so each
//   unreachable state is explored at most once. Every added stamp advances
//   `tidx` by at least one, guaranteeing at most `target.size()` moves.
//   Time complexity: O(M * N * M) worst-case, where M = stamp.size() and
//   N = target.size() (often near O(N) in practice).
//   Space complexity: O(M * N) for the memoized failure table plus O(N) stack.
//
// Data Structures:
//   - std::vector<std::uint8_t>: Flat `(M + 1) x (N + 1)` memoization table
//     marking dead `(sidx, tidx)` states.
//   - std::deque<int>: Double-ended move buffer supporting O(1) push/pop at
//     both ends (prepending earlier underlying stamps, appending later
//     overlapping stamps).
//
// Real-World Applications:
//   - Painter's algorithm / layered UI compositing order reconstruction (z-order
//     recovery from a final rasterized strip).
//   - Reverse patch / overwrite-sequence synthesis in firmware flashing and
//     memory-block initialization.
//   - Template-based DNA / string assembly verification.
//
// Related LeetCode Problems:
//   - #1591 Strange Printer II [new] (2D rectangular stamping / topological order)
//   - #664 Strange Printer [new] (minimum interval-painting DP)
//   - #207 Course Schedule [done] (dependency-order recovery)
//   - #546 Remove Boxes [new] (interval DP over nested overwrites)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <deque>
#include <string>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Returns a valid sequence of leftmost stamp indices that produces `target`,
  // or an empty vector if stamping `target` is impossible.
  //
  // Invariant: Every index placed into `moves` is strictly in `[0, t_len - s_len]`
  // and contributes at least one visible character to `target`, ensuring the
  // move count never exceeds `target.size()`.
  // Time: O(M^2 * N) worst-case; Space: O(M * N).
  std::vector<int> movesToStamp(std::string_view stamp,
                                std::string_view target) {
    const std::size_t s_len = stamp.size();
    const std::size_t t_len = target.size();
    if (s_len == 0uz || t_len < s_len) {
      return {};
    }

    // Memoize states (sidx, tidx) proven unable to complete `target[tidx..]`.
    SearchState state{
        .stamp = stamp,
        .target = target,
        .stride = t_len + 1uz,
        .dead = std::vector<std::uint8_t>((s_len + 1uz) * (t_len + 1uz), 0),
        .moves = {0},
    };

    if (!dfs(state, 0uz, 0uz)) {
      return {};
    }
    return {state.moves.begin(), state.moves.end()};
  }

 private:
  struct SearchState {
    std::string_view stamp;
    std::string_view target;
    std::size_t stride = 0uz;
    std::vector<std::uint8_t> dead;
    std::deque<int> moves;
  };

  // Attempts to reveal an earlier stamp placed beneath the just-completed stamp
  // starting at offset `idx` in `stamp` (`pos = tidx - idx`), prepending `pos`
  // to `state.moves` before recursing.
  static bool tryRevealUnderlyingStamp(SearchState& state, std::size_t tidx) {
    const std::size_t s_len = state.stamp.size();
    const std::size_t t_len = state.target.size();
    for (std::size_t idx = 0uz; idx < s_len; ++idx) {
      const std::size_t pos = tidx - idx;
      if (pos + s_len > t_len) {
        continue;
      }
      state.moves.push_front(static_cast<int>(pos));
      if (dfs(state, idx, tidx)) {
        return true;
      }
      state.moves.pop_front();
    }
    return false;
  }

  // Recursive memoized DFS over `(sidx, tidx)` with zero-copy deque
  // backtracking. Returns true as soon as a valid full stamping order is found.
  static bool dfs(SearchState& state, std::size_t sidx, std::size_t tidx) {
    const std::size_t s_len = state.stamp.size();
    const std::size_t t_len = state.target.size();
    if (tidx == t_len) {
      return sidx == s_len;
    }

    const std::size_t key = (sidx * state.stride) + tidx;
    if (state.dead[key] != 0) {
      return false;
    }

    if (sidx == s_len) {
      if (tryRevealUnderlyingStamp(state, tidx)) {
        return true;
      }
      state.dead[key] = 1;
      return false;
    }

    if (state.stamp[sidx] == state.target[tidx]) {
      // Option 1: Keep matching characters from the current stamp.
      if (dfs(state, sidx + 1uz, tidx + 1uz)) {
        return true;
      }
      // Option 2: Cut off the current stamp here by overlaying a later stamp
      // starting at `tidx + 1`.
      if (tidx + 1uz + s_len <= t_len) {
        state.moves.push_back(static_cast<int>(tidx + 1uz));
        if (dfs(state, 0uz, tidx + 1uz)) {
          return true;
        }
        state.moves.pop_back();
      }
    }

    state.dead[key] = 1;
    return false;
  }
};

// Simulates applying `moves` of `stamp` onto an initial string of '?' of length
// `target_len` and returns the resulting string (or empty on invalid index).
std::string simulateStamps(std::string_view stamp, std::size_t target_len,
                           const std::vector<int>& moves) {
  std::string canvas(target_len, '?');
  for (const int pos : moves) {
    if (pos < 0) {
      return "";
    }
    const auto start = static_cast<std::size_t>(pos);
    if (start + stamp.size() > target_len) {
      return "";
    }
    for (std::size_t j = 0uz; j < stamp.size(); ++j) {
      canvas[start + j] = stamp[j];
    }
  }
  return canvas;
}

LC_PROBLEM(936, "Stamping The Sequence", "dp", ctx) {
  Solution sol;

  auto expect_valid_stamping = [&](std::string_view stamp,
                                   std::string_view target) {
    const std::vector<int> moves = sol.movesToStamp(stamp, target);
    ctx.expect_false(moves.empty(), "expected non-empty move sequence");
    ctx.expect_true(moves.size() <= 10uz * target.size(),
                    "move count must not exceed 10 * target.size()");
    ctx.expect_eq(simulateStamps(stamp, target.size(), moves),
                  std::string(target));
  };

  ctx.test("example_1_ababc", [&] {
    expect_valid_stamping("abc", "ababc");
  });

  ctx.test("example_2_aabcaca", [&] {
    expect_valid_stamping("abca", "aabcaca");
  });

  ctx.test("middle_stamp_overwritten_both_sides", [&] {
    expect_valid_stamping("abc", "abcbabc");
  });

  ctx.test("python_build_long_valid_case", [&] {
    expect_valid_stamping("qxq", "qxqxqxqqxqxqqxqxqxqqxqxqqqxqqxqqqxq");
  });

  ctx.test("python_build_impossible_cases", [&] {
    ctx.expect_true(
        sol.movesToStamp("qxq", "qxqxqxqqxqxqqxqxqxqqxqxqqqxqqxqqqxqqxyq")
            .empty());
    ctx.expect_true(sol.movesToStamp("qxq", "qxqqxxq").empty());
  });

  ctx.test("exact_match_and_short_impossible", [&] {
    ctx.expect_eq(sol.movesToStamp("abc", "abc"), std::vector<int>{0});
    ctx.expect_true(sol.movesToStamp("abc", "ab").empty());
  });
}

}  // namespace
