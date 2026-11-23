// LeetCode 460: LFU Cache
// https://leetcode.com/problems/lfu-cache/
//
// Problem:
//   Design and implement a Least Frequently Used (LFU) cache with a fixed
//   integer capacity supporting O(1) average time operations:
//     - LFUCache(capacity): Initialize the cache with the given capacity.
//     - get(key): Return the value associated with key if present (and
//       increment its use counter by 1); otherwise return -1.
//     - put(key, value): Update an existing key's value and increment its use
//       counter, or insert a new key with use counter 1. When inserting at full
//       capacity, evict the least frequently used key first; if multiple keys
//       share the minimum frequency, evict the least recently used (LRU) among
//       them.
//
// Algorithm:
//   Partition cached entries by exact access frequency into per-frequency
//   doubly linked lists, ordered from most recently used (front) to least
//   recently used (back). Maintain `min_freq_` tracking the smallest non-empty
//   frequency bucket and a hash map from key directly to its list iterator.
//   On any get() or update put(), `std::list::splice` moves the entry node in
//   O(1) without heap allocation from bucket `f` to the front of bucket `f + 1`
//   while preserving iterator validity; if bucket `f` becomes empty and equaled
//   `min_freq_`, increment `min_freq_`. On eviction, pop the back of bucket
//   `min_freq_` in O(1). New insertions always land at bucket 1 and reset
//   `min_freq_` to 1.
//   Time Complexity:  O(1) average per get() and put().
//   Space Complexity: O(capacity) to store cached entries and bucket indices.
//
// Data Structures:
//   - std::unordered_map<int, std::list<Entry>>: Maps each active frequency to
//     a doubly linked list ordered MRU-to-LRU for O(1) tie-breaking eviction.
//   - std::unordered_map<int, EntryList::iterator>: Maps each cached key
//     directly to its node iterator for O(1) lookup and O(1) splice promotion.
//
// Real-World Applications:
//   - Content Delivery Networks (CDNs) and web reverse proxies (e.g., NGINX,
//     Redis LFU eviction mode) retaining hot objects over scan-heavy traffic.
//   - Database buffer pool managers and OS page caches resisting sequential
//     full-table scan pollution where pure LRU degrades.
//   - DNS resolvers and microservices API response caches prioritizing high
//     frequency endpoints.
//
// Related LeetCode Problems:
//   - #146 LRU Cache [done] (pure recency-based O(1) eviction cache)
//   - #432 All Oone Data Structure [done] (O(1) frequency-bucketed tracking)
//   - #588 Design In Memory File System [done] (in-memory stateful design)
//   - #895 Maximum Frequency Stack [done] (frequency-bucketed O(1) pop/push)
//   - #1756 Design Most Recently Used Queue [new] (recency-ordered structure)

#include "framework/lc_framework.h"

#include <list>
#include <unordered_map>
#include <utility>

namespace {

// O(1) Least Frequently Used (LFU) cache with LRU tie-breaking per frequency.
class LFUCache {
 public:
  explicit LFUCache(int capacity) : capacity_(capacity) {}

  // Returns cached value for `key` and increments its frequency, or -1 if
  // absent.
  // Time: O(1) average. Space: O(1).
  int get(int key) {
    const auto map_it = key_to_iter_.find(key);
    if (map_it == key_to_iter_.end()) {
      return -1;
    }
    const EntryIt entry_it = map_it->second;
    promote(entry_it);
    return entry_it->value;
  }

  // Updates an existing key or inserts a new key (evicting the LFU/LRU victim
  // if the cache is at full capacity).
  // Time: O(1) average. Space: O(1) amortized.
  void put(int key, int value) {
    if (capacity_ <= 0) {
      return;
    }

    // Existing key: update value and promote its frequency bucket.
    const auto map_it = key_to_iter_.find(key);
    if (map_it != key_to_iter_.end()) {
      const EntryIt entry_it = map_it->second;
      entry_it->value = value;
      promote(entry_it);
      return;
    }

    // At capacity: evict the least recently used entry in `min_freq_`.
    if (std::cmp_equal(key_to_iter_.size(), capacity_)) {
      auto& min_list = freq_to_list_[min_freq_];
      const int victim_key = min_list.back().key;
      min_list.pop_back();
      if (min_list.empty()) {
        freq_to_list_.erase(min_freq_);
      }
      key_to_iter_.erase(victim_key);
    }

    // Insert the new key at MRU position in frequency bucket 1.
    auto& list_one = freq_to_list_[1];
    list_one.emplace_front(key, value, 1);
    key_to_iter_.emplace(key, list_one.begin());
    min_freq_ = 1;
  }

 private:
  struct Entry {
    int key;
    int value;
    int freq;

    Entry(int entry_key, int entry_value, int entry_freq)
        : key(entry_key), value(entry_value), freq(entry_freq) {}
  };

  using EntryList = std::list<Entry>;
  using EntryIt = EntryList::iterator;

  // Moves `entry_it` from bucket `f` to the front of bucket `f + 1` via
  // std::list::splice, avoiding allocation and preserving iterator validity.
  void promote(EntryIt entry_it) {
    const int old_freq = entry_it->freq;
    const int new_freq = old_freq + 1;
    entry_it->freq = new_freq;

    auto& old_list = freq_to_list_[old_freq];
    auto& new_list = freq_to_list_[new_freq];
    new_list.splice(new_list.begin(), old_list, entry_it);

    // Erase emptied bucket and advance `min_freq_` if it was the lowest bucket.
    if (old_list.empty()) {
      freq_to_list_.erase(old_freq);
      if (min_freq_ == old_freq) {
        min_freq_ = new_freq;
      }
    }
  }

  int capacity_ = 0;
  int min_freq_ = 0;
  // Frequency -> doubly linked list of entries ordered MRU (front) to LRU
  // (back).
  std::unordered_map<int, EntryList> freq_to_list_;
  // Key -> stable list iterator pointing to the key's Entry node.
  std::unordered_map<int, EntryIt> key_to_iter_;
};

LC_PROBLEM(460, "LFU Cache", "design", ctx) {
  ctx.test("leetcode_example_1", [&] {
    LFUCache lfu(2);
    lfu.put(1, 1);
    lfu.put(2, 2);
    ctx.expect_eq(lfu.get(1), 1);
    lfu.put(3, 3);  // Evicts key 2 (freq 1 < freq 2).
    ctx.expect_eq(lfu.get(2), -1);
    ctx.expect_eq(lfu.get(3), 3);  // Now freq(1) == freq(3) == 2, 1 is LRU.
    lfu.put(4, 4);                 // Evicts key 1 on frequency tie.
    ctx.expect_eq(lfu.get(1), -1);
    ctx.expect_eq(lfu.get(3), 3);
    ctx.expect_eq(lfu.get(4), 4);
  });

  ctx.test("zero_capacity", [&] {
    LFUCache lfu(0);
    lfu.put(1, 10);
    ctx.expect_eq(lfu.get(1), -1);
  });

  ctx.test("capacity_one_update_and_evict", [&] {
    LFUCache lfu(1);
    lfu.put(1, 10);
    ctx.expect_eq(lfu.get(1), 10);
    lfu.put(1, 20);  // Overwrite in place without eviction.
    ctx.expect_eq(lfu.get(1), 20);
    lfu.put(2, 30);  // Evicts key 1 regardless of its higher frequency.
    ctx.expect_eq(lfu.get(1), -1);
    ctx.expect_eq(lfu.get(2), 30);
  });

  ctx.test("put_update_increments_frequency", [&] {
    LFUCache lfu(2);
    lfu.put(1, 10);
    lfu.put(2, 20);
    lfu.put(1, 100);  // Updating key 1 bumps freq(1) to 2.
    lfu.put(3, 30);   // Must evict key 2 (freq 1), not key 1.
    ctx.expect_eq(lfu.get(2), -1);
    ctx.expect_eq(lfu.get(1), 100);
    ctx.expect_eq(lfu.get(3), 30);
  });
}

}  // namespace
