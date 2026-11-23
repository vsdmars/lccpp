// LeetCode 920: Number Of Music Playlists
// https://leetcode.com/problems/number-of-music-playlists/
//
// Problem:
//   You have `n` distinct songs and want to build a playlist of length `goal`
//   such that:
//     1. Every song is played at least once.
//     2. A song can be played again only after at least `k` other songs have
//        been played since its previous occurrence.
//   Return the number of valid playlists modulo 10^9 + 7 (`0 <= k < n <= goal <= 100`).
//
// Examples:
//   Example 1:
//     Input:  n = 3, goal = 3, k = 1
//     Output: 6
//     Explanation:
//       - Initialize prev = [1, 0, 0, 0] for an empty playlist of length 0 with 0
//         distinct songs.
//       - At i = 1, only j = 1 is reachable by picking a new song, giving curr[1] =
//         prev[0] * 3 = 3 and prev = [0, 3, 0, 0].
//       - At i = 2, j = 1 gets 0 (since 1 > 1 is false) and j = 2 adds a second new
//         song in prev[1] * 2 = 6 ways, giving prev = [0, 0, 6, 0].
//       - At i = 3, curr[3] = prev[2] * (3 - 3 + 1) = 6 * 1 = 6 while j = 2 replays
//         in prev[2] * (2 - 1) = 6 ways, so prev[3] = 6 is returned.
//
//   Example 2:
//     Input:  n = 3, goal = 4, k = 1
//     Output: 18
//     Explanation:
//       - Advance through lengths i = 1..3 as above to reach prev = [0, 0, 6, 6].
//       - At i = 4 and j = 2, no new-song transition exists (prev[1] = 0) and
//         replaying outside the last k = 1 song gives curr[2] = prev[2] * (2 - 1) =
//         6.
//       - At i = 4 and j = 3, picking the third song from j = 2 adds prev[2] * 1 =
//         6 and replaying one of 3 - 1 = 2 eligible songs adds prev[3] * 2 = 12,
//         giving curr[3] = 18.
//       - Swap prev and curr and return prev[3] = 18.
//
//   Example 3:
//     Input:  n = 1, goal = 5, k = 0
//     Output: 1
//     Explanation:
//       - Initialize prev = [1, 0] for n = 1 and k = 0.
//       - At i = 1 and j = 1, Case 1 picks the single song in prev[0] * 1 = 1 way,
//         setting prev = [0, 1].
//       - For each subsequent length i = 2..5 at j = 1, Case 2 is enabled (1 > 0)
//         and contributes prev[1] * (1 - 0) = 1, keeping prev[1] = 1 until
//         returning 1.
//
// Algorithm:
//   Bottom-up combinatorial dynamic programming. Let `dp[i][j]` be the number
//   of valid playlists of length `i` containing exactly `j` distinct songs,
//   with base case `dp[0][0] = 1`. When appending the `i`-th song:
//     - Case 1 (pick a brand-new song): choose any of the `n - (j - 1)` unused
//       songs, contributing `dp[i - 1][j - 1] * (n - j + 1)`.
//     - Case 2 (replay an old song): valid only when `j > k`, because the `k`
//       most recently played songs are necessarily distinct (otherwise they
//       would have violated the cooldown constraint) and blocked from replay,
//       leaving `j - k` eligible songs and contributing `dp[i - 1][j] * (j - k)`.
//   Because row `i` depends only on row `i - 1`, we roll two 1D vectors of
//   length `n + 1` in O(n) space.
//   Time complexity: O(goal * n).
//   Space complexity: O(n).
//
// Data Structures:
//   - Two 1D std::vector<std::int64_t> rows (`prev`, `curr`) of length `n + 1`
//     storing modular playlist counts for lengths `i - 1` and `i`.
//
// Real-World Applications:
//   - Streaming music and ad-rotation schedulers enforcing minimum recency
//     cooldowns and full-catalog coverage guarantees.
//   - Combinatorial counting of constrained Markov sequences and surjective
//     mappings (generalized Stirling numbers of the second kind).
//   - Load-balancer round-robin / cooldown test-vector enumeration.
//
// Related LeetCode Problems:
//   - #621 Task Scheduler [done] (cooldown scheduling for tasks)
//   - #1220 Count Vowels Permutation [new] (modular transition-count sequence DP)
//   - #552 Student Attendance Record II [done] (modular constrained sequence DP)
//   - #1866 Number of Ways to Rearrange Sticks With K Sticks Visible [new] (Stirling-style 2D DP)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace {

constexpr std::int64_t kMod = 1'000'000'007;

class Solution {
 public:
  // Counts the number of valid playlists of length `goal` using all `n` songs
  // with a repeat cooldown of `k` other songs, modulo 10^9 + 7.
  //
  // Invariant: After outer iteration `i`, `prev[j]` equals the number of valid
  // length-`i` playlists that contain exactly `j` distinct songs (mod kMod).
  // Time: O(goal * n); Space: O(n).
  int numMusicPlaylists(int n, int goal, int k) {
    if (n <= 0 || goal < n || k < 0 || k >= n) {
      return 0;
    }

    const auto num_songs = static_cast<std::size_t>(n);
    const auto target_len = static_cast<std::size_t>(goal);

    std::vector<std::int64_t> prev(num_songs + 1uz, 0);
    std::vector<std::int64_t> curr(num_songs + 1uz, 0);
    prev[0] = 1;  // One empty playlist of length 0 with 0 distinct songs.

    for (std::size_t i = 1uz; i <= target_len; ++i) {
      std::ranges::fill(curr, 0);
      const std::size_t max_distinct = std::min(i, num_songs);

      for (std::size_t j = 1uz; j <= max_distinct; ++j) {
        const auto distinct = static_cast<std::int64_t>(j);

        // Case 1: Append one of the `n - (j - 1)` never-played songs.
        std::int64_t ways =
            (prev[j - 1uz] * (static_cast<std::int64_t>(n) - distinct + 1)) %
            kMod;

        // Case 2: Replay an existing song outside the last `k` distinct plays.
        if (distinct > k) {
          ways = (ways + ((prev[j] * (distinct - k)) % kMod)) % kMod;
        }

        curr[j] = ways;
      }

      std::swap(prev, curr);
    }

    return static_cast<int>(prev[num_songs]);
  }
};

LC_PROBLEM(920, "Number Of Music Playlists", "dp", ctx) {
  Solution sol;

  ctx.test("example_1_permutations", [&] {
    ctx.expect_eq(sol.numMusicPlaylists(3, 3, 1), 6);
  });

  ctx.test("example_2_no_cooldown", [&] {
    ctx.expect_eq(sol.numMusicPlaylists(2, 3, 0), 6);
  });

  ctx.test("example_3_alternating", [&] {
    ctx.expect_eq(sol.numMusicPlaylists(2, 3, 1), 2);
  });

  ctx.test("exact_combinatorial_checks", [&] {
    // n=3, goal=4, k=1: 3 choices of repeated song * 3 non-adjacent pairs * 2!
    ctx.expect_eq(sol.numMusicPlaylists(3, 4, 1), 18);
    // n=3, goal=5, k=2: first 3 songs form any permutation (6), rest forced.
    ctx.expect_eq(sol.numMusicPlaylists(3, 5, 2), 6);
  });

  ctx.test("single_song_edge_cases", [&] {
    ctx.expect_eq(sol.numMusicPlaylists(1, 1, 0), 1);
    ctx.expect_eq(sol.numMusicPlaylists(1, 5, 0), 1);
  });

  ctx.test("large_modular_case", [&] {
    // Known LeetCode case: n=16, goal=16, k=4 -> 16! mod (10^9 + 7) = 789741546
    ctx.expect_eq(sol.numMusicPlaylists(16, 16, 4), 789741546);
  });
}

}  // namespace
