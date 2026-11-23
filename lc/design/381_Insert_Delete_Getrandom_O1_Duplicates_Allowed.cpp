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
// Examples:
//   Example 1:
//     Input:  ops = ["RandomizedCollection", "insert", "insert", "insert",
//             "getRandom", "remove", "getRandom"], args = [[], [1], [1], [2], [],
//             [1], []]
//     Output: [null, true, false, true, 1, true, 2]
//     Explanation:
//       - insert(1) sees indices_[1] is empty, records index 0, appends 1 to vals_
//         = [1], and returns true; the second insert(1) inserts index 1 into
//         indices_[1], appends 1 to vals_ = [1, 1], and returns false.
//       - insert(2) finds indices_[2] empty, inserts index 2, appends 2 to vals_ =
//         [1, 1, 2], and returns true.
//       - getRandom() samples uniformly from vals_ = [1, 1, 2] over indices [0, 2],
//         returning 1 with probability 2/3 and 2 with probability 1/3 (e.g., 1).
//       - remove(1) detaches one index of 1 from indices_[1], swaps the tail value
//         2 into that slot if it was not already the last index, updates
//         indices_[2], and pops the back to leave one 1 and one 2, returning true.
//       - The final getRandom() draws uniformly from the two remaining entries [1,
//         2], returning 1 or 2 with equal 1/2 probability (e.g., 2).
//
//   Example 2:
//     Input:  ops = ["RandomizedCollection", "insert", "insert", "insert",
//             "remove", "remove", "getRandom", "remove", "remove", "insert",
//             "getRandom"], args = [[], [42], [42], [42], [42], [42], [], [42],
//             [42], [42], []]
//     Output: [null, true, false, false, true, true, 42, true, false, true, 42]
//     Explanation:
//       - The three insert(42) calls grow vals_ to [42, 42, 42] with indices_[42] =
//         {0, 1, 2}, returning true only on the first call when indices_[42] was
//         empty.
//       - Two remove(42) calls each erase one index from indices_[42], relocate the
//         back element 42 if needed, and pop the back of vals_, leaving a single 42
//         so getRandom() returns 42.
//       - The third remove(42) removes index 0, leaves indices_[42] empty so key 42
//         is erased from indices_, and returns true; the fourth remove(42) misses
//         in indices_ and returns false.
//       - Re-calling insert(42) finds indices_[42] absent again, records index 0 in
//         vals_ = [42], and returns true, after which getRandom() returns 42.
//
//   Example 3:
//     Input:  ops = ["RandomizedCollection", "insert", "insert", "insert",
//             "remove", "remove", "remove", "getRandom", "remove", "remove"], args
//             = [[], [-5], [0], [-5], [-5], [-5], [-5], [], [0], [0]]
//     Output: [null, true, true, false, true, true, false, 0, true, false]
//     Explanation:
//       - The three inserts produce vals_ = [-5, 0, -5] with indices_[-5] = {0, 2}
//         and indices_[0] = {1}.
//       - On remove(-5), remove_idx is erased from indices_[-5] before updating
//         last_val: whether index 0 or 2 is picked, vals_ safely shrinks to one -5
//         and one 0 without corrupting indices_[-5].
//       - The second remove(-5) removes the remaining -5 and erases -5 from
//         indices_, leaving vals_ = [0] and indices_[0] = {0}, so the third
//         remove(-5) returns false and getRandom() returns 0.
//       - remove(0) removes index 0, erases 0 from indices_, and returns true; the
//         second remove(0) returns false.
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
