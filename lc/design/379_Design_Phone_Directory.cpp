// LeetCode 379: Design Phone Directory
// https://leetcode.com/problems/design-phone-directory/
//
// Problem:
//   Design a phone directory managing `maxNumbers` unique slots numbered
//   `0` through `maxNumbers - 1`, supporting three operations:
//     - `get()`: allocate and return any currently unassigned number, or -1
//       if every number is in use.
//     - `check(number)`: return true iff `number` is currently available.
//     - `release(number)`: recycle `number` back into the available pool (safe
//       no-op if already available or out of range).
//
// Algorithm:
//   Combine a monotonic high-water mark (`next_fresh_`) with a free-list stack
//   (`recycled_`) and a direct-indexed availability bitmap (`available_`):
//     - Initially `next_fresh_ = 0`, `recycled_` is empty, and every entry in
//       `available_` is marked available (1).
//     - `get()` first pops from `recycled_` if non-empty; otherwise it assigns
//       `next_fresh_++` if `< max_numbers_`; otherwise returns -1. In both
//       success cases it marks `available_[num] = 0`.
//     - `check(num)` checks bounds and reads `available_[num]` directly.
//     - `release(num)` checks bounds and, ONLY if `available_[num] == 0`,
//       flips it back to 1 and pushes `num` onto `recycled_` (preventing
//       duplicate entries in the free list on repeated releases).
//   Time Complexity: O(1) for `get()`, `check()`, and `release()`;
//     O(maxNumbers) for initialization.
//   Space Complexity: O(maxNumbers).
//
// Data Structures:
//   - `std::vector<std::uint8_t>` availability bitmap: O(1) membership check
//     and duplicate-release guard (avoids `std::vector<bool>` bit-proxy
//     overhead).
//   - `std::vector<int>` recycled free-list stack: O(1) push/pop for released
//     IDs without pre-populating all `maxNumbers` slots upfront.
//   - Monotonic `int` cursor `next_fresh_`: lazily dispenses never-used IDs.
//
// Real-World Applications:
//   - OS file descriptor tables, PID allocators, and ephemeral TCP port pools.
//   - Slab / pool memory allocators using free lists for fixed-size slots.
//   - Seat reservation, license-key checkout, and DHCP IPv4 lease managers.
//
// Related LeetCode Problems:
//   - #1845 Seat Reservation Manager [new] (min-heap + high-water mark)
//   - #2336 Smallest Number in Infinite Set [new] (fresh cursor + released set)
//   - #380 Insert Delete GetRandom O [done] (1) (O(1) slot pool management)
//   - #146 LRU Cache [done] (O(1) handle allocation and eviction)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <unordered_set>
#include <vector>

namespace {

// O(1) phone number pool using a lazy monotonic cursor, free-list stack, and
// direct-indexed availability bitmap.
class PhoneDirectory {
 public:
  // Creates a directory managing numbers in `[0, max_numbers - 1]`.
  // Time: O(max_numbers), Space: O(max_numbers).
  explicit PhoneDirectory(int max_numbers)
      : max_numbers_(max_numbers > 0 ? max_numbers : 0),
        available_(static_cast<std::size_t>(max_numbers_), 1) {}

  // Allocates and returns an available number, or -1 if the pool is exhausted.
  // Time: O(1), Space: O(1).
  int get() {
    int assigned = -1;
    if (!recycled_.empty()) {
      assigned = recycled_.back();
      recycled_.pop_back();
    } else if (next_fresh_ < max_numbers_) {
      assigned = next_fresh_;
      ++next_fresh_;
    } else {
      return -1;
    }

    available_[static_cast<std::size_t>(assigned)] = 0;
    return assigned;
  }

  // Returns true iff `number` is within bounds and currently unassigned.
  // Time: O(1), Space: O(1).
  [[nodiscard]] bool check(int number) const noexcept {
    if (number < 0 || number >= max_numbers_) {
      return false;
    }
    return available_[static_cast<std::size_t>(number)] != 0;
  }

  // Recycles `number` back into the pool if it is valid and currently in use.
  // Repeated releases of an already-available number are safely ignored.
  // Time: O(1), Space: O(1).
  void release(int number) {
    if (number < 0 || number >= max_numbers_) {
      return;
    }
    const auto idx = static_cast<std::size_t>(number);
    if (available_[idx] == 0) {
      available_[idx] = 1;
      recycled_.push_back(number);
    }
  }

 private:
  int max_numbers_ = 0;
  int next_fresh_ = 0;
  std::vector<std::uint8_t> available_;
  std::vector<int> recycled_;
};

LC_PROBLEM(379, "Design Phone Directory", "design", ctx) {
  ctx.test("leetcode example maxNumbers=3", [&] {
    PhoneDirectory dir(3);
    ctx.expect_eq(dir.get(), 0);
    ctx.expect_eq(dir.get(), 1);
    ctx.expect_true(dir.check(2));
    ctx.expect_eq(dir.get(), 2);
    ctx.expect_false(dir.check(2));
    ctx.expect_eq(dir.get(), -1);
    dir.release(2);
    ctx.expect_true(dir.check(2));
    ctx.expect_eq(dir.get(), 2);
    ctx.expect_false(dir.check(2));
  });

  ctx.test("python main sequence maxNumbers=10", [&] {
    PhoneDirectory dir(10);
    const int first = dir.get();
    ctx.expect_true(first >= 0 && first < 10);
    ctx.expect_false(dir.check(first));

    const int second = dir.get();
    ctx.expect_ne(second, first);
    ctx.expect_false(dir.check(second));

    dir.release(second);
    ctx.expect_true(dir.check(second));
  });

  ctx.test("idempotent release prevents duplicate allocations", [&] {
    PhoneDirectory dir(2);
    const int a = dir.get();
    const int b = dir.get();
    ctx.expect_eq(dir.get(), -1);

    // Releasing the same slot multiple times must only return it once.
    dir.release(a);
    dir.release(a);
    dir.release(a);
    // Releasing out-of-range numbers must be a no-op.
    dir.release(-1);
    dir.release(2);

    ctx.expect_eq(dir.get(), a);
    ctx.expect_eq(dir.get(), -1);
    ctx.expect_false(dir.check(b));
  });

  ctx.test("full allocation uniqueness and boundary checks", [&] {
    PhoneDirectory dir(5);
    std::unordered_set<int> seen;
    for (int i = 0; i < 5; ++i) {
      const int num = dir.get();
      ctx.expect_true(num >= 0 && num < 5);
      ctx.expect_false(seen.contains(num));
      seen.insert(num);
    }
    ctx.expect_eq(dir.get(), -1);
    ctx.expect_false(dir.check(-1));
    ctx.expect_false(dir.check(5));

    PhoneDirectory empty_dir(0);
    ctx.expect_eq(empty_dir.get(), -1);
    ctx.expect_false(empty_dir.check(0));
  });
}

}  // namespace
