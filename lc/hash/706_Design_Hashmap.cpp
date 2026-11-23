// LeetCode 706: Design Hashmap
// https://leetcode.com/problems/design-hashmap/
//
// Problem:
//   Implement a HashMap (`MyHashMap`) without using any built-in hash table
//   libraries, supporting three operations:
//     - put(key, value): Insert a (key, value) mapping, or update the existing
//       value if `key` is already present.
//     - get(key): Return the mapped value for `key`, or -1 if absent.
//     - remove(key): Delete the mapping for `key` if present.
//   Constraints: 0 <= key, value <= 10^6, at most 10^4 calls total.
//
// Examples:
//   Example 1:
//     Input:  operations = ["MyHashMap", "put", "put", "get", "get", "put", "get",
//             "remove", "get"], args = [[], [1, 1], [2, 2], [1], [3], [2, 1], [2],
//             [2], [2]]
//     Output: [null, null, null, 1, -1, null, 1, null, -1]
//     Explanation:
//       - put(1, 1) and put(2, 2) compute key % 1009 to prepend ListNode(1, 1) in
//         buckets_[1] and ListNode(2, 2) in buckets_[2].
//       - get(1) traverses buckets_[1] and returns 1, while get(3) finds
//         buckets_[3] is nullptr and returns -1.
//       - put(2, 1) walks buckets_[2], finds cur->key == 2, and overwrites
//         cur->value from 2 to 1 so get(2) returns 1.
//       - remove(2) unlinks the node at buckets_[2] via *link =
//         std::move((*link)->next), leaving buckets_[2] empty so get(2) returns -1.
//
//   Example 2:
//     Input:  operations = ["MyHashMap", "put", "put", "put", "put", "get",
//             "remove", "get", "get", "remove", "get"], args = [[], [0, 10], [1009,
//             20], [2018, 30], [1009, 25], [1009], [1009], [1009], [2018], [2018],
//             [2018]]
//     Output: [null, null, null, null, null, 25, null, -1, 30, null, -1]
//     Explanation:
//       - Keys 0, 1009, and 2018 all satisfy key % 1009 == 0, so prepending each
//         builds the chain 2018:30 -> 1009:20 -> 0:10 in buckets_[0].
//       - put(1009, 25) traverses buckets_[0], finds middle node key == 1009, and
//         updates its value to 25 so get(1009) returns 25.
//       - remove(1009) relinks 2018->next to 0:10, making get(1009) return -1 while
//         get(2018) still returns 30.
//       - remove(2018) unlinks the head node in buckets_[0] so the chain becomes
//         0:10 and get(2018) returns -1.
//
//   Example 3:
//     Input:  operations = ["MyHashMap", "remove", "get", "put", "put", "get",
//             "get"], args = [[], [42], [42], [0, 0], [1000000, 999999], [0],
//             [1000000]]
//     Output: [null, null, -1, null, null, 0, 999999]
//     Explanation:
//       - remove(42) inspects buckets_[42], sees *link == nullptr, and returns
//         without modifying anything, so get(42) returns -1.
//       - put(0, 0) stores (0, 0) in buckets_[0], and put(1000000, 999999) hashes
//         1000000 % 1009 = 81 to store (1000000, 999999) in buckets_[81].
//       - get(0) returns the stored value 0 (distinct from the -1 missing-key
//         sentinel) and get(1000000) returns 999999.
//
// Algorithm:
//   Use separate chaining over a fixed-size bucket array of prime capacity
//   `kBucketCount = 1009` (a prime near 1000 that spreads patterned keys such
//   as multiples of 10 evenly across buckets).
//     - Hash function: `bucket = key % kBucketCount`.
//     - `put(key, value)`: Traverse the bucket's singly-linked chain; if `key`
//       exists, overwrite its value in place. Otherwise prepend a new node at
//       the bucket head in O(1) time.
//     - `get(key)`: Traverse the bucket chain and return the matching value,
//       or -1 if not found.
//     - `remove(key)`: Walk a pointer-to-`std::unique_ptr<ListNode>` down the
//       chain so removing the head node or any interior node uses the exact
//       same single unlink assignment (`*link = std::move((*link)->next)`).
//   Time Complexity: O(1) average per operation (load factor <= ~10 for 10^4
//   operations); O(N) worst case if all keys collide into one bucket.
//   Space Complexity: O(B + N) where B = 1009 buckets and N = stored keys.
//
// Data Structures:
//   - Fixed-size bucket array (`std::array<std::unique_ptr<ListNode>, 1009>`):
//     maps each hash code to the head of its collision chain.
//   - RAII singly-linked `ListNode` chains (`std::unique_ptr<ListNode>`):
//     stores `(key, value)` pairs with automatic deallocation and zero leaks.
//
// Real-World Applications:
//   - Language runtime symbol tables and associative containers (Java
//     `HashMap`, Python `dict`, C++ `std::unordered_map`).
//   - Database buffer pool page tables and in-memory key-value caches (Redis,
//     Memcached) mapping page/object IDs to memory locations.
//   - Operating system kernel caches (dentry cache, inode hash tables, routing
//     flow tables).
//
// Related LeetCode Problems:
//   - #705 Design HashSet [new] (key-only counterpart using the same chaining design)
//   - #146 LRU Cache [done] (hash map combined with doubly-linked list for eviction)
//   - #380 Insert Delete GetRandom O [done] (1) (hash map + dynamic array for O(1) random access)
//   - #460 LFU Cache [done] (hash maps combined with frequency buckets)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <memory>
#include <utility>

namespace {

// Hash map implemented via separate chaining with RAII singly-linked lists.
class MyHashMap {
 public:
  MyHashMap() = default;

  // Inserts (key, value) or updates the existing entry if `key` is present.
  // Time: O(1) average, O(N) worst-case chain length.
  void put(int key, int value) {
    const std::size_t idx = bucketIndex(key);

    // First scan the bucket chain to update in place if `key` already exists.
    for (ListNode* cur = buckets_[idx].get(); cur != nullptr;
         cur = cur->next.get()) {
      if (cur->key == key) {
        cur->value = value;
        return;
      }
    }

    // Key not found: prepend a new node at the bucket head in O(1).
    buckets_[idx] =
        std::make_unique<ListNode>(key, value, std::move(buckets_[idx]));
  }

  // Returns the value mapped to `key`, or -1 if no mapping exists.
  // Time: O(1) average, O(N) worst-case chain length.
  [[nodiscard]] int get(int key) const {
    const std::size_t idx = bucketIndex(key);
    for (const ListNode* cur = buckets_[idx].get(); cur != nullptr;
         cur = cur->next.get()) {
      if (cur->key == key) {
        return cur->value;
      }
    }
    return -1;
  }

  // Removes the mapping for `key` if present; no-op otherwise.
  // Time: O(1) average, O(N) worst-case chain length.
  void remove(int key) {
    // Indirect pointer traversal unifies head and interior node deletion.
    std::unique_ptr<ListNode>* link = &buckets_[bucketIndex(key)];
    while (*link != nullptr) {
      if ((*link)->key == key) {
        *link = std::move((*link)->next);
        return;
      }
      link = &((*link)->next);
    }
  }

 private:
  // Prime bucket count near 1000 to reduce clustering on structured keys.
  static constexpr std::size_t kBucketCount = 1009uz;

  // Singly-linked chain node storing one (key, value) mapping.
  struct ListNode {
    int key;
    int value;
    std::unique_ptr<ListNode> next;

    ListNode(int k, int v, std::unique_ptr<ListNode> next_node)
        : key(k), value(v), next(std::move(next_node)) {}
  };

  // Maps a non-negative integer key to its bucket slot.
  [[nodiscard]] static std::size_t bucketIndex(int key) noexcept {
    return static_cast<std::size_t>(key) % kBucketCount;
  }

  std::array<std::unique_ptr<ListNode>, kBucketCount> buckets_{};
};

LC_PROBLEM(706, "Design Hashmap", "hash", ctx) {
  ctx.test("leetcode_example", [&] {
    MyHashMap hash_map;
    hash_map.put(1, 1);
    hash_map.put(2, 2);
    ctx.expect_eq(hash_map.get(1), 1);
    ctx.expect_eq(hash_map.get(3), -1);
    hash_map.put(2, 1);
    ctx.expect_eq(hash_map.get(2), 1);
    hash_map.remove(2);
    ctx.expect_eq(hash_map.get(2), -1);
  });

  ctx.test("hash_collisions_and_chain_deletion", [&] {
    MyHashMap hash_map;
    // All three keys collide into bucket 0 (multiples of 1009).
    hash_map.put(0, 10);
    hash_map.put(1009, 20);
    hash_map.put(2018, 30);

    ctx.expect_eq(hash_map.get(0), 10);
    ctx.expect_eq(hash_map.get(1009), 20);
    ctx.expect_eq(hash_map.get(2018), 30);

    // Update middle element in the collision chain.
    hash_map.put(1009, 25);
    ctx.expect_eq(hash_map.get(1009), 25);

    // Remove middle, head, and tail nodes in order.
    hash_map.remove(1009);
    ctx.expect_eq(hash_map.get(1009), -1);
    ctx.expect_eq(hash_map.get(0), 10);
    ctx.expect_eq(hash_map.get(2018), 30);

    hash_map.remove(2018);
    ctx.expect_eq(hash_map.get(2018), -1);
    ctx.expect_eq(hash_map.get(0), 10);

    hash_map.remove(0);
    ctx.expect_eq(hash_map.get(0), -1);
  });

  ctx.test("boundary_keys_and_missing_remove", [&] {
    MyHashMap hash_map;
    // Removing a missing key must be a safe no-op.
    hash_map.remove(42);
    ctx.expect_eq(hash_map.get(42), -1);

    // Boundary key and zero value.
    hash_map.put(0, 0);
    hash_map.put(1000000, 999999);
    ctx.expect_eq(hash_map.get(0), 0);
    ctx.expect_eq(hash_map.get(1000000), 999999);
  });
}

}  // namespace
