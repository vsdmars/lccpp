// LeetCode 432: All Oone Data Structure
// https://leetcode.com/problems/all-oone-data-structure/
//
// Problem:
//   Design a data structure that stores string keys with positive integer
//   counts and supports four operations in O(1) average time:
//     - inc(key): Increment key's count by 1 (inserting it with count 1 if
//       absent).
//     - dec(key): Decrement key's count by 1 (removing it when the count drops
//       to 0, or doing nothing if the key is absent).
//     - getMaxKey(): Return any key currently holding the maximum count, or ""
//       if the structure is empty.
//     - getMinKey(): Return any key currently holding the minimum count, or ""
//       if the structure is empty.
//
// Examples:
//   Example 1:
//     Input:  ops = ["AllOne", "inc", "inc", "getMaxKey", "getMinKey", "inc",
//             "getMaxKey", "getMinKey"], args = [[], ["hello"], ["hello"], [], [],
//             ["leet"], [], []]
//     Output: [null, null, null, "hello", "hello", null, "hello", "leet"]
//     Explanation:
//       - inc("hello") creates count-1 bucket 1:{"hello"} at the front of buckets_,
//         and the second inc("hello") moves "hello" into a newly created count-2
//         bucket and erases the empty count-1 bucket, leaving buckets_ =
//         [2:{"hello"}].
//       - getMaxKey() reads buckets_.back() and getMinKey() reads buckets_.front(),
//         both returning "hello".
//       - inc("leet") sees buckets_.front().count != 1, prepends 1:{"leet"} to give
//         buckets_ = [1:{"leet"}, 2:{"hello"}], and maps "leet" to it.
//       - getMaxKey() returns "hello" from buckets_.back() (count 2) and
//         getMinKey() returns "leet" from buckets_.front() (count 1).
//
//   Example 2:
//     Input:  ops = ["AllOne", "inc", "inc", "inc", "dec", "inc", "inc",
//             "getMaxKey", "getMinKey", "dec", "dec", "dec", "getMaxKey",
//             "getMinKey"], args = [[], ["hello"], ["world"], ["hello"], ["world"],
//             ["hello"], ["leet"], [], [], ["hello"], ["hello"], ["hello"], [], []]
//     Output: [null, null, null, null, null, null, null, "hello", "leet", null,
//             null, null, "leet", "leet"]
//     Explanation:
//       - After inc("hello"), inc("world"), and inc("hello"), buckets_ is
//         [1:{"world"}, 2:{"hello"}].
//       - dec("world") removes "world" at count 1 and deletes the empty count-1
//         bucket; inc("hello") and inc("leet") then leave buckets_ = [1:{"leet"},
//         3:{"hello"}], so getMaxKey() returns "hello" and getMinKey() returns
//         "leet".
//       - Three dec("hello") calls step "hello" down from count 3 to 2 to 1
//         (joining 1:{"leet", "hello"}) and then delete it, pruning empty buckets
//         to leave buckets_ = [1:{"leet"}].
//       - With only 1:{"leet"} remaining in buckets_, both getMaxKey() and
//         getMinKey() return "leet".
//
//   Example 3:
//     Input:  ops = ["AllOne", "getMaxKey", "getMinKey", "dec", "getMaxKey", "inc",
//             "dec", "getMaxKey", "getMinKey"], args = [[], [], [],
//             ["nonexistent"], [], ["a"], ["a"], [], []]
//     Output: [null, "", "", null, "", null, null, "", ""]
//     Explanation:
//       - Initially buckets_ is empty, so getMaxKey() and getMinKey() return "".
//       - dec("nonexistent") misses in key_to_bucket_ and returns without modifying
//         buckets_, so getMaxKey() still returns "".
//       - inc("a") inserts 1:{"a"} into buckets_, and dec("a") sees curr->count ==
//         1, erases "a" from key_to_bucket_, and deletes the now-empty count-1
//         bucket.
//       - Because buckets_ is empty again, both getMaxKey() and getMinKey() return
//         "".
//
// Algorithm:
//   Maintain a doubly linked list of non-empty frequency buckets ordered
//   strictly by ascending count, paired with a hash map from each active key
//   to its owning bucket iterator. Because inc() and dec() change a key's
//   count by exactly +1 or -1, the destination bucket is always either the
//   immediately adjacent list node or a newly inserted neighbor. Removing a
//   bucket as soon as its key set becomes empty preserves the invariant that
//   the list front holds the minimum count and the list back holds the maximum
//   count.
//   Time Complexity:  O(1) average per inc, dec, getMaxKey, and getMinKey.
//   Space Complexity: O(N), where N is the number of distinct active keys.
//
// Data Structures:
//   - std::list<Bucket>: Doubly linked list of strictly increasing frequency
//     buckets; provides O(1) adjacent insertion/erasure and iterator stability.
//   - std::unordered_set<std::string> (inside each Bucket): O(1) average key
//     insertion, removal, and arbitrary representative lookup.
//   - std::unordered_map<std::string, BucketIt>: Maps every active key to its
//     bucket iterator in O(1) average time.
//
// Real-World Applications:
//   - Real-time heavy-hitter / cold-key tracking in telemetry and rate limiters
//     where both hottest and coldest active keys must be queried instantly.
//   - In-memory frequency-based eviction policies (such as O(1) LFU caches)
//     and priority schedulers with unit-step priority adjustments.
//   - Live leaderboard and vote-tallying services with frequent +/-1 updates.
//
// Related LeetCode Problems:
//   - #146 LRU Cache [done] (O(1) doubly linked list + hash map pattern)
//   - #460 LFU Cache [done] (O(1) frequency-bucketed doubly linked list cache)
//   - #380 Insert Delete GetRandom O [done] (1) (O(1) hash-indexed design)
//   - #895 Maximum Frequency Stack [done] (O(1) frequency-bucketed stack)
//   - #2034 Stock Price Fluctuation [done] (dynamic min/max tracking with updates)

#include "framework/lc_framework.h"

#include <iterator>
#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace {

// O(1) frequency tracker maintaining strictly sorted non-empty count buckets.
class AllOne {
 public:
  AllOne() = default;

  // Inserts a new key with count 1 or increments an existing key's count by 1.
  // Time: O(1) average. Space: O(1) amortized.
  void inc(const std::string& key) {
    const auto map_it = key_to_bucket_.find(key);
    if (map_it == key_to_bucket_.end()) {
      // New keys always belong in the count-1 bucket at the front of the list.
      if (buckets_.empty() || buckets_.front().count != 1) {
        buckets_.emplace_front(1);
      }
      buckets_.front().keys.insert(key);
      key_to_bucket_.emplace(key, buckets_.begin());
      return;
    }

    // Existing key moves from its current bucket to the count + 1 neighbor.
    const BucketIt curr = map_it->second;
    const int target_count = curr->count + 1;
    auto next = std::next(curr);
    if (next == buckets_.end() || next->count != target_count) {
      next = buckets_.emplace(next, target_count);
    }

    next->keys.insert(key);
    map_it->second = next;

    // Remove the key from its old bucket and prune the bucket if now empty.
    curr->keys.erase(key);
    if (curr->keys.empty()) {
      buckets_.erase(curr);
    }
  }

  // Decrements an existing key's count by 1, removing it if its count hits 0.
  // No-op if the key is not present.
  // Time: O(1) average. Space: O(1).
  void dec(const std::string& key) {
    const auto map_it = key_to_bucket_.find(key);
    if (map_it == key_to_bucket_.end()) {
      return;
    }

    const BucketIt curr = map_it->second;
    if (curr->count == 1) {
      // Count drops to zero: remove key entirely from the index.
      key_to_bucket_.erase(map_it);
    } else {
      // Move key to the count - 1 bucket immediately preceding `curr`.
      const int target_count = curr->count - 1;
      BucketIt prev = curr;
      if (curr == buckets_.begin() || std::prev(curr)->count != target_count) {
        prev = buckets_.emplace(curr, target_count);
      } else {
        prev = std::prev(curr);
      }
      prev->keys.insert(key);
      map_it->second = prev;
    }

    // Clean up the old bucket so front/back invariants remain exact.
    curr->keys.erase(key);
    if (curr->keys.empty()) {
      buckets_.erase(curr);
    }
  }

  // Returns any key with the maximal count, or "" if empty.
  // Time: O(1). Space: O(1).
  [[nodiscard]] std::string getMaxKey() const {
    if (buckets_.empty()) {
      return {};
    }
    return *buckets_.back().keys.begin();
  }

  // Returns any key with the minimal count, or "" if empty.
  // Time: O(1). Space: O(1).
  [[nodiscard]] std::string getMinKey() const {
    if (buckets_.empty()) {
      return {};
    }
    return *buckets_.front().keys.begin();
  }

 private:
  struct Bucket {
    int count;
    std::unordered_set<std::string> keys;

    explicit Bucket(int initial_count) : count(initial_count) {}
  };

  using BucketIt = std::list<Bucket>::iterator;

  // Strictly ascending by `count`; every bucket in `buckets_` is non-empty.
  std::list<Bucket> buckets_;
  // Maps each active key directly to its bucket node in `buckets_`.
  std::unordered_map<std::string, BucketIt> key_to_bucket_;
};

LC_PROBLEM(432, "All Oone Data Structure", "design", ctx) {
  ctx.test("leetcode_example", [&] {
    AllOne ds;
    ds.inc("hello");
    ds.inc("hello");
    ctx.expect_eq(ds.getMaxKey(), std::string("hello"));
    ctx.expect_eq(ds.getMinKey(), std::string("hello"));
    ds.inc("leet");
    ctx.expect_eq(ds.getMaxKey(), std::string("hello"));
    ctx.expect_eq(ds.getMinKey(), std::string("leet"));
  });

  ctx.test("python_main_trace", [&] {
    AllOne ds;
    ds.inc("hello");
    ds.inc("world");
    ds.inc("hello");
    ds.dec("world");
    ds.inc("hello");
    ds.inc("leet");
    ctx.expect_eq(ds.getMaxKey(), std::string("hello"));
    ctx.expect_eq(ds.getMinKey(), std::string("leet"));
    ds.dec("hello");
    ds.dec("hello");
    ds.dec("hello");
    ctx.expect_eq(ds.getMaxKey(), std::string("leet"));
    ctx.expect_eq(ds.getMinKey(), std::string("leet"));
  });

  ctx.test("empty_and_missing_dec", [&] {
    AllOne ds;
    ctx.expect_eq(ds.getMaxKey(), std::string());
    ctx.expect_eq(ds.getMinKey(), std::string());
    ds.dec("nonexistent");
    ctx.expect_eq(ds.getMaxKey(), std::string());
    ds.inc("a");
    ds.dec("a");
    ctx.expect_eq(ds.getMaxKey(), std::string());
    ctx.expect_eq(ds.getMinKey(), std::string());
  });

  ctx.test("tied_counts_and_transitions", [&] {
    AllOne ds;
    ds.inc("a");
    ds.inc("b");
    ds.inc("b");
    ds.inc("c");
    ds.inc("c");
    ds.inc("c");
    ctx.expect_eq(ds.getMinKey(), std::string("a"));
    ctx.expect_eq(ds.getMaxKey(), std::string("c"));

    // Promote "a" past "b" and "c" while demoting "c".
    ds.inc("a");
    ds.inc("a");
    ds.inc("a");
    ds.dec("c");
    ds.dec("c");
    ctx.expect_eq(ds.getMinKey(), std::string("c"));
    ctx.expect_eq(ds.getMaxKey(), std::string("a"));

    // Demote "a" down to 2 (tied with "b") and remove "c".
    ds.dec("c");
    ds.dec("a");
    ds.dec("a");
    const std::string max_k = ds.getMaxKey();
    const std::string min_k = ds.getMinKey();
    ctx.expect_true(max_k == "a" || max_k == "b");
    ctx.expect_true(min_k == "a" || min_k == "b");
  });
}

}  // namespace
