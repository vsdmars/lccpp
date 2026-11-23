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
// Examples:
//   Example 1:
//     Input:  operations = ["RandomizedSet", "insert", "remove", "insert",
//             "getRandom", "remove", "insert", "getRandom"], args = [[], [1], [2],
//             [2], [], [1], [2], []]
//     Output: [null, true, false, true, 2, true, false, 2]
//     Explanation:
//       - insert(1) appends 1 to values_ ([1]), maps val_to_index_[1] = 0, and
//         returns true, while remove(2) finds 2 absent and returns false.
//       - insert(2) appends 2 to values_ ([1, 2]), maps val_to_index_[2] = 1, and
//         returns true, so getRandom() uniformly samples index 0 or 1 from [1, 2]
//         (returning 2).
//       - remove(1) looks up remove_idx = 0 and last_idx = 1, copies last_val = 2
//         into values_[0], updates val_to_index_[2] = 0, pops back so values_ =
//         [2], and erases 1.
//       - insert(2) finds 2 in val_to_index_ and returns false, and getRandom()
//         samples index 0 from values_ = [2] to return 2.
//
//   Example 2:
//     Input:  operations = ["RandomizedSet", "insert", "remove", "remove",
//             "insert", "insert", "insert", "getRandom"], args = [[], [3], [3],
//             [3], [7], [7], [8], []]
//     Output: [null, true, true, false, true, false, true, 7]
//     Explanation:
//       - insert(3) stores values_ = [3] with val_to_index_[3] = 0 and returns
//         true.
//       - First remove(3) has remove_idx = 0 equal to last_idx = 0, so it skips
//         swapping, pops values_ to [], erases 3 from val_to_index_, and returns
//         true; the second remove(3) returns false.
//       - insert(7) returns true (values_ = [7]), duplicate insert(7) returns
//         false, and insert(8) returns true (values_ = [7, 8]).
//       - getRandom() uniformly draws index 0 or 1 from values_ = [7, 8], returning
//         7 (or 8).
//
//   Example 3:
//     Input:  operations = ["RandomizedSet", "insert", "insert", "remove",
//             "getRandom"], args = [[], [0], [-1], [0], []]
//     Output: [null, true, true, true, -1]
//     Explanation:
//       - insert(0) and insert(-1) populate values_ = [0, -1] with val_to_index_[0]
//         = 0 and val_to_index_[-1] = 1, both returning true.
//       - remove(0) finds remove_idx = 0 != last_idx = 1, overwrites values_[0]
//         with last_val = -1, updates val_to_index_[-1] = 0, pops back to values_ =
//         [-1], and erases 0.
//       - getRandom() draws index 0 from values_ = [-1] and returns -1.
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
