// LeetCode 2034: Stock Price Fluctuation
// https://leetcode.com/problems/stock-price-fluctuation/
//
// Problem:
//   Design a data structure `StockPrice` to ingest an out-of-order stream of
//   `(timestamp, price)` stock ticks where later records with the same
//   timestamp overwrite (correct) earlier prices. Support:
//     - `update(timestamp, price)`: record or correct the price at `timestamp`.
//     - `current()`: return the price at the latest recorded timestamp.
//     - `maximum()`: return the maximum valid price across all timestamps.
//     - `minimum()`: return the minimum valid price across all timestamps.
//
// Examples:
//   Example 1:
//     Input:  ["StockPrice", "update", "update", "current", "maximum", "update",
//             "maximum", "update", "minimum"], [[], [1, 10], [2, 5], [], [], [1,
//             3], [], [4, 2], []]
//     Output: [null, null, null, 5, 10, null, 5, null, 2]
//     Explanation:
//       - update(1, 10) and update(2, 5) store prices_[1] = 10 and prices_[2] = 5,
//         advance latest_timestamp_ to 2, and push {10, 1}, {5, 2} onto both heaps.
//       - current() returns prices_[2] = 5 in O(1), and maximum() sees top {10, 1}
//         matches prices_[1] = 10 and returns 10.
//       - update(1, 3) overwrites prices_[1] = 3 and pushes {3, 1} onto both heaps
//         without removing the old {10, 1} entry.
//       - maximum() pops stale top {10, 1} because prices_[1] == 3 != 10, then
//         finds valid top {5, 2} matching prices_[2] = 5 and returns 5.
//       - update(4, 2) sets prices_[4] = 2, updates latest_timestamp_ to 4, and
//         pushes {2, 4}; minimum() finds {2, 4} at the min-heap top matching
//         prices_[4] = 2 and returns 2.
//
//   Example 2:
//     Input:  ["StockPrice", "update", "update", "update", "current", "maximum",
//             "minimum"], [[], [10, 50], [5, 100], [2, 20], [], [], []]
//     Output: [null, null, null, null, 50, 100, 20]
//     Explanation:
//       - update(10, 50) sets prices_[10] = 50 and latest_timestamp_ = 10, pushing
//         {50, 10} to both heaps.
//       - update(5, 100) and update(2, 20) record prices_[5] = 100 and prices_[2] =
//         20 while keeping latest_timestamp_ = max(10, 5, 2) = 10.
//       - current() reads prices_[10] = 50, ignoring the lower out-of-order
//         timestamps 5 and 2.
//       - maximum() returns 100 from top {100, 5} and minimum() returns 20 from top
//         {20, 2}, both validated against prices_.
//
//   Example 3:
//     Input:  ["StockPrice", "update", "update", "update", "maximum", "minimum",
//             "update", "update", "update", "update", "current", "maximum",
//             "minimum"], [[], [1, 100], [2, 1], [3, 50], [], [], [1, 90], [1, 40],
//             [2, 10], [2, 60], [], [], []]
//     Output: [null, null, null, null, 100, 1, null, null, null, null, 50, 60, 40]
//     Explanation:
//       - The first three updates record prices_ = {1: 100, 2: 1, 3: 50} with
//         latest_timestamp_ = 3, so maximum() returns 100 and minimum() returns 1.
//       - Four corrections update prices_[1] to 40 and prices_[2] to 60 while
//         leaving latest_timestamp_ = 3, pushing new entries onto both heaps.
//       - current() returns prices_[3] = 50 since timestamp 3 remains the highest
//         timestamp.
//       - maximum() lazily discards stale {100, 1} and {90, 1} until valid top {60,
//         2} matches prices_[2] = 60, and minimum() discards {1, 2} and {10, 2}
//         until valid top {40, 1} matches prices_[1] = 40.
//
// Algorithm:
//   Hash Map + Dual Heaps with Lazy Deletion:
//     - Keep an authoritative hash map `prices_[timestamp] = price` and track
//       `latest_timestamp_` so `current()` is an O(1) lookup.
//     - On each `update(timestamp, price)`, push `{price, timestamp}` into both
//       a max-heap and a min-heap without trying to locate or remove any older
//       price for that timestamp.
//     - On `maximum()` / `minimum()`, inspect the heap top `{price, timestamp}`
//       and discard it while `prices_.at(timestamp) != price` (stale entry from
//       an overwritten tick). The first entry matching `prices_` is guaranteed
//       to be the true current extremum.
//   Time Complexity:
//     - `update`:  O(log n)
//     - `current`: O(1)
//     - `maximum` / `minimum`: amortized O(log n) (each pushed entry is popped
//       at most once across all queries).
//   Space Complexity: O(n), where n is the total number of `update` calls.
//
// Data Structures:
//   - Hash map (`std::unordered_map<int, int>`): source of truth mapping each
//     timestamp to its latest corrected price.
//   - Max-heap & Min-heap (`std::priority_queue<std::pair<int, int>>`): track
//     candidate extrema with lazy invalidation against the hash map.
//
// Real-World Applications:
//   - Real-time market data feeds and order-book tick engines handling
//     out-of-order packets and exchange price corrections/revisions.
//   - Streaming telemetry / SLA dashboards tracking live min/max metrics over
//     late-arriving or retroactively corrected sensor readings.
//   - Priority queues with key updates (e.g., Dijkstra's algorithm with lazy
//     duplicate-entry deletion).
//
// Related LeetCode Problems:
//   - #155 Min Stack [done] (tracking running extrema alongside updates)
//   - #295 Find Median from Data Stream [done] (dual-heap streaming statistics)
//   - #480 Sliding Window Median [done] (dual heaps with lazy deletion)
//   - #2349 Design a Number Container System [new] (hash map + lazy heap/set updates)
//   - #981 Time Based Key-Value Store [done] (timestamp-indexed value retrieval)

#include "framework/lc_framework.h"

#include <algorithm>
#include <functional>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

// Tracks live, corrected stock ticks with O(1) latest price and amortized
// O(log n) minimum/maximum queries via hash-map-validated lazy heaps.
class StockPrice {
 public:
  // Records or corrects the stock price at `timestamp`.
  // Time: O(log n).
  void update(int timestamp, int price) {
    prices_[timestamp] = price;
    latest_timestamp_ = std::max(latest_timestamp_, timestamp);

    // Push into both heaps; any superseded entries for `timestamp` will be
    // discarded lazily when they surface to the top.
    max_heap_.emplace(price, timestamp);
    min_heap_.emplace(price, timestamp);
  }

  // Returns the price at the highest timestamp seen so far.
  // Time: O(1).
  [[nodiscard]] int current() const { return prices_.at(latest_timestamp_); }

  // Returns the highest currently valid price, discarding stale heap entries.
  // Time: amortized O(log n).
  [[nodiscard]] int maximum() {
    while (!max_heap_.empty()) {
      const auto [price, timestamp] = max_heap_.top();
      if (prices_.at(timestamp) == price) {
        return price;
      }
      max_heap_.pop();
    }
    return 0;
  }

  // Returns the lowest currently valid price, discarding stale heap entries.
  // Time: amortized O(log n).
  [[nodiscard]] int minimum() {
    while (!min_heap_.empty()) {
      const auto [price, timestamp] = min_heap_.top();
      if (prices_.at(timestamp) == price) {
        return price;
      }
      min_heap_.pop();
    }
    return 0;
  }

 private:
  using PriceTick = std::pair<int, int>;  // {price, timestamp}

  std::unordered_map<int, int> prices_;
  int latest_timestamp_ = 0;
  std::priority_queue<PriceTick> max_heap_;
  std::priority_queue<PriceTick, std::vector<PriceTick>, std::greater<>>
      min_heap_;
};

LC_PROBLEM(2034, "Stock Price Fluctuation", "stack", ctx) {
  ctx.test("example_1", [&] {
    StockPrice sp;
    sp.update(1, 10);
    sp.update(2, 5);
    ctx.expect_eq(sp.current(), 5);
    ctx.expect_eq(sp.maximum(), 10);
    sp.update(1, 3);  // Correct timestamp 1 from 10 down to 3.
    ctx.expect_eq(sp.maximum(), 5);
    sp.update(4, 2);
    ctx.expect_eq(sp.minimum(), 2);
  });

  ctx.test("out_of_order_timestamps", [&] {
    StockPrice sp;
    sp.update(10, 50);
    sp.update(5, 100);
    sp.update(2, 20);
    // Latest timestamp remains 10 even though 5 and 2 arrived later.
    ctx.expect_eq(sp.current(), 50);
    ctx.expect_eq(sp.maximum(), 100);
    ctx.expect_eq(sp.minimum(), 20);
  });

  ctx.test("repeated_corrections_flush_stale_entries", [&] {
    StockPrice sp;
    sp.update(1, 100);
    sp.update(2, 1);
    sp.update(3, 50);
    ctx.expect_eq(sp.maximum(), 100);
    ctx.expect_eq(sp.minimum(), 1);

    // Overwrite both the maximum and minimum multiple times.
    sp.update(1, 90);
    sp.update(1, 40);
    sp.update(2, 10);
    sp.update(2, 60);
    ctx.expect_eq(sp.current(), 50);
    ctx.expect_eq(sp.maximum(), 60);
    ctx.expect_eq(sp.minimum(), 40);
  });

  ctx.test("single_timestamp_and_duplicate_prices", [&] {
    StockPrice sp;
    sp.update(7, 25);
    sp.update(7, 30);
    ctx.expect_eq(sp.current(), 30);
    ctx.expect_eq(sp.maximum(), 30);
    ctx.expect_eq(sp.minimum(), 30);

    // Two timestamps share the same price; correcting one leaves the other.
    sp.update(8, 30);
    sp.update(7, 15);
    ctx.expect_eq(sp.current(), 30);
    ctx.expect_eq(sp.maximum(), 30);
    ctx.expect_eq(sp.minimum(), 15);
  });
}

}  // namespace
