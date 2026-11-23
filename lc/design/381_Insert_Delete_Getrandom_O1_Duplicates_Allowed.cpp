// LeetCode 381: Insert Delete Getrandom O1 Duplicates Allowed
// https://leetcode.com/problems/insert-delete-getrandom-o1-duplicates-allowed/
//
// Problem:
//   Design a multiset data structure (`RandomizedCollection`) that supports
//   three operations in average O(1) time while allowing duplicate values:
//     - `insert(val)`: inserts an occurrence of `val`; returns true iff `val`
//       was not previously present in the collection.
//     - `remove(val)`: removes one occurrence of `val` if present; returns
//       true iff an element was removed.
//     - `getRandom()`: returns a random element from the current collection
//       with probability linearly proportional to its multiplicity.
//
// Algorithm:
//   Store all elements densely in a dynamic array `vals_` (so uniform random
//   indexing naturally weights each value by its frequency), and maintain a
//   hash map `indices_` from each value to the hash set of its current array
//   indices:
//     - `insert(val)`: append `val` to `vals_`, record `vals_.size() - 1` in
//       `indices_[val]`, and return whether `indices_[val]` was empty before.
//     - `remove(val)`: look up `val` in `indices_`; if missing, return false.
//       Pop an arbitrary index `remove_idx` from `indices_[val]` FIRST. If
//       `remove_idx` is not the last element of `vals_`, overwrite
//       `vals_[remove_idx]` with `last_val = vals_.back()` and update
//       `indices_[last_val]` by replacing `last_idx` with `remove_idx` (erasing
//       `remove_idx` before updating `last_val` makes the swap-with-back
//       trick work seamlessly even when `last_val == val`). Pop the back of
//       `vals_` and erase `val`'s map entry if its index set becomes empty.
//     - `getRandom()`: draw a uniform index in `[0, vals_.size() - 1]`.
//   Time Complexity: O(1) average for `insert()`, `remove()`, and
//     `getRandom()`.
//   Space Complexity: O(n), where n is the total number of stored elements
//     including duplicates.
//
// Data Structures:
//   - `std::vector<int>` (`vals_`): contiguous storage enabling O(1) uniform
//     random access and O(1) swap-and-pop deletion.
//   - `std::unordered_map<int, std::unordered_set<std::size_t>>` (`indices_`):
//     inverted index mapping each distinct value to all its positions in
//     `vals_` for O(1) average deletion and relocation.
//   - `std::mt19937`: Mersenne Twister PRNG for unbiased index selection.
//
// Real-World Applications:
//   - Weighted load balancers where backends with higher capacity appear with
//     higher multiplicity and dynamically join/leave in O(1).
//   - Monte Carlo / importance-sampling pools with dynamic event insertion and
//     cancellation.
//   - Game loot tables and ad-auction candidate pools requiring fast updates
//     and frequency-proportional random draws.
//
// Related LeetCode Problems:
//   - #380 Insert Delete GetRandom O [done] (1) (unique-element version)
//   - #528 Random Pick with Weight [done] (static prefix-sum weighted sampling)
//   - #398 Random Pick Index [done] (uniform index selection among duplicates)
//   - #710 Random Pick with Blacklist [new] (O(1) remapped uniform sampling)

#include "framework/lc_framework.h"

#include <cstddef>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

// Average-O(1) multiset supporting insert, remove, and multiplicity-weighted
// random sampling.
class RandomizedCollection {
 public:
  // Inserts one occurrence of `val`. Returns true iff `val` had zero
  // occurrences prior to insertion.
  // Time: O(1) average, Space: O(1) amortized.
  bool insert(int val) {
    auto& positions = indices_[val];
    const bool is_first = positions.empty();
    positions.insert(vals_.size());
    vals_.push_back(val);
    return is_first;
  }

  // Removes one occurrence of `val` if present. Returns true iff removed.
  // Time: O(1) average, Space: O(1).
  bool remove(int val) {
    auto it = indices_.find(val);
    if (it == indices_.end() || it->second.empty()) {
      return false;
    }

    // Grab and detach one position of `val` before relocating `last_val` so
    // the `last_val == val` case stays consistent without special casing.
    const std::size_t remove_idx = *it->second.begin();
    it->second.erase(it->second.begin());

    const std::size_t last_idx = vals_.size() - 1;
    const int last_val = vals_.back();

    if (remove_idx != last_idx) {
      vals_[remove_idx] = last_val;
      // `last_val` is guaranteed present in `indices_`, so `find` avoids any
      // insertion/rehash that could invalidate `it`.
      auto last_it = indices_.find(last_val);
      last_it->second.erase(last_idx);
      last_it->second.insert(remove_idx);
    }

    vals_.pop_back();
    if (it->second.empty()) {
      indices_.erase(it);
    }
    return true;
  }

  // Returns a random element weighted by its multiplicity in the collection.
  // Precondition: collection is non-empty.
  // Time: O(1), Space: O(1).
  int getRandom() {
    std::uniform_int_distribution<std::size_t> dist(0, vals_.size() - 1);
    return vals_[dist(rng_)];
  }

 private:
  std::vector<int> vals_;
  std::unordered_map<int, std::unordered_set<std::size_t>> indices_;
  std::mt19937 rng_{std::random_device{}()};
};

LC_PROBLEM(381, "Insert Delete Getrandom O1 Duplicates Allowed", "design",
           ctx) {
  ctx.test("leetcode example with weighted distribution check", [&] {
    RandomizedCollection col;
    ctx.expect_true(col.insert(1));
    ctx.expect_false(col.insert(1));
    ctx.expect_true(col.insert(2));

    // Collection is [1, 1, 2]: expect ~2/3 ones and ~1/3 twos.
    int count_one = 0;
    constexpr int kTrials = 6000;
    for (int i = 0; i < kTrials; ++i) {
      const int picked = col.getRandom();
      ctx.expect_true(picked == 1 || picked == 2);
      if (picked == 1) {
        ++count_one;
      }
    }
    ctx.expect_true(count_one > 3500 && count_one < 4500);

    // Remove one 1 -> collection is [1, 2]: expect ~1/2 ones and ~1/2 twos.
    ctx.expect_true(col.remove(1));
    count_one = 0;
    for (int i = 0; i < kTrials; ++i) {
      const int picked = col.getRandom();
      ctx.expect_true(picked == 1 || picked == 2);
      if (picked == 1) {
        ++count_one;
      }
    }
    ctx.expect_true(count_one > 2500 && count_one < 3500);
  });

  ctx.test("python main duplicate-heavy remove and re-insert", [&] {
    RandomizedCollection col;
    ctx.expect_true(col.insert(42));
    ctx.expect_false(col.insert(42));
    ctx.expect_false(col.insert(42));
    ctx.expect_true(col.remove(42));
    ctx.expect_true(col.remove(42));
    ctx.expect_eq(col.getRandom(), 42);
    ctx.expect_true(col.remove(42));
    ctx.expect_false(col.remove(42));

    // Re-inserting after complete removal must return true again.
    ctx.expect_true(col.insert(42));
    ctx.expect_eq(col.getRandom(), 42);
  });

  ctx.test("swap-with-same-value and negative elements", [&] {
    RandomizedCollection col;
    ctx.expect_true(col.insert(-5));
    ctx.expect_true(col.insert(0));
    ctx.expect_false(col.insert(-5));
    // Removing -5 when -5 is also at the tail exercises the self-swap path.
    ctx.expect_true(col.remove(-5));
    ctx.expect_true(col.remove(-5));
    ctx.expect_false(col.remove(-5));
    ctx.expect_eq(col.getRandom(), 0);
    ctx.expect_true(col.remove(0));
    ctx.expect_false(col.remove(0));
  });
}

}  // namespace
