// LeetCode 362: Design Hit Counter
// https://leetcode.com/problems/design-hit-counter/
//
// Problem:
//   Design a hit counter that counts the number of hits received in the past
//   5 minutes (300 seconds). Both `hit(timestamp)` and `getHits(timestamp)`
//   accept an integer timestamp in seconds (earliest timestamp >= 1) in
//   monotonically non-decreasing chronological order. Multiple hits may share
//   the same timestamp. Follow-up: scale cleanly when hits per second are
//   very large.
//
// Examples:
//   Example 1:
//     Input:  HitCounter(); calls: hit(1), hit(2), hit(3), getHits(4), hit(300),
//             getHits(300), getHits(301)
//     Output: [3, 4, 3] (for the getHits calls)
//     Explanation:
//       - hit(1), hit(2), and hit(3) populate ring-buffer buckets 1, 2, and 3 with
//         timestamps_[i] = i and hits_[i] = 1.
//       - getHits(4) sums all buckets with timestamps_[i] > 0 and 4 -
//         timestamps_[i] < 300, matching buckets 1, 2, and 3 for a total of 3.
//       - hit(300) sets bucket 300 % 300 = 0 to (timestamp 300, hits 1), so
//         getHits(300) includes buckets 0..3 (300 - 1 = 299 < 300) and returns 4.
//       - getHits(301) drops bucket 1 because 301 - 1 = 300 is not < 300, returning
//         3 from buckets 2, 3, and 0.
//
//   Example 2:
//     Input:  HitCounter(); calls: hit(1), hit(1), hit(1), hit(300), getHits(300),
//             hit(300), getHits(300), getHits(301)
//     Output: [4, 5, 2] (for the getHits calls)
//     Explanation:
//       - Three hit(1) calls all map to bucket 1: the first sets timestamps_[1] = 1
//         and hits_[1] = 1, and the next two find timestamps_[1] == 1 and increment
//         hits_[1] to 3.
//       - hit(300) records (timestamps_[0] = 300, hits_[0] = 1), so getHits(300)
//         sums hits_[1] + hits_[0] = 3 + 1 = 4; a second hit(300) increments
//         hits_[0] to 2, making getHits(300) return 5.
//       - getHits(301) excludes bucket 1 (301 - 1 = 300) and counts only bucket 0
//         (301 - 300 = 1 < 300), returning 2.
//
//   Example 3:
//     Input:  HitCounter(); calls: getHits(1), hit(1), getHits(300), getHits(301),
//             hit(301), hit(301), getHits(600), getHits(601)
//     Output: [0, 1, 0, 2, 0] (for the getHits calls)
//     Explanation:
//       - Before any hits, all timestamps_ entries are 0, so getHits(1) skips all
//         buckets and returns 0.
//       - After hit(1) sets bucket 1 to (1, 1), getHits(300) includes it (299 <
//         300) to return 1, while getHits(301) excludes it (300 is not < 300) and
//         returns 0.
//       - Two hit(301) calls map to bucket 301 % 300 = 1, overwriting the expired
//         timestamp 1 with (301, 1) and incrementing hits_[1] to 2, so getHits(600)
//         returns 2 before expiring at getHits(601) with 0.
//
// Algorithm:
//   Use a fixed-size 300-slot circular buffer (ring buffer) indexed by
//   `timestamp % 300`. Each slot stores the most recent timestamp mapped to
//   that bucket and the aggregated hit count at that exact second:
//     - `hit(t)`: compute `bucket = t % 300`. If `timestamps_[bucket] == t`,
//       increment `hits_[bucket]`; otherwise overwrite `timestamps_[bucket] = t`
//       and reset `hits_[bucket] = 1` (older data in that bucket is guaranteed
//       to be at least 300 seconds stale because calls are chronological).
//     - `getHits(t)`: sum `hits_[i]` across all 300 buckets whose recorded
//       timestamp lies in the half-open window `(t - 300, t]`, i.e.,
//       `timestamps_[i] > 0 && t - timestamps_[i] < 300`.
//   This directly solves the high-QPS follow-up with constant memory and
//   constant work per operation regardless of traffic burst volume.
//   Time Complexity: O(1) for `hit()`, O(300) = O(1) for `getHits()`.
//   Space Complexity: O(300) = O(1).
//
// Data Structures:
//   - Two fixed-size `std::array<int, 300>` ring-buffer arrays (`timestamps_`
//     and `hits_`) providing zero-allocation, cache-friendly O(1) aggregation.
//
// Real-World Applications:
//   - API gateway and load-balancer sliding-window rate limiters (requests per
//     minute / per 5 minutes).
//   - Real-time observability metrics (Prometheus/StatsD rolling counters and
//     error-rate circuit breakers).
//   - DDoS detection and anomaly alerting over bounded time horizons.
//
// Related LeetCode Problems:
//   - #359 Logger Rate Limiter [done] (per-key timestamp throttling window)
//   - #933 Number of Recent Calls [new] (sliding 3000 ms ping counter)
//   - #1797 Design Authentication Manager [new] (TTL expiration tracking)
//   - #346 Moving Average from Data Stream [done] (fixed-window ring buffer)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>

namespace {

// High-throughput O(1)-space hit counter over a 300-second rolling window.
class HitCounter {
 public:
  // Records a hit occurring at `timestamp` (in seconds, monotonically
  // non-decreasing, >= 1).
  // Time: O(1), Space: O(1).
  void hit(int timestamp) noexcept {
    const auto idx = static_cast<std::size_t>(timestamp % kWindowSeconds);
    if (timestamps_[idx] != timestamp) {
      // Previous entry in this bucket belongs to an expired cycle (>= 300s old).
      timestamps_[idx] = timestamp;
      hits_[idx] = 1;
    } else {
      // Aggregate multiple concurrent hits within the same second.
      ++hits_[idx];
    }
  }

  // Returns the total number of hits strictly within `(timestamp - 300,
  // timestamp]`.
  // Time: O(kWindowSeconds) = O(1), Space: O(1).
  [[nodiscard]] int getHits(int timestamp) const noexcept {
    int total = 0;
    for (std::size_t i = 0uz; i < timestamps_.size(); ++i) {
      if (timestamps_[i] > 0 && (timestamp - timestamps_[i]) < kWindowSeconds) {
        total += hits_[i];
      }
    }
    return total;
  }

 private:
  static constexpr int kWindowSeconds = 300;
  static constexpr std::size_t kWindowSize =
      static_cast<std::size_t>(kWindowSeconds);

  std::array<int, kWindowSize> timestamps_{};
  std::array<int, kWindowSize> hits_{};
};

LC_PROBLEM(362, "Design Hit Counter", "design", ctx) {
  ctx.test("leetcode example", [&] {
    HitCounter counter;
    counter.hit(1);
    counter.hit(2);
    counter.hit(3);
    ctx.expect_eq(counter.getHits(4), 3);
    counter.hit(300);
    ctx.expect_eq(counter.getHits(300), 4);
    ctx.expect_eq(counter.getHits(301), 3);
  });

  ctx.test("python main duplicate timestamps", [&] {
    HitCounter counter;
    counter.hit(1);
    counter.hit(1);
    counter.hit(1);
    counter.hit(300);
    ctx.expect_eq(counter.getHits(300), 4);
    counter.hit(300);
    ctx.expect_eq(counter.getHits(300), 5);
    ctx.expect_eq(counter.getHits(301), 2);
  });

  ctx.test("exact window expiration and cycle wrap-around", [&] {
    HitCounter counter;
    ctx.expect_eq(counter.getHits(1), 0);
    counter.hit(1);
    ctx.expect_eq(counter.getHits(300), 1);
    ctx.expect_eq(counter.getHits(301), 0);

    // Overwrite bucket 1 in the next 300-second cycle.
    counter.hit(301);
    counter.hit(301);
    ctx.expect_eq(counter.getHits(600), 2);
    ctx.expect_eq(counter.getHits(601), 0);
  });

  ctx.test("high burst traffic at single second", [&] {
    HitCounter counter;
    for (int i = 0; i < 10000; ++i) {
      counter.hit(50);
    }
    counter.hit(349);
    ctx.expect_eq(counter.getHits(349), 10001);
    ctx.expect_eq(counter.getHits(350), 1);
    ctx.expect_eq(counter.getHits(100000), 0);
  });
}

}  // namespace
