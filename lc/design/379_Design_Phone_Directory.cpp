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
// Examples:
//   Example 1:
//     Input:  ops = ["PhoneDirectory", "get", "get", "check", "get", "check",
//             "get", "release", "check", "get", "check"], args = [[3], [], [], [2],
//             [], [2], [], [2], [2], [], [2]]
//     Output: [null, 0, 1, true, 2, false, -1, null, true, 2, false]
//     Explanation:
//       - PhoneDirectory(3) sets max_numbers_ = 3, next_fresh_ = 0, available_ =
//         [1, 1, 1], and recycled_ = [].
//       - The first two get() calls find recycled_ empty, dispense next_fresh_ = 0
//         then 1, advance next_fresh_ to 2, and set available_[0] = available_[1] =
//         0.
//       - check(2) reads available_[2] == 1 and returns true; the third get()
//         assigns next_fresh_ = 2, sets available_[2] = 0, and increments
//         next_fresh_ to 3, so check(2) returns false and the fourth get() returns
//         -1.
//       - release(2) sees available_[2] == 0, sets available_[2] = 1, and pushes 2
//         onto recycled_, so check(2) returns true.
//       - The final get() pops 2 from recycled_, sets available_[2] = 0, and
//         returns 2, leaving check(2) equal to false.
//
//   Example 2:
//     Input:  ops = ["PhoneDirectory", "get", "get", "get", "release", "release",
//             "release", "release", "release", "get", "get", "check"], args = [[2],
//             [], [], [], [0], [0], [0], [-1], [2], [], [], [1]]
//     Output: [null, 0, 1, -1, null, null, null, null, null, 0, -1, false]
//     Explanation:
//       - PhoneDirectory(2) initializes available_ = [1, 1] and next_fresh_ = 0;
//         the first two get() calls return 0 and 1, setting available_ = [0, 0] and
//         next_fresh_ = 2, so the third get() returns -1.
//       - The first release(0) flips available_[0] from 0 to 1 and pushes 0 onto
//         recycled_, while the next two release(0) calls see available_[0] == 1 and
//         ignore the duplicate release.
//       - release(-1) and release(2) fail the [0, 2) bounds check and are no-ops.
//       - get() pops the single 0 from recycled_ and marks available_[0] = 0, after
//         which the next get() finds recycled_ empty and next_fresh_ == 2 so it
//         returns -1, and check(1) returns false.
//
//   Example 3:
//     Input:  ops = ["PhoneDirectory", "get", "check"], args = [[0], [], [0]]
//     Output: [null, -1, false]
//     Explanation:
//       - PhoneDirectory(0) sets max_numbers_ = 0, next_fresh_ = 0, available_ =
//         [], and recycled_ = [].
//       - get() sees recycled_ is empty and next_fresh_ < max_numbers_ (0 < 0) is
//         false, so it returns -1.
//       - check(0) fails the number >= max_numbers_ bounds check (0 >= 0) and
//         returns false.
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
