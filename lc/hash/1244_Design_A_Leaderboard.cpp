// LeetCode 1244: Design A Leaderboard
// https://leetcode.com/problems/design-a-leaderboard/
//
// Problem:
//   Design a Leaderboard class supporting three operations on an initially
//   empty leaderboard of players identified by integer IDs:
//     - addScore(playerId, score): Add `score` to `playerId`'s cumulative
//       score, inserting the player with `score` if not currently present.
//     - top(K): Return the sum of scores of the top `K` highest-scoring
//       players currently on the leaderboard.
//     - reset(playerId): Reset `playerId`'s score to 0 by removing the player
//       from the leaderboard (guaranteed to exist before the call).
//   Constraints: 1 <= playerId, K <= 10^4; 1 <= score <= 100; at most 1000
//   operations; K <= current number of active players when top(K) is called.
//
// Examples:
//   Example 1:
//     Input:  ["Leaderboard", "addScore", "addScore", "addScore", "addScore",
//             "addScore", "top", "reset", "reset", "addScore", "top"], [[], [1,
//             73], [2, 56], [3, 39], [4, 51], [5, 4], [1], [1], [2], [2, 51], [3]]
//     Output: [null, null, null, null, null, null, 73, null, null, null, 141]
//     Explanation:
//       - The first five addScore calls populate scores_ = {1: 73, 2: 56, 3: 39, 4:
//         51, 5: 4} and descending score_counts_ = {73: 1, 56: 1, 51: 1, 39: 1, 4:
//         1}.
//       - top(1) takes min(1, 1) = 1 player at the highest score 73 and returns 73.
//       - reset(1) and reset(2) remove players 1 and 2 from scores_ and erase keys
//         73 and 56 from score_counts_; addScore(2, 51) then inserts scores_[2] =
//         51, updating score_counts_ to {51: 2, 39: 1, 4: 1}.
//       - top(3) takes 2 players at score 51 (total = 102, remaining = 1) and 1
//         player at score 39 (total = 141, remaining = 0), returning 141.
//
//   Example 2:
//     Input:  ["Leaderboard", "addScore", "addScore", "addScore", "top", "top",
//             "addScore", "top", "top", "addScore", "top"], [[], [1, 10], [2, 10],
//             [3, 10], [2], [3], [1, 25], [1], [2], [2, 25], [2]]
//     Output: [null, null, null, null, 20, 30, null, 35, 45, null, 70]
//     Explanation:
//       - Inserting players 1, 2, and 3 with score 10 produces score_counts_ = {10:
//         3}, so top(2) returns 2 * 10 = 20 and top(3) returns 3 * 10 = 30.
//       - addScore(1, 25) decrements score_counts_[10] to 2, updates scores_[1] to
//         35, and adds score_counts_[35] = 1, giving score_counts_ = {35: 1, 10:
//         2}.
//       - top(1) returns 35, and top(2) sums 1 * 35 + 1 * 10 = 45.
//       - addScore(2, 25) moves player 2 from 10 to 35 (score_counts_ = {35: 2, 10:
//         1}), so top(2) takes both players at 35 and returns 70.
//
//   Example 3:
//     Input:  ["Leaderboard", "addScore", "top", "reset", "top", "addScore",
//             "top"], [[], [10, 100], [1], [10], [1], [10, 42], [1]]
//     Output: [null, null, 100, null, 0, null, 42]
//     Explanation:
//       - addScore(10, 100) records scores_[10] = 100 and score_counts_[100] = 1,
//         so top(1) returns 100.
//       - reset(10) decrements and erases 100 from score_counts_ and erases player
//         10 from scores_; top(1) sees an empty score_counts_ map and returns 0.
//       - addScore(10, 42) treats player 10 as new, setting scores_[10] = 42 and
//         score_counts_[42] = 1, so top(1) returns 42.
//
// Algorithm:
//   Maintain a two-index structure: a hash map mapping each active playerId to
//   its current cumulative score, paired with a descending ordered frequency
//   map (balanced BST) mapping each active score value to the number of
//   players holding that score.
//     - addScore(playerId, score): Look up the player in O(1) average time. If
//       already present, decrement (and erase if zero) the old score's count
//       in the ordered map, update the player's score, and increment the new
//       score's count. Time: O(log U), where U is the number of distinct
//       scores (U <= N).
//     - top(K): Walk the descending score map from highest to lowest, taking
//       min(remaining, count) players at each score level and accumulating
//       `take * score` until K players have been summed. Time: O(min(K, U)).
//     - reset(playerId): Look up the player's score in the hash map, decrement
//       its count in the ordered map, and erase the player from the hash map.
//       Time: O(log U).
//   Space Complexity: O(N), where N is the number of active players.
//
// Data Structures:
//   - std::unordered_map<int, int>: Maps playerId -> current score for O(1)
//     average lookup and update.
//   - std::map<int, int, std::greater<>>: Balanced BST storing score -> player
//     count in strictly descending score order so top(K) scans only the top
//     entries without sorting all players on every query.
//
// Real-World Applications:
//   - Online multiplayer gaming rankings and real-time tournament leaderboards
//     (similar to Redis Sorted Sets / ZSET).
//   - Live sales, ad-impression, or telemetry top-K dashboards with frequent
//     point updates and periodic account resets.
//   - Priority-based resource schedulers that track dynamic tenant quotas and
//     query the highest-usage consumers.
//
// Related LeetCode Problems:
//   - #703 Kth Largest Element in a Stream [done] (streaming top-K tracking)
//   - #355 Design Twitter [new] (stateful ranking/feed design with hash maps)
//   - #146 LRU Cache [done] (dual data-structure design combining hash map and ordered structure)
//   - #2353 Design a Food Rating System [new] (hash map + ordered set for live top-rated queries)

#include "framework/lc_framework.h"

#include <algorithm>
#include <functional>
#include <map>
#include <unordered_map>

namespace {

// Tracks player scores and supports fast top-K sum queries via a hash map
// paired with a descending ordered score-frequency map.
class Leaderboard {
 public:
  // Adds `score` to `player_id`'s cumulative score, creating the player entry
  // if not already present.
  // Time: O(log U), Space: O(1) amortized (U = distinct active scores).
  void addScore(int player_id, int score) {
    const auto it = scores_.find(player_id);
    int new_score = score;
    if (it != scores_.end()) {
      // Remove one occurrence of the stale score before updating it so
      // `score_counts_` always mirrors the exact multiset of active scores.
      decrement_score_count(it->second);
      it->second += score;
      new_score = it->second;
    } else {
      scores_.emplace(player_id, score);
    }
    ++score_counts_[new_score];
  }

  // Returns the sum of the top `k` scores currently on the leaderboard.
  // Time: O(min(k, U)), Space: O(1).
  [[nodiscard]] int top(int k) const {
    if (k <= 0) {
      return 0;
    }

    int remaining = k;
    int total = 0;

    // `score_counts_` is ordered from highest score to lowest, so greedily
    // taking players in iteration order yields the exact top-K sum.
    for (const auto& [score, count] : score_counts_) {
      const int take = std::min(remaining, count);
      total += take * score;
      remaining -= take;
      if (remaining == 0) {
        break;
      }
    }
    return total;
  }

  // Removes `player_id` from the leaderboard (effectively resetting score to 0).
  // Time: O(log U), Space: O(1).
  void reset(int player_id) {
    const auto it = scores_.find(player_id);
    if (it == scores_.end()) {
      return;
    }
    decrement_score_count(it->second);
    scores_.erase(it);
  }

 private:
  // Decrements the frequency of `score` in `score_counts_` and erases the key
  // when its count reaches zero so `top()` never traverses empty buckets.
  void decrement_score_count(int score) {
    const auto it = score_counts_.find(score);
    if (it == score_counts_.end()) {
      return;
    }
    --it->second;
    if (it->second == 0) {
      score_counts_.erase(it);
    }
  }

  std::unordered_map<int, int> scores_;
  std::map<int, int, std::greater<>> score_counts_;
};

LC_PROBLEM(1244, "Design A Leaderboard", "hash", ctx) {
  ctx.test("example_1", [&] {
    Leaderboard lb;
    lb.addScore(1, 73);
    lb.addScore(2, 56);
    lb.addScore(3, 39);
    lb.addScore(4, 51);
    lb.addScore(5, 4);
    ctx.expect_eq(lb.top(1), 73);
    lb.reset(1);
    lb.reset(2);
    lb.addScore(2, 51);
    ctx.expect_eq(lb.top(3), 141);
  });

  ctx.test("cumulative_updates_and_ties", [&] {
    Leaderboard lb;
    lb.addScore(1, 10);
    lb.addScore(2, 10);
    lb.addScore(3, 10);
    ctx.expect_eq(lb.top(2), 20);
    ctx.expect_eq(lb.top(3), 30);

    // Player 1 climbs ahead while players 2 and 3 remain tied at 10.
    lb.addScore(1, 25);
    ctx.expect_eq(lb.top(1), 35);
    ctx.expect_eq(lb.top(2), 45);

    // Player 2 catches up to player 1 at 35.
    lb.addScore(2, 25);
    ctx.expect_eq(lb.top(2), 70);
  });

  ctx.test("reset_and_reinsert", [&] {
    Leaderboard lb;
    lb.addScore(10, 100);
    ctx.expect_eq(lb.top(1), 100);
    lb.reset(10);
    ctx.expect_eq(lb.top(1), 0);

    // Re-adding after reset starts fresh from 0 rather than keeping old 100.
    lb.addScore(10, 42);
    ctx.expect_eq(lb.top(1), 42);
  });
}

}  // namespace
