// LeetCode 380: Insert Delete Getrandom O1
// https://leetcode.com/problems/insert-delete-getrandom-o1/
//
// Problem:
//   Implement the `RandomizedSet` data structure supporting three operations
//   in average O(1) time complexity:
//   - `insert(val)`: Inserts `val` if not present; returns true if inserted,
//     false if already present.
//   - `remove(val)`: Removes `val` if present; returns true if removed, false
//     if absent.
//   - `getRandom()`: Returns a uniformly random element from the current
//     non-empty set (every present element has equal probability 1/n).
//
// Algorithm:
//   A hash table alone cannot support O(1) uniform random selection because
//   bucket arrays are sparse, while a dynamic array alone cannot support O(1)
//   membership lookup or mid-array deletion. Combining both solves all three:
//   - `insert(val)`: Check `val_to_index_`; if absent, append `val` to the end
//     of `values_` and record its index in `val_to_index_`.
//   - `remove(val)`: Look up `idx = val_to_index_[val]`. Swap-overwrite
//     `values_[idx]` with the back element `last_val = values_.back()`, update
//     `val_to_index_[last_val] = idx`, pop the back of `values_`, and erase
//     `val` from `val_to_index_`. This keeps `values_` densely packed in O(1).
//   - `getRandom()`: Draw a uniform index in `[0, values_.size() - 1]` using a
//     Mersenne Twister PRNG and return `values_[index]`.
//   Time Complexity: O(1) average for insert, remove, and getRandom.
//   Space Complexity: O(n) for n stored elements.
//
// Data Structures:
//   - std::vector<int>: contiguous dense storage of active values enabling
//     O(1) uniform random indexing and O(1) back pop.
//   - std::unordered_map<int, std::size_t>: maps each active value to its
//     exact index in the vector for O(1) membership checks and swap-delete.
//   - std::mt19937: pseudo-random number generator paired with
//     std::uniform_int_distribution for unbiased uniform sampling.
//
// Real-World Applications:
//   - Randomized load balancing over a dynamic pool of healthy backend servers
//     (adding/draining nodes while picking a random healthy peer in O(1)).
//   - Game-engine entity pools and loot/spawn tables with constant-time random
//     selection and removal.
//   - Reservoir sampling and Monte Carlo simulations over evolving active sets.
//
// Related LeetCode Problems:
//   - #381 Insert Delete GetRandom O [done] (1) - Duplicates allowed (extends #380 by mapping values to sets of indices)
//   - #710 Random Pick with Blacklist [new] (swap-remapping blacklisted indices into a dense prefix)
//   - #146 LRU Cache [done] (combining a hash map with a linear structure for O(1) operations)
//   - #359 Logger Rate Limiter [done] (hash-map-backed O(1) online design problem)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <random>
#include <unordered_map>
#include <vector>

namespace {

// Dynamic set supporting O(1) average insert, remove, and uniform getRandom.
//
// Invariant: `values_` contains each present element exactly once at indices
// `[0, values_.size())`, and for every `i`, `val_to_index_.at(values_[i]) == i`.
class RandomizedSet {
 public:
  // Default constructor seeds the PRNG from std::random_device.
  RandomizedSet() : rng_(std::random_device{}()) {}

  // Deterministic constructor for reproducible testing.
  explicit RandomizedSet(std::uint32_t seed) : rng_(seed) {}

  // Inserts `val` if not already present. Returns true if inserted.
  // Time Complexity: O(1) average.
  bool insert(int val) {
    if (val_to_index_.contains(val)) {
      return false;
    }

    val_to_index_.emplace(val, values_.size());
    values_.push_back(val);
    return true;
  }

  // Removes `val` if present via swap-with-last to preserve dense packing.
  // Returns true if removed.
  // Time Complexity: O(1) average.
  bool remove(int val) {
    const auto it = val_to_index_.find(val);
    if (it == val_to_index_.end()) {
      return false;
    }

    const std::size_t remove_idx = it->second;
    const std::size_t last_idx = values_.size() - 1;

    // Move the last element into the vacated slot unless we are already
    // removing the last element; using `.at()` avoids any rehash that could
    // invalidate iterator `it`.
    if (remove_idx != last_idx) {
      const int last_val = values_[last_idx];
      values_[remove_idx] = last_val;
      val_to_index_.at(last_val) = remove_idx;
    }

    values_.pop_back();
    val_to_index_.erase(it);
    return true;
  }

  // Returns a uniformly random element from the non-empty set.
  // Time Complexity: O(1).
  int getRandom() {
    std::uniform_int_distribution<std::size_t> dist(0, values_.size() - 1);
    return values_[dist(rng_)];
  }

 private:
  std::vector<int> values_;
  std::unordered_map<int, std::size_t> val_to_index_;
  std::mt19937 rng_;
};

LC_PROBLEM(380, "Insert Delete Getrandom O1", "hash", ctx) {
  ctx.test("leetcode_example_sequence", [&] {
    RandomizedSet rset(42U);
    ctx.expect_true(rset.insert(1));
    ctx.expect_false(rset.remove(2));
    ctx.expect_true(rset.insert(2));

    const int sample = rset.getRandom();
    ctx.expect_true(sample == 1 || sample == 2);

    ctx.expect_true(rset.remove(1));
    ctx.expect_false(rset.insert(2));
    ctx.expect_eq(rset.getRandom(), 2);
  });

  ctx.test("remove_single_element_and_reinsert", [&] {
    RandomizedSet rset(123U);
    ctx.expect_true(rset.insert(3));
    ctx.expect_true(rset.remove(3));
    ctx.expect_false(rset.remove(3));
    ctx.expect_true(rset.insert(7));
    ctx.expect_false(rset.insert(7));
    ctx.expect_true(rset.insert(8));

    const int sample = rset.getRandom();
    ctx.expect_true(sample == 7 || sample == 8);
  });

  ctx.test("remove_middle_and_back_preserves_remaining_elements", [&] {
    RandomizedSet rset(999U);
    for (int v : {10, 20, 30, 40, 50}) {
      ctx.expect_true(rset.insert(v));
    }
    // Remove first, middle, and back elements to exercise all swap-delete paths.
    ctx.expect_true(rset.remove(10));
    ctx.expect_true(rset.remove(30));
    ctx.expect_true(rset.remove(50));
    ctx.expect_false(rset.remove(30));

    // Remaining elements must be strictly {20, 40}, both sampled frequently.
    std::unordered_map<int, int> counts;
    for (int trial = 0; trial < 200; ++trial) {
      const int val = rset.getRandom();
      ctx.expect_true(val == 20 || val == 40);
      ++counts[val];
    }
    ctx.expect_true(counts[20] > 50);
    ctx.expect_true(counts[40] > 50);
  });

  ctx.test("negative_and_zero_values", [&] {
    RandomizedSet rset(7U);
    ctx.expect_true(rset.insert(0));
    ctx.expect_true(rset.insert(-1));
    ctx.expect_true(rset.remove(0));
    ctx.expect_eq(rset.getRandom(), -1);
  });
}

}  // namespace
