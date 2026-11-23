// LeetCode 146: LRU Cache
// https://leetcode.com/problems/lru-cache/
//
// Problem:
//   Design a data structure implementing a Least Recently Used (LRU) cache
//   with a fixed positive capacity supporting two O(1) operations:
//     - `get(key)`: Return the value associated with `key` if present (and
//       mark it as most recently used), otherwise return -1.
//     - `put(key, value)`: Update the value of `key` if present (marking it as
//       most recently used), or insert the key-value pair. If inserting causes
//       the cache size to exceed `capacity`, evict the least recently used key.
//
// Examples:
//   Example 1:
//     Input:  ["LRUCache", "put", "put", "get", "put", "get", "put", "get", "get",
//             "get"], [[2], [1, 1], [2, 2], [1], [3, 3], [2], [4, 4], [1], [3],
//             [4]]
//     Output: [null, null, null, 1, null, -1, null, -1, 3, 4]
//     Explanation:
//       - Initialize `LRUCache(2)`, then `put(1, 1)` and `put(2, 2)` push to the
//         front of `order_`, leaving `order_ = [(2, 2), (1, 1)]` from MRU to LRU.
//       - `get(1)` finds key `1` in `index_`, splices `(1, 1)` to the front
//         (`order_ = [(1, 1), (2, 2)]`), and returns `1`.
//       - `put(3, 3)` sees `index_.size() == 2`, evicts `order_.back()` (key `2`),
//         and inserts `(3, 3)` at the front (`order_ = [(3, 3), (1, 1)]`), so
//         `get(2)` misses and returns `-1`.
//       - `put(4, 4)` evicts `order_.back()` (key `1`) and inserts `(4, 4)` at the
//         front (`order_ = [(4, 4), (3, 3)]`), so `get(1)` misses and returns `-1`.
//       - `get(3)` splices `(3, 3)` to the front and returns `3`, and `get(4)`
//         splices `(4, 4)` to the front and returns `4`.
//
//   Example 2:
//     Input:  ["LRUCache", "put", "put", "put", "put", "get", "get", "get"], [[2],
//             [1, 10], [2, 20], [1, 100], [3, 30], [1], [2], [3]]
//     Output: [null, null, null, null, null, 100, -1, 30]
//     Explanation:
//       - Initialize `LRUCache(2)` and execute `put(1, 10)` and `put(2, 20)`,
//         producing `order_ = [(2, 20), (1, 10)]` with key `1` at the LRU back.
//       - `put(1, 100)` finds key `1` in `index_`, updates its value to `100`, and
//         splices it to the front so `order_ = [(1, 100), (2, 20)]`.
//       - `put(3, 30)` sees `index_.size() == 2`, evicts `order_.back()` (key `2`),
//         and inserts `(3, 30)` at the front (`order_ = [(3, 30), (1, 100)]`).
//       - `get(1)` returns `100`, `get(2)` misses in `index_` and returns `-1`, and
//         `get(3)` returns `30`.
//
//   Example 3:
//     Input:  ["LRUCache", "put", "get", "put", "get", "get"], [[1], [1, 5], [1],
//             [2, 6], [1], [2]]
//     Output: [null, null, 5, null, -1, 6]
//     Explanation:
//       - Initialize `LRUCache(1)`, then `put(1, 5)` inserts `(1, 5)` at the front
//         of `order_` and records `index_[1]`; `get(1)` hits and returns `5`.
//       - `put(2, 6)` sees `index_.size() == capacity_` (`1`), erases
//         `order_.back()` (key `1`) from `index_` and pops it from `order_`, then
//         inserts `(2, 6)`.
//       - `get(1)` misses in `index_` and returns `-1`, while `get(2)` finds key
//         `2` in `index_` and returns `6`.
//
// Algorithm:
//   Hash Map + Doubly-Linked List (`std::unordered_map` + `std::list`):
//   - Maintain recency order in a doubly-linked list `order_` storing
//     `(key, value)` pairs, with the most recently used (MRU) item at
//     `order_.front()` and the least recently used (LRU) item at
//     `order_.back()`.
//   - Maintain a hash map `index_` mapping each key directly to its
//     `std::list` iterator so any node can be located in O(1) average time.
//   - On `get(key)`: look up the iterator in `index_`; if found, move that
//     node to the front via `std::list::splice` (O(1), zero allocations) and
//     return its value.
//   - On `put(key, value)`:
//     * If `key` exists, overwrite its value and `splice` its node to the
//       front.
//     * Otherwise, if at capacity, read the LRU key from `order_.back()`,
//       erase it from `index_`, and `pop_back()`. Then `emplace_front` the new
//       pair and record `order_.begin()` in `index_`.
//   - Time Complexity: O(1) average for both `get` and `put`.
//   - Space Complexity: O(capacity) for storing up to `capacity` entries across
//     the list and hash map.
//
// Data Structures:
//   - Doubly-linked list (`std::list<std::pair<int, int>>`) preserving exact
//     access recency with O(1) splice/insert/evict and stable iterators.
//   - Hash map (`std::unordered_map<int, ListIter>`) providing O(1) average
//     key-to-node lookup.
//
// Real-World Applications:
//   - OS virtual memory page replacement and filesystem buffer caches.
//   - Database buffer pools (InnoDB/PostgreSQL page cache eviction).
//   - In-memory key-value stores and CDNs (Redis `allkeys-lru`, Memcached,
//     browser HTTP/DNS caches).
//
// Related LeetCode Problems:
//   - #460 LFU Cache [done] (evicts by least frequency with LRU tie-breaking)
//   - #432 All O`one Data Structure [done] (hash map + doubly-linked list of count buckets)
//   - #380 Insert Delete GetRandom O [done] (1) (hash map + auxiliary container for O(1) operations)
//   - #1756 Design Most Recently Used Queue [new] (recency-ordered container design)

#include "framework/lc_framework.h"

#include <cstddef>
#include <list>
#include <unordered_map>
#include <utility>

namespace {

// Fixed-capacity Least Recently Used (LRU) cache with O(1) average get/put.
class LRUCache {
 public:
  // Initializes the cache with the given maximum entry capacity.
  explicit LRUCache(int capacity)
      : capacity_(capacity > 0 ? static_cast<std::size_t>(capacity) : 0uz) {
    index_.reserve(capacity_);
  }

  // Returns the cached value for `key` and promotes it to most recently used,
  // or returns -1 if `key` is not in the cache.
  // Time: O(1) average.
  int get(int key) {
    const auto it = index_.find(key);
    if (it == index_.end()) {
      return -1;
    }

    // Relink the accessed node to the MRU position (front) without copying.
    order_.splice(order_.begin(), order_, it->second);
    return it->second->second;
  }

  // Inserts or updates `(key, value)` as most recently used, evicting the
  // least recently used entry if the cache is already at capacity.
  // Time: O(1) average.
  void put(int key, int value) {
    if (capacity_ == 0uz) {
      return;
    }

    if (const auto it = index_.find(key); it != index_.end()) {
      it->second->second = value;
      order_.splice(order_.begin(), order_, it->second);
      return;
    }

    // Evict the LRU entry at the back before inserting a new key at capacity.
    if (index_.size() == capacity_) {
      const int lru_key = order_.back().first;
      index_.erase(lru_key);
      order_.pop_back();
    }

    order_.emplace_front(key, value);
    index_[key] = order_.begin();
  }

 private:
  using EntryList = std::list<std::pair<int, int>>;

  std::size_t capacity_ = 0uz;
  // Front = most recently used, back = least recently used.
  EntryList order_;
  std::unordered_map<int, EntryList::iterator> index_;
};

LC_PROBLEM(146, "LRU Cache", "design", ctx) {
  ctx.test("example_1_standard_sequence", [&] {
    LRUCache cache(2);
    cache.put(1, 1);
    cache.put(2, 2);
    ctx.expect_eq(cache.get(1), 1);   // 1 becomes MRU, 2 is LRU
    cache.put(3, 3);                  // evicts key 2
    ctx.expect_eq(cache.get(2), -1);  // not found
    cache.put(4, 4);                  // evicts key 1
    ctx.expect_eq(cache.get(1), -1);  // not found
    ctx.expect_eq(cache.get(3), 3);
    ctx.expect_eq(cache.get(4), 4);
  });

  ctx.test("update_existing_key_refreshes_recency", [&] {
    LRUCache cache(2);
    cache.put(1, 10);
    cache.put(2, 20);
    // Updating key 1 promotes it to MRU so key 2 becomes LRU.
    cache.put(1, 100);
    cache.put(3, 30);  // must evict key 2, not key 1
    ctx.expect_eq(cache.get(1), 100);
    ctx.expect_eq(cache.get(2), -1);
    ctx.expect_eq(cache.get(3), 30);
  });

  ctx.test("capacity_one", [&] {
    LRUCache cache(1);
    cache.put(1, 5);
    ctx.expect_eq(cache.get(1), 5);
    cache.put(2, 6);
    ctx.expect_eq(cache.get(1), -1);
    ctx.expect_eq(cache.get(2), 6);
  });

  ctx.test("zero_capacity_noop", [&] {
    LRUCache cache(0);
    cache.put(1, 1);
    ctx.expect_eq(cache.get(1), -1);
  });
}

}  // namespace
